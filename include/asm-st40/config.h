/*
 * Copyright 2009 Freescale Semiconductor, Inc.
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

#ifndef _ASM_CONFIG_H_
#define _ASM_CONFIG_H_

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


#endif /* _ASM_CONFIG_H_ */
