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

#ifndef _ASM_ARCH_SPEAR320_H
#define _ASM_ARCH_SPEAR320_H

#include <asm/arch/spear3xx.h>

#define CONFIG_SPEAR_EMIBASE			0x40000000
#define CONFIG_SYS_FSMC_BASE			0x4C000000
#define CONFIG_SYS_NAND_CLE			(1 << 16)
#define CONFIG_SYS_NAND_ALE			(1 << 17)

#define CONFIG_SYS_MMC_BASE			0x70000000
#define CONFIG_SYS_MACB0_BASE			0xAA000000
#define CONFIG_SYS_MACB1_BASE			0xAB000000
#define CONFIG_SPEAR_RASBASE			0xB3000000

/* SPEAr320 SoC device modes */
#define SPEAR320_AUTO_NET_SMII_MODE	(1 << 0)
#define SPEAR320_AUTO_NET_MII_MODE	(1 << 1)
#define SPEAR320_AUTO_EXP_MODE		(1 << 2)
#define SPEAR320_SMALL_PRINTERS_MODE	(1 << 3)
#define SPEAR320_EXTENDED_MODE		(1 << 4)

/* SPEAr320 RAS misc space registers and bitmasks */
#define SPEAR320_BOOT_REG		(CONFIG_SPEAR_RASBASE + 0x0)
	#define SPEAR320_BOOTSHFT		0x0
	#define SPEAR320_BOOTMASK		0xF
	#define SPEAR320_USBBOOT		0x0
	#define SPEAR320_TFTPI2CBOOT		0x1
	#define SPEAR320_TFTPSPIBOOT		0x2
	#define SPEAR320_SNORBOOT		0x3
	#define SPEAR320_PNOR8BOOT		0x4
	#define SPEAR320_PNOR16BOOT		0x5
	#define SPEAR320_NAND8BOOT		0x6
	#define SPEAR320_NAND16BOOT		0x7
	#define SPEAR320_UARTBOOT		0xA
	#define SPEAR320_PNOR8NOACKBOOT		0xC
	#define SPEAR320_PNOR16NOACKBOOT	0xD

#define SPEAR320_RASSELECT_REG		(CONFIG_SPEAR_RASBASE + 0x000C)

#define SPEAR320_CONTROL_REG		(CONFIG_SPEAR_RASBASE + 0x0010)
	#define MII_ENB_SHFT			5

/* Extended mode registers and their offsets */
#define SPEAR320_EXT_CTRL_REG		(CONFIG_SPEAR_RASBASE + 0x0018)
	#define EXT_MODE_MASK			(1 << 0)
	#define MII_MDIO_MASK			(1 << 4)
	#define MII_MDIO_10_11_VAL		0
	#define MII_MDIO_81_VAL			(1 << 4)
	#define EMI_FSMC_DYNAMIC_MUX_MASK	(1 << 5)
	#define MAC_MODE_MII			0
	#define MAC_MODE_RMII			1
	#define MAC_MODE_SMII			2
	#define MAC_MODE_SS_SMII		3
	#define MAC_MODE_MASK			0x3
	#define MAC1_MODE_SHIFT			16
	#define MAC2_MODE_SHIFT			18

#define SPEAR320_GPIO_SELECT0		(CONFIG_SPEAR_RASBASE + 0x0024)
#define SPEAR320_GPIO_OUT0		(CONFIG_SPEAR_RASBASE + 0x0034)
#define SPEAR320_GPIO_EN0		(CONFIG_SPEAR_RASBASE + 0x0044)
#define SPEAR320_GPIO_IN0		(CONFIG_SPEAR_RASBASE + 0x0054)

#define SPEAR320_IP_SEL_PAD_0_9_REG	(CONFIG_SPEAR_RASBASE + 0x00A4)
	#define PMX_PL_0_1_MASK			(0x3F << 0)
	#define PMX_UART2_PL_0_1_VAL		0x0
	#define PMX_I2C2_PL_0_1_VAL		(0x4 | (0x4 << 3))

	#define PMX_PL_2_3_MASK			(0x3F << 6)
	#define PMX_I2C2_PL_2_3_VAL		0x0
	#define PMX_UART6_PL_2_3_VAL		((0x1 << 6) | (0x1 << 9))
	#define PMX_UART1_ENH_PL_2_3_VAL	((0x4 << 6) | (0x4 << 9))

	#define PMX_PL_4_5_MASK			(0x3F << 12)
	#define PMX_UART5_PL_4_5_VAL		((0x1 << 12) | (0x1 << 15))
	#define PMX_UART1_ENH_PL_4_5_VAL	((0x4 << 12) | (0x4 << 15))
	#define PMX_PL_5_MASK			(0x7 << 15)
	#define PMX_TOUCH_Y_PL_5_VAL		0x0

	#define PMX_PL_6_7_MASK			(0x3F << 18)
	#define PMX_PL_6_MASK			(0x7 << 18)
	#define PMX_PL_7_MASK			(0x7 << 21)
	#define PMX_UART4_PL_6_7_VAL		((0x1 << 18) | (0x1 << 21))
	#define PMX_PWM_3_PL_6_VAL		(0x2 << 18)
	#define PMX_PWM_2_PL_7_VAL		(0x2 << 21)
	#define PMX_UART1_ENH_PL_6_7_VAL	((0x4 << 18) | (0x4 << 21))

	#define PMX_PL_8_9_MASK			(0x3F << 24)
	#define PMX_UART3_PL_8_9_VAL		((0x1 << 24) | (0x1 << 27))
	#define PMX_PWM_0_1_PL_8_9_VAL		((0x2 << 24) | (0x2 << 27))
	#define PMX_I2C1_PL_8_9_VAL		((0x4 << 24) | (0x4 << 27))

#define SPEAR320_IP_SEL_PAD_10_19_REG	(CONFIG_SPEAR_RASBASE + 0x00A8)
	#define PMX_PL_10_11_MASK		(0x3F << 0)
	#define PMX_SMII_PL_10_11_VAL		0
	#define PMX_RMII_PL_10_11_VAL		((0x4 << 0) | (0x4 << 3))

	#define PMX_PL_12_MASK			(0x7 << 6)
	#define PMX_PWM3_PL_12_VAL		0
	#define PMX_SDHCI_CD_PL_12_VAL		(0x4 << 6)

	#define PMX_PL_13_14_MASK		(0x3F << 9)
	#define PMX_PL_13_MASK			(0x7 << 9)
	#define PMX_PL_14_MASK			(0x7 << 12)
	#define PMX_SSP2_PL_13_14_15_16_VAL	0
	#define PMX_UART4_PL_13_14_VAL		((0x1 << 9) | (0x1 << 12))
	#define PMX_RMII_PL_13_14_VAL		((0x4 << 9) | (0x4 << 12))
	#define PMX_PWM2_PL_13_VAL		(0x2 << 9)
	#define PMX_PWM1_PL_14_VAL		(0x2 << 12)

	#define PMX_PL_15_MASK			(0x7 << 15)
	#define PMX_PWM0_PL_15_VAL		(0x2 << 15)
	#define PMX_PL_15_16_MASK		(0x3F << 15)
	#define PMX_UART3_PL_15_16_VAL		((0x1 << 15) | (0x1 << 18))
	#define PMX_RMII_PL_15_16_VAL		((0x4 << 15) | (0x4 << 18))

	#define PMX_PL_17_18_MASK		(0x3F << 21)
	#define PMX_SSP1_PL_17_18_19_20_VAL	0
	#define PMX_RMII_PL_17_18_VAL		((0x4 << 21) | (0x4 << 24))

	#define PMX_PL_19_MASK			(0x7 << 27)
	#define PMX_I2C2_PL_19_VAL		(0x1 << 27)
	#define PMX_RMII_PL_19_VAL		(0x4 << 27)

#define SPEAR320_IP_SEL_PAD_20_29_REG	(CONFIG_SPEAR_RASBASE + 0x00AC)
	#define PMX_PL_20_MASK			(0x7 << 0)
	#define PMX_I2C2_PL_20_VAL		(0x1 << 0)
	#define PMX_RMII_PL_20_VAL		(0x4 << 0)

	#define PMX_PL_21_TO_27_MASK		(0x1FFFFF << 3)
	#define PMX_SMII_PL_21_TO_27_VAL	0
	#define PMX_RMII_PL_21_TO_27_VAL	((0x4 << 3) | (0x4 << 6) | \
						(0x4 << 9) | (0x4 << 12) | \
						(0x4 << 15) | (0x4 << 18) | \
						(0x4 << 21))

	#define PMX_PL_28_29_MASK		(0x3F << 24)
	#define PMX_PL_28_MASK			(0x7 << 24)
	#define PMX_PL_29_MASK			(0x7 << 27)
	#define PMX_UART1_PL_28_29_VAL		0
	#define PMX_PWM_3_PL_28_VAL		(0x4 << 24)
	#define PMX_PWM_2_PL_29_VAL		(0x4 << 27)

#define SPEAR320_IP_SEL_PAD_30_39_REG	(CONFIG_SPEAR_RASBASE + 0x00B0)
	#define PMX_PL_30_31_MASK		(0x3F << 0)
	#define PMX_CAN1_PL_30_31_VAL		(0)
	#define PMX_PL_30_MASK			(0x7 << 0)
	#define PMX_PL_31_MASK			(0x7 << 3)
	#define PMX_PWM1_EXT_PL_30_VAL		(0x4 << 0)
	#define PMX_PWM0_EXT_PL_31_VAL		(0x4 << 3)
	#define PMX_UART1_ENH_PL_31_VAL		(0x3 << 3)

	#define PMX_PL_32_33_MASK		(0x3F << 6)
	#define PMX_CAN0_PL_32_33_VAL		0
	#define PMX_UART1_ENH_PL_32_33_VAL	((0x3 << 6) | (0x3 << 9))
	#define PMX_SSP2_PL_32_33_VAL		((0x4 << 6) | (0x4 << 9))

	#define PMX_PL_34_MASK			(0x7 << 12)
	#define PMX_PWM2_PL_34_VAL		0
	#define PMX_UART1_ENH_PL_34_VAL		(0x2 << 12)
	#define PMX_SSP2_PL_34_VAL		(0x4 << 12)

	#define PMX_PL_35_MASK			(0x7 << 15)
	#define PMX_I2S_REF_CLK_PL_35_VAL	0
	#define PMX_UART1_ENH_PL_35_VAL		(0x2 << 15)
	#define PMX_SSP2_PL_35_VAL		(0x4 << 15)

	#define PMX_PL_36_MASK			(0x7 << 18)
	#define PMX_TOUCH_X_PL_36_VAL		0
	#define PMX_UART1_ENH_PL_36_VAL		(0x2 << 18)
	#define PMX_SSP1_PL_36_VAL		(0x4 << 18)

	#define PMX_PL_37_38_MASK		(0x3F << 21)
	#define PMX_PWM0_1_PL_37_38_VAL		0
	#define PMX_UART5_PL_37_38_VAL		((0x2 << 21) | (0x2 << 24))
	#define PMX_SSP1_PL_37_38_VAL		((0x4 << 21) | (0x4 << 24))

	#define PMX_PL_39_MASK			(0x7 << 27)
	#define PMX_I2S_PL_39_VAL		0
	#define PMX_UART4_PL_39_VAL		(0x2 << 27)
	#define PMX_SSP1_PL_39_VAL		(0x4 << 27)

#define SPEAR320_IP_SEL_PAD_40_49_REG	(CONFIG_SPEAR_RASBASE + 0x00B4)
	#define PMX_PL_40_MASK			(0x7 << 0)
	#define PMX_I2S_PL_40_VAL		0
	#define PMX_UART4_PL_40_VAL		(0x2 << 0)
	#define PMX_PWM3_PL_40_VAL		(0x4 << 0)

	#define PMX_PL_41_42_MASK		(0x3F << 3)
	#define PMX_PL_41_MASK			(0x7 << 3)
	#define PMX_PL_42_MASK			(0x7 << 6)
	#define PMX_I2S_PL_41_42_VAL		0
	#define PMX_UART3_PL_41_42_VAL		((0x2 << 3) | (0x2 << 6))
	#define PMX_PWM2_PL_41_VAL		(0x4 << 3)
	#define PMX_PWM1_PL_42_VAL		(0x4 << 6)

	#define PMX_PL_43_MASK			(0x7 << 9)
	#define PMX_SDHCI_PL_43_VAL		0
	#define PMX_UART1_ENH_PL_43_VAL		(0x2 << 9)
	#define PMX_PWM0_PL_43_VAL		(0x4 << 9)

	#define PMX_PL_44_45_MASK		(0x3F << 12)
	#define PMX_SDHCI_PL_44_45_VAL	0
	#define PMX_UART1_ENH_PL_44_45_VAL	((0x2 << 12) | (0x2 << 15))
	#define PMX_SSP2_PL_44_45_VAL		((0x4 << 12) | (0x4 << 15))

	#define PMX_PL_46_47_MASK		(0x3F << 18)
	#define PMX_SDHCI_PL_46_47_VAL	0
	#define PMX_FSMC_EMI_PL_46_47_VAL	((0x2 << 18) | (0x2 << 21))
	#define PMX_SSP2_PL_46_47_VAL		((0x4 << 18) | (0x4 << 21))

	#define PMX_PL_48_49_MASK		(0x3F << 24)
	#define PMX_SDHCI_PL_48_49_VAL	0
	#define PMX_FSMC_EMI_PL_48_49_VAL	((0x2 << 24) | (0x2 << 27))
	#define PMX_SSP1_PL_48_49_VAL		((0x4 << 24) | (0x4 << 27))

#define SPEAR320_IP_SEL_PAD_50_59_REG	(CONFIG_SPEAR_RASBASE + 0x00B8)
	#define PMX_PL_50_51_MASK		(0x3F << 0)
	#define PMX_EMI_PL_50_51_VAL		((0x2 << 0) | (0x2 << 3))
	#define PMX_SSP1_PL_50_51_VAL		((0x4 << 0) | (0x4 << 3))
	#define PMX_PL_50_MASK			(0x7 << 0)
	#define PMX_PL_51_MASK			(0x7 << 3)
	#define PMX_SDHCI_PL_50_VAL		0
	#define PMX_SDHCI_CD_PL_51_VAL		0

	#define PMX_PL_52_53_MASK		(0x3F << 6)
	#define PMX_FSMC_PL_52_53_VAL		0
	#define PMX_EMI_PL_52_53_VAL		((0x2 << 6) | (0x2 << 9))
	#define PMX_UART3_PL_52_53_VAL		((0x4 << 6) | (0x4 << 9))

	#define PMX_PL_54_55_56_MASK		(0x1FF << 12)
	#define PMX_FSMC_EMI_PL_54_55_56_VAL	((0x2 << 12) | (0x2 << 15) | \
						(0x2 << 18))

	#define PMX_PL_57_MASK			(0x7 << 21)
	#define PMX_FSMC_PL_57_VAL		0
	#define PMX_PWM3_PL_57_VAL		(0x4 << 21)

	#define PMX_PL_58_59_MASK		(0x3F << 24)
	#define PMX_PL_58_MASK			(0x7 << 24)
	#define PMX_PL_59_MASK			(0x7 << 27)
	#define PMX_FSMC_EMI_PL_58_59_VAL	((0x2 << 24) | (0x2 << 27))
	#define PMX_PWM2_PL_58_VAL		(0x4 << 24)
	#define PMX_PWM1_PL_59_VAL		(0x4 << 27)

#define SPEAR320_IP_SEL_PAD_60_69_REG	(CONFIG_SPEAR_RASBASE + 0x00BC)
	#define PMX_PL_60_MASK			(0x7 << 0)
	#define PMX_FSMC_PL_60_VAL		0
	#define PMX_PWM0_PL_60_VAL		(0x4 << 0)

	#define PMX_PL_61_TO_64_MASK		(0xFFF << 3)
	#define PMX_FSMC_PL_61_TO_64_VAL	((0x2 << 3) | (0x2 << 6) | \
						(0x2 << 9) | (0x2 << 12))
	#define PMX_SSP2_PL_61_TO_64_VAL	((0x4 << 3) | (0x4 << 6) | \
						(0x4 << 9) | (0x4 << 12))

	#define PMX_PL_65_TO_68_MASK		(0xFFF << 15)
	#define PMX_FSMC_PL_65_TO_68_VAL	((0x2 << 15) | (0x2 << 18) | \
						(0x2 << 21) | (0x2 << 24))
	#define PMX_SSP1_PL_65_TO_68_VAL	((0x4 << 15) | (0x4 << 18) | \
						(0x4 << 21) | (0x4 << 24))

	#define PMX_PL_69_MASK			(0x7 << 27)
	#define PMX_CLCD_PL_69_VAL		(0)
	#define PMX_EMI_PL_69_VAL		(0x2 << 27)
	#define PMX_SPP_PL_69_VAL		(0x3 << 27)
	#define PMX_UART5_PL_69_VAL		(0x4 << 27)

#define SPEAR320_IP_SEL_PAD_70_79_REG	(CONFIG_SPEAR_RASBASE + 0x00C0)
	#define PMX_PL_70_MASK			(0x7 << 0)
	#define PMX_CLCD_PL_70_VAL		(0)
	#define PMX_FSMC_EMI_PL_70_VAL		(0x2 << 0)
	#define PMX_SPP_PL_70_VAL		(0x3 << 0)
	#define PMX_UART5_PL_70_VAL		(0x4 << 0)

	#define PMX_PL_71_72_MASK		(0x3F << 3)
	#define PMX_CLCD_PL_71_72_VAL		(0)
	#define PMX_FSMC_EMI_PL_71_72_VAL	((0x2 << 3) | (0x2 << 6))
	#define PMX_SPP_PL_71_72_VAL		((0x3 << 3) | (0x3 << 6))
	#define PMX_UART4_PL_71_72_VAL		((0x4 << 3) | (0x4 << 6))

	#define PMX_PL_73_MASK			(0x7 << 9)
	#define PMX_CLCD_PL_73_VAL		(0)
	#define PMX_FSMC_EMI_PL_73_VAL		(0x2 << 9)
	#define PMX_SPP_PL_73_VAL		(0x3 << 9)
	#define PMX_UART3_PL_73_VAL		(0x4 << 9)

	#define PMX_PL_74_MASK			(0x7 << 12)
	#define PMX_CLCD_PL_74_VAL		(0)
	#define PMX_EMI_PL_74_VAL		(0x2 << 12)
	#define PMX_SPP_PL_74_VAL		(0x3 << 12)
	#define PMX_UART3_PL_74_VAL		(0x4 << 12)

	#define PMX_PL_75_76_MASK		(0x3F << 15)
	#define PMX_CLCD_PL_75_76_VAL		(0)
	#define PMX_EMI_PL_75_76_VAL		((0x2 << 15) | (0x2 << 18))
	#define PMX_SPP_PL_75_76_VAL		((0x3 << 15) | (0x3 << 18))
	#define PMX_I2C2_PL_75_76_VAL		((0x4 << 15) | (0x4 << 18))

	#define PMX_PL_77_78_79_MASK		(0x1FF << 21)
	#define PMX_CLCD_PL_77_78_79_VAL	(0)
	#define PMX_EMI_PL_77_78_79_VAL		((0x2 << 21) | (0x2 << 24) | \
						(0x2 << 27))
	#define PMX_SPP_PL_77_78_79_VAL		((0x3 << 21) | (0x3 << 24) | \
						(0x3 << 27))
	#define PMX_RS485_PL_77_78_79_VAL	((0x4 << 21) | (0x4 << 24) | \
						(0x4 << 27))

#define SPEAR320_IP_SEL_PAD_80_89_REG	(CONFIG_SPEAR_RASBASE + 0x00C4)
	#define PMX_PL_80_TO_85_MASK		(0x3FFFF << 0)
	#define PMX_CLCD_PL_80_TO_85_VAL	0
	#define PMX_MII2_PL_80_TO_85_VAL	((0x1 << 0) | (0x1 << 3) | \
						(0x1 << 6) | (0x1 << 9) | \
						(0x1 << 12) | (0x1 << 15))
	#define PMX_EMI_PL_80_TO_85_VAL		((0x2 << 0) | (0x2 << 3) | \
						(0x2 << 6) | (0x2 << 9) | \
						(0x2 << 12) | (0x2 << 15))
	#define PMX_SPP_PL_80_TO_85_VAL		((0x3 << 0) | (0x3 << 3) | \
						(0x3 << 6) | (0x3 << 9) | \
						(0x3 << 12) | (0x3 << 15))
	#define PMX_UART1_ENH_PL_80_TO_85_VAL	((0x4 << 0) | (0x4 << 3) | \
						(0x4 << 6) | (0x4 << 9) | \
						(0x4 << 12) | (0x4 << 15))

	#define PMX_PL_86_87_MASK		(0x3F << 18)
	#define PMX_PL_86_MASK			(0x7 << 18)
	#define PMX_PL_87_MASK			(0x7 << 21)
	#define PMX_CLCD_PL_86_87_VAL		0
	#define PMX_MII2_PL_86_87_VAL		((0x1 << 18) | (0x1 << 21))
	#define PMX_EMI_PL_86_87_VAL		((0x2 << 18) | (0x2 << 21))
	#define PMX_PWM3_PL_86_VAL		(0x4 << 18)
	#define PMX_PWM2_PL_87_VAL		(0x4 << 21)

	#define PMX_PL_88_89_MASK		(0x3F << 24)
	#define PMX_CLCD_PL_88_89_VAL		0
	#define PMX_MII2_PL_88_89_VAL		((0x1 << 24) | (0x1 << 27))
	#define PMX_EMI_PL_88_89_VAL		((0x2 << 24) | (0x2 << 27))
	#define PMX_UART6_PL_88_89_VAL		((0x3 << 24) | (0x3 << 27))
	#define PMX_PWM0_1_PL_88_89_VAL		((0x4 << 24) | (0x4 << 27))

#define SPEAR320_IP_SEL_PAD_90_99_REG	(CONFIG_SPEAR_RASBASE + 0x00C8)
	#define PMX_PL_90_91_MASK		(0x3F << 0)
	#define PMX_CLCD_PL_90_91_VAL		0
	#define PMX_MII2_PL_90_91_VAL		((0x1 << 0) | (0x1 << 3))
	#define PMX_EMI1_PL_90_91_VAL		((0x2 << 0) | (0x2 << 3))
	#define PMX_UART5_PL_90_91_VAL		((0x3 << 0) | (0x3 << 3))
	#define PMX_SSP2_PL_90_91_VAL		((0x4 << 0) | (0x4 << 3))

	#define PMX_PL_92_93_MASK		(0x3F << 6)
	#define PMX_CLCD_PL_92_93_VAL		0
	#define PMX_MII2_PL_92_93_VAL		((0x1 << 6) | (0x1 << 9))
	#define PMX_EMI1_PL_92_93_VAL		((0x2 << 6) | (0x2 << 9))
	#define PMX_UART4_PL_92_93_VAL		((0x3 << 6) | (0x3 << 9))
	#define PMX_SSP2_PL_92_93_VAL		((0x4 << 6) | (0x4 << 9))

	#define PMX_PL_94_95_MASK		(0x3F << 12)
	#define PMX_CLCD_PL_94_95_VAL		0
	#define PMX_MII2_PL_94_95_VAL		((0x1 << 12) | (0x1 << 15))
	#define PMX_EMI1_PL_94_95_VAL		((0x2 << 12) | (0x2 << 15))
	#define PMX_UART3_PL_94_95_VAL		((0x3 << 12) | (0x3 << 15))
	#define PMX_SSP1_PL_94_95_VAL		((0x4 << 12) | (0x4 << 15))

	#define PMX_PL_96_97_MASK		(0x3F << 18)
	#define PMX_CLCD_PL_96_97_VAL		0
	#define PMX_MII2_PL_96_97_VAL		((0x1 << 18) | (0x1 << 21))
	#define PMX_EMI1_PL_96_97_VAL		((0x2 << 18) | (0x2 << 21))
	#define PMX_I2C2_PL_96_97_VAL		((0x3 << 18) | (0x3 << 21))
	#define PMX_SSP1_PL_96_97_VAL		((0x4 << 18) | (0x4 << 21))

	#define PMX_PL_98_MASK			(0x7 << 24)
	#define PMX_CLCD_PL_98_VAL		0
	#define PMX_I2C1_PL_98_VAL		(0x2 << 24)
	#define PMX_UART3_PL_98_VAL		(0x4 << 24)

	#define PMX_PL_99_MASK			(0x7 << 27)
	#define PMX_SDHCI_PL_99_VAL		0
	#define PMX_I2C1_PL_99_VAL		(0x2 << 27)
	#define PMX_UART3_PL_99_VAL		(0x4 << 27)

#define SPEAR320_IP_SEL_MIX_PAD_REG	(CONFIG_SPEAR_RASBASE + 0x00CC)
	#define PMX_PL_100_101_MASK		(0x3F << 0)
	#define PMX_SDHCI_PL_100_101_VAL	0
	#define PMX_UART4_PL_100_101_VAL	((0x4 << 0) | (0x4 << 3))

	#define PMX_SSP1_PORT_SEL_MASK		(0x7 << 8)
	#define PMX_SSP1_PORT_94_TO_97_VAL	0
	#define PMX_SSP1_PORT_65_TO_68_VAL	(0x1 << 8)
	#define PMX_SSP1_PORT_48_TO_51_VAL	(0x2 << 8)
	#define PMX_SSP1_PORT_36_TO_39_VAL	(0x3 << 8)
	#define PMX_SSP1_PORT_17_TO_20_VAL	(0x4 << 8)

	#define PMX_SSP2_PORT_SEL_MASK		(0x7 << 11)
	#define PMX_SSP2_PORT_90_TO_93_VAL	0
	#define PMX_SSP2_PORT_61_TO_64_VAL	(0x1 << 11)
	#define PMX_SSP2_PORT_44_TO_47_VAL	(0x2 << 11)
	#define PMX_SSP2_PORT_32_TO_35_VAL	(0x3 << 11)
	#define PMX_SSP2_PORT_13_TO_16_VAL	(0x4 << 11)

	#define PMX_UART1_ENH_PORT_SEL_MASK		(0x3 << 14)
	#define PMX_UART1_ENH_PORT_81_TO_85_VAL		0
	#define PMX_UART1_ENH_PORT_44_45_34_36_VAL	(0x1 << 14)
	#define PMX_UART1_ENH_PORT_32_TO_34_36_VAL	(0x2 << 14)
	#define PMX_UART1_ENH_PORT_3_TO_5_7_VAL		(0x3 << 14)

	#define PMX_UART3_PORT_SEL_MASK		(0x7 << 16)
	#define PMX_UART3_PORT_94_VAL		0
	#define PMX_UART3_PORT_73_VAL		(0x1 << 16)
	#define PMX_UART3_PORT_52_VAL		(0x2 << 16)
	#define PMX_UART3_PORT_41_VAL		(0x3 << 16)
	#define PMX_UART3_PORT_15_VAL		(0x4 << 16)
	#define PMX_UART3_PORT_8_VAL		(0x5 << 16)
	#define PMX_UART3_PORT_99_VAL		(0x6 << 16)

	#define PMX_UART4_PORT_SEL_MASK		(0x7 << 19)
	#define PMX_UART4_PORT_92_VAL		0
	#define PMX_UART4_PORT_71_VAL		(0x1 << 19)
	#define PMX_UART4_PORT_39_VAL		(0x2 << 19)
	#define PMX_UART4_PORT_13_VAL		(0x3 << 19)
	#define PMX_UART4_PORT_6_VAL		(0x4 << 19)
	#define PMX_UART4_PORT_101_VAL		(0x5 << 19)

	#define PMX_UART5_PORT_SEL_MASK		(0x3 << 22)
	#define PMX_UART5_PORT_90_VAL		0
	#define PMX_UART5_PORT_69_VAL		(0x1 << 22)
	#define PMX_UART5_PORT_37_VAL		(0x2 << 22)
	#define PMX_UART5_PORT_4_VAL		(0x3 << 22)

	#define PMX_UART6_PORT_SEL_MASK		(0x1 << 24)
	#define PMX_UART6_PORT_88_VAL		0
	#define PMX_UART6_PORT_2_VAL		(0x1 << 24)

	#define PMX_I2C1_PORT_SEL_MASK		(0x1 << 25)
	#define PMX_I2C1_PORT_8_9_VAL		0
	#define PMX_I2C1_PORT_98_99_VAL		(0x1 << 25)

	#define PMX_I2C2_PORT_SEL_MASK		(0x3 << 26)
	#define PMX_I2C2_PORT_96_97_VAL		0
	#define PMX_I2C2_PORT_75_76_VAL		(0x1 << 26)
	#define PMX_I2C2_PORT_19_20_VAL		(0x2 << 26)
	#define PMX_I2C2_PORT_2_3_VAL		(0x3 << 26)
	#define PMX_I2C2_PORT_0_1_VAL		(0x4 << 26)

	#define PMX_SDHCI_CD_PORT_SEL_MASK	(0x1 << 29)
	#define PMX_SDHCI_CD_PORT_12_VAL	0
	#define PMX_SDHCI_CD_PORT_51_VAL	(0x1 << 29)

#define SNOR_BOOT_SUPPORTED			1
#define NAND_BOOT_SUPPORTED			1
#define PNOR_BOOT_SUPPORTED			1
#define USBD_BOOT_SUPPORTED			1
#define TFTP_BOOT_SUPPORTED			1
#define UART_BOOT_SUPPORTED			1
#define MMC_BOOT_SUPPORTED			0
#define SPI_BOOT_SUPPORTED			0
#define I2C_BOOT_SUPPORTED			0

#define SPEAR3XX_FUNC_ENB_REG		SPEAR320_RASSELECT_REG

/* externs related to pinmux */
extern void spear320_pins_default(void);
extern void spear320_select_mode(u32 mode);
extern void spear320_enable_pins(u32 ip, u32 mode);
extern void spear320_configure_pin(u32 plgpio, u32 mode);
extern void spear320_plgpio_set(u32 plgpio, u32 val);
extern int  spear320_plgpio_get(u32 plgpio);

#endif
