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
#include <asm/arch/hardware.h>
#include <asm/arch/spr_generic.h>
#include <asm/arch/sys_proto.h>

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = get_ram_size(PHYS_SDRAM_1,
					       PHYS_SDRAM_1_MAXSIZE);

	return 0;
}

int misc_init_r(void)
{
	setenv("verify", "n");

#if defined(CONFIG_SPEAR_USBTTY)
	setenv("stdin", "usbtty");
	setenv("stdout", "usbtty");
	setenv("stderr", "usbtty");

#ifndef CONFIG_SYS_NO_DCACHE
	invalidate_dcache(get_device_type());
	dcache_enable();
#endif
#endif
	return 0;
}

#if defined(CONFIG_CMD_MMC)
int board_mmc_init(bd_t *bis)
{
	int ret = 0;
#if defined(CONFIG_SPEAR_MMC)
	if (sdhci_initialize(0, CONFIG_SYS_MMC_BASE) >= 0)
		ret++;
#endif
	return ret;
}
#endif
