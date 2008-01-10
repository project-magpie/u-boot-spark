/*
 * (C) Copyright 2005 STMicroelectronics.
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
#include <asm/ctrlregdef.h>

/*
 * The poke table is a series of long words, in the format
 *
 *       opcode, address, value1, (value2)
 *
 * An opcode of 0 marks the table end
 */

#define LOG2_LXTLB_MIN_PAGE_SIZE 0xd

#define WHILE_NE(A, AND, VAL)   .long 7, A, AND, VAL
#define UPDATE_LONG(A, AND, OR) .long 6, A, AND, OR
#define OR_LONG(A, V)           .long 5, A, V
#define POKE_LONG(A, V)         .long 4, A, V
#define POKE_SHORT(A, V)        .long 2, A, V
#define POKE_CHAR(A, V)         .long 1, A, V
#define END_MARKER              .long 0, 0, 0

	.text
	.balign 32
.global _memory_setup_table
_memory_setup_table:

#include "stm8010mboard_memory.S"

	END_MARKER

.global _memory_setup_table_end
_memory_setup_table_end:
.global _xpu_mmu_setup_table
_xpu_mmu_setup_table:

#include "stm8010mboard_mmu.S"

	END_MARKER

	.balign 8
/*
 * Routines to switch an LED on and off.
 */
_led_off:
	goto    1f
	  mov   $r0.24 = 0x1
	;;
_led_on:
	mov     $r0.24 = 0x0
	;;
1:      mov     $r0.25 = 0x00900000
	;;
	return  $r0.63
	  stb   (0)[$r0.25] = $r0.24
	;;


	.global led_blink
led_blink:

	/* Led_Blink the LED a few times */
	.global led_blink
led_blink:
	/* Save call address in SCRATCH1 */
	stw   SCRATCH1[$r0] = $r63
	;;
	mov   $r8 = 0x00900000
	;;
	shl  $r16 = $r16, 24
	;;
	mov   $r9 = 4
	mov   $r10 = 0
	;;
1:      sth	0[$r8] = $r10
	  xor	$r10 = $r10, 1
	  add	$r9 = $r9, -1
	  mov	$r62 = $r16
	;;
2:	add	$r62 = $r62, -1
	  cmpeq	$b0 = $r62, $r0
	;;
	brf	$b0, 2b
	;;
	cmpeq	$b0 = $r9, 0
	;;
	brf	$b0, 1b
	;;
	mov   $r10 = 0
	;;
	sth	0[$r8] = $r10
	return	$r63
	;;



