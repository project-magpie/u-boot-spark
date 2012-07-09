/*
 * Copyright (C) 2009 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
 * Copyright (C) 2010-2012 STMicroelectronics.
 *	Sean McGoogan STMicroelectronics, <Sean.McGoogan@st.com>
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
#ifndef __ASM_ST40_CLK_H
#define __ASM_ST40_CLK_H

/*
 * The clocking infrastructure on the ST40 has now got very
 * complicated, and U-Boot sources (for ST40) were no longer
 * able to cope with this increase complexity, without some
 * form of re-factoring exercise.
 *
 * In the good old days, there was just a single "peripheral"
 * clock, then a "comms" clock was added.  At that point,
 * there were two distinct clocks we needed to deal with:
 *	The "comms" clock was used by: ASC, SSC, Watchdog(Comms)
 *	The "peripheral" clock was used by: TMU, SCIF, Watchdog(ST40)
 *
 * Now, there may be two different "comms" clocks - one "system"
 * clock, and a second one for the SBC (Stand-By Controller)
 * domain, and these two "comms" clocks typically run at
 * different frequencies. So, (for example) the UART divider
 * value to be used depended on which "comms" clock is actually
 * being used, depending if the ASC is in the SBC domain or not!
 *
 * It is not required for U-Boot to have a very sophisticated
 * appreciation of the clock infrastructure, and their myriad
 * of interdependencies and their hierarchy. However, U-Boot does
 * need to know how to obtain the actual clock frequencies for
 * each class of peripheral, and know how to compute the divisors.
 * To this end, it has been decided to have a few simple functions
 * which will return the raw frequency for each peripheral that
 * needs to know its clock frequency.
 *
 * Hence we now have the following new functions:
 *
 *	stm_get_uart_clk_rate()	-> UART for the ASC (serial console)
 *	stm_get_tmu_clk_rate()	-> TMU  for the Timers (udelay())
 *	stm_get_ssc_clk_rate()	-> SSC  for the SPI via SSC
 *
 * Each of these functions simply returns the appropriate frequency
 * in the "bd_info" structure (scaled to be in Hertz).
 *
 * The key point, is that we have moved from being clock-centric
 * (where one clock serves many different IP blocks), to being
 * block-centric (where one clock serves only one IP block).
 * This is slightly wasteful if two IP blocks use the same physical
 * clock, but it gives greater flexibility if that association
 * changes in the future! Given the relatively few clocks that
 * U-Boot needs to know about, this seems to be the best option.
 *
 *				Sean McGoogan July 2012.
 */


DECLARE_GLOBAL_DATA_PTR;


	/* Return the UART's Clock Frequency (in Hz) */
static inline unsigned long stm_get_uart_clk_rate(void)
{
	const bd_t * const bd = gd->bd;
	return bd->bi_uart_frq;
}

	/* Return the TMU's Clock Frequency (in Hz) */
static inline unsigned long stm_get_tmu_clk_rate(void)
{
	const bd_t * const bd = gd->bd;
	return bd->bi_tmu_frq;
}

	/* Return the SSC's Clock Frequency (in Hz) */
static inline unsigned long stm_get_ssc_clk_rate(void)
{
	const bd_t * const bd = gd->bd;
	return bd->bi_ssc_frq;
}

#endif /* __ASM_ST40_CLK_H */
