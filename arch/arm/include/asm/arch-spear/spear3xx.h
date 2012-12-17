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

#ifndef _ASM_ARCH_SPEAR3XX_H
#define _ASM_ARCH_SPEAR3XX_H

#define CONFIG_SYS_I2C_BASE			0xD0180000
#define CONFIG_GPIO_BASE			0xD8100000
#define CONFIG_SPEAR_ETHBASE			0xE0800000
#define CONFIG_SYS_FIFO_BASE			0xE1000800
#define CONFIG_SYS_USBD_BASE			0xE1100000
#define CONFIG_SYS_PLUG_BASE			0xE1200000
#define CONFIG_SYS_UHC0_EHCI_BASE		0xE1800000
#define CONFIG_SYS_SMI_BASE			0xFC000000
#define CONFIG_SPEAR_MPMCBASE			0xFC600000
#define CONFIG_SPEAR_MPMCREGS			109

#define CONFIG_SPEAR_TIMERBASE			0xFC800000
#define CONFIG_SPEAR_SYSCNTLBASE		0xFCA00000
#define CONFIG_SPEAR_MISCBASE			0xFCA80000

/* spear3xx pinmux register and related bit masks */
#define PMX_FIRDA_MASK			(1 << 14)
#define PMX_I2C_MASK			(1 << 13)
#define PMX_SSP_CS_MASK			(1 << 12)
#define PMX_SSP_MASK			(1 << 11)
#define PMX_MII_MASK			(1 << 10)
#define PMX_GPIO_PIN0_MASK		(1 << 9)
#define PMX_GPIO_PIN1_MASK		(1 << 8)
#define PMX_GPIO_PIN2_MASK		(1 << 7)
#define PMX_GPIO_PIN3_MASK		(1 << 6)
#define PMX_GPIO_PIN4_MASK		(1 << 5)
#define PMX_GPIO_PIN5_MASK		(1 << 4)
#define PMX_UART0_MODEM_MASK		(1 << 3)
#define PMX_UART0_MASK			(1 << 2)
#define PMX_TIMER_2_3_MASK		(1 << 1)
#define PMX_TIMER_0_1_MASK		(1 << 0)

#define SPEAR3XX_MAX_PLGPIOS			101

/* externs related to pinmux */
extern void spear3xx_enable_pins(u32 ip, u32 mode);

#endif
