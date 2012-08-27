/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
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

#ifndef __ARCH_ARM_CA9TIMER_H
#define __ARCH_ARM_CA9TIMER_H

struct ca9_timer_regs {
	u32 load;
	u32 count;
	u32 control;
};

/* control related definitions */
#define AUTO_RELOAD		(1 << 1)
#define TIMER_ENABLE		(1 << 0)

/* load related definitions */
#define FREE_RUNNING		(0xFFFFFFFF)

#endif
