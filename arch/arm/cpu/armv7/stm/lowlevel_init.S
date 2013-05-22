/*
 * Copyright (C) 2011-2013 STMicroelectronics Limited.
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

#include <config.h>
#include "armv7reg.h"


	.syntax		unified
	.arch		armv7
	.cpu		cortex-a9
	.arm
	.section	.text.init, "ax"

	.global		lowlevel_init
	.type		lowlevel_init, %function
	.globl		reset_timer			/* QQQ - DELETE ? */
	.type		reset_timer, %function
	.globl		reset_cpu			/* QQQ - DELETE ? */
	.type		reset_cpu, %function


	.balign 4
reset_timer:		/* QQQ - TO IMPLEMENT PROPERLY */
	bx	lr	/* return immediately! */

reset_cpu:		/* QQQ - TO IMPLEMENT PROPERLY */
	bx	lr	/* return immediately! */


	/*
	 * Main entry point, jumped to by start.S code.
	 */
	.balign 4
lowlevel_init:

	/*
	 * First, we need to invalidate the D-caches. This must be done
	 * *before* we enable the D-caches. Even if we are not enabling the
	 * D-caches in U-Boot, this still needs to be done before we pass
	 * control to a linux kernel - so we may as well do it here anyway.
	 *
	 * We want to do this on all ARM cores (master+slaves), and irrespective
	 * if we are booting from FLASH, via GDB, or some other boot-loader.
	 * Although, interestingly, GDB will (by default) invalidate all
	 * the caches when attaching to a target board via JTAG. However,
	 * performing this unconditionally here is still a "good thing" to do.
	 *
	 * Failure to do this, resulted in a U-Boot which could boot from
	 * FLASH successfully, but could not subsequently boot linux reliably.
	 * So, doing this, really is important!
	 *
	 * The following (cache-invalidating) code was based on code taken
	 * from the file "pbl/src/arch/armv7/mmucache.S", in the OBSP package
	 * (stm-obsp.20131.1-2013.1.1-noarch.tar.gz) from STMicroelectronics.
	 * Many thanks to the original author, for this procedural sequence!
	 */
cacheInvalidateDataAll:
	read_cp15	r0, CP15_CLIDR			/* Cache Level ID Register */
	ands	r3, r0, #CLIDR_LOC_MASK			/* Extract Level of Coherency */
	mov	r3, r3, LSR #(CLIDR_LOC_SHIFT - 1)	/* Cache level value (naturally aligned; 1 in field means level 2) */
	beq	cacheInvalidateDataDone
	ldr	r10, =0					/* R10 = Current cache level minus 1 from bit 1 upwards (ready for use in CSSELR) */
1:	add	r2, r10, r10, LSR #1			/* R2 = 3 * cache level */
	mov	r1, r0, LSR r2				/* Shift cache type to LSBs */
	and	r1, r1, #(CLIDR_CTYPE_MASK(1))
	cmp	r1, #CLIDR_CTYPE_DCACHE			/* Has a D-cache? */
	blt	4f					/* Skip this level if no D-cache */

	write_cp15	r10, CP15_CSSELR		/* Cache Size Selection Register */
	isb						/* ISB to sync the change to the CSSELR */
	read_cp15	r1, CP15_CSIDR			/* Cache Size ID Register */
	and	r2, r1, #CSIDR_LINE_SIZE_MASK
	add	r2, r2, #4				/* Add 4 for the line length offset (log2 16 bytes) */
	ldr	r4, =0x3FF
	ands	r4, r4, r1, LSR #CSIDR_NUM_WAYS_SHIFT	/* R4 is the max number of the way size (right aligned) */
	clz	r8, r4					/* R8 is the bit position of the way size increment */
	ldr	r7, =0x00007FFF
	ands	r7, r7, r1, LSR #CSIDR_NUM_SETS_SHIFT	/* R7 is the max number of the index size (right aligned) */
2:	mov	r9, r4					/* R9 working copy of the max way size (right aligned) */
3:	orr	r11, r10, r9, LSL r8			/* Factor in the way number and cache number into R11 */
	orr	r11, r11, r7, LSL r2			/* Factor in the index number */
	CACHE_INVALIDATE_DCACHE_SET_WAY(r11)		/* Invalidate by set/way */
	subs	r9, r9, #1				/* Decrement the way number */
	bge	3b
	subs	r7, r7, #1				/* Decrement the index */
	bge	2b
4:	add	r10, r10, #2				/* Increment the cache level number (CSSELR usage) */
	cmp	r3, r10
	bgt	1b
cacheInvalidateDataDone:
	dsb						/* Ensure everything is complete */
	isb

	/*
	 * Next, we need to determine if we are already running in the
	 * correct location, or not. The 2 most likely cases are:
	 *
	 *	1) The current PC agrees with the (virtual) address
	 *	   we were originally linked against.
	 *	   This means we are running out of RAM, and we are
	 *	   already correctly loaded at the correct address.
	 *	   So, some other "entity" (e.g. GDB, or some other boot-loader)
	 *	   has already copied us to RAM, and initialized our environment.
	 *	   As this "entity" has copied us to RAM, then we can assume
	 *	   that all the low-level initialisation has been done.
	 *	   That is, we do not need to execute the "pokeloop" interpreter,
	 *	   nor do we need to be kept in a "holding-pen".
	 *	   We simply return, knowing we do not need to do anymore…
	 *
	 *	2) The current PC does NOT agree with the (virtual) address
	 *	   we were originally linked against.
	 *	   This means we are (probably) running out of FLASH, and we are
	 *	   not loaded at the correct address.
	 *	   So, no other "entity" (e.g. GDB, U-Boot, or other boot-loader)
	 *	   has initialized RAM yet, or initialized our environment yet.
	 *	   So, all the low-level initialisation has yet to be done.
	 *	   If we are the "boot-master", we now need to complete the
	 *	   low-level initialisation, and execute the "pokeloop" interpreter.
	 *	   If we are not the "boot-master", we are a SLAVE core,
	 *	   and we need to be kept in a "holding-pen"… for a while!
	 */
whereAreWe:
	adr	r0, here		/* r0 = current position of code */
	ldr	r1, here		/* r1 = &here; (link-time) */
	cmp	r0, r1			/* test if we in the correct place? */
	it	eq			/* Z = (r1 == r0); */
	bxeq	lr			/* return, if in the correct place */

	/*
	 * We now know we are executing at the "wrong" address, so we
	 * assume we are running from FLASH (and not from RAM).
	 * At this stage we can send slave ARM cores to a holding pen where they
	 * can wait, until we are ready to do something with them.
	 * First, we figure out whether we are the boot-master or not...
	 */
flashBooting:
	read_cp15	r1, CP15_MPIDR
	tst	r1, #(1 << 31)			/* Register has MPCore format?  0 means no */
	beq	masterCore
	tst	r1, #(1 << 30)			/* 1 means this is a uni-processor ARM */
	bne	masterCore
	ands	r1, #0xFF			/* Is ARM core 0 (in affinity level 0)? */
	beq	masterCore
	read_cp15	r1, CP15_PERIPHBASE
	cmp	r1, #0				/* 0 means this is a uni-processor ARM */
	beq	masterCore

	/*
	 * So we are presumably, a SLAVE core running from FLASH.
	 * Capture the slave(s) in a holding pen, when booting from FLASH.
	 * We need the I-caches to be activated, to ensure the entirety
	 * of this holding pen resides in I-caches as, in parallel the
	 * boot-master core should be executing the "pokeloop" interpreter,
	 * and our EMI (FLASH) and LMI (RAM) may "disappear" from under our feet!
	 */
flashSlaveHoldingPen:
	dsb
	isb
	/*
	 * QQQ - We need to decide when to exit from this pen!
	 * For the time being, we will simply tell *all* slave
	 * cores just to loop forever …
	 * Clearly this is wrong, but it will suffice for now.
	 */
1:	wfe
	b	1b


masterCore:
	/*
	 * ARM core 0 (the boot-master) continues here.
	 * We also assume we are booting from FLASH, and that we
	 * need to execute the "pokeloop" interpreter, to complete
	 * the low-level initialization - i.e. call init_ram().
	 * Our caller should relocate us into RAM on our return.
	 */
	ldr	r0, init_ram_pic
	adr	r1, init_ram_pic
	add	r0, r1, r0		/* r0 = &init_ram(); */
	bx	r0			/* execute the "pokeloop" interpreter */
					/* The callee, should return to our caller */


	/*
	 * Variables …
	 */
	.balign 4
init_ram_pic:	.word init_ram - .
here:		.word .
