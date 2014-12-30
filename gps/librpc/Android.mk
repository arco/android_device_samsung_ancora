LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= xdr.c rpc.c svc.c clnt.c ops.c svc_clnt_common.c

LOCAL_C_INCLUDES:=$(LOCAL_PATH)

LOCAL_CFLAGS:= -fno-short-enums 

LOCAL_CFLAGS+=-DRPC_OFFSET=0
#LOCAL_CFLAGS+=-DDEBUG -DVERBOSE

LOCAL_COPY_HEADERS_TO:= librpc/rpc
LOCAL_COPY_HEADERS:= \
	rpc/clnt.h \
	rpc/pmap_clnt.h \
	rpc/rpc.h \
	rpc/rpc_router_ioctl.h \
	rpc/svc.h \
	rpc/types.h \
	rpc/xdr.h

LOCAL_MODULE:= librpc

LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := librpc
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_STATIC_LIBRARIES := libpower
LOCAL_WHOLE_STATIC_LIBRARIES := librpc

# LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)
