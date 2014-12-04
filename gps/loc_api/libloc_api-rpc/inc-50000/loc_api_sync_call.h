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

#ifndef LOC_API_CB_SYNC_H
#define LOC_API_CB_SYNC_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "loc_api_rpc_glue.h"
#define LOC_SYNC_CALL_BUFFER_SIZE 8

typedef struct {
   pthread_mutex_t                lock;

   /* Client ID */
   rpc_loc_client_handle_type     loc_handle;

   /* Callback waiting conditional variable */
   pthread_cond_t                 loc_cb_arrived_cond;

   /* Callback waiting data block, protected by loc_cb_data_mutex */
   boolean                        loc_cb_is_selected;            /* is cb selected? */
   boolean                        loc_cb_is_waiting;             /* is waiting?     */
   boolean                        loc_cb_has_arrived;            /* callback has arrived */
   rpc_loc_event_mask_type        loc_cb_wait_event_mask;        /* event to wait for */
   rpc_loc_ioctl_e_type           ioctl_type;                    /* ioctl to wait for */
   rpc_loc_event_payload_u_type   loc_cb_received_payload;       /* received payload */
   rpc_loc_event_mask_type        loc_cb_received_event_mask;    /* received event   */
} loc_sync_call_data_s_type;

typedef struct {
   pthread_mutex_t                lock;
   int                            size;
   boolean                        in_use;  /* at least one sync call is active */
   boolean                        slot_in_use[LOC_SYNC_CALL_BUFFER_SIZE];
   loc_sync_call_data_s_type      slots[LOC_SYNC_CALL_BUFFER_SIZE];
} loc_sync_call_data_array_s_type;

/* Init function */
extern void loc_api_sync_call_init();

/* Select the callback to wait for, e.g., IOCTL REPORT */
extern int loc_api_select_callback(
      rpc_loc_client_handle_type       loc_handle,           /* Client handle */
      rpc_loc_event_mask_type          event_mask,           /* Event mask to wait for */
      rpc_loc_ioctl_e_type             ioctl_type            /* IOCTL type to wait for */
);

/* Wait for the call back after an API call, returns 0 if successful */
extern int loc_api_wait_callback(
      int select_id,        /* ID from loc_select_callback() */
      int timeout_seconds,  /* Timeout in this number of seconds  */
      rpc_loc_event_payload_u_type     *callback_payload,    /* Pointer to callback payload buffer, can be NULL */
      rpc_loc_ioctl_callback_s_type    *ioctl_payload        /* Pointer to IOCTL payload, can be NULL */
);

/* Process Loc API callbacks to wake up blocked user threads */
extern void loc_api_callback_process_sync_call(
      rpc_loc_client_handle_type            loc_handle,             /* handle of the client */
      rpc_loc_event_mask_type               loc_event,              /* event mask           */
      const rpc_loc_event_payload_u_type*   loc_event_payload       /* payload              */
);

/* Reentrant synchronous IOCTL call, using Loc API return code */
extern int loc_api_sync_ioctl
(
      rpc_loc_client_handle_type           handle,
      rpc_loc_ioctl_e_type                 ioctl_type,
      rpc_loc_ioctl_data_u_type*           ioctl_data_ptr,
      uint32                               timeout_msec,
      rpc_loc_ioctl_callback_s_type       *cb_data_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* LOC_API_CB_SYNC_H */
