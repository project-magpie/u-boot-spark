/*
 * (C) Copyright 2008-2012 STMicroelectronics.
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
#include <asm/soc.h>
#include <asm/io.h>
#include <asm/sysconf.h>


#define TRUE			1
#define FALSE			0


#ifdef DEBUG_PAD_CONFIGS
	extern volatile int debug_pad_configs;
#endif


extern void stm_configure_sysconfs(
	const struct stm_pad_sysconf * const sys_configs,
	const size_t num_sys)
{
	size_t i;
	const struct stm_pad_sysconf * sys;
	unsigned long sysconf;

		/* now configure all the SYS_CONFIGs */
	for (sys=sys_configs, i=0; i<num_sys; sys++, i++)
	{
#ifdef DEBUG_PAD_CONFIGS
		if (debug_pad_configs) {
			printf("%2u: SYSCFG=%p,  [%u:%u]\t0x%08lx\n",
				i+1,
				sys->address,
				sys->msb, sys->lsb,
				sys->value
			);
			printf("ante: *%p = 0x%08lx\n", sys->address, *sys->address);
		}
#endif
		/* the heart of the function -- read, modify, and write */
		sysconf = readl(sys->address);
		SET_SYSCONF_BITS(sysconf, TRUE, sys->lsb, sys->msb, sys->value,sys->value);
		writel(sysconf, sys->address);

#ifdef DEBUG_PAD_CONFIGS
		if (debug_pad_configs)
			printf("post: *%p = 0x%08lx\n", sys->address, *sys->address);
#endif
	}
}


