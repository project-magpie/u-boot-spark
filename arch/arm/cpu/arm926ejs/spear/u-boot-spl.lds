/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Microelectronics, <vipin.kumar@st.com>
 *
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
 *
 * January 2004 - Changed to support H4 device
 * Copyright (c) 2004-2008 Texas Instruments
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
	xloader : ORIGIN = 0xD2800B00, LENGTH = 0x1500
}

SECTIONS
{
	.text :
	{
		arch/arm/cpu/arm926ejs/spear/start.o	(.text)
		*(.text); . = ALIGN(4);
	} > xloader

	.rodata : { *(.rodata*); . = ALIGN(4); } > xloader
	.data : { *(.data*); . = ALIGN(4); } > xloader
	.bss : { __bss_start = .; *(.bss*); . = ALIGN(4); __bss_end = .;} > xloader
}
