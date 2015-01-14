LOCAL_PATH := $(call my-dir)

GPS_DIR_LIST += $(LOCAL_PATH)/libloc_api-rpc/
GPS_DIR_LIST += $(LOCAL_PATH)/libloc_api/

#call the subfolders
include $(addsuffix Android.mk, $(GPS_DIR_LIST))
