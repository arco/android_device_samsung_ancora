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

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay

# Boot screen
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/ARIESVE.rle:root/ARIESVE.rle \
    $(LOCAL_PATH)/charging.rle:root/charging.rle \
    $(LOCAL_PATH)/ARIESVE.rle:recovery/ARIESVE.rle \
    $(LOCAL_PATH)/charging.rle:recovery/charging.rle

# Support files
PRODUCT_COPY_FILES += \
    frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/base/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/base/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/base/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/base/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/base/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml \
    frameworks/base/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/base/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/base/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml

# Media configuration xml file
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/media_profiles.xml:system/etc/media_profiles.xml

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/init.qcom.rc:root/init.qcom.rc \
    $(LOCAL_PATH)/init.qcom.usb.rc:root/init.qcom.usb.rc \
    $(LOCAL_PATH)/init.qcom.sh:root/init.qcom.sh \
    $(LOCAL_PATH)/lpm.rc:root/lpm.rc \
    $(LOCAL_PATH)/lpm.rc:recovery/root/lpm.rc \
    $(LOCAL_PATH)/ueventd.qcom.rc:root/ueventd.qcom.rc \
    $(LOCAL_PATH)/etc/vold.fstab:system/etc/vold.fstab \
    $(LOCAL_PATH)/etc/wifi/nvram_net.txt:system/etc/wifi/nvram_net.txt \
    $(LOCAL_PATH)/recovery/fix_reboot.sh:recovery/root/sbin/fix_reboot.sh \
    $(LOCAL_PATH)/prebuilt/FileManager.apk:system/app/FileManager.apk \
    $(LOCAL_PATH)/prebuilt/bootanimation.zip:system/media/bootanimation.zip

# Input device calibration files
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/sec_touchscreen.idc:system/usr/idc/sec_touchscreen.idc

# Bluetooth configuration files
PRODUCT_COPY_FILES += \
    system/bluetooth/data/main.conf:system/etc/bluetooth/main.conf

# Keychars and keylayout files
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/keyfiles/Broadcom_Bluetooth_HID.kcm.bin:system/usr/keychars/Broadcom_Bluetooth_HID.kcm.bin \
    $(LOCAL_PATH)/keyfiles/ancora_keypad_numeric.kcm.bin:system/usr/keychars/ancora_keypad_numeric.kcm.bin \
    $(LOCAL_PATH)/keyfiles/ancora_keypad_qwerty.kcm.bin:system/usr/keychars/ancora_keypad_qwerty.kcm.bin \
    $(LOCAL_PATH)/keyfiles/ancora_handset.kl:system/usr/keylayout/ancora_handset.kl \
    $(LOCAL_PATH)/keyfiles/sec_jack.kl:system/usr/keylayout/sec_jack.kl \
    $(LOCAL_PATH)/keyfiles/sec_key.kl:system/usr/keylayout/sec_key.kl \
    $(LOCAL_PATH)/keyfiles/sec_power_key.kl:system/usr/keylayout/sec_power_key.kl

# Modules
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/prebuilt/bthid.ko:root/lib/modules/bthid.ko \
    $(LOCAL_PATH)/prebuilt/dhd.ko:root/lib/modules/dhd.ko

# Audio profiles
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/etc/audio/codec/FMRadioEar.ini:system/etc/audio/codec/FMRadioEar.ini \
    $(LOCAL_PATH)/etc/audio/codec/FMRadioSpk.ini:system/etc/audio/codec/FMRadioSpk.ini \
    $(LOCAL_PATH)/etc/audio/codec/MusicEar.ini:system/etc/audio/codec/MusicEar.ini \
    $(LOCAL_PATH)/etc/audio/codec/MusicSpk.ini:system/etc/audio/codec/MusicSpk.ini \
    $(LOCAL_PATH)/etc/audio/codec/RecHeadSetMic.ini:system/etc/audio/codec/RecHeadSetMic.ini \
    $(LOCAL_PATH)/etc/audio/codec/RecMainMic.ini:system/etc/audio/codec/RecMainMic.ini \
    $(LOCAL_PATH)/etc/audio/codec/RecSubMic.ini:system/etc/audio/codec/RecSubMic.ini \
    $(LOCAL_PATH)/etc/audio/codec/RingtoneEar.ini:system/etc/audio/codec/RingtoneEar.ini \
    $(LOCAL_PATH)/etc/audio/codec/RingtoneSpk.ini:system/etc/audio/codec/RingtoneSpk.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoiceCall3pEar.ini:system/etc/audio/codec/VoiceCall3pEar.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoiceCall4pEar.ini:system/etc/audio/codec/VoiceCall4pEar.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoiceCallBT.ini:system/etc/audio/codec/VoiceCallBT.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoiceCallRcv.ini:system/etc/audio/codec/VoiceCallRcv.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoiceCallSpk.ini:system/etc/audio/codec/VoiceCallSpk.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoiceRecHeadSetMic.ini:system/etc/audio/codec/VoiceRecHeadSetMic.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoiceRecMainMic.ini:system/etc/audio/codec/VoiceRecMainMic.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoipCall3pEar.ini:system/etc/audio/codec/VoipCall3pEar.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoipCall4pEar.ini:system/etc/audio/codec/VoipCall4pEar.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoipCallRcv.ini:system/etc/audio/codec/VoipCallRcv.ini \
    $(LOCAL_PATH)/etc/audio/codec/VoipCallSpk.ini:system/etc/audio/codec/VoipCallSpk.ini \
    $(LOCAL_PATH)/etc/audio/codec/VtCall3pEar.ini:system/etc/audio/codec/VtCall3pEar.ini \
    $(LOCAL_PATH)/etc/audio/codec/VtCall4pEar.ini:system/etc/audio/codec/VtCall4pEar.ini \
    $(LOCAL_PATH)/etc/audio/codec/VtCallBT.ini:system/etc/audio/codec/VtCallBT.ini \
    $(LOCAL_PATH)/etc/audio/codec/VtCallRcv.ini:system/etc/audio/codec/VtCallRcv.ini \
    $(LOCAL_PATH)/etc/audio/codec/VtCallSpk.ini:system/etc/audio/codec/VtCallSpk.ini

# LPM
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/lpm/bin/charging_mode:system/bin/charging_mode \
    $(LOCAL_PATH)/lpm/bin/immvibed:system/bin/immvibed \
    $(LOCAL_PATH)/lpm/bin/playlpm:system/bin/playlpm \
    $(LOCAL_PATH)/lpm/lib/libQmageDecoder.so:system/lib/libQmageDecoder.so \
    $(LOCAL_PATH)/lpm/media/Disconnected.qmg:system/media/Disconnected.qmg \
    $(LOCAL_PATH)/lpm/media/battery_batteryerror.qmg:system/media/battery_batteryerror.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_5.qmg:system/media/battery_charging_5.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_10.qmg:system/media/battery_charging_10.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_15.qmg:system/media/battery_charging_15.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_20.qmg:system/media/battery_charging_20.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_25.qmg:system/media/battery_charging_25.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_30.qmg:system/media/battery_charging_30.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_35.qmg:system/media/battery_charging_35.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_40.qmg:system/media/battery_charging_40.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_45.qmg:system/media/battery_charging_45.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_50.qmg:system/media/battery_charging_50.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_55.qmg:system/media/battery_charging_55.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_60.qmg:system/media/battery_charging_60.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_65.qmg:system/media/battery_charging_65.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_70.qmg:system/media/battery_charging_70.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_75.qmg:system/media/battery_charging_75.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_80.qmg:system/media/battery_charging_80.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_85.qmg:system/media/battery_charging_85.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_90.qmg:system/media/battery_charging_90.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_95.qmg:system/media/battery_charging_95.qmg \
    $(LOCAL_PATH)/lpm/media/battery_charging_100.qmg:system/media/battery_charging_100.qmg \
    $(LOCAL_PATH)/lpm/media/battery_error.qmg:system/media/battery_error.qmg \
    $(LOCAL_PATH)/lpm/media/chargingwarning.qmg:system/media/chargingwarning.qmg

PRODUCT_PACKAGES += \
    librs_jni \
    copybit.msm7x30 \
    gralloc.msm7x30 \
    hwcomposer.msm7x30 \
    audio.primary.msm7x30 \
    audio_policy.msm7x30 \
    gps.ancora \
    audio.a2dp.default \
    libaudioutils \
    libgenlock \
    libmemalloc \
    liboverlay \
    libtilerenderer \
    libQcomUI \
    libOmxCore \
    libOmxVenc \
    libOmxVdec \
    libstagefrighthw \
    com.android.future.usb.accessory

PRODUCT_PACKAGES += \
    make_ext4fs \
    setup_fs

# For userdebug builds
ADDITIONAL_DEFAULT_PROPERTIES += \
    ro.secure=0 \
    ro.allow.mock.location=1 \
    ro.debuggable=1

# We have enough storage space to hold precise GC data
PRODUCT_TAGS += dalvik.gc.type-precise

$(call inherit-product, frameworks/base/build/phone-hdpi-dalvik-heap.mk)

PRODUCT_LOCALES += hdpi
