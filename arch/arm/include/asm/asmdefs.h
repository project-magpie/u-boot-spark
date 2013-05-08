/*
 * (C) Copyright 2004, 2007, 2009, 2011, 2013 STMicroelectronics.
 *
 * Andy Sturges <andy.sturges@st.com>
 * Sean McGoogan <Sean.McGoogan@st.com>
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


/*
 * Write out a series of bytes, monotonically increasing
 * in value from "first" to "last" (inclusive).
 *
 * Usage:	BYTES <first> <last>
 * where <first> is the first byte to generate
 * where <last>  is the last byte to generate
 *
 * Note: this macro uses recursion (one level per byte)
 */
.macro BYTES first=0, last=63
	.byte \first
	.if \last-\first
	BYTES "(\first+1)",\last	/* note: recursion */
	.endif
.endm
