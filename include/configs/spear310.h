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

#ifndef __CONFIG_SPEAR310_H
#define __CONFIG_SPEAR310_H

#define CONFIG_SOC_SPEAR310

/* PL011 configs */
#if defined(CONFIG_PL011_SERIAL)
	#define CONFIG_SYS_SERIAL0		0xD0000000
	#define CONFIG_SYS_SERIAL1		0xB2000000
	#define CONFIG_SYS_SERIAL2		0xB2080000
	#define CONFIG_SYS_SERIAL3		0xB2100000
	#define CONFIG_SYS_SERIAL4		0xB2180000
	#define CONFIG_SYS_SERIAL5		0xB2200000

	#if (CONFIG_CONS_INDEX)
		#define CONFIG_PL011_CLOCK		(83 * 1000 * 1000)
	#else
		#define CONFIG_PL011_CLOCK		(48 * 1000 * 1000)
	#endif

	#define CONFIG_PL01x_PORTS		{(void *)CONFIG_SYS_SERIAL0, \
						(void *)CONFIG_SYS_SERIAL1, \
						(void *)CONFIG_SYS_SERIAL2, \
						(void *)CONFIG_SYS_SERIAL3, \
						(void *)CONFIG_SYS_SERIAL4, \
						(void *)CONFIG_SYS_SERIAL5 }
#endif

/* CFI driver (Parallel flash) configs */
#if defined(CONFIG_FLASH_CFI_DRIVER)
	#define CONFIG_SYS_FLASH_PROTECTION
	#define CONFIG_SYS_FLASH_BASE		0x50000000
	#define CONFIG_SYS_CS1_FLASH_BASE	0x60000000
	#define CONFIG_SYS_CS2_FLASH_BASE	0x70000000
	#define CONFIG_SYS_CS3_FLASH_BASE	0x80000000
	#define CONFIG_SYS_CS4_FLASH_BASE	0x90000000
	#define CONFIG_SYS_CS5_FLASH_BASE	0xA0000000
	#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE,   \
						CONFIG_SYS_CS1_FLASH_BASE, \
						CONFIG_SYS_CS2_FLASH_BASE, \
						CONFIG_SYS_CS3_FLASH_BASE, \
						CONFIG_SYS_CS4_FLASH_BASE, \
						CONFIG_SYS_CS5_FLASH_BASE }
	#define CONFIG_SYS_MAX_FLASH_BANKS	6
#endif

/* FSMC NAND configs */
#if defined(CONFIG_NAND_FSMC)
	#define CONFIG_SYS_NAND_BASE			0x40000000
#endif

#include <configs/spear3xx.h>
#endif /* __CONFIG_SPEAR310_H */
