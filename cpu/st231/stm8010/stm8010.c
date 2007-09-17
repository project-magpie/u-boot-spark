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

#define PLL_INPUT_FREQ_KHZ 30000

/*
 * Clockgen registers.
 */

#define ST200_PLL1CR1      (0x56000000)
#define ST200_CLOCK_RATIO  (0x56000048)

typedef struct _divisors_s
{
  unsigned int st200_clk_divisor;
  unsigned int stbus_clk_divisor;
} _divisors_t;

static _divisors_t divisors[16] =
{
  {2, 4},
  {2, 6},
  {2, 6},
  {2, 6},
  {2, 6},
  {2, 6},
  {2, 6},
  {2, 6},
  {2, 6},
  {1, 3},
  {1, 3},
  {1, 2},
  {1, 3},
  {1, 1},
  {1, 3},
  {1, 2}
};

/*
 * Work out a PLL1 frequency
 */
static unsigned int bsp_pll_freq (volatile unsigned int * pllcr)

{
  unsigned int m;
  unsigned int n;
  unsigned int p;

  /*
   * Extract the values of the PLL dividers.
   */
  m = 0xFF &  *pllcr;
  n = 0xFF & (*pllcr >> 8);
  p = 0x07 & (*pllcr >> 16);

  /*
   * We can now work out the PLL frequency!
   */
  return ((2 * n * (PLL_INPUT_FREQ_KHZ)) / (m * (1 << p)));
}

/*
 * Return the core clock frequency.
 */
static unsigned int    bsp_clock_freqs (int * cpu_freq, int * bus_freq)
{
  unsigned int pll_freq_khz ;
  unsigned int ratio =  (*((volatile unsigned int  *)(ST200_CLOCK_RATIO))) & 0x0000000f;
  pll_freq_khz  = bsp_pll_freq ( (volatile unsigned int *)  ST200_PLL1CR1  );

  if (cpu_freq )
    *cpu_freq = 1000*pll_freq_khz / divisors[ratio ].st200_clk_divisor ;

  if (bus_freq )
    *bus_freq  = 1000 * pll_freq_khz / divisors[ratio ].stbus_clk_divisor;

  return 0;
}

void stb8010_display_clocks(void)
{
  int cpu, bus;
  bsp_clock_freqs(&cpu, &bus);
  printf("PLL1      = %d MHz\n", *(volatile unsigned int *)ST200_PLL1CR1);
  printf("ST231 CPU = %d MHz, bus=%dMHz\n", cpu/1000000, bus/1000000);
}

/* Use watchdog timer to reset chip */

#define WDT_HZ 36000

#define COMMS_SS_BASE   0x54000000
#define ILC_BASE_ADDR   (COMMS_SS_BASE + 0x80000)

#define LPA_LS     (ILC_BASE_ADDR+0x410)
#define LPA_MS     (ILC_BASE_ADDR+0x414)
#define LPA_START  (ILC_BASE_ADDR+0x418)
#define WD_ENABLE  (ILC_BASE_ADDR+0x510)

#define WD_COUNT    (2 * WDT_HZ)

void stm8010_reset (void)
{
	writel(1, WD_ENABLE);
	writel(0, LPA_MS);
	writel(WD_COUNT, LPA_LS);
	writel(1, LPA_START);
	while (1);
	{
	  ;
	}
	 /*NOTREACHED*/
	return;
}

