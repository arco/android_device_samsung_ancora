/* include/linux/android_pmem.h
 *
 * Copyright (C) 2007 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _ANDROID_PMEM_H_
#define _ANDROID_PMEM_H_

#include <linux/fs.h>

#define PMEM_KERNEL_TEST_MAGIC 0xc0
#define PMEM_KERNEL_TEST_NOMINAL_TEST_IOCTL \
	_IO(PMEM_KERNEL_TEST_MAGIC, 1)
#define PMEM_KERNEL_TEST_ADVERSARIAL_TEST_IOCTL \
	_IO(PMEM_KERNEL_TEST_MAGIC, 2)
#define PMEM_KERNEL_TEST_HUGE_ALLOCATION_TEST_IOCTL \
	_IO(PMEM_KERNEL_TEST_MAGIC, 3)
#define PMEM_KERNEL_TEST_FREE_UNALLOCATED_TEST_IOCTL \
	_IO(PMEM_KERNEL_TEST_MAGIC, 4)
#define PMEM_KERNEL_TEST_LARGE_REGION_NUMBER_TEST_IOCTL \
	_IO(PMEM_KERNEL_TEST_MAGIC, 5)

#define PMEM_IOCTL_MAGIC 'p'
#define PMEM_GET_PHYS		_IOW(PMEM_IOCTL_MAGIC, 1, unsigned int)
#define PMEM_MAP		_IOW(PMEM_IOCTL_MAGIC, 2, unsigned int)
#define PMEM_GET_SIZE		_IOW(PMEM_IOCTL_MAGIC, 3, unsigned int)
#define PMEM_UNMAP		_IOW(PMEM_IOCTL_MAGIC, 4, unsigned int)
/* This ioctl will allocate pmem space, backing the file, it will fail
 * if the file already has an allocation, pass it the len as the argument
 * to the ioctl */
#define PMEM_ALLOCATE		_IOW(PMEM_IOCTL_MAGIC, 5, unsigned int)
/* This will connect a one pmem file to another, pass the file that is already
 * backed in memory as the argument to the ioctl
 */
#define PMEM_CONNECT		_IOW(PMEM_IOCTL_MAGIC, 6, unsigned int)
/* Returns the total size of the pmem region it is sent to as a pmem_region
 * struct (with offset set to 0). 
 */
#define PMEM_GET_TOTAL_SIZE	_IOW(PMEM_IOCTL_MAGIC, 7, unsigned int)
/* Revokes gpu registers and resets the gpu.  Pass a pointer to the
 * start of the mapped gpu regs (the vaddr returned by mmap) as the argument.
 */
#define HW3D_REVOKE_GPU		_IOW(PMEM_IOCTL_MAGIC, 8, unsigned int)
#define HW3D_GRANT_GPU		_IOW(PMEM_IOCTL_MAGIC, 9, unsigned int)
#define HW3D_WAIT_FOR_INTERRUPT	_IOW(PMEM_IOCTL_MAGIC, 10, unsigned int)

#define PMEM_CLEAN_INV_CACHES	_IOW(PMEM_IOCTL_MAGIC, 11, unsigned int)
#define PMEM_CLEAN_CACHES	_IOW(PMEM_IOCTL_MAGIC, 12, unsigned int)
#define PMEM_INV_CACHES		_IOW(PMEM_IOCTL_MAGIC, 13, unsigned int)

#define PMEM_GET_FREE_SPACE	_IOW(PMEM_IOCTL_MAGIC, 14, unsigned int)
#define PMEM_ALLOCATE_ALIGNED	_IOW(PMEM_IOCTL_MAGIC, 15, unsigned int)
struct pmem_region {
	unsigned long offset;
	unsigned long len;
};

struct pmem_addr {
	unsigned long vaddr;
	unsigned long offset;
	unsigned long length;
};

struct pmem_freespace {
	unsigned long total;
	unsigned long largest;
};

struct pmem_allocation {
	unsigned long size;
	unsigned int align;
};


#endif //_ANDROID_PPP_H_

