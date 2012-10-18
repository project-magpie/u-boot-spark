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

#ifndef __CONFIG_SPEAR13XX_H
#define __CONFIG_SPEAR13XX_H

#define CONFIG_ARCH_SPEAR13XX

/* AMBA PL022 SPI configurations */
#if defined(CONFIG_PL022_SPI)
	#define CONFIG_SYS_SPI_CLK		83000000
#endif

/* Designware Ethernet configurations */
#if defined(CONFIG_DESIGNWARE_ETH)
	#define CONFIG_DW_ALTDESCRIPTOR
#endif

/* ST SMI (Serial flash) configurations */
#if defined(CONFIG_ST_SMI)
	#define CONFIG_SYS_MAX_FLASH_BANKS	2
	#define CONFIG_SYS_FLASH_BASE		0xE6000000
	#define CONFIG_SYS_CS1_FLASH_BASE	0xE7000000
	#define CONFIG_SYS_FLASH_BANK_SIZE	0x01000000
	#define CONFIG_SYS_FLASH_ADDR_BASE	{CONFIG_SYS_FLASH_BASE, \
						CONFIG_SYS_CS1_FLASH_BASE}
	#define CONFIG_SYS_MAX_FLASH_SECT	128
#endif

/* PL011 UART configurations */
#if defined(CONFIG_PL011_SERIAL)
	#define CONFIG_PL011_CLOCK		(48 * 1000 * 1000)
	#define CONFIG_SYS_SERIAL0		0xE0000000
	#define CONFIG_PL01x_PORTS		{(void *)CONFIG_SYS_SERIAL0}

#endif

/* FSMC NAND UART configurations */
#if defined(CONFIG_NAND_FSMC)
	#define CONFIG_SYS_NAND_BASE		0xB0800000
#endif

/* Timer configurations */
#if defined(CONFIG_ARMV7_CA9LTIMER)
#endif

/* Internal memory address for spear3xx */
#define CONFIG_SYS_INIT_SP_ADDR			(0xB3800000 + 0x8000 - \
						GENERATED_GBL_DATA_SIZE)

#include <configs/spear.h>
#endif /* __CONFIG_SPEAR13XX_H */
