# Inherit device configuration
$(call inherit-product, $(LOCAL_PATH)/ancora.mk)

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)
$(call inherit-product, vendor/cm/config/gsm.mk)

# Setup device configuration
PRODUCT_NAME := cm_ancora
PRODUCT_RELEASE_NAME := ancora
PRODUCT_DEVICE := ancora
PRODUCT_BRAND := samsung
PRODUCT_MODEL := GT-I8150
PRODUCT_MANUFACTURER := samsung
PRODUCT_BUILD_PROP_OVERRIDES += BUILD_FINGERPRINT=samsung/GT-I8150/GT-I8150:2.3.6/GINGERBREAD/XXKL3:user/release-keys PRIVATE_BUILD_DESC="GT-I8150-user 2.3.6 GINGERBREAD XXKL3 release-keys"

# Release name and versioning
PRODUCT_VERSION_DEVICE_SPECIFIC :=
