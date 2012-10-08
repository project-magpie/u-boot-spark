/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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

/* Pin multiplexing for fsmc nand 16 bit device */
static void enable_nand16bit_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_SHARED_IP_EN_1,
			KBD_ROW_COL_MASK,
			0);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_1,
			FSMC_16_BIT_AND_KBD_ROW_COL_REG0_MASK,
			FSMC_16_BIT_AND_KBD_ROW_COL_REG0_MASK);
}

/* Pin multiplexing for fsmc nand 8 bit device */
static void enable_nand8bit_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_8,
			FSMC_8BIT_REG7_MASK,
			FSMC_8BIT_REG7_MASK);
}

/* Pin multiplexing for fsmc nand device */
static void enable_nand_pins(u32 mode)
{
	switch (mode) {
	case PMX_NAND_8BIT:
		enable_nand8bit_pins();
		break;
	case PMX_NAND_16BIT:
		enable_nand16bit_pins();
	}
}

/* Pin multiplexing for smi device */
static void enable_smi_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_3,
			SMI_REG2_MASK,
			SMI_REG2_MASK);
}

/* Pin multiplexing for ssp0 device */
static void enable_ssp0cs0_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_3,
			SSP0_REG2_MASK,
			SSP0_REG2_MASK);
}

/* Pin multiplexing for ssp0 chip select 1 */
static void enable_ssp0cs1_pins(void)
{
	enable_ssp0cs0_pins();

	pinmux_maskval(SPEAR1340_PAD_SHARED_IP_EN_1,
			SSP0_CS1_MASK,
			SSP0_CS1_MASK);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_1,
			PWM0_AND_SSP0_CS1_REG0_MASK,
			PWM0_AND_SSP0_CS1_REG0_MASK);
}

/* Pin multiplexing for ssp0 chip select 2 */
static void enable_ssp0cs2_pins(void)
{
	enable_ssp0cs0_pins();

	pinmux_maskval(SPEAR1340_PAD_SHARED_IP_EN_1,
			SSP0_CS2_MASK,
			SSP0_CS2_MASK);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_3,
			TS_AND_SSP0_CS2_REG2_MASK,
			TS_AND_SSP0_CS2_REG2_MASK);
}

/* Pin multiplexing for ssp0 chip select 3 */
static void enable_ssp0cs3_pins(void)
{
	enable_ssp0cs0_pins();

	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_5,
			SSP0_CS3_REG4_MASK,
			SSP0_CS3_REG4_MASK);
}

static void enable_ssp0_pins(u32 mode)
{
	switch (mode) {
	case PMX_SSP_CS0:
		enable_ssp0cs0_pins();
		break;
	case PMX_SSP_CS1:
		enable_ssp0cs1_pins();
		break;
	case PMX_SSP_CS2:
		enable_ssp0cs2_pins();
		break;
	case PMX_SSP_CS3:
		enable_ssp0cs3_pins();
		break;
	}
}

/* Pin multiplexing for uart0 device */
static void enable_uart0simple_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_3,
			UART0_REG2_MASK,
			UART0_REG2_MASK);
}

/* Pin multiplexing for uart0 enhanced (modem) device */
static void enable_uart0_enh_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_SHARED_IP_EN_1,
			GPT_MASK,
			0);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_1,
			UART0_ENH_AND_GPT_REG0_MASK,
			UART0_ENH_AND_GPT_REG0_MASK);
}

static void enable_uart0_pins(u32 mode)
{
	switch (mode) {
	case PMX_UART_SIMPLE:
		enable_uart0simple_pins();
		break;
	case PMX_UART_MODEM:
		enable_uart0_enh_pins();
		break;
	}
}

/* Pin multiplexing for uart1 device */
static void enable_uart1_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_3,
			UART1_REG2_MASK,
			UART1_REG2_MASK);
}

/* Pin multiplexing for fsmc-prallel nor device */
static void enable_fsmcpnor_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_SHARED_IP_EN_1,
			MCIF_MASK,
			0);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_7,
			FSMC_PNOR_AND_MCIF_REG6_MASK,
			FSMC_PNOR_AND_MCIF_REG6_MASK);
}

/*
 * Pin multiplexing for ethernet device
 * Ethernet on spear1340 can be configured as either of the below
 * - GMII
 * - RGMII
 * - RMII
 * - SGMII
 */
static void enable_miicommon_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_4,
			GMAC_REG3_MASK,
			GMAC_REG3_MASK);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_5,
			GMAC_REG4_MASK,
			GMAC_REG4_MASK);
}

static void enable_gmii_pins(void)
{
	enable_miicommon_pins();

	pinmux_maskval(SPEAR1340_GMAC_CLK_CFG,
			GMAC_PHY_IF_SEL_MASK,
			GMAC_PHY_IF_GMII_VAL);
}

static void enable_rgmii_pins(void)
{
	enable_miicommon_pins();

	pinmux_maskval(SPEAR1340_GMAC_CLK_CFG,
			GMAC_PHY_IF_SEL_MASK,
			GMAC_PHY_IF_RGMII_VAL);
}

static void enable_rmii_pins(void)
{
	enable_miicommon_pins();

	pinmux_maskval(SPEAR1340_GMAC_CLK_CFG,
			GMAC_PHY_IF_SEL_MASK,
			GMAC_PHY_IF_RMII_VAL);
}

static void enable_sgmii_pins(void)
{
	enable_miicommon_pins();

	pinmux_maskval(SPEAR1340_GMAC_CLK_CFG,
			GMAC_PHY_IF_SEL_MASK,
			GMAC_PHY_IF_SGMII_VAL);
}

static void enable_eth0_pins(u32 mode)
{
	switch (mode) {
	case PMX_ETH_GMII:
		enable_gmii_pins();
		break;
	case PMX_ETH_RGMII:
		enable_rgmii_pins();
		break;
	case PMX_ETH_RMII:
		enable_rmii_pins();
		break;
	case PMX_ETH_SGMII:
		enable_sgmii_pins();
		break;
	}
}

/* Pin multiplexing for i2c0 device */
static void enable_i2c0_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_5,
			I2C0_REG4_MASK,
			I2C0_REG4_MASK);
}

/* Pin multiplexing for i2c1 device */
static void enable_i2c1_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_1,
			I2C1_REG0_MASK,
			I2C1_REG0_MASK);
}

/* Pin multiplexing for sdmmc device */
static void enable_sdmmc_pins(void)
{
	pinmux_maskval(SPEAR1340_PAD_SHARED_IP_EN_1,
			MCIF_MASK,
			MCIF_MASK);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_7,
			FSMC_PNOR_AND_MCIF_REG6_MASK | MCIF_REG6_MASK,
			FSMC_PNOR_AND_MCIF_REG6_MASK | MCIF_REG6_MASK);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_8,
			MCIF_REG7_MASK,
			MCIF_REG7_MASK);
	pinmux_maskval(SPEAR1340_PERIP_CFG,
			MCIF_SEL_MASK,
			MCIF_SEL_SD);
}

/**
 * spear1340_pins_default: Select a default safe mode as startup
 * Generally, all pins are enabled in input mode at initialization. This can be
 * done either by
 * - enabling gpio's and keeping all pins in gpio inputs
 * - a platform specific way.
 */
void spear1340_pins_default(void)
{
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_1,
			PADS_AS_GPIO_REG0_MASK,
			0x0);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_2,
			PADS_AS_GPIO_REGS_MASK,
			0x0);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_3,
			PADS_AS_GPIO_REGS_MASK,
			0x0);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_4,
			PADS_AS_GPIO_REGS_MASK,
			0x0);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_5,
			PADS_AS_GPIO_REGS_MASK,
			0x0);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_6,
			PADS_AS_GPIO_REGS_MASK,
			0x0);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_7,
			PADS_AS_GPIO_REGS_MASK,
			0x0);
	pinmux_maskval(SPEAR1340_PAD_FUNCTION_EN_8,
			PADS_AS_GPIO_REG7_MASK,
			0x0);
}

/**
 * spear1340_enable_pins - enable pins for peripherals on spear1340 devices
 * @ip:		Peripheral index
 * @mode:	Mode in which peripheral has to run (16bit/8bit etc)
 *
 * Enable the pins for fixed peripherals on spear3xx devices.
 * mode represents the mode in which the peripheral may work and may result in
 * different pins being enabled. eg GMII mode and RGMII mode may need different
 * pins on devices to be enabled
 */
void spear1340_enable_pins(u32 ip, u32 mode)
{
	if (PMX_SDMMC == ip)
		enable_sdmmc_pins();
	else if (PMX_SMI == ip)
		enable_smi_pins();
	else if (PMX_I2C0 == ip)
		enable_i2c0_pins();
	else if (PMX_I2C1 == ip)
		enable_i2c1_pins();
	else if (PMX_UART1 == ip)
		enable_uart1_pins();
	else if (PMX_FSMCPNOR == ip)
		enable_fsmcpnor_pins();
	else if (PMX_FSMCNAND == ip)
		enable_nand_pins(mode);
	else if (PMX_UART0 == ip)
		enable_uart0_pins(mode);
	else if (PMX_ETH0 == ip)
		enable_eth0_pins(mode);
	else if (PMX_SSP0 == ip)
		enable_ssp0_pins(mode);
}

static void configure_gpio(u32 plgpio)
{
	if (plgpio > SPEAR1340_MAX_PLGPIOS)
		return;

	/* Set the pin to GPIO IN mode */
	pinmux_set_bit(plgpio, SPEAR1340_PLGPIO_EN0);

	/* Put the pin in gpio state */
	if (plgpio < 127)
		pinmux_clear_bit(plgpio + 1, SPEAR1340_PAD_FUNCTION_EN_1);
	else
		pinmux_clear_bit(plgpio + 1, SPEAR1340_PAD_FUNCTION_EN_5);
}

static void configure_pullup(u32 plgpio)
{
	if (plgpio > SPEAR1340_MAX_PLGPIOS)
		return;

	/* Deactivate pull down */
	pinmux_set_bit(plgpio, SPEAR1340_PAD_PD_CFG_1);

	/* Activate pull up */
	pinmux_clear_bit(plgpio, SPEAR1340_PAD_PU_CFG_1);
}

static void configure_pulldown(u32 plgpio)
{
	if (plgpio > SPEAR1340_MAX_PLGPIOS)
		return;

	/* Deactivate pull up */
	pinmux_set_bit(plgpio, SPEAR1340_PAD_PU_CFG_1);

	/* Activate pull down */
	pinmux_clear_bit(plgpio, SPEAR1340_PAD_PD_CFG_1);
}

/**
 * spear1340_configure_pin - Configure pin on spear1340 devices
 * @plgpio:	Pin Number (plgpio number)
 * @mode:	Pull UP, Pull DOWN, plgpio IN, plgpio OUT etc
 */
void spear1340_configure_pin(u32 plgpio, u32 mode)
{
	if (PMX_GPIO == mode)
		configure_gpio(plgpio);
	else if (PMX_PULLUP == mode)
		configure_pullup(plgpio);
	else if (PMX_PULLDOWN == mode)
		configure_pulldown(plgpio);
}

/**
 * spear1340_plgpio_get - Get the gpio input
 * @plgpio:	Pin Number (plgpio number)
 */
int spear1340_plgpio_get(u32 plgpio)
{
	if (plgpio > SPEAR1340_MAX_PLGPIOS)
		return -1;

	/* Set the pin to GPIO IN mode */
	pinmux_set_bit(plgpio, SPEAR1340_PLGPIO_EN0);

	return pinmux_test_bit(plgpio, SPEAR1340_PLGPIO_IN0);
}

/**
 * spear1340_plgpio_set - Set the gpio value
 * @plgpio:	Pin Number (plgpio number)
 */
void spear1340_plgpio_set(u32 plgpio, u32 val)
{
	if (plgpio > SPEAR1340_MAX_PLGPIOS)
		return;

	if (val & 0x1)
		pinmux_set_bit(plgpio, SPEAR1340_PLGPIO_OUT0);
	else
		pinmux_clear_bit(plgpio, SPEAR1340_PLGPIO_OUT0);

	/* Set the pin to GPIO OUT mode */
	pinmux_clear_bit(plgpio, SPEAR1340_PLGPIO_EN0);
}
