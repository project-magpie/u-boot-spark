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

#define PLGPIO_SEL_36	0xb3000028
#define PLGPIO_IO_36	0xb3000038
#define PLGPIO_SEL_76	0xb300002C
#define PLGPIO_IO_76	0xb300003C
#define PLGPIO_36	(0x1 << 4)
#define PLGPIO_76	(0x1 << 12)

#if defined(CONFIG_CMD_NAND)
static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];
#endif

static void spear_phy_reset(void)
{
	/* PLGPIO36 is used to enable oscillator */
	writel(readl(PLGPIO_IO_36) | PLGPIO_36, PLGPIO_IO_36);
	writel(readl(PLGPIO_SEL_36) | PLGPIO_36, PLGPIO_SEL_36);

	/* PLGPIO76 is used to reset phy */
	writel(readl(PLGPIO_IO_76) & ~PLGPIO_76, PLGPIO_IO_76);
	writel(readl(PLGPIO_SEL_76) | PLGPIO_76, PLGPIO_SEL_76);
	writel(readl(PLGPIO_IO_76) | PLGPIO_76, PLGPIO_IO_76);
}

int board_init(void)
{
	spear_phy_reset();
	return 0;
}

#if defined(CONFIG_BOARD_EARLY_INIT_F)
int board_early_init_f(void)
{
	spear320_select_mode(SPEAR320_EXTENDED_MODE);

	spear320_pins_default();

	spear320_enable_pins(PMX_I2C0, 0);
	spear320_enable_pins(PMX_ETH0, 0);
	spear320_enable_pins(PMX_SSP0, 0);
	spear320_enable_pins(PMX_UART0, PMX_UART_SIMPLE);
	spear320_enable_pins(PMX_ETH2, PMX_ETH_MII);
	spear320_enable_pins(PMX_SDMMC, PMX_SDMMC_CD51);

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

	if (((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG30) ||
	    ((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG31)) {

		fsmc_nand_init(nand);
	}

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
	if (macb_eth_initialize(1, (void *)CONFIG_SYS_MACB1_BASE,
				CONFIG_MACB1_PHY) >= 0)
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
#endif
