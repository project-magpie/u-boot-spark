/*
 * (C) Copyright 2010
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
#include <miiphy.h>
#include <netdev.h>
#include <nand.h>
#include <spi.h>
#include <linux/mtd/st_smi.h>
#include <linux/mtd/fsmc_nand.h>
#include <asm/arch/hardware.h>
#include <asm/arch/spr_misc.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	gd->bd->bi_arch_number = MACH_TYPE_SPEAR1340;
	gd->bd->bi_boot_params = CONFIG_BOOT_PARAMS_ADDR;

	enable_pad_mux();
#if defined(CONFIG_ST_SMI)
	smi_init();
#endif
	return 0;
}

/*
 * board_nand_init - Board specific NAND initialization
 * @nand:	mtd private chip structure
 *
 * Called by nand_init_chip to initialize the board specific functions
 */

int board_nand_init(struct nand_chip *nand)
{
#if defined(CONFIG_NAND_FSMC)
	return fsmc_nand_init(nand);
#else
	return -1;
#endif
}

#if defined(CONFIG_CMD_NET)
int board_eth_init(bd_t *bis)
{
	int ret = 0;
	u32 interface = PHY_INTERFACE_MODE_RMII;
#if defined(CONFIG_DESIGNWARE_ETH)
#if defined(CONFIG_DW_AUTONEG)
	interface = PHY_INTERFACE_MODE_RGMII;
#endif
	if (designware_initialize(0, CONFIG_SPEAR_ETHBASE, CONFIG_DW0_PHY,
				interface) >= 0)
		ret++;
#endif
	return ret;
}
#endif

static struct misc_regs *const misc_regs_p =
	(struct misc_regs *)CONFIG_SPEAR_MISCBASE;

#ifdef CONFIG_DW_OTG
void phy_init(void)
{
	u32 temp;

	/* phy por assert */
	temp = readl(&misc_regs_p->usbphy_gen_cfg);
	temp |= USBPHY_POR;
	writel(temp, &misc_regs_p->usbphy_gen_cfg);
	udelay(1);

	/* phy clock disable */
	temp = readl(&misc_regs_p->usbphy_gen_cfg);
	temp &= ~USBPHY_RST;
	writel(temp, &misc_regs_p->usbphy_gen_cfg);

	udelay(150);

	/* phy por deassert */
	temp = readl(&misc_regs_p->usbphy_gen_cfg);
	temp &= ~USBPHY_POR;
	writel(temp, &misc_regs_p->usbphy_gen_cfg);
	udelay(1);

	/* phy clock enable */
	temp = readl(&misc_regs_p->usbphy_gen_cfg);
	temp |= USBPHY_RST;
	writel(temp, &misc_regs_p->usbphy_gen_cfg);

	/* wait for pll lock */
	while (!(readl(&misc_regs_p->usbphy_gen_cfg) & USB_PLL_LOCK))
		;

	udelay(1);

	/* OTG HCLK Disable */
	temp = readl(&misc_regs_p->perip1_clk_enb);
	temp &= ~UDC_UPD_CLKEN;
	writel(temp, &misc_regs_p->perip1_clk_enb);

	/* OTG HRESET deassert */
	temp = readl(&misc_regs_p->perip1_sw_rst);
	temp &= ~UDC_UPD_SWRST;
	writel(temp, &misc_regs_p->perip1_sw_rst);

	/* OTG HCLK Enable */
	temp = readl(&misc_regs_p->perip1_clk_enb);
	temp |= UDC_UPD_CLKEN;
	writel(temp, &misc_regs_p->perip1_clk_enb);
}
#endif

#if defined(CONFIG_PL022_SPI)
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	u32 perip_cfg;

	perip_cfg = readl(&misc_regs_p->perip_cfg);
	perip_cfg &= ~SSP_CS_EN_MSK;
	perip_cfg |= cs << SSP_CS_EN_SHFT;

	perip_cfg |= HS_SSP_SW_CS;
	perip_cfg |= HS_SSP_EN;
	writel(perip_cfg, &misc_regs_p->perip_cfg);

	return 1;
}

void spi_cs_activate(struct spi_slave *slave)
{
	u32 perip_cfg;

	perip_cfg = readl(&misc_regs_p->perip_cfg);
	perip_cfg &= ~HS_SSP_SW_CS;
	writel(perip_cfg, &misc_regs_p->perip_cfg);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	u32 perip_cfg;

	perip_cfg = readl(&misc_regs_p->perip_cfg);
	perip_cfg |= HS_SSP_SW_CS;
	writel(perip_cfg, &misc_regs_p->perip_cfg);
}
#endif
