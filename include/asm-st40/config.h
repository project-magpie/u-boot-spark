/*
 * Copyright 2009 Freescale Semiconductor, Inc.
 * Copyright 2012 STMicroelectronics Ltd.
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

#endif /* _ASM_CONFIG_H_ */
