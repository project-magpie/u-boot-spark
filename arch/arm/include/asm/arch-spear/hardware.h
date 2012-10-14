/*
 * (C) Copyright 2009
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

#ifndef _ASM_ARCH_SPEAR_HARDWARE_H
#define _ASM_ARCH_SPEAR_HARDWARE_H

#if   defined(CONFIG_SOC_SPEAR600)
#include <asm/arch/spear600.h>
#elif defined(CONFIG_SOC_SPEAR300)
#include <asm/arch/spear300.h>
#elif defined(CONFIG_SOC_SPEAR310)
#include <asm/arch/spear310.h>
#elif defined(CONFIG_SOC_SPEAR320)
#include <asm/arch/spear320.h>
#endif

#endif /* _ASM_ARCH_HARDWARE_H */
