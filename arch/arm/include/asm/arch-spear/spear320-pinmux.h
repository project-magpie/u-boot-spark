/*
 * SPEAr320 Machine specific definition
 *
 * Copyright (C) 2009 ST Microelectronics
 * Viresh Kumar<viresh.kumar@st.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __MACH_SPEAR320_H
#define __MACH_SPEAR320_H

#define SPEAR320_SOC_CONFIG_BASE	0xB3000000

/* Interrupt registers offsets and masks */
	#define SPEAR320_INT_STS_MASK_REG		0x04
	#define SPEAR320_INT_CLR_MASK_REG		0x04
	#define SPEAR320_INT_ENB_MASK_REG		0x08
	#define SPEAR320_GPIO_IRQ_MASK			(1 << 0)

	#define SPEAR320S_I2S_IRQ_REC_DA_S		(1 << 2)
	#define SPEAR320S_I2S_IRQ_REC_OR_S		(1 << 3)
	#define SPEAR320S_I2S_IRQ_PLAY_EMP_M		(1 << 4)
	#define SPEAR320S_I2S_IRQ_PLAY_OR_M		(1 << 5)
	#define SPEAR320S_GPIO_IRQ_MASK			(1 << 6)

	#define SPEAR320_EMI_IRQ_MASK			(1 << 7)
	#define SPEAR320_CLCD_IRQ_MASK			(1 << 8)
	#define SPEAR320_SPP_IRQ_MASK			(1 << 9)
	#define SPEAR320_SDHCI_IRQ_MASK			(1 << 10)
	#define SPEAR320_CAN_U_IRQ_MASK			(1 << 11)
	#define SPEAR320_CAN_L_IRQ_MASK			(1 << 12)
	#define SPEAR320_UART1_IRQ_MASK			(1 << 13)
	#define SPEAR320_UART2_IRQ_MASK			(1 << 14)
	#define SPEAR320_SSP1_IRQ_MASK			(1 << 15)
	#define SPEAR320_SSP2_IRQ_MASK			(1 << 16)
	#define SPEAR320_SMII0_IRQ_MASK			(1 << 17)
	#define SPEAR320_MII1_SMII1_IRQ_MASK		(1 << 18)
	#define SPEAR320_WAKEUP_SMII0_IRQ_MASK		(1 << 19)
	#define SPEAR320_WAKEUP_MII1_SMII1_IRQ_MASK	(1 << 20)
	#define SPEAR320_I2C1_IRQ_MASK			(1 << 21)

	#define SPEAR320S_I2C2_IRQ_MASK			(1 << 22)
	#define SPEAR320S_UART3_IRQ_MASK		(1 << 23)
	#define SPEAR320S_UART4_IRQ_MASK		(1 << 24)
	#define SPEAR320S_UART5_IRQ_MASK		(1 << 25)
	#define SPEAR320S_UART6_IRQ_MASK		(1 << 26)
	#define SPEAR320S_RS485_IRQ_MASK		(1 << 27)

	#define SPEAR320_SHIRQ_RAS1_MASK		0x000380
	#define SPEAR320_SHIRQ_RAS3_MASK		0x000069
	#define SPEAR320_SHIRQ_INTRCOMM_RAS_MASK	0x3FF800

#define SPEAR320_CONTROL_REG		(SPEAR320_SOC_CONFIG_BASE + 0x10)
	#define SMII_PCLK_SHIFT				18
	#define SMII_PCLK_MASK				0x3
	#define SMII_PCLK_VAL_PAD			0x0
	#define SMII_PCLK_VAL_PLL2			0x1
	#define SMII_PCLK_VAL_SYNTH0			0x2
	#define SDHCI_PCLK_SHIFT			15
	#define SDHCI_PCLK_MASK				0x1
	#define SDHCI_PCLK_VAL_48M			0x0
	#define SDHCI_PCLK_VAL_SYNTH3			0x1
	#define I2S_REF_PCLK_SHIFT			8
	#define I2S_REF_PCLK_MASK			0x1
	#define I2S_REF_PCLK_SYNTH_VAL			0x1
	#define I2S_REF_PCLK_PLL2_VAL			0x0
	#define UART1_2_PCLK_SHIFT			6
	#define UART1_2_PCLK_MASK			0x1
	#define SPEAR320_UARTX_PCLK_VAL_SYNTH1		0x0
	#define SPEAR320_UARTX_PCLK_VAL_APB		0x1
	#define MII_ENB					(5)
	#define AUTO_NET_SMII_MODE_VAL			(0)
	#define AUTO_NET_MII_MODE_VAL			(1)
	#define AUTO_EXP_MODE_VAL			(2)
	#define SMALL_PRINTERS_MODE_VAL			(3)

	#define SPEAR320_PLGPIO_ENB_OFF		0x0024
	#define SPEAR320_PLGPIO_WDATA_OFF	0x0034
	#define SPEAR320_PLGPIO_DIR_OFF		0x0044
	#define SPEAR320_PLGPIO_IE_OFF		0x0064
	#define SPEAR320_PLGPIO_RDATA_OFF	0x0054
	#define SPEAR320_PLGPIO_MIS_OFF		0x0074

/* SPEAr320s specific registers */
#define SPEAR320S_EXT_CTRL_REG		(SPEAR320_SOC_CONFIG_BASE + 0x0018)
	#define SPEAR320S_LEGACY_MODE_VAL		0x0
	#define SPEAR320S_EXTENDED_MODE_VAL		0x1
	#define SPEAR320S_MII_MDIO_MASK			(1 << 4)
	#define SPEAR320S_MII_MDIO_10_11_VAL		0
	#define SPEAR320S_MII_MDIO_81_VAL		(1 << 4)
	#define SPEAR320S_EMI_FSMC_DYNAMIC_MUX_MASK	(1 << 5)
	#define SPEAR320S_UARTX_PCLK_MASK		0x1
	#define SPEAR320S_UART2_PCLK_SHIFT		8
	#define SPEAR320S_UART3_PCLK_SHIFT		9
	#define SPEAR320S_UART4_PCLK_SHIFT		10
	#define SPEAR320S_UART5_PCLK_SHIFT		11
	#define SPEAR320S_UART6_PCLK_SHIFT		12
	#define SPEAR320S_RS485_PCLK_SHIFT		13

	#define SPEAR320S_RS485_OE			(1 << 14)
	#define SPEAR320S_MAC_MODE_MII			0
	#define SPEAR320S_MAC_MODE_RMII			1
	#define SPEAR320S_MAC_MODE_SMII			2
	#define SPEAR320S_MAC_MODE_SS_SMII		3
	#define SPEAR320S_MAC_MODE_MASK			0x3
	#define SPEAR320S_MAC1_MODE_SHIFT		16
	#define SPEAR320S_MAC2_MODE_SHIFT		18

	#define SPEAR320S_PLGPIO_MIS_OFF		0x0084
	#define SPEAR320S_PLGPIO_EI_OFF			0x0094

	extern void enable_uart0_pads(void);
	extern void enable_uart1_pads(void);
	extern void enable_fsmc_pads(void);
	extern void enable_sdhci_pads(void);
	extern void enable_mii_pads(void);
	extern void enable_smii_pads(void);

	#define GPIO_SELECT0_OFFSET 0x24
	#define GPIO_SELECT1_OFFSET 0x28
	#define GPIO_SELECT2_OFFSET 0x2C
	#define GPIO_SELECT3_OFFSET 0x30
	#define MODE_CONTROL_OFFSET 0x10
	#define RAS_SELECT_OFFSET   0x0C

#endif /* __MACH_SPEAR320_H */
