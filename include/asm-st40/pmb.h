/*
 * (C) Copyright 2008-2012 STMicroelectronics.
 *
 * Sean McGoogan <Sean.McGoogan@st.com>
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


#ifndef __ASM_ST40_PMB_H
#define __ASM_ST40_PMB_H

	/*
	 * The PMB contains 16 entries, and supports the following
	 * 4 page sizes: 16MiB, 64MiB, 128MiB, and 512MiB.
	 *
	 * Although the PMB has a total of 16 entries, but we will
	 * pretend that there are only 14, by grouping #0 and #1
	 * logically together, we will also group #2 and #3 logically
	 * together. Each of these two new groups will be used to map
	 * up to 256MB of main memory (LMI0). One group ([2:3]) will
	 * always map main memory as UN-cached. The other group ([0:1])
	 * will usually be cached, but will sometimes be un-cached for
	 * special needs. Both the new groups will reference the *same*
	 * physical memory, and hence they are virtual aliases for each other.
	 *
	 * Thus, PMB[0:1] is used as main memory (LMI), and will mostly
	 * be used as cached, except on initialization, and when passing
	 * control to the linux kernel, where it will be un-cached.
	 * The cacheability of PMB[0:1] may be toggled by calling
	 * the function sh_toggle_pmb_cacheability().
	 *
	 * However, when using 512 MiB, the two groups mentioned above
	 * are split differently, the "cached" group comprises only
	 * of a single 512MiB page (#0), and the other group has up
	 * to 3 un-cached PMB mappings (#1, #2, #3). It is expected
	 * that the un-cached group will map a *sub-set* of the cached
	 * group. That is, PMB[0] will map 512MiB as cached, but
	 * PMB[1:3] will probably map less than the full 512MiB, as
	 * un-cached.
	 *
	 * With 256MiB (or less) of usable SDRAM, PMB[2:3] is an
	 * alias for PMB[0:1], except it is guaranteed to always
	 * be UN-cached.
	 *
	 * PMB[0:1]  will map 0x80000000 .. 0x8fffffff.	(256MB)
	 * PMB[2:3]  will map 0x90000000 .. 0x9fffffff.	(256MB)
	 * PMB[4:15] will map 0xa0000000 .. 0xbfffffff.	(512MB)
	 *
	 * If sizeof(LMI) <= 128MB, then PMB[1] and PMB[3] are *unused*,
	 * i.e. PMB[1].V == PMB[3].V == 0.
	 *
	 * With 512MiB of usable SDRAM, only PMB[0] has a defined
	 * virtual address range. Typically, there will not be
	 * an un-cached alias for all of it, but the sub-set that is
	 * aliased will be mapped via the 3 slots PMB[1:3].
	 *
	 * PMB[0]    will map 0x80000000 .. 0x9fffffff.	(512MB)
	 * PMB[1:15] will map 0xa0000000 .. 0xbfffffff.	(512MB)
	 *
	 * Note: PMB[0:3] should all be initialized as UN-cached,
	 * and then sh_toggle_pmb_cacheability() should be called
	 * to enable the caching of PMB[0]. If the predicate macro
	 * CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES is true, then
	 * sh_toggle_pmb_cacheability() will also toggle the
	 * cacheability of PMB[1] at the same time as PMB[0].
	 *
	 * Note: When using 512MiB of SDRAM, extreme care needs
	 * to be taken to ensure that only addresses in the mapped
	 * "sub-set" of the aliased SDRAM is de-referenced.  Any
	 * de-references to unmapped addresses will reset the system!
	 */


	/*
	 * Firstly, check that the size of the RAM is a strict
	 * multiple of 16 MiB (the smallest possible PMB page).
	 */
#if (CONFIG_SYS_SDRAM_SIZE & (16*1024*1024-1)) != 0
#	error CONFIG_SYS_SDRAM_SIZE is not a multiple of 16 MiB
#endif

	 /*
	  * If the main LMI memory is 256MB, then we need to have
	  * two PMB entries to map this amount of memory.
	  * Otherwise, we will always just use a single PMB page
	  * to map *all* the "usable" SDRAM (as cached).
	  * i.e.:
	  *
	  *	1 * 512 MiB page  for 512 MiB of RAM
	  *	2 * 128 MiB pages for 256 MiB of RAM
	  *	1 * 128 MiB page  for 128 MiB of RAM
	  *	1 *  64 MiB page  for  64 MiB of RAM (and smaller).
	  *
	  * CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES will yield TRUE
	  * if U-boot requires 2 PMB entries (#0, and #1) for main memory
	  * and FALSE if only a single entry (#0) is required.
	  */
#if (CONFIG_SYS_SDRAM_SIZE == (512*1024*1024) )			/* 512 MiB of RAM ? */
#	define CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES	0	/* use 1 * 512 MiB PMB page */
#	define CONFIG_SYS_ST40_LMI_PMB_PAGE_SIZE	512
		/* the PMB 16MiB VIRTUAL page where U-Boot will logically reside CACHED */
#	define CONFIG_SYS_ST40_PMB_TEXT_CACHED_PAGE	(TEXT_BASE & 0xFF000000)
		/* the PMB 16MiB VIRTUAL page where U-Boot will logically reside UN-cached */
#	define CONFIG_SYS_ST40_PMB_TEXT_UNCACHED_PAGE	(CONFIG_SYS_SE_UNCACHED_BASE)
		/* the PMB 16MiB PHYSICAL page where U-Boot will physically reside */
#	define CONFIG_SYS_ST40_PMB_TEXT_PHYSICAL_PAGE	(CONFIG_SYS_SE_PHYSICAL_BASE + ((512-16)*1024*1024))
#elif (CONFIG_SYS_SDRAM_SIZE == (256*1024*1024) )		/* 256 MiB of RAM ? */
#	define CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES	1	/* use 2 * 128 MiB PMB page */
#	define CONFIG_SYS_ST40_LMI_PMB_PAGE_SIZE	128
#elif (CONFIG_SYS_SDRAM_SIZE == (128*1024*1024) )		/* 128 MiB of RAM ? */
#	define CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES	0	/* use 1 * 128 MiB PMB page */
#	define CONFIG_SYS_ST40_LMI_PMB_PAGE_SIZE	128
#elif (CONFIG_SYS_SDRAM_SIZE == (64*1024*1024) )		/* 64 MiB of RAM ? */
#	define CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES	0	/* use 1 * 64 MiB PMB page */
#	define CONFIG_SYS_ST40_LMI_PMB_PAGE_SIZE	64
#elif (CONFIG_SYS_SDRAM_SIZE == (48*1024*1024) )		/* 48 MiB of RAM ? */
#	define CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES	0	/* use 1 * 64 MiB PMB page */
#	define CONFIG_SYS_ST40_LMI_PMB_PAGE_SIZE	64
#elif (CONFIG_SYS_SDRAM_SIZE == (32*1024*1024) )		/* 32 MiB of RAM ? */
#	define CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES	0	/* use 1 * 64 MiB PMB page */
#	define CONFIG_SYS_ST40_LMI_PMB_PAGE_SIZE	64
#else								/* Oh, dear! */
#	error Unepected value for CONFIG_SYS_SDRAM_SIZE
#endif



#endif	/* __ASM_ST40_PMB_H */

