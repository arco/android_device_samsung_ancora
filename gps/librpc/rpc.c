#include <rpc/rpc.h>

/*
 * Functions to compose RPC messages from XDR primitives
 */

bool_t xdr_call_msg_start(
    xdr_s_type *xdr,
    uint32 prog,
    uint32 ver,
    uint32 proc,
    opaque_auth *cred,
    opaque_auth *verf)
{
  uint32 vers = RPC_MSG_VERSION;

  xdr->x_prog = prog;
  xdr->x_proc = proc;

  return (XDR_MSG_START(xdr, RPC_MSG_CALL) &&
          XDR_SEND_UINT32(xdr, &vers) &&
          XDR_SEND_UINT32(xdr, &prog) &&
          XDR_SEND_UINT32(xdr, &ver) &&
          XDR_SEND_UINT32(xdr, &proc) &&
          xdr_send_auth(xdr, cred) &&
          xdr_send_auth(xdr, verf));
} // xdr_call_msg_start

bool_t xdr_reply_msg_start(
    xdr_s_type *xdr,
    opaque_auth *verf)
{
  int32 stat   = (int32) RPC_MSG_ACCEPTED;
  int32 accept = (int32) RPC_ACCEPT_SUCCESS;

  return(XDR_MSG_START(xdr, RPC_MSG_REPLY) &&
          XDR_SEND_INT32(xdr, &stat) &&
          xdr_send_auth(xdr, verf) &&
          XDR_SEND_INT32(xdr, &accept));
} // xdr_reply_msg_start

static bool_t xdr_send_accepted_reply_header(
    xdr_s_type *xdr,
    struct rpc_accepted_reply_header const *accreply)
{
  if (!xdr_send_auth(xdr, &accreply->verf))
      return FALSE;

  if (!XDR_SEND_ENUM(xdr, &accreply->stat))
      return FALSE;

  switch ((*accreply).stat){
  case RPC_PROG_MISMATCH:
    if (!XDR_SEND_UINT32(xdr, &accreply->u.versions.low))
        return FALSE;

    if (!XDR_SEND_UINT32(xdr, &accreply->u.versions.high))
        return FALSE;
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
} /* xdr_send_accepted_reply_header */

static bool_t xdr_send_denied_reply(
    xdr_s_type *xdr,
    struct rpc_denied_reply const *rejreply)
{
    if (!XDR_SEND_ENUM(xdr, &rejreply->stat))
        return FALSE;

    switch ((*rejreply).stat){
    case RPC_MISMATCH:
        if (!XDR_SEND_UINT32(xdr, &rejreply->u.versions.low))
            return FALSE;
        if (!XDR_SEND_UINT32(xdr, &rejreply->u.versions.high))
            return FALSE;
        break;
    case RPC_AUTH_ERROR:
        if (!XDR_SEND_ENUM(xdr, &rejreply->u.why))
            return FALSE;
        break;
    default:
        return FALSE;
    }

    return TRUE;
} /* xdr_send_denied_reply */

bool_t xdr_send_reply_header(
    xdr_s_type *xdr,
    rpc_reply_header const *reply)
{
    if (!XDR_SEND_ENUM(xdr, &reply->stat))
        return FALSE;

    switch ((*reply).stat) {
    case RPC_MSG_ACCEPTED:
        if (!xdr_send_accepted_reply_header(xdr, &reply->u.ar))
            return FALSE;
        break;
    case RPC_MSG_DENIED:
        if (!xdr_send_denied_reply(xdr, &reply->u.dr))
            return FALSE;
        break;
    default:
        return FALSE;
    }

    return TRUE;
} /* xdr_send_reply_header */

#include <stdio.h>

bool_t
xdr_send_auth(xdr_s_type *xdr, const opaque_auth *auth)
{
#define FAILIF(x) do { if (x) return FALSE; } while(0)

    switch (sizeof(auth->oa_flavor)) {
    case 1:
        FAILIF(!XDR_SEND_INT8(xdr, (int8_t *)&auth->oa_flavor));
        break;
    case 2:
        FAILIF(!XDR_SEND_INT16(xdr, (int16_t *)&auth->oa_flavor));
        break;
    case 4:
        FAILIF(!XDR_SEND_INT32(xdr, (int32_t *)&auth->oa_flavor));
        break;
    default:
        return FALSE;
    }

    return (XDR_SEND_UINT(xdr, (uint32_t *)&auth->oa_length) &&
            (auth->oa_length == 0 ||
             XDR_SEND_BYTES(xdr, (uint8_t *)auth->oa_base, auth->oa_length)));
}

void xdr_free(xdrproc_t proc, char *objp)
{
    XDR x;
    x.x_op = XDR_FREE;
    (*proc)(&x, objp);
}
