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

/* Pinmux support for all fixed spear3xx devices */

/* Pinmux for I2C0 */
static void enable_i2c0_pins(void)
{
	pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
			PMX_I2C_MASK,
			PMX_I2C_MASK);
}

/* Pinmux for SSP0 */
static void enable_ssp0_pins(void)
{
	pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
			PMX_SSP_MASK,
			PMX_SSP_MASK);
}

/* Pinmux for ETH0 */
static void enable_eth0_pins(void)
{
	pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
			PMX_MII_MASK,
			PMX_MII_MASK);
}

/* Pinmux for UART0 ext */
static void enable_uart0_ext_pins(void)
{
	pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
			PMX_UART0_MODEM_MASK,
			PMX_UART0_MODEM_MASK);

}

/* Pinmux for UART0 */
static void enable_uart0_simple_pins(void)
{
	pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
			PMX_UART0_MASK,
			PMX_UART0_MASK);
}

static void enable_uart0_pins(u32 mode)
{
	switch (mode) {
	case PMX_UART_SIMPLE:
		enable_uart0_simple_pins();
		break;
	case PMX_UART_MODEM:
		enable_uart0_ext_pins();
		break;
	}
}

/**
 * spear3xx_enable_pins - enable pins for fixed peripherals on spear3xx devices
 * @ip:		Peripheral index
 * @mode:	Mode in which peripheral has to run (16bit/8bit etc)
 *
 * Enable the pins for fixed peripherals on spear3xx devices.
 * mode represents the mode in which the peripheral may work and may result in
 * different pins being enabled. eg GMII mode and RGMII mode may need different
 * pins on devices to be enabled
 */
void spear3xx_enable_pins(u32 ip, u32 mode)
{
	switch (ip) {
	case PMX_I2C0:
		enable_i2c0_pins();
		break;
	case PMX_SSP0:
		enable_ssp0_pins();
		break;
	case PMX_ETH0:
		enable_eth0_pins();
		break;
	case PMX_UART0:
		enable_uart0_pins(mode);
		break;
	default:
		printf("Unsupported device\n");
		break;
	}
}
