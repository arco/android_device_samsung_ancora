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
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>

#include <rpc/rpc.h>
#include <loc_api_rpc_glue.h>

#include <loc_eng.h>

#define LOG_TAG "libloc"
#include <utils/Log.h>

// comment this out to enable logging
// #undef LOGD
// #define LOGD(...) {}

#define LOC_XTRA_INJECT_DEFAULT_TIMEOUT (3100)
#define XTRA_BLOCK_SIZE                 (1024)

static int qct_loc_eng_xtra_init (GpsXtraCallbacks* callbacks);
static int qct_loc_eng_inject_xtra_data(char* data, int length);
static int qct_loc_eng_inject_xtra_data_proxy(char* data, int length);

const GpsXtraInterface sLocEngXTRAInterface =
{
    sizeof(GpsXtraInterface),
    qct_loc_eng_xtra_init,
    /* qct_loc_eng_inject_xtra_data, */
    qct_loc_eng_inject_xtra_data_proxy, // This func buffers xtra data if GPS is in-session
};

/*===========================================================================
FUNCTION    qct_loc_eng_xtra_init

DESCRIPTION
   Initialize XTRA module.

DEPENDENCIES
   N/A

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static int qct_loc_eng_xtra_init (GpsXtraCallbacks* callbacks)
{
   rpc_loc_event_mask_type event;
   loc_eng_xtra_data_s_type *xtra_module_data_ptr;

   xtra_module_data_ptr = &loc_eng_data.xtra_module_data;
   xtra_module_data_ptr->download_request_cb = callbacks->download_request_cb;

   return 0;
}

/*===========================================================================
FUNCTION    qct_loc_eng_inject_xtra_data

DESCRIPTION
   Injects XTRA file into the engine.

DEPENDENCIES
   N/A

RETURN VALUE
   0: success
   error code > 0

SIDE EFFECTS
   N/A

===========================================================================*/
static int qct_loc_eng_inject_xtra_data_one(char* data, int length)
{
   int     rpc_ret_val = RPC_LOC_API_GENERAL_FAILURE;
   boolean ret_val = 0;
   int     total_parts;
   uint8   part;
   uint16  part_len;
   uint16  len_injected;
   rpc_loc_ioctl_data_u_type            ioctl_data;
   rpc_loc_ioctl_e_type                 ioctl_type = RPC_LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA;
   rpc_loc_predicted_orbits_data_s_type *predicted_orbits_data_ptr;

   LOC_LOGD("qct_loc_eng_inject_xtra_data, xtra size = %d, data ptr = 0x%x\n", length, (int) data);

   predicted_orbits_data_ptr = &ioctl_data.rpc_loc_ioctl_data_u_type_u.predicted_orbits_data;
   predicted_orbits_data_ptr->format_type = RPC_LOC_PREDICTED_ORBITS_XTRA;
   predicted_orbits_data_ptr->total_size = length;
   total_parts = (length - 1) / XTRA_BLOCK_SIZE + 1;
   predicted_orbits_data_ptr->total_parts = total_parts;

   len_injected = 0; // O bytes injected
   ioctl_data.disc = ioctl_type;

   // XTRA injection starts with part 1
   for (part = 1; part <= total_parts; part++)
   {
      predicted_orbits_data_ptr->part = part;
      predicted_orbits_data_ptr->part_len = XTRA_BLOCK_SIZE;
      if (XTRA_BLOCK_SIZE > (length - len_injected))
      {
         predicted_orbits_data_ptr->part_len = length - len_injected;
      }
      predicted_orbits_data_ptr->data_ptr.data_ptr_len = predicted_orbits_data_ptr->part_len;
      predicted_orbits_data_ptr->data_ptr.data_ptr_val = data + len_injected;

      LOC_LOGD("qct_loc_eng_inject_xtra_data, part %d/%d, len = %d, total = %d\n",
            predicted_orbits_data_ptr->part,
            total_parts,
            predicted_orbits_data_ptr->part_len,
            len_injected);

      if (part < total_parts)
      {
         // No callback in this case
         rpc_ret_val = loc_ioctl (loc_eng_data.client_handle,
                                  ioctl_type,
                                  &ioctl_data);

         if (rpc_ret_val != RPC_LOC_API_SUCCESS)
         {
            ret_val = EIO; // return error
            LOC_LOGE("loc_ioctl for xtra error: %s\n", loc_get_ioctl_status_name(rpc_ret_val));
            break;
         }
      }
      else // part == total_parts
      {
         // Last part injection, will need to wait for callback
         if (!loc_eng_ioctl(loc_eng_data.client_handle,
                                  ioctl_type,
                                  &ioctl_data,
                                  LOC_XTRA_INJECT_DEFAULT_TIMEOUT,
                                  NULL))
         {
            ret_val = EIO;
            LOC_LOGE("loc_eng_ioctl for xtra error\n");
         }
         break; // done with injection
      }

      len_injected += predicted_orbits_data_ptr->part_len;
      LOC_LOGD("loc_ioctl XTRA injected length: %d\n", len_injected);
   }

   return ret_val;
}
static int qct_loc_eng_inject_xtra_data(char* data, int length)
{
   int rc = EIO;
   int tries = 0;
   while (tries++ < 3) {
      rc = qct_loc_eng_inject_xtra_data_one(data, length);
      if (!rc) break;
   }
   return rc;
}

/*===========================================================================
FUNCTION    loc_eng_inject_xtra_data_in_buffer

DESCRIPTION
   Injects buffered XTRA file into the engine and clears the buffer.

DEPENDENCIES
   N/A

RETURN VALUE
   0: success
   >0: failure

SIDE EFFECTS
   N/A

===========================================================================*/
int loc_eng_inject_xtra_data_in_buffer()
{
   int rc = 0;
   char *data;
   int length;

   pthread_mutex_lock(&loc_eng_data.xtra_module_data.lock);

   data = loc_eng_data.xtra_module_data.xtra_data_for_injection;
   length = loc_eng_data.xtra_module_data.xtra_data_len;

   loc_eng_data.xtra_module_data.xtra_data_for_injection = NULL;
   loc_eng_data.xtra_module_data.xtra_data_len = 0;

   pthread_mutex_unlock(&loc_eng_data.xtra_module_data.lock);

   if (data)
   {
      if (qct_loc_eng_inject_xtra_data(data, length))
      {
         // FIXME gracefully handle injection error
         LOC_LOGE("XTRA injection failed.");
         rc = -1;
      }

      free(data);
   }

   return rc;
}

/*===========================================================================
FUNCTION    qct_loc_eng_inject_xtra_data_proxy

DESCRIPTION
   Injects XTRA file into the engine but buffers the data if engine is busy.

DEPENDENCIES
   N/A

RETURN VALUE
   0: success
   >0: failure

SIDE EFFECTS
   N/A

===========================================================================*/
static int qct_loc_eng_inject_xtra_data_proxy(char* data, int length)
{
   if (!data || !length) return -1;

   pthread_mutex_lock(&loc_eng_data.xtra_module_data.lock);

   char *buf = (char*) malloc(length);
   if (buf)
   {
      memcpy(buf, data, length);
      loc_eng_data.xtra_module_data.xtra_data_for_injection = buf;
      loc_eng_data.xtra_module_data.xtra_data_len = length;
   }

   pthread_mutex_unlock(&loc_eng_data.xtra_module_data.lock);

   if (loc_eng_data.engine_status != GPS_STATUS_ENGINE_ON)
   {
      pthread_cond_signal(&loc_eng_data.deferred_action_cond);
   }

   return 0;
}
