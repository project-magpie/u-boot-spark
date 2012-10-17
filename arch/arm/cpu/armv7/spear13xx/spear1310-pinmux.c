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

/* Pin multiplexing for i2c0 device */
static void enable_i2c0_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_I2C0_MASK,
			PMX_I2C0_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_I2C0_MASK,
			PMX_I2C0_MASK);
}

/* Pin multiplexing for ssp cs0 device */
static void enable_ssp0cs0_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_SSP0_MASK,
			PMX_SSP0_MASK);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_3,
			PMX_SSP0_CS0_MASK,
			PMX_SSP0_CS0_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_SSP0_MASK,
			PMX_SSP0_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_3,
			PMX_SSP0_CS0_MASK,
			PMX_SSP0_CS0_MASK);
}

/* Pin multiplexing for ssp0 cs1 and cs2 */
static void enable_ssp0cs1cs2_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_3,
			PMX_SSP0_CS1_2_MASK,
			PMX_SSP0_CS1_2_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_3,
			PMX_SSP0_CS1_2_MASK,
			PMX_SSP0_CS1_2_MASK);
}

static void enable_ssp0_pins(u32 mode)
{
	switch (mode) {
	case PMX_SSP_CS0:
		enable_ssp0cs0_pins();
	case PMX_SSP_CS1_CS2:
		enable_ssp0cs1cs2_pins();
		break;
	}
}
/* Pin multiplexing for arm gpio device */
static void enable_leggpio_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_EGPIO_0_GRP_MASK,
			PMX_EGPIO_0_GRP_MASK);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
			PMX_EGPIO_1_GRP_MASK,
			PMX_EGPIO_1_GRP_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_EGPIO_0_GRP_MASK,
			PMX_EGPIO_0_GRP_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
			PMX_EGPIO_1_GRP_MASK,
			PMX_EGPIO_1_GRP_MASK);
}

/* Pin multiplexing for smi two chips device */
static void enable_smi2chips_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_SMI_MASK,
			PMX_SMI_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_SMI_MASK,
			PMX_SMI_MASK);
}

/* Pin multiplexing for smi four chips device */
static void enable_smi4chips_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_SMI_MASK,
			PMX_SMI_MASK);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
			PMX_SMINCS2_MASK | PMX_SMINCS3_MASK,
			PMX_SMINCS2_MASK | PMX_SMINCS3_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_SMI_MASK,
			PMX_SMI_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
			PMX_SMINCS2_MASK | PMX_SMINCS3_MASK,
			PMX_SMINCS2_MASK | PMX_SMINCS3_MASK);
}

static void enable_smi_pins(u32 mode)
{
	switch (mode) {
	case PMX_SMI_2CHIPS:
		enable_smi2chips_pins();
		break;
	case PMX_SMI_4CHIPS:
		enable_smi4chips_pins();
		break;
	}
}

/*
 * Pin multiplexing for ethernet device
 * Ethernet on spear1310 can be configured as either of the below
 * - GMII
 * - RGMII
 * - RMII
 * - SGMII
 */
/* Pin multiplexing for gmii device */
static void enable_gmii_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_GMII_MASK,
			PMX_GMII_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_GMII_MASK,
			PMX_GMII_MASK);
}

/* Pin multiplexing for rgmii device */
static void enable_rgmii_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_RGMII_REG1_MASK,
			0);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
			PMX_RGMII_REG2_MASK,
			0);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_3,
			PMX_RGMII_REG3_MASK,
			0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_RGMII_REG1_MASK,
			PMX_RGMII_REG1_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
			PMX_RGMII_REG2_MASK,
			PMX_RGMII_REG2_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_3,
			PMX_RGMII_REG3_MASK,
			PMX_RGMII_REG3_MASK);
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
	}
}

/* Pin multiplexing for nand 8bit device */
static void enable_nand8bit_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_NAND8BIT_0_MASK,
			PMX_NAND8BIT_0_MASK);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
			PMX_NAND8BIT_1_MASK,
			PMX_NAND8BIT_1_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_NAND8BIT_0_MASK,
			PMX_NAND8BIT_0_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
			PMX_NAND8BIT_1_MASK,
			PMX_NAND8BIT_1_MASK);
}

/* Pin multiplexing for nand 16bit device */
static void enable_nand16bit_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_NAND8BIT_0_MASK,
			PMX_NAND8BIT_0_MASK);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
			PMX_NAND8BIT_1_MASK,
			PMX_NAND8BIT_1_MASK);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
			PMX_NAND16BIT_1_MASK,
			PMX_NAND16BIT_1_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_NAND8BIT_0_MASK,
			PMX_NAND8BIT_0_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
			PMX_NAND8BIT_1_MASK,
			PMX_NAND8BIT_1_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
			PMX_NAND16BIT_1_MASK,
			PMX_NAND16BIT_1_MASK);
}

/* Pin multiplexing for nand 4chips device */
static void enable_nand4chips_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
			PMX_NAND_4CHIPS_MASK,
			PMX_NAND_4CHIPS_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
			PMX_NAND_4CHIPS_MASK,
			PMX_NAND_4CHIPS_MASK);
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
		break;
	case PMX_NAND_4CHIPS:
		enable_nand4chips_pins();
	}
}

/* Pin multiplexing for uart0 device */
static void enable_uart0simple_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_UART0_MASK,
			PMX_UART0_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_UART0_MASK,
			PMX_UART0_MASK);
}

/* Pin multiplexing for uart0 modem device */
static void enable_uart0modem_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
			PMX_UART0_MODEM_MASK,
			PMX_UART0_MODEM_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
			PMX_UART0_MODEM_MASK,
			PMX_UART0_MODEM_MASK);
}


static void enable_uart0_pins(u32 mode)
{
	switch (mode) {
	case PMX_UART_SIMPLE:
		enable_uart0simple_pins();
		break;
	case PMX_UART_MODEM:
		enable_uart0modem_pins();
		break;
	}
}

/* Pin multiplexing for sdmmc device */
static void enable_sdmmc_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_MCI_DATA8_15_MASK,
			PMX_MCI_DATA8_15_MASK);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
			PMX_MCIFALL_1_MASK | PMX_NFWPRT1_MASK |
			PMX_NFWPRT2_MASK,
			PMX_MCIFALL_1_MASK);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_3,
			PMX_MCIFALL_2_MASK,
			PMX_MCIFALL_2_MASK);
	pinmux_maskval(SPEAR1310_PERIP_CFG,
			MCIF_SEL_MASK,
			MCIF_SEL_SD);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_MCI_DATA8_15_MASK,
			PMX_MCI_DATA8_15_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
			PMX_MCIFALL_1_MASK | PMX_NFWPRT1_MASK |
			PMX_NFWPRT2_MASK,
			PMX_MCIFALL_1_MASK | PMX_NFWPRT1_MASK |
			PMX_NFWPRT2_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_3,
			PMX_MCIFALL_2_MASK,
			PMX_MCIFALL_2_MASK);
}

/* Pin multiplexing for uart1disi2c0 device */
static void enable_uart1disi2c0_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_I2C0_MASK,
			0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_I2C0_MASK,
			PMX_I2C0_MASK);
}

/* Pin multiplexing for uart1 (disables SD) device */
static void enable_uart1dissd_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
			PMX_MCIDATA1_MASK |
			PMX_MCIDATA2_MASK,
			0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
			PMX_MCIDATA1_MASK | PMX_MCIDATA2_MASK,
			PMX_MCIDATA1_MASK | PMX_MCIDATA2_MASK);
}

static void enable_uart1_pins(u32 mode)
{
	switch (mode) {
	case PMX_UART_DISSD:
		enable_uart1dissd_pins();
		break;
	case PMX_UART_DISI2C0:
		enable_uart1disi2c0_pins();
		break;
	}
}

/* Pin multiplexing for uart 2 and 3 device */
static void enable_uart23_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_I2S0_MASK,
			0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_I2S0_MASK,
			PMX_I2S0_MASK);
}

/* Pin multiplexing for uart 4 device */
static void enable_uart4_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_I2S0_MASK | PMX_CLCD1_MASK,
			0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_I2S0_MASK | PMX_CLCD1_MASK,
			PMX_I2S0_MASK | PMX_CLCD1_MASK);
}

/* Pin multiplexing for uart 5 device */
static void enable_uart5_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
			PMX_CLCD1_MASK,
			0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
			PMX_CLCD1_MASK,
			PMX_CLCD1_MASK);
}

/* Pin multiplexing for i2c 1 and 2 device */
static void enable_i2c12_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
		PMX_CLCD1_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
		PMX_CLCD1_MASK,
		PMX_CLCD1_MASK);
}

#if 0
/* Pin multiplexing for i2c3 (Disables smi and clcd) device */
static void enable_i2c3dissmiclcd_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
		PMX_CLCD1_MASK | PMX_SMI_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
		PMX_CLCD1_MASK | PMX_SMI_MASK,
		PMX_CLCD1_MASK | PMX_SMI_MASK);
}

/* Pin multiplexing for i2c3 (Disables sd and i2s0) device */
static void enable_i2c3dissdi2s0_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
		PMX_I2S1_MASK | PMX_MCIDATA3_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
		PMX_I2S1_MASK | PMX_MCIDATA3_MASK,
		PMX_I2S1_MASK | PMX_MCIDATA3_MASK);
}

/* Pin multiplexing for i2c4 and 5 (Disables smi and clcd) device */
static void enable_i2c3dissmiclcd_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_1,
		PMX_SMI_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_1,
		PMX_SMI_MASK,
		PMX_SMI_MASK);
}

/* Pin multiplexing for i2c4 (Disables sd) device */
static void enable_i2c4dissd_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
		PMX_MCIDATA4_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_3,
		PMX_MCIDATA5_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
		PMX_MCIDATA4_MASK,
		PMX_MCIDATA4_MASK);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_3,
		PMX_MCIDATA5_MASK,
		PMX_MCIDATA5_MASK);
}

/* Pin multiplexing for i2c5 (Disables sd) device */
static void enable_i2c5dissd_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_3,
			PMX_MCIDATA6_MASK | PMX_MCIDATA7_MASK,
			0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_3,
			PMX_MCIDATA6_MASK | PMX_MCIDATA7_MASK,
			PMX_MCIDATA6_MASK | PMX_MCIDATA7_MASK);
}

/* Pin multiplexing for i2c6 and 7 (Disables kbd) device */
static void enable_i2c67diskbd_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
		PMX_KBD_ROWCOL25_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
		PMX_KBD_ROWCOL25_MASK,
		PMX_KBD_ROWCOL25_MASK);
}

/* Pin multiplexing for i2c6 (Disables sd) device */
static void enable_i2c6dissd_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_3,
		PMX_MCIIORDRE_MASK | PMX_MCIIOWRWE_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_3,
		PMX_MCIIORDRE_MASK | PMX_MCIIOWRWE_MASK,
		PMX_MCIIORDRE_MASK | PMX_MCIIOWRWE_MASK);
}

/* Pin multiplexing for i2c7 (Disables sd) device */
static void enable_i2c7dissd_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_3,
		PMX_MCIRESETCF_MASK | PMX_MCICS0CE_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_3,
		PMX_MCIRESETCF_MASK | PMX_MCICS0CE_MASK,
		PMX_MCIRESETCF_MASK | PMX_MCICS0CE_MASK);
}

/* Pin multiplexing for ssp1 (Disables kbd) device */
static void enable_ssp1diskbd_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_2,
		PMX_KBD_ROWCOL25_MASK | PMX_KBD_COL1_MASK |
			PMX_KBD_COL0_MASK | PMX_NFIO8_15_MASK | PMX_NFCE1_MASK |
			PMX_NFCE2_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_2,
		PMX_KBD_ROWCOL25_MASK | PMX_KBD_COL1_MASK |
			PMX_KBD_COL0_MASK | PMX_NFIO8_15_MASK | PMX_NFCE1_MASK |
			PMX_NFCE2_MASK,
		PMX_KBD_ROWCOL25_MASK | PMX_KBD_COL1_MASK |
			PMX_KBD_COL0_MASK | PMX_NFIO8_15_MASK | PMX_NFCE1_MASK |
			PMX_NFCE2_MASK);
}

/* Pin multiplexing for ssp1 (Disables sd) device */
static void enable_ssp1dissd_pins(void)
{
	pinmux_maskval(SPEAR1310_PAD_FUNCTION_EN_3,
		PMX_MCIADDR0ALE_MASK | PMX_MCIADDR2_MASK |
			PMX_MCICECF_MASK | PMX_MCICEXD_MASK,
		0);
	pinmux_maskval(SPEAR1310_PAD_DIR_SEL_3,
		PMX_MCIADDR0ALE_MASK | PMX_MCIADDR2_MASK |
			PMX_MCICECF_MASK | PMX_MCICEXD_MASK,
		PMX_MCIADDR0ALE_MASK | PMX_MCIADDR2_MASK |
			PMX_MCICECF_MASK | PMX_MCICEXD_MASK);
}
#endif

/**
 * spear1310_pins_default: Select a default safe mode as startup
 * Generally, all pins are enabled in input mode at initialization. This can be
 * done either by
 * - enabling gpio's and keeping all pins in gpio inputs
 * - a platform specific way.
 */
void spear1310_pins_default(void)
{
}

/**
 * spear1310_enable_pins - enable pins for peripherals on spear1310 devices
 * @ip:		Peripheral index
 * @mode:	Mode in which peripheral has to run (16bit/8bit etc)
 *
 * Enable the pins for fixed peripherals on spear3xx devices.
 * mode represents the mode in which the peripheral may work and may result in
 * different pins being enabled. eg GMII mode and RGMII mode may need different
 * pins on devices to be enabled
 */
void spear1310_enable_pins(u32 ip, u32 mode)
{
	if (PMX_SDMMC == ip)
		enable_sdmmc_pins();
	else if (PMX_SMI == ip)
		enable_smi_pins(mode);
	else if (PMX_I2C0 == ip)
		enable_i2c0_pins();
	else if ((PMX_I2C1 == ip) || (PMX_I2C1 == ip))
		enable_i2c12_pins();
	else if (PMX_FSMCNAND == ip)
		enable_nand_pins(mode);
	else if (PMX_UART0 == ip)
		enable_uart0_pins(mode);
	else if (PMX_UART1 == ip)
		enable_uart1_pins(mode);
	else if ((PMX_UART2 == ip) || (PMX_UART3 == ip))
		enable_uart23_pins();
	else if (PMX_UART4 == ip)
		enable_uart4_pins();
	else if (PMX_UART5 == ip)
		enable_uart5_pins();
	else if (PMX_ETH0 == ip)
		enable_eth0_pins(mode);
	else if (PMX_SSP0 == ip)
		enable_ssp0_pins(mode);
	else if (PMX_LEGGPIO0 == ip)
		enable_leggpio_pins();
}

static void configure_gpio(u32 plgpio)
{
	if (plgpio > SPEAR1310_MAX_PLGPIOS)
		return;

	/* Set the GPIO direction to input */
	pinmux_set_bit(plgpio, SPEAR1310_GPIO_EN0);

	/* Enable PLGPIO from RAS */
	pinmux_set_bit(plgpio, SPEAR1310_GPIO_FUN_EN0);

	if (plgpio <= 3) {
		pinmux_clear_bit(16, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(16, SPEAR1310_PAD_DIR_SEL_2);

	} else if ((plgpio >= 4) && (plgpio <= 5)) {
		pinmux_clear_bit(15, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(15, SPEAR1310_PAD_DIR_SEL_2);

	} else if ((plgpio >= 6) && (plgpio <= 7)) {
		pinmux_clear_bit(14, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(14, SPEAR1310_PAD_DIR_SEL_2);

	} else if ((plgpio >= 8) && (plgpio <= 9)) {
		pinmux_clear_bit(13, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(13, SPEAR1310_PAD_DIR_SEL_2);

	} else if ((plgpio >= 10) && (plgpio <= 11)) {
		pinmux_clear_bit(12, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(12, SPEAR1310_PAD_DIR_SEL_2);

	} else if ((plgpio >= 12) && (plgpio <= 17)) {
		pinmux_clear_bit(11, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(11, SPEAR1310_PAD_DIR_SEL_2);

	} else if (plgpio == 18) {
		pinmux_clear_bit(10, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(10, SPEAR1310_PAD_DIR_SEL_2);

	} else if (plgpio == 19) {
		pinmux_clear_bit(9, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(9, SPEAR1310_PAD_DIR_SEL_2);

	} else if (plgpio == 20) {
		pinmux_clear_bit(8, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(8, SPEAR1310_PAD_DIR_SEL_2);

	} else if (plgpio == 21) {
		pinmux_clear_bit(7, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(7, SPEAR1310_PAD_DIR_SEL_2);

	} else if (plgpio == 22) {
		pinmux_clear_bit(6, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(6, SPEAR1310_PAD_DIR_SEL_2);

	} else if (plgpio == 23) {
		pinmux_clear_bit(5, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(5, SPEAR1310_PAD_DIR_SEL_2);

	} else if ((plgpio >= 24) && (plgpio <= 29)) {
		pinmux_clear_bit(4, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(4, SPEAR1310_PAD_DIR_SEL_2);

	} else if ((plgpio >= 30) && (plgpio <= 53)) {
		pinmux_clear_bit(3, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(3, SPEAR1310_PAD_DIR_SEL_2);

	} else if (plgpio == 54) {
		pinmux_clear_bit(2, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(2, SPEAR1310_PAD_DIR_SEL_2);

	} else if (plgpio == 55) {
		pinmux_clear_bit(1, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(1, SPEAR1310_PAD_DIR_SEL_2);

	} else if (plgpio == 56) {
		pinmux_clear_bit(0, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(0, SPEAR1310_PAD_DIR_SEL_2);

	} else if (plgpio == 57) {
		pinmux_clear_bit(31, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(31, SPEAR1310_PAD_DIR_SEL_1);

	} else if (plgpio == 58) {
		pinmux_clear_bit(30, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(30, SPEAR1310_PAD_DIR_SEL_1);

	} else if (plgpio == 59) {
		pinmux_clear_bit(29, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(29, SPEAR1310_PAD_DIR_SEL_1);

	} else if (plgpio == 60) {
		pinmux_clear_bit(28, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(28, SPEAR1310_PAD_DIR_SEL_1);

	} else if (plgpio == 61) {
		pinmux_clear_bit(27, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(27, SPEAR1310_PAD_DIR_SEL_1);

	} else if (plgpio == 62) {
		pinmux_clear_bit(26, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(26, SPEAR1310_PAD_DIR_SEL_1);

	} else if (plgpio == 63) {
		pinmux_clear_bit(25, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(25, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 64) && (plgpio <= 85)) {
		pinmux_clear_bit(24, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(24, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 86) && (plgpio <= 93)) {
		pinmux_clear_bit(23, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(23, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 94) && (plgpio <= 95)) {
		pinmux_clear_bit(30, SPEAR1310_PAD_FUNCTION_EN_3);
		pinmux_set_bit(30, SPEAR1310_PAD_DIR_SEL_3);

	} else if (plgpio >= 96) {
		pinmux_clear_bit(29, SPEAR1310_PAD_FUNCTION_EN_3);
		pinmux_set_bit(29, SPEAR1310_PAD_DIR_SEL_3);

	} else if (plgpio >= 97) {
		pinmux_clear_bit(28, SPEAR1310_PAD_FUNCTION_EN_3);
		pinmux_set_bit(28, SPEAR1310_PAD_DIR_SEL_3);

	} else if ((plgpio >= 98) && (plgpio <= 99)) {
		pinmux_clear_bit(22, SPEAR1310_PAD_FUNCTION_EN_2);
		pinmux_set_bit(22, SPEAR1310_PAD_DIR_SEL_2);

	} else if ((plgpio >= 100) && (plgpio <= 101)) {
		pinmux_clear_bit(1, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(1, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 102) && (plgpio <= 103)) {
		pinmux_clear_bit(2, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(2, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 104) && (plgpio <= 108)) {
		pinmux_clear_bit(3, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(3, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 109) && (plgpio <= 112)) {
		pinmux_clear_bit(4, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(4, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 113) && (plgpio <= 142)) {
		pinmux_clear_bit(5, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(5, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 143) && (plgpio <= 152)) {
		pinmux_clear_bit(plgpio - 137, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(plgpio - 137, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 153) && (plgpio <= 157)) {
		pinmux_clear_bit(16, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(16, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 158) && (plgpio <= 172)) {
		pinmux_clear_bit(17, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(17, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 173) && (plgpio <= 174)) {
		pinmux_clear_bit(18, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(18, SPEAR1310_PAD_DIR_SEL_1);

	} else if (plgpio == 175) {
		pinmux_clear_bit(19, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(19, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 176) && (plgpio <= 179)) {
		pinmux_clear_bit(20, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(20, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 180) && (plgpio <= 183)) {
		pinmux_clear_bit(21, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(21, SPEAR1310_PAD_DIR_SEL_1);

	} else if (plgpio == 184) {
		pinmux_clear_bit(20, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(20, SPEAR1310_PAD_DIR_SEL_1);

	} else if (plgpio == 185) {
		pinmux_clear_bit(19, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(19, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 186) && (plgpio <= 187)) {
		pinmux_clear_bit(20, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(20, SPEAR1310_PAD_DIR_SEL_1);

	} else if (plgpio == 188) {
		pinmux_clear_bit(19, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(19, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 189) && (plgpio <= 192)) {
		pinmux_clear_bit(20, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(20, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 193) && (plgpio <= 196)) {
		pinmux_clear_bit(21, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(21, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 197) && (plgpio <= 198)) {
		pinmux_clear_bit(19, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(19, SPEAR1310_PAD_DIR_SEL_1);

	} else if ((plgpio >= 199) && (plgpio <= 200)) {
		pinmux_clear_bit(22, SPEAR1310_PAD_FUNCTION_EN_1);
		pinmux_set_bit(22, SPEAR1310_PAD_DIR_SEL_1);

	}
}

static void configure_pullup(u32 plgpio)
{
	if (plgpio > SPEAR1310_MAX_PLGPIOS)
		return;

	/* Deactivate pull down */
	pinmux_set_bit(plgpio, SPEAR1310_PAD_PD_CFG_1);

	/* Activate pull up */
	pinmux_clear_bit(plgpio, SPEAR1310_PAD_PU_CFG_1);
}

static void configure_pulldown(u32 plgpio)
{
	if (plgpio > SPEAR1310_MAX_PLGPIOS)
		return;

	/* Deactivate pull up */
	pinmux_set_bit(plgpio, SPEAR1310_PAD_PU_CFG_1);

	/* Activate pull down */
	pinmux_clear_bit(plgpio, SPEAR1310_PAD_PD_CFG_1);
}

/**
 * spear1310_configure_pin - Configure pin on spear1310 devices
 * @plgpio:	Pin Number (plgpio number)
 * @mode:	Pull UP, Pull DOWN, plgpio IN, plgpio OUT etc
 */
void spear1310_configure_pin(u32 plgpio, u32 mode)
{
	if (PMX_GPIO == mode)
		configure_gpio(plgpio);
	else if (PMX_PULLUP == mode)
		configure_pullup(plgpio);
	else if (PMX_PULLDOWN == mode)
		configure_pulldown(plgpio);
}

/**
 * spear1310_plgpio_get - Get the gpio input
 * @plgpio:	Pin Number (plgpio number)
 */
int spear1310_plgpio_get(u32 plgpio)
{
	if (plgpio > SPEAR1310_MAX_PLGPIOS)
		return -1;

	/* Set the pin to GPIO IN mode */
	pinmux_set_bit(plgpio, SPEAR1310_GPIO_EN0);

	return pinmux_test_bit(plgpio, SPEAR1310_GPIO_IN0);
}

/**
 * spear1310_plgpio_set - Set the gpio value
 * @plgpio:	Pin Number (plgpio number)
 */
void spear1310_plgpio_set(u32 plgpio, u32 val)
{
	if (plgpio > SPEAR1310_MAX_PLGPIOS)
		return;

	if (val & 0x1)
		pinmux_set_bit(plgpio, SPEAR1310_GPIO_OUT0);
	else
		pinmux_clear_bit(plgpio, SPEAR1310_GPIO_OUT0);

	/* Set the pin to GPIO OUT mode */
	pinmux_clear_bit(plgpio, SPEAR1310_GPIO_EN0);
}
