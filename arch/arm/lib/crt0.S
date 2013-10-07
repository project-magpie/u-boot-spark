/*
 *  crt0 - C-runtime startup Code for ARM U-Boot
 *
 *  Copyright (c) 2012  Albert ARIBAUD <albert.u.boot@aribaud.net>
 *
 *  Copyright (c) 2013  STMicroelectronics Limited.
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
#include <asm-offsets.h>

/*
 * This file handles the target-independent stages of the U-Boot
 * start-up where a C runtime environment is needed. Its entry point
 * is _main and is branched into from the target's start.S file.
 *
 * _main execution sequence is:
 *
 * 1. Set up initial environment for calling board_init_f().
 *    This environment only provides a stack and a place to store
 *    the GD ('global data') structure, both located in some readily
 *    available RAM (SRAM, locked cache...). In this context, VARIABLE
 *    global data, initialized or not (BSS), are UNAVAILABLE; only
 *    CONSTANT initialized data are available.
 *
 * 2. Call board_init_f(). This function prepares the hardware for
 *    execution from system RAM (DRAM, DDR...) As system RAM may not
 *    be available yet, , board_init_f() must use the current GD to
 *    store any data which must be passed on to later stages. These
 *    data include the relocation destination, the future stack, and
 *    the future GD location.
 *
 * (the following applies only to non-SPL builds)
 *
 * 3. Set up intermediate environment where the stack and GD are the
 *    ones allocated by board_init_f() in system RAM, but BSS and
 *    initialized non-const data are still not available.
 *
 * 4. Call relocate_code(). This function relocates U-Boot from its
 *    current location into the relocation destination computed by
 *    board_init_f().
 *
 * 5. Set up final environment for calling board_init_r(). This
 *    environment has BSS (initialized to 0), initialized non-const
 *    data (initialized to their intended value), and stack in system
 *    RAM. GD has retained values set by board_init_f(). Some CPUs
 *    have some work left to do at this point regarding memory, so
 *    call c_runtime_cpu_setup.
 *
 * 6. Branch to either nand_boot() or board_init_r().
 */

/*
 * declare nand_boot() or board_init_r() to jump to at end of crt0
 */

#if defined(CONFIG_NAND_SPL)

.globl nand_boot

#elif ! defined(CONFIG_SPL_BUILD)

.globl board_init_r

#endif

/*
 * start and end of BSS
 */

.globl __bss_start
.globl __bss_end__

/*
 * entry point of crt0 sequence
 */

.global _main

_main:

/*
 * Set up initial C runtime environment and call board_init_f(0).
 */

#if defined(CONFIG_STM)
	/*
	 * As described above, it is expected that all ARM systems
	 * will call board_init_f() whilst executing directly out of
	 * FLASH, before any relocations, or even the C run-time
	 * environment is properly set up!
	 *
	 * However, for some SoCs from STMicroelectronics, this is
	 * a problem, as when booting from SPI serial FLASH, the SPI
	 * Serial Flash "boot-controller" on some of these SoCs do not
	 * support LD1 (8-bit accesses) or LD2 (16-bit accesses)
	 * operations on the STbus. Hence, it is not possible to
	 * call board_init_f() (and all of its callee functions),
	 * if there are any "char" or "short" accesses to the
	 * SPI serial FLASH "boot-controller".
	 *
	 * To solve this problem, we will "temporary" relocate
	 * U-Boot from FLASH to RAM, before we call board_init_f().
	 * That is, we will perform an additional "copy" (the
	 * "first copy") to RAM, if we are booting directly from
	 * FLASH. For simplicity, and for consistency, we will use
	 * this technique for all FLASH booting types (e.g. including
	 * boot-from-NAND), even if this is not strictly necessary,
	 * for all SoCs from STMicroelectronics.
	 */

	/*
	 * Have we been loaded to the correct address (i.e. via GDB to
	 * CONFIG_SYS_TEXT_BASE), or are we running from the "wrong"
	 * location (i.e. booting directly from FLASH)?
	 * We work out where we are running from - FLASH or RAM!
	 */
	adr	r0, where		/* r0 = current position of code */
	ldr	r1, where		/* r1 = &where; (link-time) */
	subs	r3, r1, r0		/* test if we are in the correct place */
	beq	2f			/* branch, if in the correct place */

	/*
	 * If we get here, then we assume are in the "wrong" place,
	 * and we further assume we are booting directly from FLASH,
	 * which is typically at address 0x00000000.
	 * So, we will copy both the .text and .data sections
	 * (as well as the relocation tables) from FLASH into RAM.
	 *
	 * Note: This is a simple copy - no "fix-ups" will be performed
	 * during this copy. The idea is just to copy the aforementioned
	 * raw binary sections ("as is") into the address the binary was
	 * originally linked at (i.e. CONFIG_SYS_TEXT_BASE) in RAM.
	 *
	 * Note: After board_init_f() returns, we will still call
	 * relocate_code(), which will copy the code again to RAM,
	 * but this 2nd copy will *also* perform the relocation "fix-ups".
	 * Typically the 2nd copy will be to a different destination
	 * address, from the first copy.
	 */
	ldr	r1, where_offset	/* r1 = where - CONFIG_SYS_TEXT_BASE */
	sub	r0, r0, r1		/* r0 = start of source address (i.e. _start) */
	add	r1, r0, r3		/* r1 = start of destination address */
	ldr	r2, end_offset
	ldr	r2, [r2, r0]		/* total number of bytes to copy */
	add	r2, r0, r2		/* r2 = end of source address */

	/*
	 * 1st copy - copy 16 bytes at a time, from FLASH to RAM.
	 */
1:	ldmia	r0!, {r4 - r7}		/* copy from source address [r0] */
	stmia	r1!, {r4 - r7}		/* copy to   target address [r1] */
	cmp	r0, r2			/* until source end address [r2] */
	ble	1b

	/*
	 * We will now "jump" to the version in RAM we have just copied.
	 * This allows the code in board_init_f() (before relocation) to
	 * access both code and data in the aforementioned copied sections
	 * to be correct - this is important!
	 */
	adr	r0, 2f			/* jump to immediately after this chunk */
	add	r3, r0, r3		/* add on the copy's displacement */
	bx	r3			/* jump into the copy! */
2:					/* All done - carry on in RAM */
#endif	/* CONFIG_STM */

#if defined(CONFIG_NAND_SPL)
	/* deprecated, use instead CONFIG_SPL_BUILD */
	ldr	sp, =(CONFIG_SYS_INIT_SP_ADDR)
#elif defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_STACK)
	ldr	sp, =(CONFIG_SPL_STACK)
#else
	ldr	sp, =(CONFIG_SYS_INIT_SP_ADDR)
#endif
	bic	sp, sp, #7	/* 8-byte alignment for ABI compliance */
	sub	sp, #GD_SIZE	/* allocate one GD above SP */
	bic	sp, sp, #7	/* 8-byte alignment for ABI compliance */
	mov	r8, sp		/* GD is above SP */
	mov	r0, #0
	bl	board_init_f

#if ! defined(CONFIG_SPL_BUILD)

/*
 * Set up intermediate environment (new sp and gd) and call
 * relocate_code(addr_sp, gd, addr_moni). Trick here is that
 * we'll return 'here' but relocated.
 */

	ldr	sp, [r8, #GD_START_ADDR_SP]	/* r8 = gd->start_addr_sp */
	bic	sp, sp, #7	/* 8-byte alignment for ABI compliance */
	ldr	r8, [r8, #GD_BD]		/* r8 = gd->bd */
	sub	r8, r8, #GD_SIZE		/* new GD is below bd */

	adr	lr, here
	ldr	r0, [r8, #GD_RELOC_OFF]		/* lr = gd->start_addr_sp */
	add	lr, lr, r0
	ldr	r0, [r8, #GD_START_ADDR_SP]	/* r0 = gd->start_addr_sp */
	mov	r1, r8				/* r1 = gd */
	ldr	r2, [r8, #GD_RELOCADDR]		/* r2 = gd->relocaddr */
	b	relocate_code
here:

/* Set up final (full) environment */

	bl	c_runtime_cpu_setup	/* we still call old routine here */

	ldr	r0, =__bss_start	/* this is auto-relocated! */
	ldr	r1, =__bss_end__	/* this is auto-relocated! */

	mov	r2, #0x00000000		/* prepare zero to clear BSS */

clbss_l:cmp	r0, r1			/* while not at end of BSS */
	strlo	r2, [r0]		/* clear 32-bit BSS word */
	addlo	r0, r0, #4		/* move to next */
	blo	clbss_l

	bl coloured_LED_init
	bl red_led_on

#if defined(CONFIG_NAND_SPL)

	/* call _nand_boot() */
	ldr     pc, =nand_boot

#else

	/* call board_init_r(gd_t *id, ulong dest_addr) */
	mov	r0, r8			/* gd_t */
	ldr	r1, [r8, #GD_RELOCADDR]	/* dest_addr */
	/* call board_init_r */
	ldr	pc, =board_init_r	/* this is auto-relocated! */

#endif

	/* we should not return here. */

#endif

#if defined(CONFIG_STM)
	/*
	 * Constants …
	 */
	.balign 4
where:		.word	.
where_offset:	.word	where    - CONFIG_SYS_TEXT_BASE
end_offset:	.word	_end_ofs - CONFIG_SYS_TEXT_BASE

#endif	/* CONFIG_STM */
