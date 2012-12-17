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

#ifndef _ASM_ARCH_SPEAR1310_PINMUX_H
#define _ASM_ARCH_SPEAR1310_PINMUX_H

#include <asm/arch/spear13xx.h>

#define SPEAR1310_PERIP_CFG			(CONFIG_SYS_MISC_BASE + 0x32C)
	#define MCIF_SEL_SHIFT				3
	#define MCIF_SEL_SD				(0x1 << MCIF_SEL_SHIFT)
	#define MCIF_SEL_CF				(0x2 << MCIF_SEL_SHIFT)
	#define MCIF_SEL_XD				(0x3 << MCIF_SEL_SHIFT)
	#define MCIF_SEL_MASK				(0x3 << MCIF_SEL_SHIFT)

#define SPEAR1310_PCIE_SATA_CFG			(CONFIG_SYS_MISC_BASE + 0x3A4)
	#define PCIE_SATA2_SEL_PCIE			(0 << 31)
	#define PCIE_SATA1_SEL_PCIE			(0 << 30)
	#define PCIE_SATA0_SEL_PCIE			(0 << 29)
	#define PCIE_SATA2_SEL_SATA			(1 << 31)
	#define PCIE_SATA1_SEL_SATA			(1 << 30)
	#define PCIE_SATA0_SEL_SATA			(1 << 29)
	#define SATA2_CFG_TX_CLK_EN			(1 << 27)
	#define SATA2_CFG_RX_CLK_EN			(1 << 26)
	#define SATA2_CFG_POWERUP_RESET			(1 << 25)
	#define SATA2_CFG_PM_CLK_EN			(1 << 24)
	#define SATA1_CFG_TX_CLK_EN			(1 << 23)
	#define SATA1_CFG_RX_CLK_EN			(1 << 22)
	#define SATA1_CFG_POWERUP_RESET			(1 << 21)
	#define SATA1_CFG_PM_CLK_EN			(1 << 20)
	#define SATA0_CFG_TX_CLK_EN			(1 << 19)
	#define SATA0_CFG_RX_CLK_EN			(1 << 18)
	#define SATA0_CFG_POWERUP_RESET			(1 << 17)
	#define SATA0_CFG_PM_CLK_EN			(1 << 16)
	#define PCIE2_CFG_DEVICE_PRESENT		(1 << 11)
	#define PCIE2_CFG_POWERUP_RESET			(1 << 10)
	#define PCIE2_CFG_CORE_CLK_EN			(1 << 9)
	#define PCIE2_CFG_AUX_CLK_EN			(1 << 8)
	#define PCIE1_CFG_DEVICE_PRESENT		(1 << 7)
	#define PCIE1_CFG_POWERUP_RESET			(1 << 6)
	#define PCIE1_CFG_CORE_CLK_EN			(1 << 5)
	#define PCIE1_CFG_AUX_CLK_EN			(1 << 4)
	#define PCIE0_CFG_DEVICE_PRESENT		(1 << 3)
	#define PCIE0_CFG_POWERUP_RESET			(1 << 2)
	#define PCIE0_CFG_CORE_CLK_EN			(1 << 1)
	#define PCIE0_CFG_AUX_CLK_EN			(1 << 0)

#define SPEAR1310_PAD_DIR_SEL_1			(CONFIG_SYS_MISC_BASE + 0x65C)
#define SPEAR1310_PAD_FUNCTION_EN_1		(CONFIG_SYS_MISC_BASE + 0x650)
	#define PMX_UART0_MASK				(1 << 1)
	#define PMX_I2C0_MASK				(1 << 2)
	#define PMX_I2S0_MASK				(1 << 3)
	#define PMX_SSP0_MASK				(1 << 4)
	#define PMX_CLCD1_MASK				(1 << 5)
	#define PMX_EGPIO00_MASK			(1 << 6)
	#define PMX_EGPIO01_MASK			(1 << 7)
	#define PMX_EGPIO02_MASK			(1 << 8)
	#define PMX_EGPIO03_MASK			(1 << 9)
	#define PMX_EGPIO04_MASK			(1 << 10)
	#define PMX_EGPIO05_MASK			(1 << 11)
	#define PMX_EGPIO06_MASK			(1 << 12)
	#define PMX_EGPIO07_MASK			(1 << 13)
	#define PMX_EGPIO08_MASK			(1 << 14)
	#define PMX_EGPIO09_MASK			(1 << 15)
	#define PMX_SMI_MASK				(1 << 16)
	#define PMX_NAND8_MASK				(1 << 17)
	#define PMX_GMIICLK_MASK			(1 << 18)
	#define PMX_GMIICOL_CRS_XFERER_MIITXCLK_MASK	(1 << 19)
	#define PMX_RXCLK_RDV_TXEN_D03_MASK		(1 << 20)
	#define PMX_GMIID47_MASK			(1 << 21)
	#define PMX_MDC_MDIO_MASK			(1 << 22)
	#define PMX_MCI_DATA8_15_MASK			(1 << 23)
	#define PMX_NFAD23_MASK				(1 << 24)
	#define PMX_NFAD24_MASK				(1 << 25)
	#define PMX_NFAD25_MASK				(1 << 26)
	#define PMX_NFCE3_MASK				(1 << 27)
	#define PMX_NFWPRT3_MASK			(1 << 28)
	#define PMX_NFRSTPWDWN0_MASK			(1 << 29)
	#define PMX_NFRSTPWDWN1_MASK			(1 << 30)
	#define PMX_NFRSTPWDWN2_MASK			(1 << 31)

#define SPEAR1310_PAD_DIR_SEL_2			(CONFIG_SYS_MISC_BASE + 0x660)
#define SPEAR1310_PAD_FUNCTION_EN_2		(CONFIG_SYS_MISC_BASE + 0x654)
	#define PMX_NFRSTPWDWN3_MASK			(1 << 0)
	#define PMX_SMINCS2_MASK			(1 << 1)
	#define PMX_SMINCS3_MASK			(1 << 2)
	#define PMX_CLCD2_MASK				(1 << 3)
	#define PMX_KBD_ROWCOL68_MASK			(1 << 4)
	#define PMX_EGPIO10_MASK			(1 << 5)
	#define PMX_EGPIO11_MASK			(1 << 6)
	#define PMX_EGPIO12_MASK			(1 << 7)
	#define PMX_EGPIO13_MASK			(1 << 8)
	#define PMX_EGPIO14_MASK			(1 << 9)
	#define PMX_EGPIO15_MASK			(1 << 10)
	#define PMX_UART0_MODEM_MASK			(1 << 11)
	#define PMX_GPT0_TMR0_MASK			(1 << 12)
	#define PMX_GPT0_TMR1_MASK			(1 << 13)
	#define PMX_GPT1_TMR0_MASK			(1 << 14)
	#define PMX_GPT1_TMR1_MASK			(1 << 15)
	#define PMX_I2S1_MASK				(1 << 16)
	#define PMX_KBD_ROWCOL25_MASK			(1 << 17)
	#define PMX_NFIO8_15_MASK			(1 << 18)
	#define PMX_KBD_COL1_MASK			(1 << 19)
	#define PMX_NFCE1_MASK				(1 << 20)
	#define PMX_KBD_COL0_MASK			(1 << 21)
	#define PMX_NFCE2_MASK				(1 << 22)
	#define PMX_KBD_ROW1_MASK			(1 << 23)
	#define PMX_NFWPRT1_MASK			(1 << 24)
	#define PMX_KBD_ROW0_MASK			(1 << 25)
	#define PMX_NFWPRT2_MASK			(1 << 26)
	#define PMX_MCIDATA0_MASK			(1 << 27)
	#define PMX_MCIDATA1_MASK			(1 << 28)
	#define PMX_MCIDATA2_MASK			(1 << 29)
	#define PMX_MCIDATA3_MASK			(1 << 30)
	#define PMX_MCIDATA4_MASK			(1 << 31)

#define SPEAR1310_PAD_DIR_SEL_3			(CONFIG_SYS_MISC_BASE + 0x664)
#define SPEAR1310_PAD_FUNCTION_EN_3		(CONFIG_SYS_MISC_BASE + 0x658)
	#define PMX_MCIDATA5_MASK			(1 << 0)
	#define PMX_MCIDATA6_MASK			(1 << 1)
	#define PMX_MCIDATA7_MASK			(1 << 2)
	#define PMX_MCIDATA1SD_MASK			(1 << 3)
	#define PMX_MCIDATA2SD_MASK			(1 << 4)
	#define PMX_MCIDATA3SD_MASK			(1 << 5)
	#define PMX_MCIADDR0ALE_MASK			(1 << 6)
	#define PMX_MCIADDR1CLECLK_MASK			(1 << 7)
	#define PMX_MCIADDR2_MASK			(1 << 8)
	#define PMX_MCICECF_MASK			(1 << 9)
	#define PMX_MCICEXD_MASK			(1 << 10)
	#define PMX_MCICESDMMC_MASK			(1 << 11)
	#define PMX_MCICDCF1_MASK			(1 << 12)
	#define PMX_MCICDCF2_MASK			(1 << 13)
	#define PMX_MCICDXD_MASK			(1 << 14)
	#define PMX_MCICDSDMMC_MASK			(1 << 15)
	#define PMX_MCIDATADIR_MASK			(1 << 16)
	#define PMX_MCIDMARQWP_MASK			(1 << 17)
	#define PMX_MCIIORDRE_MASK			(1 << 18)
	#define PMX_MCIIOWRWE_MASK			(1 << 19)
	#define PMX_MCIRESETCF_MASK			(1 << 20)
	#define PMX_MCICS0CE_MASK			(1 << 21)
	#define PMX_MCICFINTR_MASK			(1 << 22)
	#define PMX_MCIIORDY_MASK			(1 << 23)
	#define PMX_MCICS1_MASK				(1 << 24)
	#define PMX_MCIDMAACK_MASK			(1 << 25)
	#define PMX_MCISDCMD_MASK			(1 << 26)
	#define PMX_MCILEDS_MASK			(1 << 27)
	#define PMX_TOUCH_XY_MASK			(1 << 28)
	#define PMX_SSP0_CS0_MASK			(1 << 29)
	#define PMX_SSP0_CS1_2_MASK			(1 << 30)

/* combined macros */
#define PMX_GMII_MASK		(PMX_GMIICLK_MASK |			\
				PMX_GMIICOL_CRS_XFERER_MIITXCLK_MASK |	\
				PMX_RXCLK_RDV_TXEN_D03_MASK |		\
				PMX_GMIID47_MASK | PMX_MDC_MDIO_MASK)

#define PMX_EGPIO_0_GRP_MASK	(PMX_EGPIO00_MASK | PMX_EGPIO01_MASK |	\
				PMX_EGPIO02_MASK |			\
				PMX_EGPIO03_MASK | PMX_EGPIO04_MASK |	\
				PMX_EGPIO05_MASK | PMX_EGPIO06_MASK |	\
				PMX_EGPIO07_MASK | PMX_EGPIO08_MASK |	\
				PMX_EGPIO09_MASK)
#define PMX_EGPIO_1_GRP_MASK	(PMX_EGPIO10_MASK | PMX_EGPIO11_MASK |	\
				PMX_EGPIO12_MASK | PMX_EGPIO13_MASK |	\
				PMX_EGPIO14_MASK | PMX_EGPIO15_MASK)

#define PMX_KEYBOARD_6X6_MASK	(PMX_KBD_ROW0_MASK | PMX_KBD_ROW1_MASK | \
				PMX_KBD_ROWCOL25_MASK | PMX_KBD_COL0_MASK | \
				PMX_KBD_COL1_MASK)

#define PMX_NAND8BIT_0_MASK	(PMX_NAND8_MASK | PMX_NFAD23_MASK |	\
				PMX_NFAD24_MASK | PMX_NFAD25_MASK |	\
				PMX_NFWPRT3_MASK | PMX_NFRSTPWDWN0_MASK | \
				PMX_NFRSTPWDWN1_MASK | PMX_NFRSTPWDWN2_MASK | \
				PMX_NFCE3_MASK)
#define PMX_NAND8BIT_1_MASK	PMX_NFRSTPWDWN3_MASK

#define PMX_NAND16BIT_1_MASK	(PMX_KBD_ROWCOL25_MASK | PMX_NFIO8_15_MASK)
#define PMX_NAND_4CHIPS_MASK	(PMX_NFCE1_MASK | PMX_NFCE2_MASK |	\
				PMX_NFWPRT1_MASK | PMX_NFWPRT2_MASK |	\
				PMX_KBD_ROW0_MASK | PMX_KBD_ROW1_MASK |	\
				PMX_KBD_COL0_MASK | PMX_KBD_COL1_MASK)

#define PMX_MCIFALL_1_MASK	0xF8000000
#define PMX_MCIFALL_2_MASK	0x0FFFFFFF

#define PMX_PCI_REG2_MASK	(PMX_SMINCS2_MASK | PMX_SMINCS3_MASK |	\
				PMX_CLCD2_MASK | PMX_KBD_ROWCOL68_MASK | \
				PMX_EGPIO_1_GRP_MASK | PMX_GPT0_TMR0_MASK | \
				PMX_GPT0_TMR1_MASK | PMX_GPT1_TMR0_MASK | \
				PMX_GPT1_TMR1_MASK | PMX_I2S1_MASK |	\
				PMX_NFCE2_MASK)
#define PMX_PCI_REG3_MASK	(PMX_TOUCH_XY_MASK | PMX_SSP0_CS0_MASK | \
				PMX_SSP0_CS1_2_MASK)

#define PMX_SMII_0_1_2_MASK	(PMX_CLCD2_MASK | PMX_KBD_ROWCOL68_MASK)
#define PMX_RGMII_REG1_MASK	(PMX_MCI_DATA8_15_MASK |		\
				PMX_GMIICOL_CRS_XFERER_MIITXCLK_MASK |	\
				PMX_GMIID47_MASK)
#define PMX_RGMII_REG2_MASK	(PMX_KBD_ROWCOL68_MASK | PMX_EGPIO_1_GRP_MASK |\
				PMX_KBD_ROW1_MASK | PMX_NFWPRT1_MASK |	\
				PMX_KBD_ROW0_MASK | PMX_NFWPRT2_MASK)
#define PMX_RGMII_REG3_MASK	(PMX_TOUCH_XY_MASK | PMX_SSP0_CS0_MASK | \
				PMX_SSP0_CS1_2_MASK)

/* Pull DOWN and Pull UP */
#define SPEAR1310_PAD_PU_CFG_1		(CONFIG_SYS_MISC_BASE + 0x600)
#define SPEAR1310_PAD_PD_CFG_1		(CONFIG_SYS_MISC_BASE + 0x620)

/* Macro's to configure plgpios as Pull UP, Pull Down */
#define CONFIG_SYS_RASCFG_BASE		0xD8400000
	#define SPEAR1310_GPIO_IN0		(CONFIG_SYS_RASCFG_BASE + 0x70)
	#define SPEAR1310_GPIO_OUT0		(CONFIG_SYS_RASCFG_BASE + 0x90)
	#define SPEAR1310_GPIO_EN0		(CONFIG_SYS_RASCFG_BASE + 0xB0)
	#define SPEAR1310_GPIO_FUN_EN0		(CONFIG_SYS_RASCFG_BASE + 0xD0)
	#define SPEAR1310_MAX_PLGPIOS		200

extern void spear1310_pins_default(void);
extern void spear1310_enable_pins(u32 ip, u32 mode);
extern void spear1310_configure_pin(u32 plgpio, u32 mode);
extern void spear1310_plgpio_set(u32 plgpio, u32 val);
extern int  spear1310_plgpio_get(u32 plgpio);

#endif
