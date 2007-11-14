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

	/* Disable the watchdog timer */
	MOV_CONST32_R0 ST40_CPG_WTCSR
	mov	r0, r1
	MOV_CONST16_R0 0xA500
	mov.w   r0, @r1

	/* disable Caches */
	MOV_CONST16_R0 (SH4_CCR_OCE|SH4_CCR_ICE|SH4_CCR_EMODE)	/* QQQ: use -1 instead ? */
	mov	r0, a0
	CALL	sh_cache_clear_op_offset

	/* invalidate+enable the caches: both I$ & D$ (with write-through) */
	MOV_CONST16_R0 (SH4_CCR_OCI|SH4_CCR_ICI|SH4_CCR_ICE|SH4_CCR_OCE|SH4_CCR_EMODE|SH4_CCR_WT)
	mov	r0, a0
	CALL	sh_cache_set_op_offset

	/*
	 * are we in the correct location ? (i.e. RAM)
	 *  r8 == 1 in RAM, assume initialization already done by GDB.
	 *  r8 == 0 in FLASH, need to relocate, set up memory, etc...
	 *  r9 == where we actualy ARE (PIC relocate source).
	 * r10 == were we WANT to be   (PIC relocate target).
	 */
#ifndef CONFIG_SH_SE_MODE
	MOV_CONST32_R0 0X20000000	/* set up P2 mask  */
	mov	r0, r4
#endif	/* CONFIG_SH_SE_MODE */
	mova	_start_offset, r0	/* Load source address in r9 */
	mov	r0, r9			/* i.e. where we actualy ARE */
#ifndef CONFIG_SH_SE_MODE
	or	r4, r9			/* make sure its P2 */
#endif	/* CONFIG_SH_SE_MODE */
	mov.l	@r9, r10
	sub	r10, r9
	mov.l	target_addr, r10	/* Load target address in r10 */
					/* i.e. were we WANT to be */
#ifndef CONFIG_SH_SE_MODE
	or	r4, r10			/* make sure its P2 */
#endif	/* CONFIG_SH_SE_MODE */
	cmp/eq	r9, r10			/* Are we in correct place already ? */
	movt	r8			/* save SR.T in r8. */

	/* set up the PMB entries we want to use */
#ifdef CONFIG_SH_SE_MODE
	/* QQQ: we need to explicitly invalidate all unused entires */
	/* set up the new PMB entries we want to use */
	SH4_SET_PMB 0 0x80 0x40 128 0 0 1	/* LMI0  UN-cached */
	SH4_SET_PMB 1 0x90 0x40 128 0 0 1	/* LMI0  UN-cached */
	SH4_SET_PMB 2 0xb0 0x00  16 0 0 1	/* FLASH UN-cached */
	SH4_SET_PMB 3 0xb1 0x01  16 0 0 1	/* FLASH UN-cached */
	SH4_SET_PMB 4 0xb5 0x05  16 0 0 1	/* EPLD  UN-cached */
#endif	/* CONFIG_SH_SE_MODE */

	/*
	 * initialize the memory controllers, if we need to.
	 */
	cmp/pl	r8			/* Are we in correct place already ? */
	bt	done_memory_init
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
	 * relocate code, if we need to.
	 */
	mov	r9, r1			/* where we actualy ARE */
	mov	r10, r2			/* were we WANT to be */
	/* relocate code: r1=source, r2=dest, r3=dest_end */
	mov.l	bss_start, r3
#ifndef CONFIG_SH_SE_MODE
	MOV_CONST32_R0 0X20000000	/* set up P2 mask  */
	or	r0, r3			/* make sure its P2 */
#endif	/* CONFIG_SH_SE_MODE */
1:	mov.w	@r1+, r0
	mov.w	r0, @r2
	add	#2, r2
	cmp/hi	r2, r3
	bt	1b

	/*
	 * now jump to absolute (non-PIC) address.
	 */
	mov.l	hyperspace_addr, r1
	jmp	@r1		/* never returns */
	  nop
hyperspace:

	/* Put the caches in Copy-Back (and not Write-Through) */
done_memory_init:
	MOV_CONST16_R0 (SH4_CCR_WT)
	mov	r0, a0
	CALL	sh_cache_clear_op_offset

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
	bf/s	1b		! while (r1 < r2)
	  mov.l	r0, @-r2

	/* prepare to call board init routine: start_sh4boot() */
	mov.l	start_sh4boot_addr, r1
	jmp	@r1		/* never returns */
	  nop

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
