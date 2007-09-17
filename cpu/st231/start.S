/*
 * (C) Copyright 2005 STMicroelectronics.
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

#include <config.h>
#include <asm/ctrlregdef.h>

#include "macros.h"

#define	PSW_TLB_ENABLE_LO 2
#define PSW_CACHE_ENABLE (1 << PSW_TLB_ENABLE_LO)

#define LATENCY_1()
#define LATENCY_2()
#define LATENCY_3()
#define LATENCY_4()

#define DEBUG 0

#if DEBUG
#define LED_BLINK(D) CALL_FN_NO_STACK(led_blink,D)
#else
#define LED_BLINK(D)
#endif


/*
 * Macro to purge the caches
 */
#define PURGE_CACHES()\
	mov     $r1 = $r0;\
	;; ;\
1:      prgset  0[$r1];\
	  add     $r1 = $r1,32;\
	;; ;\
	cmplt   $b0 = $r1,8*1024;\
	;; ;\
	br      $b0,1b;\
	;; ;\
	sync 			; \
	;; ;\
	prgins ;\
	;; ;\
	syncins ;\
	;;

/**********************************************************
*
*	MAIN ENTRY POINT
*
**********************************************************/

	.text
	.balign         8
	.global _start
	.import start_st200boot
	.import led_blink
	.import _memory_setup_table
	.import _memory_setup_end
	.import _xpu_setup_table

_start:
	/* Make sure we grab the trap handler */

	ldw	$r2 = HANDLER_PC[$r0]
	;;
	stw	SCRATCH3[$r0] = $r2
	;;
	GET_PIC_PTR(trap_handler, $r2)
	;;
	stw	HANDLER_PC[$r0] = $r2
	;;

	/* We need to clear all TLB entries (indicies 0 to 63) to ensure
	   no undefined values are left after reset.
	 */

	mov     $r1 = $r0
	GET_PIC_PTR(tlb_index_addr, $r2)
	ldw     $r2 = 0[$r2]
	GET_PIC_PTR(tlb_entry0_addr, $r3)
	ldw     $r3 = 0[$r3]
	;;
1:
	/* Set index */
	stw     0[$r2] = $r1
	;;

	/* Write 0 to disable */
	stw     0[$r3] = $r0

	/* Add 1 and loop */
	add     $r1 = $r1, 1
	;;
	cmpeq   $b0 = $r1, 64
	;;
	brf     $b0, 1b
	;;
	/* If we didn't branch, we're finished. */


	/*
	 * Program up the MMU
	 */

	GET_PIC_PTR(_xpu_mmu_setup_table, $r0.16) /* R16: poke table addr*/
	CALL_FN_NO_STACK(poke_loop, $r0.16)
	;;
	mov     $r1 = (1 << PSW_TLB_ENABLE_LO)
	;;
	pswset  $r1
	;;

init_memory:
	/* Skip if not in flash */
	GET_PIC_PTR(_start, $r1)
	cmpleu	$b0 = $r1, CFG_SDRAM_BASE
	;;
	br	$b0, 1f
	;;
	cmpltu	$b0 = $r1, (CFG_SDRAM_BASE + CFG_SDRAM_SIZE)
	;;
	br	$b0, relocate_code
	;;
1:
	LED_BLINK(2)
	/*
	 * Read the user supplied poke table to get it into cache
	 */
	GET_PIC_PTR(_memory_setup_table, $r0.6)     /* R6 = start address */
	GET_PIC_PTR(_memory_setup_table_end, $r0.2) /* R2 = end address */
	mov $r0.5 = $r0.6                           /* R5 = scratch data ptr */
	;;

pull_table_in:
	ldw     $r0.4 = (0)[$r0.5]
	  add   $r0.5 = $r0.5, 32
	;;
	cmpge   $b0.0 = $r0.5, $r0.2
	;;
	brf     $b0.0, pull_table_in
	;;

	/*
	 * Its now safe to call the poke loop with real data, since it
	 * and its associated data table are in on-chip caches. Setting
	 * up the memory interfaces may cause the EMI (where this FLASH
	 * image resides) to briefly hang - but the CPU will be safely
	 * executing from cache should this happen.
	 * This also configures the DPU & IPU for us.
	 */
	CALL_FN_NO_STACK(poke_loop, $r0.6)      /* R6 still points to table */
	;;
relocate_code:
	LED_BLINK(4)

	GET_PIC_PTR(_start, $r1)
	mov	$r2 = _start
	;;
	cmpeq	$b0 = $r2, $r1
	;;
	br	$b0, init_sp	 /* Skip copy if loaded at target address */
	;;
	LED_BLINK(4)

	;;
1:	ldw	$r4 = 0[$r1]
	  add $r1 = $r1, 4
	;;
	stw	0[$r2] = $r4
	  add $r2 = $r2, 4
	;;
	cmpeq	$b0 = $r2, _uboot_end_data
	;;
	brf	$b0, 1b
	;;

	PURGE_CACHES()
	;;

init_sp:
	LED_BLINK(1)

	GET_PIC_PTR(stack_addr, $r2)
	ldw     $r12 = 0[$r2]
	;;
	LED_BLINK(1)
	;;

init_bss:
	LED_BLINK(2)
	GET_PIC_PTR(bss_start, $r2)
	ldw     $r2 = 0[$r2]
	GET_PIC_PTR(bss_end, $r3)
	ldw     $r3 = 0[$r3]
	;;
1:
	stb     0[$r2] = $r0
	add     $r2 = $r2, 1
	;;
	cmpeq   $b0 = $r2, $r3
	;;
	brf     $b0, 1b
	;;
	/* If we didn't branch, we're finished. */

	LED_BLINK(8)

	/* call main entry */

	GET_PIC_PTR(start_st200boot_addr, $r1)
	ldw     $r63 = 0[$r1]
	;;
	call	$r63 = $r63
	;;
spin:   br	$b0.0, spin        /* Should not get here */
	;;

#if DEBUG
	/*****************************
	*   Trap handler -
	*	for debugger turns off tlb and branches to old handler
	*****************************/

	.global trap_handler
trap_handler:
	stw	SCRATCH2[$r0] = $r63
	;;
	mov     $r1 = (1 << PSW_TLB_ENABLE_LO)
	;;
	pswclr  $r1
	;;
	ldw	$r63 = SCRATCH3[$r0]
	;;
	goto	$r63
	;;
#else
	/*****************************
	*   Trap handler -
	*	reset the chip
	*****************************/

	.global trap_handler
trap_handler:
	CALL_FN_NO_STACK(do_reset, $r0)
	;;
	br	$b0.0, spin        /* Should not get here */
	;;
#endif

/*
 * Poke loop routine.
 * Routine to setup peripheral registers.
 * It is executed from within the I-cache,
 * with all its data in the D-cache.
 * It is structured so that the 1st pass through the code
 * pulls it all into I-cache.
 * On entry $r0.16 points to the table to use.
 */

.balign 32

poke_loop:
	ldw     $r0.1 = 0[$r0.16]       /* opcode */
	;;
	ldw     $r0.2 = 4[$r0.16]       /* address */
	;;
	stw	SCRATCH1[$r0] = $r0.2
	;;
	ldw     $r0.3 = 8[$r0.16]       /* value */
	;;
	ldw     $r0.4 = 12[$r0.16]      /* possible extra value */
	  add   $r0.16 = $r0.16, 12
	;;
	cmpeq   $b0.0 = $r0.1, 4        /* 4 byte write? */
	cmpeq   $b0.1 = $r0.1, 2        /* 2 byte write? */
	cmpeq   $b0.2 = $r0.1, 1        /* 1 byte write? */
	cmpeq   $b0.3 = $r0.1, 0        /* End marker? */
	;;
	cmpeq   $b0.4 = $r0.1, 5        /* 4 byte OR? */
	cmpeq   $b0.5 = $r0.1, 6        /* 4 byte UPDATE? */
	cmpeq   $b0.6 = $r0.1, 7        /* 4 byte WHILE & != */
	;;
	brf     $b0.0, 1f               /* ... not 4 byte write */
	;;
	stw     (0)[$r0.2] = $r0.3
	;;
1:      brf     $b0.1, 1f               /* ... not 2 byte write */
	;;
	sth     (0)[$r0.2] = $r0.3
	;;
1:      brf     $b0.2, 1f               /* ... not 1 byte write */
	;;
	stb     (0)[$r0.2] = $r0.3
	;;
1:      brf     $b0.3, 1f               /* ... not end marker */
	;;
	return  $r0.63                  /* Exit point */
	;;
1:      brf     $b0.4, 1f               /* ... not 4 byte OR */
	;;
	ldw     $r0.4 = (0)[$r0.2]
	;;
	or      $r0.4 = $r0.4, $r0.3
	;;
	stw     (0)[$r0.2] = $r0.4
	;;
1:      brf     $b0.5, 1f               /* ... not 4 byte update */
	;;
	ldw     $r0.5 = (0)[$r0.2]
	;;
	and     $r0.5 = $r0.5, $r0.3
	;;
	or      $r0.5 = $r0.5, $r0.4
	  add   $r0.16 = $r0.16, 4
	;;
	stw     (0)[$r0.2], $r0.5
	;;
1:      brf     $b0.6,1f                /* ... not while */
	;;
2:      ldw     $r0.5, (0)[$r0.2]
	;;
	and     $r0.5 = $r0.5, $r0.3
	;;
	cmpeq   $b0.7 = $r0.5, $r0.4
	;;
	brf     $b0.7, 2b
	;;
	add     $r0.16 = $r0.16, 4
	;;
1:      mov     $r0.10,0x10000           /* small delay */
	;;
2:      add     $r0.10 = $r0.10, -1
	;;
	cmpeq   $b0.0 = $r0.10, $r0.0
	;;
	brf     $b0.0, 2b
	;;
	br      $b0.0, poke_loop
	;;


/************* mmu_off:	Purge caches and diable the mmu *************/

	.global mmu_off
mmu_off:
	PURGE_CACHES()
	;;
	mov     $r1 = (1 << PSW_TLB_ENABLE_LO)
	;;
	pswclr  $r1
	;;
	goto	$r63
	;;

/****************  Constants ****************************************/

tlb_index_addr:		.long 0xffffff80
tlb_entry0_addr:	.long 0xffffff78
trap_handler_addr:	.long 0xffffffe0

target_addr:		.long TEXT_BASE
stack_addr:		.long TEXT_BASE - (CFG_GBL_DATA_SIZE + CFG_MALLOC_LEN + CFG_BOOTPARAMS_LEN + 16)
start_st200boot_addr:   .long start_st200boot
bss_start:		.long __bss_start
bss_end:		.long __bss_end
