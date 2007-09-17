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
ENTRY(sh_cache_set_op)

	/*
	 * void sh_cache_set_op (ulong op)
	 * set bits in ccn.cr
	 */


	/* get ccn.cr address into r0 (0xff00001c) */
	ENTER_P2
	MOV_CONST32_R0	0xff00001c
	mov.l   @r0, r1
	or	a0,  r1
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
	MOV_CONST32_R0	0xff00001c
	mov.l	@r0, r1
	not	a0, a0 /* invert op */
	and	a0, r1 /* clear bits */
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
