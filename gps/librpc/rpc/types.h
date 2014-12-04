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

/* fixincludes should not add extern "C" to this file */
/*
 * Rpc additions to <sys/types.h>
 */
/* Copyright (c) 2011, Code Aurora Forum. */
#ifndef RPCXDRTYPES_H
#define RPCXDRTYPES_H

/*
 * XDR provides a conventional way for converting between C data
 * types and an external bit-string representation.  Library supplied
 * routines provide for the conversion on built-in C data types.  These
 * routines and utility routines defined here are used to help implement
 * a type encode/decode routine for each user-defined type.
 *
 * Each data type provides a single procedure which takes two arguments:
 *
 *      bool_t
 *      xdrproc(xdrs, argresp)
 *              XDR *xdrs;
 *              <type> *argresp;
 *
 * xdrs is an instance of a XDR handle, to which or from which the data
 * type is to be converted.  argresp is a pointer to the structure to be
 * converted.  The XDR handle contains an operation field which indicates
 * which of the operations (ENCODE, DECODE * or FREE) is to be performed.
 *
 * XDR_DECODE may allocate space if the pointer argresp is null.  This
 * data can be freed with the XDR_FREE operation.
 *
 * We write only one procedure per data type to make it easy
 * to keep the encode and decode procedures for a data type consistent.
 * In many cases the same code performs all operations on a user defined type,
 * because all the hard work is done in the component type routines.
 * decode as a series of calls on the nested data types.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* The version of ONCRPC supported */
#define RPC_MSG_VERSION    ((u_long) 2)

#include <inttypes.h>
#include <string.h>
#include <pthread.h>

typedef int bool_t; /* This has to be a long, as it is used for XDR boolean too, which is a 4-byte value */
typedef unsigned long rpcprog_t;
typedef unsigned long rpcproc_t;
typedef unsigned long rpcvers_t;
typedef unsigned long rpcprot_t;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t  uint8;
typedef int32_t  int32;
typedef int16_t  int16;
typedef int8_t   int8;
typedef int32_t  enum_t;
typedef int64_t  quad_t;
typedef uint64_t u_quad_t;
//typedef uint16_t u_short; /* defined in kernel_headers */
#define TRUE 1
#define FALSE 0
static inline void* mem_alloc(size_t size) { return malloc(size); }
static inline void mem_free(void* ptr) { free(ptr); }

/*
 * Xdr operations.  XDR_ENCODE causes the type to be encoded into the
 * stream.  XDR_DECODE causes the type to be extracted from the stream.
 * XDR_FREE can be used to release the space allocated by an XDR_DECODE
 * request.
 */
enum xdr_op {
  XDR_ENCODE = 0,
  XDR_DECODE = 1,
  XDR_FREE = 2
};

/*
 * This is the number of bytes per unit of external data.
 */
#define BYTES_PER_XDR_UNIT  (4)

/*
 * The XDR handle.
 * Contains operation which is being applied to the stream,
 * an operations vector for the particular implementation (e.g. see xdr_mem.c),
 * and two private fields for the use of the particular implementation.
 */
typedef struct xdr_struct XDR;
/*
 * A xdrproc_t exists for each data type which is to be encoded or decoded.
 *
 * The second argument to the xdrproc_t is a pointer to an opaque pointer.
 * The opaque pointer generally points to a structure of the data type
 * to be decoded.  If this pointer is 0, then the type routines should
 * allocate dynamic storage of the appropriate size and return it.
 * bool_t       (*xdrproc_t)(XDR *, caddr_t *);
 */
typedef bool_t (*xdrproc_t) (XDR *, void *,...);


#define ONCRPC_CONTROL_GET_MTU                (1)
#define ONCRPC_CONTROL_GET_TX_QUOTA           (2)
#define ONCRPC_CONTROL_GET_RX_BUFFER_SIZE     (3)
#define ONCRPC_CONTROL_REGISTER_SERVER        (4)
#define ONCRPC_CONTROL_UNREGISTER_SERVER      (5)
#define ONCRPC_CONTROL_GET_DEST               (6)
#define ONCRPC_CONTROL_OPEN_XPORT             (7)
#define ONCRPC_CONTROL_CLOSE_XPORT            (8)
#define ONCRPC_CONTROL_SET_DEST               (9)
#define ONCRPC_CONTROL_GET_SOURCE_ADDR        (10)

typedef struct oncrpc_prog_ver_struct
{
  rpcprog_t prog;
  rpcvers_t ver;
  void (*dispatch)();
} oncrpc_prog_ver_type;

typedef uint64  oncrpc_addr_type;

typedef struct {
  oncrpc_addr_type        addr;
  oncrpc_prog_ver_type    prog_ver;
} oncrpc_control_get_dest_type;

typedef struct {
  oncrpc_addr_type        addr;  
} oncrpc_control_get_source_type;

typedef struct{
  oncrpc_prog_ver_type   prog_ver;
} oncrpc_control_register_server_type;

typedef struct{
  oncrpc_prog_ver_type   prog_ver;
} oncrpc_control_unregister_server_type;

typedef struct{
  oncrpc_addr_type  dest;
} oncrpc_control_set_dest_type;

typedef struct{
  unsigned int xp;
  unsigned int port;
} oncrpc_control_open_xport_type;

#define NULL_xdrproc_t ((xdrproc_t)0)

/*
 * Support struct for discriminated unions.
 * You create an array of xdrdiscrim structures, terminated with
 * a entry with a null procedure pointer.  The xdr_union routine gets
 * the discriminant value and then searches the array of structures
 * for a matching value.  If a match is found the associated xdr routine
 * is called to handle that part of the union.  If there is
 * no match, then a default routine may be called.
 * If there is no match and no default routine it is an error.
 */
struct xdr_discrim
{
  int value;
  xdrproc_t proc;
};

/* Message enums */
typedef enum {
  RPC_MSG_CALL=0,
  RPC_MSG_REPLY=1,
  RPC_MSG_UNDEF = 2,
} rpc_msg_e_type;

typedef enum {
  RPC_MSG_ACCEPTED=0,
  RPC_MSG_DENIED=1
} rpc_reply_stat_e_type;

typedef enum {
  RPC_ACCEPT_SUCCESS = 0,
  RPC_PROG_UNAVAIL   = 1,
  RPC_PROG_MISMATCH  = 2,
  RPC_PROC_UNAVAIL   = 3,
  RPC_GARBAGE_ARGS   = 4,
  RPC_SYSTEM_ERR     = 5,
  RPC_PROG_LOCKED    = 6
} rpc_accept_stat_e_type;

typedef enum {
  RPC_MISMATCH=0,
  RPC_AUTH_ERROR=1
} rpc_reject_stat_e_type ;

/* Auth types */

/*
 * Status returned from authentication check
 */
typedef enum {
  AUTH_OK=0,
  /*
   * failed at remote end
   */
  AUTH_BADCRED=1,       /* bogus credentials (seal broken) */
  AUTH_REJECTEDCRED=2,  /* client should begin new session */
  AUTH_BADVERF=3,       /* bogus verifier (seal broken) */
  AUTH_REJECTEDVERF=4,  /* verifier expired or was replayed */
  AUTH_TOOWEAK=5,       /* rejected due to security reasons */
  /*
   * failed locally
   */
  AUTH_INVALIDRESP=6,   /* bogus response verifier */
  AUTH_FAILED=7         /* some unknown reason */
} auth_stat;

typedef enum {
  AUTH_NONE  =0,   /* no authentication */
  AUTH_NULL  =0,   /* backward compatibility */
  AUTH_SYS   =1,   /* unix style (uid, gids) */
  AUTH_UNIX  =1, 
  AUTH_SHORT =2    /* short hand unix style */
} oncrpc_auth_types;
/*
 * Authentication info.  Opaque to client.
 */
typedef struct opaque_auth {
  oncrpc_auth_types oa_flavor;    /* flavor of auth */
  caddr_t           oa_base;      /* address of more auth stuff */
  u_int             oa_length;    /* not to exceed MAX_AUTH_BYTES */
} opaque_auth;

#define MAX_AUTH_BYTES  400
#define MAXNETNAMELEN   255  /* maximum length of network user's name */

/* Error types */
/*
 * Reply header to an rpc request that was accepted by the server.
 * Note: there could be an error even though the request was
 * accepted.
 */
struct rpc_accepted_reply_header
{
  opaque_auth              verf;
  rpc_accept_stat_e_type   stat;
  union
  {
    struct
    {
      uint32 low;
      uint32 high;
    } versions;
  } u;
};

/*
 * Reply to an rpc request that was denied by the server.
 */
struct rpc_denied_reply
{
  rpc_reject_stat_e_type stat;
  union
  {
    struct
    {
      uint32 low;
      uint32 high;
    } versions;
    auth_stat why;  /* why authentication did not work */
  } u;
};

/*
 * RPC reply header structure. The reply header contains error codes in
 * case of errors in the server side or the RPC call being rejected.
 */
typedef struct rpc_reply_header
{
  rpc_reply_stat_e_type stat;
  union
  {
    struct rpc_accepted_reply_header ar;
    struct rpc_denied_reply dr;
  } u;
} rpc_reply_header;

/* XDR memory wrapper structure */
typedef struct oncrpcxdr_mem_struct {
  struct oncrpcxdr_mem_struct *next;

#ifdef IMAGE_APPS_PROC
  /* make structure size 8-bytes so we
     keep 8-byte alignment */
  uint32 padding;
#endif
} oncrpcxdr_mem_s_type;

// TODO - keep XPORT objects on queue to help track down memory leaks

/*===========================================================================
  Defining the XPORT structure
  ===========================================================================*/

#define XPORT_FLAG_XPORT_ALLOCED        0x0001

/*===========================================================================
  Defining the XDR structure
  ===========================================================================*/

typedef struct xdr_struct xdr_s_type;

/* Call back definition for non-blocking RPC calls */
typedef void (*rpc_reply_cb_type)(xdr_s_type *xdr, void *data);

/* Entry points that must be provided by xdr */
struct xdr_ops_struct {
  /* Transport control functions */
  void        (*xdr_destroy) (xdr_s_type *xdr);
  bool_t      (*xdr_control) (xdr_s_type *xdr, int request, void *info);

  /* Incoming message control functions */
  bool_t (*read)           (xdr_s_type *xdr);
  bool_t (*msg_done)       (xdr_s_type *xdr);

  /* Outgoing message control functions */
  bool_t (*msg_start) (xdr_s_type *xdr, rpc_msg_e_type rpc_msg_type);
  bool_t (*msg_abort) (xdr_s_type *xdr);
  bool_t (*msg_send)  (xdr_s_type *xdr);
  
  /* Message data functions */
  bool_t (*send_int8)   (xdr_s_type *xdr, const int8 *value);
  bool_t (*send_uint8)  (xdr_s_type *xdr, const uint8 *value);
  bool_t (*send_int16)  (xdr_s_type *xdr, const int16 *value);
  bool_t (*send_uint16) (xdr_s_type *xdr, const uint16 *value);
  bool_t (*send_int32)  (xdr_s_type *xdr, const int32 *value);
  bool_t (*send_uint32) (xdr_s_type *xdr, const uint32 *value);
  bool_t (*send_bytes)  (xdr_s_type *xdr, const uint8 *buf, uint32 len);

  bool_t (*recv_int8)   (xdr_s_type *xdr, int8 *value);
  bool_t (*recv_uint8)  (xdr_s_type *xdr, uint8 *value);
  bool_t (*recv_int16)  (xdr_s_type *xdr, int16 *value);
  bool_t (*recv_uint16) (xdr_s_type *xdr, uint16 *value);
  bool_t (*recv_int32)  (xdr_s_type *xdr, int32 *value);
  bool_t (*recv_uint32) (xdr_s_type *xdr, uint32 *value);
  bool_t (*recv_bytes)  (xdr_s_type *xdr, uint8 *buf, uint32 len);
};

typedef struct xdr_ops_struct xdr_ops_s_type;

/*===========================================================================
  XDR structure definition - provides a generic interface to each
  supported transport. The xdr structure is used both for clients and
  for servers.
  ===========================================================================*/

#define RPCROUTER_MSGSIZE_MAX (18432)

struct xdr_struct {
  const xdr_ops_s_type      *xops;
  enum xdr_op                x_op;           /* used for ENCODE and DECODE */
  uint32                     x_prog;         /* program number */
  uint32                     x_vers;         /* program version */
  uint32                     x_proc;         /* for debug output */

  opaque_auth                verf;           /* verf to send back */
  uint32                     xid;
  int                        fd;
  int                        is_client;

  /* RPC-call message (if XDR is a client) or RPC-reply message (if
     XDR is a server). */

  uint8                      out_msg[RPCROUTER_MSGSIZE_MAX];
  int                        out_next;

  /* Reply message or incoming-call message.  For a client XDR, this
     buffer always contains the reply received in response to an RPC
     call.  For a server XDR, this buffer always contains an incoming
     RPC call.
  */
  uint8                      in_msg[RPCROUTER_MSGSIZE_MAX];
  int                        in_next;
  int                        in_len;
  int                        xdr_err;
};

// Transport flag definitions
#define XDR_FLAG_XDR_ALLOCED        0x0001
#define XDR_FLAG_DEDICATED          0x0002
#define XDR_FLAG_DESTROYING         0x0004
#define XDR_FLAG_RETRY              0x0008


/// @todo FIXME this goes with the callback stuff in oncrpcxdr.c. Move
/// elsewhere and pick better symbol names.
typedef struct rpc_cb_data_type {
  void                     * cb_handler;
  uint32                     cb_id;
  rpcprot_t                  protocol;
  oncrpc_addr_type           cb_source;
} rpc_cb_data_type;


/* Events returned by the server and clients when a reset event occurs.
 */
enum rpc_reset_event {
  RPC_SUBSYSTEM_RESTART_BEGIN,
  RPC_SUBSYSTEM_RESTART_END
};

#ifdef __cplusplus
}
#endif

#endif /* _RPC_XDR_TYPES_H */
