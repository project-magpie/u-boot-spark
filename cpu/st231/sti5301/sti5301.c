/*
 * (C) Copyright 2004 STMicroelectronics.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>

#define WATCHDOG_COUNTER_CFG0 0x20F00130
#define WATCHDOG_COUNTER_CFG1 0x20F00134
#define RESET_STATUS 0x20F00140

#define WD_COUNT_EN (1<<4)
#define WD_COUNT    2
#define WD_RESET_MASK (1<<6)

/* Use watchdog timer to reset chip */

void sti5301_reset (void)
{
	long status;
	status = readl (RESET_STATUS);
	writel (status & ~WD_RESET_MASK, RESET_STATUS);
	writel (WD_COUNT, WATCHDOG_COUNTER_CFG0);
	writel (WD_COUNT_EN, WATCHDOG_COUNTER_CFG1);
	while (1);
	 /*NOTREACHED*/
	return;
}

