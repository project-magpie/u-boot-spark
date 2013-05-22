/*
 * Copyright (C) 2011-2013 STMicroelectronics Limited.
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

#ifndef __ARMV7REG_H_
#define __ARMV7REG_H_

/* CPSR: Disable asynchronous abort exceptions (enabled when unset) */
#define CPSR_A_BIT		(1 << 8)

/* CP15 Main ID Register */
#define CP15_MIDR		c0, 0, c0, 0
#define MIDR_ARCH_SHIFT		16
#define MIDR_ARCH(val)		((val) << MIDR_ARCH_SHIFT)
#define MIDR_ARCH_MASK		(0xF << MIDR_ARCH_SHIFT)
#define MIDR_ARCH_CPUID_SCHEME	0xF

/* CP15 Cache Type Register */
#define CP15_CTR		c0, 0, c0, 1

/* CP15 Multiprocessor Affinity Register */
#define CP15_MPIDR		c0, 0, c0, 5

/* CP15 Processor Feature Registers */
#define CP15_ID_PFR0		c0, 0, c1, 0
#define CP15_ID_PFR1		c0, 0, c1, 1

/* CP15 Memory Model Feature Registers */
#define CP15_ID_MMFR0		c0, 0, c1, 4
#define CP15_ID_MMFR1		c0, 0, c1, 5
#define CP15_ID_MMFR2		c0, 0, c1, 6
#define CP15_ID_MMFR3		c0, 0, c1, 7

/* CP15 Cache Size ID Register */
#define CP15_CSIDR		c0, 1, c0, 0
/* Write-through supported bit */
#define CSIDR_WT_SHIFT		31
#define CSIDR_WT_MASK		(1 << CSIDR_WT_SHIFT)
/* Write-back supported bit */
#define CSIDR_WB_SHIFT		30
#define CSIDR_WB_MASK		(1 << CSIDR_WB_SHIFT)
/* Read allocation supported bit */
#define CSIDR_RA_SHIFT		29
#define CSIDR_RA_MASK		(1 << CSIDR_RA_SHIFT)
/* Write allocation supported bit */
#define CSIDR_WA_SHIFT		28
#define CSIDR_WA_MASK		(1 << CSIDR_WA_SHIFT)
/* (Number of sets in cache) - 1 */
#define CSIDR_NUM_SETS_SHIFT	13
#define CSIDR_NUM_SETS_MASK	(0x3fff << CSIDR_NUM_SETS_SHIFT)
/* (Associativity of cache) - 1 */
#define CSIDR_NUM_WAYS_SHIFT	3
#define CSIDR_NUM_WAYS_MASK	(0x1ff << CSIDR_NUM_WAYS_SHIFT)
/* (Log2(Number of words in cache line)) -2.  For example:
    - For a line length of 4 words: Log2(4) = 2, LineSize entry = 0
     (This is the minimum line length)
    - For a line length of 8 words: Log2(8) = 3, LineSize entry = 1
 */
#define CSIDR_LINE_SIZE_MASK	7

/* CP15 Cache Level ID Register */
#define CP15_CLIDR		c0, 1, c0, 1
/* Level of Unification Uniprocessor for the cache hierarchy */
#define CLIDR_LOUU_SHIFT	27
#define CLIDR_LOUU_MASK		(0xf << CLIDR_LOUU_SHIFT)
/* Level of Coherency for the cache hierarchy */
#define CLIDR_LOC_SHIFT		24
#define CLIDR_LOC_MASK		(7 << CLIDR_LOC_SHIFT)
/* Level of Unification Inner Shareable for the cache hierarchy */
#define CLIDR_LOUIS_SHIFT	21
#define CLIDR_LOUIS_MASK	(7 << CLIDR_LOC_SHIFT)
/* Cache Type */
#define CLIDR_CTYPE_SHIFT(level)	((level - 1) * 3)
#define CLIDR_CTYPE_MASK(level)	(7 << CLIDR_CTYPE_SHIFT(level))
#define CLIDR_CTYPE_NO_CACHE	0
#define CLIDR_CTYPE_ICACHE	1
#define CLIDR_CTYPE_DCACHE	2
#define CLIDR_CTYPE_I_AND_D	(CLIDR_CTYPE_ICACHE | CLIDR_CTYPE_DCACHE)
#define CLIDR_CTYPE_UNIFIED	4

/* CP15 Cache Size Selection Register */
#define CP15_CSSELR		c0, 2, c0, 0


/* CP15 System Control Register */
#define CP15_SCTLR		c1, 0, c0, 0
/* Thumb exception bit */
#define SCTLR_TE_SHIFT		30
#define SCTLR_TE_MASK		(1 << SCTLR_TE_SHIFT)
/* High exception vectors bit */
#define SCTLR_V_SHIFT		13
#define SCTLR_V_MASK		(1 << SCTLR_V_SHIFT)
/* Instruction cache enable bit */
#define SCTLR_I_SHIFT		12
#define SCTLR_I_MASK		(1 << SCTLR_I_SHIFT)
/* Branch prediction enable bit */
#define SCTLR_Z_SHIFT		11
#define SCTLR_Z_MASK		(1 << SCTLR_Z_SHIFT)
/* SW enable bit */
#define SCTLR_SW_SHIFT		10
#define SCTLR_SW_MASK		(1 << SCTLR_SW_SHIFT)
/* Data/unified cache enable bit */
#define SCTLR_C_SHIFT		2
#define SCTLR_C_MASK		(1 << SCTLR_C_SHIFT)
/* Alignment fault checking enable bit */
#define SCTLR_A_SHIFT		1
#define SCTLR_A_MASK		(1 << SCTLR_A_SHIFT)
/* MMU enable bit */
#define SCTLR_M_SHIFT		0
#define SCTLR_M_MASK		(1 << SCTLR_M_SHIFT)

/* CP15 Auxiliary Control Register */
#define CP15_ACTLR		c1, 0, c0, 1
/* SMP bit */
#define ACTLR_SMP_SHIFT		6
#define ACTLR_SMP_MASK		(1 << ACTLR_SMP_SHIFT)

/* CP15 Translation Table Base Registers */
#define CP15_TTBR0		c2, 0, c0, 0
#define CP15_TTBR1		c2, 0, c0, 1

/* CP15 Translation Table Base Control Register */
#define CP15_TTBCR		c2, 0, c0, 2

/* CP15 Domain Access Control Register */
#define	CP15_DACR		c3, 0, c0, 0

/* CP15 Performance Monitor registers */
#define CP15_PMCR		c9, 0, c12, 0
#define CP15_PMCNTENSET		c9, 0, c12, 1
#define CP15_PMCNTENCLR		c9, 0, c12, 2
#define CP15_PMOVSR		c9, 0, c12, 3
#define CP15_PMSWINC		c9, 0, c12, 4
#define CP15_PMSELR		c9, 0, c12, 5
#define CP15_PMCCNTR		c9, 0, c13, 0
#define CP15_PMXEVTYPER		c9, 0, c13, 1
#define CP15_PMXEVCNTR		c9, 0, c13, 2
#define CP15_PMUSERENR		c9, 0, c14, 0
#define CP15_PMINTENSET		c9, 0, c14, 1
#define CP15_PMINTENCLR		c9, 0, c14, 2

/* CP15 Vector Base Address Register */
#define CP15_VBAR		c12, 0, c0, 0

/* CP15 Software Thread ID registers (ARMv7 only) */
#define CP15_TPIDRURW		c13, 0, c0, 2
#define CP15_TPIDRURO		c13, 0, c0, 3
#define CP15_TPIDRPRW		c13, 0, c0, 4

/* CP15 Configuration Base Address Register (Cortex-A9 to access MPCore global
   timer and local watchdog/interrupt registers.
 */
#define CP15_PERIPHBASE		c15, 4, c0, 0

/* Cortex-A9 MP-Core Global Timer register offsets from peripheral base */
#define A9MP_GLOBAL_TIMER_COUNTER_LO_OFFSET	0x200
#define A9MP_GLOBAL_TIMER_COUNTER_HI_OFFSET	0x204
#define A9MP_GLOBAL_TIMER_CONTROL_OFFSET	0x208
#define A9MP_GLOBAL_TIMER_ISR_OFFSET		0x20C
#define A9MP_GLOBAL_TIMER_COMPVAL_LO_OFFSET	0x210
#define A9MP_GLOBAL_TIMER_COMPVAL_HI_OFFSET	0x214
#define A9MP_GLOBAL_TIMER_AUTOINC_OFFSET	0x218

#ifdef __ASSEMBLER__

/* Generic move to/from ARM registers for CP15 */
.macro READ_CP15 reg:req, crn:req, opc1:req, crm:req, opc2:req
	MRC	p15, \opc1, \reg, \crn, \crm, \opc2
.endm

.macro WRITE_CP15 reg:req, crn:req, opc1:req, crm:req, opc2:req
	MCR	p15, \opc1, \reg, \crn, \crm, \opc2
.endm

/* Cache manipulation helpers */
#define CACHE_INVALIDATE_DCACHE_SET_WAY(reg)	WRITE_CP15 reg, c7, 0, c6, 2

#endif /* __ASSEMBLER__ */

#endif /* __ARMV7REG_H_ */
