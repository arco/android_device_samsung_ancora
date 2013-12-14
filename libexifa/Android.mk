LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE:=false

LOCAL_C_INCLUDES += $(LOCAL_PATH)\
	$(LOCAL_PATH)/libexif/\
	$(LOCAL_PATH)/libexif/canon/\
	$(LOCAL_PATH)/libexif/fuji/\
	$(LOCAL_PATH)/libexif/olympus/\
	$(LOCAL_PATH)/libexif/pentax/

LOCAL_SRC_FILES:=\
	canon/exif-mnote-data-canon.c\
	canon/mnote-canon-tag.c\
	canon/mnote-canon-entry.c\
	pentax/mnote-pentax-tag.c\
	pentax/mnote-pentax-entry.c\
	pentax/exif-mnote-data-pentax.c\
	exif-loader.c\
	exif-byte-order.c\
	exif-content.c\
	exif-log.c\
	exif-mem.c\
	olympus/mnote-olympus-tag.c\
	olympus/mnote-olympus-entry.c\
	olympus/exif-mnote-data-olympus.c\
	exif-format.c\
	exif-utils.c\
	exif-entry.c\
	exif-ifd.c\
	exif-data.c\
	exif-tag.c\
	fuji/mnote-fuji-tag.c\
	fuji/mnote-fuji-entry.c\
	fuji/exif-mnote-data-fuji.c\
	exif-mnote-data.c

#LOCAL_CFLAGS:=-O2 -g
#LOCAL_CFLAGS+=-DHAVE_CONFIG_H -D_U_="__attribute__((unused))" -Dlinux -D__GLIBC__ -D_GNU_SOURCE

LOCAL_MODULE:= libexifa

include $(BUILD_SHARED_LIBRARY)
