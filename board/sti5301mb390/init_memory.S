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

/*
 * The poke table is a series of long words, in the format
 *
 *       opcode, address, value1, (value2)
 *
 * An opcode of 0 marks the table end
 */

#define LOG2_LXTLB_MIN_PAGE_SIZE 0xd

#define WHILE_NE(A, AND, VAL)   .long 7, A, AND, VAL
#define UPDATE_LONG(A, AND, OR) .long 6, A, AND, OR
#define OR_LONG(A, V)           .long 5, A, V
#define POKE_LONG(A, V)         .long 4, A, V
#define POKE_SHORT(A, V)        .long 2, A, V
#define POKE_CHAR(A, V)         .long 1, A, V
#define END_MARKER              .long 0, 0, 0

	.text
	.balign 32
.global _memory_setup_table
_memory_setup_table:

#include "mb390_memory_std.S"

	END_MARKER

.global _memory_setup_table_end
_memory_setup_table_end:
.global _xpu_mmu_setup_table
_xpu_mmu_setup_table:
	/*
	 * Add TLB regions:
	 *  - LMI, cached
	 *  - Flash, cached
	 */
	POKE_LONG(0xFFFFFF50, 0x3) /* Flush instruction and data TLBs */

	/* Set up identity RAM region as cached, rwx, 256MB */
	POKE_LONG(0xFFFFFF80, 0x0) /* Set TLB index to 0 */
	POKE_LONG(0xFFFFFF70, (0xC0000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0xC0000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x7000 |    /* User rwx */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x10000 |   /* Cached policy */
			      0x300000)   /* 256MB page size */


	/* Set up identity flash region as cached, rwx, 256MB */
	POKE_LONG(0xFFFFFF80, 0x1) /* Set TLB index to 1 */
	POKE_LONG(0xFFFFFF70, (0x70000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x70000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x7000 |    /* User rwx */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |	  /* Uncached policy */
			      0x300000)   /* 256MB page size */

	/* Set up an identity uncached peripheral regions to cover the registers
	   we'll be poking in the poke table.
	 */
	POKE_LONG(0xFFFFFF80, 0x2) /* Set TLB index to 2 */
	POKE_LONG(0xFFFFFF70, (0x20000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x20000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x6000 |    /* User rw */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |       /* Uncached policy */
			      0x300000)   /* 256MB page size */

	POKE_LONG(0xFFFFFF80, 0x3) /* Set TLB index to 3 */
	POKE_LONG(0xFFFFFF70, (0xE0000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0xE0000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x6000 |    /* User rw */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |       /* Uncached policy */
			      0x100000)   /* 8KB page size */

	POKE_LONG(0xFFFFFF80, 0x4) /* Set TLB index to 4 */
	POKE_LONG(0xFFFFFF70, (0x41100000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x41100000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x6000 |    /* User rw */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |       /* Uncached policy */
			      0x100000)   /* 8KB page size */

	POKE_LONG(0xFFFFFF80, 0x5) /* Set TLB index to 5 */
	POKE_LONG(0xFFFFFF70, (0x30000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x30000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x6000 |    /* User rw */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |       /* Uncached policy */
			      0x100000)   /* 8KB page size */

	POKE_LONG(0xFFFFFF80, 0x6) /* Set TLB index to 6 */
	POKE_LONG(0xFFFFFF70, (0x41400000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x41400000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x6000 |    /* User rw */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |       /* Uncached policy */
			      0x100000)   /* 8KB page size */

	POKE_LONG(0xFFFFFF80, 0x7) /* Set TLB index to 6 */
	POKE_LONG(0xFFFFFF70, (0x43020000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x43020000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x6000 |    /* User rw */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |       /* Uncached policy */
			      0x100000)   /* 8KB page size */


	POKE_LONG(0xFFFFFF50, 0x3) /* Flush instruction and data TLBs */
	END_MARKER

	/***********************
	/* LED Blinking routing
	/***********************/

#define LED_BASE 0x41100000
	/* Led_Blink the LED a few times */
	.global led_blink
led_blink:
	/* Save call address in SCRATCH1 */
	mov   $r8 = SCRATCH1
	;;
	stw   0[$r8] = $r63
	;;
	mov   $r8 = LED_BASE
	;;
	shl  $r16 = $r16, 24
	;;
	mov   $r9 = 8
	mov   $r10 = 0
	mov   $r11  = 1
	;;
1:	sth	0[$r8] = $r10
	  xor	$r10 = $r10, 1
	  sub	$r9 = $r9, $r11
	  mov	$r62 = $r16
	;;

	;;
2:	sub	$r62 = $r62, $r11
	  cmpeq	$b0 = $r62, $r0
	;;
	brf	$b0, 2b
	;;
	cmpeq	$b0 = $r9, 0
	;;
	brf	$b0, 1b
	;;
	return	$r63
	;;

