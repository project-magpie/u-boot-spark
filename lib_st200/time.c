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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/ctrlregdef.h>

#define TIMEDIVIDE   0x0248
#define TIMECONST0   0x0200
#define TIMECOUNT0   0x0208
#define TIMECONTROL0 0x0210

#define PERIPHERAL_CLK_FREQ     133	/* MHz */

static ulong peripheral_base;

#define TMU_WRITE(reg,num) writel(num,peripheral_base+(reg))
#define TMU_READ(reg) readl(peripheral_base+(reg))

#define MAX_TIME 0xffffffff

int timer_init (void)
{
	peripheral_base = readl (CTRL_ADDR (0xffb0));
	TMU_WRITE (TIMEDIVIDE, PERIPHERAL_CLK_FREQ * 10);	/* 10 us clock */
	TMU_WRITE (TIMECOUNT0, MAX_TIME);
	TMU_WRITE (TIMECONST0, MAX_TIME);
	TMU_WRITE (TIMECONTROL0, 1);	/* start timer */
	return 0;
}

void reset_timer (void)
{
	TMU_WRITE (TIMECOUNT0, MAX_TIME);
}

ulong get_timer (ulong base)
{
	return (TMU_READ (TIMECOUNT0) ^ MAX_TIME) - base;
}

void set_timer (ulong t)
{
	TMU_WRITE (TIMECOUNT0, t ^ MAX_TIME);
}

void udelay (unsigned long usec)
{
	ulong tmo;
	ulong start = get_timer (0);

	tmo = (usec * CFG_HZ) / 1000000;
	while (get_timer (start) < tmo)
		/*NOP*/;

}

ulong get_tbclk (void)
{
	return CFG_HZ;
}
