/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
 */

#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>

#include <rpc/rpc.h>
#include <loc_api_rpc_glue.h>
#include "loc_api_sync_call.h"

/* Logging */
#define LOG_TAG "loc_api_rpc_glue"
// #define LOG_NDDEBUG 0
#include <utils/Log.h>

/***************************************************************************
 *                 DATA FOR ASYNCHRONOUS RPC PROCESSING
 **************************************************************************/
loc_sync_call_data_array_s_type loc_sync_data;

/*===========================================================================

FUNCTION    loc_api_sync_call_init

DESCRIPTION
   Initialize this module

DEPENDENCIES
   N/A

RETURN VALUE
   none

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_api_sync_call_init()
{
   pthread_mutex_init(&loc_sync_data.lock, NULL);
   pthread_mutex_lock(&loc_sync_data.lock);

   loc_sync_data.size = LOC_SYNC_CALL_BUFFER_SIZE;
   loc_sync_data.in_use = FALSE;

   memset(loc_sync_data.slot_in_use, 0, sizeof loc_sync_data.slot_in_use);

   int i;
   for (i = 0; i < loc_sync_data.size; i++)
   {
      loc_sync_call_data_s_type *slot = &loc_sync_data.slots[i];

      pthread_mutex_init(&slot->lock, NULL);
      pthread_cond_init(&slot->loc_cb_arrived_cond, NULL);

      slot->loc_handle = -1;
      slot->loc_cb_is_selected = FALSE;       /* is cb selected? */
      slot->loc_cb_is_waiting  = FALSE;       /* is waiting?     */
      slot->loc_cb_has_arrived = FALSE;       /* callback has arrived */
      slot->loc_cb_wait_event_mask = 0;       /* event to wait   */
      slot->loc_cb_received_event_mask = 0;   /* received event   */
   }

   pthread_mutex_unlock(&loc_sync_data.lock);
}

/*===========================================================================

FUNCTION    loc_match_callback

DESCRIPTION
   Checks if an awaited event has arrived

RETURN VALUE
   TRUE                 arrived
   FALSE                not matching

===========================================================================*/
static boolean loc_match_callback(
      rpc_loc_event_mask_type             wait_mask,
      rpc_loc_ioctl_e_type                wait_ioctl,
      rpc_loc_event_mask_type             event_mask,
      const rpc_loc_event_payload_u_type  *callback_payload
)
{
   if ((event_mask & wait_mask) == 0) return FALSE;

   if (event_mask != RPC_LOC_EVENT_IOCTL_REPORT || wait_ioctl == 0 ||
         callback_payload->rpc_loc_event_payload_u_type_u.ioctl_report.type == wait_ioctl)
      return TRUE;

   return FALSE;
}

/*===========================================================================

FUNCTION    loc_api_callback_process_sync_call

DESCRIPTION
   Wakes up blocked API calls to check if the needed callback has arrived

DEPENDENCIES
   N/A

RETURN VALUE
   none

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_api_callback_process_sync_call(
      rpc_loc_client_handle_type            loc_handle,             /* handle of the client */
      rpc_loc_event_mask_type               loc_event,              /* event mask           */
      const rpc_loc_event_payload_u_type*   loc_event_payload       /* payload              */
)
{
   pthread_mutex_lock(&loc_sync_data.lock);

   if (!loc_sync_data.in_use)
   {
      pthread_mutex_unlock(&loc_sync_data.lock);
      return;
   }

   loc_sync_call_data_s_type *slot;
   boolean in_use = FALSE, consumed = FALSE;
   int i;

   for (i = 0; i < loc_sync_data.size && !consumed; i++)
   {
      loc_sync_call_data_s_type *slot = &loc_sync_data.slots[i];
      in_use |= loc_sync_data.slot_in_use[i];

      pthread_mutex_lock(&slot->lock);

      if (loc_sync_data.slot_in_use[i] &&
          slot->loc_handle == loc_handle &&
          loc_match_callback(slot->loc_cb_wait_event_mask, slot->ioctl_type, loc_event, loc_event_payload) &&
          !slot->loc_cb_has_arrived)
      {
         memcpy(&slot->loc_cb_received_payload, loc_event_payload, sizeof (rpc_loc_event_payload_u_type));
         consumed = TRUE;

         /* Received a callback while waiting, wake up thread to check it */
         if (slot->loc_cb_is_waiting)
         {
            slot->loc_cb_received_event_mask = loc_event;
            pthread_cond_signal(&slot->loc_cb_arrived_cond);
         }
         else {
            /* If callback arrives before wait, remember it */
            slot->loc_cb_has_arrived = TRUE;
         }
      }

      pthread_mutex_unlock(&slot->lock);
   }

   if (!in_use) {
      loc_sync_data.in_use = FALSE;
   }

   pthread_mutex_unlock(&loc_sync_data.lock);
}

/*===========================================================================

FUNCTION    loc_alloc_slot

DESCRIPTION
   Allocates a buffer slot for the synchronous API call

DEPENDENCIES
   N/A

RETURN VALUE
   Select ID (>=0)     : successful
   -1                  : buffer full

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_alloc_slot()
{
   int i, select_id = -1; /* no free buffer */

   pthread_mutex_lock(&loc_sync_data.lock);

   for (i = 0; i < loc_sync_data.size; i++)
   {
      if (!loc_sync_data.slot_in_use[i])
      {
         select_id = i;
         loc_sync_data.slot_in_use[i] = 1;
         loc_sync_data.in_use = TRUE;
         break;
      }
   }

   pthread_mutex_unlock(&loc_sync_data.lock);

   return select_id;
}

/*===========================================================================

FUNCTION    loc_free_slot

DESCRIPTION
   Frees a buffer slot after the synchronous API call

DEPENDENCIES
   N/A

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_free_slot(int select_id)
{
   pthread_mutex_lock(&loc_sync_data.lock);

   loc_sync_data.slot_in_use[select_id] = 0;

   int i;
   for (i = 0; i < loc_sync_data.size; i++)
   {
      if (loc_sync_data.slot_in_use[i]) break;
   }

   if (i >= loc_sync_data.size)
   {
      loc_sync_data.in_use = FALSE;
   }

   pthread_mutex_unlock(&loc_sync_data.lock);
}

/*===========================================================================

FUNCTION    loc_api_select_callback

DESCRIPTION
   Selects which callback or IOCTL event to wait for.

   The event_mask specifies the event(s). If it is RPC_LOC_EVENT_IOCTL_REPORT,
   then ioctl_type specifies the IOCTL event.

   If ioctl_type is non-zero, RPC_LOC_EVENT_IOCTL_REPORT is automatically added.

DEPENDENCIES
   N/A

RETURN VALUE
   Select ID (>=0)     : successful
   -1                  : out of buffer

SIDE EFFECTS
   N/A

===========================================================================*/
int loc_api_select_callback(
      rpc_loc_client_handle_type       loc_handle,           /* Client handle */
      rpc_loc_event_mask_type          event_mask,           /* Event mask to wait for */
      rpc_loc_ioctl_e_type             ioctl_type            /* IOCTL type to wait for */
)
{
   int select_id = loc_alloc_slot();

   if (select_id < 0)
   {
      LOGE("loc_select_callback: buffer full for this synchronous Loc API call, mask: 0x%x",
            (unsigned) event_mask);
      return -1;
   }

   loc_sync_call_data_s_type *slot = &loc_sync_data.slots[select_id];

   pthread_mutex_lock(&slot->lock);

   slot->loc_handle = loc_handle;
   slot->loc_cb_is_selected = TRUE;
   slot->loc_cb_is_waiting = FALSE;
   slot->loc_cb_has_arrived = FALSE;

   slot->loc_cb_wait_event_mask = event_mask;
   slot->ioctl_type = ioctl_type;
   if (ioctl_type) slot->loc_cb_wait_event_mask |= RPC_LOC_EVENT_IOCTL_REPORT;

   pthread_mutex_unlock(&slot->lock);

   return select_id;
}

/*===========================================================================

FUNCTION    loc_save_user_payload

DESCRIPTION
   Saves received payload into user data structures

RETURN VALUE
   None

===========================================================================*/
static void loc_save_user_payload(
      rpc_loc_event_payload_u_type  *user_cb_payload,
      rpc_loc_ioctl_callback_s_type *user_ioctl_buffer,
      const rpc_loc_event_payload_u_type  *received_cb_payload
)
{
   if (user_cb_payload)
   {
      memcpy(user_cb_payload, received_cb_payload,
            sizeof (rpc_loc_event_payload_u_type));
   }
   if (user_ioctl_buffer)
   {
      memcpy(user_ioctl_buffer,
            &received_cb_payload->rpc_loc_event_payload_u_type_u.ioctl_report,
            sizeof *user_ioctl_buffer);
   }
}

/*===========================================================================

FUNCTION    loc_api_wait_callback

DESCRIPTION
   Waits for a selected callback. The wait expires in timeout_seconds seconds.

   If the function is called before an existing wait has finished, it will
   immediately return EBUSY.

DEPENDENCIES
   N/A

RETURN VALUE
   RPC_LOC_API_SUCCESS              if successful (0)
   RPC_LOC_API_TIMEOUT              if timed out
   RPC_LOC_API_ENGINE_BUSY          if already in a wait
   RPC_LOC_API_INVALID_PARAMETER    if callback is not yet selected

SIDE EFFECTS
   N/A

===========================================================================*/
int loc_api_wait_callback(
      int select_id,        /* ID from loc_select_callback() */
      int timeout_seconds,  /* Timeout in this number of seconds  */
      rpc_loc_event_payload_u_type     *callback_payload,    /* Pointer to callback payload buffer, can be NULL */
      rpc_loc_ioctl_callback_s_type    *ioctl_payload        /* Pointer to IOCTL payload, can be NULL */
)
{
   if (select_id < 0 || select_id >= loc_sync_data.size || !loc_sync_data.slot_in_use[select_id])
   {
      LOGE("loc_wait_callback: invalid select_id: %d", select_id);
      return RPC_LOC_API_INVALID_PARAMETER;
   }

   loc_sync_call_data_s_type *slot = &loc_sync_data.slots[select_id];

   int ret_val = RPC_LOC_API_SUCCESS;  /* the return value of this function: 0 = no error */
   int rc;                             /* return code from pthread calls */

   struct timeval present_time;
   struct timespec expire_time;

   pthread_mutex_lock(&slot->lock);

   if (slot->loc_cb_has_arrived)
   {
      loc_save_user_payload(callback_payload, ioctl_payload, &slot->loc_cb_received_payload);
      pthread_mutex_unlock(&slot->lock);
      loc_free_slot(select_id);
      ret_val = RPC_LOC_API_SUCCESS;
      return ret_val;    /* success */
   }

   if (slot->loc_cb_is_waiting)
   {
      pthread_mutex_unlock(&slot->lock);
      loc_free_slot(select_id);
      ret_val = RPC_LOC_API_ENGINE_BUSY; /* busy, rejected */
      return ret_val;  /* exit */
   }

   /* Calculate absolute expire time */
   gettimeofday(&present_time, NULL);
   expire_time.tv_sec  = present_time.tv_sec;
   expire_time.tv_nsec = present_time.tv_usec * 1000;
   expire_time.tv_sec += timeout_seconds;

   /* Take new wait request */
   slot->loc_cb_is_waiting = TRUE;

   /* Waiting */
   rc = pthread_cond_timedwait(&slot->loc_cb_arrived_cond,
         &slot->lock, &expire_time);

   if (rc == ETIMEDOUT)
   {
      slot->loc_cb_is_waiting = FALSE;
      ret_val = RPC_LOC_API_TIMEOUT; /* Timed out */
   }
   else {
      /* Obtained the first awaited callback */
      slot->loc_cb_is_waiting = FALSE;     /* stop waiting */
      ret_val = RPC_LOC_API_SUCCESS;       /* Successful */
      loc_save_user_payload(callback_payload, ioctl_payload, &slot->loc_cb_received_payload);
   }

   pthread_mutex_unlock(&slot->lock);

   loc_free_slot(select_id);

   return ret_val;
}

/*===========================================================================

FUNCTION    loc_api_sync_ioctl

DESCRIPTION
   Synchronous IOCTL call (reentrant version)

DEPENDENCIES
   N/A

RETURN VALUE
   Loc API error code (0 = success)

SIDE EFFECTS
   N/A

===========================================================================*/
int loc_api_sync_ioctl
(
      rpc_loc_client_handle_type           handle,
      rpc_loc_ioctl_e_type                 ioctl_type,
      rpc_loc_ioctl_data_u_type*           ioctl_data_ptr,
      uint32                               timeout_msec,
      rpc_loc_ioctl_callback_s_type       *cb_data_ptr
)
{
   int                              rc = RPC_LOC_API_SUCCESS;
   int                              select_id;
   rpc_loc_ioctl_callback_s_type    callback_data;

   // Select the callback we are waiting for
   select_id = loc_api_select_callback(handle, 0, ioctl_type);

   if (select_id >= 0)
   {
      rc =  loc_ioctl(handle, ioctl_type, ioctl_data_ptr);
      LOGV("loc_api_sync_ioctl: select_id = %d, loc_ioctl returned %d\n", select_id, rc);

      if (rc != RPC_LOC_API_SUCCESS)
      {
         loc_free_slot(select_id);
      }
      else {
         // Wait for the callback of loc_ioctl
         if ((rc = loc_api_wait_callback(select_id, timeout_msec / 1000, NULL, &callback_data)) != 0)
         {
            // Callback waiting failed
            LOGE("loc_api_sync_ioctl: loc_api_wait_callback failed, returned %d (select id %d)\n", rc, select_id);
         }
         else
         {
            if (cb_data_ptr) memcpy(cb_data_ptr, &callback_data, sizeof *cb_data_ptr);
            if (callback_data.status != RPC_LOC_API_SUCCESS)
            {
               rc = callback_data.status;
               LOGE("loc_api_sync_ioctl: IOCTL result failed, result: %d (select id %d)\n", rc, select_id);
            }
         } /* wait callback */
      } /* loc_ioctl */
   } /* select id */

   return rc;
}


