/*
 * Copyright (C) 2013 STMicroelectronics
 *	Sean McGoogan <Sean.McGoogan@st.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
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


#ifndef CONFIG_SYS_DCACHE_OFF
extern void enable_caches(void)
{
	/* Enable D-cache. I-cache is already enabled in start.S */
	dcache_enable();
}
#endif

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
