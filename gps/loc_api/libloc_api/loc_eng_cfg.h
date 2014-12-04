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

#ifndef LOC_ENG_CFG_H
#define LOC_ENG_CFG_H

#define LOC_MAX_PARAM_NAME                 36
#define LOC_MAX_PARAM_STRING               80
#define LOC_MAX_PARAM_LINE                 80

#define GPS_CONF_FILE            "/etc/gps.conf"            /* primary */

/*=============================================================================
 *
 *                        MODULE TYPE DECLARATION
 *
 *============================================================================*/
typedef struct
{
  char                           param_name[LOC_MAX_PARAM_NAME];
  void                          *param_ptr;
  char                           param_type;  /* 's' for number; 's' for string */
} loc_param_s_type;

/* GPS.conf support */
typedef struct loc_gps_cfg_s
{
  unsigned long  INTERMEDIATE_POS;
  unsigned long  ACCURACY_THRES;
  unsigned long  ENABLE_WIPER;
  unsigned long  DEBUG_LEVEL;
  // char           string_val[LOC_MAX_PARAM_STRING + 1]; /* An example string value */
} loc_gps_cfg_s_type;

/*=============================================================================
 *
 *                          MODULE EXTERNAL DATA
 *
 *============================================================================*/

extern loc_gps_cfg_s_type gps_conf;

/*=============================================================================
 *
 *                       MODULE EXPORTED FUNCTIONS
 *
 *============================================================================*/
extern void loc_read_gps_conf(void);

#endif /* LOC_ENG_CFG_H */
