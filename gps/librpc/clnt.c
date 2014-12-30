/* Copyright (c) 2009-2011, Code Aurora Forum. */

#include <rpc/rpc.h>
#include <arpa/inet.h>
#include <rpc/rpc_router_ioctl.h>
#include <debug.h>
#include <pthread.h>
#include <sys/select.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <hardware_legacy/power.h>
#include <poll.h>

#define ANDROID_WAKE_LOCK_NAME "rpc-interface"

void
grabPartialWakeLock() {
    acquire_wake_lock(PARTIAL_WAKE_LOCK, ANDROID_WAKE_LOCK_NAME);
}

void
releaseWakeLock() {
    release_wake_lock(ANDROID_WAKE_LOCK_NAME);
}

struct CLIENT {
    xdr_s_type *xdr;
    struct CLIENT *next;
    /* common attribute struct for setting up recursive mutexes */
    pthread_mutexattr_t lock_attr;

    /* We insist that there is only one outstanding call for a client at any
       time, and we use this mutex to enforce the rule.  When we start
       supporting multiple outstanding RPCs on a client, we will have to
       maintain a queue of them, and match incoming replies (by the XID of the
       incoming packet).  For now, we just block until we get that reply.
    */
    pthread_mutex_t lock;

    pthread_mutex_t wait_reply_lock;
    pthread_cond_t wait_reply;

    pthread_mutex_t input_xdr_lock;
    pthread_cond_t input_xdr_wait;
    volatile int input_xdr_busy;

    pthread_mutex_t wait_cb_lock;
    pthread_cond_t wait_cb;
    pthread_t cb_thread;
    volatile int got_cb;
    volatile int cb_stop;

    volatile int in_reset;
    clnt_reset_notif_cb reset_cb;
};

extern void* svc_find(void *xprt, rpcprog_t prog, rpcvers_t vers);
extern void svc_dispatch(void *svc, void *xprt);
extern int  r_open(const char *name);
extern void r_close(int handle);
extern xdr_s_type *xdr_init_common(const char *name, int is_client);
extern xdr_s_type *xdr_clone(xdr_s_type *);
extern void xdr_destroy_common(xdr_s_type *xdr);
extern bool_t xdr_recv_reply_header (xdr_s_type *xdr, rpc_reply_header *reply);
extern void *the_xprt;
extern int svc_is_in_reset(void* xprt);
extern void svc_set_in_reset(void* xprt, int val);
extern void svc_reset_cb(void* xprt, enum rpc_reset_event event);

static pthread_mutex_t rx_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t rx_thread;
static volatile unsigned int num_clients;
static volatile struct CLIENT *clients;
static int router_fd;

/* pipe used to unblock receive thread using self-pipe method */
static int wakeup_pipe[2];

/* There's one of these for each RPC client which has received an RPC call. */
static void *cb_context(void *__u)
{
    CLIENT *client = (CLIENT *)__u;
    D("RPC-callback thread for %08x:%08x starting.\n",
      (client->xdr->x_prog | 0x01000000),
      client->xdr->x_vers);
    pthread_mutex_lock(&client->wait_cb_lock);
    while (client->cb_stop == 0) {
        if (!client->got_cb)
            pthread_cond_wait(&client->wait_cb,
                              &client->wait_cb_lock);

        if (client->in_reset != svc_is_in_reset(the_xprt)) {
            svc_set_in_reset(the_xprt, client->in_reset);

            if (client->in_reset) {
                LIBRPC_DEBUG("xprt=%p - restart begin\n", the_xprt);
                svc_reset_cb(the_xprt, RPC_SUBSYSTEM_RESTART_BEGIN);
            } else {
                LIBRPC_DEBUG("xprt=%p - restart end\n", the_xprt);
                svc_reset_cb(the_xprt, RPC_SUBSYSTEM_RESTART_END);
            }
            continue;
        }

        /* We tell the thread it's time to exit by setting cb_stop to nonzero
           and signalling the conditional variable.  When there's no data, we
           skip to the top of the loop and exit. 
        */
        if (!client->got_cb) {
            LIBRPC_DEBUG("RPC-callback thread for %08x:%08x: signalled but no data.\n",
              (client->xdr->x_prog | 0x01000000),
              client->xdr->x_vers);
            continue;
        }
        client->got_cb = 0;

        /* We dispatch the message to the server representing the callback
         * client.
         */
        if (the_xprt) {
            void *svc;
            rpcprog_t prog =
                ntohl(((uint32 *)(client->xdr->in_msg))[RPC_OFFSET+3]);
            rpcvers_t vers =
                ntohl(((uint32 *)(client->xdr->in_msg))[RPC_OFFSET+4]);
            
            svc = svc_find(the_xprt, prog, vers);
            if (svc) {
                XDR **svc_xdr = (XDR **)svc;
                LIBRPC_DEBUG("%08x:%08x dispatching RPC call (XID %d, xdr %p) for "
                  "callback client %08x:%08x.\n",
                  client->xdr->x_prog,
                  client->xdr->x_vers,
                  ntohl(((uint32 *)(client->xdr->in_msg))[RPC_OFFSET]),
                  client->xdr,
                  (uint32_t)prog, (int)vers);
                /* We transplant the xdr of the client into the entry 
                   representing the callback client in the list of servers.
                   Note that since we hold the wait_cb_lock for this client,
                   if another call for this callback client arrives before
                   we've finished processing this call, that will block until
                   we're done with this one.  If this happens, it would be
                   most likely a bug in the arm9 rpc router.
                */
                if (*svc_xdr) {
                    D("%08x:%08x expecting XDR == NULL"
                      "callback client %08x:%08x!\n",
                      client->xdr->x_prog,
                      client->xdr->x_vers,
                      (uint32_t)prog, (int)vers);
                    xdr_destroy_common(*svc_xdr);
                }
                

                /* Do these checks before the clone */
                if (client->xdr->in_len < 0) {
                    E("%08x:%08x xdr->in_len = %i error %s (%d)",
                        client->xdr->in_len,
                        client->xdr->x_prog, client->xdr->x_vers,
                        strerror(errno), errno);
                    continue;
                }
                if (client->xdr->out_next < 0) {
                    E("%08x:%08x xdr->out_next = %i error %s (%d)",
                        client->xdr->out_next,
                        client->xdr->x_prog, client->xdr->x_vers,
                    strerror(errno), errno);
                    continue;
                }

                D("%08x:%08x cloning XDR for "
                  "callback client %08x:%08x.\n",
                  client->xdr->x_prog,
                  client->xdr->x_vers,
                  (uint32_t)prog, (int)vers);
                *svc_xdr = xdr_clone(client->xdr);
                
                (*svc_xdr)->x_prog = prog;
                (*svc_xdr)->x_vers = vers;
                memcpy((*svc_xdr)->in_msg,
                       client->xdr->in_msg, client->xdr->in_len);
                memcpy((*svc_xdr)->out_msg,
                       client->xdr->out_msg, client->xdr->out_next);
                (*svc_xdr)->in_len = client->xdr->in_len;
                (*svc_xdr)->out_next = client->xdr->out_next;

                pthread_mutex_lock(&client->input_xdr_lock);
                D("%08x:%08x marking input buffer as free.\n",
                  client->xdr->x_prog, client->xdr->x_vers);
                client->input_xdr_busy = 0;
                pthread_cond_signal(&client->input_xdr_wait);
                pthread_mutex_unlock(&client->input_xdr_lock);

                svc_dispatch(svc, the_xprt);
                xdr_destroy_common(*svc_xdr);
                *svc_xdr = NULL;
            }
            else E("%08x:%08x call packet arrived, but there's no "
                   "RPC server registered for %08x:%08x.\n",
                   client->xdr->x_prog,
                   client->xdr->x_vers,                   
                   (uint32_t)prog, (int)vers);                           
        }
        else E("%08x:%08x call packet arrived, but there's "
               "no RPC transport!\n",
               client->xdr->x_prog,
               client->xdr->x_vers);

        releaseWakeLock();
    }
    pthread_mutex_unlock(&client->wait_cb_lock);


    E("RPC-callback thread for %08x:%08x terminating.\n",
      (client->xdr->x_prog | 0x01000000),
      client->xdr->x_vers);
    return NULL;
}

static void *rx_context(void *__u __attribute__((unused)))
{
    int n;
    int ret;
    struct pollfd *pbits = NULL;
    unsigned int num_clients_cached = 0;
    CLIENT *client;

    while(num_clients) {
        /* setup file poll structure */
        pthread_mutex_lock(&rx_mutex);
        if (pbits == NULL || num_clients_cached != num_clients) {
            if (pbits != NULL)
                free(pbits);

            num_clients_cached = num_clients;
            pbits = calloc(num_clients_cached + 1, sizeof(struct pollfd));

            /* add wakeup pipe */
            pbits[0].fd = wakeup_pipe[0];
            pbits[0].events = POLL_IN;
        }

        client = (CLIENT *)clients;
        for (n=1; client; client = client->next, n++) {
            pbits[n].fd = client->xdr->fd;
            if (client->in_reset)
                pbits[n].events = POLLOUT;
            else
                pbits[n].events = POLLIN | POLLRDHUP;
        }
        pthread_mutex_unlock(&rx_mutex);

        /* wait for event */
        n = poll(pbits, num_clients_cached + 1, -1);

        /* clear wakeup pipe */
        if (pbits[0].revents) {
            char ch;
            read(wakeup_pipe[0], &ch, 1);
            LIBRPC_DEBUG("wakeup[0]=%x\n", pbits[0].revents);
        }

        if (!num_clients)
            break;

        if (n < 0) {
            E("poll() error %s (%d)\n", strerror(errno), errno);
            continue;
        }

        pthread_mutex_lock(&rx_mutex);
        if (num_clients_cached != num_clients) {
            LIBRPC_DEBUG("Number of clients changed from %d to %d\n",
                num_clients_cached, num_clients);
            pthread_mutex_unlock(&rx_mutex);
            continue;
        }

        client = (CLIENT *)clients;
        for (n=1; client; client = client->next, n++) {
            D("poll events IN=%d, OUT=%d, RDHUP=%d, in_reset=%d\n",
                    pbits[n].revents & POLLIN ? 1 : 0,
                    pbits[n].revents & POLLOUT ? 1 : 0,
                    pbits[n].revents & POLLRDHUP ? 1 : 0,
                    client->in_reset
             );

            if (!client->in_reset) {
                if (pbits[n].revents & POLLRDHUP) {
                    LIBRPC_DEBUG("modem entered reset for client %p, cb=%p\n",
                            client, client->reset_cb);

                    /* unblock any pending calls */
                    pthread_mutex_lock(&client->wait_reply_lock);
                    client->in_reset = 1;
                    pthread_cond_signal(&client->wait_reply);
                    pthread_mutex_unlock(&client->wait_reply_lock);

                    /* wakeup any callback threads */
                    pthread_mutex_lock(&client->wait_cb_lock);
                    pthread_cond_signal(&client->wait_cb);
                    pthread_mutex_unlock(&client->wait_cb_lock);

                    if (client->reset_cb)
                        client->reset_cb(client, RPC_SUBSYSTEM_RESTART_BEGIN);

                    /* prevent processing of POLLIN and unblock xdr */
                    pbits[n].revents = 0;
                    client->input_xdr_busy = 0;
                }
            } else if (pbits[n].revents & POLLOUT) {
                LIBRPC_DEBUG("modem exited reset for client %p, cb=%p\n",
                        client, client->reset_cb);

                /* clear reset flags */
                pthread_mutex_lock(&client->wait_reply_lock);
                client->xdr->xops->xdr_control(client->xdr,
                        RPC_ROUTER_IOCTL_CLEAR_NETRESET, NULL);
                client->in_reset = 0;
                pthread_mutex_unlock(&client->wait_reply_lock);

                /* wakeup any callback threads */
                pthread_mutex_lock(&client->wait_cb_lock);
                pthread_cond_signal(&client->wait_cb);
                pthread_mutex_unlock(&client->wait_cb_lock);

                if (client->reset_cb)
                    client->reset_cb(client, RPC_SUBSYSTEM_RESTART_END);
            }

            if (!(pbits[n].revents & POLLIN))
                continue;

            /* We need to make sure that the XDR's in_buf is not in
               use before we read into it.  The in_buf may be in use
               in a race between processing an incoming call and
               receiving a reply to an outstanding call, or processing
               an incoming reply and receiving a call.
            */

            pthread_mutex_lock(&client->input_xdr_lock);
            while (client->input_xdr_busy) {
                LIBRPC_DEBUG("%08x:%08x waiting for XDR input buffer "
                  "to be consumed.\n",
                  client->xdr->x_prog, client->xdr->x_vers);
                pthread_cond_wait(
                    &client->input_xdr_wait,
                    &client->input_xdr_lock);
            }

            D("%08x:%08x reading data.\n",
               client->xdr->x_prog, client->xdr->x_vers);
            grabPartialWakeLock();

            ret = client->xdr->xops->read(client->xdr);
            if (ret == FALSE) {
                E("%08x:%08x xops->read() error %s (%d)\n",
                  client->xdr->x_prog, client->xdr->x_vers,
                strerror(errno), errno);
                pthread_mutex_unlock(&client->input_xdr_lock);
                releaseWakeLock();
                continue;
            }
            client->input_xdr_busy = 1;
            pthread_mutex_unlock(&client->input_xdr_lock);

            if (((uint32 *)(client->xdr->in_msg))[RPC_OFFSET+1] ==
                htonl(RPC_MSG_REPLY)) {
                /* Wake up the RPC client to receive its data. */
                LIBRPC_DEBUG("%08x:%08x received REPLY (XID %d), "
                  "grabbing mutex to wake up client.\n",
                  client->xdr->x_prog,
                  client->xdr->x_vers,
                  ntohl(((uint32 *)client->xdr->in_msg)[RPC_OFFSET]));
                pthread_mutex_lock(&client->wait_reply_lock);
                D("%08x:%08x got mutex, waking up client.\n",
                  client->xdr->x_prog,
                  client->xdr->x_vers);
                pthread_cond_signal(&client->wait_reply);
                pthread_mutex_unlock(&client->wait_reply_lock);
                releaseWakeLock();
            }
            else {
                pthread_mutex_lock(&client->wait_cb_lock);
                LIBRPC_DEBUG("%08x:%08x received CALL.\n",
                  client->xdr->x_prog,
                  client->xdr->x_vers);
                client->got_cb = 1;
                if (client->cb_stop < 0) {
                    D("%08x:%08x starting callback thread.\n",
                      client->xdr->x_prog,
                      client->xdr->x_vers);
                    client->cb_stop = 0;
                    pthread_create(&client->cb_thread,
                                   NULL,
                                   cb_context, client);
                }
                D("%08x:%08x waking up callback thread.\n",
                  client->xdr->x_prog,
                  client->xdr->x_vers);
                pthread_cond_signal(&client->wait_cb);
                pthread_mutex_unlock(&client->wait_cb_lock);
            }
        }
        pthread_mutex_unlock(&rx_mutex);
    }

    if (pbits != NULL)
        free(pbits);

    E("RPC-client RX thread exiting!\n");
    return NULL;
}

enum clnt_stat
clnt_call(
    CLIENT       * client,
    u_long         proc,
    xdrproc_t      xdr_args,
    caddr_t        args_ptr,
    xdrproc_t      xdr_results,
    caddr_t        rets_ptr,
    struct timeval timeout)
{
    opaque_auth cred;
    opaque_auth verf;
    rpc_reply_header reply_header;
    enum clnt_stat ret = RPC_SUCCESS;

    xdr_s_type *xdr = client->xdr;

    pthread_mutex_lock(&client->lock);
    if (client->in_reset) {
        ret = RPC_SUBSYSTEM_RESTART;
        goto out;
    }

    cred.oa_flavor = AUTH_NONE;
    cred.oa_length = 0;
    verf.oa_flavor = AUTH_NONE;
    verf.oa_length = 0;

    xdr->x_op = XDR_ENCODE;

    /* Send message header */

    if (!xdr_call_msg_start (xdr, xdr->x_prog, xdr->x_vers,
                             proc, &cred, &verf)) {
        XDR_MSG_ABORT (xdr);
        ret = RPC_CANTENCODEARGS; 
        E("%08x:%08x error in xdr_call_msg_start()\n",
          client->xdr->x_prog,
          client->xdr->x_vers);
        goto out;
    }

    /* Send arguments */

    if (!xdr_args (xdr, args_ptr)) {
        XDR_MSG_ABORT(xdr);
        ret = RPC_CANTENCODEARGS; 
        E("%08x:%08x error in xdr_args()\n",
          client->xdr->x_prog,
          client->xdr->x_vers);
        goto out;
    }

    /* Finish message - blocking */
    pthread_mutex_lock(&client->wait_reply_lock);
    if (client->in_reset) {
        ret = RPC_SUBSYSTEM_RESTART;
        goto out_unlock;
    }

    LIBRPC_DEBUG("%08x:%08x sending call (XID %d).\n",
      client->xdr->x_prog, client->xdr->x_vers, client->xdr->xid);
    if (!XDR_MSG_SEND(xdr)) {
        E("error %d in XDR_MSG_SEND\n", xdr->xdr_err);
        ret = RPC_CANTSEND;
        if (xdr->xdr_err == -ENETRESET) {
            client->in_reset = 1;
            ret = RPC_SUBSYSTEM_RESTART;
        }
        goto out_unlock;
    }

    D("%08x:%08x waiting for reply.\n",
      client->xdr->x_prog, client->xdr->x_vers);
    pthread_cond_wait(&client->wait_reply, &client->wait_reply_lock);
    if (client->in_reset) {
        ret = RPC_SUBSYSTEM_RESTART;
        goto out_unlock;
    }

    D("%08x:%08x received reply.\n", client->xdr->x_prog, client->xdr->x_vers);

    if (((uint32 *)xdr->out_msg)[RPC_OFFSET] != 
        ((uint32 *)xdr->in_msg)[RPC_OFFSET]) {
        E("%08x:%08x XID mismatch: got %d, expecting %d.\n",
          client->xdr->x_prog, client->xdr->x_vers,
          ntohl(((uint32 *)xdr->in_msg)[RPC_OFFSET]),
          ntohl(((uint32 *)xdr->out_msg)[RPC_OFFSET]));
        ret = RPC_CANTRECV;
        goto out_unlock;
    }

    D("%08x:%08x decoding reply header.\n",
      client->xdr->x_prog, client->xdr->x_vers);
    if (!xdr_recv_reply_header (client->xdr, &reply_header)) {
        E("%08x:%08x error reading reply header.\n",
          client->xdr->x_prog, client->xdr->x_vers);
        ret = RPC_CANTRECV;
        goto out_unlock;
    }

    /* Check that other side accepted and responded */
    if (reply_header.stat != RPC_MSG_ACCEPTED) {
        /* Offset to map returned error into clnt_stat */
        ret = reply_header.u.dr.stat + RPC_VERSMISMATCH;
        E("%08x:%08x call was not accepted.\n",
          (uint32_t)client->xdr->x_prog, client->xdr->x_vers);
        goto out_unlock;
    } else if (reply_header.u.ar.stat != RPC_ACCEPT_SUCCESS) {
        /* Offset to map returned error into clnt_stat */
        ret = reply_header.u.ar.stat + RPC_AUTHERROR;
        E("%08x:%08x call failed with an authentication error.\n",
          (uint32_t)client->xdr->x_prog, client->xdr->x_vers);
        goto out_unlock;
    }

    xdr->x_op = XDR_DECODE;
    /* Decode results */
    if (!xdr_results(xdr, rets_ptr) || ! XDR_MSG_DONE(xdr)) {
        ret = RPC_CANTDECODERES;
        E("%08x:%08x error decoding results.\n",
          client->xdr->x_prog, client->xdr->x_vers);
        goto out_unlock;
    }

    LIBRPC_DEBUG("%08x:%08x call success.\n",
      client->xdr->x_prog, client->xdr->x_vers);

  out_unlock:
    pthread_mutex_lock(&client->input_xdr_lock);
    D("%08x:%08x marking input buffer as free.\n",
      client->xdr->x_prog, client->xdr->x_vers);
    client->input_xdr_busy = 0;
    pthread_cond_signal(&client->input_xdr_wait);
    pthread_mutex_unlock(&client->input_xdr_lock);

    pthread_mutex_unlock(&client->wait_reply_lock);
  out:
    pthread_mutex_unlock(&client->lock);
    return ret;
} /* clnt_call */

bool_t xdr_recv_auth (xdr_s_type *xdr, opaque_auth *auth)
{
    switch(sizeof(auth->oa_flavor)) {
    case 1:
        if(!XDR_RECV_INT8(xdr, (int8_t *)&(auth->oa_flavor))) return FALSE;
        break;
    case 2:
        if(!XDR_RECV_INT16(xdr, (int16_t *)&(auth->oa_flavor))) return FALSE;
        break;
    case 4:
        if(!XDR_RECV_INT32(xdr, (int32_t *)&(auth->oa_flavor))) return FALSE;
        break;
    }
    if (!XDR_RECV_UINT (xdr, (unsigned *)&(auth->oa_length))) {
        return FALSE;
    }
    
    if (auth->oa_length != 0) {
        /* We throw away the auth stuff--it's always the default. */
        auth->oa_base = NULL;
        if (!XDR_RECV_BYTES (xdr, NULL, auth->oa_length))
            return FALSE;
        else
            return FALSE;
    }
    
    return TRUE;
} /* xdr_recv_auth */

static bool_t
xdr_recv_accepted_reply_header(xdr_s_type *xdr,
                               struct rpc_accepted_reply_header *accreply)
{
    if (!xdr_recv_auth(xdr, &accreply->verf)) {
        return FALSE;
    }

    if (!XDR_RECV_ENUM(xdr, &accreply->stat)) {
        return FALSE;
    }

    switch ((*accreply).stat) {
    case RPC_PROG_MISMATCH:
        if (!XDR_RECV_UINT32(xdr, &accreply->u.versions.low)) {
            return FALSE;
        }

        if (!XDR_RECV_UINT32(xdr, &accreply->u.versions.high)) {
            return FALSE;
        }
        break;

    case RPC_ACCEPT_SUCCESS:
    case RPC_PROG_UNAVAIL:
    case RPC_PROC_UNAVAIL:
    case RPC_GARBAGE_ARGS:
    case RPC_SYSTEM_ERR:
    case RPC_PROG_LOCKED:
        // case ignored
        break;

    default:
        return FALSE;
    }

    return TRUE;
} /* xdr_recv_accepted_reply_header */

static bool_t xdr_recv_denied_reply(xdr_s_type *xdr,
                                    struct rpc_denied_reply *rejreply)
{
    if (!XDR_RECV_ENUM (xdr, &rejreply->stat))
        return FALSE;

    switch ((*rejreply).stat) {
    case RPC_MISMATCH:
        if (!XDR_RECV_UINT32(xdr, &rejreply->u.versions.low))
            return FALSE;
        if (!XDR_RECV_UINT32(xdr, &rejreply->u.versions.high))
            return FALSE;
        break;
    case RPC_AUTH_ERROR:
        if (!XDR_RECV_ENUM (xdr, &rejreply->u.why))
            return FALSE;
        break;
    default:
        return FALSE;
    }

    return TRUE;
} /* xdr_recv_denied_reply */

bool_t xdr_recv_reply_header (xdr_s_type *xdr, rpc_reply_header *reply)
{
    if (!XDR_RECV_ENUM(xdr, &reply->stat)) {
        return FALSE;
    }

    switch ((*reply).stat) {
    case RPC_MSG_ACCEPTED:
        if (!xdr_recv_accepted_reply_header(xdr, &reply->u.ar)) {
            return FALSE;
    }
        break;
    case RPC_MSG_DENIED:
        if (!xdr_recv_denied_reply(xdr, &reply->u.dr)) {
            return FALSE;
    }
        break;
    default:
        return FALSE;
    }

    return TRUE;
} /* xdr_recv_reply_header */

CLIENT *clnt_create(
    char * host,
    uint32 prog,
    uint32 vers,
    char * proto)
{
    CLIENT *client = calloc(1, sizeof(CLIENT));
    if (client) {
        char name[20];

        /* for versions like 0x00010001, only compare against major version */
        if ((vers & 0xFFF00000) == 0)
            vers &= 0xFFFF0000;

        pthread_mutex_lock(&rx_mutex);

	if (!num_clients) {
	    /* Open the router device to load the modem */
	    router_fd = r_open("00000000:0");
	    if (router_fd < 0) {
		free(client);
		pthread_mutex_unlock(&rx_mutex);
		return NULL;
	    }
	}
        /* Implment backwards compatibility */
        vers = (vers & 0x80000000) ? vers : vers & 0xFFFF0000;

        snprintf(name, sizeof(name), "%08x:%08x", (uint32_t)prog, (int)vers);
        client->xdr = xdr_init_common(name, 1 /* client XDR */);
        if (!client->xdr) {
            E("failed to initialize client (permissions?)!\n");
	    if (!num_clients)
		r_close(router_fd);
            free(client);
            pthread_mutex_unlock(&rx_mutex);
            return NULL;
        }
        client->xdr->x_prog = prog;
        client->xdr->x_vers = vers;
        client->cb_stop = -1; /* callback thread has not been started */

        if (!num_clients) {
            if (pipe(wakeup_pipe) == -1) {
               E("failed to create pipe\n");
	       r_close(router_fd);
               free(client);
               pthread_mutex_unlock(&rx_mutex);
               return NULL;
            }
        }

        client->next = (CLIENT *)clients;
        clients = client;
        if (!num_clients++) {
            D("launching RX thread.\n");
            pthread_create(&rx_thread, NULL, rx_context, NULL);
        } else {
            /* client added, wake up rx_thread */
            if (write(wakeup_pipe[1], "a", 1) < 0)
	        E("error writing to pipe\n");
	}

        pthread_mutexattr_init(&client->lock_attr);
//      pthread_mutexattr_settype(&client->lock_attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&client->lock, &client->lock_attr);
        pthread_mutex_init(&client->wait_reply_lock, &client->lock_attr);
        pthread_cond_init(&client->wait_reply, NULL);
        pthread_mutex_init(&client->wait_cb_lock, &client->lock_attr);
        pthread_cond_init(&client->wait_cb, NULL);
        pthread_mutex_init(&client->input_xdr_lock, &client->lock_attr);
        pthread_cond_init(&client->input_xdr_wait, NULL);

        pthread_mutex_unlock(&rx_mutex);
    }

    return client;
}

int clnt_register_reset_notification_cb(CLIENT *client, clnt_reset_notif_cb cb) {
    int ret = 1;


    if (client) {
        pthread_mutex_lock(&client->lock);
        client->reset_cb = cb;
        ret = 0;
        pthread_mutex_unlock(&client->lock);
    }
    return ret;
}

clnt_reset_notif_cb clnt_unregister_reset_notification_cb(CLIENT *client) {
    clnt_reset_notif_cb cb = NULL;

    if (client) {
        pthread_mutex_lock(&client->lock);
        cb = client->reset_cb;
        client->reset_cb = NULL;
        pthread_mutex_unlock(&client->lock);
    }
    return cb;
}

void clnt_destroy(CLIENT *client) {
    if (client) {
        pthread_mutex_lock(&client->lock);
        D("%08x:%08x destroying client\n",
          client->xdr->x_prog,
          client->xdr->x_vers);


        if (!client->cb_stop) {
            /* The callback thread is running, we need to stop it */
            client->cb_stop = 1;
            D("%08x:%08x stopping callback thread\n",
              client->xdr->x_prog,
              client->xdr->x_vers);
            pthread_mutex_lock(&client->wait_cb_lock);
            pthread_cond_signal(&client->wait_cb);
            pthread_mutex_unlock(&client->wait_cb_lock);
            D("%08x:%08x joining callback thread\n",
              client->xdr->x_prog,
              client->xdr->x_vers);
            pthread_join(client->cb_thread, NULL);
        }

        pthread_mutex_lock(&rx_mutex); /* sync access to the client list */
        {
            CLIENT *trav = (CLIENT *)clients, *prev = NULL;
            for(; trav; trav = trav->next) {
                if (trav == client) {
                     D("%08x:%08x removing from client list\n",
                      client->xdr->x_prog,
                      client->xdr->x_vers);
                    if (prev)
                        prev->next = trav->next;
                    else
                        clients = trav->next;
                    num_clients--;
                    break;
                }
                prev = trav;
            }
        }
        if (!num_clients) {
            /* no clients, wake up rx_thread */
            if (write(wakeup_pipe[1], "d", 1) < 0)
	        E("error writing to pipe\n");

            D("stopping rx thread!\n");
            pthread_join(rx_thread, NULL);
            D("stopped rx thread\n");

            close(wakeup_pipe[0]);
            close(wakeup_pipe[1]);
	    r_close(router_fd);
        }
        pthread_mutex_unlock(&rx_mutex); /* sync access to the client list */
 
        pthread_mutex_destroy(&client->input_xdr_lock);
        pthread_cond_destroy(&client->input_xdr_wait);

        pthread_mutex_destroy(&client->wait_reply_lock);
        pthread_cond_destroy(&client->wait_reply);
        xdr_destroy_common(client->xdr);

        // FIXME: what happens when we lock the client while destroying it,
        // and another thread locks the mutex in clnt_call, and then we 
        // call pthread_mutex_destroy?  Does destroy automatically unlock and
        // then cause the lock in clnt_call() to return an error?  When we
        // unlock the mutex here there can be a context switch to the other
        // thread, which will cause it to obtain the mutex on the destroyed
        // client (and probably crash), and then we get to the destroy call
        // here... will that call fail?
        pthread_mutex_unlock(&client->lock);        
        pthread_mutex_destroy(&client->lock);
        pthread_mutexattr_destroy(&client->lock_attr);
        D("client destroy done\n");
        free(client);
    }
}
