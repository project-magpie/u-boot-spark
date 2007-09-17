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


.macro MOV_CONST32_R0 p1
	mov #(\p1>>24), r0
	shll8 r0
	or #((\p1 >> 16)&0xFF), r0
	shll8 r0
	or #((\p1 >> 8) &0xFF), r0
	shll8 r0
	or #(\p1 &0xFF), r0
.endm

.macro MOV_CONST16_R0 p1
	mov #((\p1 >> 8) &0xFF), r0
	shll8 r0
	or #(\p1 &0xFF), r0
.endm

.macro ENTER_P2	/* or 0x20 into top byte of address */
	mova 1f, r0
	mov r0, r1
	mov #0x20, r0
	shll16 r0
	shll8 r0
	or r0, r1
	jmp @r1
1:
	.endm

.macro CALL p1	/* call a routine in another file */
	mova \p1, r0
	mov.l @ r0, r1
	add r1, r0
	jsr @r0
	nop
.endm

.macro GETDEVID p1	/* put device id in p1 */
#ifdef CONFIG_SH_STB7100
	mov.l	1f,  r0
	mov.l	@r0, \p1
	mov.l   2f, r0
	bra 3f
	  and     r0, \p1
	.balign 4
1:	.long STB7100_SYSCONF_DEVICEID_0
2:      .long (STB7100_DEVID_ID_MASK << STB7100_DEVID_ID_SHIFT) | (STB7100_DEVID_CUT_MASK << STB7100_DEVID_CUT_SHIFT)
3:
#endif
.endm
