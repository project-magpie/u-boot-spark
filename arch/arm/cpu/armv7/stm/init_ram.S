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


#define LSB(x)		((x) & 1)	/* Return only the Least Significant Bit */
#define LSB_MASK(x)	((x) & ~1)	/* Mask off the Least Significant Bit */


	.balign 4
	/*
	 * The init_ram() function should initialize all the RAM, and it does
	 * that by calling the "pokeloop" interpreter function: poke_loop().
	 * That function executes the entire "poke-table", that is typically
	 * created by the "romgen" utility, and should perform the same set
	 * of "pokes" that GDB would normally perform.
	 *
	 * This code is just a wrapper around the "poke_loop()" function,
	 * which relocates the entirety of the poke-table *AND* the entirety
	 * of the poke_loop() interpreter into SRAM, and passes the correct
	 * parameters to the called function, whose C prototype would be:
	 *
	 * extern int poke_loop(const unsigned int* pokeTable, unsigned int deviceID);
	 *
	 * We are presumably booting from FLASH, and we do not have RAM yet,
	 * so there is no stack. As this code will be running from the wrong
	 * location, this code *must* be PIC!
	 * This code expects to be run with the I-cache enabled.
	 * This code expects to be run with the D-cache DISabled.
	 * This code will trash r0-r9 (and lr), it will preserve IP (r12).
	 */
init_ram:
	mov	r4, lr			/* Stash the LR somewhere safe */
	mov	r5, ip			/* Stash the IP (r12) somewhere safe */

	/*
	 * The following code and layout assumes:
	 *
	 *	1) The poke_loop() (both code+data) is placed immediately
	 *	   before the poke-table (i.e. they abut).
	 *
	 *	2) The poke interpreter begins at poke_loop
	 *
	 *	3) The poke interpreter is PIC (no fix-ups needed)
	 *
	 *	4) The poke-table begins at __memory_setup_table
	 *
	 *	5) The poke-table ends at __memory_setup_table_end
	 *
	 *	6) Both the poke_loop(), and the poke-table will
	 *	   easily fit in the SRAM, with no issues.
	 */

	/*
	 * Initialize our local variables
	 */
	adr	r3, source_start_pic	/* r3 = base of PIC calculations */
	ldr	r0, source_start_pic
	add	r0, r0, r3		/* r0 = poke_loop() in FLASH */
	ldr	r2, source_end_pic
	add	r2, r2, r3		/* r2 = __memory_setup_table_end in FLASH */
	ldr	r1, sram_start_address	/* r1 = CONFIG_STM_SRAM_POKE_STORAGE */

	/*
	 * Load the entire poke-table into the SRAM/ERAM buffer.
	 */
					/* copy 16 bytes at a time */
1:	ldmia	r0!, {r6 - r9}		/* copy from source address [r0] */
	stmia	r1!, {r6 - r9}		/* copy to   target address [r1] */
	cmp	r0, r2			/* until source end address [r2] */
	ble	1b

	/*
	 * Synchronisation Barriers … before we call the relocated code!
	 */
	dsb
	isb

	/*
	 * It is now safe to call poke_loop() with real data, since the
	 * poke-table data is now safely in the SRAM buffer. In addition,
	 * both the code and the (data for the) jump-table for the poke_loop()
	 * interpreter, have also been safely copied into SRAM.  Recall,
	 * setting up the memory interfaces may cause the EMI (where FLASH
	 * images resides) briefly to hang - but the CPU will be safely
	 * executing from the I-cache, with all its corresponding data in SRAM.
	 * That is, we can safely run with the data-caches OFF, knowing we
	 * will not try and read ANYTHING from the boot-device on the EMI.
	 */
	ldr	r0, poketable_start_address	/* r0 = __memory_setup_table in SRAM */
	ldr	r1, deviceid_address
	ldr	r1, [r1]		/* r1 = deviceID */
	ldr	r2, poke_loop_in_sram	/* r2 = poke_loop() in SRAM */
	blx	r2			/* poke_loop(r0,r1); */

	mov	ip, r5			/* Restore the IP (r12) */
	bx	r4			/* return to my caller */

	/*
	 * Variables …
	 */
	.balign 4
source_start_pic:	.long LSB_MASK(poke_loop - source_start_pic)
source_end_pic:		.long __memory_setup_table_end - source_start_pic
sram_start_address:	.long CONFIG_STM_SRAM_POKE_STORAGE
poke_loop_in_sram:	.long CONFIG_STM_SRAM_POKE_STORAGE + 1 /* QQQ: ought to be LSB(poke_loop) */
poketable_start_address:.long CONFIG_STM_SRAM_POKE_STORAGE + __memory_setup_table - poke_loop
	/*
	 * The poke_loop() function, should be passed the SoC's DeviceID.
	 * The following should be used to hold the address of the
	 * system configuration register which contains the DeviceID.
	 * Note: we can not retrieve this ID from stm_devid in struct global_data,
	 * as we do have RAM yet, hence this structure is not initialized.
	 */
deviceid_address:
#if defined(CONFIG_STM_STXH407)
		STXH407_SYSCONF_DEVICEID
#elif defined(CONFIG_STM_STXH415)
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
