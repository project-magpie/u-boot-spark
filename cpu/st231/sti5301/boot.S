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

#include "../macros.h"

/**********************************************************
*
*	Boot vector entry
*
**********************************************************/

	.section .bootvector, "ax"
	.global entry
	.import _start
	.balign 8
entry:
/* This loop is to enable the debugger time to grab the processor before us */
	mov   $r1 = (1 << 22)
	;;
1:	add	$r1 = $r1, -1
	  cmpeq	$b0 = $r1, $r0
	;;
	brf	$b0, 1b
	;;
	GET_PIC_PTR(_start, $r1)
	;;
	mov   $r63 = $r1
	;;
	goto  $r63
	;;
	goto entry /* Should never get here */
	;;



