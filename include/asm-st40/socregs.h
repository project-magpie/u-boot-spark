/*
 * Copyright (C) STMicroelectronics Ltd. 2004-2013
 *
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


#ifndef __ASM_ST40_SOC_REG_H
#define __ASM_ST40_SOC_REG_H


#if defined(CONFIG_ST40_STX5197)
#	include <asm/stx5197reg.h>
#elif defined(CONFIG_ST40_STX5206)
#	include <asm/stx5206reg.h>
#elif defined(CONFIG_ST40_STX7105)
#	include <asm/stx7105reg.h>
#elif defined(CONFIG_ST40_STX7108)
#	include <asm/stx7108reg.h>
#elif defined(CONFIG_ST40_STX7111)
#	include <asm/stx7111reg.h>
#elif defined(CONFIG_ST40_STX7141)
#	include <asm/stx7141reg.h>
#elif defined(CONFIG_ST40_STX7200)
#	include <asm/stx7200reg.h>
#elif defined(CONFIG_ST40_FLI7510)
#	include <asm/fli7510reg.h>
#elif defined(CONFIG_ST40_FLI7540)
#	include <asm/fli7540reg.h>
#elif defined(CONFIG_ST40_STXH205)
#	include <asm/stxh205reg.h>
#elif defined(CONFIG_ST40_STXH415)
#	include <asm/stxh415reg.h>
#elif defined(CONFIG_ST40_STXH416)
#	include <asm/stxh416reg.h>
#else
#	error Missing Device Definitions!
#endif


#endif /* __ASM_ST40_SOC_REG_H */
