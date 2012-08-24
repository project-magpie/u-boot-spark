/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Microelectronics, vipin.kumar@st.com
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _ASM_ARCH_PINMUX_H
#define _ASM_ARCH_PINMUX_H

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>

static inline void pinmux_maskval(ulong reg, u32 mask, u32 val)
{
	u32 temp = readl((u32 *)reg);
	temp &= ~mask;
	temp |= val & mask;
	writel(temp, (u32 *)reg);
}

enum pinmux_ip {
	PMX_UART0,
	PMX_UART0_EXT,
	PMX_UART1,
	PMX_UART1_MDM,
	PMX_UART2,
	PMX_UART3,
	PMX_UART4,
	PMX_UART5,
	PMX_I2C0,
	PMX_I2C1,
	PMX_I2C2,
	PMX_SSP0,
	PMX_SSP1,
	PMX_SSP2,
	PMX_MII0,
	PMX_MII2,
	PMX_NAND,
	PMX_SDMMC,
	PMX_EMI,
};

/* UART1 modem modes */
#define PMX_UART1_MDM_2_7	1
#define PMX_UART1_MDM_31_36	2
#define PMX_UART1_MDM_34_45	3
#define PMX_UART1_MDM_80_85	4

/* I2C1 modes */
#define PMX_I2C1_8_9		1
#define PMX_I2C1_98_99		2

/* I2C2 modes */
#define PMX_I2C2_0_1		1
#define PMX_I2C2_2_3		2
#define PMX_I2C2_19_20		3
#define PMX_I2C2_75_76		4
#define PMX_I2C2_96_97		5

/* SSP1 modes */
#define PMX_SSP1_17_20		1
#define PMX_SSP1_36_39		2
#define PMX_SSP1_48_51		3
#define PMX_SSP1_65_68		4
#define PMX_SSP1_94_97		5

/* SSP2 modes */
#define PMX_SSP2_13_16		1
#define PMX_SSP2_32_35		2
#define PMX_SSP2_44_47		3
#define PMX_SSP2_61_64		4
#define PMX_SSP2_90_93		5

/* MII0 modes */
#define PMX_MII0_SMII		1
#define PMX_MII0_RMII		2

/* NAND modes */
#define PMX_NAND_8BIT		1
#define PMX_NAND_16BIT		2
#define PMX_NAND_2CHIP		3
#define PMX_NAND_4CHIP		4

/* SDMMC modes */
#define PMX_SDMMC_LED		1
#define PMX_SDMMC_CD12		2
#define PMX_SDMMC_CD51		3
#define PMX_SDMMC_4BIT		4
#define PMX_SDMMC_8BIT		5

#endif
