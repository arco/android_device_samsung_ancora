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
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>

#include <rpc/rpc.h>
#include <loc_api_rpc_glue.h>

#include <hardware/gps.h>

#include <loc_eng.h>

#define LOG_TAG "libloc"
#include <utils/Log.h>

#include <loc_api_sync_call.h>

// comment this out to enable logging
// #undef LOGD
// #define LOGD(...) {}

/*===========================================================================

FUNCTION    loc_eng_ioctl

DESCRIPTION
   This function calls loc_ioctl and waits for the callback result before
   returning back to the user.

DEPENDENCIES
   N/A

RETURN VALUE
   TRUE                 if successful
   FALSE                if failed

SIDE EFFECTS
   N/A

===========================================================================*/
boolean loc_eng_ioctl
(
      rpc_loc_client_handle_type           handle,
      rpc_loc_ioctl_e_type                 ioctl_type,
      rpc_loc_ioctl_data_u_type*           ioctl_data_ptr,
      uint32                               timeout_msec,
      rpc_loc_ioctl_callback_s_type       *cb_data_ptr
)
{
   int ret_val = RPC_LOC_API_SUCCESS;

   LOC_LOGD("loc_eng_ioctl called: client = %d, ioctl_type = %s\n", (int32) handle,
         loc_get_ioctl_type_name(ioctl_type));

   ret_val = loc_api_sync_ioctl(handle, ioctl_type, ioctl_data_ptr, timeout_msec, cb_data_ptr);

   LOC_LOGD("loc_eng_ioctl result: client = %d, ioctl_type = %s, %s\n",
         (int32) handle,
         loc_get_ioctl_type_name(ioctl_type),
         loc_get_ioctl_status_name(ret_val) );

   return ret_val == RPC_LOC_API_SUCCESS;
}
