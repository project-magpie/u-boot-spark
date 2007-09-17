/*
 * (C) Copyright 2004 STMicroelectronics
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

#include "common.h"
#include <asm/cache.h>

/*
 * Write back the dirty D-caches, but not invalidate them.
 *
 * START: Virtual Address (U0, P1, or P3)
 * SIZE: Size of the region.
 */

struct __large_struct { unsigned long buf[100]; };
#define __m(x) (*(struct __large_struct *)(x))

#if 0
static void __flush_wback_region(void *start, int size)
{
	unsigned long v;
	unsigned long begin, end;

	begin = (unsigned long)start & ~(L1_CACHE_BYTES-1);
	end = ((unsigned long)start + size + L1_CACHE_BYTES-1)
		& ~(L1_CACHE_BYTES-1);
	for (v = begin; v < end; v+=L1_CACHE_BYTES) {
		asm volatile("ocbwb	%0"
			     : /* no output */
			     : "m" __m(v));
	}
}
#endif
/*
 * Write back the dirty D-caches and invalidate them.
 *
 * START: Virtual Address (U0, P1, or P3)
 * SIZE: Size of the region.
 */
static void __flush_purge_region(void *start, int size)
{
	unsigned long v;
	unsigned long begin, end;

	begin = (unsigned long)start & ~(L1_CACHE_BYTES-1);
	end = ((unsigned long)start + size + L1_CACHE_BYTES-1)
		& ~(L1_CACHE_BYTES-1);
	for (v = begin; v < end; v+=L1_CACHE_BYTES) {
		asm volatile("ocbp	%0"
			     : /* no output */
			     : "m" __m(v));
	}
}

#if 0
/*
 * No write back please
 */

static void __flush_invalidate_region(void *start, int size)
{
	unsigned long v;
	unsigned long begin, end;

	begin = (unsigned long)start & ~(L1_CACHE_BYTES-1);
	end = ((unsigned long)start + size + L1_CACHE_BYTES-1)
		& ~(L1_CACHE_BYTES-1);
	for (v = begin; v < end; v+=L1_CACHE_BYTES) {
		asm volatile("ocbi	%0"
			     : /* no output */
			     : "m" __m(v));
	}
}


#endif

void flush_cache (ulong start_addr, ulong size)
{
   __flush_purge_region(start_addr, size);
}

int cleanup_before_linux (void)
{
	return (0);
}
