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

void arch_get_mdio_control(const char *devname)
{
	u32 val;

	val = readl(SPEAR320_CONTROL_REG);

	if (!strcmp(devname, "macb0"))
		val &= ~(0x1 << MII_ENB_SHFT);
	else if (!strcmp(devname, "macb1"))
		val |= (0x1 << MII_ENB_SHFT);
	else
		printf("no such device:%s\n", devname);

	writel(val, SPEAR320_CONTROL_REG);
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
	pinmux_maskval(SPEAR320_IP_SEL_PAD_40_49_REG,
			PMX_PL_46_47_MASK | PMX_PL_48_49_MASK,
			PMX_FSMC_EMI_PL_46_47_VAL | PMX_FSMC_EMI_PL_48_49_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_50_59_REG,
			PMX_PL_50_51_MASK | PMX_PL_52_53_MASK |
			PMX_PL_54_55_56_MASK | PMX_PL_58_59_MASK,
			PMX_EMI_PL_50_51_VAL | PMX_EMI_PL_52_53_VAL |
			PMX_FSMC_EMI_PL_54_55_56_VAL |
			PMX_FSMC_EMI_PL_58_59_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_60_69_REG,
			PMX_PL_69_MASK,
			PMX_EMI_PL_69_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_70_79_REG,
			PMX_PL_70_MASK | PMX_PL_71_72_MASK | PMX_PL_73_MASK |
			PMX_PL_74_MASK | PMX_PL_75_76_MASK |
			PMX_PL_77_78_79_MASK,
			PMX_FSMC_EMI_PL_70_VAL | PMX_FSMC_EMI_PL_71_72_VAL |
			PMX_FSMC_EMI_PL_73_VAL | PMX_EMI_PL_74_VAL |
			PMX_EMI_PL_75_76_VAL | PMX_EMI_PL_77_78_79_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_80_89_REG,
			PMX_PL_80_TO_85_MASK | PMX_PL_86_87_MASK |
			PMX_PL_88_89_MASK,
			PMX_EMI_PL_80_TO_85_VAL | PMX_EMI_PL_86_87_VAL |
			PMX_EMI_PL_88_89_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_90_99_REG,
			PMX_PL_90_91_MASK | PMX_PL_92_93_MASK |
			PMX_PL_94_95_MASK | PMX_PL_96_97_MASK,
			PMX_EMI1_PL_90_91_VAL | PMX_EMI1_PL_92_93_VAL |
			PMX_EMI1_PL_94_95_VAL | PMX_EMI1_PL_96_97_VAL);
	pinmux_maskval(SPEAR320_EXT_CTRL_REG,
			EMI_FSMC_DYNAMIC_MUX_MASK,
			EMI_FSMC_DYNAMIC_MUX_MASK);
}

/* Pinmux for 8bit NAND interface */
static void enable_nand8bit_pins(void)
{
	pinmux_maskval(SPEAR320_IP_SEL_PAD_50_59_REG,
			PMX_PL_52_53_MASK | PMX_PL_54_55_56_MASK |
			PMX_PL_57_MASK | PMX_PL_58_59_MASK,
			PMX_FSMC_PL_52_53_VAL | PMX_FSMC_EMI_PL_54_55_56_VAL |
			PMX_FSMC_PL_57_VAL | PMX_FSMC_EMI_PL_58_59_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_60_69_REG,
			PMX_PL_60_MASK | PMX_PL_61_TO_64_MASK |
			PMX_PL_65_TO_68_MASK,
			PMX_FSMC_PL_60_VAL | PMX_FSMC_PL_61_TO_64_VAL |
			PMX_FSMC_PL_65_TO_68_VAL);
	pinmux_maskval(SPEAR320_EXT_CTRL_REG,
			EMI_FSMC_DYNAMIC_MUX_MASK,
			EMI_FSMC_DYNAMIC_MUX_MASK);
}

/* Pinmux for 16bit NAND interface */
static void enable_nand16bit_pins(void)
{
	enable_nand8bit_pins();

	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_TIMER_0_1_MASK | PMX_TIMER_2_3_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_40_49_REG,
			PMX_PL_46_47_MASK | PMX_PL_48_49_MASK,
			PMX_FSMC_EMI_PL_46_47_VAL | PMX_FSMC_EMI_PL_48_49_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_70_79_REG,
			PMX_PL_70_MASK | PMX_PL_71_72_MASK | PMX_PL_73_MASK,
			PMX_FSMC_EMI_PL_70_VAL | PMX_FSMC_EMI_PL_71_72_VAL |
			PMX_FSMC_EMI_PL_73_VAL);
}

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

/* Pinmux for sdmmc led */
static void enable_sdmmc_led_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_SSP_CS_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_30_39_REG,
			PMX_PL_34_MASK,
			PMX_PWM2_PL_34_VAL);
}

static void enable_sdmmc_common_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_TIMER_0_1_MASK | PMX_TIMER_2_3_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_40_49_REG,
			PMX_PL_43_MASK | PMX_PL_44_45_MASK | PMX_PL_46_47_MASK |
			PMX_PL_48_49_MASK,
			PMX_SDHCI_PL_43_VAL | PMX_SDHCI_PL_44_45_VAL |
			PMX_SDHCI_PL_46_47_VAL | PMX_SDHCI_PL_48_49_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_50_59_REG,
			PMX_PL_50_MASK,
			PMX_SDHCI_PL_50_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_90_99_REG,
			PMX_PL_99_MASK,
			PMX_SDHCI_PL_99_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_PL_100_101_MASK,
			PMX_SDHCI_PL_100_101_VAL);
}

static void enable_sdmmc_cd12_pins(void)
{
	enable_sdmmc_common_pins();

	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_MII_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_10_19_REG,
			PMX_PL_12_MASK,
			PMX_SDHCI_CD_PL_12_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SDHCI_CD_PORT_SEL_MASK,
			PMX_SDHCI_CD_PORT_12_VAL);
}

static void enable_sdmmc_cd51_pins(void)
{
	enable_sdmmc_common_pins();

	pinmux_maskval(SPEAR320_IP_SEL_PAD_50_59_REG,
			PMX_PL_51_MASK,
			PMX_SDHCI_CD_PL_51_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SDHCI_CD_PORT_SEL_MASK,
			PMX_SDHCI_CD_PORT_51_VAL);
}

static void enable_sdmmc_pins(u32 mode)
{
	switch (mode) {
	case PMX_SDMMC_LED:
		enable_sdmmc_led_pins();
		break;
	case PMX_SDMMC_CD12:
		enable_sdmmc_cd12_pins();
		break;
	case PMX_SDMMC_CD51:
		enable_sdmmc_cd51_pins();
		break;
	}
}

/* Pinmux for UART1 */
static void enable_uart1simple_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_GPIO_PIN0_MASK | PMX_GPIO_PIN1_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_20_29_REG,
			PMX_PL_28_29_MASK,
			PMX_UART1_PL_28_29_VAL);
}

/* Pinmux for UART1 modem */
static void enable_uart1_modem_2to7_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_UART0_MASK | PMX_I2C_MASK | PMX_SSP_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_0_9_REG,
			PMX_PL_2_3_MASK | PMX_PL_6_7_MASK,
			PMX_UART1_ENH_PL_2_3_VAL | PMX_UART1_ENH_PL_4_5_VAL |
			PMX_UART1_ENH_PL_6_7_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_UART1_ENH_PORT_SEL_MASK,
			PMX_UART1_ENH_PORT_3_TO_5_7_VAL);
}

static void enable_uart1_modem_31to36_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_GPIO_PIN3_MASK | PMX_GPIO_PIN4_MASK |
			PMX_GPIO_PIN5_MASK | PMX_SSP_CS_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_30_39_REG,
			PMX_PL_31_MASK | PMX_PL_32_33_MASK | PMX_PL_34_MASK |
			PMX_PL_35_MASK | PMX_PL_36_MASK,
			PMX_UART1_ENH_PL_31_VAL | PMX_UART1_ENH_PL_32_33_VAL |
			PMX_UART1_ENH_PL_34_VAL | PMX_UART1_ENH_PL_35_VAL |
			PMX_UART1_ENH_PL_36_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_UART1_ENH_PORT_SEL_MASK,
			PMX_UART1_ENH_PORT_32_TO_34_36_VAL);
}

static void enable_uart1_modem_34to45_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_TIMER_0_1_MASK | PMX_TIMER_2_3_MASK |
			PMX_SSP_CS_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_30_39_REG,
			PMX_PL_34_MASK | PMX_PL_35_MASK | PMX_PL_36_MASK,
			PMX_UART1_ENH_PL_34_VAL | PMX_UART1_ENH_PL_35_VAL |
			PMX_UART1_ENH_PL_36_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_40_49_REG,
			PMX_PL_43_MASK | PMX_PL_44_45_MASK,
			PMX_UART1_ENH_PL_43_VAL | PMX_UART1_ENH_PL_44_45_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_UART1_ENH_PORT_SEL_MASK,
			PMX_UART1_ENH_PORT_44_45_34_36_VAL);
}

static void enable_uart1_modem_80to85_pins(void)
{
	pinmux_maskval(SPEAR320_IP_SEL_PAD_80_89_REG,
			PMX_PL_80_TO_85_MASK,
			PMX_UART1_ENH_PL_80_TO_85_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_40_49_REG,
			PMX_PL_43_MASK | PMX_PL_44_45_MASK,
			PMX_UART1_ENH_PL_43_VAL | PMX_UART1_ENH_PL_44_45_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_UART1_ENH_PORT_SEL_MASK,
			PMX_UART1_ENH_PORT_81_TO_85_VAL);
}

static void enable_uart1_pins(u32 mode)
{
	switch (mode) {
	case PMX_UART_SIMPLE:
		enable_uart1simple_pins();
		break;
	case PMX_UART_MDM_2_7:
		enable_uart1_modem_2to7_pins();
		break;
	case PMX_UART_MDM_31_36:
		enable_uart1_modem_31to36_pins();
		break;
	case PMX_UART_MDM_34_45:
		enable_uart1_modem_34to45_pins();
		break;
	case PMX_UART_MDM_80_85:
		enable_uart1_modem_80to85_pins();
		break;
	}
}

/* Pinmux for UART2 */
static void enable_uart2_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_FIRDA_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_0_9_REG,
			PMX_PL_0_1_MASK,
			PMX_UART2_PL_0_1_VAL);
};

/* Pinmux for SSP1 */
static void enable_ssp1_17to20_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_MII_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_10_19_REG,
			PMX_PL_17_18_MASK | PMX_PL_19_MASK,
			PMX_SSP1_PL_17_18_19_20_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_20_29_REG,
			PMX_PL_20_MASK,
			PMX_SSP1_PL_17_18_19_20_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SSP1_PORT_SEL_MASK,
			PMX_SSP1_PORT_17_TO_20_VAL);
}

static void enable_ssp1_36to39_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_UART0_MODEM_MASK | PMX_SSP_CS_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_30_39_REG,
			PMX_PL_36_MASK | PMX_PL_37_38_MASK | PMX_PL_39_MASK,
			PMX_SSP1_PL_36_VAL | PMX_SSP1_PL_37_38_VAL |
			PMX_SSP1_PL_39_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SSP1_PORT_SEL_MASK,
			PMX_SSP1_PORT_36_TO_39_VAL);
}

static void enable_ssp1_48to51_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_TIMER_0_1_MASK | PMX_TIMER_2_3_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_40_49_REG,
			PMX_PL_48_49_MASK,
			PMX_SSP1_PL_48_49_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_50_59_REG,
			PMX_PL_50_51_MASK,
			PMX_SSP1_PL_50_51_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SSP1_PORT_SEL_MASK,
			PMX_SSP1_PORT_48_TO_51_VAL);
}

static void enable_ssp1_65to68_pins(void)
{
	pinmux_maskval(SPEAR320_IP_SEL_PAD_60_69_REG,
			PMX_PL_65_TO_68_MASK,
			PMX_SSP1_PL_65_TO_68_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SSP1_PORT_SEL_MASK,
			PMX_SSP1_PORT_65_TO_68_VAL);
}

static void enable_ssp1_94to97_pins(void)
{
	pinmux_maskval(SPEAR320_IP_SEL_PAD_90_99_REG,
			PMX_PL_94_95_MASK | PMX_PL_96_97_MASK,
			PMX_SSP1_PL_94_95_VAL | PMX_SSP1_PL_96_97_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SSP1_PORT_SEL_MASK,
			PMX_SSP1_PORT_94_TO_97_VAL);
}

static void enable_ssp1_pins(u32 mode)
{
	switch (mode) {
	case PMX_SSP_17_20:
		enable_ssp1_17to20_pins();
		break;
	case PMX_SSP_36_39:
		enable_ssp1_36to39_pins();
		break;
	case PMX_SSP_48_51:
		enable_ssp1_48to51_pins();
		break;
	case PMX_SSP_65_68:
		enable_ssp1_65to68_pins();
		break;
	case PMX_SSP_94_97:
		enable_ssp1_94to97_pins();
		break;
	}
}

/* Pinmux for SSP2 */
static void enable_ssp2_13to16_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_MII_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_10_19_REG,
			PMX_PL_13_14_MASK | PMX_PL_15_16_MASK,
			PMX_SSP2_PL_13_14_15_16_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SSP2_PORT_SEL_MASK,
			PMX_SSP2_PORT_13_TO_16_VAL);
}

static void enable_ssp2_32to35_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_SSP_CS_MASK | PMX_GPIO_PIN4_MASK |
			PMX_GPIO_PIN5_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_30_39_REG,
			PMX_PL_32_33_MASK | PMX_PL_34_MASK | PMX_PL_35_MASK,
			PMX_SSP2_PL_32_33_VAL | PMX_SSP2_PL_34_VAL |
			PMX_SSP2_PL_35_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SSP2_PORT_SEL_MASK,
			PMX_SSP2_PORT_32_TO_35_VAL);
}

static void enable_ssp2_44to47_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_TIMER_0_1_MASK | PMX_TIMER_2_3_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_40_49_REG,
			PMX_PL_44_45_MASK | PMX_PL_46_47_MASK,
			PMX_SSP2_PL_44_45_VAL | PMX_SSP2_PL_46_47_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SSP2_PORT_SEL_MASK,
			PMX_SSP2_PORT_44_TO_47_VAL);
}

static void enable_ssp2_61to64_pins(void)
{
	pinmux_maskval(SPEAR320_IP_SEL_PAD_60_69_REG,
			PMX_PL_61_TO_64_MASK,
			PMX_SSP2_PL_61_TO_64_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SSP2_PORT_SEL_MASK,
			PMX_SSP2_PORT_61_TO_64_VAL);
}

static void enable_ssp2_90to93_pins(void)
{
	pinmux_maskval(SPEAR320_IP_SEL_PAD_90_99_REG,
			PMX_PL_90_91_MASK | PMX_PL_92_93_MASK,
			PMX_SSP2_PL_90_91_VAL | PMX_SSP2_PL_92_93_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_SSP2_PORT_SEL_MASK,
			PMX_SSP2_PORT_90_TO_93_VAL);
}

static void enable_ssp2_pins(u32 mode)
{
	switch (mode) {
	case PMX_SSP_13_16:
		enable_ssp2_13to16_pins();
		break;
	case PMX_SSP_32_35:
		enable_ssp2_32to35_pins();
		break;
	case PMX_SSP_44_47:
		enable_ssp2_44to47_pins();
		break;
	case PMX_SSP_61_64:
		enable_ssp2_61to64_pins();
		break;
	case PMX_SSP_90_93:
		enable_ssp2_90to93_pins();
		break;
	}
}

/* Pinmux for ETH2 */
static void enable_mii2_pins(void)
{
	pinmux_maskval(SPEAR320_IP_SEL_PAD_80_89_REG,
			PMX_PL_80_TO_85_MASK | PMX_PL_86_87_MASK |
			PMX_PL_88_89_MASK,
			PMX_MII2_PL_80_TO_85_VAL | PMX_MII2_PL_86_87_VAL |
			PMX_MII2_PL_88_89_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_90_99_REG,
			PMX_PL_90_91_MASK | PMX_PL_92_93_MASK |
			PMX_PL_94_95_MASK | PMX_PL_96_97_MASK,
			PMX_MII2_PL_90_91_VAL | PMX_MII2_PL_92_93_VAL |
			PMX_MII2_PL_94_95_VAL | PMX_MII2_PL_96_97_VAL);
	pinmux_maskval(SPEAR320_EXT_CTRL_REG,
			(MAC_MODE_MASK << MAC2_MODE_SHIFT) |
			(MAC_MODE_MASK << MAC1_MODE_SHIFT) |
			MII_MDIO_MASK,
			(MAC_MODE_MII << MAC2_MODE_SHIFT) |
			(MAC_MODE_MII << MAC1_MODE_SHIFT) |
			MII_MDIO_81_VAL);
}

static void enable_smii2_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_MII_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_10_19_REG,
			PMX_PL_10_11_MASK,
			PMX_SMII_PL_10_11_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_20_29_REG,
			PMX_PL_21_TO_27_MASK,
			PMX_SMII_PL_21_TO_27_VAL);
	pinmux_maskval(SPEAR320_EXT_CTRL_REG,
			(MAC_MODE_MASK << MAC2_MODE_SHIFT) |
			(MAC_MODE_MASK << MAC1_MODE_SHIFT) |
			MII_MDIO_MASK,
			(MAC_MODE_SMII << MAC2_MODE_SHIFT)
			| (MAC_MODE_SMII << MAC1_MODE_SHIFT)
			| MII_MDIO_10_11_VAL);
}

static void enable_rmii2_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_MII_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_10_19_REG,
			PMX_PL_10_11_MASK | PMX_PL_13_14_MASK |
			PMX_PL_15_16_MASK | PMX_PL_17_18_MASK | PMX_PL_19_MASK,
			PMX_RMII_PL_10_11_VAL | PMX_RMII_PL_13_14_VAL |
			PMX_RMII_PL_15_16_VAL | PMX_RMII_PL_17_18_VAL |
			PMX_RMII_PL_19_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_20_29_REG,
			PMX_PL_20_MASK | PMX_PL_21_TO_27_MASK,
			PMX_RMII_PL_20_VAL | PMX_RMII_PL_21_TO_27_VAL);
	pinmux_maskval(SPEAR320_EXT_CTRL_REG,
			(MAC_MODE_MASK << MAC2_MODE_SHIFT) |
			(MAC_MODE_MASK << MAC1_MODE_SHIFT) |
			MII_MDIO_MASK,
			(MAC_MODE_RMII << MAC2_MODE_SHIFT)
			| (MAC_MODE_RMII << MAC1_MODE_SHIFT)
			| MII_MDIO_10_11_VAL);
}

static void enable_eth2_pins(u32 mode)
{
	switch (mode) {
	case PMX_ETH_MII:
		enable_mii2_pins();
		break;
	}
}

static void enable_eth1_eth2_pins(u32 mode)
{
	switch (mode) {
	case PMX_ETH_SMII:
		enable_smii2_pins();
		break;
	case PMX_ETH_RMII:
		enable_rmii2_pins();
		break;
	}
}

/* Pinmux for I2C1 */
static void enable_i2c1_8to9_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_SSP_CS_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_0_9_REG,
			PMX_PL_8_9_MASK,
			PMX_I2C1_PL_8_9_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_I2C1_PORT_SEL_MASK,
			PMX_I2C1_PORT_8_9_VAL);
}

static void enable_i2c1_98to99_pins(void)
{
	pinmux_maskval(SPEAR320_IP_SEL_PAD_90_99_REG,
			PMX_PL_98_MASK | PMX_PL_99_MASK,
			PMX_I2C1_PL_98_VAL | PMX_I2C1_PL_99_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_I2C1_PORT_SEL_MASK,
			PMX_I2C1_PORT_98_99_VAL);
}

static void enable_i2c1_pins(u32 mode)
{
	switch (mode) {
	case PMX_I2C_8_9:
		enable_i2c1_8to9_pins();
		break;
	case PMX_I2C_98_99:
		enable_i2c1_98to99_pins();
		break;
	}
}

/* Pinmux for I2C2 */
static void enable_i2c2_0to1_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_FIRDA_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_0_9_REG,
			PMX_PL_0_1_MASK,
			PMX_I2C2_PL_0_1_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_I2C2_PORT_SEL_MASK,
			PMX_I2C2_PORT_0_1_VAL);
}

static void enable_i2c2_2to3_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_UART0_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_0_9_REG,
			PMX_PL_2_3_MASK,
			PMX_I2C2_PL_2_3_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_I2C2_PORT_SEL_MASK,
			PMX_I2C2_PORT_2_3_VAL);
}

static void enable_i2c2_19to20_pins(void)
{
	pinmux_maskval(SPEAR320_RASSELECT_REG,
			PMX_MII_MASK,
			0);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_10_19_REG,
			PMX_PL_19_MASK,
			PMX_I2C2_PL_19_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_PAD_20_29_REG,
			PMX_PL_20_MASK,
			PMX_I2C2_PL_20_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_I2C2_PORT_SEL_MASK,
			PMX_I2C2_PORT_19_20_VAL);
}

static void enable_i2c2_75to76_pins(void)
{
	pinmux_maskval(SPEAR320_IP_SEL_PAD_70_79_REG,
			PMX_PL_75_76_MASK,
			PMX_I2C2_PL_75_76_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_I2C2_PORT_SEL_MASK,
			PMX_I2C2_PORT_75_76_VAL);
}

static void enable_i2c2_96to97_pins(void)
{
	pinmux_maskval(SPEAR320_IP_SEL_PAD_90_99_REG,
			PMX_PL_96_97_MASK,
			PMX_I2C2_PL_96_97_VAL);
	pinmux_maskval(SPEAR320_IP_SEL_MIX_PAD_REG,
			PMX_I2C2_PORT_SEL_MASK,
			PMX_I2C2_PORT_96_97_VAL);
}

static void enable_i2c2_pins(u32 mode)
{
	switch (mode) {
	case PMX_I2C_0_1:
		enable_i2c2_0to1_pins();
		break;
	case PMX_I2C_2_3:
		enable_i2c2_2to3_pins();
		break;
	case PMX_I2C_19_20:
		enable_i2c2_19to20_pins();
		break;
	case PMX_I2C_75_76:
		enable_i2c2_75to76_pins();
		break;
	case PMX_I2C_96_97:
		enable_i2c2_96to97_pins();
		break;
	}
}

/**
 * spear320_select_mode
 * @mode:	SoC mode to e selected
 */
void spear320_select_mode(u32 mode)
{
	if (mode == SPEAR320_EXTENDED_MODE) {
		pinmux_maskval(SPEAR320_EXT_CTRL_REG,
				EXT_MODE_MASK,
				EXT_MODE_MASK);
	} else
		printf("deprecated: Please run device only in ext mode\n");
}

/**
 * spear320_pins_default: Select a default safe mode as startup
 * Generally, all pins are enabled in input mode at initialization. This can be
 * done either by
 * - enabling gpio's and keeping all pins in gpio inputs
 * - a platform specific way.
 */
void spear320_pins_default(void)
{
}

/**
 * spear320_enable_pins - enable pins for fixed peripherals on spear3xx devices
 * @ip:		Peripheral index
 * @mode:	Mode in which peripheral has to run (16bit/8bit etc)
 *
 * Enable the pins for fixed peripherals on spear3xx devices.
 * mode represents the mode in which the peripheral may work and may result in
 * different pins being enabled. eg GMII mode and RGMII mode may need different
 * pins on devices to be enabled
 */
void spear320_enable_pins(u32 ip, u32 mode)
{
	if (PMX_UART2 == ip)
		enable_uart2_pins();
	else if (PMX_EMI == ip)
		enable_emi_pins();
	else if (PMX_FSMCNAND == ip)
		enable_nand_pins(mode);
	else if (PMX_SDMMC == ip)
		enable_sdmmc_pins(mode);
	else if (PMX_UART1 == ip)
		enable_uart1_pins(mode);
	else if (PMX_I2C1 == ip)
		enable_i2c1_pins(mode);
	else if (PMX_I2C2 == ip)
		enable_i2c2_pins(mode);
	else if (PMX_SSP1 == ip)
		enable_ssp1_pins(mode);
	else if (PMX_SSP2 == ip)
		enable_ssp2_pins(mode);
	else if (PMX_ETH2 == ip)
		enable_eth2_pins(mode);
	else if (PMX_ETH1_ETH2 == ip)
		enable_eth1_eth2_pins(mode);
	else if ((PMX_I2C0 == ip) || (PMX_SSP0 == ip) || \
			(PMX_ETH0 == ip) || (PMX_UART0 == ip))
		spear3xx_enable_pins(ip, mode);
}

static void configure_gpio(u32 plgpio)
{
	if (plgpio > SPEAR3XX_MAX_PLGPIOS)
		return;

	/* Set the pin to GPIO IN mode */
	pinmux_set_bit(plgpio, SPEAR320_GPIO_EN0);

	/* Select GPIO mode */
	pinmux_set_bit(plgpio, SPEAR320_GPIO_SELECT0);

	/* Select RAS from Fixed Part / RAS */
	if (plgpio < 2) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_FIRDA_MASK, 0);
	} else if (plgpio < 4) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_UART0_MASK, 0);
	} else if (plgpio < 6) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_I2C_MASK, 0);
	} else if (plgpio < 10) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_SSP_MASK, 0);
	} else if (plgpio < 28) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_MII_MASK, 0);
	} else if (plgpio < 29) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_GPIO_PIN0_MASK, 0);
	} else if (plgpio < 30) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_GPIO_PIN1_MASK, 0);
	} else if (plgpio < 31) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_GPIO_PIN2_MASK, 0);
	} else if (plgpio < 32) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_GPIO_PIN3_MASK, 0);
	} else if (plgpio < 33) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_GPIO_PIN4_MASK, 0);
	} else if (plgpio < 34) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_GPIO_PIN5_MASK, 0);
	} else if (plgpio < 37) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_SSP_CS_MASK, 0);
	} else if (plgpio < 43) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_UART0_MODEM_MASK, 0);
	} else if (plgpio < 51) {
		pinmux_maskval(SPEAR3XX_FUNC_ENB_REG,
				PMX_TIMER_0_1_MASK | PMX_TIMER_2_3_MASK, 0);
	}
}

/**
 * spear320_configure_pin - Configure pin on spear320 devices
 * @plgpio:	Pin Number (plgpio number)
 * @mode:	Pull UP, Pull DOWN, plgpio IN, plgpio OUT etc
 */
void spear320_configure_pin(u32 plgpio, u32 mode)
{
	if (PMX_GPIO == mode)
		configure_gpio(plgpio);
}

/**
 * spear320_plgpio_get - Get the gpio input
 * @plgpio:	Pin Number (plgpio number)
 */
int spear320_plgpio_get(u32 plgpio)
{
	if (plgpio > SPEAR3XX_MAX_PLGPIOS)
		return -1;

	/* Set the pin to GPIO IN mode */
	pinmux_set_bit(plgpio, SPEAR320_GPIO_EN0);

	return pinmux_test_bit(plgpio, SPEAR320_GPIO_IN0);
}

/**
 * spear320_plgpio_set - Set the gpio value
 * @plgpio:	Pin Number (plgpio number)
 */
void spear320_plgpio_set(u32 plgpio, u32 val)
{
	if (plgpio > SPEAR3XX_MAX_PLGPIOS)
		return;

	if (val & 0x1)
		pinmux_set_bit(plgpio, SPEAR320_GPIO_OUT0);
	else
		pinmux_clear_bit(plgpio, SPEAR320_GPIO_OUT0);

	/* Set the pin to GPIO OUT mode */
	pinmux_clear_bit(plgpio, SPEAR320_GPIO_EN0);
}
