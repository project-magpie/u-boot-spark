/*
 * (C) Copyright 2004-2013 STMicroelectronics.
 *
 * Andy Sturges <andy.sturges@st.com>
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

#include <config.h>
#include "asm/regdef.h"
#include "asm/addrspace.h"
#include "asm/sh4reg.h"
#include "asm/asmdefs.h"
#include "asm/pmb.h"
#include <stm/socregs.h>


	.section .text.init, "ax"

	.global _start
_start:

	/* Add a few NOP instructions, to workaround the
	 * problem reported in the following bugzilla report:
	 * https://bugzilla.stlinux.com/show_bug.cgi?id=4173 */
	nop
	nop

	/* Set SR:	MD=1, RB=0, BL=0, FD=0, IMASK=0xF */
set_sr:
	MOV_CONST32_R0 0x400000F0
	ldc	r0, sr

	/* disable the watchdog timer */
disable_watchdog:
	MOV_CONST32_R0 ST40_CPG_WTCSR
	mov	r0, r1
	MOV_CONST16_R0 0xA500	/* WTCSR.TME=0 */
	mov.w   r0, @r1

	/*
	 * If were are going to boot U-boot from NAND flash, then
	 * we need add a special "signature" pattern in the first
	 * logical block (block zero).
	 */
#ifdef CONFIG_SYS_BOOT_FROM_NAND
skip_signature:
	bra	skipped_signature	/* skip over the "block 0 signature" */
	 nop

	/*
	 * Write out the 64-byte preamble signature pattern, to identify
	 * the current NAND flash block, as the good "logical block ZERO".
	 * This signature must start at offset +0x0080 in the block.
	 */
	.balign 0x80, 0x00		/* starts 128 bytes into "Block Zero" */
preamble_signature_pattern:
	BYTES 0x00 0x3f			/* 64-bytes: 0x00, 0x01, ..., 0x3f */

#if defined(CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING)
	/*
	 *	const static struct skip_bb
	 *	{
	 *		char	pattern[8];
	 *		u32	block_size;
	 *		u32	num_blocks;
	 *		u32	array[num_blocks];
	 *	} skip_bb;
	 *
	 *	"array[]" is a set of flags, whose value is
	 *	interpreted as:
	 *		if array[x] == 0,
	 *		then physical block #x is GOOD,
	 *		else physical block #x is BAD.
	 *	NOTE Physical Block #0 *must* always be GOOD.
	 *
	 *	assert( strcmp(pattern,"SKIP_BBs") == 0 );
	 *	assert( (block_size % (16<<10)) == 0 );
	 *	assert( sizeof(array) == num_blocks * 4 );
	 *	assert( array[0] == 0 );
	 *
	 *	By default the linker will fill "array[]"
	 *	with ZEROS (assume all blocks are GOOD).
	 *	If is the responsibility of the configurer
	 *	to ensure that "array[]" is big enough.
	 *	It is the responsibility of the NAND flasher
	 *	to fill the array appropriately.
	 */
skip_bb:			/* the skip_bb structure */
skip_bb_pattern:		/* 8-byte "magic" pattern */
	.ascii "SKIP_BBs"
skip_bb_size:			/* size of each NAND block */
	.long	CONFIG_SYS_NAND_SKIP_BLOCK_SIZE
skip_bb_count:			/* number of bad-block entries in array */
	.long	CONFIG_SYS_NAND_SKIP_BLOCK_COUNT
skip_bb_array:			/* the array itself (0==GOOD) */
	.fill CONFIG_SYS_NAND_SKIP_BLOCK_COUNT, 4, 0
#endif	/* CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING */

skipped_signature:
					/* resume from here */
#endif	/* CONFIG_SYS_BOOT_FROM_NAND */


	/*
	 * If were are going to boot U-boot from SPI (serial flash),
	 * then the following pokes are required/recommended to be
	 * done early, when using the EMI's SPIBOOT mode controller.
	 */
#if defined(CONFIG_SYS_BOOT_FROM_SPI)
config_spiboot_controller:
	MOV_CONST32_R0 STM_EMI_SPI_REGS_BASE
	mov	r0, r1

	MOV_CONST32_R0 0x00020011	/* *STM_EMI_SPI_CONFIGDATA = 0x00020010|ST */
	mov.l	r0, @(STM_EMI_SPI_CONFIGDATA-STM_EMI_SPI_REGS_BASE,r1)

#if 1
	/* For serial flash we use FAST_READ */
	mov	#2, r0			/* *STM_EMI_SPI_MODESELECT = FAST_READ */
#else
	mov	#3, r0			/* *STM_EMI_SPI_MODESELECT = CONTIG_MODE|FAST_READ */
#endif
	mov.l	r0, @(STM_EMI_SPI_MODESELECT-STM_EMI_SPI_REGS_BASE,r1)

	/* Slow (divide) the SPI clock down to improve reliability. */
	/* Note: for some (?) FSM SPI controllers, diving by 4 fails to boot! */
#if defined(CONFIG_STM_FLI7510) || defined(CONFIG_STM_FLI7540) || defined(CONFIG_STM_STX5206) || defined(CONFIG_STM_STX7108)
	mov	#6, r0			/* *STM_EMI_SPI_CLOCKDIV = 6 */
#else
	mov	#4, r0			/* *STM_EMI_SPI_CLOCKDIV = 4 */
#endif
	mov.l	r0, @(STM_EMI_SPI_CLOCKDIV-STM_EMI_SPI_REGS_BASE,r1)

	/*
	 * Note: we need to ensure that there has been at least one
	 * transaction on the SPI bus, before we can read (or poll)
	 * the SPI_STATUSMODECHANGE register later on.
	 * Any valid peek will do. We may as well do it now.
	 */
dummy_peek_on_spi:
	mov	#0, r0		/* Read word from SPI at offset zero */
	mov.l	@r0, r0		/* peek(<somewhere-in-SPI>) */
#endif	/* CONFIG_SYS_BOOT_FROM_SPI */


	/*
	 * Determine if we are in the correct location ? (i.e. RAM)
	 *  r8 == 1 in RAM, assume initialization already done by GDB.
	 *  r8 == 0 in FLASH, need to relocate, set up memory, etc...
	 *  r9 == where we actually ARE      (PIC relocate source).
	 * r10 == were we WANT to be [BEGIN] (PIC relocate target).
	 * r11 == were we WANT to be [END]   (PIC relocate target).
	 *
	 * NOTE: when CONFIG_ST40_SE_MODE is defined, then also:
	 *  r8 == 1 assume we are already in 32-bit SE mode (done by GDB).
	 *  r8 == 0 we are in 29-bit mode, and need to switch to 32-bit mode.
	 */
which_location:
	MOV_CONST32_R0 0x1fffffff	/* set up P0 mask  */
	mov	r0, r4
	mova	_start_offset, r0	/* Load source address in r9 */
	mov	r0, r9			/* i.e. where we actually ARE */
	mov.l	@r9, r10
	sub	r10, r9
#ifndef CONFIG_ST40_SE_MODE
	and	r4, r9			/* make sure its P0 (cached) */
#endif	/* CONFIG_ST40_SE_MODE */
	mov.l	target_addr, r10	/* Load target address in r10 */
					/* i.e. were we WANT to be */
#ifndef CONFIG_ST40_SE_MODE
	and	r4, r10			/* make sure its P0 (cached) */
#endif	/* CONFIG_ST40_SE_MODE */
	mov.l	target_end, r11		/* load target END address in r11 */
#ifndef CONFIG_ST40_SE_MODE
	and	r4, r11			/* make sure its P0 (cached) */
#endif	/* CONFIG_ST40_SE_MODE */
	cmp/eq	r9, r10			/* Are we in correct place already ? */
	movt	r8			/* save SR.T in r8. */
#ifdef CONFIG_ST40_SE_MODE
	and	r4, r9			/* make sure its P0 (cached) */
#endif	/* CONFIG_ST40_SE_MODE */
#if defined(CONFIG_SYS_STM_BART_BASE)
	/* OR, are we already running from RAM, mapped via the BART ? */
	MOV_CONST32_R0 (CONFIG_SYS_STM_BART_BASE + (TEXT_BASE & ((64<<20)-1)))
	cmp/eq	r9, r0			/* is the PC in the BART region ? */
	movt	r0
	or	r0, r8			/* r8 |= (PC==BART); */
#endif	/* CONFIG_SYS_STM_BART_BASE */

	/* set up the PMB entries we want to use */
#ifdef CONFIG_ST40_SE_MODE
	/*
	 * explicitly invalidate all unused entries
	 * NOTE: for running from RAM, then keep PMB[0] as is.
	 */
invalidate_pmb:
	MOV_CONST32_R0 (P4SEG_PMB_ADDR)
	mov	r0, r1
	mov	#0, r2			/* PMB[n].V = 0, i.e. INVALID */
	mov	#1, r3
	shll8	r3			/* R3 = address stride is 0x100 */
	mov	#1, r0			/* R0 = counter (indexes: 1..15) */
1:	add	r3, r1			/* next index pointer */
	mov.l	r2, @r1			/* invalidate current entry */
	cmp/eq	#15, r0			/* finished ? */
	bf/s	1b			/* more to do ? */
	  add	#1, r0			/* next index counter */

	/*
	 * Set up the new PMB entries we want to use.
	 *
	 * Note, the first 4 PMB slots should exclusively be used for
	 * mapping the (first) LMI (ultimately half cached, half uncached).
	 * In *this* file they *must* all be exclusively UN-CACHED!
	 * We also define "BASE" as a convenience macro to ensure it
	 * automatically tracks with CONFIG_SYS_SE_PHYSICAL_BASE, which should
	 * be set to the physical base of the (first) LMI.
	 */
#undef  BASE	/* for the "convenience" macro */
#define BASE	(CONFIG_SYS_SE_PHYSICAL_BASE>>24)/* move the MSB to the LSB */
#undef  UBVPN	/* U-Boot's ".text" section, as PMB VIRTUAL page number UN-CACHED */
#define UBVPN	(CONFIG_SYS_ST40_PMB_TEXT_UNCACHED_PAGE>>24)
#undef  UBPPN	/* U-Boot's ".text" section, as PMB PHYSICAL page number */
#define UBPPN	(CONFIG_SYS_ST40_PMB_TEXT_PHYSICAL_PAGE>>24)
#undef  SIZE	/* default PMB page size to use for main LMI memory */
#define SIZE	(CONFIG_SYS_ST40_LMI_PMB_PAGE_SIZE)

set_pmb:
#if CONFIG_SYS_SDRAM_SIZE == 512*1024*1024	/* 512 MiB of RAM ? */
	/*
	 *	For 512 MiB, we create 2 PMB mappings, which ultimately will be:
	 *
	 *	Slot	Page-Size	Caching	  Virtual-Address
	 *	----	---------	-------	  ---------------
	 *	PMB[0]	512 MiB		CACHED	  0x80000000..0x9fffffff
	 *	PMB[2]	 16 MiB		UN-cached 0xbf000000..0xbfffffff
	 *
	 * The entirety of the 16 MiB page (PMB[2]), will *PHYSICALLY* be
	 * the same as the last 16 MiB region in the 512MiB page. That is,
	 * the same 16 MiB of physical memory will be mapped twice (once
	 * cached, and once un-cached) in the PMB virtual area. This
	 * area that is mapped twice should include U-Boot's ".text"
	 * section, i.e. the memory around "TEXT_BASE".
	 */
	SH4_SET_PMB 0,0x80,BASE,SIZE,0,0,1	/* LMI (512 MiB), will eventually be CACHED */
	SH4_SET_PMB 2,UBVPN,UBPPN,16,0,0,1	/* Last 16MiB of LMI un-cached (for U-Boot's TEXT) */
#else	/* 512 MiB of RAM ? */
	SH4_SET_PMB 0,0x80,BASE,SIZE,0,0,1	/* LMI (first-half), will eventually be CACHED */
	SH4_SET_PMB 2,0x90,BASE,SIZE,0,0,1	/* LMI (first-half), permanently UN-cached */
	#if CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES
	SH4_SET_PMB 1,0x88,BASE+0x08,SIZE,0,0,1	/* LMI (second-half), will eventually be CACHED */
	SH4_SET_PMB 3,0x98,BASE+0x08,SIZE,0,0,1	/* LMI (second-half), permanently UN-cached */
	#endif	/* CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES */
#endif	/* 512 MiB of RAM ? */

#undef  BASE
#undef  UBVPN
#undef  UBPPN
#undef  SIZE

#if defined(CONFIG_STM_STX5197) && defined(CONFIG_STM_MB704)
	SH4_SET_PMB 4,0xa0,0x00,128,0,0,1	/* SPI Flash (in SPIBOOT mode) UN-cached */
#elif defined(CONFIG_STM_STX5197) && defined(CONFIG_STM_5197CAB)
	SH4_SET_PMB 4,0xa0,0x00,128,0,0,1	/* SPI Flash (in SPIBOOT mode) UN-cached */
#elif defined(CONFIG_STM_STX5206) && defined(CONFIG_STM_MB796)
	#if defined(CONFIG_SYS_BOOT_FROM_NAND) || defined(CONFIG_SYS_BOOT_FROM_SPI)
		/* booting from NAND or SPI, so CSA and CSB swapped in EPLD */
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 5,0xa4,0x04, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6,0xa5,0x05, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 7,0xa8,0x04, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8,0xa9,0x05, 16		/* NOR FLASH CACHED */
	#else	/* else, CSA and CSB are not swapped in EPLD */
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5,0xa1,0x01, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6,0xa4,0x04, 16,0,0,1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 7,0xa8,0x00, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8,0xa9,0x01, 16		/* NOR FLASH CACHED */
	#endif	/* CONFIG_SYS_BOOT_FROM_NAND */
#elif defined(CONFIG_STM_STX5206) && defined(CONFIG_STM_HDK5289)
	#if defined(CONFIG_SYS_BOOT_FROM_NAND) || defined(CONFIG_SYS_BOOT_FROM_SPI)
		/* booting from NAND or SPI, so CSA and CSB are swapped */
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 5,0xa4,0x04, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6,0xa5,0x05, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 7,0xa8,0x04, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8,0xa9,0x05, 16		/* NOR FLASH CACHED */
	#else	/* else, CSA and CSB are not swapped */
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5,0xa1,0x01, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6,0xa4,0x04, 16,0,0,1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 7,0xa8,0x00, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8,0xa9,0x01, 16		/* NOR FLASH CACHED */
	#endif	/* CONFIG_SYS_BOOT_FROM_NAND */
#elif defined(CONFIG_STM_STX7105) && defined(CONFIG_STM_MB680)
	#ifdef CONFIG_SYS_BOOT_FROM_NAND /* booting from NAND, so CSA and CSB swapped in EPLD */
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 5,0xa4,0x04, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6,0xa5,0x05, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 7,0xa8,0x04, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8,0xa9,0x05, 16		/* NOR FLASH CACHED */
	#else	/* else, CSA and CSB are not swapped in EPLD */
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5,0xa1,0x01, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6,0xa4,0x04, 16,0,0,1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 7,0xa8,0x00, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8,0xa9,0x01, 16		/* NOR FLASH CACHED */
	#endif	/* CONFIG_SYS_BOOT_FROM_NAND */
#elif defined(CONFIG_STM_STX7105) && defined(CONFIG_STM_HDK7106)
	SH4_SET_PMB  4,0xa0,0x00, 64,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB  5,0xa4,0x04, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB  6,0xa5,0x05, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB  7,0xa6,0x06, 16,0,0,1	/* NAND + CI UN-cached */
	SH4_SET_PMB  8,0xa7,0x07, 16,0,0,1	/* PCI UN-cached */
	SH4_SET_PMB  9,0xa8,0x00, 64		/* NOR FLASH CACHED */
	SH4_SET_PMB 10,0xac,0x04, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 11,0xad,0x05, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 12,0xae,0x06, 16		/* NAND + CI CACHED */
	SH4_SET_PMB 13,0xaf,0x07, 16		/* PCI CACHED */
#elif defined(CONFIG_STM_STX7105) && defined(CONFIG_STM_PDK7105)
	SH4_SET_PMB  4,0xa0,0x00, 64,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB  5,0xa4,0x04, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB  6,0xa5,0x05, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB  7,0xa6,0x06, 16,0,0,1	/* NAND + CI UN-cached */
	SH4_SET_PMB  8,0xa7,0x07, 16,0,0,1	/* PCI UN-cached */
	SH4_SET_PMB  9,0xa8,0x00, 64		/* NOR FLASH CACHED */
	SH4_SET_PMB 10,0xac,0x04, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 11,0xad,0x05, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 12,0xae,0x06, 16		/* NAND + CI CACHED */
	SH4_SET_PMB 13,0xaf,0x07, 16		/* PCI CACHED */
#elif defined(CONFIG_STM_STX7105) && defined(CONFIG_STM_IPIDTV7105)
	SH4_SET_PMB 4,0xa0,0x00, 64,0,0,1	/* FLASH UN-cached */
#elif defined(CONFIG_STM_STX7108) && defined(CONFIG_STM_MB837)
	#ifdef CONFIG_SYS_BOOT_FROM_NAND /* booting from NAND, so CSA and CSB swapped in EPLD */
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 5,0xa4,0x04, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6,0xa5,0x05, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 7,0xa8,0x04, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8,0xa9,0x05, 16		/* NOR FLASH CACHED */
	#else	/* else, CSA and CSB are not swapped in EPLD */
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5,0xa1,0x01, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6,0xa4,0x04, 16,0,0,1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 7,0xa8,0x00, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8,0xa9,0x01, 16		/* NOR FLASH CACHED */
	#endif	/* CONFIG_SYS_BOOT_FROM_NAND */
#elif defined(CONFIG_STM_STX7108) && defined(CONFIG_STM_MB903)
	SH4_SET_PMB 4,0xa0,0x00, 128,0,0,1	/* NOR FLASH UN-cached (lower 64MiB) */
	SH4_SET_PMB 5,0xb0,0x00, 128		/* NOR FLASH CACHED */
#elif defined(CONFIG_STM_STX7108) && defined(CONFIG_STM_HDK7108)
	SH4_SET_PMB 4,0xa0,0x00, 128,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5,0xb0,0x00, 128		/* NOR FLASH CACHED */
	SH4_SET_PMB 6,0xa8,0x08, 128,0,0,1	/* NAND FLASH UN-cached */
#elif defined(CONFIG_STM_STX7108) && defined(CONFIG_STM_B2037)
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* SPI FLASH UN-cached */
	SH4_SET_PMB 5,0xb0,0x00, 16		/* SPI FLASH CACHED */
	SH4_SET_PMB 6,0xa8,0x08, 16,0,0,1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 7,0xab,0x0b, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 8,0xbb,0x0b, 16		/* NOR FLASH CACHED */
#elif defined(CONFIG_STM_STX7111) && defined(CONFIG_STM_MB618)
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NOR/NAND FLASH UN-cached */
	SH4_SET_PMB 5,0xa1,0x01, 16,0,0,1	/* NOR/NAND FLASH UN-cached */
	SH4_SET_PMB 6,0xa8,0x00, 16		/* NOR/NAND FLASH CACHED */
	SH4_SET_PMB 7,0xa9,0x01, 16		/* NOR/NAND FLASH CACHED */
#ifndef CONFIG_STM_NO_EPLD
	SH4_SET_PMB 8,0xb6,0x06, 16,0,0,1	/* EPLD UN-cached */
#endif	/* CONFIG_STM_NO_EPLD */
#elif defined(CONFIG_STM_STX7111) && defined(CONFIG_STM_HDK7111)
	SH4_SET_PMB 4,0xa0,0x00, 64,0,0,1	/* NOR/NAND/SPI FLASH UN-cached */
	SH4_SET_PMB 5,0xa8,0x00, 64		/* NOR/NAND/SPI FLASH CACHED */
	SH4_SET_PMB 6,0xa4,0x04, 16,0,0,1	/* NOR/NAND     FLASH UN-cached */
	SH4_SET_PMB 7,0xa5,0x05, 16,0,0,1	/* NOR/NAND     FLASH UN-cached */
	SH4_SET_PMB 8,0xac,0x04, 16		/* NOR/NAND     FLASH CACHED */
	SH4_SET_PMB 9,0xad,0x05, 16		/* NOR/NAND     FLASH CACHED */
#elif defined(CONFIG_STM_STX7141) && defined(CONFIG_STM_MB628)
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NOR/NAND FLASH UN-cached */
	SH4_SET_PMB 5,0xa1,0x01, 16,0,0,1	/* NOR/NAND FLASH UN-cached */
	SH4_SET_PMB 6,0xa8,0x00, 16		/* NOR/NAND FLASH CACHED */
	SH4_SET_PMB 7,0xa9,0x01, 16		/* NOR/NAND FLASH CACHED */
	SH4_SET_PMB 8,0xb5,0x05, 16,0,0,1	/* EPLD UN-cached */
#elif defined(CONFIG_STM_STX7141) && defined(CONFIG_STM_EUD7141)
	SH4_SET_PMB 4,0xa0,0x00, 64,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5,0xa8,0x00, 64		/* NOR FLASH CACHED */
	SH4_SET_PMB 6,0xa4,0x04, 16,0,0,1	/* NAND FLASH UN-cached */
#elif defined(CONFIG_STM_STX7141) && defined(CONFIG_STM_B2042)
	SH4_SET_PMB 4,0xa0,0x00, 64,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5,0xa8,0x00, 64		/* NOR FLASH CACHED */
	SH4_SET_PMB 6,0xa4,0x04, 16,0,0,1	/* NAND FLASH UN-cached */
#elif defined(CONFIG_STM_STX7200) && defined(CONFIG_STM_MB671)
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5,0xa1,0x01, 16,0,0,1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6,0xa8,0x00, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 7,0xa9,0x01, 16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8,0xb5,0x05, 16,0,0,1	/* EPLD UN-cached */
#elif defined(CONFIG_STM_FLI7510) && defined(CONFIG_STM_FLDB)
	SH4_SET_PMB 4,0xa0,0x00, 64,0,0,1	/* NOR/NAND FLASH UN-cached */
	SH4_SET_PMB 5,0xa8,0x00, 64		/* NOR/NAND FLASH CACHED */
#elif defined(CONFIG_STM_FLI7540) && defined(CONFIG_STM_FUDB)
	SH4_SET_PMB 4,0xa0,0x00, 64,0,0,1	/* NOR/NAND FLASH UN-cached */
	SH4_SET_PMB 5,0xa8,0x00, 64		/* NOR/NAND FLASH CACHED */
#elif ( defined(CONFIG_STM_STXH415) || defined(CONFIG_STM_STXH416) ) &&	\
	( defined(CONFIG_STM_B2000) || defined(CONFIG_STM_B2020) )
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NAND/SPI FLASH UN-cached */
	SH4_SET_PMB 5,0xb0,0x00, 16		/* NAND/SPI FLASH CACHED */
	SH4_SET_PMB 6,0xa1,0x01, 16,0,0,1	/* NAND/SPI FLASH UN-cached */
#elif defined(CONFIG_STM_STXH416) && defined(CONFIG_STM_B2116)
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NAND/SPI FLASH UN-cached */
	SH4_SET_PMB 5,0xb0,0x00, 16		/* NAND/SPI FLASH CACHED */
	SH4_SET_PMB 6,0xa1,0x01, 16,0,0,1	/* NAND/SPI FLASH UN-cached */
#elif defined(CONFIG_STM_STXH205) &&	\
	( defined(CONFIG_STM_B2057) || defined(CONFIG_STM_B2064) || defined(CONFIG_STM_B2067) || defined(CONFIG_STM_B2075) )
	SH4_SET_PMB 4,0xa0,0x00, 16,0,0,1	/* NAND/SPI FLASH UN-cached */
	SH4_SET_PMB 5,0xb0,0x00, 16		/* NAND/SPI FLASH CACHED */
	SH4_SET_PMB 6,0xa1,0x01, 16,0,0,1	/* NAND/SPI FLASH UN-cached */
#else
	#error Do not know which board/chip we are using for PMB setup.
#endif	/* defined(CONFIG_STM_STX....) */

	/*
	 * Invalidate the ITLB, in case GDB put a 512 MiB page in slot #0.
	 * Note: For SH4-200 series cores, we need to ensure that
	 * we preserve the MMUCR.SE bit faithfully. For SH4-300 series
	 * cores, the SE bit is actually in a different register (PASCR).
	 */
invalidate_itlb:
	MOV_CONST32_R0 SH4_CCN_MMUCR
	mov	#(SH4_MMUCR_TI), r1
#ifdef CONFIG_CPU_SUBTYPE_SH4_2XX	/* it is an SH4-200 */
	mov.l	@r0, r2			/* read the MMUCR */
	mov	#(SH4_MMUCR_SE), r3
	and	r3, r2			/* mask every bit, except MMUCR.SE */
	or	r2, r1			/* OR in MMUCR.TI */
#endif	/* CONFIG_CPU_SUBTYPE_SH4_2XX */
	mov.l	r1, @r0			/* set MMUCR.TI, preserving MMUCR.SE */
#endif	/* CONFIG_ST40_SE_MODE */

consider_hyperspace:
	cmp/pl	r8			/* Are we in correct place already ? */
	bt	hyperspace

	/* disable the caches */
do_disable_caches:
	mov	#-1, a0		/* clear all bits in the CCR */
	CALL	sh_cache_clear_op_offset

	/* invalidate+enable the caches: both I$ & D$ (with copy-back) */
do_enable_caches:
	MOV_CONST16_R0 (SH4_CCR_OCI|SH4_CCR_ICI|SH4_CCR_ICE|SH4_CCR_OCE|SH4_CCR_CB|SH4_CCR_EMODE)
	mov	r0, a0
	CALL	sh_cache_set_op_offset

	/*
	 * switch to P0 region (cachable), if we need to.
	 * Recall, we are still in 29-bit mode at this juncture.
	 */
enter_p0:
	ENTER_P0

	/*
	 * initialize the memory controllers, if we need to.
	 */
do_init_ram:
	CALL	init_ram_offset		/* init_ram() */

	/*
	 * enable SE (32-bit) mode, if we need to.
	 */
#ifdef CONFIG_ST40_SE_MODE
enable_se_mode:
#ifdef CONFIG_CPU_SUBTYPE_SH4_2XX	/* it is an SH4-200 */
	/* enable SE mode & invalidate the UTLB/ITLB */
	MOV_CONST32_R0 SH4_CCN_MMUCR
	mov	#(SH4_MMUCR_TI|SH4_MMUCR_SE), r1
	mov.l	r1, @r0			/* MMUCR.TI = MMUCR.SE = 1 */
#else					/* it is an SH4-300 */
	/* invalidate the UTLB/ITLB, first. */
	MOV_CONST32_R0 SH4_CCN_MMUCR
	mov	#(SH4_MMUCR_TI), r1
	mov.l	r1, @r0			/* MMUCR.TI = 1 */
	/* then, enable SE mode */
	MOV_CONST32_R0 SH4_PASCR_SE
	mov	r0, r1
	MOV_CONST32_R0 SH4_CCN_PASCR
	mov.l	r1, @r0			/* PASCR.SE = 1 */
#endif	/* CONFIG_CPU_SUBTYPE_SH4_2XX */
	/* ensure MMU coherency, by issuing an RTE instruction */
	/* this idiom works for all SH4-x00 series cores */
	mova	mmu_coherent, r0
	ldc	r0, spc		/* SPC = address of "mmu_coherent" */
	stc	sr, r0
	ldc	r0, ssr		/* SSR = SR */
	rte			/* do it */
	  nop
.balign 4
mmu_coherent:
#endif	/* CONFIG_ST40_SE_MODE */


	/*
	 * Support for Linux Hibernation-on-Memory (HoM).
	 *
	 * If the 3 long-words at CONFIG_HOM_TAG_VIRTUAL_ADDRESS
	 * are "frozen_linux", then there is a valid linux kernel
	 * and HoM environment to restore. The 4th long-word should
	 * contain the address of stm_defrost_kernel() in vmlinux.
	 */
#if defined(CONFIG_HIBERNATION_ON_MEMORY)

#if !defined(CONFIG_HOM_TAG_VIRTUAL_ADDRESS)
#error Please define CONFIG_HOM_TAG_VIRTUAL_ADDRESS to use HoM.
#endif
#define LINUX_HOM_MARKER_0		0x7a6f7266 /* froz */
#define LINUX_HOM_MARKER_1		0x6c5f6e65 /* en_l */
#define LINUX_HOM_MARKER_2		0x78756e69 /* inux */

hom_check_frozen_linux_marker:
	MOV_CONST32_R0	CONFIG_HOM_TAG_VIRTUAL_ADDRESS
	mov	r0, r1		! start of marker (we hope)

	MOV_CONST32_R0	LINUX_HOM_MARKER_0
	mov.l	@r1, r2
	cmp/eq	r0, r2		! check the first marker
	bf	hom_no_frozen_linux_marker

	MOV_CONST32_R0	LINUX_HOM_MARKER_1
	mov.l	@(4, r1), r2
	cmp/eq	r0, r2		! check the second marker
	bf	hom_no_frozen_linux_marker

	MOV_CONST32_R0	LINUX_HOM_MARKER_2
	mov.l	@(8, r1), r2
	cmp/eq	r0, r2		! check the third marker
	bf	hom_no_frozen_linux_marker

	! if we get here, then the "frozen_linux" marker was found
hom_got_frozen_linux_marker:
	mov	#-1, r0		! r0 = 0xFFFFFFFF
	mov.l	r0, @r1		! over-write the marker with all ones.
	mov.l	r0, @(4, r1)
	mov.l	r0, @(8, r1)
	mov.l	@(12, r1), r8	! r8 = stm_defrost_kernel()
	ocbp	@r1		! write-back the (now dirty) cache-line

#if defined(CONFIG_ST40_SE_MODE)
	/*
	 * explicitly invalidate all PMB entries, except for PMB[0],
	 * and possibly PMB[1].
	 */
hom_invalidate_pmb:
	MOV_CONST32_R0	(P4SEG_PMB_ADDR)
	mov	r0, r1
	mov	#0, r2			! PMB[n].V = 0, i.e. INVALID
	mov	#1, r3
	shll8	r3			! R3 = address stride is 0x100
#if CONFIG_SYS_ST40_LMI_NEEDS_2_PMB_ENTRIES
	add	r3, r1			! skip first PMB slot (PMB[0])
	mov	#2, r0			! R0 = counter (indexes: 2..15)
#else
	mov	#1, r0			! R0 = counter (indexes: 1..15)
#endif
1:	add	r3, r1			! next PMB slot
	mov.l	r2, @r1			! invalidate current entry
	cmp/eq	#15, r0			! finished ?
	bf/s	1b			! more to do ?
	  add	#1, r0			! next index counter

	/*
	 * we need to ensure that the ITLB is flushed, and not
	 * harbouring any mappings from the recently invalidated
	 * PMB entries.
	 */
hom_invalidate_itlb:
	MOV_CONST32_R0 SH4_CCN_MMUCR
	mov	#(SH4_MMUCR_TI), r1
	mov.l	r1, @r0			/* MMUCR.TI = 1 */
#endif	/* CONFIG_ST40_SE_MODE */

	/*
	 * re-invalidate the caches: both I$ & D$
	 */
hom_invalidate_caches:
	MOV_CONST16_R0 (SH4_CCR_OCI|SH4_CCR_ICI)
	mov	r0, a0
	CALL	sh_cache_set_op_offset

	/*
	 * Disable interrupts, and jump to stm_defrost_kernel()
	 */
hom_call_stm_defrost_kernel:
	mov	#0x3c, r2
	shll2	r2		! R2 = 0x000000F0
	stc	sr, r1
	or	r2, r1
	ldc	r1, sr		! set SR.IMASK = 15
	jmp	@r8		! Finally, jump to stm_defrost_kernel()
	  nop

hom_no_frozen_linux_marker:
				! no HoM kernel to restore
#endif	/* CONFIG_HIBERNATION_ON_MEMORY */


	/*
	 * now relocate "u-boot.bin" (from FLASH to RAM):
	 *	r0 == scratch/temp
	 *	r1 == source address (SRC)
	 *	r2 == destination address (DST)
	 *	r3 == destination END address
	 *	r4 == &skip_bb.array[x] (x=block number: 0,1,2,...)
	 *	r5 == skip_bb.block_size	(CONFIG_SYS_NAND_SKIP_BLOCK_SIZE)
	 *	r6 == skip_bb.block_size - 1	(block mask)
	 *
	 *	NOTE:	r4, r5 & r6 are only used if the the macro
	 *		CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING is defined.
	 *
	 *	QQQ: in 32-bit mode, "DST" is UN-cached - should optimize this.
	 */
relocate:
	mov	#0x1f, r0
	not	r0, r0			/* mask = 0xffffffe0 */
	mov	r9, r1			/* where we actually ARE */
	and	r0, r1			/* ensure source is 32-byte cache aligned */
	mov	r10, r2			/* were we WANT to be: START off */
	and	r0, r2			/* ensure destination is 32-byte cache aligned */
	mov	r11, r3			/* were we WANT to be: END off */
#if defined(CONFIG_SYS_BOOT_FROM_NAND) && defined(CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING)
	mov.l	skip_bb_offset, r4	/* r4 = &skip_bb - _start */
	add	r9, r4			/* r4 = &skip_bb (P0 (cached)) */
	mov.l	@(8,r4), r5		/* r5 = NAND block_size */
	mov	r5, r6
	add	#-1, r6			/* r6 = block_size - 1 (i.e. mask) */
	add	#16, r4			/* r4 = &array[0]  */
#endif	/* CONFIG_SYS_BOOT_FROM_NAND && CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING */
1:	mov.l	@(0,r1), r0		/* read & write a cache line at a time */
	mov.l	r0, @(0,r2)
	mov.l	@(4,r1), r0
	mov.l	r0, @(4,r2)
	mov.l	@(8,r1), r0
	mov.l	r0, @(8,r2)
	mov.l	@(12,r1), r0
	mov.l	r0, @(12,r2)
	mov.l	@(16,r1), r0
	mov.l	r0, @(16,r2)
	mov.l	@(20,r1), r0
	mov.l	r0, @(20,r2)
	mov.l	@(24,r1), r0
	mov.l	r0, @(24,r2)
	mov.l	@(28,r1), r0
	mov.l	r0, @(28,r2)
	ocbp	@r2			/* flush one line */
	add	#32, r1			/* next source line */
#if defined(CONFIG_SYS_BOOT_FROM_NAND) && defined(CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING)
	mov	r1, r0			/* test for a new block */
	and	r6, r0			/* r0 = SRC & (block_size - 1) */
	cmp/eq	#0, r0			/* is it a new block ? */
	bf	do_next_line		/* branch if it is the same block */
do_next_block:				/* okay, we are on a new NAND block */
	add	#4, r4
	mov.l	@r4, r0			/* r0 = array[++x] */
	cmp/eq	#0, r0			/* is it a GOOD block ? */
	bt	do_next_line		/* branch if it is a GOOD block */
					/* okay, we have a BAD block, try NEXT */
	bra	do_next_block		/* check it is good as well! */
	  add	r5, r1			/* SRC += block_size */
do_next_line:
#endif	/* CONFIG_SYS_BOOT_FROM_NAND && CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING */
	pref	@r1			/* pre-fetch next line */
	add	#32, r2			/* next line */
	cmp/hi	r2, r3			/* while (r2<=r3) ... */
	bt	1b


	/*
	 * now jump to absolute (non-PIC) address environment.
	 */
do_hyperspace:
	mov.l	hyperspace_addr, r1
	jmp	@r1		/* never returns */
	  nop

hyperspace:

	/*
	 * At this point:
	 *	We can stop using PIC, and use absolute code.
	 *	The EMI/LMI initialization has been completed.
	 */

#ifdef CONFIG_ST40_SE_MODE
do_enable_pmb_cache:
	/* Enable the cacheability for PMB array #0 */
	CALL sh_toggle_pmb_cacheability_offset
#endif	/* CONFIG_ST40_SE_MODE */

	/* init stack pointer */
init_sp:
	mov.l	stack_addr, sp

	/* Clear the bss */
clear_bss:
	mov.l	bss_start, r1
	add	#4, r1
	mov.l	bss_end, r2
	mov	#0, r0
1:	cmp/hs	r2, r1
	bf/s	1b			/* while (r1 < r2) */
	  mov.l	r0, @-r2
done_bss:

#if defined(CONFIG_SYS_USE_VMA_DATA)
	/* Copy ".data" (and ".u_boot_cmd") from LMA to VMA (RAM to RAM) */
copy_data:
	mov.l	data_vma_start, r1
	mov.l	data_lma_start, r3
	cmp/eq	r1, r3			/* VMA == LMA ? */
	bt done_data			/* do not copy data, if == */
	mov.l	data_vma_end,   r2
1:	mov.l	@r3+, r0		/* read from LMA */
	mov.l	r0, @r1			/* write to VMA */
	cmp/hs	r2, r1
	bf/s	1b			/* while (r1 < r2) */
	  add	#4, r1
done_data:
#endif	/* CONFIG_SYS_USE_VMA_DATA */

	/* prepare to call board init routine: start_st40_boot() */
do_start_st40_boot:
	mov.l	start_st40_boot_addr, r1
	jmp	@r1			/* never returns */
	  mov r8, a0


	/* Constants used above */

.balign 4
_start_offset:	.long . - _start
target_addr:	.long TEXT_BASE
target_end:	.long TEXT_BASE + __uboot_bin_size
#if defined(CONFIG_SYS_USE_VMA_DATA)
stack_addr:	.long __USE_VMA_DATA	/* start of the stack, just below relocated ".data" */
#else
stack_addr:	.long TEXT_BASE - (CONFIG_SYS_GBL_DATA_SIZE + CONFIG_SYS_MALLOC_LEN)
#endif	/* CONFIG_SYS_USE_VMA_DATA */
bss_start:	.long __bss_start
bss_end:	.long __bss_end
hyperspace_addr:.long hyperspace
#if defined(CONFIG_SYS_BOOT_FROM_NAND) && defined(CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING)
skip_bb_offset:	.long skip_bb - _start
#endif	/* CONFIG_SYS_BOOT_FROM_NAND && CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING */

start_st40_boot_addr:.long start_st40_boot

#if defined(CONFIG_SYS_USE_VMA_DATA)
data_lma_start:	.long __data_lma_start
data_vma_start:	.long __data_vma_start
data_vma_end:	.long __data_vma_end - 4
#endif	/* CONFIG_SYS_USE_VMA_DATA */

	/* offsets for PIC calls */

init_ram_offset:		.long init_ram - .
sh_cache_clear_op_offset:	.long sh_cache_clear_op - .
sh_cache_set_op_offset:		.long sh_cache_set_op - .
#ifdef CONFIG_ST40_SE_MODE
sh_toggle_pmb_cacheability_offset:
				.long sh_toggle_pmb_cacheability - .
#endif	/* CONFIG_ST40_SE_MODE */
