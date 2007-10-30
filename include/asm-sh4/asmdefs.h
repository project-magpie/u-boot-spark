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

/* Macros to support assembly programing */


	/* build up a 32-bit constant */
.macro MOV_CONST32_R0 p1:req
	mov	#(\p1>>24), r0
	shll8	r0
	.ifne    ((\p1 >> 16)&0xFF)
	or	#((\p1 >> 16)&0xFF), r0
	.endif
	shll8	r0
	.ifne    ((\p1 >> 8) &0xFF)
	or	#((\p1 >> 8) &0xFF), r0
	.endif
	shll8	r0
	.ifne    (\p1 &0xFF)
	or	#(\p1 &0xFF), r0
	.endif
.endm

	/* build up a 16-bit constant */
.macro MOV_CONST16_R0 p1:req
	mov	#((\p1 >> 8) &0xFF), r0
	shll8	r0
	.ifne    (\p1 &0xFF)
	or	#(\p1 &0xFF), r0
	.endif
.endm

	/* OR 0x20 into top byte of PC */
.macro ENTER_P2
#ifndef CONFIG_SH_SE_MODE
	mova	1f, r0
	mov	#0x20, r1
	shll16	r1
	shll8	r1
	or	r0, r1
	jmp	@r1
	  nop
.balign 4
1:
#endif
.endm

	/* call a routine in another file */
.macro CALL p1:req
	mova	\p1, r0
	mov.l	@r0, r1
	add	r1, r0
	jsr	@r0
	  nop
.endm

	/* put device id in p1 */
.macro GETDEVID p1:req
#ifdef CONFIG_SH_STB7100
	mov.l	1f, r0
	mov.l	@r0, \p1
	mov.l	2f, r0
	bra	3f
	  and	r0, \p1
	.balign 4
1:	.long STB7100_SYSCONF_DEVICEID_0
2:	.long (STB7100_DEVID_ID_MASK << STB7100_DEVID_ID_SHIFT) | (STB7100_DEVID_CUT_MASK << STB7100_DEVID_CUT_SHIFT)
3:
#endif
.endm
