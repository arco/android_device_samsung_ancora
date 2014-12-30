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
 * xdr.h, External Data Representation Serialization Routines.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#ifndef XDR_H
#define XDR_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
  Macros for calling primitive XDR routines (this is independent of RPC)
  ===========================================================================*/

#define XDR_RECV_BYTES(XDR, BUF, LEN) (XDR)->xops->recv_bytes(XDR, BUF, LEN)
#define XDR_RECV_INT16(XDR, VALUE)    (XDR)->xops->recv_int16(XDR, VALUE)
#define XDR_RECV_INT32(XDR, VALUE)    (XDR)->xops->recv_int32(XDR, VALUE)
#define XDR_RECV_INT8(XDR, VALUE)     (XDR)->xops->recv_int8(XDR, VALUE)
#define XDR_RECV_UINT(XDR, VALUE)     (XDR)->xops->recv_uint32(XDR, (uint32 *)(VALUE))
#define XDR_RECV_UINT32(XDR, VALUE)   (XDR)->xops->recv_uint32(XDR, VALUE)
#define XDR_SEND_BYTES(XDR, BUF, LEN) (XDR)->xops->send_bytes(XDR, BUF, LEN)
#define XDR_SEND_INT16(XDR, VALUE)    (XDR)->xops->send_int16(XDR, VALUE)
#define XDR_SEND_INT32(XDR, VALUE)    (XDR)->xops->send_int32(XDR, VALUE)
#define XDR_SEND_INT8(XDR, VALUE)     (XDR)->xops->send_int8(XDR, VALUE)
#define XDR_SEND_UINT(XDR, VALUE)     (XDR)->xops->send_uint32(XDR, (uint32 *)(VALUE))
#define XDR_SEND_UINT32(XDR, VALUE)   (XDR)->xops->send_uint32(XDR, VALUE)

/*===========================================================================
  Macros for sending and receiving an RPC message through the transport
  ===========================================================================*/

#define XDR_MSG_START(XDR, TYPE)      (XDR)->xops->msg_start(XDR, TYPE)
#define XDR_MSG_SEND(XDR)             (XDR)->xops->msg_send(XDR)
#define XDR_MSG_DONE(XDR)             (XDR)->xops->msg_done(XDR)
#define XDR_MSG_ABORT(XDR)            (XDR)->xops->msg_abort(XDR)

extern bool_t xdr_call_msg_start (XDR *xdr, uint32 prog, uint32 ver, uint32 proc, opaque_auth *cred, opaque_auth *verf);
extern bool_t xdr_reply_msg_start (XDR *xdr, opaque_auth *verf);
extern bool_t xdr_send_auth (XDR *xdr, const opaque_auth *auth);
extern bool_t xdr_send_reply_header (XDR *xdr, rpc_reply_header const *reply);
extern void   xdr_free (xdrproc_t proc, char *objp);

/***********************************************************************
        Support for rpcgen
 ***********************************************************************/

#define XDR_SEND_ENUM(XDR, VALUE) xdr_send_enum(XDR, (void *) (VALUE), sizeof(*(VALUE)))
#define XDR_RECV_ENUM(XDR, VALUE) xdr_recv_enum(XDR, (void *) (VALUE), sizeof(*(VALUE)))
extern bool_t xdr_send_enum (XDR *xdr, const void *value, uint32 size);
extern bool_t xdr_recv_enum (XDR *xdr, void *value, uint32 size);

extern bool_t xdr_bytes (XDR *xdr, char **cpp, u_int *sizep, u_int maxsize);
extern bool_t xdr_enum (XDR *xdr, enum_t *ep);
extern bool_t xdr_pointer (XDR *xdrs, char **_objpp, u_int obj_size, xdrproc_t xdr_obj);
extern bool_t xdr_int (XDR *xdr, int *ip);
extern bool_t xdr_u_int (XDR *xdr, u_int *ip);
extern bool_t xdr_char (XDR *xdr, char *cp);
extern bool_t xdr_u_char (XDR *xdr, u_char *cp);
extern bool_t xdr_long (XDR *xdr, long *ulp);
extern bool_t xdr_u_long (XDR *xdr, u_long *ulp);
#define xdr_uint32 xdr_u_long
extern bool_t xdr_quad_t (XDR *xdrs, quad_t *llp);
extern bool_t xdr_u_quad_t (XDR *__xdrs, u_quad_t *__ullp);
extern bool_t xdr_short (XDR *xdr, short *sp);
extern bool_t xdr_u_short (XDR *xdr, u_short *usp);
extern bool_t xdr_vector (XDR *xdrs, char *basep, u_int nelem, u_int elemsize, xdrproc_t xdr_elem);
extern bool_t xdr_void (void);
extern bool_t xdr_opaque (XDR *xdr, caddr_t cp, u_int cnt);
extern bool_t xdr_string (XDR *xdr, char **cpp, u_int maxsize);

extern bool_t xdr_array (
    XDR *xdrs,
    caddr_t *addrp,/* array pointer */
    u_int *sizep,  /* number of elements */
    u_int maxsize,  /* max numberof elements */
    u_int elsize,  /* size in bytes of each element */
    xdrproc_t elproc); /* xdr routine to handle each element */

#ifdef __cplusplus
}
#endif

#endif /* rpc/xdr.h */
