/*
 * (C) Copyright 2004-2009 STMicroelectronics.
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
#include "asm/socregs.h"


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

	/* Set FPSCR:	FR=0, SZ=0, PR=1, DN=0, RM=01 */
set_fpscr:
	MOV_CONST32_R0 0x00080001
	lds	r0, fpscr

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
#ifdef CFG_BOOT_FROM_NAND
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
	bytes 0x00 0x3f			/* 64-bytes: 0x00, 0x01, ..., 0x3f */

skipped_signature:
					/* resume from here */
#endif	/* CFG_BOOT_FROM_NAND */


	/*
	 * If were are going to boot U-boot from SPI (serial flash),
	 * then the following pokes are required/recommended to be
	 * done early, when using the EMI's SPIBOOT mode controller.
	 */
#ifdef CFG_BOOT_FROM_SPI
config_spiboot_controller:
	MOV_CONST32_R0 ST40_EMI_SPI_REGS_BASE
	mov	r0, r1
	/* Slow (divide) the SPI clock down to improve reliability. */
	mov	#4, r0			/* *ST40_EMI_SPI_CLOCKDIV = 4 */
	mov.l	r0, @(ST40_EMI_SPI_CLOCKDIV-ST40_EMI_SPI_REGS_BASE,r1)
#if defined(CONFIG_SPI_FLASH_ST) || defined(CONFIG_SPI_FLASH_MXIC)
	MOV_CONST32_R0 0x000200a1	/* *ST40_EMI_SPI_CONFIGDATA = 0x000200a0|ST */
	mov.l	r0, @(ST40_EMI_SPI_CONFIGDATA-ST40_EMI_SPI_REGS_BASE,r1)
	/* For ST serial flash we use CONTIG_MODE and FAST_READ */
	mov	#3, r0			/* *ST40_EMI_SPI_MODESELECT = CONTIG_MODE|FAST_READ */
	mov.l	r0, @(ST40_EMI_SPI_MODESELECT-ST40_EMI_SPI_REGS_BASE,r1)
#elif defined(CONFIG_SPI_FLASH_ATMEL)
	MOV_CONST32_R0 0x000200a4	/* *ST40_EMI_SPI_CONFIGDATA = 0x000200a0|ATMEL */
	mov.l	r0, @(ST40_EMI_SPI_CONFIGDATA-ST40_EMI_SPI_REGS_BASE,r1)
	/* For ATMEL serial flash we use OLD_MODE and Normal (read) mode */
	mov	#0, r0			/* *ST40_EMI_SPI_MODESELECT = 0 */
	mov.l	r0, @(ST40_EMI_SPI_MODESELECT-ST40_EMI_SPI_REGS_BASE,r1)
#else
#warning Not configuring SPI_MODESELECT or SPI_CONFIGDATA registers!
#endif	/* if defined(CONFIG_SPI_FLASH_xxx) */
#endif	/* CFG_BOOT_FROM_SPI */


	/*
	 * Determine if we are in the correct location ? (i.e. RAM)
	 *  r8 == 1 in RAM, assume initialization already done by GDB.
	 *  r8 == 0 in FLASH, need to relocate, set up memory, etc...
	 *  r9 == where we actually ARE      (PIC relocate source).
	 * r10 == were we WANT to be [BEGIN] (PIC relocate target).
	 * r11 == were we WANT to be [END]   (PIC relocate target).
	 *
	 * NOTE: when CONFIG_SH_SE_MODE is defined, then also:
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
#ifndef CONFIG_SH_SE_MODE
	and	r4, r9			/* make sure its P0 (cached) */
#endif	/* CONFIG_SH_SE_MODE */
	mov.l	target_addr, r10	/* Load target address in r10 */
					/* i.e. were we WANT to be */
#ifndef CONFIG_SH_SE_MODE
	and	r4, r10			/* make sure its P0 (cached) */
#endif	/* CONFIG_SH_SE_MODE */
	mov.l	bss_start, r11		/* load target END address in r11 */
#ifndef CONFIG_SH_SE_MODE
	and	r4, r11			/* make sure its P0 (cached) */
#endif	/* CONFIG_SH_SE_MODE */
	cmp/eq	r9, r10			/* Are we in correct place already ? */
	movt	r8			/* save SR.T in r8. */
#ifdef CONFIG_SH_SE_MODE
	and	r4, r9			/* make sure its P0 (cached) */
#endif	/* CONFIG_SH_SE_MODE */

	/* set up the PMB entries we want to use */
#ifdef CONFIG_SH_SE_MODE
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

	/* set up the new PMB entries we want to use */
set_pmb:
#if defined(CONFIG_SH_STB7100)
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI-Sys UN-cached */
	SH4_SET_PMB 2 0x90 0x40 128 0 0 1	/* LMI-Sys UN-cached */
	#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	#error Need to configure PMBs properly when more than 128MiB.
	#endif
	SH4_SET_PMB 4 0xa0 0x00  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5 0xa8 0x00  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 6 0xb2 0x02  16 0 0 1	/* OFF-chip  Peripherals UN-cached */
	SH4_SET_PMB 7 0xb8 0x18  64 0 0 1	/* ON-chip   Peripherals UN-cached */
	SH4_SET_PMB 8 0xbc 0x1c  64 0 0 1	/* ST40 core Peripherals UN-cached */
#elif defined(CONFIG_SH_STX5197) && defined(CONFIG_SH_MB704)
	SH4_SET_PMB 0 0x80 0x40  64 0 0 1	/* LMI-Sys UN-cached */
	SH4_SET_PMB 2 0x90 0x40  64 0 0 1	/* LMI-Sys UN-cached */
	SH4_SET_PMB 4 0xa0 0x00 128 0 0 1	/* SPI Flash (in SPIBOOT mode) UN-cached */
#elif defined(CONFIG_SH_STX5197) && defined(CONFIG_SH_5197CAB)
	SH4_SET_PMB 0 0x80 0x40  64 0 0 1	/* LMI-Sys UN-cached */
	SH4_SET_PMB 2 0x90 0x40  64 0 0 1	/* LMI-Sys UN-cached */
	SH4_SET_PMB 4 0xa0 0x00 128 0 0 1	/* SPI Flash (in SPIBOOT mode) UN-cached */
#elif defined(CONFIG_SH_STX7105) && defined(CONFIG_SH_MB680)
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI0 (first-half)  UN-cached */
	SH4_SET_PMB 2 0x90 0x40 128 0 0 1	/* LMI0 (first-half)  UN-cached */
	#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	SH4_SET_PMB 1 0x88 0x48 128 0 0 1	/* LMI0 (second-half) UN-cached */
	SH4_SET_PMB 3 0x98 0x48 128 0 0 1	/* LMI0 (second-half) UN-cached */
	#endif
	#ifdef CFG_BOOT_FROM_NAND /* booting from NAND, so CSA and CSB swapped in EPLD */
	SH4_SET_PMB 4 0xa0 0x00  16 0 0 1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 5 0xa4 0x04  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6 0xa5 0x05  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 7 0xa8 0x04  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8 0xa9 0x05  16		/* NOR FLASH CACHED */
	#else	/* else, CSA and CSB are not swapped in EPLD */
	SH4_SET_PMB 4 0xa0 0x00  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5 0xa1 0x01  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6 0xa4 0x04  16 0 0 1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 7 0xa8 0x00  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8 0xa9 0x01  16		/* NOR FLASH CACHED */
	#endif	/* CFG_BOOT_FROM_NAND */
	SH4_SET_PMB 9 0xb7 0x07  16 0 0 1	/* EPLD UN-cached */
#elif defined(CONFIG_SH_STX7105) && defined(CONFIG_SH_PDK7105)
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI0 (first-half)  UN-cached */
	SH4_SET_PMB 2 0x90 0x40 128 0 0 1	/* LMI0 (first-half)  UN-cached */
	#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	SH4_SET_PMB 1 0x88 0x48 128 0 0 1	/* LMI0 (second-half) UN-cached */
	SH4_SET_PMB 3 0x98 0x48 128 0 0 1	/* LMI0 (second-half) UN-cached */
	#endif
	SH4_SET_PMB  4 0xa0 0x00  64 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB  5 0xa4 0x04  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB  6 0xa5 0x05  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB  7 0xa6 0x06  16 0 0 1	/* NAND + CI UN-cached */
	SH4_SET_PMB  8 0xa7 0x07  16 0 0 1	/* PCI UN-cached */
	SH4_SET_PMB  9 0xa8 0x00  64		/* NOR FLASH CACHED */
	SH4_SET_PMB 10 0xac 0x04  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 11 0xad 0x05  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 12 0xae 0x06  16		/* NAND + CI CACHED */
	SH4_SET_PMB 13 0xaf 0x07  16		/* PCI CACHED */
#elif defined(CONFIG_SH_STX7111) && defined(CONFIG_SH_MB618)
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI UN-cached */
	SH4_SET_PMB 2 0x90 0x40 128 0 0 1	/* LMI UN-cached */
	#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	#error Need to configure PMBs properly when more than 128MiB.
	#endif
	SH4_SET_PMB 4 0xa0 0x00  16 0 0 1	/* NOR/NAND FLASH UN-cached */
	SH4_SET_PMB 5 0xa1 0x01  16 0 0 1	/* NOR/NAND FLASH UN-cached */
	SH4_SET_PMB 6 0xa8 0x00  16		/* NOR/NAND FLASH CACHED */
	SH4_SET_PMB 7 0xa9 0x01  16		/* NOR/NAND FLASH CACHED */
#ifndef CONFIG_SH_NO_EPLD
	SH4_SET_PMB 8 0xb6 0x06  16 0 0 1	/* EPLD UN-cached */
#endif	/* CONFIG_SH_NO_EPLD */
#elif defined(CONFIG_SH_STX7141) && defined(CONFIG_SH_MB628)
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI UN-cached */
	SH4_SET_PMB 2 0x90 0x40 128 0 0 1	/* LMI UN-cached */
	#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	#error Need to configure PMBs properly when more than 128MiB.
	#endif
	SH4_SET_PMB 4 0xa0 0x00  16 0 0 1	/* NOR/NAND FLASH UN-cached */
	SH4_SET_PMB 5 0xa1 0x01  16 0 0 1	/* NOR/NAND FLASH UN-cached */
	SH4_SET_PMB 6 0xa8 0x00  16		/* NOR/NAND FLASH CACHED */
	SH4_SET_PMB 7 0xa9 0x01  16		/* NOR/NAND FLASH CACHED */
	SH4_SET_PMB 8 0xb5 0x05  16 0 0 1	/* EPLD UN-cached */
#elif defined(CONFIG_SH_STX7200) && defined(CONFIG_SH_MB519)
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI0 UN-cached */
	SH4_SET_PMB 2 0x90 0x40 128 0 0 1	/* LMI0 UN-cached */
	#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	#error Need to configure PMBs properly when more than 128MiB.
	#endif
	SH4_SET_PMB 4 0xa0 0x00  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5 0xa1 0x01  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6 0xa8 0x00  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 7 0xa9 0x01  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8 0xb5 0x05  16 0 0 1	/* EPLD UN-cached */
#elif defined(CONFIG_SH_STX7200) && defined(CONFIG_SH_CB101)
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI0 UN-cached */
	SH4_SET_PMB 2 0x90 0x40 128 0 0 1	/* LMI0 UN-cached */
	#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	#error Need to configure PMBs properly when more than 128MiB.
	#endif
	SH4_SET_PMB 4 0xa0 0x00  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5 0xa1 0x01  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6 0xa8 0x00  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 7 0xa9 0x01  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8 0xa2 0x02  16 0 0 1	/* NAND FLASH UN-cached */
	SH4_SET_PMB 9 0xa3 0x03  16 0 0 1	/* NAND FLASH UN-cached */
#elif defined(CONFIG_SH_STX7200) && defined(CONFIG_SH_MB671)
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI0 (first-half)  UN-cached */
	SH4_SET_PMB 2 0x90 0x40 128 0 0 1	/* LMI0 (first-half)  UN-cached */
	#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	SH4_SET_PMB 1 0x88 0x48 128 0 0 1	/* LMI0 (second-half) UN-cached */
	SH4_SET_PMB 3 0x98 0x48 128 0 0 1	/* LMI0 (second-half) UN-cached */
	#endif
	SH4_SET_PMB 4 0xa0 0x00  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5 0xa1 0x01  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6 0xa8 0x00  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 7 0xa9 0x01  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 8 0xb5 0x05  16 0 0 1	/* EPLD UN-cached */
#elif defined(CONFIG_SH_STX7200) && defined(CONFIG_SH_CB102)
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI0 (first-half)  UN-cached */
	SH4_SET_PMB 2 0x90 0x40 128 0 0 1	/* LMI0 (first-half)  UN-cached */
	#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	SH4_SET_PMB 1 0x88 0x48 128 0 0 1	/* LMI0 (second-half) UN-cached */
	SH4_SET_PMB 3 0x98 0x48 128 0 0 1	/* LMI0 (second-half) UN-cached */
	#endif
	SH4_SET_PMB 4 0xa0 0x00  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 5 0xa1 0x01  16 0 0 1	/* NOR FLASH UN-cached */
	SH4_SET_PMB 6 0xa8 0x00  16		/* NOR FLASH CACHED */
	SH4_SET_PMB 7 0xa9 0x01  16		/* NOR FLASH CACHED */
#else
	#error Do not know which board/chip we are using for PMB setup.
#endif	/* defined(CONFIG_SH_STB7100) */
#endif	/* CONFIG_SH_SE_MODE */

concider_hyperspace:
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
#ifdef CONFIG_SH_SE_MODE
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
	mova	relocate, r0
	ldc	r0, spc		/* SPC = address of relocate */
	stc	sr, r0
	ldc	r0, ssr		/* SSR = SR */
	rte			/* do it */
	  nop
.balign 4
#endif	/* CONFIG_SH_SE_MODE */

	/*
	 * relocate code: r1=source, r2=dest, r3=dest_end
	 * QQQ: in 32-bit mode, "dest" is UN-cached - should optimize this.
	 */
relocate:
	mov	#0x1f, r0
	not	r0, r0			/* mask = 0xffffffe0 */
	mov	r9, r1			/* where we actually ARE */
	and	r0, r1			/* ensure source is 32-byte cache aligned */
	mov	r10, r2			/* were we WANT to be: START off */
	and	r0, r2			/* ensure destination is 32-byte cache aligned */
	mov	r11, r3			/* were we WANT to be: END off */
1:	mov.l	@(0,r1), r0		/* read & write a line at a time */
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
	add	#32, r1			/* next line */
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

#ifdef CONFIG_SH_SE_MODE
do_enable_pmb_cache:
	/* Enable the cacheability for PMB array #0 */
	CALL sh_toggle_pmb_cacheability_offset
#endif	/* CONFIG_SH_SE_MODE */

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

	/* prepare to call board init routine: start_sh4boot() */
do_start_sh4boot:
	mov.l	start_sh4boot_addr, r1
	jmp	@r1			/* never returns */
	  mov r8, a0


	/* Constants used above */

.balign 4
_start_offset:	.long . - _start
target_addr:	.long TEXT_BASE
stack_addr:	.long TEXT_BASE - (CFG_GBL_DATA_SIZE + CFG_MALLOC_LEN + CFG_BOOTPARAMS_LEN)
bss_start:	.long __bss_start
bss_end:	.long __bss_end
hyperspace_addr:.long hyperspace

start_sh4boot_addr:.long start_sh4boot

	/* offsets for PIC calls */

init_ram_offset:		.long init_ram - .
sh_cache_clear_op_offset:	.long sh_cache_clear_op - .
sh_cache_set_op_offset:		.long sh_cache_set_op - .
#ifdef CONFIG_SH_SE_MODE
sh_toggle_pmb_cacheability_offset:
				.long sh_toggle_pmb_cacheability - .
#endif	/* CONFIG_SH_SE_MODE */


#if defined(CFG_ENV_IS_IN_EEPROM) && defined(CFG_BOOT_FROM_SPI)
	/*
	 * The following is a hack to work around a awkward linker
	 * dependency issue. We need to force "ld" to pull in the CPU
	 * specific version of "env_eeprom.o", rather than the generic
	 * "common/env_eeprom.o" file that the linker seems to prefer!
	 * QQQ: try to solve this properly, or make it non-loadable!
	 */
	.long env_init	/* any reference to anything in "env_eeprom.o" */
#endif /* CFG_ENV_IS_IN_EEPROM && CFG_BOOT_FROM_SPI */

