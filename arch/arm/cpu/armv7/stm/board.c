/*
 * Copyright (C) 2013 STMicroelectronics
 *	Sean McGoogan <Sean.McGoogan@st.com>
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


DECLARE_GLOBAL_DATA_PTR;


extern int dram_init(void)
{
	/*
	 * Note: this function should only called when gd->bd == NULL
	 */
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;		/* size of DRAM memory in bytes */

	return 0;
}


extern void dram_init_banksize(void)
{
	/*
	 * Note: this function should only called when gd->bd != NULL
	 */
	bd_t * const bd = gd->bd;

	bd->bi_memstart = CONFIG_SYS_SDRAM_BASE;	/* start of DRAM memory */
	bd->bi_dram[0].start = bd->bi_memstart;

	bd->bi_memsize  = CONFIG_SYS_SDRAM_SIZE;	/* size  of DRAM memory in bytes */
	bd->bi_dram[0].size  = bd->bi_memsize;
}


#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
extern int misc_init_r(void)
{
	bd_t * const bd = gd->bd;

#if defined(CONFIG_MACH_TYPE)
	bd->bi_arch_number = CONFIG_MACH_TYPE;		/* unique Machine ID for this board */
#endif	/* CONFIG_MACH_TYPE */

	return 0;
}
#endif	/* CONFIG_MISC_INIT_R */
