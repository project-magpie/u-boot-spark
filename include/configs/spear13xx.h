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

/* Internal memory address for spear13xx */
#define CONFIG_SYS_INIT_SP_ADDR			(0xB3800000 + 0x8000 - \
						GENERATED_GBL_DATA_SIZE)

/* SPL configurations */
#if defined(CONFIG_SPL)
	#if !defined(CONFIG_SPL_TEXT_BASE)
		#define CONFIG_SPL_TEXT_BASE		0xB3801504
	#endif
	#if !defined(CONFIG_SYS_SNOR_BOOT_BASE)
		#define CONFIG_SYS_SNOR_BOOT_BASE	0xE6010000
	#endif
	#if !defined(CONFIG_SYS_NAND_BOOT_BASE)
		#define CONFIG_SYS_NAND_BOOT_BASE	0x00080000
	#endif
	#if !defined(CONFIG_SYS_PNOR_BOOT_BASE)
		#define CONFIG_SYS_PNOR_BOOT_BASE	0xA0020000
	#endif
	#if !defined(CONFIG_SYS_MMC_BOOT_FILE)
		#define CONFIG_SYS_MMC_BOOT_FILE	"u-boot.img"
	#endif

	#define CONFIG_SPEAR13XX_ARM2PHOLD		0xB3800600
	#define CONFIG_SPEAR13XX_ARM2STACK		0xB3800100

	#define CONFIG_SPL_NO_CPU_SUPPORT_CODE
	#define CONFIG_SPL_LIBCOMMON_SUPPORT
	#define CONFIG_SPL_LIBGENERIC_SUPPORT
	#define CONFIG_SPL_MTD_SUPPORT
	#define CONFIG_SPL_START_S_PATH	"arch/arm/cpu/armv7/spear13xx"
	#define CONFIG_SPL_LDSCRIPT	\
			"arch/arm/cpu/armv7/spear13xx/u-boot-spl.lds"

	#if defined(CONFIG_DDR_ECC_ENABLE)
		#define CONFIG_C3
		#define CONFIG_SPL_MISC_SUPPORT
	#endif

	#if defined(CONFIG_DDR_MT41J64M16) || \
		defined(CONFIG_DDR_MT41J256M8) || \
		defined(CONFIG_DDR_H5TQ2G63BFRPBC) || \
		defined(CONFIG_DDR_K4B2G1646CK0) || \
		defined(CONFIG_DDR_MT41J128M16)

		#define CONFIG_DDR3
	#elif defined(CONFIG_DDR_MT47H128M16)

		#define CONFIG_DDR2
	#else
		#error Define a supported DDR
	#endif
#endif

#include <configs/spear.h>
#endif /* __CONFIG_SPEAR13XX_H */
