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

/*
 * Macros to help with position independent code
 */

#define GET_PIC_PTR(LBL, REG)\
	call    $r63 = 1f ;\
	;; ;\
1:      add $r63 = $r63, @neggprel(.) ;\
	  mov REG = @neggprel(LBL) ;\
	;; ; \
	sub REG = $r63, REG ;\
	;; ;

#define CALL_FN(LBL,VAL)\
	GET_PIC_PTR(LBL, $r24);\
	add $r16 = $r0, VAL;\
	  mov $r63 = $r24;\
	;; ;\
	call $r63 = $r63;\
	  add  $r12 = $r12,-16;\
	;; ;\
	add  $r12 = $r12,16;\
	;;

#define CALL_FN_NO_STACK(LBL,VAL)\
	GET_PIC_PTR(LBL, $r24);\
	add $r16 = $r0, VAL;\
	  mov $r63 = $r24;\
	;; ;\
	call $r63 = $r63;\
	;; ;
