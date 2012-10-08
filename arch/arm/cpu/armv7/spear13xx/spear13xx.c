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

void enable_caches(void)
{
#ifndef CONFIG_SYS_DCACHE_OFF
	/* Enable D-cache. I-cache is already enabled in start.S */
	dcache_enable();
#endif
}

#ifdef CONFIG_POST
int arch_memory_test_prepare(u32 *vstart, u32 *size, phys_addr_t *phys_offset)
{
	/*
	 * Run the POST test on 64 MB memory starting from CONFIG_SYS_LOAD_ADDR
	 * The assumption here is that the DDR present on board is >= 128MB.
	 *
	 * The test runs before relocation (after the code copy has taken
	 * place), so it can not touch either before or after relocation areas
	 * of U-boot
	 *
	 * DDR usage
	 * <--------->|<---------------- / --------------->|<---------->
	 *   U-boot		Area to be used for		U-boot
	 *   before		POST test			after
	 *   relocation						relocation
	 */

	*vstart = CONFIG_SYS_LOAD_ADDR;
	*size = 64 << 20;

	return 0;
}

void arch_memory_failure_handle(void)
{
	hang();
}
#endif
