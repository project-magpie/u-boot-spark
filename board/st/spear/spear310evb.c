/*
 * (C) Copyright 2009
 * Ryan Chen, ST Micoelectronics, ryan.chen@st.com.
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
#include <asm/io.h>
#include <linux/mtd/fsmc_nand.h>
#include <asm/arch/hardware.h>
#include <asm/arch/generic.h>
#include <asm/arch/misc.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/spl_pnor.h>

#if defined(CONFIG_CMD_NAND)
static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];
#endif

#if defined(CONFIG_BOARD_EARLY_INIT_F)
int board_early_init_f(void)
{
	spear310_pins_default();
	spear3xx_enable_pins(PMX_I2C0, 0);
	spear3xx_enable_pins(PMX_SSP0, 0);
	spear3xx_enable_pins(PMX_ETH0, 0);
	spear3xx_enable_pins(PMX_UART0, PMX_UART_SIMPLE);

	spear310_enable_pins(PMX_ETH1, 0);

	return 0;
}
#endif

#if defined(CONFIG_CMD_NAND)
/*
 * board_nand_init - Board specific NAND initialization
 * @nand:	mtd private chip structure
 *
 * Called by nand_init_chip to initialize the board specific functions
 */
void board_nand_init()
{
	struct misc_regs *const misc_regs_p =
	    (struct misc_regs *)CONFIG_SPEAR_MISCBASE;
	struct nand_chip *nand = &nand_chip[0];

#if defined(CONFIG_NAND_FSMC)
	if (((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG30) ||
	    ((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG31)) {

		fsmc_nand_init(nand);
	}
#endif
	return;
}
#endif

#if defined(CONFIG_CMD_NET)
int board_eth_init(bd_t *bis)
{
	int ret = 0;

#if defined(CONFIG_DESIGNWARE_ETH)
	u32 interface = PHY_INTERFACE_MODE_MII;
	if (designware_initialize(0, CONFIG_SPEAR_ETHBASE, CONFIG_DW0_PHY,
				interface) >= 0)
		ret++;
#endif
#if defined(CONFIG_MACB)
	if (macb_eth_initialize(0, (void *)CONFIG_SYS_MACB0_BASE,
				CONFIG_MACB0_PHY) >= 0)
		ret++;

	if (macb_eth_initialize(1, (void *)CONFIG_SYS_MACB1_BASE,
				CONFIG_MACB1_PHY) >= 0)
		ret++;

	if (macb_eth_initialize(2, (void *)CONFIG_SYS_MACB2_BASE,
				CONFIG_MACB2_PHY) >= 0)
		ret++;

	if (macb_eth_initialize(3, (void *)CONFIG_SYS_MACB3_BASE,
				CONFIG_MACB3_PHY) >= 0)
		ret++;
#endif
	return ret;
}
#endif

#if defined(CONFIG_SPL_BUILD)
void board_ddr_init(void)
{
	spear3xx_ddr_comp_init();
}

/*
void soc_init(void)
{
	writel(0x2f7bc210, &misc_p->plgpio3_pad_prg);
	writel(0x017bdef6, &misc_p->plgpio4_pad_prg);
}
*/

pnor_width_t get_pnor_width(void)
{
	u32 emi_buswidth = (readl(SPEAR310_BOOT_REG) & SPEAR310_EMIBW_MASK) >>
		SPEAR310_EMIBW_SHFT;

	if (SPEAR310_EMIBW_8 == emi_buswidth)
		return PNOR_WIDTH_8;
	else if (SPEAR310_EMIBW_16 == emi_buswidth)
		return PNOR_WIDTH_16;
	else if (SPEAR310_EMIBW_32 == emi_buswidth)
		return PNOR_WIDTH_32;
	else
		return PNOR_WIDTH_SEARCH;
}
#endif
