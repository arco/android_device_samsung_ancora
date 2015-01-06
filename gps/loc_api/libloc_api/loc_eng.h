/* Copyright (c) 2009,2011 Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef LOC_ENG_H
#define LOC_ENG_H

// Uncomment to keep all LOG messages (LOGD, LOGI, LOGV, etc.)
// #define LOG_NDEBUG 0

// Define boolean type to be used by libgps on loc api module
typedef unsigned char boolean;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#include <loc_eng_ioctl.h>
#include <loc_eng_xtra.h>
#include <loc_eng_ni.h>
#include <loc_eng_log.h>
#include <loc_eng_cfg.h>

#define LOC_IOCTL_DEFAULT_TIMEOUT 1500 // 1500 milli-seconds

// The data connection minimal open time
#define DATA_OPEN_MIN_TIME        1  /* sec */

// The system sees GPS engine turns off after inactive for this period of time
#define GPS_AUTO_OFF_TIME         2  /* secs */
//To signify that when requesting a data connection HAL need not specify whether CDMA or UMTS
#define DONT_CARE                 0

#define MIN_POSSIBLE_FIX_INTERVAL 1000 /* msec */
enum loc_mute_session_e_type {
   LOC_MUTE_SESS_NONE,
   LOC_MUTE_SESS_WAIT,
   LOC_MUTE_SESS_IN_SESSION
};

enum {
    DEFERRED_ACTION_EVENT               = 0x01,
    DEFERRED_ACTION_DELETE_AIDING       = 0x02,
    DEFERRED_ACTION_AGPS_STATUS         = 0x04,
    DEFERRED_ACTION_AGPS_DATA_SUCCESS   = 0x08,
    DEFERRED_ACTION_AGPS_DATA_CLOSED    = 0x10,
    DEFERRED_ACTION_AGPS_DATA_FAILED    = 0x20,
    DEFERRED_ACTION_QUIT                = 0x40,
};

// Module data
typedef struct
{
   rpc_loc_client_handle_type     client_handle;

   gps_location_callback          location_cb;
   gps_status_callback            status_cb;
   gps_sv_status_callback         sv_status_cb;
   agps_status_callback           agps_status_cb;
   gps_nmea_callback              nmea_cb;
   gps_ni_notify_callback         ni_notify_cb;
   gps_acquire_wakelock           acquire_wakelock_cb;
   gps_release_wakelock           release_wakelock_cb;
   AGpsStatusValue                agps_status;
   // used to defer stopping the GPS engine until AGPS data calls are done
   boolean                         agps_request_pending;
   boolean                         stop_request_pending;
   pthread_mutex_t                 deferred_stop_mutex;
   loc_eng_xtra_data_s_type       xtra_module_data;
   // data from loc_event_cb
   rpc_loc_event_mask_type        loc_event;
   rpc_loc_event_payload_u_type   loc_event_payload;

   boolean                        client_opened;
   boolean                        navigating;
   boolean                        data_connection_is_on;

   // ATL variables
   char                           apn_name[100];
   rpc_loc_server_connection_handle  conn_handle;
   time_t                         data_conn_open_time;

   // GPS engine status
   GpsStatusValue                 engine_status;
   GpsStatusValue                 fix_session_status;

   // Aiding data information to be deleted, aiding data can only be deleted when GPS engine is off
   GpsAidingData                  aiding_data_for_deletion;

   // IOCTL CB lock
   pthread_mutex_t                ioctl_cb_lock;

   // Data variables used by deferred action thread
   pthread_t                      deferred_action_thread;

   // Timer thread (wakes up every second)
   pthread_t                      timer_thread;

   // Mutex used by deferred action thread
   pthread_mutex_t                deferred_action_mutex;
   // Condition variable used by deferred action thread
   pthread_cond_t                 deferred_action_cond;
   // flags for pending events for deferred action thread
   int                             deferred_action_flags;
   // For muting session broadcast
   pthread_mutex_t                mute_session_lock;
   loc_mute_session_e_type        mute_session_state;

} loc_eng_data_s_type;

extern loc_eng_data_s_type loc_eng_data;

extern void loc_eng_mute_one_session();

/* LOGGING MACROS */
#define LOC_LOGE(...) \
if (gps_conf.DEBUG_LEVEL >= 1) { ALOGE(__VA_ARGS__); }

#define LOC_LOGW(...) \
if (gps_conf.DEBUG_LEVEL >= 2) { ALOGW(__VA_ARGS__); }

#define LOC_LOGI(...) \
if (gps_conf.DEBUG_LEVEL >= 3) { ALOGI(__VA_ARGS__); }

#define LOC_LOGD(...) \
if (gps_conf.DEBUG_LEVEL >= 4) { ALOGD(__VA_ARGS__); }

#define LOC_LOGV(...) \
if (gps_conf.DEBUG_LEVEL >= 5) { ALOGV(__VA_ARGS__); }

#endif // LOC_ENG_H
