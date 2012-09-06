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

DECLARE_GLOBAL_DATA_PTR;

void lowlevel_init(void)
{
}

int checkboard(void)
{
#ifdef CONFIG_MACH_SPEAR1340EVB
	printf("BOARD: SPEAr1340-EVB\n");
#else
#error BOARD not supported
#endif
	return 0;
}

int dram_init(void)
{
	/* Store complete RAM size and return */
	gd->ram_size = get_ram_size(CONFIG_SYS_SDRAM_BASE, SDRAM_MAX_SIZE);

	return 0;
}
