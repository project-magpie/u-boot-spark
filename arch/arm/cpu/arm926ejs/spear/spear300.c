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

/* NAND pinmux */
static void enable_nand_2chips_pins(void)
{
	pinmux_maskval(SPEAR300_RAS_REG1,
			PMX_FIRDA_MASK,
			0);
}

static void enable_nand_4chips_pins(void)
{
	pinmux_maskval(SPEAR300_RAS_REG1,
			PMX_FIRDA_MASK | PMX_UART0_MASK,
			0);
}

static void enable_nand_pins(u32 mode)
{
	switch (mode) {
	case PMX_NAND_2CHIP:
		enable_nand_2chips_pins();
		break;
	case PMX_NAND_4CHIP:
		enable_nand_4chips_pins();
		break;
	}
}

/* SDMMC pinmux */
static void enable_sdmmc_4bit_pins(void)
{
	pinmux_maskval(SPEAR300_RAS_REG1,
			PMX_GPIO_PIN0_MASK | PMX_GPIO_PIN1_MASK |
			PMX_GPIO_PIN2_MASK | PMX_GPIO_PIN3_MASK |
			PMX_GPIO_PIN4_MASK | PMX_GPIO_PIN5_MASK,
			0);
}

static void enable_sdmmc_8bit_pins(void)
{
	pinmux_maskval(SPEAR300_RAS_REG1,
			PMX_GPIO_PIN0_MASK | PMX_GPIO_PIN1_MASK |
			PMX_GPIO_PIN2_MASK | PMX_GPIO_PIN3_MASK |
			PMX_GPIO_PIN4_MASK | PMX_GPIO_PIN5_MASK | PMX_MII_MASK,
			0);
}

static void enable_sdmmc_pins(u32 mode)
{
	switch (mode) {
	case PMX_SDMMC_4BIT:
		enable_sdmmc_4bit_pins();
		break;
	case PMX_SDMMC_8BIT:
		enable_sdmmc_8bit_pins();
		break;
	}
}

/**
 * spear300_select_mode
 * @mode:	SoC mode to e selected
 */
void spear300_select_mode(u32 mode)
{
	pinmux_maskval(SPEAR300_RAS_REG2,
			SPEAR300_MODE_MSK,
			mode);
}

/**
 * spear300_pins_default: Select a default safe mode as startup
 * Generally, all pins are enabled in input mode at initialization. This can be
 * done either by
 * - enabling gpio's and keeping all pins in gpio inputs
 * - a platform specific way.
 */
void spear300_pins_default(void)
{
}

/**
 * spear300_enable_pins - enable pins for fixed peripherals on spear3xx devices
 * @ip:		Peripheral index
 * @mode:	Mode in which peripheral has to run (16bit/8bit etc)
 *
 * Enable the pins for fixed peripherals on spear3xx devices.
 * mode represents the mode in which the peripheral may work and may result in
 * different pins being enabled. eg GMII mode and RGMII mode may need different
 * pins on devices to be enabled
 */
void spear300_enable_pins(u32 ip, u32 mode)
{
	if (PMX_FSMCNAND == ip)
		enable_nand_pins(mode);
	else if (PMX_SDMMC == ip)
		enable_sdmmc_pins(mode);
	else if ((PMX_I2C0 == ip) || (PMX_SSP0 == ip) || \
			(PMX_ETH0 == ip) || (PMX_UART0 == ip))
		spear3xx_enable_pins(ip, mode);
}
