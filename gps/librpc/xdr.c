#include <rpc/rpc.h>
#include <string.h>

#define LASTUNSIGNED    ((u_int)((int)0-1))

/* 
 * Primitives for stuffing data into and retrieving data from an XDR 
 */

bool_t xdr_bytes (XDR *xdr, char **cpp, u_int *sizep, u_int maxsize)
{
    switch(xdr->x_op) {
    case XDR_DECODE:
        if(!XDR_RECV_UINT(xdr, sizep) || *sizep > maxsize)
            return FALSE;        
        if(*sizep == 0)
            return TRUE;        
        if(*cpp == NULL)
            *cpp = (char *) mem_alloc(*sizep);
        if(*cpp == NULL) return FALSE;
        return XDR_RECV_BYTES(xdr, (uint8 *) *cpp, *sizep);        
    case XDR_ENCODE:
        return (XDR_SEND_UINT(xdr, sizep) &&
                *sizep <= maxsize &&
                XDR_SEND_BYTES(xdr, (uint8 *) *cpp, *sizep));        
    case XDR_FREE:
        if (*cpp) {
            mem_free(*cpp);
            *cpp = NULL;
        }
        return TRUE;        
    default:
        break;
    }
    return FALSE;
} /* xdr_bytes */

bool_t xdr_send_enum (xdr_s_type *xdr, const void *value, uint32 size)
{
    switch (size) {
    case 4:
        return XDR_SEND_INT32(xdr, (int32 *) value);
    case 2:
        return XDR_SEND_INT16(xdr, (int16 *) value);
    case 1:
        return XDR_SEND_INT8(xdr, (int8 *) value);
    default:
        return FALSE;
    }
} /* xdr_send_enum */

bool_t xdr_recv_enum (xdr_s_type *xdr, void *value, uint32 size)
{
    switch (size) {
    case 4:
        return XDR_RECV_INT32(xdr, (int32 *) value);
    case 2:
        return XDR_RECV_INT16(xdr, (int16 *) value);
    case 1:
        return XDR_RECV_INT8(xdr, (int8 *) value);
    default:
        return FALSE;
    }
} /* xdr_recv_enum */

#include <stdio.h>

bool_t xdr_enum (XDR *xdr, enum_t *ep)
{
    switch(xdr->x_op) {
    case XDR_ENCODE:
        return XDR_SEND_INT32(xdr, (int32 *)ep);  
    case XDR_DECODE:
        return XDR_RECV_INT32(xdr, (int32 *)ep);
    case XDR_FREE:
        return TRUE;
    default:
        break;
    }
    return FALSE;
} /* xdr_enum */

bool_t xdr_u_int (XDR *xdr, u_int *uip)
{
    switch(xdr->x_op) {
    case XDR_ENCODE:
        return XDR_SEND_UINT32(xdr, (uint32 *) uip);
    case XDR_DECODE:
        return XDR_RECV_UINT32(xdr, (uint32 *) uip);
    case XDR_FREE:
        return TRUE;
    default:
        break;
    }
    return FALSE;
} /* xdr_u_int */

bool_t xdr_u_char (XDR *xdr, u_char *cp)
{
    u_int u = (*cp);
    if (!xdr_u_int (xdr, &u))
        return FALSE;
    *cp = (u_char) u;
    return TRUE;
} /* xdr_u_char */

bool_t xdr_long (XDR *xdr, long *lp)
{
    switch (xdr->x_op) {
    case XDR_ENCODE:
        return XDR_SEND_INT32(xdr, (int32_t *)lp);
    case XDR_DECODE:
        return XDR_RECV_INT32(xdr, (int32_t *)lp);
    case XDR_FREE:
        return TRUE;
    default:
        break;
    }
    return FALSE;
} /* xdr_long */

bool_t xdr_u_long (XDR *xdr, u_long *ulp)
{
    switch (xdr->x_op) {
    case XDR_ENCODE:
        return XDR_SEND_UINT32(xdr, (uint32_t *)ulp);
    case XDR_DECODE:
        return XDR_RECV_UINT32(xdr, (uint32_t *)ulp);
    case XDR_FREE:
        return TRUE;
    default:
        break;
    }
    return FALSE;
} /* xdr_u_long */

/*
 * XDR hyper integers
 * same as xdr_hyper - open coded to save a proc call!
 */
bool_t xdr_u_hyper (XDR *xdrs, u_quad_t *ullp)
{
    unsigned long t1;
    unsigned long t2;
    
    if (xdrs->x_op == XDR_ENCODE) {
        t1 = (unsigned long) ((*ullp) >> 32);
        t2 = (unsigned long) (*ullp);
        return (XDR_SEND_INT32(xdrs, (int32 *)&t1) &&
                XDR_SEND_INT32(xdrs, (int32 *)&t2));
    }

    if (xdrs->x_op == XDR_DECODE) {
        if (!XDR_RECV_INT32(xdrs, (int32 *)&t1) ||
            !XDR_RECV_INT32(xdrs, (int32 *)&t2))
            return FALSE;
        *ullp = ((u_quad_t) t1) << 32;
        *ullp |= t2;
        return TRUE;
    }
    
    return xdrs->x_op == XDR_FREE;
}

bool_t
xdr_u_quad_t (XDR *xdrs, u_quad_t *ullp)
{
    return xdr_u_hyper(xdrs, ullp);
}

bool_t xdr_u_short (XDR *xdr, u_short *usp)
{
    u_long l;

    switch (xdr->x_op) {
    case XDR_ENCODE:
        l = *usp;
        return XDR_SEND_UINT32(xdr, (uint32_t *)&l);
    case XDR_DECODE:
        if(!XDR_RECV_UINT32(xdr, (uint32_t *)&l))
            return FALSE;
        *usp = (u_short)l;
        return TRUE;
    case XDR_FREE:
        return TRUE;      
    default:
        break;
    }

    return FALSE;
} /* xdr_u_short */

/*
 * xdr_vector():
 *
 * XDR a fixed length array. Unlike variable-length arrays,
 * the storage of fixed length arrays is static and unfreeable.
 * > basep: base of the array
 * > size: size of the array
 * > elemsize: size of each element
 * > xdr_elem: routine to XDR each element
 */
bool_t
xdr_vector (XDR *xdrs,
            char *basep,
            u_int nelem,
            u_int elemsize,
            xdrproc_t xdr_elem)
{
    u_int i;
    char *elptr;
    
    elptr = basep;
    for (i = 0; i < nelem; i++) {
        if (!(*xdr_elem) (xdrs, elptr, LASTUNSIGNED))
            return FALSE;
        elptr += elemsize;
    }
    return TRUE;
}

bool_t xdr_bool (XDR *xdr, bool_t *bp)
{
    uint32 lb;
    
    switch(xdr->x_op) {
    case XDR_ENCODE:
        lb = *bp ? TRUE : FALSE;
        return XDR_SEND_UINT32(xdr, &lb);
    case XDR_DECODE:
        if (!XDR_RECV_UINT32(xdr, &lb))
            return FALSE;
        *bp = (lb == FALSE) ? FALSE : TRUE;
        return TRUE;
    case XDR_FREE:
        return TRUE;
    default:
        break;
    }
    
    return FALSE;
} /* xdr_bool */

/*
 * XDR an indirect pointer
 * xdr_reference is for recursively translating a structure that is
 * referenced by a pointer inside the structure that is currently being
 * translated.  pp references a pointer to storage. If *pp is null
 * the  necessary storage is allocated.
 * size is the size of the referneced structure.
 * proc is the routine to handle the referenced structure.
 */
bool_t
xdr_reference (XDR *xdrs,
               caddr_t *pp,     /* the pointer to work on */
               u_int size,      /* size of the object pointed to */
               xdrproc_t proc)   /* xdr routine to handle the object */
{
    bool_t stat;
    
    if (*pp == NULL) {
        switch (xdrs->x_op) {
        case XDR_FREE:
            return TRUE;
            
        case XDR_DECODE:
            *pp = (caddr_t) mem_alloc (size);
            if (*pp == NULL) return FALSE;
            memset(*pp, 0, size);
            break;
        default:
            break;
        }
    }

    stat = (*proc) (xdrs, *pp, LASTUNSIGNED);
    
    if (xdrs->x_op == XDR_FREE) {
        mem_free(*pp);
        *pp = NULL;
    }
    return stat;
} /* xdr_reference */

/*
 * xdr_pointer():
 *
 * XDR a pointer to a possibly recursive data structure. This
 * differs with xdr_reference in that it can serialize/deserialize
 * trees correctly.
 *
 *  What's sent is actually a union:
 *
 *  union object_pointer switch (bool_t b) {
 *  case TRUE: object_data data;
 *  case FALSE: void nothing;
 *  }
 *
 * > objpp: Pointer to the pointer to the object.
 * > obj_size: size of the object.
 * > xdr_obj: routine to XDR an object.
 *
 */

bool_t
xdr_pointer (XDR *xdrs,
             char **objpp,
             u_int obj_size,
             xdrproc_t xdr_obj)
{
    bool_t more_data;
    
    more_data = (*objpp != NULL);
    if (!xdr_bool (xdrs, &more_data))
        return FALSE;
    
    if (!more_data) {
        *objpp = NULL;
        return TRUE;
    }
    return xdr_reference (xdrs, objpp, obj_size, xdr_obj);
} /* xdr_pointer */

bool_t xdr_void (void)
{
    return TRUE;
} /* xdr_void */

/*
 * XDR an array of arbitrary elements
 * *addrp is a pointer to the array, *sizep is the number of elements.
 * If addrp is NULL (*sizep * elsize) bytes are allocated.
 * elsize is the size (in bytes) of each element, and elproc is the
 * xdr procedure to call to handle each element of the array.
 */
bool_t
xdr_array (XDR *xdrs,
           caddr_t *addrp,/* array pointer */
           u_int *sizep,  /* number of elements */
           u_int maxsize,  /* max numberof elements */
           u_int elsize,  /* size in bytes of each element */
           xdrproc_t elproc) /* xdr routine to handle each element */
{
    u_int i;
    caddr_t target = *addrp;
    u_int c;/* the actual element count */
    bool_t stat = TRUE;
    u_int nodesize;

    /* like strings, arrays are really counted arrays */
    if (!xdr_u_int (xdrs, sizep))
        return FALSE;
    c = *sizep;
    if ((c > maxsize) && (xdrs->x_op != XDR_FREE))
        return FALSE;
    nodesize = c * elsize;

    /*
     * if we are deserializing, we may need to allocate an array.
     * We also save time by checking for a null array if we are freeing.
     */
    if (target == NULL)
        switch (xdrs->x_op) {
        case XDR_DECODE:
            if (c == 0)
                return TRUE;
            *addrp = target = mem_alloc (nodesize);
            if (!*addrp) return FALSE;
            memset (target, 0, nodesize);
            break;            
        case XDR_FREE:
            return TRUE;
        default:
            break;
        }
    
    /*
     * now we xdr each element of array
     */
    for (i = 0; (i < c) && stat; i++) {
        stat = (*elproc) (xdrs, target, LASTUNSIGNED);
        target += elsize;
    }

    /*
     * the array may need freeing
     */
    if (xdrs->x_op == XDR_FREE) {
        mem_free(*addrp);
        *addrp = NULL;
    }

    return stat;
}

bool_t xdr_int(XDR *xdr, int *ip)
{
    switch (xdr->x_op) {
    case XDR_ENCODE:
        return XDR_SEND_INT32(xdr, (int32 *) ip);
    case XDR_DECODE:
        return XDR_RECV_INT32(xdr, (int32 *) ip);
    case XDR_FREE:
        return TRUE;
    default:
        break;
    }

    return FALSE;
} /* xdr_int */

bool_t xdr_opaque (XDR *xdr, caddr_t cp, u_int cnt)
{
    /* if no data we are done */
    if (cnt == 0)
        return TRUE;

    switch (xdr->x_op) {
    case XDR_ENCODE:
        return XDR_SEND_BYTES(xdr, (uint8 *) cp, cnt);
    case XDR_DECODE:
        return XDR_RECV_BYTES(xdr, (uint8 *) cp, cnt);
    case XDR_FREE:
        return TRUE;
    default:
        break;
    }

    return FALSE;
} /* xdr_opaque */

bool_t xdr_char (XDR *xdr, char *cp)
{
    int i;
    i = (*cp);
    if (!xdr_int (xdr, &i))
        return FALSE;
    *cp = i;
    return TRUE;
} /* xdr_char */

bool_t
xdr_quad_t (XDR *xdrs, quad_t *llp)
{
    return xdr_u_quad_t(xdrs, (u_quad_t *)llp);
}

bool_t xdr_short (XDR *xdr, short *sp)
{
    long l;
    switch (xdr->x_op) {
    case XDR_ENCODE:
        l = *sp;
        return XDR_SEND_INT32(xdr, (int32_t *)&l);
    case XDR_DECODE:
        if (!XDR_RECV_INT32(xdr, (int32_t *)&l))
            return FALSE;
        *sp = (short)l;
        return TRUE;
    case XDR_FREE:
        return TRUE;
    default:
        break;
    }
    return FALSE;
} /* xdr_short */

/*
 * Non-portable xdr primitives.
 * Care should be taken when moving these routines to new architectures.
 */

/*
 * XDR null terminated ASCII strings
 * xdr_string deals with "C strings" - arrays of bytes that are
 * terminated by a NULL character.  The parameter cpp references a
 * pointer to storage; If the pointer is null, then the necessary
 * storage is allocated.  The last parameter is the max allowed length
 * of the string as specified by a protocol.
 */
bool_t xdr_string (XDR *xdr, char **cpp, u_int maxsize)
{
    u_int size;
    u_int nodesize;

    /*
     * first deal with the length since xdr strings are counted-strings
     */
    switch (xdr->x_op) {
    case XDR_FREE:
        if (*cpp == NULL) return TRUE;
        /* fall through... */
    case XDR_ENCODE:
        if (*cpp == NULL) return FALSE;
        size = strlen(*cpp);
        break;
    case XDR_DECODE:
        break;
    default:
        break;
    }

    if (!xdr_u_int(xdr, &size)) return FALSE;
    if (size > maxsize) return FALSE;
    nodesize = size + 1;

    /*
     * now deal with the actual bytes
     */
    switch (xdr->x_op) {
    case XDR_DECODE:
        if (nodesize == 0) return TRUE;
        if (*cpp == NULL)
            *cpp = (char *)mem_alloc(nodesize);
        if (*cpp == NULL) return FALSE;
        (*cpp)[size] = 0;
        /* fall through... */
    case XDR_ENCODE:
        return xdr_opaque(xdr, *cpp, size);
    case XDR_FREE:
        mem_free(*cpp);
        *cpp = NULL;
        return TRUE;
    default:
        break;
    }
    return FALSE;
} /* xdr_string */
