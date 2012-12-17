/*
 * (C) Copyright 2012
 * ST Micoelectronics Pvt. Ltd.
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

#ifndef _MISC_C3_
#define _MISC_C3_

#include <asm/arch/hardware.h>

#define C3_HIF_OFF		0x400	/* master interface registers */

#define C3_INT_MEM_BASE_ADDR		(CONFIG_SYS_C3_BASE + 0x400)
#define C3_HIF_MBAR			(C3_INT_MEM_BASE_ADDR + 0x304)
	#define C3_LOCAL_MEM_ADDR		0xF0000000
#define C3_HIF_MCR			(C3_INT_MEM_BASE_ADDR + 0x308)
	#define C3_HIF_MCR_ENB_INT_MEM		0x01
#define C3_HIF_MAAR			(C3_INT_MEM_BASE_ADDR + 0x310)
#define C3_HIF_MADR			(C3_INT_MEM_BASE_ADDR + 0x314)

/* ID0 Registers definition */
#define C3_ID0_SCR			(CONFIG_SYS_C3_BASE + 0x1000)
	#define C3_ID0_SCR_RST			(1 << 16)
#define C3_ID0_IP			(CONFIG_SYS_C3_BASE + 0x1000 + 0x10)
	#define C3_ID0_DEF_RDY_VAL		0x80002AAA
	#define C3_ID0_STATE_MASK		0xC0000000
	#define C3_ID0_STATE_RUN		0xC0000000
	#define C3_ID0_STATE_IDLE		0x80000000

/* C3 Register Offsets */
#define C3_MOVE_CHANNEL_ID		(CONFIG_SYS_C3_BASE + 0x2000 + 0x3FC)
	#define C3_MOVE_CHANNEL_ID_VAL		0x00000102

#define C3_INT_MEM_SIZE				0x4000
#define C3_MOVE_AND				(1 << 21)

/* Some Commands */
#define C3_CMD_MOVE_INIT			0x06000000
#define C3_CMD_MOVE_DATA			0x0A800000
#define C3_CMD_STOP				0x00000000

void *c3_memset(void *s, int c, size_t count);
int c3_init(void);

#endif
