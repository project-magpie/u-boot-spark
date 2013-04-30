/*
 * (C) Copyright 2013 STMicroelectronics.
 *
 * Sean McGoogan <Sean.McGoogan@st.com>
 *
 * Copied from the file "arch/st40/lib/board.c"
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

#if defined(CONFIG_ST40)
#include <asm/addrspace.h>
#endif	/* CONFIG_ST40 */


/*
 *	The STMicroelectronics Memory Map
 *	=================================
 *
 *	Base:						Size:			Comment:
 *	-----						-----			--------
 *
 *	CONFIG_SYS_SDRAM_BASE+CONFIG_SYS_SDRAM_SIZE				Top Of DRAM Memory
 *
 *	TEXT_BASE					1MiB			entry point
 *
 *	mem_malloc_start				CONFIG_SYS_MALLOC_LEN	"heap" used by malloc() and friends
 *
 *	gd						sizeof(gd_t)		struct global_data
 *
 *	gd->bd						sizeof(bd_t)		struct bd_info
 *
 *	small "hole"					CONFIG_SYS_GBL_DATA_SIZE-(sizeof(gd_t)+sizeof(bd_t))
 *
 *	stack_addr								Stack (growing downwards, towards CONFIG_SYS_MEMTEST_END)
 *										stack_addr = TEXT_BASE-(CONFIG_SYS_GBL_DATA_SIZE+CONFIG_SYS_MALLOC_LEN)
 *
 *	CONFIG_SYS_MEMTEST_END							default upper-bound for "mtest"
 *
 *	CONFIG_SYS_MEMTEST_START						default lower-bound for "mtest"
 *
 *	CONFIG_SYS_SDRAM_BASE							Bottom Of DRAM Memory
 *
 *
 *	Note: the above assumes that:
 *
 *		1) sizeof(gd_t) + sizeof(bd_t) <= CONFIG_SYS_GBL_DATA_SIZE
 *
 *		2) CONFIG_SYS_MEMTEST_END is ~ 1MiB below "stack_addr"
 */


/*
 * The "mtest" command will totally trash the system, if the address
 * U-Boot is running from (starting at TEXT_BASE) is included the
 * range of memory we are testing. We ensure here that the "default"
 * range that "mtest" uses is not stupid!
 * This is done only as a compile-time test.
 *
 * Currently, there are several macros which define where SDRAM
 * starts, how big it is, and where various things in RAM are located.
 * Unfortunately, it is possible to define these different
 * macros such that the overall set is mutually inconsistent!
 * In the future, it should be a goal to define only TWO of these
 * macros, and derive all the others automatically. To this end
 * the following code will issue compile-time diagnostics, if
 * the proposed derivations would fail!
 *
 * The two essential macros to be defined:
 *	CONFIG_SYS_SDRAM_BASE, CONFIG_SYS_SDRAM_SIZE
 *
 * Derived Macros:
 *	CONFIG_SYS_LOAD_ADDR		= CONFIG_SYS_SDRAM_BASE
 *	CONFIG_SYS_MEMTEST_START	= CONFIG_SYS_SDRAM_BASE
 *	CONFIG_SYS_MEMTEST_END		= TEXT_BASE - CONFIG_SYS_MALLOC_LEN - 1MiB
 *	TEXT_BASE			= CONFIG_SYS_SDRAM_BASE + CONFIG_SYS_SDRAM_SIZE - 1MiB
 *	CONFIG_SYS_SE_SDRAM_WINDOW	= CONFIG_SYS_SDRAM_SIZE - 1
 *
 * Note: Both 1MiB figures above are probably "too safe", and
 * could (if needed) be reduced somewhat.
 *
 * The 1MiB figure in CONFIG_SYS_MEMTEST_END really only needs to be
 * CONFIG_SYS_GBL_DATA_SIZE plus the size of the maximum stack size!
 * Where CONFIG_SYS_GBL_DATA_SIZE could be reduced to be exactly:
 * sizeof(gd_t) + sizeof(bd_t), removing the "small hole" in the
 * memory map. However, using 1MiB should be uber-conservative.
 * It is essential that "mtest" does not trash the stack!
 *
 * The 1MiB figure in verifying TEXT_BASE, is just a "nice round number".
 * It could probably be reduced to be just the size of the raw u-boot.bin
 * binary image, plus the size of the (zero-initialized) ".bss" section!
 * However, having it over-sized is very convenient, as it means the
 * entry point is always just 1MiB below the "top of RAM".
 *
 * It is recommended that all include/config/<board>.h files
 * use the following two definitions:
 *
 *	#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
 *	#define CONFIG_SYS_MEMTEST_END		(TEXT_BASE - CONFIG_SYS_MALLOC_LEN - (1 << 20))
 */
#if (TEXT_BASE >= CONFIG_SYS_MEMTEST_START) && (TEXT_BASE < CONFIG_SYS_MEMTEST_END)
#	warning "mtest" will fail when CONFIG_SYS_MEMTEST_START < TEXT_BASE < CONFIG_SYS_MEMTEST_END!
#endif

#if CONFIG_SYS_LOAD_ADDR != CONFIG_SYS_SDRAM_BASE
#	warning CONFIG_SYS_LOAD_ADDR != CONFIG_SYS_SDRAM_BASE
#endif

#if CONFIG_SYS_MEMTEST_START != CONFIG_SYS_SDRAM_BASE
#	warning CONFIG_SYS_MEMTEST_START != CONFIG_SYS_SDRAM_BASE
#endif

#if CONFIG_SYS_MEMTEST_END != (TEXT_BASE - CONFIG_SYS_MALLOC_LEN - (1 << 20))
#	warning CONFIG_SYS_MEMTEST_END != TEXT_BASE - CONFIG_SYS_MALLOC_LEN - 1MiB
#endif

#if TEXT_BASE != (CONFIG_SYS_SDRAM_BASE + CONFIG_SYS_SDRAM_SIZE - (1 << 20))
#	warning TEXT_BASE != CONFIG_SYS_SDRAM_BASE + CONFIG_SYS_SDRAM_SIZE - 1MiB
#endif

#if defined(CONFIG_ST40) && defined(CONFIG_ST40_SE_MODE)	/* ST40, only in 32-bit mode */
#   if !defined(CONFIG_SYS_SE_SDRAM_WINDOW)
#	warning CONFIG_SYS_SE_SDRAM_WINDOW is not defined in 32-bit mode
#   elif CONFIG_SYS_SE_SDRAM_WINDOW != (CONFIG_SYS_SDRAM_SIZE - 1)
#	warning CONFIG_SYS_SE_SDRAM_WINDOW != CONFIG_SYS_SDRAM_SIZE - 1
#   endif
#endif	/* CONFIG_ST40 && CONFIG_ST40_SE_MODE */
