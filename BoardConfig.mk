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

# inherit from the proprietary version
-include vendor/samsung/ancora/BoardConfigVendor.mk

TARGET_BOARD_PLATFORM := msm7x30
TARGET_BOARD_PLATFORM_GPU := qcom-adreno200

# CPU ARCHİTECTURE stuff
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := scorpion
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_USE_QCOM_BIONIC_OPTIMIZATION := true

TARGET_BOOTLOADER_BOARD_NAME := ancora
TARGET_OTA_ASSERT_DEVICE := ancora,GT-I8150

TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

TARGET_NO_INITLOGO := true

TARGET_SCREEN_HEIGHT := 800
TARGET_SCREEN_WIDTH := 480

# Wifi related defines
WIFI_BAND                        := 802_11_ABG
WPA_SUPPLICANT_VERSION           := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER      := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_bcmdhd
BOARD_HOSTAPD_DRIVER             := NL80211
BOARD_HOSTAPD_PRIVATE_LIB        := lib_driver_cmd_bcmdhd
BOARD_WLAN_DEVICE                := bcmdhd
BOARD_HAVE_SAMSUNG_WIFI          := true

WIFI_DRIVER_MODULE_PATH          := "/system/lib/modules/dhd.ko"
WIFI_DRIVER_MODULE_NAME          := "dhd"
WIFI_DRIVER_MODULE_ARG           := "firmware_path=/vendor/firmware/fw_bcmdhd.bin nvram_path=/vendor/firmware/nvram_net.txt"
WIFI_DRIVER_MODULE_AP_ARG        := "firmware_path=/vendor/firmware/fw_bcmdhd_apsta.bin nvram_path=/vendor/firmware/nvram_net.txt"
WIFI_DRIVER_FW_PATH_PARAM        := "/sys/module/dhd/parameters/firmware_path"
WIFI_DRIVER_FW_PATH_STA          := "/vendor/firmware/fw_bcmdhd.bin"
WIFI_DRIVER_FW_PATH_AP           := "/vendor/firmware/fw_bcmdhd_apsta.bin"

BOARD_KERNEL_BASE := 0x00400000
BOARD_KERNEL_PAGESIZE := 4096

# Override healthd HAL
BOARD_HAL_STATIC_LIBRARIES := libhealthd.qcom

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/samsung/ancora/bluetooth
BOARD_BLUEDROID_VENDOR_CONF := device/samsung/ancora/bluetooth/vnd_ancora.txt

# RIL stuff
BOARD_MOBILEDATA_INTERFACE_NAME = "pdp0"
BOARD_RIL_CLASS := ../../../device/samsung/ancora/ril/
BOARD_USES_LEGACY_RIL := true
BOARD_USES_LIBSECRIL_STUB := true

# Audio stuff
TARGET_QCOM_AUDIO_VARIANT := caf
BOARD_USES_LEGACY_ALSA_AUDIO := true
BOARD_HAVE_SAMSUNG_AUDIO := true
BOARD_USES_QCOM_AUDIO_LPA := true
BOARD_USES_QCOM_AUDIO_RESETALL := true
BOARD_USES_QCOM_AUDIO_VOIPMUTE := true
COMMON_GLOBAL_CFLAGS += -DWITH_QCOM_VOIP_OVER_MVS

# QCOM stuff
TARGET_QCOM_DISPLAY_VARIANT := caf
TARGET_QCOM_MEDIA_VARIANT := caf
TARGET_USES_ION := true
TARGET_USES_C2D_COMPOSITION := true
USE_OPENGL_RENDERER := true
TARGET_ENABLE_QC_AV_ENHANCEMENTS := true
TARGET_USES_QCOM_BSP := true
BOARD_USES_QCOM_HARDWARE := true
COMMON_GLOBAL_CFLAGS += -DQCOM_HARDWARE
COMMON_GLOBAL_CFLAGS += -DQCOM_BSP

BOARD_EGL_CFG := device/samsung/ancora/config/egl.cfg

# Use insecure heap
TARGET_DISPLAY_INSECURE_MM_HEAP := true

BOARD_NEEDS_MEMORYHEAPPMEM := true

# Camera stuff
USE_CAMERA_STUB := true
BOARD_USES_LEGACY_OVERLAY := true
TARGET_DISABLE_ARM_PIE := true
USE_DEVICE_SPECIFIC_CAMERA := true
COMMON_GLOBAL_CFLAGS += -DBINDER_COMPAT
COMMON_GLOBAL_CFLAGS += -DSAMSUNG_CAMERA_LEGACY
COMMON_GLOBAL_CFLAGS += -DNEEDS_VECTORIMPL_SYMBOLS

# Lights
TARGET_PROVIDES_LIBLIGHT := true

# PowerHAL
TARGET_PROVIDES_POWERHAL := true

# GPS stuff
BOARD_USES_QCOM_GPS := true
BOARD_VENDOR_QCOM_AMSS_VERSION := 6225
BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE := msm7x30
BOARD_VENDOR_QCOM_GPS_LOC_API_AMSS_VERSION := 50000

TARGET_USERIMAGES_USE_EXT4 := true

BOARD_VOLD_EMMC_SHARES_DEV_MAJOR := true
BOARD_VOLD_MAX_PARTITIONS := 28

# Begin recovery stuff
#
# Partition sizes must match your phone, or all hell will break loose!
# For the Galaxy W, these are calculated from /proc/partitions
BOARD_BOOTIMAGE_PARTITION_SIZE := 5767168
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 7864320
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 560988160
BOARD_USERDATAIMAGE_PARTITION_SIZE := 1163919360
BOARD_CACHEIMAGE_PARTITION_SIZE := 62914560
BOARD_FLASH_BLOCK_SIZE := 4096

BOARD_USES_MMCUTILS := true
BOARD_HAS_NO_MISC_PARTITION := true
BOARD_HAS_SDCARD_INTERNAL := true
TARGET_RECOVERY_PIXEL_FORMAT := "RGBX_8888"
BOARD_CUSTOM_RECOVERY_KEYMAPPING := ../../device/samsung/ancora/recovery/recovery_keys.c
TARGET_RECOVERY_INITRC := device/samsung/ancora/config/init.recovery.rc
TARGET_RECOVERY_FSTAB := device/samsung/ancora/config/fstab.qcom
COMMON_GLOBAL_CFLAGS += -DNO_SECURE_DISCARD
# End recovery stuff

TARGET_KERNEL_SOURCE := kernel/samsung/msm7x30-common
TARGET_KERNEL_CONFIG := ancora_defconfig

TARGET_USE_CUSTOM_LUN_FILE_PATH := /sys/devices/platform/msm_hsusb/gadget/lun%d/file

# SELinux
BOARD_SEPOLICY_DIRS += \
    device/samsung/ancora/sepolicy

BOARD_SEPOLICY_UNION += \
    file_contexts \
    property_contexts \
    bridge.te \
    camera.te \
    device.te \
    dhcp.te \
    domain.te \
    file.te \
    healthd.te \
    init.te \
    mac_update.te \
    mediaserver.te \
    rild.te \
    rmt.te \
    surfaceflinger.te \
    system.te \
    tee.te \
    ueventd.te \
    wpa_supplicant.te
