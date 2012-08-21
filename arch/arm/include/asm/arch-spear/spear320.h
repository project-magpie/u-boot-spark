/*
 * (C) Copyright 2012
 * Vipin Kumar, STMicroelectronics, <vipin.kumar@st.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _ASM_ARCH_SPEAR320_H
#define _ASM_ARCH_SPEAR320_H

#include <asm/arch/spear3xx.h>

#define CONFIG_SPEAR_EMIBASE			0x40000000
#define CONFIG_SYS_FSMC_BASE			0x4C000000
#define CONFIG_SYS_NAND_CLE			(1 << 16)
#define CONFIG_SYS_NAND_ALE			(1 << 17)

#define CONFIG_SYS_MACB0_BASE			0xAA000000
#define CONFIG_SYS_MACB1_BASE			0xAB000000
#define CONFIG_SPEAR_RASBASE			0xB3000000

#endif
