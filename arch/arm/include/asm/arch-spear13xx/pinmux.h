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

static inline void pinmux_set_bit(int nr, unsigned long addr)
{
	__set_bit(nr, (volatile unsigned long *)addr);
}

static inline void pinmux_clear_bit(int nr, unsigned long addr)
{
	__clear_bit(nr, (volatile unsigned long *)addr);
}

static inline int pinmux_test_bit(int nr, unsigned long addr)
{
	return __test_bit(nr, (volatile unsigned long *)addr);
}

enum pinmux_ip {
	PMX_UART0,
	PMX_UART1,
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
	PMX_ETH0,
	PMX_ETH1,
	PMX_ETH2,
	PMX_FSMCNAND,
	PMX_FSMCPNOR,
	PMX_SDMMC,
	PMX_EMI,
	PMX_SMI,
	PMX_LEGGPIO0,
	PMX_LEGGPIO1,
};

/* UART0 modem modes */
#define PMX_UART_SIMPLE		1
#define PMX_UART_MODEM		2
#define PMX_UART_DISSD		3
#define PMX_UART_DISI2C0	4

/* SSP modes */
#define PMX_SSP_CS0		1
#define PMX_SSP_CS1		2
#define PMX_SSP_CS2		3
#define PMX_SSP_CS3		4
#define PMX_SSP_CS1_CS2		5

/* ETH modes */
#define PMX_ETH_GMII		1
#define PMX_ETH_MII		2
#define PMX_ETH_RGMII		3
#define PMX_ETH_RMII		4
#define PMX_ETH_SGMII		5
#define PMX_ETH_SMII		6

/* NAND modes */
#define PMX_NAND_8BIT		1
#define PMX_NAND_16BIT		2
#define PMX_NAND_4CHIPS		3

/* SMI modes */
#define PMX_SMI_2CHIPS		1
#define PMX_SMI_4CHIPS		2

/* PLGPIO modes */
#define PMX_GPIO		1
#define PMX_PULLUP		2
#define PMX_PULLDOWN		3

#endif
