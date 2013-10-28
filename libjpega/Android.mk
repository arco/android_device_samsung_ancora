LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE:=false

LOCAL_C_INCLUDES += $(LOCAL_PATH)\
	$(LOCAL_PATH)/libjpeg/\
	$(LOCAL_PATH)/libexif/\
	$(LOCAL_PATH)/libexif/canon/\
	$(LOCAL_PATH)/libexif/fuji/\
	$(LOCAL_PATH)/libexif/olympus/\
	$(LOCAL_PATH)/libexif/pentax/

LOCAL_SHARED_LIBRARIES:= \
	libexifa

LOCAL_SRC_FILES:=\
	jpeg-data.c\
	jpeg-marker.c\
	exif-i18n

#LOCAL_CFLAGS:=-O2 -g
#LOCAL_CFLAGS+=-DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -Dlinux -D__GLIBC__ -D_GNU_SOURCE

LOCAL_MODULE:= libjpega

include $(BUILD_SHARED_LIBRARY)
