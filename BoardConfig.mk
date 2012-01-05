# Copyright (C) 2012 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This file sets variables that control the way modules are built
# thorughout the system. It should not be used to conditionally
# disable makefiles (the proper mechanism to control what gets
# included in a build is to use PRODUCT_PACKAGES in a product
# definition file).
#

# WARNING: This line must come *before* including the proprietary
# variant, so that it gets overwritten by the parent (which goes
# against the traditional rules of inheritance).
USE_CAMERA_STUB := true

# inherit from the proprietary version
-include vendor/samsung/ancora/BoardConfigVendor.mk

TARGET_SPECIFIC_HEADER_PATH := device/samsung/ancora/include

TARGET_BOARD_PLATFORM := msm7x30
TARGET_BOARD_PLATFORM_GPU := qcom-adreno200

TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
ARCH_ARM_HAVE_TLS_REGISTER := true

TARGET_GLOBAL_CFLAGS += -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
TARGET_GLOBAL_CPPFLAGS += -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp

COMMON_GLOBAL_CFLAGS += -DREFRESH_RATE=60 -DMISSING_EGL_EXTERNAL_IMAGE -DMISSING_EGL_PIXEL_FORMAT_YV12 -DMISSING_GRALLOC_BUFFERS

TARGET_BOOTLOADER_BOARD_NAME := ancora
TARGET_OTA_ASSERT_DEVICE := ancora,GT-I8150

TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

# Wifi related defines
BOARD_WPA_SUPPLICANT_DRIVER      := WEXT
WPA_SUPPLICANT_VERSION           := VER_0_8_X
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_bcmdhd
BOARD_WLAN_DEVICE                := bcmdhd
BOARD_WLAN_DEVICE_REV            := bcm4329
WIFI_DRIVER_MODULE_PATH          := "/lib/modules/dhd.ko"
WIFI_DRIVER_FW_PATH_STA          := "/system/vendor/firmware/bcm4329_sta.bin"
WIFI_DRIVER_FW_PATH_AP           := "/system/vendor/firmware/bcm4329_aps.bin"
WIFI_DRIVER_MODULE_ARG           := "firmware_path=/system/vendor/firmware/bcm4329_sta.bin nvram_path=/system/etc/wifi/nvram_net.txt"
WIFI_DRIVER_MODULE_NAME          := "dhd"

BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true

BOARD_USES_LIBSECRIL_STUB := true
BOARD_PREBUILT_LIBAUDIO := false

BOARD_KERNEL_CMDLINE := console=null androidboot.hardware=ancora
BOARD_KERNEL_BASE := 0x00400000
BOARD_KERNEL_PAGESIZE := 4096

# FM Radio
BOARD_HAVE_FM_RADIO := true
BOARD_GLOBAL_CFLAGS += -DHAVE_FM_RADIO
BOARD_FM_DEVICE := si4709

BOARD_EGL_CFG := device/samsung/ancora/egl.cfg

BOARD_VENDOR_QCOM_AMSS_VERSION := 6225
BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := ancora
BOARD_VENDOR_QCOM_GPS_LOC_API_AMSS_VERSION := 50000

USE_OPENGL_RENDERER := true

TARGET_USES_C2D_COMPOSITION := true
TARGET_USES_OVERLAY := true
TARGET_USES_SF_BYPASS := false
TARGET_HAVE_BYPASS := false
TARGET_USES_GENLOCK := true
TARGET_FORCE_CPU_UPLOAD := true

BOARD_USES_QCOM_HARDWARE := true
BOARD_USES_QCOM_LIBS := true
BOARD_USES_QCOM_LIBRPC := true
BOARD_USES_QCOM_GPS := true

BOARD_USE_LEGACY_TOUCHSCREEN := true

TARGET_USERIMAGES_USE_EXT4 := true

BOARD_BOOTIMAGE_PARTITION_SIZE := 5767168
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 7864320
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 560988160
BOARD_USERDATAIMAGE_PARTITION_SIZE := 1163919360
BOARD_CACHEIMAGE_PARTITION_SIZE := 62914560
BOARD_FLASH_BLOCK_SIZE := 4096

BOARD_USES_MMCUTILS := true
BOARD_HAS_NO_MISC_PARTITION := true
BOARD_VOLD_EMMC_SHARES_DEV_MAJOR := true
BOARD_VOLD_MAX_PARTITIONS := 29

TARGET_PREBUILT_KERNEL := device/samsung/ancora/prebuilt/kernel

BOARD_CUSTOM_RECOVERY_KEYMAPPING := ../../device/samsung/ancora/recovery/recovery_keys.c
BOARD_CUSTOM_GRAPHICS := ../../../device/samsung/ancora/recovery/graphics.c
TARGET_RECOVERY_INITRC := device/samsung/ancora/init.recovery.rc
BOARD_UMS_LUNFILE := "/sys/devices/platform/usb_mass_storage/lun0/file"
