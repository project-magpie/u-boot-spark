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
#include <netdev.h>
#include <nand.h>
#include <phy.h>
#include <linux/mtd/fsmc_nand.h>
#include <asm/arch/hardware.h>
#include <asm/arch/mmc.h>
#include <asm/arch/pinmux.h>

#if defined(CONFIG_CMD_NAND)
static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];
#endif

#if defined(CONFIG_BOARD_EARLY_INIT_F)
int board_early_init_f(void)
{
	spear1310_pins_default();

	spear1310_enable_pins(PMX_FSMCNAND, PMX_NAND_8BIT);
	spear1310_enable_pins(PMX_UART0, PMX_UART_SIMPLE);
	spear1310_enable_pins(PMX_I2C0, 0);
	spear1310_enable_pins(PMX_SSP0, PMX_SSP_CS0);
	spear1310_enable_pins(PMX_SMI, PMX_SMI_2CHIPS);
	spear1310_enable_pins(PMX_SDMMC, 0);
	spear1310_enable_pins(PMX_ETH0, PMX_ETH_GMII);

	return 0;
}
#endif

#if defined(CONFIG_CMD_NAND)
/*
 * board_nand_init - Board specific NAND initialization
 */
void board_nand_init(void)
{
#if defined(CONFIG_NAND_FSMC)
	fsmc_nand_init(&nand_chip[0]);
#endif
}
#endif

#if defined(CONFIG_CMD_NET)
int board_eth_init(bd_t *bis)
{
	int ret = 0;
	u32 interface = PHY_INTERFACE_MODE_MII;
#if defined(CONFIG_DESIGNWARE_ETH)
#if defined(CONFIG_DW_AUTONEG)
	interface = PHY_INTERFACE_MODE_GMII;
#endif
	if (designware_initialize(0, CONFIG_SYS_ETH_BASE, CONFIG_DW0_PHY,
				interface) >= 0)
		ret++;
#endif
	return ret;
}
#endif

#if defined(CONFIG_CMD_MMC)
int board_mmc_init(bd_t *bis)
{
	int ret = 0;
#if defined(CONFIG_SPEAR_SDHCI)
	if (spear_sdhci_init(CONFIG_SYS_MMC_BASE, 24000000, 6000000, 0) >= 0)
		ret++;
#endif
	return ret;
}
#endif
