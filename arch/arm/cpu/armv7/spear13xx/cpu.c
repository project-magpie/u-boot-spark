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
#include <asm/arch/misc.h>

int arch_cpu_init(void)
{
	struct misc_regs *const misc_p =
	    (struct misc_regs *)CONFIG_SPEAR_MISCBASE;
	u32 perip1_clk_enb, perip2_clk_enb;
#ifdef CONFIG_SOC_SPEAR1340
	u32 perip3_clk_enb;
#endif
	u32 perip_clk_cfg;
#if defined(CONFIG_SDHCI)
	u32 perip_cfg;
#endif
#if defined(CONFIG_NAND_FSMC)
	u32 fsmc_cfg;
#endif
	perip1_clk_enb = readl(&misc_p->perip1_clk_enb);
	perip2_clk_enb = readl(&misc_p->perip2_clk_enb);

	perip1_clk_enb |= GPT1_CLKEN;

#if defined(CONFIG_PL011_SERIAL)
	/* select USB PLL 48 MHz as the src clock */
	perip_clk_cfg = readl(&misc_p->perip_clk_cfg);
	perip_clk_cfg &= ~CONFIG_SPEAR_UARTCLKMSK;
	perip_clk_cfg |= CONFIG_SPEAR_UART48M;
	writel(perip_clk_cfg, &misc_p->perip_clk_cfg);

	perip1_clk_enb |= UART_CLKEN;
#endif

#if defined(CONFIG_DESIGNWARE_ETH)
#if defined(CONFIG_SPEAR_RGMII)
	writel(PHY_IF_RGMII | CLK_SEL_PLL2, &misc_p->gmac_clk_cfg);
#else
	writel(PHY_IF_GMII | CLK_SEL_PLL2, &misc_p->gmac_clk_cfg);
#endif

	perip1_clk_enb |= GETH_CLKEN;
#endif

#if defined(CONFIG_DW_UDC)
	perip1_clk_enb |= UDC_UPD_CLKEN;
#endif

#if defined(CONFIG_USB_EHCI_SPEAR)
	perip1_clk_enb |= UHC1_CLKEN;
#endif

#if defined(CONFIG_DW_I2C)
	perip1_clk_enb |= I2C_CLKEN;
#endif

#if defined(CONFIG_ST_SMI)
	perip1_clk_enb |= SMI_CLKEN;
#endif

#if defined(CONFIG_PL022_SPI)
	perip1_clk_enb |= SSP_CLKEN;
#endif

#if defined(CONFIG_SDHCI)
	perip_cfg = readl(&misc_p->perip_cfg);
	perip_cfg &= ~MCIF_MSK;
	perip_cfg |= MCIF_SD;
	writel(perip_cfg, &misc_p->perip_cfg);

	writel(SYNT_X_2 | SYNT_Y_21 | SYNT_CLK_ENB,
			&misc_p->mcif_sd_clk_synt);

	perip1_clk_enb |= SD_CLKEN;
#endif

#if defined(CONFIG_NAND_FSMC)
	fsmc_cfg = readl(&misc_p->fsmc_cfg);
#if defined(CONFIG_SPEAR1310)
	fsmc_cfg &= ~DEV_SEL_NANDCS0_NORCS4;
#else
	fsmc_cfg &= ~DEV_SEL_MSK;
	fsmc_cfg |= DEV_SEL_NAND;
#if defined(CONFIG_SYS_FSMC_NAND_16BIT)
	fsmc_cfg |= DEV_WIDTH_16;
#elif defined(CONFIG_SYS_FSMC_NAND_8BIT)
	fsmc_cfg |= DEV_WIDTH_8;
#endif
#endif
	writel(fsmc_cfg, &misc_p->fsmc_cfg);

	perip1_clk_enb |= FSMC_CLKEN;
#endif

	writel(perip1_clk_enb, &misc_p->perip1_clk_enb);
	writel(perip2_clk_enb, &misc_p->perip2_clk_enb);
#ifdef CONFIG_SPEAR1340
	perip3_clk_enb = readl(&misc_p->perip3_clk_enb);
	writel(perip3_clk_enb, &misc_p->perip3_clk_enb);
#endif

	/* Early driver initializations */
#if defined(CONFIG_ST_SMI)
	smi_init();
#endif
	return 0;
}

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
#if defined(CONFIG_SPEAR1340)
	printf("CPU:   SPEAr1340\n");
#endif
	return 0;
}
#endif
