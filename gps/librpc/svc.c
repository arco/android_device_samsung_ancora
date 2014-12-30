/* Parts of this file are derived from the original Sun (ONC) RPC
 * code, under the following copyright:
 */
/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */
/*
 * svc.c, Server-side remote procedure call interface.
 *
 * There are two sets of procedures here.  The xprt routines are
 * for handling transport handles.  The svc routines handle the
 * list of service routines.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

/* Copyright (c) 2010-2011, Code Aurora Forum. */

#include <rpc/rpc.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <rpc/rpc_router_ioctl.h>
#include <debug.h>
#include <pthread.h>
#include <stdlib.h>

extern XDR *xdr_init_common(const char *name, int is_client);
extern void xdr_destroy_common(XDR *xdr);
extern int r_control(int handle, const uint32 cmd, void *arg);
extern void grabPartialWakeLock();
extern void releaseWakeLock();

#include <stdio.h>
#include <errno.h>
#include <string.h>

typedef struct registered_server_struct {
    /* MUST BE AT OFFSET ZERO!  The client code assumes this when it overwrites
       the XDR for server entries which represent a callback client.  Those
       server entries do not have their own XDRs.
    */
    XDR *xdr;
    /* Because the xdr is NULL for callback clients (as opposed to true
       servers), we keep track of the program number and version number in this
       structure as well.
    */       
    rpcprog_t x_prog; /* program number */
    rpcvers_t x_vers; /* program version */

    int active;
    struct registered_server_struct *next;
    SVCXPRT *xprt;
    __dispatch_fn_t dispatch;
} registered_server;

struct SVCXPRT {
    fd_set fdset;
    int max_fd;
    pthread_attr_t thread_attr;
    pthread_t  svc_thread;
    pthread_mutexattr_t lock_attr;
    pthread_mutex_t lock;
    registered_server *servers;
    int num_cb_servers;
    volatile int num_servers;

    volatile int in_reset;
    svc_reset_notif_cb reset_cb;
};

static pthread_mutex_t xprt_lock = PTHREAD_MUTEX_INITIALIZER;
int xprt_refcount;
SVCXPRT *the_xprt; /* FIXME: have a list or something */

/*
  This routine is only of interest if a service implementor does not
  call svc_run(), but instead implements custom asynchronous event
  processing.  It is called when the select() system call has
  determined that an RPC request has arrived on some RPC socket(s);
  rdfds is the resultant read file descriptor bit mask.  The routine
  returns when all sockets associated with the value of rdfds have
  been serviced.
 */

void svc_dispatch(registered_server *svc, SVCXPRT *xprt);

int svc_is_in_reset(void* xprt) {
    return ((SVCXPRT *)xprt)->in_reset;
}

void svc_set_in_reset(void* xprt, int val) {
    ((SVCXPRT *)xprt)->in_reset = val;
}

void svc_reset_cb(void* xprt, enum rpc_reset_event event) {
    SVCXPRT *xprt_type = (SVCXPRT *)xprt;

    pthread_mutex_lock(&xprt_lock);

    D("Calling reset cb %p with event %d\n", xprt_type->reset_cb, event);
    if (xprt_type->reset_cb)
        xprt_type->reset_cb(xprt, event);

    pthread_mutex_unlock(&xprt_lock);
}

static void* svc_context(void *__u)
{
    SVCXPRT *xprt = (SVCXPRT *)__u;
    int n;
    struct timeval tv;
    volatile fd_set rfds;
    while(xprt->num_servers) {
        rfds = xprt->fdset;
        tv.tv_sec = 1; tv.tv_usec = 0;
        n = select(xprt->max_fd + 1, (fd_set *)&rfds, NULL, NULL, &tv);
        if (n < 0) {
            E("select() error %s (%d)\n", strerror(errno), errno);
            continue;
        }
        if (n) {
            grabPartialWakeLock();
            for (n = 0; n <= xprt->max_fd; n++) {
                if (FD_ISSET(n, &rfds)) {
                    /* the file descriptor points to the service instance; we
                       simply look that service by its file descriptor, and
                       call its service function. */
                    /* FIXME: need to take xprt->lock */
                    registered_server *trav = xprt->servers;
                    for (; trav; trav = trav->next)
		      if ((trav->xdr) && (trav->xdr->fd == n)) {
                            /* read the entire RPC */
                            if (trav->xdr->xops->read(trav->xdr) == 0) {
                                E("%08x:%08x ONCRPC read error: aborting!\n",
                                  trav->xdr->x_prog, trav->xdr->x_vers);
                                abort();
                            }
                            svc_dispatch(trav, xprt);
                            break;
                        }
                } /* if fd is set */
            } /* for each fd */
            releaseWakeLock();
        }
    }
    D("RPC-server thread exiting!\n");
    return NULL;
}

SVCXPRT *svcrtr_create (void)
{
    SVCXPRT *xprt;
    pthread_mutex_lock(&xprt_lock);
    if (the_xprt) {
        D("The RPC transport has already been created.\n");
        xprt = the_xprt;
    } else {
        xprt = calloc(1, sizeof(SVCXPRT));
        if (xprt) {
            FD_ZERO(&xprt->fdset);
            xprt->max_fd = 0;
            pthread_attr_init(&xprt->thread_attr);
            pthread_attr_setdetachstate(&xprt->thread_attr,
                                        PTHREAD_CREATE_DETACHED);
            pthread_mutexattr_init(&xprt->lock_attr);
//          pthread_mutexattr_settype(&xprt->lock_attr,
//                                    PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&xprt->lock, &xprt->lock_attr);
        }
    }    
    pthread_mutex_unlock(&xprt_lock);
    return xprt;
}

void svc_destroy(SVCXPRT *xprt)
{
    /* the last call to xprt_unregister() does the job */
}

/* NOTE: this function must always be called with the xprt->lock held! */
static registered_server* svc_find_nosync(SVCXPRT *xprt, 
                                          rpcprog_t prog, rpcvers_t vers,
                                          registered_server **prev)
{
    registered_server *trav;
    trav = xprt->servers;
    if (prev) *prev = NULL;
    for (; trav; trav = trav->next) {
        if (trav->x_prog == prog && trav->x_vers == vers)
            break;
        if (prev) *prev = trav;
    }
    return trav;
}

registered_server* svc_find(SVCXPRT *xprt, 
                            rpcprog_t prog, rpcvers_t vers)
{
    pthread_mutex_lock(&xprt->lock);
    registered_server *svc = svc_find_nosync(xprt, prog, vers, NULL);
    pthread_mutex_unlock(&xprt->lock);
    return svc;
}

bool_t svc_register (SVCXPRT *xprt, rpcprog_t prog, rpcvers_t vers, 
                     __dispatch_fn_t dispatch,
                     rpcprot_t protocol)
{
    struct rpcrouter_ioctl_server_args args;
    registered_server* svc;

    pthread_mutex_lock(&xprt->lock);

    D("registering for service %08x:%d\n", (uint32_t)prog, (int)vers);

    svc = svc_find_nosync(xprt, prog, vers, NULL);

    if (svc) {
        E("service is already registered!\n");
        pthread_mutex_unlock(&xprt->lock);
        return svc->dispatch == dispatch;
    }

    svc = malloc(sizeof(registered_server));

    /* If the program number of the RPC server ANDs with 0x01000000, then it is
       not a true RPC server, but a callback client for an existing RPC client.
       For example, if you have an RPC client with the program number
       0x30000000, then its callback client will have a program number
       0x31000000.  RPC calls on program number 0x31000000 will arrive on the
       RPC client 0x30000000.
    */

    if (prog & 0x01000000) {
        D("RPC server %08x:%d is a callback client, "
          "creating dummy service entry!\n", (uint32_t)prog, (int)vers);
        svc->xdr = NULL;
        svc->x_prog = prog;
        svc->x_vers = vers;        
    } else {
        V("RPC server %08x:%d is a real server.\n", (uint32_t)prog, (int)vers);
        svc->xdr = xdr_init_common("00000000:0", 0 /* not a client XDR */);
        if (svc->xdr == NULL) {
            E("failed to initialize service (permissions?)!\n");
            free(svc);
            pthread_mutex_unlock(&xprt->lock);
            return FALSE;
        }

        args.prog = prog;
        args.vers = vers;    
        V("RPC server %08x:%d: registering with kernel.\n",
          (uint32_t)prog, (int)vers);
        if (r_control(svc->xdr->fd, 
                      RPC_ROUTER_IOCTL_REGISTER_SERVER, 
                      &args) < 0) {
            E("ioctl(RPC_ROUTER_IOCTL_REGISTER_SERVER) failed: %s!\n",
              strerror(errno));
            xdr_destroy_common(svc->xdr);
            free(svc);
            pthread_mutex_unlock(&xprt->lock);
            return FALSE;
        }

        FD_SET(svc->xdr->fd, &xprt->fdset);
        if (svc->xdr->fd > xprt->max_fd) xprt->max_fd = svc->xdr->fd;
        svc->x_prog = svc->xdr->x_prog = prog;
        svc->x_vers = svc->xdr->x_vers = vers;        
    }

    svc->dispatch = dispatch;
    svc->next = xprt->servers;
    xprt->servers = svc;
    if (svc->xdr)
        xprt->num_servers++;
    else
        xprt->num_cb_servers++;

    V("RPC server %08x:%d: after registering,"
      "total %d servers, %d cb servers.\n",
      (uint32_t)prog, (int)vers, xprt->num_servers, xprt->num_cb_servers);
    svc->xprt = xprt;
    if (xprt->num_servers == 1) {
        D("creating RPC-server thread (detached)!\n");
        pthread_create(&xprt->svc_thread,
                       &xprt->thread_attr,
                       svc_context, xprt);
    }
    pthread_mutex_unlock(&xprt->lock);
    return TRUE;
}

int svc_register_reset_notification_cb(SVCXPRT *xprt, svc_reset_notif_cb cb) {
    int ret = 1;

    if (xprt) {
        pthread_mutex_lock(&xprt->lock);
        xprt->reset_cb = cb;
        ret = 0;
        pthread_mutex_unlock(&xprt->lock);
    }
    return ret;
}

svc_reset_notif_cb svc_unregister_reset_notification_cb(SVCXPRT *xprt) {
    svc_reset_notif_cb cb = NULL;

    if (xprt) {
        pthread_mutex_lock(&xprt->lock);
        cb = xprt->reset_cb;
        xprt->reset_cb = NULL;
        pthread_mutex_unlock(&xprt->lock);
    }
    return cb;
}

void svc_unregister (SVCXPRT *xprt, rpcprog_t prog, rpcvers_t vers) {
    registered_server *prev, *found;
    pthread_mutex_lock(&xprt->lock);
    found = svc_find_nosync(xprt, prog, vers, &prev);
    D("unregistering RPC server %08x:%d\n", (unsigned)prog, (unsigned)vers);
    if (found) {
        struct rpcrouter_ioctl_server_args args;
        if (prev) {
            V("RPC server %08x:%d is not the first in the list\n", 
              (unsigned)prog, (unsigned)vers);
            prev->next = found->next;
        } else {
            V("RPC server %08x:%d the first in the list\n", 
              (unsigned)prog, (unsigned)vers);
            xprt->servers = found->next;
        }

        /* Is is an RPC server or a callback client? */
        if (found->xdr) {
            if (!(prog & 0x01000000)) {
                V("RPC server %08x:%d is not a callback server.\n",
                  (unsigned)prog, (unsigned)vers);
                /* don't bother decreasing the xprt->max_fd to the previous
                 * minimum.
                 */
                args.prog = prog;
                args.vers = vers;    
                if (r_control(found->xdr->fd,
                              RPC_ROUTER_IOCTL_UNREGISTER_SERVER,
                              &args) < 0) {
                    E("ioctl(RPC_ROUTER_IOCTL_UNREGISTER_SERVER) "
                      "failed: %s!\n", 
                      strerror(errno));
                }                
                FD_CLR(found->xdr->fd, &xprt->fdset);
            }
            V("RPC server %08x:%d: destroying XDR\n",
                   (unsigned)prog, (unsigned)vers);
            xdr_destroy_common(found->xdr);
        }
        else V("RPC server %08x:%d does not have an associated XDR\n", 
               (unsigned)prog, (unsigned)vers);

        /* When this goes to zero, the RPC-server thread will exit.  We do not
         * need to wait for the thread to exit, because it is detached.
         */
        if (found->xdr)
            xprt->num_servers--;
        else
            xprt->num_cb_servers--;

        free(found);
        V("RPC server %08x:%d: after unregistering,"
	  "%d servers, %d cb servers left.\n",
          (unsigned)prog, (unsigned)vers,
	  xprt->num_servers, xprt->num_cb_servers);
    }
    pthread_mutex_unlock(&xprt->lock);
}

/* 
RPC_OFFSET + 
0  00000000 RPC xid                 network-byte order
1  00000000 RPC call 
2  00000002 rpc version
3  3000005d prog num
4  00000000 prog vers
5  00000001 proc num

6  00000000 cred
7  00000000 cred
8  00000000 verf
9  00000000 verf

a  0001fcc1 parms...
b  00354230 
c  00000000
 */

void svc_dispatch(registered_server *svc, SVCXPRT *xprt)
{
    struct svc_req req;

    /* Read enough of the packet to be able to find the program number, the
       program-version number, and the procedure call.  Notice that anything
       arriving on this channel must be an RPC call.  Also, the program and
       program-version numbers must match what's in the XDR of the service. */

    D("reading on fd %d for %08x:%d\n", 
      svc->xdr->fd, (int)svc->x_prog, (int)svc->x_vers);

    uint32 prog = ntohl(((uint32 *)(svc->xdr->in_msg))[RPC_OFFSET+3]);
    uint32 vers = ntohl(((uint32 *)(svc->xdr->in_msg))[RPC_OFFSET+4]);
    uint32 proc = ntohl(((uint32 *)(svc->xdr->in_msg))[RPC_OFFSET+5]);

    if (ntohl(((uint32 *)svc->xdr->in_msg)[RPC_OFFSET+1]) != RPC_MSG_CALL) {
        E("ERROR: expecting an RPC call on server channel!\n");
        return;
    }

    if (prog != svc->x_prog) {
        E("ERROR: prog num %08x does not match expected %08x!\n",
          (unsigned)prog, (unsigned)svc->x_prog);
        return;
    }

    if (vers != svc->xdr->x_vers) {
        E("ERROR: prog vers %08x does not match expected %08x!\n",
                vers, svc->xdr->x_vers);
        return;
    }

    req.rq_prog = prog;
    req.rq_vers = vers;
    req.rq_proc = proc;
    req.rq_xprt = xprt;

    D("START: SVC DISPATCH %08x:%08x --> %08x\n",
      (uint32_t)prog, (int)vers, proc);
    /* The RPC header (XID, call type, RPC version, program number, program
       version, proc number) is 6 long words; the default credentials are 4
       long words.  This the offset (RPC_OFFSET + 10)<<2 is where the first
       arguments start.
    */
    svc->xdr->in_next = (RPC_OFFSET + 6 + 4)*sizeof(uint32); 

    svc->active = getpid();
    svc->xdr->x_op = XDR_DECODE;
    svc->dispatch(&req, (SVCXPRT *)svc);
    svc->active = 0;
    D("DONE: SVC DISPATCH %08x:%08x --> %08x\n",
      (uint32_t)prog, (int)vers, proc);
}

void xprt_register(SVCXPRT *xprt)
{
    pthread_mutex_lock(&xprt_lock);
    if (!the_xprt || (xprt && (xprt == the_xprt))) {
        xprt_refcount++;
        the_xprt = xprt;
        D("registering RPC transport (refcount %d)\n", xprt_refcount);
    }
    else E("a different RPC transport has already been registered!\n");
    pthread_mutex_unlock(&xprt_lock);
}

void xprt_unregister (SVCXPRT *xprt)
{
    pthread_mutex_lock(&xprt_lock);
    if (xprt && xprt == the_xprt) {
        if (xprt_refcount == 1) {
            xprt_refcount = 0;
            D("Destroying RPC transport (servers %d, cb servers %d)\n",
              the_xprt->num_servers, the_xprt->num_cb_servers);

            pthread_attr_destroy(&xprt->thread_attr);
            pthread_mutexattr_destroy(&xprt->lock_attr);
            pthread_mutex_destroy(&xprt->lock);
            /* Make sure the thread has existed before we free the xprt
               structure.  The thread is created as detached, so we do not wait
               for it after we set the terminate flag in svc_unregister, but we
               do have to wait for it to exit when we call svc_destroy.
            */
            pthread_join(xprt->svc_thread, NULL);
            free(xprt);
            the_xprt = NULL;
        }
        else xprt_refcount--;
        D("unregistering RPC transport (refcount %d)\n", xprt_refcount);
    }
    else E("no RPC transport has been registered!\n");
    pthread_mutex_unlock(&xprt_lock);
}

/* The functions that follow all take a pointer to the SVCXPRT instead of the
   XDR of the server that they refer to.  The xprt pointer is actually a 
   pointer to a registered_server, which identified the RPC server in
   question.
*/

bool_t svc_getargs(SVCXPRT *xprt, xdrproc_t xdr_args, caddr_t args_ptr)
{
    registered_server *serv = (registered_server *)xprt;
    if (serv->active) {
        bool_t result = (bool_t) (*xdr_args)(serv->xdr, args_ptr);            
        XDR_MSG_DONE (serv->xdr);
        return result;
    }
    return FALSE;
} /* svc_getargs */

bool_t svc_freeargs (SVCXPRT * xprt, xdrproc_t xdr_args, caddr_t args_ptr)
{
    registered_server *serv = (registered_server *)xprt;
    if (serv->active) { 
        serv->xdr->x_op = XDR_FREE;
        return (*xdr_args)((XDR *)serv->xdr, args_ptr);
    }
    return FALSE;
}

/* Send a reply to an rpc request */
bool_t
svc_sendreply (SVCXPRT *xprt, xdrproc_t xdr_results,
               caddr_t xdr_location)
{
    registered_server *serv = (registered_server *)xprt;
    if (serv->active) { 
        opaque_auth verf; 
        verf.oa_flavor = AUTH_NONE;
        verf.oa_length = 0;
        
        serv->xdr->x_op = XDR_ENCODE;
        
        if (!xdr_reply_msg_start(serv->xdr, &verf) ||
            !xdr_results(serv->xdr, xdr_location)) 
            return FALSE;

        ((uint32 *)(serv->xdr->out_msg))[RPC_OFFSET] =
            ((uint32 *)(serv->xdr->in_msg))[RPC_OFFSET]; //RPC xid
        LIBRPC_DEBUG("%08x:%d sending RPC reply (XID %d)\n",
          serv->xdr->x_prog,
          serv->xdr->x_vers,
          ntohl(((uint32 *)(serv->xdr->out_msg))[RPC_OFFSET]));
        XDR_MSG_SEND(serv->xdr);
        return TRUE;
    }
    return FALSE;
}

/* Service error functions. */

#define SVCERR_XDR_SEND(xdr, reply) \
  ( XDR_MSG_START(xdr, RPC_MSG_REPLY) && \
    xdr_send_reply_header(xdr, &reply) && \
    XDR_MSG_SEND(xdr) )

void svcerr_decode (SVCXPRT *xprt)
{
    registered_server *serv = (registered_server *)xprt;
    if (serv->active) { 
        rpc_reply_header reply;
        reply.stat = RPC_MSG_ACCEPTED;
        reply.u.ar.verf = serv->xdr->verf;
        reply.u.ar.stat = RPC_GARBAGE_ARGS;
        
        if (!SVCERR_XDR_SEND(serv->xdr, reply))
            /* Couldn't send the reply - just give up */
            XDR_MSG_ABORT(serv->xdr);
    }
} /* svcerr_decode */

void svcerr_systemerr (SVCXPRT *xprt)
{
    registered_server *serv = (registered_server *)xprt;
    if (serv->active) { 
        rpc_reply_header reply;
        reply.stat = RPC_MSG_ACCEPTED;
        reply.u.ar.verf = serv->xdr->verf;
        reply.u.ar.stat = RPC_SYSTEM_ERR;
        
        if (!SVCERR_XDR_SEND(serv->xdr, reply))
            /* Couldn't send the reply - just give up */
            XDR_MSG_ABORT(serv->xdr);
    }
} /* svcerr_systemerr */

void svcerr_noproc(SVCXPRT *xprt)
{
    registered_server *serv = (registered_server *)xprt;
    if (serv->active) { 
        rpc_reply_header reply;        
        reply.stat = RPC_MSG_ACCEPTED;
        reply.u.ar.verf = serv->xdr->verf;
        reply.u.ar.stat = RPC_PROC_UNAVAIL;
        
        if (!SVCERR_XDR_SEND(serv->xdr, reply))
            /* Couldn't send the reply - just give up */
            XDR_MSG_ABORT(serv->xdr);        
    }
} /* svcerr_noproc */
