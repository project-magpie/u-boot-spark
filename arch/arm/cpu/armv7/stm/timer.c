/*
 * (C) Copyright 2012 Nobuhiro Iwamatsu <nobuhiro.iwamatsu.yj@renesas.com>
 * (C) Copyright 2012 Renesas Solutions Corp.
 *
 * Copyright (C) 2013 STMicroelectronics
 *	Sean McGoogan <Sean.McGoogan@st.com>
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
#include <asm/arch-armv7/globaltimer.h>
#include <stm/socregs.h>

#if !defined(STM_A9_GLOBAL_TIMER_BASE)
#define STM_A9_GLOBAL_TIMER_BASE		(STM_A9_CONFIG_BASE + 0x0200)
#endif /* STM_A9_GLOBAL_TIMER_BASE */

static struct globaltimer * const global_timer =
	(struct globaltimer *)STM_A9_GLOBAL_TIMER_BASE;

#define CLK2MHZ(clk)    ((clk) / 1000 / 1000)

/*
 * Extracted from kernel/lib/linux/lib64.c
 */
static u32 div64_32(u64 n,u32 base)
{
    u64 rem = n;
    u64 b = base;
    u64 res, d = 1;
    u32 high = rem >> 32;

    /* Reduce the thing a bit first */
    res = 0;
    if (high >= base)
    {
      high /= base;
      res = (u64) high << 32;
      rem -= (u64) (high*base) << 32;
    }

    while ((s64)b > 0 && b < rem)
    {
      b = b+b;
      d = d+d;
    }

    do
    {
      if (rem >= b)
      {
        rem -= b;
        res += d;
      }
      b >>= 1;
      d >>= 1;
    } while (d);

    /* rem is reminder */
    return res;
}

static u64 get_cpu_global_timer(void)
{
    u32 low, high;
    u64 timer;

    u32 old = readl(&global_timer->cnt_h);
    while (1) {
        low = readl(&global_timer->cnt_l);
        high = readl(&global_timer->cnt_h);
        if (old == high)
            break;
        else
            old = high;
    }

    timer = high;
    return (u64)((timer << 32) | low);
}

static u64 get_time_us(void)
{
    u64 timer = get_cpu_global_timer();

    timer = ((timer << 1) + (CLK2MHZ(CONFIG_SYS_CPU_CLK) >> 1));
    timer = div64_32(timer,CLK2MHZ(CONFIG_SYS_CPU_CLK));
    return timer;
}

static ulong get_time_ms(void)
{
   return (ulong)(div64_32(get_time_us(),1000));
}

extern int timer_init(void)
{
    writel(0x01, &global_timer->ctl);
    return 0;
}

extern void __udelay(unsigned long usec)
{
    u64 start, current;
    u64 wait;

    start = get_cpu_global_timer();
    wait = (u64)((usec * CLK2MHZ(CONFIG_SYS_CPU_CLK)) >> 1);
    do {
        current = get_cpu_global_timer();
    } while ((current - start) < wait);
}

extern ulong get_timer(ulong base)
{
    return get_time_ms() - base;
}

extern unsigned long long get_ticks(void)
{
    return get_cpu_global_timer();
}

extern ulong get_tbclk(void)
{
    return (ulong)(CONFIG_SYS_CPU_CLK >> 1);
}
