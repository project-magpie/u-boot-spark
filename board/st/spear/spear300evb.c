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
#include <asm/arch/hardware.h>
#include <asm/arch/generic.h>
#include <asm/arch/misc.h>
#include <asm/arch/pinmux.h>

#if defined(CONFIG_CMD_NAND)
static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];
#endif

#if defined(CONFIG_BOARD_EARLY_INIT_F)
int board_early_init_f(void)
{
	spear300_select_mode(SPEAR300_MODE_NAND);

	spear300_pins_default();
	spear3xx_enable_pins(PMX_I2C0, 0);
	spear3xx_enable_pins(PMX_SSP0, 0);
	spear3xx_enable_pins(PMX_ETH0, 0);
	spear3xx_enable_pins(PMX_UART0, PMX_UART_SIMPLE);

	spear300_enable_pins(PMX_SDMMC, PMX_SDMMC_4BIT);
	spear300_enable_pins(PMX_FSMCNAND, PMX_NAND_2CHIP);

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
#if defined(CONFIG_SPEAR_MACID_IN_I2CMEM)
	uchar mac_id[6];
	if (!eth_getenv_enetaddr("ethaddr", mac_id) && !i2c_read_mac(mac_id))
		eth_setenv_enetaddr("ethaddr", mac_id);
#endif
	if (designware_initialize(0, CONFIG_SPEAR_ETHBASE, CONFIG_DW0_PHY,
				interface) >= 0)
		ret++;
#endif
	return ret;
}
#endif
