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
 *  The 8010 has 3 processors host, audio and VES
 *  bootstrap addresses are 0x00000000, 0x00000040, 0x00000100
 *  The host CPU must boot into u-boot, the others should spin waiting
 *  for an interrupt telling them to boot from the address held in the
 *  mbox register
 *
 */

#include "../macros.h"

/**********************************************************
*
*	Boot vector entry
*
**********************************************************/

	.text
	.balign 8
	.global entry
	.import _start
entry:
/* HOST CPU */
	GET_PIC_PTR(_start, $r24)
	;;
	mov   $r63 = $r24
	;;
	call  $r63 = $r63
	;;

.balign 0x040
	/* AUDIO CPU */
	/* just spin the moment */
.global entry_audio
entry_audio:
	;;
1:	call	$r63 = 1b
	;;

.balign 0x100
	/* VES CPU */
	/* just spin the moment */
.global entry_ves
entry_ves:
1:	call	$r63 = 1b
	;;
.balign 32

