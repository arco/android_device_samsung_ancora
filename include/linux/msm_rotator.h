/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, and the entire permission notice in its entirety,
 *    including the disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * ALTERNATIVELY, this product may be distributed under the terms of
 * the GNU General Public License, version 2, in which case the provisions
 * of the GPL version 2 are required INSTEAD OF the BSD license.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ALL OF
 * WHICH ARE HEREBY DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF NOT ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 */

#ifndef __MSM_ROTATOR_H__

#include <linux/types.h>
#include <linux/msm_mdp.h>

#define MSM_ROTATOR_IOCTL_MAGIC 'R'

#define MSM_ROTATOR_IOCTL_START   \
		_IOWR(MSM_ROTATOR_IOCTL_MAGIC, 1, struct msm_rotator_img_info)
#define MSM_ROTATOR_IOCTL_ROTATE   \
		_IOW(MSM_ROTATOR_IOCTL_MAGIC, 2, struct msm_rotator_data_info)
#define MSM_ROTATOR_IOCTL_FINISH   \
		_IOW(MSM_ROTATOR_IOCTL_MAGIC, 3, int)

#define ROTATOR_VERSION_01	0xA5B4C301

enum rotator_clk_type {
	ROTATOR_CORE_CLK,
	ROTATOR_AXI_CLK,
	ROTATOR_PCLK,
	ROTATOR_IMEM_CLK
};

struct msm_rotator_img_info {
	unsigned int session_id;
	struct msmfb_img  src;
	struct msmfb_img  dst;
	struct mdp_rect src_rect;
	unsigned int    dst_x;
	unsigned int    dst_y;
	unsigned char   rotations;
	int enable;
};

struct msm_rotator_data_info {
	int session_id;
	struct msmfb_data src;
	struct msmfb_data dst;
	unsigned int version_key;
	struct msmfb_data src_chroma;
	struct msmfb_data dst_chroma;
};

struct msm_rot_clocks {
	const char *clk_name;
	enum rotator_clk_type clk_type;
	unsigned int clk_rate;
};

struct msm_rotator_platform_data {
	unsigned int number_of_clocks;
	unsigned int hardware_version_number;
	struct msm_rot_clocks *rotator_clks;
	const char *regulator_name;
};
#endif

