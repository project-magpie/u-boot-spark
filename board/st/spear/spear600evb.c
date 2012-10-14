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

#if defined(CONFIG_CMD_NAND)
static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];
#endif

#if defined(CONFIG_BOARD_EARLY_INIT_F)
int board_early_init_f(void)
{
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
	if (!(readl(&misc_regs_p->auto_cfg_reg) & MISC_NANDDIS))
		fsmc_nand_init(nand);
#endif
	return;
}
#endif

#if defined(CONFIG_CMD_NET)
int board_eth_init(bd_t *bis)
{
	int ret = 0;
#if defined(CONFIG_SPEAR_MACID_IN_I2CMEM)
	uchar mac_id[6];
#endif
#if defined(CONFIG_DESIGNWARE_ETH)
	u32 interface = PHY_INTERFACE_MODE_MII;
#if defined(CONFIG_DW_AUTONEG)
	interface = PHY_INTERFACE_MODE_GMII;
#endif
#if defined(CONFIG_SPEAR_MACID_IN_I2CMEM)
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

#if defined(CONFIG_SPL_BUILD)
static void sel_1v8(void)
{
	struct misc_regs *misc_p = (struct misc_regs *)CONFIG_SPEAR_MISCBASE;
	u32 ddr1v8, ddr2v5;

	ddr2v5 = readl(&misc_p->ddr_2v5_compensation);
	ddr2v5 &= 0x8080ffc0;
	ddr2v5 |= 0x78000003;
	writel(ddr2v5, &misc_p->ddr_2v5_compensation);

	ddr1v8 = readl(&misc_p->ddr_1v8_compensation);
	ddr1v8 &= 0x8080ffc0;
	ddr1v8 |= 0x78000010;
	writel(ddr1v8, &misc_p->ddr_1v8_compensation);

	while (!(readl(&misc_p->ddr_1v8_compensation) & MISC_DDR_COMP_ACCURATE))
		;
}

static void sel_2v5(void)
{
	struct misc_regs *misc_p = (struct misc_regs *)CONFIG_SPEAR_MISCBASE;
	u32 ddr1v8, ddr2v5;

	ddr1v8 = readl(&misc_p->ddr_1v8_compensation);
	ddr1v8 &= 0x8080ffc0;
	ddr1v8 |= 0x78000003;
	writel(ddr1v8, &misc_p->ddr_1v8_compensation);

	ddr2v5 = readl(&misc_p->ddr_2v5_compensation);
	ddr2v5 &= 0x8080ffc0;
	ddr2v5 |= 0x78000010;
	writel(ddr2v5, &misc_p->ddr_2v5_compensation);

	while (!(readl(&misc_p->ddr_2v5_compensation) & MISC_DDR_COMP_ACCURATE))
		;
}

void board_ddr_init(void)
{
	struct misc_regs *misc_p = (struct misc_regs *)CONFIG_SPEAR_MISCBASE;
	u32 ddrpad;
	u32 core3v3, ddr1v8, ddr2v5;

	/* DDR pad register configurations */
	ddrpad = readl(&misc_p->ddr_pad);
	ddrpad &= ~MISC_DDR_PAD_CNF_MSK;

#if defined(CONFIG_SPEAR_DDR_HCLK)
	ddrpad |= 0xEAAB;
#elif defined(CONFIG_SPEAR_DDR_2HCLK)
	ddrpad |= 0xEAAD;
#elif defined(CONFIG_SPEAR_DDR_PLL2)
	ddrpad |= 0xEAAD;
#endif
	writel(ddrpad, &misc_p->ddr_pad);

	/* Compensation register configurations */
	core3v3 = readl(&misc_p->core_3v3_compensation);
	core3v3 &= 0x8080ffe0;
	core3v3 |= 0x78000002;
	writel(core3v3, &misc_p->core_3v3_compensation);

	ddr1v8 = readl(&misc_p->ddr_1v8_compensation);
	ddr1v8 &= 0x8080ffc0;
	ddr1v8 |= 0x78000004;
	writel(ddr1v8, &misc_p->ddr_1v8_compensation);

	ddr2v5 = readl(&misc_p->ddr_2v5_compensation);
	ddr2v5 &= 0x8080ffc0;
	ddr2v5 |= 0x78000004;
	writel(ddr2v5, &misc_p->ddr_2v5_compensation);

	if ((readl(&misc_p->ddr_pad) & MISC_DDR_PAD_SW_CONF) ==
			MISC_DDR_PAD_SW_CONF) {
		/* Software memory configuration */
		if (readl(&misc_p->ddr_pad) & MISC_DDR_PAD_SSTL_SEL)
			sel_1v8();
		else
			sel_2v5();
	} else {
		/* Hardware memory configuration */
		if (readl(&misc_p->ddr_pad) & MISC_DDR_PAD_DRAM_TYPE)
			sel_1v8();
		else
			sel_2v5();
	}
}
#endif
