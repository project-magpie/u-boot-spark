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

#ifndef _ASM_ARCH_SPEAR310_H
#define _ASM_ARCH_SPEAR310_H

#include <asm/arch/spear3xx.h>

#define CONFIG_SYS_FSMC_BASE			0x44000000
#define CONFIG_SYS_NAND_CLE			(1 << 17)
#define CONFIG_SYS_NAND_ALE			(1 << 16)

#define CONFIG_SPEAR_EMIBASE			0x4F000000
#define CONFIG_SYS_MACB0_BASE			0xB0000000
#define CONFIG_SYS_MACB1_BASE			0xB0800000
#define CONFIG_SYS_MACB2_BASE			0xB1000000
#define CONFIG_SYS_MACB3_BASE			0xB1800000
#define CONFIG_SPEAR_RASBASE			0xB4000000

/* SPEAr310 RAS misc space registers and bitmasks */
#define SPEAR310_BOOT_REG		(CONFIG_SPEAR_RASBASE + 0x0)
	#define SPEAR310_BOOTSHFT		0x0
	#define SPEAR310_BOOTMASK		0x7
	#define SPEAR310_USBBOOT		0x3
	#define SPEAR310_NANDBOOT		0x2
	#define SPEAR310_PNORBOOT		0x1
	#define SPEAR310_SNORBOOT		0x0
	#define SPEAR310_EMIBW_SHFT		0x3
	#define SPEAR310_EMIBW_MASK		0x18
	#define SPEAR310_EMIBW_8		0x0
	#define SPEAR310_EMIBW_16		0x1
	#define SPEAR310_EMIBW_32		0x2
#define SPEAR310_FUNCENB_REG		(CONFIG_SPEAR_RASBASE + 0x8)

#define SPEAR310_SMII_REG		(CONFIG_SPEAR_RASBASE + 0xC)
	#define SPEAR310_SMII_PHY_SHIFT		0x0
	#define SPEAR310_SMII_PHY_MASK		0x3

#define SNOR_BOOT_SUPPORTED			1
#define NAND_BOOT_SUPPORTED			1
#define PNOR_BOOT_SUPPORTED			1
#define USBD_BOOT_SUPPORTED			1
#define TFTP_BOOT_SUPPORTED			0
#define UART_BOOT_SUPPORTED			0
#define MMC_BOOT_SUPPORTED			0
#define SPI_BOOT_SUPPORTED			0
#define I2C_BOOT_SUPPORTED			0

#define SPEAR3XX_FUNC_ENB_REG		SPEAR310_FUNCENB_REG

/* externs related to pinmux */
extern void spear310_pins_default(void);
extern void spear310_enable_pins(u32 ip, u32 mode);

#endif
