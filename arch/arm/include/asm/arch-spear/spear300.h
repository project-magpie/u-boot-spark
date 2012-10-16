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

#ifndef _ASM_ARCH_SPEAR300_H
#define _ASM_ARCH_SPEAR300_H

#include <asm/arch/spear3xx.h>

#define CONFIG_SYS_FSMC_BASE			0x94000000
#define CONFIG_SYS_NAND_CLE			(1 << 16)
#define CONFIG_SYS_NAND_ALE			(1 << 17)

/* RAS misc registers and respective bitmasks */
#define CONFIG_SYS_RAS_BASE			0x99000000
#define SPEAR300_RAS_REG1			CONFIG_SYS_RAS_BASE
	/* Bitmaks for REG1 are in spear3xx.h */
#define SPEAR300_RAS_REG2			(CONFIG_SYS_RAS_BASE + 0x04)
	#define SPEAR300_MODE_MSK		(0xF << 0)
	#define SPEAR300_MODE_NAND		(0x0 << 0)
	#define SPEAR300_MODE_NOR		(0x1 << 0)
	#define SPEAR300_MODE_PHOTOFRAME	(0x2 << 0)
	#define SPEAR300_MODE_LENDIPFONE	(0x3 << 0)
	#define SPEAR300_MODE_HENDIPFONE	(0x4 << 0)
	#define SPEAR300_MODE_LENDWIFIFONE	(0x5 << 0)
	#define SPEAR300_MODE_HENDWIFIFONE	(0x6 << 0)
	#define SPEAR300_MODE_ATAPABXWI2S	(0x7 << 0)
	#define SPEAR300_MODE_ATAPABXI2S	(0x8 << 0)
	#define SPEAR300_MODE_CAM1LCDW		(0xC << 0)
	#define SPEAR300_MODE_CAMULCD		(0xD << 0)
	#define SPEAR300_MODE_CAMULCDW		(0xE << 0)
	#define SPEAR300_MODE_CAM1LCD		(0xF << 0)

#define SPEAR3XX_FUNC_ENB_REG		SPEAR300_RAS_REG1

/* externs related to pinmux */
extern void spear300_pins_default(void);
extern void spear300_select_mode(u32 mode);
extern void spear300_enable_pins(u32 ip, u32 mode);

#endif
