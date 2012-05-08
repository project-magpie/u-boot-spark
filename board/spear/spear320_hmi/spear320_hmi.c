/*
 * (C) Copyright 2009
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
#include <asm/arch//spr_generic.h>
#include <asm/arch/hardware.h>
#include <asm/arch/padmux.h>
#include <asm/arch/spr_defs.h>
#include <asm/arch/spr_misc.h>

/* padmux devices to enable */
static struct pmx_dev *pmx_devs[] = {
	/* spear3xx specific devices */
	&spear3xx_pmx_i2c,
	&spear3xx_pmx_ssp,
	&spear3xx_pmx_uart0,

	/* hmi specific devices */
	&spear320s_pmx_fsmc[0],
	&spear320_pmx_sdhci[1],
	&spear320_pmx_mii1_2[1],
};

int board_init(void)
{
	/* call spear320 machine init function */
	spear320_common_init(&spear320s_extended_mode, pmx_devs,
			ARRAY_SIZE(pmx_devs));

	return spear_board_init(MACH_TYPE_SPEAR320_HMI);
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
	struct misc_regs *const misc_regs_p =
	    (struct misc_regs *)CONFIG_SPEAR_MISCBASE;

	if (((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG30) ||
	    ((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG31)) {

		return fsmc_nand_init(nand);
	}
#endif
	return -1;
}

int board_eth_init(bd_t *bis)
{
	int ret = 0;
#if defined(CONFIG_MACB)
	struct misc_regs *const misc_regs_p =
		(struct misc_regs *)CONFIG_SPEAR_MISCBASE;

	/* Enable AMEM clock for memory port access */
	writel(readl(&misc_regs_p->amem_cfg_ctrl) | 0x1,
			&misc_regs_p->amem_cfg_ctrl);

	if (macb_eth_initialize(0, (void *)CONFIG_SYS_MACB0_BASE,
				CONFIG_MACB0_PHY) >= 0)
		ret++;

	if (macb_eth_initialize(1, (void *)CONFIG_SYS_MACB1_BASE,
				CONFIG_MACB1_PHY) >= 0)
		ret++;
#endif
	return ret;
}
