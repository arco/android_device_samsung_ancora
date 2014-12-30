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

#define LOG_NDDEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#include <rpc/rpc.h>
#include <loc_api_rpc_glue.h>

#include <loc_eng.h>

#define LOG_TAG "libloc"
#include <utils/Log.h>

// comment this out to enable logging
// #undef LOGD
// #define LOGD(...) {}

/*=============================================================================
 *
 *                             DATA DECLARATION
 *
 *============================================================================*/

const GpsNiInterface sLocEngNiInterface =
{
   sizeof(GpsNiInterface),
   loc_eng_ni_init,
   loc_eng_ni_respond,
};

boolean loc_eng_ni_data_init = FALSE;
loc_eng_ni_data_s_type loc_eng_ni_data;

extern loc_eng_data_s_type loc_eng_data;
/* User response callback waiting conditional variable */
pthread_cond_t             user_cb_arrived_cond = PTHREAD_COND_INITIALIZER;

/* User callback waiting data block, protected by user_cb_data_mutex */
pthread_mutex_t            user_cb_data_mutex   = PTHREAD_MUTEX_INITIALIZER;

/*=============================================================================
 *
 *                             FUNCTION DECLARATIONS
 *
 *============================================================================*/
static void* loc_ni_thread_proc(void *threadid);
/*===========================================================================

FUNCTION respond_from_enum

DESCRIPTION
   Returns the name of the response

RETURN VALUE
   response name string

===========================================================================*/
static const char* respond_from_enum(rpc_loc_ni_user_resp_e_type resp)
{
   switch (resp)
   {
   case RPC_LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT:
      return "accept";
   case RPC_LOC_NI_LCS_NOTIFY_VERIFY_DENY:
      return "deny";
   case RPC_LOC_NI_LCS_NOTIFY_VERIFY_NORESP:
      return "no response";
   default:
      return NULL;
   }
}

/*===========================================================================

FUNCTION loc_ni_respond

DESCRIPTION
   Displays the NI request and awaits user input. If a previous request is
   in session, the new one is handled using sys.ni_default_response (if exists);
   otherwise, it is denied.

DEPENDENCY
   Do not lock the data by mutex loc_ni_lock

RETURN VALUE
   none

===========================================================================*/
static void loc_ni_respond
(
      rpc_loc_ni_user_resp_e_type resp,
      const rpc_loc_ni_event_s_type *request_pass_back
)
{
   LOC_LOGD("Sending NI response: %s\n", respond_from_enum(resp));

   rpc_loc_ioctl_data_u_type data;
   rpc_loc_ioctl_callback_s_type callback_payload;

   memcpy(&data.rpc_loc_ioctl_data_u_type_u.user_verify_resp.ni_event_pass_back,
         request_pass_back, sizeof (rpc_loc_ni_event_s_type));
   data.rpc_loc_ioctl_data_u_type_u.user_verify_resp.user_resp = resp;

   loc_eng_ioctl(
         loc_eng_data.client_handle,
         RPC_LOC_IOCTL_INFORM_NI_USER_RESPONSE,
         &data,
         LOC_IOCTL_DEFAULT_TIMEOUT,
         &callback_payload
   );
}

/*===========================================================================

FUNCTION loc_ni_fill_notif_verify_type

DESCRIPTION
   Fills need_notify, need_verify, etc.

RETURN VALUE
   none

===========================================================================*/
static boolean loc_ni_fill_notif_verify_type(GpsNiNotification *notif,
      rpc_loc_ni_notify_verify_e_type notif_priv)
{
   notif->notify_flags       = 0;
   notif->default_response   = GPS_NI_RESPONSE_NORESP;

   switch (notif_priv)
   {
   case RPC_LOC_NI_USER_NO_NOTIFY_NO_VERIFY:
      notif->notify_flags = 0;
      break;

   case RPC_LOC_NI_USER_NOTIFY_ONLY:
      notif->notify_flags = GPS_NI_NEED_NOTIFY;
      break;

   case RPC_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
      notif->notify_flags = GPS_NI_NEED_NOTIFY | GPS_NI_NEED_VERIFY;
      notif->default_response = GPS_NI_RESPONSE_ACCEPT;
      break;

   case RPC_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
      notif->notify_flags = GPS_NI_NEED_NOTIFY | GPS_NI_NEED_VERIFY;
      notif->default_response = GPS_NI_RESPONSE_DENY;
      break;

   case RPC_LOC_NI_USER_PRIVACY_OVERRIDE:
      notif->notify_flags = GPS_NI_PRIVACY_OVERRIDE;
      break;

   default:
      return FALSE;
   }

   return TRUE;
}

/*===========================================================================

FUNCTION hexcode

DESCRIPTION
   Converts a binary array into a Hex string. E.g., 1F 00 3F --> "1F003F"

RETURN VALUE
   bytes encoded

===========================================================================*/
static int hexcode(char *hexstring, int string_size, const char *data, int data_size)
{
   int i;
   for (i = 0; i < data_size; i++)
   {
      char ch = data[i];
      if (i*2 + 3 <= string_size)
      {
         snprintf(&hexstring[i*2], 3, "%02X", ch);
      }
      else {
         break;
      }
   }
   return i;
}

/*===========================================================================

FUNCTION decode_address

DESCRIPTION
   Converts a binary encoding into an address string. E.g., 91 21 F3 --> "123".
The 91 is a prefix, hex digits are reversed in order. 0xF means the absence of
a digit.

RETURN VALUE
   number of bytes encoded

===========================================================================*/
static int decode_address(char *addr_string, int string_size, const char *data, int data_size)
{
   const char addr_prefix = 0x91;
   int i, idxOutput = 0;

   if (!data || !addr_string) { return 0; }

   if (data[0] != addr_prefix)
   {
      LOC_LOGW("decode_address: address prefix is not 0x%x but 0x%x", addr_prefix, data[0]);
      addr_string[0] = '\0';
      return 0; // prefix not correct
   }

   for (i = 1; i < data_size; i++)
   {
      unsigned char ch = data[i], low = ch & 0x0F, hi = ch >> 4;
      if (low <= 9 && idxOutput < string_size - 1) { addr_string[idxOutput++] = low + '0'; }
      if (hi <= 9 && idxOutput < string_size - 1) { addr_string[idxOutput++] = hi + '0'; }
   }

   addr_string[idxOutput] = '\0'; // Terminates the string

   return idxOutput;
}

static GpsNiEncodingType convert_encoding_type(int loc_encoding)
{
   GpsNiEncodingType enc = GPS_ENC_UNKNOWN;

   switch (loc_encoding)
   {
   case RPC_LOC_NI_SUPL_UTF8:
      enc = GPS_ENC_SUPL_UTF8;
      break;
   case RPC_LOC_NI_SUPL_UCS2:
      enc = GPS_ENC_SUPL_UCS2;
      break;
   case RPC_LOC_NI_SUPL_GSM_DEFAULT:
      enc = GPS_ENC_SUPL_GSM_DEFAULT;
      break;
   case RPC_LOC_NI_SS_LANGUAGE_UNSPEC:
      enc = GPS_ENC_SUPL_GSM_DEFAULT; // SS_LANGUAGE_UNSPEC = GSM
      break;
   default:
      break;
   }

   return enc;
}

/*===========================================================================

FUNCTION loc_ni_request_handler

DESCRIPTION
   Displays the NI request and awaits user input. If a previous request is
   in session, it is ignored.

RETURN VALUE
   none

===========================================================================*/
static void loc_ni_request_handler(const char *msg, const rpc_loc_ni_event_s_type *ni_req)
{
   GpsNiNotification notif;
   char lcs_addr[32]; // Decoded LCS address for UMTS CP NI

   notif.size = sizeof(notif);
   strlcpy(notif.text, "[text]", sizeof notif.text);    // defaults
   strlcpy(notif.requestor_id, "[requestor id]", sizeof notif.requestor_id);

   /* If busy, use default or deny */
   if (loc_eng_ni_data.notif_in_progress)
   {
      /* XXX Consider sending a NO RESPONSE reply or queue the request */
      LOC_LOGW("loc_ni_request_handler, notification in progress, new NI request ignored, type: %d",
            ni_req->event);
   }
   else {
      /* Print notification */
      LOC_LOGD("NI Notification: %s, event: %d", msg, ni_req->event);

      pthread_mutex_lock(&loc_eng_ni_data.loc_ni_lock);

      /* Save request */
      memcpy(&loc_eng_ni_data.loc_ni_request, ni_req, sizeof loc_eng_ni_data.loc_ni_request);

      /* Set up NI response waiting */
      loc_eng_ni_data.notif_in_progress = TRUE;
      loc_eng_ni_data.current_notif_id = abs(rand());

      /* Fill in notification */
      notif.notification_id = loc_eng_ni_data.current_notif_id;

      const rpc_loc_ni_vx_notify_verify_req_s_type *vx_req;
      const rpc_loc_ni_supl_notify_verify_req_s_type *supl_req;
      const rpc_loc_ni_umts_cp_notify_verify_req_s_type *umts_cp_req;

      switch (ni_req->event)
      {
      case RPC_LOC_NI_EVENT_VX_NOTIFY_VERIFY_REQ:
         vx_req = &ni_req->payload.rpc_loc_ni_event_payload_u_type_u.vx_req;
         notif.ni_type     = GPS_NI_TYPE_VOICE;
         notif.timeout     = LOC_NI_NO_RESPONSE_TIME; // vx_req->user_resp_timer_val;
         memset(notif.extras, 0, sizeof notif.extras);
         memset(notif.text, 0, sizeof notif.text);
         memset(notif.requestor_id, 0, sizeof notif.requestor_id);

         // Requestor ID
         hexcode(notif.requestor_id, sizeof notif.requestor_id,
               vx_req->requester_id.requester_id,
               vx_req->requester_id.requester_id_length);

         notif.text_encoding = 0; // No text and no encoding
         notif.requestor_id_encoding = convert_encoding_type(vx_req->encoding_scheme);

         // Set default_response & notify_flags
         loc_ni_fill_notif_verify_type(&notif, vx_req->notification_priv_type);

         // Privacy override handling
         if (vx_req->notification_priv_type == RPC_LOC_NI_USER_PRIVACY_OVERRIDE)
         {
            loc_eng_mute_one_session();
         }

         break;

      case RPC_LOC_NI_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ:
         umts_cp_req = &ni_req->payload.rpc_loc_ni_event_payload_u_type_u.umts_cp_req;
         notif.ni_type     = GPS_NI_TYPE_UMTS_CTRL_PLANE;
         notif.timeout     = LOC_NI_NO_RESPONSE_TIME; // umts_cp_req->user_response_timer;
         memset(notif.extras, 0, sizeof notif.extras);
         memset(notif.text, 0, sizeof notif.text);
         memset(notif.requestor_id, 0, sizeof notif.requestor_id);

         // Stores notification text
#if (AMSS_VERSION==3200||AMSS_VERSION==20000)
         hexcode(notif.text, sizeof notif.text,
               umts_cp_req->notification_text.notification_text_val,
               umts_cp_req->notification_length);
#else
         hexcode(notif.text, sizeof notif.text,
               umts_cp_req->notification_text,
               umts_cp_req->notification_length);
#endif /* #if (AMSS_VERSION==3200||AMSS_VERSION==20000) */

         // Stores requestor ID
#if (AMSS_VERSION==3200||AMSS_VERSION==20000)
         hexcode(notif.requestor_id, sizeof notif.requestor_id,
               umts_cp_req->requestor_id.requestor_id_string.requestor_id_string_val,
               umts_cp_req->requestor_id.string_len);
#else
         hexcode(notif.requestor_id, sizeof notif.requestor_id,
               umts_cp_req->requestor_id.requestor_id_string,
               umts_cp_req->requestor_id.string_len);
#endif

         // Encodings
         notif.text_encoding = convert_encoding_type(umts_cp_req->datacoding_scheme);
         notif.requestor_id_encoding = convert_encoding_type(umts_cp_req->datacoding_scheme);

         // LCS address (using extras field)
         if (umts_cp_req->ext_client_address_data.ext_client_address_len != 0)
         {
            // Copy LCS Address into notif.extras in the format: Address = 012345
            strlcat(notif.extras, LOC_NI_NOTIF_KEY_ADDRESS, sizeof notif.extras);
            strlcat(notif.extras, " = ", sizeof notif.extras);
            int addr_len = 0;
            const char *address_source = NULL;

#if (AMSS_VERSION==3200||AMSS_VERSION==20000)
            address_source = umts_cp_req->ext_client_address_data.ext_client_address.ext_client_address_val;
#else
            address_source = umts_cp_req->ext_client_address_data.ext_client_address;
#endif /* #if (AMSS_VERSION==3200||AMSS_VERSION==20000) */

            addr_len = decode_address(lcs_addr, sizeof lcs_addr,
               address_source, umts_cp_req->ext_client_address_data.ext_client_address_len);

            // The address is ASCII string
            if (addr_len)
            {
               strlcat(notif.extras, lcs_addr, sizeof notif.extras);
            }
         }

         // Set default_response & notify_flags
         loc_ni_fill_notif_verify_type(&notif, umts_cp_req->notification_priv_type);

         // Privacy override handling
         if (umts_cp_req->notification_priv_type == RPC_LOC_NI_USER_PRIVACY_OVERRIDE)
         {
            loc_eng_mute_one_session();
         }

         break;

      case RPC_LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ:
         supl_req = &ni_req->payload.rpc_loc_ni_event_payload_u_type_u.supl_req;
         notif.ni_type     = GPS_NI_TYPE_UMTS_SUPL;
         notif.timeout     = LOC_NI_NO_RESPONSE_TIME; // supl_req->user_response_timer;
         memset(notif.extras, 0, sizeof notif.extras);
         memset(notif.text, 0, sizeof notif.text);
         memset(notif.requestor_id, 0, sizeof notif.requestor_id);

         // Client name
         if (supl_req->flags & RPC_LOC_NI_CLIENT_NAME_PRESENT)
         {

#if (AMSS_VERSION==3200||AMSS_VERSION==20000)
            hexcode(notif.text, sizeof notif.text,
                    supl_req->client_name.client_name_string.client_name_string_val,   /* buffer */
                    supl_req->client_name.string_len                                   /* length */
            );
#else
            hexcode(notif.text, sizeof notif.text,
                            supl_req->client_name.client_name_string,   /* buffer */
                            supl_req->client_name.string_len            /* length */
            );
#endif /* #if (AMSS_VERSION==3200||AMSS_VERSION==20000) */

            LOC_LOGV("SUPL NI: client_name: %s len=%d", notif.text, supl_req->client_name.string_len);
         }
         else {
            LOC_LOGV("SUPL NI: client_name not present.");
         }

         // Requestor ID
         if (supl_req->flags & RPC_LOC_NI_REQUESTOR_ID_PRESENT)
         {
#if (AMSS_VERSION==3200||AMSS_VERSION==20000)
            hexcode(notif.requestor_id, sizeof notif.requestor_id,
                  supl_req->requestor_id.requestor_id_string.requestor_id_string_val,  /* buffer */
                  supl_req->requestor_id.string_len                                    /* length */
            );
#else
            hexcode(notif.requestor_id, sizeof notif.requestor_id,
                  supl_req->requestor_id.requestor_id_string,  /* buffer */
                  supl_req->requestor_id.string_len            /* length */
            );
#endif /* #if (AMSS_VERSION==3200||AMSS_VERSION==20000) */
            LOC_LOGV("SUPL NI: requestor_id: %s len=%d", notif.requestor_id, supl_req->requestor_id.string_len);
         }
         else {
            LOC_LOGV("SUPL NI: requestor_id not present.");
         }

         // Encoding type
         if (supl_req->flags & RPC_LOC_NI_ENCODING_TYPE_PRESENT)
         {
            notif.text_encoding = convert_encoding_type(supl_req->datacoding_scheme);
            notif.requestor_id_encoding = convert_encoding_type(supl_req->datacoding_scheme);
         }
         else {
            notif.text_encoding = notif.requestor_id_encoding = GPS_ENC_UNKNOWN;
         }

         // Set default_response & notify_flags
         loc_ni_fill_notif_verify_type(&notif, ni_req->payload.rpc_loc_ni_event_payload_u_type_u.supl_req.notification_priv_type);

         // Privacy override handling
         if (ni_req->payload.rpc_loc_ni_event_payload_u_type_u.supl_req.notification_priv_type == RPC_LOC_NI_USER_PRIVACY_OVERRIDE)
         {
            loc_eng_mute_one_session();
         }

         break;

      default:
         LOC_LOGE("loc_ni_request_handler, unknown request event: %d", ni_req->event);
         return;
      }

      /* Log requestor ID and text for debugging */
      LOC_LOGI("Notification: notif_type: %d, timeout: %d, default_resp: %d", notif.ni_type, notif.timeout, notif.default_response);
      LOC_LOGI("              requestor_id: %s (encoding: %d)", notif.requestor_id, notif.requestor_id_encoding);
      LOC_LOGI("              text: %s text (encoding: %d)", notif.text, notif.text_encoding);
      if (notif.extras[0])
      {
         LOC_LOGI("              extras: %s", notif.extras);
      }

      /* For robustness, spawn a thread at this point to timeout to clear up the notification status, even though
       * the OEM layer in java does not do so.
       **/
      loc_eng_ni_data.response_time_left = 5 + (notif.timeout != 0 ? notif.timeout : LOC_NI_NO_RESPONSE_TIME);
      LOC_LOGI("Automatically sends 'no response' in %d seconds (to clear status)\n", loc_eng_ni_data.response_time_left);

      /* @todo may required when android framework issue is fixed
       * loc_eng_ni_data.callbacks_ref->create_thread_cb("loc_api_ni", loc_ni_thread_proc, NULL);
       */

      int rc = 0;
      rc = pthread_create(&loc_eng_ni_data.loc_ni_thread, NULL, loc_ni_thread_proc, NULL);
      if (rc)
      {
         LOC_LOGE("Loc NI thread is not created.\n");
      }
      rc = pthread_detach(loc_eng_ni_data.loc_ni_thread);
      if (rc)
      {
         LOC_LOGE("Loc NI thread is not detached.\n");
      }
      pthread_mutex_unlock(&loc_eng_ni_data.loc_ni_lock);

      /* Notify callback */
      if (loc_eng_data.ni_notify_cb != NULL)
      {
         loc_eng_data.ni_notify_cb(&notif);
      }
   }
}

/*===========================================================================

FUNCTION loc_ni_process_user_response

DESCRIPTION
   Handles user input from the UI

RETURN VALUE
   error code (0 for successful, -1 for error)

===========================================================================*/
int loc_ni_process_user_response(GpsUserResponseType userResponse)
{
   int rc=0;
   LOC_LOGD("NI response from UI: %d", userResponse);

   rpc_loc_ni_user_resp_e_type resp;
   switch (userResponse)
   {
   case GPS_NI_RESPONSE_ACCEPT:
      resp = RPC_LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT;
      break;
   case GPS_NI_RESPONSE_DENY:
      resp = RPC_LOC_NI_LCS_NOTIFY_VERIFY_DENY;
      break;
   case GPS_NI_RESPONSE_NORESP:
      resp = RPC_LOC_NI_LCS_NOTIFY_VERIFY_NORESP;
      break;
   default:
      return -1;
   }
   /* Turn of the timeout*/
   pthread_mutex_lock(&user_cb_data_mutex);
   loc_eng_ni_data.resp = resp;
   loc_eng_ni_data.user_response_received = TRUE;
   rc = pthread_cond_signal(&user_cb_arrived_cond);
   pthread_mutex_unlock(&user_cb_data_mutex);
   return 0;
}

/*===========================================================================

FUNCTION loc_eng_ni_callback

DESCRIPTION
   Loc API callback handler

RETURN VALUE
   error code (0 for success)

===========================================================================*/
int loc_eng_ni_callback (
      rpc_loc_event_mask_type               loc_event,              /* event mask           */
      const rpc_loc_event_payload_u_type*   loc_event_payload       /* payload              */
)
{
   int rc = 0;
   const rpc_loc_ni_event_s_type *ni_req = &loc_event_payload->rpc_loc_event_payload_u_type_u.ni_request;
   if (loc_event == RPC_LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST)
   {
      switch (ni_req->event)
      {
      case RPC_LOC_NI_EVENT_VX_NOTIFY_VERIFY_REQ:
         LOC_LOGI("VX Notification");
         loc_ni_request_handler("VX Notify", ni_req);
         break;

      case RPC_LOC_NI_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ:
         LOC_LOGI("UMTS CP Notification\n");
         loc_ni_request_handler("UMTS CP Notify", ni_req);
         break;

      case RPC_LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ:
         LOC_LOGI("SUPL Notification\n");
         loc_ni_request_handler("SUPL Notify", ni_req);
         break;

      default:
         LOC_LOGE("Unknown NI event: %x\n", (int) ni_req->event);
         break;
      }
   }
   return rc;
}

/*===========================================================================

FUNCTION loc_ni_thread_proc

===========================================================================*/
static void* loc_ni_thread_proc(void *threadid)
{
   int rc = 0;          /* return code from pthread calls */

   struct timeval present_time;
   struct timespec expire_time;

   LOC_LOGD("Starting Loc NI thread...\n");
   pthread_mutex_lock(&user_cb_data_mutex);
   /* Calculate absolute expire time */
   gettimeofday(&present_time, NULL);
   expire_time.tv_sec  = present_time.tv_sec;
   expire_time.tv_nsec = present_time.tv_usec * 1000;
   expire_time.tv_sec += loc_eng_ni_data.response_time_left;
   LOC_LOGD("loc_ni_thread_proc-Time out set for abs time %ld\n", (long) expire_time.tv_sec );

   while (!loc_eng_ni_data.user_response_received)
   {
      rc = pthread_cond_timedwait(&user_cb_arrived_cond, &user_cb_data_mutex, &expire_time);
      if (rc == ETIMEDOUT)
      {
         loc_eng_ni_data.resp = RPC_LOC_NI_LCS_NOTIFY_VERIFY_NORESP;
         LOC_LOGD("loc_ni_thread_proc-Thread time out after valting for specified time. Ret Val %d\n",rc );
         break;
      }
   }
      if (loc_eng_ni_data.user_response_received == TRUE)
      {
         LOC_LOGD("loc_ni_thread_proc-Java layer has sent us a user response and return value from "
                  "pthread_cond_timedwait = %d\n",rc );
         loc_eng_ni_data.user_response_received = FALSE; /* Reset the user response flag for the next session*/
      }
   loc_ni_respond(loc_eng_ni_data.resp, &loc_eng_ni_data.loc_ni_request);
   pthread_mutex_unlock(&user_cb_data_mutex);
   pthread_mutex_lock(&loc_eng_ni_data.loc_ni_lock);
   loc_eng_ni_data.notif_in_progress = FALSE;
   loc_eng_ni_data.response_time_left = 0;
   loc_eng_ni_data.current_notif_id = -1;
   pthread_mutex_unlock(&loc_eng_ni_data.loc_ni_lock);
   return NULL;
}

/*===========================================================================
FUNCTION    loc_eng_ni_init

DESCRIPTION
   This function initializes the NI interface

DEPENDENCIES
   NONE

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_eng_ni_init(GpsNiCallbacks *callbacks)
{
   LOC_LOGD("loc_eng_ni_init: entered.");

   if (!loc_eng_ni_data_init)
   {
      pthread_mutex_init(&loc_eng_ni_data.loc_ni_lock, NULL);
      loc_eng_ni_data_init = TRUE;
   }

   loc_eng_ni_data.notif_in_progress = FALSE;
   loc_eng_ni_data.current_notif_id = -1;
   loc_eng_ni_data.response_time_left = 0;
   loc_eng_ni_data.user_response_received = FALSE;

   srand(time(NULL));
   loc_eng_data.ni_notify_cb = callbacks->notify_cb;
}

/*===========================================================================
FUNCTION    loc_eng_ni_respond

DESCRIPTION
   This function sends an NI respond to the modem processor

DEPENDENCIES
   NONE

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_eng_ni_respond(int notif_id, GpsUserResponseType user_response)
{
   if (notif_id == loc_eng_ni_data.current_notif_id &&
         loc_eng_ni_data.notif_in_progress)
   {
      LOC_LOGI("loc_eng_ni_respond: send user response %d for notif %d", user_response, notif_id);
      loc_ni_process_user_response(user_response);
   }
   else {
      LOC_LOGE("loc_eng_ni_respond: notif_id %d mismatch or notification not in progress, response: %d",
            notif_id, user_response);
   }
}
