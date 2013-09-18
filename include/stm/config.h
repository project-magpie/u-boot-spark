/*
 * Copyright 2009 Freescale Semiconductor, Inc.
 *
 * Copyright 2012-2013 STMicroelectronics Ltd.
 *	Sean McGoogan <Sean.McGoogan@st.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#ifndef __INCLUDE_STM_CONFIG_H
#define __INCLUDE_STM_CONFIG_H

/* Relocation to SDRAM works on all ST40 boards */
#define CONFIG_RELOC_FIXUP_WORKS

/* Enable use of the Logical Memory Blocks (LMB). */
#define CONFIG_LMB

/*
 * (Optionally) enable a few more generally 'useful' commands ...
 */
#if 0
#	define CONFIG_CMD_STTESTS	/* provides "st_mtest" */
#	define CONFIG_CMD_MD5SUM	/* provides "md5sum" */
#	define CONFIG_CMD_SHA1		/* provides "sha1sum" */
#	define CONFIG_CMD_STRINGS	/* provides "strings" */
#	define CONFIG_CMD_UNZIP		/* provides "unzip" */
#	define CONFIG_MD5		/* needed by CONFIG_CMD_MD5SUM */
#	define CONFIG_SHA1		/* needed by CONFIG_CMD_SHA1 */
#endif


	/*
	 * Increase some macros which control the maximum sizes
	 * of buffers/arrays for processing U-Boot commands.
	 * Building up linux command lines (e.g. bootargs)
	 * often needs these limits raised, as the linux
	 * command lines tend to grow (and grow)!
	 */
#if !defined(CONFIG_SYS_CBSIZE)		/* (input) Console Buffer size */
#	define CONFIG_SYS_CBSIZE	(2*1024)	/* 2KiB */
#endif

#if !defined(CONFIG_SYS_PBSIZE)		/* (output) Print Buffer size */
#	define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#endif

#if !defined(CONFIG_SYS_MAXARGS)	/* max number of command arguments */
#	define CONFIG_SYS_MAXARGS	32
#endif

	/*
	 * In the include/configs/<board>.h files, we may want to use
	 * the roundup() macro, to ensure that various configuration #defines
	 * are appropriately "rounded" up. This is especially true for
	 * dealing with very-large page NAND devices, and the definitions
	 * of both CONFIG_ENV_SIZE, and CONFIG_SYS_NAND_ENV_OFFSET, which
	 * typically will be multiples of NAND erase-size blocks.
	 * We include this macro here, to avoid its definition being replicated.
	 */
#if !defined(roundup)
#	define roundup(x, y)		((((x) + ((y) - 1)) / (y)) * (y))
#endif


	/*
	 * pull in the {in,out}[s][bwl] family of macros from
	 * the file arch/arm/include/asm/io.h
	 */
#if defined(CONFIG_ARM)
#	define __io
#endif /* CONFIG_ARM */


	/*
	 * add additional (core-specific) CONFIG_* options.
	 */
#if defined(CONFIG_ARM)
#	define CONFIG_ARCH_CPU_INIT	/* call arch_cpu_init() */
#	define CONFIG_DISPLAY_BOARDINFO	/* call checkboard() */
#	define CONFIG_MISC_INIT_R	/* call misc_init_r() */
#	define CONFIG_CMD_CACHE		/* enable "icache" & "dcache" commands */
#	define CONFIG_SETUP_MEMORY_TAGS	/* use ATAGs for Memory Specification */
#	define CONFIG_CMDLINE_TAG	/* use ATAGs for Kernel's Command Line */
#	define CONFIG_INITRD_TAG	/* use ATAGs for an init ramdisk */
#	define CONFIG_OF_LIBFDT		/* enable "fdt" command and support DTBs */
#endif /* CONFIG_ARM */


	/*
	 * We want some SRAM to store the "poke-table", whilst the
	 * "pokeloop" interpreter is being executed. This is because
	 * whilst the "pokeloop" interpreter is being executed, both
	 * LMI and EMI may temporarily "disappear" from the CPU.
	 * On the ARM, it is considered to be "unsafe" to use the
	 * D-caches to hold the poke table (c.f. ST40), so we will
	 * use a small region in SRAM instead, as this will be safer.
	 *
	 * We will also store U-Boot's (non-flash) "holding-pens"
	 * for the secondary cores, in SRAM, rather than in SDRAM.
	 * So, we also need a small region reserved for that use.
	 *
	 * The SRAM region only needs to be big enough to hold:
	 *	* the "holding-pens", and their data;
	 *	* the raw binary poke-table;
	 *	* the poke_loop() function as well.
	 * So 32KiB (or more) should be sufficient for our needs.
	 *
	 * CONFIG_STM_SRAM_START is the beginning of this SRAM region.
	 */
#if !defined(CONFIG_STM_SRAM_START)
#	define CONFIG_STM_SRAM_START		0xc00c0000	/* Start of HVA_eRAM (256KiB) */
#endif
#if !defined(CONFIG_STM_SRAM_HOLDING_PEN)	/* For U-Boot's "Holding-Pens" (in SRAM) */
#	define CONFIG_STM_SRAM_HOLDING_PEN	(CONFIG_STM_SRAM_START + 0x0000)
#endif
#if !defined(CONFIG_STM_SRAM_POKE_STORAGE)	/* For the "poke" interpreter (in SRAM) */
#	define CONFIG_STM_SRAM_POKE_STORAGE	(CONFIG_STM_SRAM_START + 0x0100)
#endif


	/*
	 * For all STMicroelectronics board, we now want to use the
	 * "newer" (multi-MAC) network API (CONFIG_NET_MULTI).
	 * The "older" API is deprecated, and about to be removed.
	 */
#define CONFIG_NET_MULTI

	/*
	 * If no MAC/GMAC driver is to be included, then we should
	 * not include *any* of the "networking" sub-modules,
	 * otherwise we may have a fatal linking problem!
	 * We conveniently un-define them all here, just in case!
	 */
#if !defined(CONFIG_DRIVER_NET_STM_GMAC) && !defined(CONFIG_DRIVER_NETSTMAC)
#	undef CONFIG_CMD_NET		/* No networking! */
#	undef CONFIG_CMD_NFS		/* No networking! */
#	undef CONFIG_CMD_PING		/* No networking! */
#	undef CONFIG_CMD_DHCP		/* No networking! */
#endif	/* !CONFIG_DRIVER_NET_STM_GMAC && !CONFIG_DRIVER_NETSTMAC */


#endif /* __INCLUDE_STM_CONFIG_H */
