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
 * svc.h, Server-side remote procedure call interface.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 * Copyright (c) 2011, Code Aurora Forum.
 */

#ifndef _RPC_SVC_H
#define _RPC_SVC_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <rpc/types.h>

/*
 * This interface must manage two items concerning remote procedure calling:
 *
 * 1) An arbitrary number of transport connections upon which rpc requests
 * are received.  The two most notable transports are TCP and UDP;  they are
 * created and registered by routines in svc_tcp.c and svc_udp.c, respectively;
 * they in turn call xprt_register and xprt_unregister.
 *
 * 2) An arbitrary number of locally registered services.  Services are
 * described by the following four data: program number, version number,
 * "service dispatch" function, a transport handle, and a bool_t that
 * indicates whether or not the exported program should be registered with a
 * local binder service;  if true the program's number and version and the
 * port number from the transport handle are registered with the binder.
 * These data are registered with the rpc svc system via svc_register.
 *
 * A service's dispatch function is called whenever an rpc request comes in
 * on a transport.  The request's program and version numbers must match
 * those of the registered service.  The dispatch function is passed two
 * parameters, struct svc_req * and SVCXPRT *, defined below.
 */

/*
 * Server side transport handle
 */
struct SVCXPRT;
typedef struct SVCXPRT SVCXPRT;

/*
 * Service request
 */
struct svc_req {
  rpcprog_t rq_prog;            /* service program number */
  rpcvers_t rq_vers;            /* service protocol version */
  rpcproc_t rq_proc;            /* the desired procedure */
  SVCXPRT *rq_xprt;             /* associated transport */
};

#ifndef __DISPATCH_FN_T
#define __DISPATCH_FN_T
typedef void (*__dispatch_fn_t) (struct svc_req*, SVCXPRT*);
#endif


/* Reset notifiction callback.
 *
 * Called when the reset state changes for the client.
 */
typedef void (*svc_reset_notif_cb)
(
  SVCXPRT* xprt,
  enum rpc_reset_event event
);

/*
 * Transport registration.
 *
 * xprt_register(xprt)
 *      SVCXPRT *xprt;
 */
extern void xprt_register (SVCXPRT *__xprt);

/*
 * Transport un-register
 *
 * xprt_unregister(xprt)
 *      SVCXPRT *xprt;
 */
extern void xprt_unregister (SVCXPRT *__xprt);

/*
 * Service registration (registers only with plugger module)
 *
 * svc_register_with_plugger(xprt, prog, vers, dispatch, protocol)
 *	SVCXPRT *xprt;
 *	rpcprog_t prog;
 *	rpcvers_t vers;
 *	void (*dispatch)(struct svc_req*, SVCXPRT*);
 *	rpcprot_t protocol;  like TCP or UDP, zero means do not register
 */
extern bool_t svc_register_with_plugger (SVCXPRT *__xprt, rpcprog_t __prog,
                                         rpcvers_t __vers,
                                         __dispatch_fn_t __dispatch,
                                         rpcprot_t __protocol);

/*
 * Service registration (registers with plugger module and lower layers)
 *
 * svc_register(xprt, prog, vers, dispatch, protocol)
 *	SVCXPRT *xprt;
 *	rpcprog_t prog;
 *	rpcvers_t vers;
 *	void (*dispatch)(struct svc_req*, SVCXPRT*);
 *	rpcprot_t protocol;  like TCP or UDP, zero means do not register
 */
extern bool_t svc_register (SVCXPRT *__xprt, rpcprog_t __prog,
                            rpcvers_t __vers, __dispatch_fn_t __dispatch,
                            rpcprot_t __protocol);

extern void svc_destroy(SVCXPRT *xprt);

/*
 * Service un-registration
 *
 * svc_unregister(xprt, prog, vers)
 *  SVCXPRT *xprt
 *	rpcprog_t prog;
 *	rpcvers_t vers;
 */
void
svc_unregister (SVCXPRT *__xprt, rpcprog_t prog, rpcvers_t vers);

/*
 * Service Enable
 *
 * svc_enable( prog, vers )
 *	rpcprog_t prog;
 *	rpcvers_t vers;
 */
#define svc_enable(prog, vers) svc_lock(prog, vers, FALSE)

/*
 * Service Disable
 *
 * svc_disable( prog, vers )
 *	rpcprog_t prog;
 *	rpcvers_t vers;
 */
#define svc_disable(prog, vers) svc_lock(prog, vers, TRUE)

extern void svc_lock(rpcprog_t __prog, rpcvers_t __vers, bool_t __lock);

/*
 * When the service routine is called, it must first check to see if it
 * knows about the procedure;  if not, it should call svcerr_noproc
 * and return.  If so, it should deserialize its arguments via
 * SVC_GETARGS (defined above).  If the deserialization does not work,
 * svcerr_decode should be called followed by a return.  Successful
 * decoding of the arguments should be followed the execution of the
 * procedure's code and a call to svc_sendreply.
 *
 * Also, if the service refuses to execute the procedure due to too-
 * weak authentication parameters, svcerr_weakauth should be called.
 * Note: do not confuse access-control failure with weak authentication!
 *
 * NB: In pure implementations of rpc, the caller always waits for a reply
 * msg.  This message is sent when svc_sendreply is called.
 * Therefore pure service implementations should always call
 * svc_sendreply even if the function logically returns void;  use
 * xdr.h - xdr_void for the xdr routine.  HOWEVER, tcp based rpc allows
 * for the abuse of pure rpc via batched calling or pipelining.  In the
 * case of a batched call, svc_sendreply should NOT be called since
 * this would send a return message, which is what batching tries to avoid.
 * It is the service/protocol writer's responsibility to know which calls are
 * batched and which are not.  Warning: responding to batch calls may
 * deadlock the caller and server processes!
 */

extern bool_t svc_getargs(SVCXPRT *xdr, xdrproc_t xdr_args, caddr_t args_ptr);
extern bool_t svc_freeargs(SVCXPRT *xdr, xdrproc_t xdr_args, caddr_t args_ptr);

extern bool_t	svc_sendreply (SVCXPRT *xprt, xdrproc_t __xdr_results,
			       caddr_t __xdr_location);

/*===========================================================================
FUNCTION svc_register_reset_notification_cb

DESCRIPTION
  Registers a callback that is called if a subsystem restart (modem restart)
  is encountered. Note that this callback is made on the context of the
  servers callback or server thread, so blocking calls cannot be made.

  Two calls will be generated.  The first will be with an event type of
  RPC_SUBSYSTEM_RESTART_BEGIN which signals that the modem has started its
  reset.  Once the modem comes out of reset, another call will be generated
  with the event type of RPC_SUBSYSTEM_RESTART_END.

DEPENDENCIES
  None.

ARGUMENTS
  xprt - pointer to the client
  cb - callback function of type svc_reset_notif_cb

RETURN VALUE
  0 - if successful
  error code otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern int svc_register_reset_notification_cb(SVCXPRT *xprt, svc_reset_notif_cb cb);

/*===========================================================================
FUNCTION svc_unregister_reset_notification_cb

DESCRIPTION
  Unregisters any callback registered by svc_register_reset_notification_cb.

  The previous callback function is returned.

DEPENDENCIES
  None.

ARGUMENTS
  xprt - pointer to the server transport

RETURN VALUE
  Pointer to previous callback (NULL if no previous callback registered).
  NULL if xprt is NULL

SIDE EFFECTS
  None.
===========================================================================*/
extern svc_reset_notif_cb svc_unregister_reset_notification_cb(SVCXPRT *xprt);

/*
 * Socket to use on svcxxx_create call to get default socket
 */
#define	RPC_ANYSOCK	-1

/* 
 * Router based rpc.
 */
extern SVCXPRT *svcrtr_create (void);

extern void svcerr_decode (SVCXPRT *);
extern void svcerr_weakauth (SVCXPRT *);
extern void svcerr_noproc (SVCXPRT *);
extern void svcerr_noprog (SVCXPRT *);
extern void svcerr_systemerr (SVCXPRT *);
extern void svcerr_progvers (SVCXPRT *, rpcvers_t __low_vers, rpcvers_t __high_vers);
extern void svcerr_auth (SVCXPRT *, auth_stat __why);

#ifdef __cplusplus
}
#endif

#endif /* rpc/svc.h */
