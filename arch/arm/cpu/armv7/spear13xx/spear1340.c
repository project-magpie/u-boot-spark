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
#include <spi.h>
#include <asm/io.h>
#include <linux/mtd/st_smi.h>
#include <asm/arch/hardware.h>
#include <asm/arch/misc.h>

int arch_cpu_init(void)
{
	struct spear1340_misc_regs *const misc_p =
		(struct spear1340_misc_regs *)CONFIG_SYS_MISC_BASE;
	u32 perip1_clk_enb, perip2_clk_enb;
	u32 perip_clk_cfg;
#if defined(CONFIG_SDHCI)
	u32 perip_cfg;
#endif
#if defined(CONFIG_NAND_FSMC)
	u32 fsmc_cfg;
#endif
#if defined(CONFIG_DESIGNWARE_ETH)
	u32 gmac_clk_cfg;
#endif

	perip1_clk_enb = readl(&misc_p->perip1_clk_enb);
	perip2_clk_enb = readl(&misc_p->perip2_clk_enb);

#if defined(CONFIG_PL011_SERIAL)
	/* select USB PLL 48 MHz as the src clock */
	perip_clk_cfg = readl(&misc_p->perip_clk_cfg);
	perip_clk_cfg &= ~SPEAR1340_UARTCLKMSK;
	perip_clk_cfg |= SPEAR1340_UART48M;
	writel(perip_clk_cfg, &misc_p->perip_clk_cfg);

	perip1_clk_enb |= SPEAR1340_UART_CLKEN;
#endif

#if defined(CONFIG_DESIGNWARE_ETH)
	gmac_clk_cfg = readl(&misc_p->gmac_clk_cfg);
	gmac_clk_cfg &= ~SPEAR1340_CLKSEL_MSK;
	gmac_clk_cfg |= SPEAR1340_CLKSEL_PLL2;
	writel(gmac_clk_cfg, &misc_p->gmac_clk_cfg);

	perip1_clk_enb |= SPEAR1340_GETH_CLKEN;
#endif

#if defined(CONFIG_DW_UDC)
	perip1_clk_enb |= SPEAR1340_UDC_UPD_CLKEN;
#endif

#if defined(CONFIG_USB_EHCI_SPEAR)
	perip1_clk_enb |= SPEAR1340_UHC1_CLKEN;
#endif

#if defined(CONFIG_DW_I2C)
	perip1_clk_enb |= SPEAR1340_I2C_CLKEN;
#endif

#if defined(CONFIG_ST_SMI)
	perip1_clk_enb |= SPEAR1340_SMI_CLKEN;
#endif

#if defined(CONFIG_PL022_SPI)
	perip1_clk_enb |= SPEAR1340_SSP_CLKEN;
#endif

#if defined(CONFIG_SDHCI)
	perip_cfg = readl(&misc_p->perip_cfg);
	perip_cfg &= ~SPEAR1340_MCIF_MSK;
	perip_cfg |= SPEAR1340_MCIF_SD;
	writel(perip_cfg, &misc_p->perip_cfg);

	writel(SPEAR1340_SYNT_X_2 | SPEAR1340_SYNT_Y_21 | SPEAR1340_SYNT_CLKENB,
			&misc_p->mcif_sd_clk_synt);

	perip1_clk_enb |= SPEAR1340_SD_CLKEN;
#endif

#if defined(CONFIG_NAND_FSMC)
	fsmc_cfg = readl(&misc_p->fsmc_cfg);
	fsmc_cfg &= ~SPEAR1340_DEV_SEL_MSK;
	fsmc_cfg |= SPEAR1340_DEV_SEL_NAND;
#if defined(CONFIG_SYS_FSMC_NAND_16BIT)
	fsmc_cfg |= SPEAR1340_DEV_WIDTH_16;
#elif defined(CONFIG_SYS_FSMC_NAND_8BIT)
	fsmc_cfg |= SPEAR1340_DEV_WIDTH_8;
#endif
	writel(fsmc_cfg, &misc_p->fsmc_cfg);

	perip1_clk_enb |= SPEAR1340_FSMC_CLKEN;
#endif

	writel(perip1_clk_enb, &misc_p->perip1_clk_enb);
	writel(perip2_clk_enb, &misc_p->perip2_clk_enb);

#if defined(CONFIG_ST_SMI)
	smi_init();
#endif
	return 0;
}

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	printf("CPU:   SPEAr1340\n");
	return 0;
}
#endif

#if defined(CONFIG_PL022_SPI)
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	struct spear1340_misc_regs *misc_regs_p =
		(struct spear1340_misc_regs *)CONFIG_SYS_MISC_BASE;
	u32 perip_cfg;

	perip_cfg = readl(&misc_regs_p->perip_cfg);
	perip_cfg &= ~SPEAR1340_SSP_CS_EN_MSK;
	perip_cfg |= cs << SPEAR1340_SSP_CS_EN_SHFT;

	perip_cfg |= SPEAR1340_HS_SSP_SW_CS;
	perip_cfg |= SPEAR1340_HS_SSP_EN;
	writel(perip_cfg, &misc_regs_p->perip_cfg);

	return 1;
}

void spi_cs_activate(struct spi_slave *slave)
{
	struct spear1340_misc_regs *misc_regs_p =
		(struct spear1340_misc_regs *)CONFIG_SYS_MISC_BASE;
	u32 perip_cfg;

	perip_cfg = readl(&misc_regs_p->perip_cfg);
	perip_cfg &= ~SPEAR1340_HS_SSP_SW_CS;
	writel(perip_cfg, &misc_regs_p->perip_cfg);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	struct spear1340_misc_regs *misc_regs_p =
		(struct spear1340_misc_regs *)CONFIG_SYS_MISC_BASE;
	u32 perip_cfg;

	perip_cfg = readl(&misc_regs_p->perip_cfg);
	perip_cfg |= SPEAR1340_HS_SSP_SW_CS;
	writel(perip_cfg, &misc_regs_p->perip_cfg);
}
#endif

#if defined(CONFIG_USB_EHCI_SPEAR)
void spear1340_usbh_stop(void)
{
	struct spear1340_misc_regs *const misc_regs_p =
		(struct spear1340_misc_regs *)CONFIG_SYS_MISC_BASE;
	u32 perip1_sw_rst = readl(&misc_regs_p->perip1_sw_rst);

	perip1_sw_rst |= SPEAR1340_UHC1_SWRST;
	writel(perip1_sw_rst, &misc_regs_p->perip1_sw_rst);

	udelay(1000);
	perip1_sw_rst &= ~SPEAR1340_UHC1_SWRST;
	writel(perip1_sw_rst, &misc_regs_p->perip1_sw_rst);
}
#endif

#ifdef CONFIG_DW_OTG_PHYINIT
void udc_phy_init(void)
{
	struct spear1340_misc_regs *const misc_regs_p =
		(struct spear1340_misc_regs *)CONFIG_SYS_MISC_BASE;
	u32 temp;
	ulong start;

	/* phy por assert */
	temp = readl(&misc_regs_p->usbphy_gen_cfg);
	temp |= SPEAR1340_USBPHY_POR;
	writel(temp, &misc_regs_p->usbphy_gen_cfg);
	udelay(1);

	/* phy clock disable */
	temp = readl(&misc_regs_p->usbphy_gen_cfg);
	temp &= ~SPEAR1340_USBPHY_RST;
	writel(temp, &misc_regs_p->usbphy_gen_cfg);

	udelay(150);

	/* phy por deassert */
	temp = readl(&misc_regs_p->usbphy_gen_cfg);
	temp &= ~SPEAR1340_USBPHY_POR;
	writel(temp, &misc_regs_p->usbphy_gen_cfg);
	udelay(1);

	/* phy clock enable */
	temp = readl(&misc_regs_p->usbphy_gen_cfg);
	temp |= SPEAR1340_USBPHY_RST;
	writel(temp, &misc_regs_p->usbphy_gen_cfg);

	start = get_timer(0);

	while (get_timer(start) < CONFIG_SYS_HZ) {
		if (readl(&misc_regs_p->usbphy_gen_cfg) &
				SPEAR1340_USB_PLL_LOCK)
			break;

		/* Try after 10 ms */
		udelay(10);
	};

	/* wait for pll to stabilize */
	udelay(1);

	/* OTG HCLK Disable */
	temp = readl(&misc_regs_p->perip1_clk_enb);
	temp &= ~SPEAR1340_UDC_UPD_CLKEN;
	writel(temp, &misc_regs_p->perip1_clk_enb);

	/* OTG HRESET deassert */
	temp = readl(&misc_regs_p->perip1_sw_rst);
	temp &= ~SPEAR1340_UDC_UPD_SWRST;
	writel(temp, &misc_regs_p->perip1_sw_rst);

	/* OTG HCLK Enable */
	temp = readl(&misc_regs_p->perip1_clk_enb);
	temp |= SPEAR1340_UDC_UPD_CLKEN;
	writel(temp, &misc_regs_p->perip1_clk_enb);
}
#endif

void reset_cpu(ulong ignored)
{
	struct spear1340_misc_regs *misc_regs_p =
		(struct spear1340_misc_regs *)CONFIG_SYS_MISC_BASE;

	printf("System is going to reboot ...\n");

	/*
	 * This 1 second delay will allow the above message
	 * to be printed before reset
	 */
	udelay((1000 * 1000));

	writel(0x01, &misc_regs_p->sys_sw_res);

	/* system will restart */
	while (1)
		;
}
