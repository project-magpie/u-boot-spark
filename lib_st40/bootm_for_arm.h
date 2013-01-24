/*
 * Copyright (C) 2013 STMicroelectronics Ltd.
 *
 * Sean McGoogan <Sean.McGoogan@st.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __LIB_ST40_BOOTM_FOR_ARM_H
#define __LIB_ST40_BOOTM_FOR_ARM_H


#include <image.h>


/*
 * Function to boot ARM linux, from a ST40 core.
 */
extern int do_bootm_armlinux
	(int flag, int argc, char *argv[], bootm_headers_t *images);


/*
 * symbols exported from "lib_st40/armlinuxprep.S" (ARM code)
 */
extern volatile uint32_t armKernelEntryAddrPtr		__asm__ ("armKernelEntryAddrPtr");
extern volatile uint32_t armKernelSecondaryEntryAddrPtr	__asm__ ("armKernelSecondaryEntryAddrPtr");
extern volatile uint32_t atagsAddrPtr			__asm__ ("atagsAddrPtr");
extern void	 * const armLinuxPrep			__asm__ ("armLinuxPrep");

/*
 * symbols exported from "lib_st40/stxp70armboothelper.c" (STxP70 code)
 */
extern const unsigned char stxp70ARMBootHelperCode_bin[];
extern const unsigned int stxp70ARMBootHelperCode_bin_len;


#endif	/* __LIB_ST40_BOOTM_FOR_ARM_H */
