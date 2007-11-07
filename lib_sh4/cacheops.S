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
#include "asm/hardware.h"
#include "asm/asmdefs.h"
#include "asm/linkage.h"

	.text

	/*
	 * void sh_cache_set_op (ulong op)
	 * set bits in ccn.cr
	 */
ENTRY(sh_cache_set_op)
	/* get ccn.cr address into r0 (0xff00001c) */
	ENTER_P2
	MOV_CONST32_R0	CCN_CCR
	mov.l	@r0, r1
	or	a0, r1
	mov.l	r1, @r0
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	rts
	  nop


	/*
	 * void sh_cache_clear_op (ulong op)
	 * clears bits in ccn.cr
	 */
ENTRY(sh_cache_clear_op)
	/* get ccn.cr address into r0 (0xff00001c) */
	ENTER_P2
	MOV_CONST32_R0	CCN_CCR
	mov.l	@r0, r1
	not	a0, a0	/* invert op */
	and	a0, r1	/* clear bits */
	mov.l	r1, @r0
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	rts
	  nop


#ifdef CONFIG_SH_SE_MODE
/* Move the PC from one PMB region to another, by masking.
Note: This macro will clobber r0.
Usage: .SWITCH_PMB <opcode> <mask_reg>
where	<opcode> should be the masking opcode: "and" or "or".
	<mask_reg> register name, containing the mask to use. */
.macro	SWITCH_PMB opcode:req, mask_reg:req
	mova	1f, r0
	\opcode	\mask_reg, r0
	ldc	r0, spc		/* SPC = PC op mask */
	stc	sr, r0
	ldc	r0, ssr		/* SSR = SR */
	rte			/* do it */
	  nop
.balign 4
1:
.endm

	/*
	 * void sh_toggle_pmb_cacheability (void)
	 *
	 * Toggles the cacheability for PMB #0
	 *
	 * We do this by changing the PC to run out of an
	 * alised PMB entry (not #0), which is UN-cached.
	 * From here we 'toggle' the cachability of PMB
	 * array entry #0, then invalidate the ITLB.
	 * Finally we switch back the PC to the region
	 * we were originaly running from.
	 *
	 * We only toggle the UB and the C flags of the PMB array.
	 *
	 * Assumptions:
	 * We assume that an un-cached PMB entry already exists
	 * which is an alias to the PMB entry we are currenty
	 * running in. We assume that the transition into
	 * the UN-cached one can be acevied by setting bit #28
	 * of the PC.  i.e. OR-ing it with 0x10000000.
	 */
ENTRY(sh_toggle_pmb_cacheability)
	/* use PMB entry array #0 */
	.set index, 0		/* PMB array #0 */

	/* set PC mask as bit #28 */
	.set mask, 0x10000000	/* 0x80000000 ^ 0x90000000 */

	/* switch PC, to run from an UN-cached PMB region */
	MOV_CONST32_R0 mask	/* Mask */
	mov	r0, r2
	SWITCH_PMB or, r2	/* PC = PC | 0x10000000 */

	/* now toggle the cacheability of indexed PMB entry */
	MOV_CONST32_R0	(PMB_C|PMB_UB)
	mov	r0, r3		/* PMB[n].C and PMB[n].UB bits */
	MOV_CONST32_R0	(PMB_DATA_ARRAY|(index<<8))
	mov.l	@r0, r1		/* read it */
	xor	r3, r1		/* xor it */
	mov.l	r1, @r0		/* write it */

	/* Invalidate the ITLB */
	MOV_CONST32_R0 CCN_MMUCR
	mov	#(MMUCR_TI|MMUCR_SE), r1
	mov.l	r1, @r0

	/* switch PC, to run from original PMB region */
	not	r2, r2		/* invert the mask */
	SWITCH_PMB and, r2	/* PC = PC & !0x10000000 */

	/* return to the caller */
	rts
	  nop
#endif	/* CONFIG_SH_SE_MODE */

