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

#include <common.h>
#include <asm/io.h>
#include <asm/ca9_ltimer.h>
#include <asm/arch/hardware.h>

#define READ_TIMER()	readl(&ca9_timer_p->count)

static struct ca9_timer_regs *const ca9_timer_p =
	(struct ca9_timer_regs *)CONFIG_ARMV7_LTIMER_BASE;

DECLARE_GLOBAL_DATA_PTR;

#define timestamp	gd->tbl
#define lastdec		gd->lastinc
#define tickshz		gd->timer_rate_hz
#define ticksper10usec	gd->tbu

int timer_init(void)
{
	u32 prescaler, timertickshz;
	/*
	 * Genrally, CortexA9 MPUs are operating from 500MHz to 1500MHz which
	 * means that CA9 local timer clock would be in the range of 250 MHz to
	 * 750MHz.
	 * Try to find a prescaler which can perfectly divide the local timer
	 * clock. Take prescaler as 200 if nothing is found
	 */
	for (prescaler = 255; prescaler > 1; prescaler--) {
		if (CONFIG_ARMV7_LTMR_CLK ==
				(CONFIG_ARMV7_LTMR_CLK / prescaler) * prescaler)
			break;
	}

	if (prescaler == 1)
		prescaler = 200;
	timertickshz = CONFIG_ARMV7_LTMR_CLK / prescaler;
	ticksper10usec = timertickshz / (100 * 1000);
	tickshz = timertickshz / CONFIG_SYS_HZ;

	/* disable timers */
	writel(((prescaler - 1) << 8) | AUTO_RELOAD, &ca9_timer_p->control);

	/* load value for free running */
	writel(FREE_RUNNING, &ca9_timer_p->load);

	/* auto reload, start timer */
	writel(readl(&ca9_timer_p->control) | TIMER_ENABLE,
			&ca9_timer_p->control);

	reset_timer_masked();

	return 0;
}

/*
 * timer without interrupts
 */

void reset_timer(void)
{
	reset_timer_masked();
}

ulong get_timer(ulong base)
{
	return (get_timer_masked() / tickshz) - base;
}

void set_timer(ulong t)
{
	timestamp = t;
}

void __udelay(unsigned long usec)
{
	ulong tmo;
	ulong start = get_timer_masked();
	ulong rndoff;

	rndoff = (usec % 10) ? 1 : 0;
	tmo = ((usec / 10) + rndoff) * ticksper10usec;

	while ((ulong) (get_timer_masked() - start) < tmo)
		;
}

void reset_timer_masked(void)
{
	/* reset time */
	lastdec = READ_TIMER();
	timestamp = 0;
}

ulong get_timer_masked(void)
{
	ulong now = READ_TIMER();

	if (now <= lastdec) {
		/* normal mode */
		timestamp += lastdec - now;
	} else {
		/* we have an overflow ... */
		timestamp += lastdec + FREE_RUNNING - now;
	}
	lastdec = now;

	return timestamp;
}

void udelay_masked(unsigned long usec)
{
	return udelay(usec);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}
