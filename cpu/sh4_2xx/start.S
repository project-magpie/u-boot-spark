/*
 * (C) Copyright 2004 STMicroelectronics.
 *
 * Andy Sturges <andy.sturges@st.com>
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
#include "asm/asmdefs.h"

#if defined(CONFIG_SH_STB7100)
#include <asm/stb7100reg.h>
#elif defined(CONFIG_SH_STX7200)
#include <asm/stx7200reg.h>
#else
#error Missing Device Defintions!
#endif


	.global _start
_start:

	/* Set SR:	MD=1, RB=0, BL=0, FD=0, IMASK=0xF */
	MOV_CONST32_R0 0x400000F0
	ldc	r0, sr

	/* Set FPSCR:	FR=0, SZ=0, PR=1, DN=0, RM=01 */
	MOV_CONST32_R0 0x00080001
	lds	r0, fpscr

	/* disable the watchdog timer */
	MOV_CONST32_R0 ST40_CPG_WTCSR
	mov	r0, r1
	MOV_CONST16_R0 0xA500
	mov.w   r0, @r1

	/*
	 * Determine if we are in the correct location ? (i.e. RAM)
	 *  r8 == 1 in RAM, assume initialization already done by GDB.
	 *  r8 == 0 in FLASH, need to relocate, set up memory, etc...
	 *  r9 == where we actualy ARE       (PIC relocate source).
	 * r10 == were we WANT to be [BEGIN] (PIC relocate target).
	 * r11 == were we WANT to be [END]   (PIC relocate target).
	 *
	 * NOTE: when CONFIG_SH_SE_MODE is defined, then also:
	 *  r8 == 1 assume we are already in 32-bit SE mode (done by GDB).
	 *  r8 == 0 we are in 29-bit mode, and need to switch to 32-bit mode.
	 */
	MOV_CONST32_R0 0x1fffffff	/* set up P0 mask  */
	mov	r0, r4
	mova	_start_offset, r0	/* Load source address in r9 */
	mov	r0, r9			/* i.e. where we actualy ARE */
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
	MOV_CONST32_R0 (PMB_ADDR_ARRAY)
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
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI0  UN-cached */
	SH4_SET_PMB 1 0x90 0x40 128 0 0 1	/* LMI0  UN-cached */
	SH4_SET_PMB 2 0xb0 0x00  16 0 0 1	/* FLASH UN-cached */
	SH4_SET_PMB 3 0xb1 0x01  16 0 0 1	/* FLASH UN-cached */
	SH4_SET_PMB 4 0xb5 0x05  16 0 0 1	/* EPLD  UN-cached */
#endif	/* CONFIG_SH_SE_MODE */

	cmp/pl	r8			/* Are we in correct place already ? */
	bt	hyperspace

	/* disable the caches */
	mov	#-1, a0		/* clear all bits in the CCR */
	CALL	sh_cache_clear_op_offset

	/* invalidate+enable the caches: both I$ & D$ (with copy-back) */
	MOV_CONST16_R0 (SH4_CCR_OCI|SH4_CCR_ICI|SH4_CCR_ICE|SH4_CCR_OCE|SH4_CCR_EMODE)
	mov	r0, a0
	CALL	sh_cache_set_op_offset

	/*
	 * switch to P0 region (cachable), if we need to.
	 * Recall, we are still in 29-bit mode at this juncture.
	 */
	ENTER_P0

	/*
	 * initialize the memory controllers, if we need to.
	 */
	CALL	init_ram_offset		/* init_ram() */

	/*
	 * enable SE (32-bit) mode, if we need to.
	 */
#ifdef CONFIG_SH_SE_MODE
	/* enable SE mode */
	/* QQQ: for SH4-300, we will need different code here!  */
	MOV_CONST32_R0 SH4_CCN_MMUCR
	mov	#(SH4_MMUCR_TI|SH4_MMUCR_SE), r1
	mov.l	r1, @r0
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
#endif	/* CONFIG_SH_SE_MODE */

	/*
	 * relocate code: r1=source, r2=dest, r3=dest_end
	 */
	mov	#0x1f, r0
	not	r0, r0			/* mask = 0xffffffe0 */
	mov	r9, r1			/* where we actualy ARE */
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
	pref	@r1			/* pre-featch next line */
	add	#32, r2			/* next line */
	cmp/hi	r2, r3			/* while (r2<=r3) ... */
	bt	1b

	/*
	 * now jump to absolute (non-PIC) address environent.
	 */
	mov.l	hyperspace_addr, r1
	jmp	@r1		/* never returns */
	  nop

hyperspace:

	/*
	 * At this point:
	 *	We can stop using PIC, and use absolute code.
	 *	The EMI/LMI initialization has been complated.
	 */

#ifdef CONFIG_SH_SE_MODE
	/* Enable the cacheability for PMB array #0 */
	CALL sh_toggle_pmb_cacheability_offset
#endif	/* CONFIG_SH_SE_MODE */

	/* init stack pointer */
	mov.l	stack_addr, sp

	/* Clear the bss */
	mov.l	bss_start, r1
	add	#4, r1
	mov.l	bss_end, r2
	mov	#0, r0
1:	cmp/hs	r2, r1
	bf/s	1b			/* while (r1 < r2) */
	  mov.l	r0, @-r2

	/* prepare to call board init routine: start_sh4boot() */
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
