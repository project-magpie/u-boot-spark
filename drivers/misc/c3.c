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

#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/c3.h>
#include <asm/arch/hardware.h>

static unsigned long c3_mem_xlate(void *addr)
{

	if (((ulong)addr < C3_INT_MEM_BASE_ADDR) || \
		((ulong)addr >= (C3_INT_MEM_BASE_ADDR + C3_INT_MEM_SIZE)))
		return (ulong)addr;

	return (unsigned long)addr - C3_INT_MEM_BASE_ADDR +
		C3_LOCAL_MEM_ADDR;
}

int c3_init(void)
{
	if (readl(C3_ID0_SCR) != C3_ID0_DEF_RDY_VAL)
		writel(C3_ID0_SCR_RST, C3_ID0_SCR);

	if (readl(C3_MOVE_CHANNEL_ID) == C3_MOVE_CHANNEL_ID_VAL)
		return -EINVAL;

	writel(C3_HIF_MCR_ENB_INT_MEM, C3_HIF_MCR);
	writel(C3_LOCAL_MEM_ADDR, C3_HIF_MBAR);

	return 0;
}

static int c3_run(void *prog_start)
{
	writel(c3_mem_xlate(prog_start), C3_ID0_IP);

	while ((readl(C3_ID0_SCR) & C3_ID0_STATE_MASK) == C3_ID0_STATE_RUN)
		;

	if ((readl(C3_ID0_SCR) & C3_ID0_STATE_MASK) != C3_ID0_STATE_IDLE) {
		/* If not back to idle an error occured */
		writel(C3_ID0_SCR_RST, C3_ID0_SCR);

		/* Set internal access to run c3 programs */
		writel(C3_HIF_MCR_ENB_INT_MEM, C3_HIF_MCR);

		return -EIO;
	}

	return 0;
}

static int c3_move(void *dest, void *src, int cnt, int optype, int opdata)
{
	unsigned long *c3_prog;
	int ret = 0;

	/* 3.b Prepare program */
	c3_prog = (unsigned long *)C3_INT_MEM_BASE_ADDR;

	/* 3.b.i. Mov init */
	c3_prog[0] = C3_CMD_MOVE_INIT;
	c3_prog[1] = opdata;

	/* 3.b.ii. Mov data */
	c3_prog[2] = C3_CMD_MOVE_DATA + cnt + optype;
	c3_prog[3] = c3_mem_xlate(src);
	c3_prog[4] = c3_mem_xlate(dest);

	/* 3.b.iii. Stop */
	c3_prog[5] = C3_CMD_STOP;

	/* 4. Execute and wait */
	ret = c3_run(c3_prog);

	return ret;
}

void *c3_memset(void *s, int c, size_t count)
{
#define DATA_SIZE (1024*4)
	u32 data = C3_INT_MEM_BASE_ADDR + 0x100;
	u32 size;
	size_t cur = 0;

	writel(0x100, C3_HIF_MAAR);
	writel(c, C3_HIF_MADR);

	for (size = 4; size < DATA_SIZE; size <<= 1)
		c3_move((void *)(data + size), (void *)data, size,
				C3_MOVE_AND, 0xffffffff);

	while (cur < count) {
		c3_move(s + cur, (void *)data, DATA_SIZE,
				C3_MOVE_AND, 0xffffffff);
		cur += DATA_SIZE;
	}

	return s;
}
