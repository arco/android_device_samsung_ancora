/*
 * Copyright (C) 2014 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdlib.h>

#define CAMERA_PARAMETERS_EXTRA_C \
const char CameraParameters::KEY_PREVIEW_SIZE[] = "preview-size"; \
const char CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES[] = "preview-size-values"; \
const char CameraParameters::KEY_SUPPORTED_HFR_SIZES[] = "hfr-size-values"; \
const char CameraParameters::KEY_PREVIEW_FORMAT[] = "preview-format"; \
const char CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS[] = "preview-format-values"; \
const char CameraParameters::KEY_PREVIEW_FRAME_RATE[] = "preview-frame-rate"; \
const char CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES[] = "preview-frame-rate-values"; \
const char CameraParameters::KEY_PREVIEW_FPS_RANGE[] = "preview-fps-range"; \
const char CameraParameters::KEY_SUPPORTED_PREVIEW_FPS_RANGE[] = "preview-fps-range-values"; \
const char CameraParameters::KEY_PREVIEW_FRAME_RATE_MODE[] = "preview-frame-rate-mode"; \
const char CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATE_MODES[] = "preview-frame-rate-modes"; \
const char CameraParameters::KEY_PREVIEW_FRAME_RATE_AUTO_MODE[] = "frame-rate-auto"; \
const char CameraParameters::KEY_PREVIEW_FRAME_RATE_FIXED_MODE[] = "frame-rate-fixed"; \
const char CameraParameters::KEY_PICTURE_SIZE[] = "picture-size"; \
const char CameraParameters::KEY_SUPPORTED_PICTURE_SIZES[] = "picture-size-values"; \
const char CameraParameters::KEY_PICTURE_FORMAT[] = "picture-format"; \
const char CameraParameters::KEY_SUPPORTED_PICTURE_FORMATS[] = "picture-format-values"; \
const char CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH[] = "jpeg-thumbnail-width"; \
const char CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT[] = "jpeg-thumbnail-height"; \
const char CameraParameters::KEY_SUPPORTED_JPEG_THUMBNAIL_SIZES[] = "jpeg-thumbnail-size-values"; \
const char CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY[] = "jpeg-thumbnail-quality"; \
const char CameraParameters::KEY_JPEG_QUALITY[] = "jpeg-quality"; \
const char CameraParameters::KEY_ROTATION[] = "rotation"; \
const char CameraParameters::KEY_GPS_LATITUDE[] = "gps-latitude"; \
const char CameraParameters::KEY_GPS_LONGITUDE[] = "gps-longitude"; \
const char CameraParameters::KEY_GPS_ALTITUDE[] = "gps-altitude"; \
const char CameraParameters::KEY_GPS_TIMESTAMP[] = "gps-timestamp"; \
const char CameraParameters::KEY_GPS_PROCESSING_METHOD[] = "gps-processing-method"; \
const char CameraParameters::KEY_WHITE_BALANCE[] = "whitebalance"; \
const char CameraParameters::KEY_SUPPORTED_WHITE_BALANCE[] = "whitebalance-values"; \
const char CameraParameters::KEY_EFFECT[] = "effect"; \
const char CameraParameters::KEY_SUPPORTED_EFFECTS[] = "effect-values"; \
const char CameraParameters::KEY_TOUCH_AF_AEC[] = "touch-af-aec"; \
const char CameraParameters::KEY_SUPPORTED_TOUCH_AF_AEC[] = "touch-af-aec-values"; \
const char CameraParameters::KEY_TOUCH_INDEX_AEC[] = "touch-index-aec"; \
const char CameraParameters::KEY_TOUCH_INDEX_AF[] = "touch-index-af"; \
const char CameraParameters::KEY_ANTIBANDING[] = "antibanding"; \
const char CameraParameters::KEY_SUPPORTED_ANTIBANDING[] = "antibanding-values"; \
const char CameraParameters::KEY_SCENE_MODE[] = "scene-mode"; \
const char CameraParameters::KEY_SUPPORTED_SCENE_MODES[] = "scene-mode-values"; \
const char CameraParameters::KEY_SCENE_DETECT[] = "scene-detect"; \
const char CameraParameters::KEY_SUPPORTED_SCENE_DETECT[] = "scene-detect-values"; \
const char CameraParameters::KEY_FLASH_MODE[] = "flash-mode"; \
const char CameraParameters::KEY_SUPPORTED_FLASH_MODES[] = "flash-mode-values"; \
const char CameraParameters::KEY_FOCUS_MODE[] = "focus-mode"; \
const char CameraParameters::KEY_SUPPORTED_FOCUS_MODES[] = "focus-mode-values"; \
const char CameraParameters::KEY_MAX_NUM_FOCUS_AREAS[] = "max-num-focus-areas"; \
const char CameraParameters::KEY_FOCUS_AREAS[] = "focus-areas"; \
const char CameraParameters::KEY_FOCAL_LENGTH[] = "focal-length"; \
const char CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE[] = "horizontal-view-angle"; \
const char CameraParameters::KEY_VERTICAL_VIEW_ANGLE[] = "vertical-view-angle"; \
const char CameraParameters::KEY_EXPOSURE_COMPENSATION[] = "exposure-compensation"; \
const char CameraParameters::KEY_MAX_EXPOSURE_COMPENSATION[] = "max-exposure-compensation"; \
const char CameraParameters::KEY_MIN_EXPOSURE_COMPENSATION[] = "min-exposure-compensation"; \
const char CameraParameters::KEY_EXPOSURE_COMPENSATION_STEP[] = "exposure-compensation-step"; \
const char CameraParameters::KEY_AUTO_EXPOSURE_LOCK[] = "auto-exposure-lock"; \
const char CameraParameters::KEY_AUTO_EXPOSURE_LOCK_SUPPORTED[] = "auto-exposure-lock-supported"; \
const char CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK[] = "auto-whitebalance-lock"; \
const char CameraParameters::KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED[] = "auto-whitebalance-lock-supported"; \
const char CameraParameters::KEY_MAX_NUM_METERING_AREAS[] = "max-num-metering-areas"; \
const char CameraParameters::KEY_METERING_AREAS[] = "metering-areas"; \
const char CameraParameters::KEY_ZOOM[] = "zoom"; \
const char CameraParameters::KEY_MAX_ZOOM[] = "max-zoom"; \
const char CameraParameters::KEY_ZOOM_RATIOS[] = "zoom-ratios"; \
const char CameraParameters::KEY_ZOOM_SUPPORTED[] = "zoom-supported"; \
const char CameraParameters::KEY_SMOOTH_ZOOM_SUPPORTED[] = "smooth-zoom-supported"; \
const char CameraParameters::KEY_FOCUS_DISTANCES[] = "focus-distances"; \
const char CameraParameters::KEY_VIDEO_FRAME_FORMAT[] = "video-frame-format"; \
\
const char CameraParameters::KEY_ISO_MODE[] = "iso"; \
const char CameraParameters::KEY_SUPPORTED_ISO_MODES[] = "iso-values"; \
const char CameraParameters::KEY_LENSSHADE[] = "lensshade"; \
const char CameraParameters::KEY_SUPPORTED_LENSSHADE_MODES[] = "lensshade-values"; \
const char CameraParameters::KEY_AUTO_EXPOSURE[] = "metering"; \
const char CameraParameters::KEY_SUPPORTED_AUTO_EXPOSURE[] = "auto-exposure-values"; \
const char CameraParameters::KEY_DENOISE[] = "denoise"; \
const char CameraParameters::KEY_SUPPORTED_DENOISE[] = "denoise-values"; \
const char CameraParameters::KEY_SELECTABLE_ZONE_AF[] = "selectable-zone-af"; \
const char CameraParameters::KEY_SUPPORTED_SELECTABLE_ZONE_AF[] = "selectable-zone-af-values"; \
const char CameraParameters::KEY_FACE_DETECTION[] = "face-detection"; \
const char CameraParameters::KEY_SUPPORTED_FACE_DETECTION[] = "face-detection-values"; \
const char CameraParameters::KEY_MEMORY_COLOR_ENHANCEMENT[] = "mce"; \
const char CameraParameters::KEY_SUPPORTED_MEM_COLOR_ENHANCE_MODES[] = "mce-values"; \
const char CameraParameters::KEY_VIDEO_HIGH_FRAME_RATE[] = "video-hfr"; \
const char CameraParameters::KEY_SUPPORTED_VIDEO_HIGH_FRAME_RATE_MODES[] = "video-hfr-values"; \
const char CameraParameters::KEY_REDEYE_REDUCTION[] = "redeye-reduction"; \
const char CameraParameters::KEY_SUPPORTED_REDEYE_REDUCTION[] = "redeye-reduction-values"; \
const char CameraParameters::KEY_HIGH_DYNAMIC_RANGE_IMAGING[] = "hdr"; \
const char CameraParameters::KEY_SUPPORTED_HDR_IMAGING_MODES[] = "hdr-values"; \
const char CameraParameters::KEY_VIDEO_SIZE[] = "video-size"; \
const char CameraParameters::KEY_SUPPORTED_VIDEO_SIZES[] = "video-size-values"; \
const char CameraParameters::KEY_PREFERRED_PREVIEW_SIZE_FOR_VIDEO[] = "preferred-preview-size-for-video"; \
const char CameraParameters::KEY_MAX_NUM_DETECTED_FACES_HW[] = "max-num-detected-faces-hw"; \
const char CameraParameters::KEY_MAX_NUM_DETECTED_FACES_SW[] = "max-num-detected-faces-sw"; \
const char CameraParameters::KEY_RECORDING_HINT[] = "recording-hint"; \
const char CameraParameters::KEY_VIDEO_SNAPSHOT_SUPPORTED[] = "video-snapshot-supported"; \
const char CameraParameters::KEY_FULL_VIDEO_SNAP_SUPPORTED[] = "full-video-snap-supported"; \
const char CameraParameters::KEY_VIDEO_STABILIZATION[] = "video-stabilization"; \
const char CameraParameters::KEY_VIDEO_STABILIZATION_SUPPORTED[] = "video-stabilization-supported"; \
const char CameraParameters::KEY_LIGHTFX[] = "light-fx"; \
const char CameraParameters::KEY_ZSL[] = "zsl"; \
const char CameraParameters::KEY_SUPPORTED_ZSL_MODES[] = "zsl-values"; \
const char CameraParameters::KEY_CAMERA_MODE[] = "camera-mode"; \
const char CameraParameters::KEY_POWER_MODE[] = "power-mode"; \
const char CameraParameters::KEY_POWER_MODE_SUPPORTED[] = "power-mode-supported"; \
const char CameraParameters::KEY_AE_BRACKET_HDR[] = "ae-bracket-hdr"; \
const char CameraParameters::FOCUS_DISTANCE_INFINITY[] = "Infinity"; \
const char CameraParameters::WHITE_BALANCE_AUTO[] = "auto"; \
const char CameraParameters::WHITE_BALANCE_INCANDESCENT[] = "incandescent"; \
const char CameraParameters::WHITE_BALANCE_FLUORESCENT[] = "fluorescent"; \
const char CameraParameters::WHITE_BALANCE_WARM_FLUORESCENT[] = "warm-fluorescent"; \
const char CameraParameters::WHITE_BALANCE_DAYLIGHT[] = "daylight"; \
const char CameraParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT[] = "cloudy-daylight"; \
const char CameraParameters::WHITE_BALANCE_TWILIGHT[] = "twilight"; \
const char CameraParameters::WHITE_BALANCE_SHADE[] = "shade"; \
const char CameraParameters::WHITE_BALANCE_MANUAL_CCT[] = "manual-cct"; \
const char CameraParameters::EFFECT_NONE[] = "none"; \
const char CameraParameters::EFFECT_MONO[] = "mono"; \
const char CameraParameters::EFFECT_NEGATIVE[] = "negative"; \
const char CameraParameters::EFFECT_SOLARIZE[] = "solarize"; \
const char CameraParameters::EFFECT_SEPIA[] = "sepia"; \
const char CameraParameters::EFFECT_POSTERIZE[] = "posterize"; \
const char CameraParameters::EFFECT_WHITEBOARD[] = "whiteboard"; \
const char CameraParameters::EFFECT_BLACKBOARD[] = "blackboard"; \
const char CameraParameters::EFFECT_AQUA[] = "aqua"; \
const char CameraParameters::EFFECT_EMBOSS[] = "emboss"; \
const char CameraParameters::EFFECT_SKETCH[] = "sketch"; \
const char CameraParameters::EFFECT_NEON[] = "neon"; \
const char CameraParameters::TOUCH_AF_AEC_OFF[] = "touch-off"; \
const char CameraParameters::TOUCH_AF_AEC_ON[] = "touch-on"; \
const char CameraParameters::ANTIBANDING_AUTO[] = "auto"; \
const char CameraParameters::ANTIBANDING_50HZ[] = "50hz"; \
const char CameraParameters::ANTIBANDING_60HZ[] = "60hz"; \
const char CameraParameters::ANTIBANDING_OFF[] = "off"; \
const char CameraParameters::FLASH_MODE_OFF[] = "off"; \
const char CameraParameters::FLASH_MODE_AUTO[] = "auto"; \
const char CameraParameters::FLASH_MODE_ON[] = "on"; \
const char CameraParameters::FLASH_MODE_RED_EYE[] = "red-eye"; \
const char CameraParameters::FLASH_MODE_TORCH[] = "torch"; \
const char CameraParameters::SCENE_MODE_AUTO[] = "auto"; \
const char CameraParameters::SCENE_MODE_ASD[] = "asd"; \
const char CameraParameters::SCENE_MODE_ACTION[] = "action"; \
const char CameraParameters::SCENE_MODE_PORTRAIT[] = "portrait"; \
const char CameraParameters::SCENE_MODE_LANDSCAPE[] = "landscape"; \
const char CameraParameters::SCENE_MODE_NIGHT[] = "night"; \
const char CameraParameters::SCENE_MODE_NIGHT_PORTRAIT[] = "night-portrait"; \
const char CameraParameters::SCENE_MODE_THEATRE[] = "theatre"; \
const char CameraParameters::SCENE_MODE_BEACH[] = "beach"; \
const char CameraParameters::SCENE_MODE_SNOW[] = "snow"; \
const char CameraParameters::SCENE_MODE_SUNSET[] = "sunset"; \
const char CameraParameters::SCENE_MODE_STEADYPHOTO[] = "steadyphoto"; \
const char CameraParameters::SCENE_MODE_FIREWORKS[] = "fireworks"; \
const char CameraParameters::SCENE_MODE_SPORTS[] = "sports"; \
const char CameraParameters::SCENE_MODE_PARTY[] = "party"; \
const char CameraParameters::SCENE_MODE_CANDLELIGHT[] = "candlelight"; \
const char CameraParameters::SCENE_MODE_BACKLIGHT[] = "back-light"; \
const char CameraParameters::SCENE_MODE_FLOWERS[] = "flowers"; \
const char CameraParameters::SCENE_MODE_BARCODE[] = "barcode"; \
const char CameraParameters::SCENE_MODE_HDR[] = "hdr"; \
const char CameraParameters::SCENE_MODE_AR[] = "AR"; \
\
const char CameraParameters::SCENE_DETECT_OFF[] = "off"; \
const char CameraParameters::SCENE_DETECT_ON[] = "on"; \
const char CameraParameters::PIXEL_FORMAT_YUV422SP[] = "yuv422sp"; \
const char CameraParameters::PIXEL_FORMAT_YUV420SP[] = "yuv420sp"; \
const char CameraParameters::PIXEL_FORMAT_YUV420SP_ADRENO[] = "yuv420sp-adreno"; \
const char CameraParameters::PIXEL_FORMAT_YUV422I[] = "yuv422i-yuyv"; \
const char CameraParameters::PIXEL_FORMAT_YUV420P[]  = "yuv420p"; \
const char CameraParameters::PIXEL_FORMAT_RGB565[] = "rgb565"; \
const char CameraParameters::PIXEL_FORMAT_RGBA8888[] = "rgba8888"; \
const char CameraParameters::PIXEL_FORMAT_JPEG[] = "jpeg"; \
const char CameraParameters::PIXEL_FORMAT_BAYER_RGGB[] = "bayer-rggb"; \
const char CameraParameters::PIXEL_FORMAT_ANDROID_OPAQUE[] = "android-opaque"; \
const char CameraParameters::PIXEL_FORMAT_RAW[] = "raw"; \
const char CameraParameters::PIXEL_FORMAT_YV12[] = "yuv420p"; \
const char CameraParameters::PIXEL_FORMAT_NV12[] = "nv12"; \
const char CameraParameters::FOCUS_MODE_AUTO[] = "auto"; \
const char CameraParameters::FOCUS_MODE_INFINITY[] = "infinity"; \
const char CameraParameters::FOCUS_MODE_MACRO[] = "macro"; \
const char CameraParameters::FOCUS_MODE_FIXED[] = "fixed"; \
const char CameraParameters::FOCUS_MODE_EDOF[] = "edof"; \
const char CameraParameters::FOCUS_MODE_CONTINUOUS_VIDEO[] = "continuous-video"; \
const char CameraParameters::FOCUS_MODE_CONTINUOUS_PICTURE[] = "continuous-picture"; \
const char CameraParameters::FOCUS_MODE_MANUAL_POSITION[] = "manual"; \
const char CameraParameters::FOCUS_MODE_NORMAL[] = "normal"; \
const char CameraParameters::KEY_SKIN_TONE_ENHANCEMENT[] = "skinToneEnhancement"; \
const char CameraParameters::KEY_SUPPORTED_SKIN_TONE_ENHANCEMENT_MODES[] = "skinToneEnhancement-values"; \
const char CameraParameters::ISO_AUTO[] = "auto"; \
const char CameraParameters::ISO_HJR[] = "ISO_HJR"; \
const char CameraParameters::ISO_100[] = "ISO100"; \
const char CameraParameters::ISO_200[] = "ISO200"; \
const char CameraParameters::ISO_400[] = "ISO400"; \
const char CameraParameters::ISO_800[] = "ISO800"; \
const char CameraParameters::ISO_1600[] = "ISO1600"; \
const char CameraParameters::ISO_3200[] = "ISO3200"; \
const char CameraParameters::ISO_6400[] = "ISO6400"; \
const char CameraParameters::LENSSHADE_ENABLE[] = "enable"; \
const char CameraParameters::LENSSHADE_DISABLE[] = "disable"; \
const char CameraParameters::AUTO_EXPOSURE_FRAME_AVG[] = "frame-average"; \
const char CameraParameters::AUTO_EXPOSURE_CENTER_WEIGHTED[] = "center-weighted"; \
const char CameraParameters::AUTO_EXPOSURE_SPOT_METERING[] = "spot-metering"; \
const char CameraParameters::KEY_GPS_LATITUDE_REF[] = "gps-latitude-ref"; \
const char CameraParameters::KEY_GPS_LONGITUDE_REF[] = "gps-longitude-ref"; \
const char CameraParameters::KEY_GPS_ALTITUDE_REF[] = "gps-altitude-ref"; \
const char CameraParameters::KEY_GPS_STATUS[] = "gps-status"; \
const char CameraParameters::KEY_EXIF_DATETIME[] = "exif-datetime"; \
const char CameraParameters::KEY_HISTOGRAM[] = "histogram"; \
const char CameraParameters::KEY_SUPPORTED_HISTOGRAM_MODES[] = "histogram-values"; \
const char CameraParameters::HISTOGRAM_ENABLE[] = "enable"; \
const char CameraParameters::HISTOGRAM_DISABLE[] = "disable"; \
const char CameraParameters::SKIN_TONE_ENHANCEMENT_ENABLE[] = "enable"; \
const char CameraParameters::SKIN_TONE_ENHANCEMENT_DISABLE[] = "disable"; \
const char CameraParameters::KEY_SHARPNESS[] = "sharpness"; \
const char CameraParameters::KEY_MAX_SHARPNESS[] = "max-sharpness"; \
const char CameraParameters::KEY_CONTRAST[] = "contrast"; \
const char CameraParameters::KEY_MAX_CONTRAST[] = "max-contrast"; \
const char CameraParameters::KEY_SATURATION[] = "saturation"; \
const char CameraParameters::KEY_MAX_SATURATION[] = "max-saturation"; \
const char CameraParameters::DENOISE_OFF[] = "denoise-off"; \
const char CameraParameters::DENOISE_ON[] = "denoise-on"; \
const char CameraParameters::SELECTABLE_ZONE_AF_AUTO[] = "auto"; \
const char CameraParameters::SELECTABLE_ZONE_AF_SPOT_METERING[] = "spot-metering"; \
const char CameraParameters::SELECTABLE_ZONE_AF_CENTER_WEIGHTED[] = "center-weighted"; \
const char CameraParameters::SELECTABLE_ZONE_AF_FRAME_AVERAGE[] = "frame-average"; \
const char CameraParameters::FACE_DETECTION_OFF[] = "off"; \
const char CameraParameters::FACE_DETECTION_ON[] = "on"; \
const char CameraParameters::MCE_ENABLE[] = "enable"; \
const char CameraParameters::MCE_DISABLE[] = "disable"; \
const char CameraParameters::VIDEO_HFR_OFF[] = "off"; \
const char CameraParameters::VIDEO_HFR_2X[] = "60"; \
const char CameraParameters::VIDEO_HFR_3X[] = "90"; \
const char CameraParameters::VIDEO_HFR_4X[] = "120"; \
const char CameraParameters::REDEYE_REDUCTION_ENABLE[] = "enable"; \
const char CameraParameters::REDEYE_REDUCTION_DISABLE[] = "disable"; \
const char CameraParameters::HDR_ENABLE[] = "enable"; \
const char CameraParameters::HDR_DISABLE[] = "disable"; \
const char CameraParameters::ZSL_OFF[] = "off"; \
const char CameraParameters::ZSL_ON[] = "on"; \
const char CameraParameters::AE_BRACKET_HDR_OFF[] = "Off"; \
const char CameraParameters::AE_BRACKET_HDR[] = "HDR"; \
const char CameraParameters::AE_BRACKET[] = "AE-Bracket"; \
const char CameraParameters::LOW_POWER[] = "Low_Power"; \
const char CameraParameters::NORMAL_POWER[] = "Normal_Power"; \
const char CameraParameters::FOCUS_MODE_FACEDETECT[] = "facedetect"; \
const char CameraParameters::FOCUS_MODE_TOUCHAF[] = "touchaf"; \
const char CameraParameters::ISO_50[] = "ISO50"; \
const char CameraParameters::KEY_ANTI_SHAKE_MODE[] = "antishake"; \
const char CameraParameters::KEY_AUTO_CONTRAST[] = "auto-contrast"; \
const char CameraParameters::KEY_BEAUTY_MODE[] = "beauty"; \
const char CameraParameters::KEY_BLUR_MODE[] = "blur"; \
const char CameraParameters::KEY_VINTAGE_MODE[] = "vintagemode"; \
const char CameraParameters::KEY_WDR_MODE[] = "wdr"; \
const char CameraParameters::VINTAGE_MODE_BNW[] = "bnw"; \
const char CameraParameters::VINTAGE_MODE_COOL[] = "cool"; \
const char CameraParameters::VINTAGE_MODE_NORMAL[] = "normal"; \
const char CameraParameters::VINTAGE_MODE_OFF[] = "off"; \
const char CameraParameters::VINTAGE_MODE_WARM[] = "warm"; \
const char CameraParameters::SCENE_MODE_DAWN[] = "dusk-dawn"; \
const char CameraParameters::SCENE_MODE_DUSKDAWN[] = "dusk-dawn"; \
const char CameraParameters::SCENE_MODE_FALL[] = "fall-color"; \
const char CameraParameters::SCENE_MODE_FALL_COLOR[] = "fall-color"; \
const char CameraParameters::SCENE_MODE_TEXT[] = "text"; \
const char CameraParameters::LIGHTFX_LOWLIGHT[] = "low-light"; \
const char CameraParameters::LIGHTFX_HDR[] = "high-dynamic-range"; \
\
int CameraParameters::getInt64(const char *key) const { return -1; }; \
void CameraParameters::setPreviewFrameRateMode(const char *mode) { set(KEY_PREVIEW_FRAME_RATE_MODE, mode); }; \
const char *CameraParameters::getPreviewFrameRateMode() const { return get(KEY_PREVIEW_FRAME_RATE_MODE); }; \
static int parse_pair_extra(const char *str, int *first, int *second, char delim, char **endptr = NULL) { \
    char *end; \
    int w = (int)strtol(str, &end, 10); \
    if (*end != delim) return -1; \
    int h = (int)strtol(end + 1, &end, 10); \
    *first = w; *second = h; \
    if (endptr) *endptr = end; \
    return 0; \
}; \
void CameraParameters::setTouchIndexAec(int x, int y) { \
    char str[32]; \
    snprintf(str, sizeof(str), "%dx%d", x, y); \
    set(KEY_TOUCH_INDEX_AEC, str); \
}; \
void CameraParameters::getTouchIndexAec(int *x, int *y) const { \
    *x = -1; *y = -1; \
    const char *p = get(KEY_TOUCH_INDEX_AEC); \
    if (p == 0) return; \
    int tempX, tempY; \
    if (parse_pair_extra(p, &tempX, &tempY, 'x') == 0) { \
        *x = tempX; *y = tempY; \
    } \
}; \
void CameraParameters::setTouchIndexAf(int x, int y) { \
    char str[32]; \
    snprintf(str, sizeof(str), "%dx%d", x, y); \
    set(KEY_TOUCH_INDEX_AF, str); \
}; \
void CameraParameters::getTouchIndexAf(int *x, int *y) const { \
    *x = -1; *y = -1; \
    const char *p = get(KEY_TOUCH_INDEX_AF); \
    if (p == 0) return; \
    int tempX, tempY; \
    if (parse_pair_extra(p, &tempX, &tempY, 'x') == 0) { \
        *x = tempX; *y = tempY; \
    } \
}; \
void CameraParameters::setPreviewFpsRange(int minFPS, int maxFPS) { \
    char str[32]; \
    snprintf(str, sizeof(str), "%d,%d",minFPS,maxFPS); \
    set(KEY_PREVIEW_FPS_RANGE,str); \
};

#define CAMERA_PARAMETERS_EXTRA_H \
    static const char KEY_PREVIEW_SIZE[]; \
    static const char KEY_SUPPORTED_PREVIEW_SIZES[]; \
    static const char KEY_SUPPORTED_HFR_SIZES[]; \
    static const char KEY_PREVIEW_FPS_RANGE[]; \
    static const char KEY_SUPPORTED_PREVIEW_FPS_RANGE[]; \
    static const char KEY_PREVIEW_FORMAT[]; \
    static const char KEY_SUPPORTED_PREVIEW_FORMATS[]; \
    static const char KEY_PREVIEW_FRAME_RATE[]; \
    static const char KEY_SUPPORTED_PREVIEW_FRAME_RATES[]; \
    static const char KEY_PREVIEW_FRAME_RATE_MODE[]; \
    static const char KEY_SUPPORTED_PREVIEW_FRAME_RATE_MODES[]; \
    static const char KEY_PREVIEW_FRAME_RATE_AUTO_MODE[]; \
    static const char KEY_PREVIEW_FRAME_RATE_FIXED_MODE[]; \
    static const char KEY_PICTURE_SIZE[]; \
    static const char KEY_SUPPORTED_PICTURE_SIZES[]; \
    static const char KEY_PICTURE_FORMAT[]; \
    static const char KEY_SUPPORTED_PICTURE_FORMATS[]; \
    static const char KEY_JPEG_THUMBNAIL_WIDTH[]; \
    static const char KEY_JPEG_THUMBNAIL_HEIGHT[]; \
    static const char KEY_SUPPORTED_JPEG_THUMBNAIL_SIZES[]; \
    static const char KEY_JPEG_THUMBNAIL_QUALITY[]; \
    static const char KEY_JPEG_QUALITY[]; \
    static const char KEY_ROTATION[]; \
    static const char KEY_GPS_LATITUDE[]; \
    static const char KEY_GPS_LONGITUDE[]; \
    static const char KEY_GPS_ALTITUDE[]; \
    static const char KEY_GPS_TIMESTAMP[]; \
    static const char KEY_GPS_PROCESSING_METHOD[]; \
    static const char KEY_WHITE_BALANCE[]; \
    static const char KEY_SUPPORTED_WHITE_BALANCE[]; \
    static const char KEY_EFFECT[]; \
    static const char KEY_SUPPORTED_EFFECTS[]; \
    static const char KEY_TOUCH_AF_AEC[]; \
    static const char KEY_SUPPORTED_TOUCH_AF_AEC[]; \
    static const char KEY_TOUCH_INDEX_AEC[]; \
    static const char KEY_TOUCH_INDEX_AF[]; \
    static const char KEY_ANTIBANDING[]; \
    static const char KEY_SUPPORTED_ANTIBANDING[]; \
    static const char KEY_SCENE_MODE[]; \
    static const char KEY_SUPPORTED_SCENE_MODES[]; \
    static const char KEY_SCENE_DETECT[]; \
    static const char KEY_SUPPORTED_SCENE_DETECT[]; \
    static const char KEY_FLASH_MODE[]; \
    static const char KEY_SUPPORTED_FLASH_MODES[]; \
    static const char KEY_FOCUS_MODE[]; \
    static const char KEY_SUPPORTED_FOCUS_MODES[]; \
    static const char KEY_MAX_NUM_FOCUS_AREAS[]; \
    static const char KEY_FOCUS_AREAS[]; \
    static const char KEY_FOCAL_LENGTH[]; \
    static const char KEY_HORIZONTAL_VIEW_ANGLE[]; \
    static const char KEY_VERTICAL_VIEW_ANGLE[]; \
    static const char KEY_EXPOSURE_COMPENSATION[]; \
    static const char KEY_MAX_EXPOSURE_COMPENSATION[]; \
    static const char KEY_MIN_EXPOSURE_COMPENSATION[]; \
    static const char KEY_EXPOSURE_COMPENSATION_STEP[]; \
    static const char KEY_AUTO_EXPOSURE_LOCK[]; \
    static const char KEY_AUTO_EXPOSURE_LOCK_SUPPORTED[]; \
    static const char KEY_AUTO_WHITEBALANCE_LOCK[]; \
    static const char KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED[]; \
    static const char KEY_MAX_NUM_METERING_AREAS[]; \
    static const char KEY_METERING_AREAS[]; \
    static const char KEY_ZOOM[]; \
    static const char KEY_MAX_ZOOM[]; \
    static const char KEY_ZOOM_RATIOS[]; \
    static const char KEY_ZOOM_SUPPORTED[]; \
    static const char KEY_SMOOTH_ZOOM_SUPPORTED[]; \
    static const char KEY_FOCUS_DISTANCES[]; \
    static const char KEY_VIDEO_SIZE[]; \
    static const char KEY_SUPPORTED_VIDEO_SIZES[]; \
    static const char KEY_MAX_NUM_DETECTED_FACES_HW[]; \
    static const char KEY_MAX_NUM_DETECTED_FACES_SW[]; \
    static const char KEY_PREFERRED_PREVIEW_SIZE_FOR_VIDEO[]; \
    static const char KEY_VIDEO_FRAME_FORMAT[]; \
\
    static const char KEY_RECORDING_HINT[]; \
    static const char KEY_VIDEO_SNAPSHOT_SUPPORTED[]; \
    static const char KEY_FULL_VIDEO_SNAP_SUPPORTED[]; \
    static const char KEY_LENSSHADE[]; \
    static const char KEY_SUPPORTED_LENSSHADE_MODES[]; \
    static const char KEY_AUTO_EXPOSURE[]; \
    static const char KEY_SUPPORTED_AUTO_EXPOSURE[]; \
    static const char KEY_GPS_LATITUDE_REF[]; \
    static const char KEY_GPS_LONGITUDE_REF[]; \
    static const char KEY_GPS_ALTITUDE_REF[]; \
    static const char KEY_GPS_STATUS[]; \
    static const char KEY_EXIF_DATETIME[]; \
    static const char KEY_VIDEO_STABILIZATION[]; \
    static const char KEY_MEMORY_COLOR_ENHANCEMENT[]; \
    static const char KEY_SUPPORTED_MEM_COLOR_ENHANCE_MODES[]; \
    static const char KEY_POWER_MODE_SUPPORTED[]; \
    static const char KEY_POWER_MODE[]; \
    static const char KEY_ZSL[]; \
    static const char KEY_SUPPORTED_ZSL_MODES[]; \
    static const char KEY_CAMERA_MODE[]; \
    static const char KEY_VIDEO_HIGH_FRAME_RATE[]; \
    static const char KEY_SUPPORTED_VIDEO_HIGH_FRAME_RATE_MODES[]; \
    static const char KEY_HIGH_DYNAMIC_RANGE_IMAGING[]; \
    static const char KEY_SUPPORTED_HDR_IMAGING_MODES[]; \
    static const char KEY_VIDEO_STABILIZATION_SUPPORTED[]; \
    static const char KEY_LIGHTFX[]; \
    static const char KEY_SUPPORTED_ISO_MODES[]; \
    static const char KEY_ISO_MODE[]; \
    static const char KEY_AE_BRACKET_HDR[]; \
    static const char FOCUS_DISTANCE_INFINITY[]; \
    static const char KEY_DENOISE[]; \
    static const char KEY_SUPPORTED_DENOISE[]; \
    static const char KEY_SELECTABLE_ZONE_AF[]; \
    static const char KEY_SUPPORTED_SELECTABLE_ZONE_AF[]; \
    static const char KEY_FACE_DETECTION[]; \
    static const char KEY_SUPPORTED_FACE_DETECTION[]; \
    static const char KEY_REDEYE_REDUCTION[]; \
    static const char KEY_SUPPORTED_REDEYE_REDUCTION[]; \
    static const char WHITE_BALANCE_AUTO[]; \
    static const char WHITE_BALANCE_INCANDESCENT[]; \
    static const char WHITE_BALANCE_FLUORESCENT[]; \
    static const char WHITE_BALANCE_WARM_FLUORESCENT[]; \
    static const char WHITE_BALANCE_DAYLIGHT[]; \
    static const char WHITE_BALANCE_CLOUDY_DAYLIGHT[]; \
    static const char WHITE_BALANCE_TWILIGHT[]; \
    static const char WHITE_BALANCE_SHADE[]; \
    static const char WHITE_BALANCE_MANUAL_CCT[]; \
    static const char EFFECT_NONE[]; \
    static const char EFFECT_MONO[]; \
    static const char EFFECT_NEGATIVE[]; \
    static const char EFFECT_SOLARIZE[]; \
    static const char EFFECT_SEPIA[]; \
    static const char EFFECT_POSTERIZE[]; \
    static const char EFFECT_WHITEBOARD[]; \
    static const char EFFECT_BLACKBOARD[]; \
    static const char EFFECT_AQUA[]; \
    static const char EFFECT_EMBOSS[]; \
    static const char EFFECT_SKETCH[]; \
    static const char EFFECT_NEON[]; \
    static const char TOUCH_AF_AEC_OFF[] ; \
    static const char TOUCH_AF_AEC_ON[] ; \
    static const char ANTIBANDING_AUTO[]; \
    static const char ANTIBANDING_50HZ[]; \
    static const char ANTIBANDING_60HZ[]; \
    static const char ANTIBANDING_OFF[]; \
    static const char FLASH_MODE_OFF[]; \
    static const char FLASH_MODE_AUTO[]; \
    static const char FLASH_MODE_ON[]; \
    static const char FLASH_MODE_RED_EYE[]; \
    static const char FLASH_MODE_TORCH[]; \
    static const char SCENE_MODE_AUTO[]; \
    static const char SCENE_MODE_ASD[]; \
    static const char SCENE_MODE_ACTION[]; \
    static const char SCENE_MODE_PORTRAIT[]; \
    static const char SCENE_MODE_LANDSCAPE[]; \
    static const char SCENE_MODE_NIGHT[]; \
    static const char SCENE_MODE_NIGHT_PORTRAIT[]; \
    static const char SCENE_MODE_THEATRE[]; \
    static const char SCENE_MODE_BEACH[]; \
    static const char SCENE_MODE_SNOW[]; \
    static const char SCENE_MODE_SUNSET[]; \
    static const char SCENE_MODE_STEADYPHOTO[]; \
    static const char SCENE_MODE_FIREWORKS[]; \
    static const char SCENE_MODE_SPORTS[]; \
    static const char SCENE_MODE_PARTY[]; \
    static const char SCENE_MODE_CANDLELIGHT[]; \
    static const char SCENE_MODE_BACKLIGHT[]; \
    static const char SCENE_MODE_FLOWERS[]; \
    static const char SCENE_MODE_AR[]; \
    static const char SCENE_MODE_BARCODE[]; \
    static const char SCENE_MODE_HDR[]; \
    static const char SCENE_DETECT_OFF[]; \
    static const char SCENE_DETECT_ON[]; \
\
    static const char PIXEL_FORMAT_YUV422SP[]; \
    static const char PIXEL_FORMAT_YUV420SP[]; \
    static const char PIXEL_FORMAT_YUV420SP_ADRENO[]; \
    static const char PIXEL_FORMAT_YUV422I[]; \
    static const char PIXEL_FORMAT_YUV420P[]; \
    static const char PIXEL_FORMAT_RGB565[]; \
    static const char PIXEL_FORMAT_RGBA8888[]; \
    static const char PIXEL_FORMAT_JPEG[]; \
    static const char PIXEL_FORMAT_BAYER_RGGB[]; \
    static const char PIXEL_FORMAT_ANDROID_OPAQUE[]; \
    static const char PIXEL_FORMAT_RAW[]; \
    static const char PIXEL_FORMAT_YV12[]; \
    static const char PIXEL_FORMAT_NV12[]; \
    static const char FOCUS_MODE_AUTO[]; \
    static const char FOCUS_MODE_INFINITY[]; \
    static const char FOCUS_MODE_MACRO[]; \
    static const char FOCUS_MODE_FIXED[]; \
    static const char FOCUS_MODE_EDOF[]; \
    static const char FOCUS_MODE_CONTINUOUS_VIDEO[]; \
    static const char FOCUS_MODE_CONTINUOUS_PICTURE[]; \
    static const char FOCUS_MODE_MANUAL_POSITION[]; \
    static const char FOCUS_MODE_NORMAL[]; \
    static const char KEY_SKIN_TONE_ENHANCEMENT[] ; \
    static const char KEY_SUPPORTED_SKIN_TONE_ENHANCEMENT_MODES[] ; \
    static const char ISO_AUTO[]; \
    static const char ISO_HJR[]; \
    static const char ISO_100[]; \
    static const char ISO_200[]; \
    static const char ISO_400[]; \
    static const char ISO_800[]; \
    static const char ISO_1600[]; \
    static const char ISO_3200[]; \
    static const char ISO_6400[]; \
    static const char LENSSHADE_ENABLE[]; \
    static const char LENSSHADE_DISABLE[]; \
    static const char AUTO_EXPOSURE_FRAME_AVG[]; \
    static const char AUTO_EXPOSURE_CENTER_WEIGHTED[]; \
    static const char AUTO_EXPOSURE_SPOT_METERING[]; \
    static const char KEY_SHARPNESS[]; \
    static const char KEY_MAX_SHARPNESS[]; \
    static const char KEY_CONTRAST[]; \
    static const char KEY_MAX_CONTRAST[]; \
    static const char KEY_SATURATION[]; \
    static const char KEY_MAX_SATURATION[]; \
    static const char KEY_HISTOGRAM[]; \
    static const char KEY_SUPPORTED_HISTOGRAM_MODES[]; \
    static const char HISTOGRAM_ENABLE[]; \
    static const char HISTOGRAM_DISABLE[]; \
    static const char SKIN_TONE_ENHANCEMENT_ENABLE[]; \
    static const char SKIN_TONE_ENHANCEMENT_DISABLE[]; \
    static const char DENOISE_OFF[]; \
    static const char DENOISE_ON[]; \
    static const char SELECTABLE_ZONE_AF_AUTO[]; \
    static const char SELECTABLE_ZONE_AF_SPOT_METERING[]; \
    static const char SELECTABLE_ZONE_AF_CENTER_WEIGHTED[]; \
    static const char SELECTABLE_ZONE_AF_FRAME_AVERAGE[]; \
    static const char FACE_DETECTION_OFF[]; \
    static const char FACE_DETECTION_ON[]; \
    static const char MCE_ENABLE[]; \
    static const char MCE_DISABLE[]; \
    static const char ZSL_OFF[]; \
    static const char ZSL_ON[]; \
    static const char AE_BRACKET_HDR_OFF[]; \
    static const char AE_BRACKET_HDR[]; \
    static const char AE_BRACKET[]; \
    static const char LOW_POWER[]; \
    static const char NORMAL_POWER[]; \
    static const char VIDEO_HFR_OFF[]; \
    static const char VIDEO_HFR_2X[]; \
    static const char VIDEO_HFR_3X[]; \
    static const char VIDEO_HFR_4X[]; \
    static const char REDEYE_REDUCTION_ENABLE[]; \
    static const char REDEYE_REDUCTION_DISABLE[]; \
    static const char HDR_ENABLE[]; \
    static const char HDR_DISABLE[]; \
    static const char FOCUS_MODE_FACEDETECT[]; \
    static const char FOCUS_MODE_TOUCHAF[]; \
    static const char ISO_50[]; \
    static const char KEY_ANTI_SHAKE_MODE[]; \
    static const char KEY_AUTO_CONTRAST[]; \
    static const char KEY_BEAUTY_MODE[]; \
    static const char KEY_BLUR_MODE[]; \
    static const char KEY_VINTAGE_MODE[]; \
    static const char KEY_WDR_MODE[]; \
    static const char VINTAGE_MODE_BNW[]; \
    static const char VINTAGE_MODE_COOL[]; \
    static const char VINTAGE_MODE_NORMAL[]; \
    static const char VINTAGE_MODE_OFF[]; \
    static const char VINTAGE_MODE_WARM[]; \
    static const char SCENE_MODE_DAWN[]; \
    static const char SCENE_MODE_DUSKDAWN[]; \
    static const char SCENE_MODE_FALL[]; \
    static const char SCENE_MODE_FALL_COLOR[]; \
    static const char SCENE_MODE_TEXT[]; \
    static const char LIGHTFX_LOWLIGHT[]; \
    static const char LIGHTFX_HDR[]; \
\
    int getInt64(const char *key) const; \
    void setPreviewFrameRateMode(const char *mode); \
    const char *getPreviewFrameRateMode() const; \
    void setTouchIndexAec(int x, int y); \
    void getTouchIndexAec(int *x, int *y) const; \
    void setTouchIndexAf(int x, int y); \
    void getTouchIndexAf(int *x, int *y) const; \
    void setPreviewFpsRange(int minFPS, int maxFPS);
