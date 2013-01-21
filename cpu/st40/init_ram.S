/*
 * (C) Copyright 2004-2013 STMicroelectronics.
 *
 * Andy Sturges <andy.sturges@st.com>
 * Start Menefy <stuart.menefy@st.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include "asm/regdef.h"
#include "asm/asmdefs.h"
#include "asm/linkage.h"
#include "asm/cache.h"

/*
 *	NOTE:	 ALL THIS CODE MUST BE PIC !!!!
 *
 *	This code expects to be run with the caches enabled.
 */

	/*
	 * If we are using ABSOLUTE (instead of PIC) addresses for the "poke-table",
	 * then we really need to define *both* of the following #defines, to
	 * convey the start and end of the poke-table. We #error otherwise!
	 *
	 *		CONFIG_SYS_ABSOLUTE_POKE_TABLE_BEGIN
	 *		CONFIG_SYS_ABSOLUTE_POKE_TABLE_END
	 *
	 * NOTE: It is (in general) ill-advisable to define either of these macros.
	 * Extreme care must be exercised if you do so - caveat emptor!
	 *
	 * If using this specific feature, then please note the following points:
	 *
	 *	1) Both addresses *must* be cached addresses (e.g. P1).
	 *	   This code will not enforce this requirement!
	 *
	 *	2) Both addresses *must* be 4-byte aligned.
	 *	   This code will not enforce this requirement!
	 *
	 *	3) The code will be executed only in 29-bit mode, so please
	 *	   ensure the addresses are chosen accordingly. Typically, the
	 *	   addresses will be in the P1 region, for a Physical address,
	 *	   which is in the EMI Boot Controller space (i.e. a cached
	 *	   address of the FLASH on EMI_BANK_0).
	 *
	 *	4) If using this on a SoC with a "direct-mapped" cache,
	 *	   (instead of 2-way set associative), then additional
	 *	   care is required to avoid any cache lines for the poke-table
	 *	   being prematurely evicted due to "cache-thrashing"!
	 *
	 *	5) As a minimum, the first 12-bytes (at the start address)
	 *	   will always be read, in poke_loop(), so the addresses
	 *	   specified must be valid, with no side-effects!
	 *
	 *	6) In all cases, the referenced poke-table should terminate
	 *	   with a special end marker, to denote the end of the poke-table
	 *	   (this was an opcode of ZERO).  The macro "END_MARKER"
	 *	   can be used from the assembler.
	 *
	 *	7) If an "empty" poke-table is required, then the empty
	 *	   poke-table referenced, should be at least a 12-byte region of
	 *	   all zeros. Please see the definition of the macro "END_MARKER"
	 *	   for a suitable definition (in "include/poketable.h").
	 */
#if defined(CONFIG_SYS_ABSOLUTE_POKE_TABLE_BEGIN) && !defined(CONFIG_SYS_ABSOLUTE_POKE_TABLE_END)
#error It is dangerous to define only *one* of CONFIG_SYS_ABSOLUTE_POKE_TABLE_BEGIN and CONFIG_SYS_ABSOLUTE_POKE_TABLE_END
#endif
#if !defined(CONFIG_SYS_ABSOLUTE_POKE_TABLE_BEGIN) && defined(CONFIG_SYS_ABSOLUTE_POKE_TABLE_END)
#error It is dangerous to define only *one* of CONFIG_SYS_ABSOLUTE_POKE_TABLE_BEGIN and CONFIG_SYS_ABSOLUTE_POKE_TABLE_END
#endif

	.section .text.init, "ax"

ENTRY(init_ram)
	/* Stash the pr somewhere safe */
	sts	pr, r14

	/* read the DEVICE_ID, and stash in a register */
	GETDEVID r5	/* use R5 for the NEW interpreter */

	/*
	 * We need to get the poke loop & associated data
	 * into caches. The poke loop is structured so that it is
	 * all pulled into cache on it 1st iteration.
	 * To get the poke table into D-cache, we simply read it all.
	 */
	mova	poke_loop_address, r0	/* R6: &poke_loop() */
	mov.l	@r0, r6
	add	r0, r6
	mov.l	p2_to_p1_mask, r3	/* R3: P2 -> P1 mapping */
	and	r3, r6			/* convert to P1 addresses */

	/*
	 * Put the (cached) address, corresponding to the FIRST byte
	 * of the poke-table, into the register R1.
	 */
	mov.l	data_start_address, r1	/* R1 = start address */
#if !defined(CONFIG_SYS_ABSOLUTE_POKE_TABLE_BEGIN)
	add	r0, r1			/* convert from PIC */
	and	r3, r1			/* convert to a P1 addresses */
#endif

	/*
	 * Put the (cached) address, corresponding to the byte
	 * AFTER the end of the poke-table, into the register R2.
	 */
	mov.l	data_end_address, r2	/* R2 = end address */
#if !defined(CONFIG_SYS_ABSOLUTE_POKE_TABLE_END)
	add	r0, r2			/* convert from PIC */
	and	r3, r2			/* convert to a P1 addresses */
#endif

	/*
	 * Load the entire poke table into the operand cache.
	 * We just need to load one word per cache line.
	 */
	mov	r1, r0			/* R0 = scratch data ptr */
1:	mov.l	@r0, r4			/* read one word per cache line */
	add	#DCACHE_LINESZ, r0	/* next cache line */
	cmp/hi	r2, r0			/* done ? */
	bf	1b

	/*
	 * Its now safe to call the poke loop with real data, since it
	 * and its associated data table are in on-chip caches. Setting
	 * up the memory interfaces may cause the EMI (where this FLASH
	 * image resides) to briefly hang - but the CPU will be safely
	 * executing from cache should this happen.
	 */
do_pokes:
	mov	r1, r4		/* __memory_setup_table */
	jsr	@r6		/* R6 still points poke_loop() */
	  nop

	/* Restore the PR */
	lds	r14, pr
	rts
	  nop


	.balign 4
poke_loop_address:	.long poke_loop - poke_loop_address
#if defined(CONFIG_SYS_ABSOLUTE_POKE_TABLE_BEGIN)
data_start_address:	.long CONFIG_SYS_ABSOLUTE_POKE_TABLE_BEGIN
#else
data_start_address:	.long __memory_setup_table - poke_loop_address
#endif
#if defined(CONFIG_SYS_ABSOLUTE_POKE_TABLE_END)
data_end_address:	.long CONFIG_SYS_ABSOLUTE_POKE_TABLE_END
#else
data_end_address:	.long __memory_setup_table_end - poke_loop_address
#endif
p2_to_p1_mask:		.long ~0x20000000



/*
 * Pull in the "poke interpreter". This interpreter should be fully
 * compatible with the poke-table produced by the "romgen" utility.
 */
#define POKELOOP_FUNC_NAME_ASM	poke_loop
#define POKELOOPSECTION		.text.init
#include "pokeloop.S"


