/*
 * (C) Copyright 2012
 * Vipin Kumar, STMicroelectronics, <vipin.kumar@st.com>
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

#ifndef __CONFIG_SPEAR300_H
#define __CONFIG_SPEAR300_H

#define CONFIG_SOC_SPEAR300

/* PL011 configs */
#if defined(CONFIG_PL011_SERIAL)
#define CONFIG_SYS_SERIAL0			0xD0000000
#define CONFIG_PL011_CLOCK			(48 * 1000 * 1000)
#define CONFIG_PL01x_PORTS			{(void *)CONFIG_SYS_SERIAL0}
#endif

/* FSMC NAND configs */
#if defined(CONFIG_NAND_FSMC)
#define CONFIG_SYS_NAND_BASE			0x80000000
#endif

#include <configs/spear3xx.h>
#endif /* __CONFIG_SPEAR300_H */
