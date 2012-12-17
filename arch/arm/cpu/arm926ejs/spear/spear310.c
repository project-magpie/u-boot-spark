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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/pinmux.h>

/*
 * Function to dnamically set control of shared mdio lines to concerned
 * mac controller
 */
void arch_get_mdio_control(const char *devname)
{
	u32 val;

	val = readl(SPEAR310_SMII_REG);
	val &= ~SPEAR310_SMII_PHY_MASK;

	if (!strcmp(devname, "macb0"))
		val |= (0x0 << SPEAR310_SMII_PHY_SHIFT);
	else if (!strcmp(devname, "macb1"))
		val |= (0x1 << SPEAR310_SMII_PHY_SHIFT);
	else if (!strcmp(devname, "macb2"))
		val |= (0x2 << SPEAR310_SMII_PHY_SHIFT);
	else if (!strcmp(devname, "macb3"))
		val |= (0x3 << SPEAR310_SMII_PHY_SHIFT);
	else
		printf("no such device:%s\n", devname);

	writel(val, SPEAR310_SMII_REG);
}

/*
 * Pinmux support
 *
 * The routines are defined by the name enable_xxx_pins with xxx being the
 * peripheral controller for which pins are to be enabled
 *
 * PS: In some cases, a multiple combination of pins can be enabled for the same
 * peripheral. In those cases, the routine is defined as enable_xxx_atob_pins.
 * Here, xxx is peripheral itself and a and b represent the pin numbers which
 * need to be enabled for this controller
 */

/* Pinmux for EMI */
static void enable_emi_pins(void)
{
	pinmux_maskval(SPEAR310_FUNCENB_REG,
			PMX_TIMER_0_1_MASK | PMX_TIMER_2_3_MASK,
			0);
}

/* Pinmux for UART1 */
static void enable_uart1_pins(void)
{
	pinmux_maskval(SPEAR310_FUNCENB_REG,
			PMX_FIRDA_MASK,
			0);
}

/* Pinmux for UART2 */
static void enable_uart2_pins(void)
{
	pinmux_maskval(SPEAR310_FUNCENB_REG,
			PMX_TIMER_0_1_MASK,
			0);
}

/* Pinmux for UART3 */
static void enable_uart3_pins(void)
{
	pinmux_maskval(SPEAR310_FUNCENB_REG,
			PMX_UART0_MODEM_MASK,
			0);
}

/* Pinmux for UART4 */
static void enable_uart4_pins(void)
{
	pinmux_maskval(SPEAR310_FUNCENB_REG,
			PMX_UART0_MODEM_MASK,
			0);
}

/* Pinmux for UART5 */
static void enable_uart5_pins(void)
{
	pinmux_maskval(SPEAR310_FUNCENB_REG,
			PMX_UART0_MODEM_MASK,
			0);
}

/* Pinmux for NAND */
static void enable_nand_pins(void)
{
	pinmux_maskval(SPEAR310_FUNCENB_REG,
			PMX_SSP_CS_MASK,
			0);
}

/**
 * spear310_pins_default: Select a default safe mode as startup
 * Generally, all pins are enabled in input mode at initialization. This can be
 * done either by
 * - enabling gpio's and keeping all pins in gpio inputs
 * - a platform specific way.
 */
void spear310_pins_default(void)
{
}

/**
 * spear310_enable_pins - enable pins for fixed peripherals on spear3xx devices
 * @ip:		Peripheral index
 * @mode:	Mode in which peripheral has to run (16bit/8bit etc)
 *
 * Enable the pins for fixed peripherals on spear3xx devices.
 * mode represents the mode in which the peripheral may work and may result in
 * different pins being enabled. eg GMII mode and RGMII mode may need different
 * pins on devices to be enabled
 */
void spear310_enable_pins(u32 ip, u32 mode)
{
	if (PMX_FSMCNAND == ip)
		enable_nand_pins();
	else if (PMX_EMI == ip)
		enable_emi_pins();
	else if (PMX_UART1 == ip)
		enable_uart1_pins();
	else if (PMX_UART2 == ip)
		enable_uart2_pins();
	else if (PMX_UART3 == ip)
		enable_uart3_pins();
	else if (PMX_UART4 == ip)
		enable_uart4_pins();
	else if (PMX_UART5 == ip)
		enable_uart5_pins();
	else if ((PMX_I2C0 == ip) || (PMX_SSP0 == ip) || \
			(PMX_ETH0 == ip) || (PMX_UART0 == ip))
		spear3xx_enable_pins(ip, mode);
}
