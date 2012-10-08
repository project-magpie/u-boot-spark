/*
 *  Startup Code for SPEAr13xx
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

#include <asm-offsets.h>
#include <config.h>
#include <asm/system.h>
#include <linux/linkage.h>

.syntax	unified

.globl _start
_start:
	.word 0xe59f0000 	@ ldr	r0, [pc, #0]	; <thumb_loc>
	.word 0xe12fff10 	@ bx	r0

thumb_loc:
#ifdef __thumb__
	.word thumb + 1
#else
	.word thumb
#endif

#ifdef __thumb__
.thumb
#endif

/*
 * the actual reset code (Thumb compatible)
 * Startup Code (reset vector)
 *
 * Critical initializations already taken place in BootROM.
 * So, these are not taken care in Xloader
 * 1. Relocation to RAM
 * 2. Initializing stacks
 */

/******************************************************************************
	PLACE ALL INITIALIZATION CODE ONLY AFTER THIS LOCATION
 ******************************************************************************/

thumb:

/*
 * Xloader has to return back to BootROM in a few cases.
 * eg. Ethernet boot, UART boot, USB boot
 * Saving registers for returning back
 */
	push	{r1-r12, lr}

	/* Relocate data if needed and Clear BSS */
	ldr	r0, =__bss_start
	ldr	r1, =__bss_end
	mov	r2, #0x00000000		/* clear			    */

clbss_l:
	cmp	r0, r1			/* clear loop... */
	bhs	clbss_e			/* if reached end of bss, exit */
	str	r2, [r0]
	add	r0, r0, #4
	b	clbss_l
clbss_e:

	bl	cpu_init_cp15
	bl	cpu_init_crit

	bl	spl_boot
	pop	{r1-r12, pc}

/*************************************************************************
 *
 * cpu_init_cp15
 *
 * Setup CP15 registers (cache, MMU, TLBs)
 *
 *************************************************************************/
ENTRY(cpu_init_cp15)
	/*
	 * Invalidate L1 I/D
	 */
	mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr	p15, 0, r0, c7, c5, 6	@ invalidate BP array
	mcr     p15, 0, r0, c7, c10, 4	@ DSB
	mcr     p15, 0, r0, c7, c5, 4	@ ISB

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00000002	@ set bit 1 (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-cache
	mcr	p15, 0, r0, c1, c0, 0
	mov	pc, lr			@ back to my caller
ENDPROC(cpu_init_cp15)

/*************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************/
ENTRY(cpu_init_crit)
	/*
	 * Jump to board specific initialization...
	 * The Mask ROM will have already initialized
	 * basic memory. Go here to bump up clock rate and handle
	 * wake up conditions.
	 */
	push	{lr}
	bl	lowlevel_init		@ go setup pll,mux,memory
	pop	{pc}
ENDPROC(cpu_init_crit)
