/*
 * (C) Copyright 2004-2013 STMicroelectronics.
 *
 * Andy Sturges <andy.sturges@st.com>
 * Stuart Menefy <stuart.menefy@st.com>
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
#include <stm/socregs.h>
#include <asm/asmdefs.h>


	.syntax		unified
	.arch		armv7
	.cpu		cortex-a9
	.arm
	.section	.text.init, "ax"

	.globl		init_ram
	.type		init_ram, %function


	/*
	 * We want some SRAM to store the "poke-table", whilst the
	 * "pokeloop" interpreter is being executed. This is because
	 * whilst the "pokeloop" interpreter is being executed, both
	 * LMI and EMI may temporarily "disappear" from the CPU.
	 * On the ARM, it is considered to be "unsafe" to use the
	 * D-caches to hold the poke table (c.f. ST40), so we will
	 * use a small SRAM buffer instead, as this will be safer.
	 * This buffer only needs to be big enough to hold the raw
	 * binary poke-table, so 32KiB (or more) should be sufficient.
	 * CONFIG_STM_SRAM_START is the beginning of this SRAM buffer.
	 */
#if !defined(CONFIG_STM_SRAM_START)
#	define CONFIG_STM_SRAM_START	0xc00c0000	/* Start of HVA_eRAM (256KiB) */
#endif


	.balign 4
	/*
	 * The init_ram() function should initialize all the RAM, and it does
	 * that by calling the "pokeloop" interpreter function: poke_loop().
	 * That function executes the entire "poke-table", that is typically
	 * created by the "romgen" utility, and should perform the same set
	 * of "pokes" that GDB would normally perform.
	 *
	 * This code is just a wrapper around the "poke_loop()" function,
	 * which copies the poke-table into SRAM, and passes the correct
	 * parameters to the called function, whose C prototype would be:
	 *
	 * extern int poke_loop(const unsigned int* pokeTable, unsigned int deviceID);
	 *
	 * We are presumably booting from FLASH, and we do not have RAM yet,
	 * so there is no stack. As this code will be running from the wrong
	 * location, this code *must* be PIC!
	 * This code expects to be run with the I-cache enabled.
	 * This code expects to be run with the D-cache DISabled.
	 * This code will trash r0-r10 (and lr), it will preserve IP (r12).
	 */
init_ram:
	mov	r4, lr			/* Stash the LR somewhere safe */
	mov	r5, ip			/* Stash the IP (r12) somewhere safe */

	/*
	 * Initialize our local variables
	 */
	adr	r6, poke_loop_address	/* r6 = base of PIC calculations */
	ldr	r3, poke_loop_address
	add	r3, r3, r6		/* r3 = &poke_loop(); */
	ldr	r2, data_end_address
	add	r2, r2, r6		/* r2 = __memory_setup_table_end; */
	ldr	r0, data_start_address
	add	r0, r0, r6		/* r0 = __memory_setup_table; */
	ldr	r1, sram_start_address	/* r1 = CONFIG_STM_SRAM_START */
	mov	r6, r1			/* r6 = CONFIG_STM_SRAM_START */

	/*
	 * Load the entire poke-table into the SRAM/ERAM buffer.
	 */
					/* copy 16 bytes at a time */
1:	ldmia	r0!, {r7 - r10}		/* copy from source address [r0] */
	stmia	r1!, {r7 - r10}		/* copy to   target address [r1] */
	cmp	r0, r2			/* until source end address [r2] */
	ble	1b

	/*
	 * It is now safe to call poke_loop() with real data, since the
	 * poke-table data is now safely in the SRAM buffer. Recall,
	 * setting up the memory interfaces may cause the EMI (where FLASH
	 * images resides) briefly to hang - but the CPU will be safely
	 * executing from the I-cache, with its corresponding data in SRAM.
	 */
	mov	r0, r6			/* r0 = CONFIG_STM_SRAM_START */
	ldr	r1, deviceid_address
	ldr	r1, [r1]		/* r1 = deviceID */
	blx	r3			/* poke_loop(r0,r1); */

	mov	ip, r5			/* Restore the IP (r12) */
	bx	r4			/* return to my caller */


	/*
	 * Variables …
	 */
	.balign 4
poke_loop_address:	.long poke_loop - poke_loop_address
data_start_address:	.long __memory_setup_table - poke_loop_address
data_end_address:	.long __memory_setup_table_end - poke_loop_address
sram_start_address:	.long CONFIG_STM_SRAM_START
	/*
	 * The poke_loop() function, should be passed the SoC's DeviceID.
	 * The following should be used to hold the address of the
	 * system configuration register which contains the DeviceID.
	 * Note: we can not retrieve this ID from bi_devid in struct bd_info,
	 * as we do have RAM yet, hence this structure is not initialized.
	 */
deviceid_address:
#if defined(CONFIG_STM_STXH415)
		STXH415_SYSCONF_DEVICEID
#elif defined(CONFIG_STM_STXH416)
		STXH416_SYSCONF_DEVICEID
#else
#error Please specify the DeviceID register's address.
#endif


/*
 * Pull in the "poke interpreter". This interpreter should be fully
 * compatible with the poke-table produced by the "romgen" utility.
 */
#define POKELOOP_FUNC_NAME_ASM	poke_loop
#define POKELOOPSECTION		.text.init
#include "pokeloop.S"
