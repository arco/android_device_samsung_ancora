/* Copyright (c) 2011 Code Aurora Forum. All rights reserved.
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

#define LOG_NDDEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#include <hardware/gps.h>

#include <rpc/rpc.h>
#include <loc_api_rpc_glue.h>
#include <loc_eng.h>

#define LOG_TAG "libloc"
#include <utils/Log.h>

// comment this out to enable logging
// #undef LOGD
// #define LOGD(...) {}

typedef struct
{
   char                 name[128];
   long                 val;
} loc_name_val_s_type;

#define NAME_VAL(x) {"" #x "", x }

#define UNKNOWN_STR "UNKNOWN"

/* Event names */
loc_name_val_s_type loc_eng_event_name[] =
   {
      NAME_VAL( RPC_LOC_EVENT_PARSED_POSITION_REPORT ),
      NAME_VAL( RPC_LOC_EVENT_SATELLITE_REPORT ),
      NAME_VAL( RPC_LOC_EVENT_NMEA_1HZ_REPORT ),
      NAME_VAL( RPC_LOC_EVENT_NMEA_POSITION_REPORT ),
      NAME_VAL( RPC_LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST ),
      NAME_VAL( RPC_LOC_EVENT_ASSISTANCE_DATA_REQUEST ),
      NAME_VAL( RPC_LOC_EVENT_LOCATION_SERVER_REQUEST ),
      NAME_VAL( RPC_LOC_EVENT_IOCTL_REPORT ),
      NAME_VAL( RPC_LOC_EVENT_STATUS_REPORT ),
#if (AMSS_VERSION != 3200&&AMSS_VERSION != 20000)
      NAME_VAL( RPC_LOC_EVENT_WPS_NEEDED_REQUEST ),
#endif
   };
int loc_event_num = sizeof loc_eng_event_name / sizeof(loc_name_val_s_type);

/* IOCTL Type names */
loc_name_val_s_type loc_ioctl_type_name[] =
   {
      NAME_VAL( RPC_LOC_IOCTL_GET_API_VERSION ),
      NAME_VAL( RPC_LOC_IOCTL_SET_FIX_CRITERIA ),
      NAME_VAL( RPC_LOC_IOCTL_GET_FIX_CRITERIA ),
      NAME_VAL( RPC_LOC_IOCTL_INFORM_NI_USER_RESPONSE ),
      NAME_VAL( RPC_LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA ),
      NAME_VAL( RPC_LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_VALIDITY ),
      NAME_VAL( RPC_LOC_IOCTL_QUERY_PREDICTED_ORBITS_DATA_SOURCE ),
      NAME_VAL( RPC_LOC_IOCTL_SET_PREDICTED_ORBITS_DATA_AUTO_DOWNLOAD ),
      NAME_VAL( RPC_LOC_IOCTL_INJECT_UTC_TIME ),
      NAME_VAL( RPC_LOC_IOCTL_INJECT_RTC_VALUE ),
      NAME_VAL( RPC_LOC_IOCTL_INJECT_POSITION ),
      NAME_VAL( RPC_LOC_IOCTL_QUERY_ENGINE_STATE ),
      NAME_VAL( RPC_LOC_IOCTL_INFORM_SERVER_OPEN_STATUS ),
      NAME_VAL( RPC_LOC_IOCTL_INFORM_SERVER_CLOSE_STATUS ),
#if (AMSS_VERSION != 3200&&AMSS_VERSION != 20000)
      NAME_VAL( RPC_LOC_IOCTL_SEND_WIPER_POSITION_REPORT ),
      NAME_VAL( RPC_LOC_IOCTL_NOTIFY_WIPER_STATUS ),
#endif
      NAME_VAL( RPC_LOC_IOCTL_SET_ENGINE_LOCK ),
      NAME_VAL( RPC_LOC_IOCTL_GET_ENGINE_LOCK ),
      NAME_VAL( RPC_LOC_IOCTL_SET_SBAS_CONFIG ),
      NAME_VAL( RPC_LOC_IOCTL_GET_SBAS_CONFIG ),
      NAME_VAL( RPC_LOC_IOCTL_SET_NMEA_TYPES ),
      NAME_VAL( RPC_LOC_IOCTL_GET_NMEA_TYPES ),
      NAME_VAL( RPC_LOC_IOCTL_SET_CDMA_PDE_SERVER_ADDR ),
      NAME_VAL( RPC_LOC_IOCTL_GET_CDMA_PDE_SERVER_ADDR ),
      NAME_VAL( RPC_LOC_IOCTL_SET_CDMA_MPC_SERVER_ADDR ),
      NAME_VAL( RPC_LOC_IOCTL_GET_CDMA_MPC_SERVER_ADDR ),
      NAME_VAL( RPC_LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR ),
      NAME_VAL( RPC_LOC_IOCTL_GET_UMTS_SLP_SERVER_ADDR ),
      NAME_VAL( RPC_LOC_IOCTL_SET_ON_DEMAND_LPM ),
      NAME_VAL( RPC_LOC_IOCTL_GET_ON_DEMAND_LPM ),
      NAME_VAL( RPC_LOC_IOCTL_DELETE_ASSIST_DATA ),
      NAME_VAL( RPC_LOC_IOCTL_SET_CUSTOM_PDE_SERVER_ADDR ),
      NAME_VAL( RPC_LOC_IOCTL_GET_CUSTOM_PDE_SERVER_ADDR ),
   };
int loc_ioctl_type_num = sizeof loc_ioctl_type_name / sizeof(loc_name_val_s_type);

/* IOCTL Status names */
loc_name_val_s_type loc_ioctl_status_name[] =
   {
      NAME_VAL( RPC_LOC_API_SUCCESS ),
      NAME_VAL( RPC_LOC_API_GENERAL_FAILURE ),
      NAME_VAL( RPC_LOC_API_UNSUPPORTED ),
      NAME_VAL( RPC_LOC_API_INVALID_HANDLE ),
      NAME_VAL( RPC_LOC_API_INVALID_PARAMETER ),
      NAME_VAL( RPC_LOC_API_ENGINE_BUSY ),
      NAME_VAL( RPC_LOC_API_PHONE_OFFLINE ),
      NAME_VAL( RPC_LOC_API_TIMEOUT )
   };
int loc_ioctl_status_num = sizeof loc_ioctl_status_name / sizeof(loc_name_val_s_type);

/* Fix session status names */
loc_name_val_s_type loc_sess_status_name[] =
   {
      NAME_VAL( RPC_LOC_SESS_STATUS_SUCCESS ),
      NAME_VAL( RPC_LOC_SESS_STATUS_IN_PROGESS ),
      NAME_VAL( RPC_LOC_SESS_STATUS_GENERAL_FAILURE ),
      NAME_VAL( RPC_LOC_SESS_STATUS_TIMEOUT ),
      NAME_VAL( RPC_LOC_SESS_STATUS_USER_END ),
      NAME_VAL( RPC_LOC_SESS_STATUS_BAD_PARAMETER ),
      NAME_VAL( RPC_LOC_SESS_STATUS_PHONE_OFFLINE ),
      NAME_VAL( RPC_LOC_SESS_STATUS_USER_END ),
      NAME_VAL( RPC_LOC_SESS_STATUS_ENGINE_LOCKED )
   };
int loc_sess_status_num = sizeof loc_sess_status_name / sizeof(loc_name_val_s_type);

/* Engine state names */
loc_name_val_s_type loc_engine_state_name[] =
   {
      NAME_VAL( RPC_LOC_ENGINE_STATE_ON ),
      NAME_VAL( RPC_LOC_ENGINE_STATE_OFF )
   };
int loc_engine_state_num = sizeof loc_engine_state_name / sizeof(loc_name_val_s_type);

/* Fix session state names */
loc_name_val_s_type loc_fix_session_state_name[] =
   {
      NAME_VAL( RPC_LOC_FIX_SESSION_STATE_BEGIN ),
      NAME_VAL( RPC_LOC_FIX_SESSION_STATE_END )
   };
int loc_fix_session_state_num = sizeof loc_fix_session_state_name / sizeof(loc_name_val_s_type);

/* GPS status names */
loc_name_val_s_type gps_status_name[] =
   {
      NAME_VAL( GPS_STATUS_NONE ),
      NAME_VAL( GPS_STATUS_SESSION_BEGIN ),
      NAME_VAL( GPS_STATUS_SESSION_END ),
      NAME_VAL( GPS_STATUS_ENGINE_ON ),
      NAME_VAL( GPS_STATUS_ENGINE_OFF ),
   };
int gps_status_num = sizeof gps_status_name / sizeof(loc_name_val_s_type);

/* Get names from value */
static const char* loc_eng_get_name_from_mask(loc_name_val_s_type table[], int table_size, long mask)
{
   int i;
   for (i = 0; i < table_size; i++)
   {
      if (table[i].val & (long) mask)
      {
         return table[i].name;
      }
   }
   return UNKNOWN_STR;
}

/* Get names from value */
static const char* loc_eng_get_name_from_val(loc_name_val_s_type table[], int table_size, long value)
{
   int i;
   for (i = 0; i < table_size; i++)
   {
      if (table[i].val == (long) value)
      {
         return table[i].name;
      }
   }
   return UNKNOWN_STR;
}

/* Finds the first event found in the mask */
const char* loc_get_event_name(rpc_loc_event_mask_type loc_event_mask)
{
   return loc_eng_get_name_from_mask(loc_eng_event_name, loc_event_num,
         (long) loc_event_mask);
}

/* Finds IOCTL type name */
const char* loc_get_ioctl_type_name(rpc_loc_ioctl_e_type ioctl_type)
{
   return loc_eng_get_name_from_val(loc_ioctl_type_name, loc_ioctl_type_num,
         (long) ioctl_type);
}

/* Finds IOCTL status name */
const char* loc_get_ioctl_status_name(uint32 status)
{
   return loc_eng_get_name_from_val(loc_ioctl_status_name, loc_ioctl_status_num,
         (long) status);
}

/* Finds session status name */
const char* loc_get_sess_status_name(rpc_loc_session_status_e_type status)
{
   return loc_eng_get_name_from_val(loc_sess_status_name, loc_sess_status_num,
         (long) status);
}

/* Find engine state name */
const char* loc_get_engine_state_name(rpc_loc_engine_state_e_type state)
{
   return loc_eng_get_name_from_val(loc_engine_state_name, loc_engine_state_num,
         (long) state);
}

/* Find engine state name */
const char* loc_get_fix_session_state_name(rpc_loc_fix_session_state_e_type state)
{
   return loc_eng_get_name_from_val(loc_fix_session_state_name, loc_fix_session_state_num,
         (long) state);
}

/* Find Android GPS status name */
const char* loc_get_gps_status_name(GpsStatusValue gps_status)
{
   return loc_eng_get_name_from_val(gps_status_name, gps_status_num,
         (long) gps_status);
}

const char* log_succ_fail_string(int is_succ)
{
   return is_succ? "successful" : "failed";
}

static const char* log_final_interm_string(int is_final)
{
   return is_final ? "final" : "intermediate";
}

/* Logs parsed report */
static void log_parsed_report(const rpc_loc_parsed_position_s_type *parsed_report)
{
   rpc_loc_session_status_e_type status = parsed_report->session_status;
   LOC_LOGD("Session status: %s   Valid mask: 0x%X\n",
         loc_get_sess_status_name(status),
         (uint) parsed_report->valid_mask);
   LOC_LOGD("Latitude:  %.7f (%s)\n", parsed_report->latitude,
         log_final_interm_string(
               (parsed_report->valid_mask & RPC_LOC_POS_VALID_LATITUDE) &&
               parsed_report->session_status == RPC_LOC_SESS_STATUS_SUCCESS));
   LOC_LOGD("Longitude: %.7f\n", parsed_report->longitude);
   LOC_LOGD("Accuracy: %.7f\n", parsed_report->hor_unc_circular);
}

/* Logs status report */
static void log_status_report(const rpc_loc_status_event_s_type *status_event)
{
   rpc_loc_status_event_e_type event = status_event->event;
   switch (event) {
   case RPC_LOC_STATUS_EVENT_ENGINE_STATE:
      LOC_LOGD("Engine state: %s\n",
            loc_get_engine_state_name(
                  status_event->payload.rpc_loc_status_event_payload_u_type_u.engine_state));
      break;
   case RPC_LOC_STATUS_EVENT_FIX_SESSION_STATE:
      LOC_LOGD("Fix session state: %s\n",
            loc_get_fix_session_state_name(
                  status_event->payload.rpc_loc_status_event_payload_u_type_u.fix_session_state));
      break;
   default:
      break;
   }
}

#define CHECK_MASK(type, value, mask_var, mask) \
   ((mask_var & mask) ? (type) value : (type) (-1))

/* Logs valid fields in the GNSS SV constellation report */
static void log_satellite_report(const rpc_loc_gnss_info_s_type *gnss)
{
   if (gnss->valid_mask & RPC_LOC_GNSS_INFO_VALID_POS_DOP)
   {
      LOC_LOGV("position dop: %.3f\n", (float) gnss->position_dop);
   }
   if (gnss->valid_mask & RPC_LOC_GNSS_INFO_VALID_HOR_DOP)
   {
      LOC_LOGV("horizontal dop: %.3f\n", (float) gnss->horizontal_dop);
   }
   if (gnss->valid_mask & RPC_LOC_GNSS_INFO_VALID_VERT_DOP)
   {
      LOC_LOGV("vertical dop: %.3f\n", (float) gnss->vertical_dop);
   }
   if (gnss->valid_mask & RPC_LOC_GNSS_INFO_VALID_ALTITUDE_ASSUMED)
   {
      LOC_LOGV("altitude assumed: %d\n", (int) gnss->altitude_assumed);
   }
   if (gnss->valid_mask & RPC_LOC_GNSS_INFO_VALID_SV_COUNT)
   {
      LOC_LOGD("sv count: %d\n", (int) gnss->sv_count);
   }
   if (gnss->valid_mask & RPC_LOC_GNSS_INFO_VALID_SV_LIST)
   {
      LOC_LOGV("sv list: ");

      if (gnss->sv_count)
      {
         LOC_LOGV("\n\tsys\tprn\thlth\tproc\teph\talm\telev\tazi\tsnr\n");
      }
      else {
         LOC_LOGV("empty\n");
      }

      int i;
      for (i = 0; i < gnss->sv_count; i++)
      {
         const rpc_loc_sv_info_s_type *sv = &gnss->sv_list.sv_list_val[i];
         rpc_loc_sv_info_valid_mask_type mask = sv->valid_mask;
         LOC_LOGV("  %d: \t%d\t%d\t%d\t%d\t%d\t%d\t%.3f\t%.3f\t%.3f\n", i,
               CHECK_MASK(int,   sv->system,         mask, RPC_LOC_SV_INFO_VALID_SYSTEM),
               CHECK_MASK(int,   sv->prn,            mask, RPC_LOC_SV_INFO_VALID_PRN),
               CHECK_MASK(int,   sv->health_status,  mask, RPC_LOC_SV_INFO_VALID_HEALTH_STATUS),
               CHECK_MASK(int,   sv->process_status, mask, RPC_LOC_SV_INFO_VALID_PROCESS_STATUS),
               CHECK_MASK(int,   sv->has_eph,        mask, RPC_LOC_SV_INFO_VALID_HAS_EPH),
               CHECK_MASK(int,   sv->has_alm,        mask, RPC_LOC_SV_INFO_VALID_HAS_ALM),
               CHECK_MASK(float, sv->elevation,      mask, RPC_LOC_SV_INFO_VALID_ELEVATION),
               CHECK_MASK(float, sv->azimuth,        mask, RPC_LOC_SV_INFO_VALID_AZIMUTH),
               CHECK_MASK(float, sv->snr,            mask, RPC_LOC_SV_INFO_VALID_SNR)
         );
      }
   }
}

/*===========================================================================

FUNCTION loc_eng_get_time

DESCRIPTION
   Logs a callback event header.
   The pointer time_string should point to a buffer of at least 13 bytes:

   XX:XX:XX.000\0

RETURN VALUE
   The time string

===========================================================================*/
char *loc_eng_get_time(char *time_string)
{
   struct timeval now;     /* sec and usec     */
   struct tm now_tm;       /* broken-down time */
   char hms_string[80];    /* HH:MM:SS         */

   gettimeofday(&now, NULL);
   localtime_r(&now.tv_sec, &now_tm);

   strftime(hms_string, sizeof hms_string, "%H:%M:%S", &now_tm);
   sprintf(time_string, "%s.%03d", hms_string, (int) (now.tv_usec / 1000));

   return time_string;
}

/* Logs a callback event header */
int loc_eng_callback_log_header(
      rpc_loc_client_handle_type            client_handle,
      rpc_loc_event_mask_type               loc_event,              /* event mask           */
      const rpc_loc_event_payload_u_type*   loc_event_payload       /* payload              */
)
{
   char time_string[64]; /* full time string */
   const char *event_name = loc_get_event_name(loc_event);
   if (event_name == NULL)
   {
      event_name = UNKNOWN_STR;
   }

   /* Event header */
   LOC_LOGD("\nEvent %s (client %d)\n",
         /* loc_eng_get_time(time_string), */
         event_name,
         (int) client_handle);

   return 0;
}

/* Logs a callback event */
int loc_eng_callback_log(
      rpc_loc_event_mask_type               loc_event,              /* event mask           */
      const rpc_loc_event_payload_u_type*   loc_event_payload       /* payload              */
)
{
   switch (loc_event)
   {
   case RPC_LOC_EVENT_SATELLITE_REPORT:
      log_satellite_report(&loc_event_payload->
            rpc_loc_event_payload_u_type_u.gnss_report);
      break;
   case RPC_LOC_EVENT_STATUS_REPORT:
      log_status_report(&loc_event_payload->
            rpc_loc_event_payload_u_type_u.status_report);
      break;
   case RPC_LOC_EVENT_PARSED_POSITION_REPORT:
      log_parsed_report(&loc_event_payload->
            rpc_loc_event_payload_u_type_u.parsed_location_report);
      break;
   default:
      break;
   }

   return 0;
}
