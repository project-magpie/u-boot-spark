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
	.globl		prepare_hpen_for_linux
	.type		prepare_hpen_for_linux, %function
	.globl		reset_timer			/* QQQ - DELETE ? */
	.type		reset_timer, %function
	.globl		reset_cpu			/* QQQ - DELETE ? */
	.type		reset_cpu, %function


	/*
	 * Some macros for defining the Holding-Pen area in SRAM.
	 */
#define HPEN_KEEP_LOOPING_VALUE		0xFFFFFFFF
#define HPEN_JUMP_POINTER		0x00		/* FLASH to SRAM */
#define HPEN_JUMP_POINTER2		0x04		/* SRAM to linux */
#define HPEN_ENTRY_POINT		0x20


	.balign 4
reset_timer:		/* QQQ - TO IMPLEMENT PROPERLY */
	bx	lr	/* return immediately! */

reset_cpu:		/* QQQ - TO IMPLEMENT PROPERLY */
	bx	lr	/* return immediately! */


	/*
	 * extern void prepare_hpen_for_linux( void (**stm_secondary_startup)(void) );
	 *
	 * We prepare to pass all the slave cores to linux.
	 * We are given a pointer into which the function pointer will be written.
	 * We ensure that pointer is initialized with HPEN_KEEP_LOOPING_VALUE.
	 * We update jump pointer #2, to point to that pointer.
	 */
prepare_hpen_for_linux:
	ldr	r1, =HPEN_KEEP_LOOPING_VALUE	/* sentinel value to keep looping */
	str	r1, [r0]			/* write sentinel to function pointer */
	dsb					/* Data Synchronization Barrier */

	ldr	r2, hpen_base			/* r2 = CONFIG_STM_SRAM_HOLDING_PEN */
	str	r0, [r2, #HPEN_JUMP_POINTER2]	/* write indirect pointer to jump pointer #2 */
	dsb					/* Data Synchronization Barrier */

	bx	lr				/* return to my caller */



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
	 *
	 * We capture all the slaves in our "FLASH" holding pen first.
	 * The slaves will write the sentinel value HPEN_KEEP_LOOPING_VALUE
	 * into the "jump pointer", and then will use Wait-For-Event to wait.
	 * When woken, they will read the "jump pointer", and see if its value
	 * has changed from HPEN_KEEP_LOOPING_VALUE. If not, they go back to waiting.
	 * If the "jump pointer" has changed, then the slave will jump to
	 * the address pointed to by the "jump pointer" - which should be in SRAM.
	 *
	 * Note: The "jump pointer" itself should also be located in SRAM,
	 * so we do not need any SDRAM available, in order to to transition
	 * from the initial FLASH Holding-Pen into the SRAM Holding-Pen.
	 */
flashSlaveHoldingPen:
	/*
	 * Each (and every) slave should initialize the "sentinel" value for
	 * the (global) "jump pointer", to indicate we have *not* been kicked yet.
	 * There is only one "jump pointer", and all slaves will use it.
	 * All slaves will initialize it with the exact *same* value, so
	 * the execution order of the slaves is unimportant. However, each
	 * slave must write to it, before it reads from it … just in case!
	 */
	ldr	r5, hpen_base			/* r5 = CONFIG_STM_SRAM_HOLDING_PEN */
	ldr	r1, =HPEN_KEEP_LOOPING_VALUE	/* sentinel value to keep looping */
	str	r1, [r5, #HPEN_JUMP_POINTER]	/* write sentinel to the jump pointer */
	dsb					/* Data Synchronization Barrier */

1:	wfe					/* Wait for an Event */
	ldr	r1, [r5, #HPEN_JUMP_POINTER]	/* read the "jump pointer" */
	cmp	r1, #HPEN_KEEP_LOOPING_VALUE	/* Have we been "kicked" yet ? */
	beq	1b				/* No, so wait some more… */

	/*
	 * Yes, if we get here, we have been "kicked", and the value in the
	 * "jump pointer" has been changed (presumably by the master core).
	 */

	/*
	 * Invalidate caches as master core may have copied into addresses
	 * this core has performed speculative reads from.
	 */
//QQQ	Do we really need to do anything here - probably not!

	/*
	 * Okay, we have now been "kicked" by the master core, and
	 * we should now transition from the initial FLASH Holding-Pen,
	 * to the SRAM Holding-Pen (i.e. jump to sramSlaveHoldingPen).
	 * Simply, jump to the address we just read from "jump pointer".
	 * We execute some memory synchronization barriers first.
	 */
	dsb
	isb
	bx	r1				/* Off we go! */


masterCore:
	/*
	 * ARM core 0 (the boot-master) continues here.
	 */
	mov	r10, lr			/* Stash the LR somewhere safe */

	/*
	 * Initialize a few "convenience" variables.
	 */
	ldr	r5, hpen_base		/* r5 = CONFIG_STM_SRAM_HOLDING_PEN */
	ldr	r4, =HPEN_ENTRY_POINT
	add	r4, r4, r5		/* r4 = CONFIG_STM_SRAM_HOLDING_PEN + HPEN_ENTRY_POINT */

	/*
	 * write HPEN_KEEP_LOOPING_VALUE to HPEN_JUMP_POINTER2
	 */
	ldr	r1, =HPEN_KEEP_LOOPING_VALUE	/* sentinel value to keep looping */
	str	r1, [r5, #HPEN_JUMP_POINTER2]	/* write sentinel to jump pointer #2 */

	/*
	 * Re-locate the SRAM Holding-Pen code, to reside in SRAM/ERAM.
	 */
copyHoldingPen:
	ldr	r0, hpen_start_pic
	adr	r1, hpen_start_pic
	add	r0, r1, r0		/* r0 = sramSlaveHoldingPen */
	mov	r1, r4			/* r1 = CONFIG_STM_SRAM_HOLDING_PEN + HPEN_ENTRY_POINT */
	ldr	r2, hpen_size
	add	r2, r2, r0		/* r2 = sramSlaveHoldingPenEnd */

	/*
	 * Copy 16 bytes at a time, from FLASH to SRAM.
	 */
copyHoldingPenLoop:
	ldmia	r0!, {r6 - r9}		/* copy from source address [r0] */
	stmia	r1!, {r6 - r9}		/* copy to   target address [r1] */
	cmp	r0, r2			/* until source end address [r2] */
	ble	copyHoldingPenLoop

	/*
	 * The master is now ready to "kick" each slave, and transition them
	 * all from the initial FLASH Holding-Pen, into the SRAM Holding-Pen.
	 * We need to write the address of sramSlaveHoldingPen to the jump
	 * pointer, over-writing the initial HPEN_KEEP_LOOPING_VALUE sentinel.
	 *
	 * WARNING! - WARNING! - WARNING! - WARNING! - WARNING! - WARNING!
	 * We make an important, and fundamental assumption here!
	 * We assume that *all* the slaves have *already* written to the
	 * jump pointer (with HPEN_KEEP_LOOPING_VALUE), and that no
	 * slaves (or anything else for that matter) will subsequently
	 * write to the location of the "jump pointer" in SRAM.
	 * Simply put - we expect all the slaves to be waiting in "WFE".
	 * QQQ - Do we need to add a delay to mitigate this risk???
	 * NOTE: Using a debugger may violate this assumption.
	 * WARNING! - WARNING! - WARNING! - WARNING! - WARNING! - WARNING!
	 */
kickSlavesToSramHoldingPen:
	str	r4, [r5, #HPEN_JUMP_POINTER]
	/*
	 * Synchronisation Barriers … before we call the relocated code!
	 */
	dsb
	isb
	/*
	 * Finally, we can now "kick" all the slaves by explicitly sending
	 * them all an event, from which they should all wake up, at which
	 * point, they should all re-read the "jump pointer", and notice
	 * that is is no longer yielding the sentinel HPEN_KEEP_LOOPING_VALUE!
	 */
	sev				/* Wakey! Wakey! */

	/*
	 * ARM core 0 (the boot-master) continues here.
	 */

	/*
	 * We assume we are booting from FLASH, and that we
	 * need to execute the "pokeloop" interpreter, to complete
	 * the low-level initialization - i.e. call init_ram().
	 * Our caller should relocate us into RAM on our return.
	 */
do_init_ram:
	ldr	r0, init_ram_pic
	adr	r1, init_ram_pic
	add	r0, r1, r0		/* r0 = &init_ram(); */
	blx	r0			/* execute the "pokeloop" interpreter */

	/*
	 * The master core's low-level initialization is done!
	 */
	mov	pc, r10			/* finally, we return to our caller */


	/*
	 * Variables …
	 */
	.balign 4
init_ram_pic:	.word init_ram - .
here:		.word .
hpen_base:	.word CONFIG_STM_SRAM_HOLDING_PEN
hpen_start_pic:	.word sramSlaveHoldingPen - .
hpen_size:	.word sramSlaveHoldingPenEnd - sramSlaveHoldingPen


	/*
	 * So we are presumably, a SLAVE core, now running in SRAM.
	 * We will now capture the slave(s) in an SRAM Holding-Pen.
	 * They will be held here until we are about to pass control
	 * to the "payload" image to be booted - typically linux.
	 */
	.balign 32	/* Cache-line aligned */
sramSlaveHoldingPen:
#if 0
	/*
	 * Enable the signalling of interrupts. i.e. ICCICR = 1
	 * This is needed to wake up from an "WFI" instruction.
	 * NOTE: we do not need to do this, if we use "WFE" instead!
	 */
	ldr	r1, =1				/* Enable signalling of interrupts by the CPU interfaces */
	read_cp15	r2, CP15_PERIPHBASE	/* r2 = CP15_PERIPHBASE */
	str	r1, [r2, #A9MP_GIC_CPU_INTERFACE_ICCICR]	/* ICCICR = ENABLE */
	dsb					/* Data Synchronization Barrier */
#endif

	/* QQQ - we ideally want to do a "WFI", and not a "WFE" here! */
1:	wfe					/* Wait for an Event (or an Interrupt) */
	ldr	r1, [r5, #HPEN_JUMP_POINTER2]	/* read jump pointer #2 */
	cmp	r1, #HPEN_KEEP_LOOPING_VALUE	/* Has it been changed yet? */
	beq	1b				/* No, so wait some more… */
	ldr	r2, [r1]			/* dereference the pointer */
	cmp	r2, #HPEN_KEEP_LOOPING_VALUE	/* Has it been changed yet? */
	beq	1b				/* No, so wait some more… */

	/*
	 * Yes, if we get here, we have been "kicked", and the value in the
	 * "jump pointer" has been changed (presumably by the master core).
	 * Furthermore, the contents of the address it points to has also
	 * been modified (presumably by the master core running linux).
	 * This should be the function pointer for the secondary entry
	 * point for all the slave cores, to enter the linux kernel.
	 */

	/*
	 * Invalidate caches as master core may have copied into addresses
	 * this core has performed speculative reads from.
	 */
//QQQ	Do we really need to do anything here - probably not!

	/*
	 * Okay, we have now been "kicked" by the master core, and
	 * we should now transition from the SRAM Holding-Pen, to the
	 * linux kernel (i.e. jump to virt_to_phys(stm_secondary_startup)).
	 * Simply, jump to the indirect address we read via "jump pointer".
	 * We execute some memory synchronization barriers first.
	 */
	dsb
	isb
	bx	r2				/* SMP, here we come! */
sramSlaveHoldingPenEnd:
