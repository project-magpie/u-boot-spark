/*
 * File     : pokeloop.S
 * Synopsis : ARMv7 poke loop/Sysinit Ops performing code.
 * Version  : 4.2
 *
 * Copyright (c) 2004-2013 STMicroelectronics Limited.  All rights reserved.
 */

#include "poketable.h"

	.syntax		unified
	.arch		armv7
	.cpu		cortex-a9
	.thumb

/*
 * The poke table is a series of long (32-bit) words, in the format:
 *
 *	opcode, operand, operand (, operand)*
 *
 * An opcode of 0 marks the table end.
 *
 * MUST_RUN_FROM_ICACHE must be defined to a non-zero value if this code is to
 * be run from a 32 bytes per line instruction cache (it will jump through
 * itself once before starting to ensure all lines are cached).  If it is not
 * defined at all, it will be defined to 1.
 */
#ifndef MUST_RUN_FROM_ICACHE
#define MUST_RUN_FROM_ICACHE			1
#endif /* !MUST_RUN_FROM_ICACHE */

/* The section the pokeLoop code should be put in may be defined */
#if defined(POKELOOPSECTION)
	.pushsection	POKELOOPSECTION
#endif /* !POKELOOPSECTION */

/* The name of the pokeLoop function may be defined */
#ifndef POKELOOP_FUNC_NAME_ASM
#define POKELOOP_FUNC_NAME_ASM			__pokeLoop
#endif

/* Jump-through helper macros */
#if (MUST_RUN_FROM_ICACHE != 0)
/* We do a single code jump-through to pull the code into the instruction
 * cache - we have put a jump point in every cache line.  One way to do
 * this is to use "<prefix>objdump -d" on an executable containing this
 * code, and visually inspect that each jump-through point is in a
 * separate 0x20 range (for 32 byte cache lines).
 * Note: PLI would be insufficient to pull the code into the I-cache.
 * In fact ARM support said "Cortex-A9 PLI instruction preloads the TLB
 * to prevent a TLB miss when the code is executed. It does not load
 * instructions into the instruction cache." (Tech support reference
 * number 497452)
 * FIXME: If we want to support this code as both ARM and Thumb-2, we
 * might need further jump-through points for the ARM case due to the
 * sometimes larger instructions.
 */
#define JUMPTHROUGH_INIT			B	8f
#define JUMPTHROUGH_POINT_NO_EXEC		8:	B	8f
#define JUMPTHROUGH_POINT_EXEC			B	9f;		\
						8:	B	8f;	\
						9:
#define JUMPTHROUGH_END				8:	B	DoPoking
#else /* (MUST_RUN_FROM_ICACHE = 0) */
#define JUMPTHROUGH_INIT
#define JUMPTHROUGH_POINT_NO_EXEC
#define JUMPTHROUGH_POINT_EXEC
#define JUMPTHROUGH_END
#endif /* (MUST_RUN_FROM_ICACHE != 0) */

	.global		POKELOOP_FUNC_NAME_ASM
	.type		POKELOOP_FUNC_NAME_ASM, %function

/*
 * Routine to setup peripheral registers.
 * It can be executed from within the I-cache (if MUST_RUN_FROM_ICACHE is
 * defined), with all its data in the D-cache.  It is the caller's
 * responsibility to ensure the data is all in the D-cache.
 *
 * On ARMv7 speculative fetches may occur:
 *  - with the MMU disabled, instruction fetches could occur within the same
 *    4KB block, or the subsequent 4KB block
 *  - with the MMU enabled, speculative fetches could occur from 'normal'
 *    memory and instruction fetches from locations not marked as 'eXecute
 *    Never'
 *
 * When wanting to run from caches, this code and the poke table should be put
 * in the same unique section to ensure they are contiguous in memory.  This
 * will ensure that, as long as the data cache is large enough, the
 * SwitchJumpTable and the poke table will not be on the same cache line
 * (guaranteeing functionality on a direct-mapped cache).
 *
 * The poke loop code honours the architecture ABI for parameter passing, with
 * the address of the poke table to process as the first argument, and a device
 * ID to use in the IF_DEVID operations as the second argument.
 *
 * A suitable C prototype might be:
 *   int __pokeLoop(const unsigned int* pokeTable, unsigned int deviceID)
 *           __asm__ ("__pokeLoop");
 */
	.balign	32
POKELOOP_FUNC_NAME_ASM:
	/* Ensure the jump table is in the data cache */
	ADR	r3, SwitchJumpTable
	PLD	[r3]		/* 4 data preloads as could cover 4 data cache lines */
	ADD	r3, r3, #32
	PLD	[r3]
	ADD	r3, r3, #32
	PLD	[r3]
	ADD	r3, r3, #32
	PLD	[r3]
JUMPTHROUGH_INIT
DoPoking:
#ifdef DEBUG_SLOW_POKELOOP
	/* Give this a go if there is a suspicion the TargetPack is missing
	   explicit delays which need to be there for correct romgen conversion.
	   The POKELOOP_DELAY may need 'tuning'.
	 */
#ifndef POKELOOP_DELAY
#define POKELOOP_DELAY				27000
#endif
	LDR	r3, =POKELOOP_DELAY
1:	SUB	r3, r3, #1
	CBZ	r3, 1f
	B	1b
1:
#endif /* DEBUG_SLOW_POKELOOP */
	LDR	r3, [r0], #4	/* Opcode */
	ADR	r12, SwitchJumpTable
	LDR	r2, [r0], #4	/* Operand 1 */
	TBH	[r12, r3]

SwitchJumpFrom:
/* The SwitchJumpTable must be in increasing numeric order of opcode (with
 * padding for any missing entries).  Distance between SwitchJumpTable and any
 * of the operations must be less than 512 bytes (the assembler should point it
 * out if we ever break that condition and have to switch to TBH, or put in a
 * stub-jump for operations further away).
 * If the size of the table goes above 32 bytes, it will also need to be pre-
 * loaded into the data cache (PLD).  Whilst 32 bytes or less, the first use
 * is safe to pull it in.
 */
	.balign	32
SwitchJumpTable:
	.short	((DoRet - SwitchJumpFrom) >> 1)
	.short	((DoPoke8 - SwitchJumpFrom) >> 1)
	.short	((DoPoke16 - SwitchJumpFrom) >> 1)
	.short	((DoPoke32 - SwitchJumpFrom) >> 1)
	.short	((DoOr8 - SwitchJumpFrom) >> 1)
	.short	((DoOr16 - SwitchJumpFrom) >> 1)
	.short	((DoOr32 - SwitchJumpFrom) >> 1)
	.short	((DoUpdate8 - SwitchJumpFrom) >> 1)
	.short	((DoUpdate16 - SwitchJumpFrom) >> 1)
	.short	((DoUpdate32 - SwitchJumpFrom) >> 1)
	.short	((DoPokeUpdate32 - SwitchJumpFrom) >> 1)
	.short	((DoWhileNe8 - SwitchJumpFrom) >> 1)
	.short	((DoWhileNe16 - SwitchJumpFrom) >> 1)
	.short	((DoWhileNe32 - SwitchJumpFrom) >> 1)
	.short	((DoIfEq32 - SwitchJumpFrom) >> 1)
	.short	((DoIfGT32 - SwitchJumpFrom) >> 1)
	.short	((DoElse - SwitchJumpFrom) >> 1)
	.short	((DoDelay - SwitchJumpFrom) >> 1)
	.short	((DoIfDevIDGE - SwitchJumpFrom) >> 1)
	.short	((DoIfDevIDLE - SwitchJumpFrom) >> 1)
	.short	((DoPoke32Series - SwitchJumpFrom) >> 1)
	.short	((DoPoke32SetRange - SwitchJumpFrom) >> 1)

	.short	((DoAnd32 - SwitchJumpFrom) >> 1)

	.short	((DoPoke32RegOff - SwitchJumpFrom) >> 1)
	.short	((DoOr32RegOff - SwitchJumpFrom) >> 1)
	.short	((DoAnd32RegOff - SwitchJumpFrom) >> 1)
	.short	((DoUpdate32RegOff - SwitchJumpFrom) >> 1)
	.short	((DoPokeUpdate32RegOff - SwitchJumpFrom) >> 1)
	.short	((DoWhileNe32RegOff - SwitchJumpFrom) >> 1)
	.short	((DoIfEq32RegOff - SwitchJumpFrom) >> 1)
	.short	((DoIfGT32RegOff - SwitchJumpFrom) >> 1)
	.short	((DoPoke32SeriesRegOff - SwitchJumpFrom) >> 1)
	.short	((DoPoke32SetRangeRegOff - SwitchJumpFrom) >> 1)

	.short	((DoCopyReg - SwitchJumpFrom) >> 1)
	.short	((DoOrReg - SwitchJumpFrom) >> 1)
	.short	((DoAndReg - SwitchJumpFrom) >> 1)
	.short	((DoNotReg - SwitchJumpFrom) >> 1)
	.short	((DoIfEqReg - SwitchJumpFrom) >> 1)
	.short	((DoIfGTReg - SwitchJumpFrom) >> 1)

	.short	((DoCopyRegRegOff - SwitchJumpFrom) >> 1)
	.short	((DoOrRegRegOff - SwitchJumpFrom) >> 1)
	.short	((DoAndRegRegOff - SwitchJumpFrom) >> 1)
	.short	((DoNotRegRegOff - SwitchJumpFrom) >> 1)
	.short	((DoIfEqRegRegOff - SwitchJumpFrom) >> 1)
	.short	((DoIfGTRegRegOff - SwitchJumpFrom) >> 1)

	.short	((DoAddConst - SwitchJumpFrom) >> 1)
	.short	((DoAddReg - SwitchJumpFrom) >> 1)
	.short	((DoMultConst - SwitchJumpFrom) >> 1)
	.short	((DoMultReg - SwitchJumpFrom) >> 1)
	.short	((DoShiftConst - SwitchJumpFrom) >> 1)
	.short	((DoShiftReg - SwitchJumpFrom) >> 1)

	.short	((DoAddConstRegOff - SwitchJumpFrom) >> 1)
	.short	((DoAddRegRegOff - SwitchJumpFrom) >> 1)
	.short	((DoMultConstRegOff - SwitchJumpFrom) >> 1)
	.short	((DoMultRegRegOff - SwitchJumpFrom) >> 1)
	.short	((DoShiftConstRegOff - SwitchJumpFrom) >> 1)
	.short	((DoShiftRegRegOff - SwitchJumpFrom) >> 1)

	.short	((DoSetDevIDConst - SwitchJumpFrom) >> 1)
	.short	((DoSetDevIDReg - SwitchJumpFrom) >> 1)

	.balign	32

JUMPTHROUGH_POINT_NO_EXEC

/* END_MARKER/RETURN(x) */
DoRet:
	MOV	r0, r2		/* Exit code */
	BX	lr		/* Return point */

/* POKE8(A, VAL) */
DoPoke8:
	LDR	r3, [r0], #4	/* Read VAL */
	STRB	r3, [r2]	/* *A = VAL */
	B	DoPoking

/* POKE16(A, VAL) */
DoPoke16:
	LDR	r3, [r0], #4	/* Read VAL */
	STRH	r3, [r2]	/* *A = VAL */
	B	DoPoking

/* POKE32(A, VAL) */
DoPoke32:
	LDR	r3, [r0], #4	/* Read VAL */
	STR	r3, [r2]	/* *A = VAL */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* OR8(A, VAL) */
DoOr8:
	LDR	r3, [r0], #4	/* Read VAL */
	LDRB	r12, [r2]	/* *A */
	ORR	r3, r3, r12	/* *A | OR */
	STRB	r3, [r2]	/* *A |= OR */
	B	DoPoking

/* OR16(A, VAL) */
DoOr16:
	LDR	r3, [r0], #4	/* Read VAL */
	LDRH	r12, [r2]	/* *A */
	ORR	r3, r3, r12	/* *A | VAL */
	STRH	r3, [r2]	/* *A |= VAL */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* OR32(A, VAL) */
DoOr32:
	LDR	r3, [r0], #4	/* VAL */
	LDR	r12, [r2]	/* *A */
	ORR	r3, r3, r12	/* *A | VAL */
	STR	r3, [r2]	/* *A |= VAL */
	B	DoPoking

/* UPDATE8(A, AND, OR) */
DoUpdate8:
	LDR	r3, [r0], #4	/* Read AND */
	LDRB	r12, [r2]	/* *A */
	AND	r12, r12, r3	/* *A & AND */
	LDR	r3, [r0], #4	/* Read OR */
	ORR	r12, r12, r3	/* (*A & AND) | OR */
	STRB	r12, [r2]	/* *A = ((*A & AND) | OR) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* UPDATE16(A, AND, OR) */
DoUpdate16:
	LDR	r3, [r0], #4	/* Read AND */
	LDRH	r12, [r2]	/* *A */
	AND	r12, r12, r3	/* *A & AND */
	LDR	r3, [r0], #4	/* Read OR */
	ORR	r12, r12, r3	/* (*A & AND) | OR */
	STRH	r12, [r2]	/* *A = ((*A & AND) | OR) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* UPDATE32(A, AND, OR) */
DoUpdate32:
	LDR	r3, [r0], #4	/* Read AND */
	LDR	r12, [r2]	/* *A */
	AND	r12, r12, r3	/* *A & AND */
	LDR	r3, [r0], #4	/* Read OR */
	ORR	r12, r12, r3	/* (*A & AND) | OR */
	STR	r12, [r2]	/* *A = ((*A & AND) | OR) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* POKE_UPDATE32(A1, A2, AND, SHIFT, OR) */
DoPokeUpdate32:
	LDR	r3, [r0], #4	/* Read A2 */
	LDR	r12, [r3]	/* *A2 */
	LDR	r3, [r0], #4	/* Read AND */
JUMPTHROUGH_POINT_EXEC
	AND	r12, r12, r3	/* *A2 & AND */
	LDR	r3, [r0], #4	/* Read SHIFT */
	CMP	r3, #0
	ITTE	MI
	RSBMI	r3, r3, #0	/* For a negative shift, shift right 0 - SHIFT */
	LSRMI	r12, r12, r3	/* (*A2 & AND) >> -SHIFT */
	LSLPL	r12, r12, r3	/* (*A2 & AND) << SHIFT */
	LDR	r3, [r0], #4	/* Read OR */
	ORR	r12, r12, r3	/* ((*A2 & AND) << SHIFT) | OR */
	STR	r12, [r2]	/* *A1 = (((*A2 & AND) << SHIFT) | OR) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* WHILE_NE8(A, AND, VAL) */
DoWhileNe8:
1:	LDR	r3, [r0]	/* Read AND */
	LDRB	r12, [r2]	/* *A */
	UXTB	r12, r12	/* 32bit(*A) */
	AND	r3, r3, r12	/* *A & AND */
	LDR	r12, [r0, #4]	/* Read VAL */
	CMP	r3, r12		/* If ((*A & AND) == VAL) */
	BNE	1b		/* Loop if false */
	ADD	r0, r0, #8
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* WHILE_NE16(A, AND, VAL) */
DoWhileNe16:
1:	LDR	r3, [r0]	/* Read AND */
	LDRH	r12, [r2]	/* *A */
	UXTH	r12, r12	/* 32bit(*A) */
	AND	r3, r3, r12	/* *A & AND */
	LDR	r12, [r0, #4]	/* Read VAL */
	CMP	r3, r12		/* If ((*A & AND) == VAL) */
	BNE	1b		/* Loop if false */
	ADD	r0, r0, #8
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* WHILE_NE32(A, AND, VAL) */
DoWhileNe32:
1:	LDR	r3, [r0]	/* Read AND */
	LDR	r12, [r2]	/* *A */
	AND	r3, r3, r12	/* *A & AND */
	LDR	r12, [r0, #4]	/* Read VAL */
	CMP	r3, r12		/* If ((*A & AND) == VAL) */
	BNE	1b		/* Loop if false */
	ADD	r0, r0, #8
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* IF_EQ32(NESTLEVEL, A, AND, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfEq32:
	LDR	r3, [r0], #4	/* Read AND */
	LDR	r2, [r2]	/* *A */
	AND	r2, r2, r3	/* *A & AND */
	LDR	r3, [r0], #4	/* Read VAL */
	CMP	r2, r3		/* If ((*A & AND) == VAL) */
	LDR	r3, [r0], #4	/* Read distance to ELSE/ENDIF */
	BEQ	DoPoking	/* Go ahead with these pokes */
	ADD	r0, r0, r3	/* Skip forward through pokes to ELSE or ENDIF */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* IF_GT32(NESTLEVEL, A, AND, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfGT32:
	LDR	r3, [r0], #4	/* Read AND */
	LDR	r2, [r2]	/* *A */
	AND	r2, r2, r3	/* *A & AND */
	LDR	r3, [r0], #4	/* Read VAL */
	CMP	r2, r3		/* If ((*A & AND) > VAL) */
	LDR	r3, [r0], #4	/* Read distance to ELSE/ENDIF */
	BHI	DoPoking	/* Go ahead with these pokes */
	ADD	r0, r0, r3	/* Skip forward through pokes to ELSE or ENDIF */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* ELSE(NESTLEVEL)
   ENDIF
   GOTO(LABEL)
   Note that NESTLEVEL/LABEL is not in the actual table, but there is a
   distance field following the opcode.
 */
DoElse:
	ADD	r0, r0, r2	/* Skip through to ENDIF */
	B	DoPoking

/* DELAY(ITERATIONS) */
DoDelay:
1:	SUBS	r2, r2, #1	/* If (--OPERATIONS == 0) */
	BNE	1b		/* SUBS should have set Z bit if equal */
	B	DoPoking

/* IF_DEVID_GE(NESTLEVEL, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfDevIDGE:
	LDR	r3, [r0], #4	/* Read distance to ELSE/ENDIF */
	CMP	r1, r2		/* If (device ID >= VAL) */
	IT	LO
	ADDLO	r0, r0, r3	/* No, so skip to ELSE/ENDIF */
	B	DoPoking

/* IF_DEVID_LE(NESTLEVEL, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfDevIDLE:
	LDR	r3, [r0], #4	/* Read distance to ELSE/ENDIF */
	CMP	r1, r2		/* If (device ID <= VAL) */
	IT	HI
	ADDHI	r0, r0, r3	/* No, so skip to ELSE/ENDIF */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* POKE32_SERIES(ADDR, «NUM_VALS,» VAL_LIST...)
   Note that NUM_VALS is not in the operation the user creates; it is
   automatically inserted in the data table by the poketable.h macro.
 */
DoPoke32Series:
	LDR	r3, [r0], #4	/* Read NUM_VALS */
1:	LDR	r12, [r0], #4	/* Read a VAL */
	STR	r12, [r2], #4
	SUBS	r3, r3, #1	/* NUM_VALS-- */
	BNE	1b		/* SUBS should have set Z bit if 0 */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* POKE32_SET_RANGE(A, VAL, WORDS) */
DoPoke32SetRange:
	LDR	r12, [r0], #4	/* Read VAL */
	LDR	r3, [r0], #4	/* Read WORDS */
	CBZ	r3, 2f
1:	STR	r12, [r2], # 4	/* *ADDR++ = VAL */
	SUBS	r3, r3, #1	/* WORDS-- */
	BNE	1b		/* SUBS should have set Z bit if 0 */
2:	B	DoPoking

/* AND32(A, VAL) */
DoAnd32:
	LDR	r3, [r0], #4	/* VAL */
	LDR	r12, [r2]	/* *A */
	AND	r3, r3, r12	/* *A & VAL */
	STR	r3, [r2]	/* *A &= VAL */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* POKE32_REGOFF(A, REGOFF, VAL) */
DoPoke32RegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r3, [r0], #4	/* Read VAL */
	STR	r3, [r2]	/* *(A + *REGOFF) = VAL */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* OR32_REGOFF(A, REGOFF, VAL) */
DoOr32RegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r12, [r2]
	LDR	r3, [r0], #4	/* Read VAL */
	ORR	r12, r12, r3
	STR	r12, [r2]	/* *(A + *REGOFF) |= VAL */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* AND32_REGOFF(A, REGOFF, VAL) */
DoAnd32RegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r12, [r2]
	LDR	r3, [r0], #4	/* Read VAL */
	AND	r12, r12, r3
	STR	r12, [r2]	/* *(A + *REGOFF) |= VAL */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* UPDATE32_REGOFF(A, REGOFF, AND, OR) */
DoUpdate32RegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r12, [r2]
	LDR	r3, [r0], #4	/* Read AND */
	AND	r12, r12, r3
	LDR	r3, [r0], #4	/* Read OR */
	ORR	r12, r12, r3
	STR	r12, [r2]	/* *(A + *REGOFF) = ((*(A + *REGOFF) & AND) | OR) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* POKE_UPDATE32_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF, AND, SHIFT, OR) -
 * - param order in table:
 *   SRC, SRC_REGOFF, AND, SHIFT, OR, DST, DST_REGOFF
 */
DoPokeUpdate32RegOff:
	LDR	r3, [r0], #4	/* Read SRC_REGOFF */
	CBZ	r3, 1f		/* Only use SRC_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r2, [r2]	/* *(SRC + *SRC_REGOFF) */
	LDR	r3, [r0], #4	/* Read AND */
	AND	r2, r2, r3
	LDR	r3, [r0], #4	/* Read SHIFT */
JUMPTHROUGH_POINT_EXEC
	CMP	r3, #0
	ITTE	MI
	RSBMI	r3, r3, #0	/* For a negative shift, shift right 0 - SHIFT */
	LSRMI	r2, r2, r3	/* (*(SRC + *SRC_REGOFF) & AND) >> -SHIFT */
	LSLPL	r2, r2, r3	/* (*(SRC + *SRC_REGOFF) & AND) << SHIFT */
	LDR	r3, [r0], #4	/* Read OR */
	ORR	r2, r2, r3
	LDR	r12, [r0], #4	/* Read DST */
	LDR	r3, [r0], #4	/* Read DST_REGOFF */
	CBZ	r3, 1f		/* Only use DST_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r12, r12, r3
1:	STR	r2, [r12]	/* *(DST + *DST_REGOFF) = (((*(SRC + *SRC_REGOFF) & AND) << SHIFT) | OR) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC
	.balign	32
JUMPTHROUGH_POINT_NO_EXEC

/* WHILE_NE32_REGOFF(A, REGOFF, AND, VAL) */
DoWhileNe32RegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r3, [r0]	/* Read AND */
	LDR	r12, [r2]	/* *A */
	AND	r3, r3, r12	/* *A & AND */
	LDR	r12, [r0, #4]	/* Read VAL */
	CMP	r3, r12		/* If ((*A & AND) == VAL) */
	BNE	1b		/* Loop if false */
	ADD	r0, r0, #8
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* IF_EQ32_REGOFF(NESTLEVEL, A, REGOFF, AND, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfEq32RegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r3, [r0], #4	/* Read AND */
	LDR	r2, [r2]	/* *A */
	AND	r2, r2, r3	/* *A & AND */
	LDR	r3, [r0], #4	/* Read VAL */
	CMP	r2, r3		/* If ((*A & AND) == VAL) */
	LDR	r3, [r0], #4	/* Read distance to ELSE/ENDIF */
	BEQ	DoPoking	/* Go ahead with these pokes */
	ADD	r0, r0, r3	/* Skip forward through pokes to ELSE or ENDIF */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* IF_GT32_REGOFF(NESTLEVEL, A, REGOFF, AND, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfGT32RegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r3, [r0], #4	/* Read AND */
	LDR	r2, [r2]	/* *A */
	AND	r2, r2, r3	/* *A & AND */
	LDR	r3, [r0], #4	/* Read VAL */
	CMP	r2, r3		/* If ((*A & AND) > VAL) */
	LDR	r3, [r0], #4	/* Read distance to ELSE/ENDIF */
	BHI	DoPoking	/* Go ahead with these pokes */
	ADD	r0, r0, r3	/* Skip forward through pokes to ELSE or ENDIF */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* POKE32_SERIES_REGOFF(ADDR, REGOFF, «NUM_VALS», VAL_LIST...)
   Note that NUM_VALS is not in the operation the user creates; it is
   automatically inserted in the data table by the poketable.h macro.
 */
DoPoke32SeriesRegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r3, [r0], #4	/* Read NUM_VALS */
1:	LDR	r12, [r0], #4	/* Read a VAL */
	STR	r12, [r2], #4
	SUBS	r3, r3, #1	/* NUM_VALS-- */
	BNE	1b		/* SUBS should have set Z bit if 0 */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* POKE32_SET_RANGE_REGOFF(A, REGOFF, VAL, WORDS) */
DoPoke32SetRangeRegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r12, [r0], #4	/* Read VAL */
	LDR	r3, [r0], #4	/* Read WORDS */
	CBZ	r3, 2f
1:	STR	r12, [r2], # 4	/* *ADDR++ = VAL */
	SUBS	r3, r3, #1	/* WORDS-- */
	BNE	1b		/* SUBS should have set Z bit if 0 */
2:	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* COPY_REG(DST, SRC) - param order in table: SRC, DST */
DoCopyReg:
	LDR	r2, [r2]	/* *SRC */
	LDR	r3, [r0], #4	/* Read DST */
	STR	r2, [r3]
	B	DoPoking

/* OR_REG(DST, SRC) - param order in table: SRC, DST */
DoOrReg:
	LDR	r2, [r2]	/* *SRC */
	LDR	r3, [r0], #4	/* Read DST */
	LDR	r12, [r3]	/* *DST */
	ORR	r2, r2, r12
	STR	r2, [r3]
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* AND_REG(DST, SRC) - param order in table: SRC, DST */
DoAndReg:
	LDR	r2, [r2]	/* *SRC */
	LDR	r3, [r0], #4	/* Read DST */
	LDR	r12, [r3]	/* *DST */
	AND	r2, r2, r12
	STR	r2, [r3]
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* NOT_REG(A) */
DoNotReg:
	LDR	r3, [r2]
	MVN	r3, r3
	STR	r3, [r2]
	B	DoPoking

/* IF_EQ_REG(NESTLEVEL, R1, R2)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following the stated parameters.
 */
DoIfEqReg:
	LDR	r2, [r2]	/* *R1 */
	LDR	r3, [r0], #4	/* Read R2 */
	LDR	r3, [r3]	/* *R2 */
	CMP	r2, r3		/* If (*R1 == *R2) */
	LDR	r3, [r0], #4	/* Read distance to ELSE/ENDIF */
	BEQ	DoPoking	/* Go ahead with these pokes */
	ADD	r0, r0, r3	/* Skip forward through pokes to ELSE or ENDIF */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* IF_GT_REG(NESTLEVEL, R1, R2)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following the stated parameters.
 */
DoIfGTReg:
	LDR	r2, [r2]	/* *R1 */
	LDR	r3, [r0], #4	/* Read R2 */
	LDR	r3, [r3]	/* *R2 */
	CMP	r2, r3		/* If (*R1 > *R2) */
	LDR	r3, [r0], #4	/* Read distance to ELSE/ENDIF */
	BHI	DoPoking	/* Go ahead with these pokes */
	ADD	r0, r0, r3	/* Skip forward through pokes to ELSE or ENDIF */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* COPY_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF)
 * - param order in table:
 *   SRC, SRC_REGOFF, DST, DST_REGOFF
 */
DoCopyRegRegOff:
	LDR	r3, [r0], #4	/* Read SRC_REGOFF */
	CBZ	r3, 1f		/* Only use SRC_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r2, [r2]	/* *(SRC + *SRC_REGOFF) */
	LDR	r12, [r0], #4	/* Read DST */
	LDR	r3, [r0], #4	/* Read DST_REGOFF */
	CBZ	r3, 1f		/* Only use DST_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r12, r12, r3
1:	STR	r2, [r12]
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC
	.balign	32
JUMPTHROUGH_POINT_NO_EXEC

/* OR_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF)
 * - param order in table:
 *   SRC, SRC_REGOFF, DST, DST_REGOFF
 */
DoOrRegRegOff:
	LDR	r3, [r0], #4	/* Read SRC_REGOFF */
	CBZ	r3, 1f		/* Only use SRC_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r2, [r2]	/* *(SRC + *SRC_REGOFF) */
	LDR	r12, [r0], #4	/* Read DST */
	LDR	r3, [r0], #4	/* Read DST_REGOFF */
	CBZ	r3, 1f		/* Only use DST_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r12, r12, r3
1:	LDR	r3, [r12]
	ORR	r2, r2, r3
	STR	r2, [r12]
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* AND_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF)
 * - param order in table:
 *   SRC, SRC_REGOFF, DST, DST_REGOFF
 */
DoAndRegRegOff:
	LDR	r3, [r0], #4	/* Read SRC_REGOFF */
	CBZ	r3, 1f		/* Only use SRC_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r2, [r2]	/* *(SRC + *SRC_REGOFF) */
	LDR	r12, [r0], #4	/* Read DST */
	LDR	r3, [r0], #4	/* Read DST_REGOFF */
	CBZ	r3, 1f		/* Only use DST_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r12, r12, r3
1:	LDR	r3, [r12]
	AND	r2, r2, r3
	STR	r2, [r12]
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* NOT_REG_REGOFF(A, A_REGOFF) */
DoNotRegRegOff:
	LDR	r3, [r0], #4	/* Read A_REGOFF */
	CBZ	r3, 1f		/* Only use A_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r3, [r2]
	MVN	r3, r3
	STR	r3, [r2]
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* IF_EQ_REG_REGOFF(NESTLEVEL, A1, A1_REGOFF, A2, A2_REGOFF)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following the last parameter.
 */
DoIfEqRegRegOff:
	LDR	r3, [r0], #4	/* Read A1_REGOFF */
	CBZ	r3, 1f		/* Only use A1_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r2, [r2]	/* *(A1 + *A1_REGOFF) */
	LDR	r12, [r0], #4	/* Read A2 */
	LDR	r3, [r0], #4	/* Read A2_REGOFF */
	CBZ	r3, 1f		/* Only use A2_REGOFF if it's not 0 */
JUMPTHROUGH_POINT_EXEC
	LDR	r3, [r3]
	ADD	r12, r12, r3
1:	LDR	r12, [r12]	/* *(A2 + *A2_REGOFF) */
	CMP	r2, r12		/* If (*(A1 + *A1_REGOFF) == *(A2 + *A2_REGOFF)) */
	LDR	r3, [r0], #4	/* Read distance to ELSE/ENDIF */
	BEQ	DoPoking	/* Go ahead with these pokes */
	ADD	r0, r0, r3	/* Skip forward through pokes to ELSE or ENDIF */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* IF_GT_REG_REGOFF(NESTLEVEL, A1, A1_REGOFF, A2, A2_REGOFF)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following the last parameter.
 */
DoIfGTRegRegOff:
	LDR	r3, [r0], #4	/* Read A1_REGOFF */
	CBZ	r3, 1f		/* Only use A1_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r2, [r2]	/* *(A1 + *A1_REGOFF) */
	LDR	r12, [r0], #4	/* Read A2 */
	LDR	r3, [r0], #4	/* Read A2_REGOFF */
	CBZ	r3, 1f		/* Only use A2_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r12, r12, r3
1:	LDR	r12, [r12]	/* *(A2 + *A2_REGOFF) */
	CMP	r2, r12		/* If (*(A1 + *A1_REGOFF) > *(A2 + *A2_REGOFF)) */
	LDR	r3, [r0], #4	/* Read distance to ELSE/ENDIF */
	BHI	DoPoking	/* Go ahead with these pokes */
	ADD	r0, r0, r3	/* Skip forward through pokes to ELSE or ENDIF */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* ADD_CONST(A, CONST) */
DoAddConst:
	LDR	r3, [r0], #4	/* Read CONST */
	LDR	r12, [r2]	/* *A */
	ADD	r3, r3, r12	/* *A + CONST */
	STR	r3, [r2]	/* *A += CONST */
	B	DoPoking

/* ADD_REG(DST, SRC) - param order in table: SRC, DST */
DoAddReg:
	LDR	r2, [r2]	/* *SRC */
	LDR	r3, [r0], #4	/* Read DST */
	LDR	r12, [r3]	/* *DST */
	ADD	r2, r2, r12
	STR	r2, [r3]
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* MULT_CONST(A, CONST) */
DoMultConst:
	LDR	r3, [r0], #4	/* Read CONST */
	LDR	r12, [r2]	/* *A */
	MUL	r3, r3, r12	/* *A * CONST */
	STR	r3, [r2]	/* *A *= CONST */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* MULT_REG(DST, SRC) - param order in table: SRC, DST */
DoMultReg:
	LDR	r2, [r2]	/* *SRC */
	LDR	r3, [r0], #4	/* Read DST */
	LDR	r12, [r3]	/* *DST */
	MUL	r2, r2, r12
	STR	r2, [r3]
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* SHIFT_CONST(A, CONST) */
DoShiftConst:
	LDR	r3, [r0], #4	/* Read CONST */
	LDR	r12, [r2]	/* *A */
	CMP	r3, #0
	ITTE	MI
	RSBMI	r3, r3, #0	/* For a negative shift, shift right 0 - SHIFT */
	LSRMI	r12, r12, r3	/* *A >> -CONST */
	LSLPL	r12, r12, r3	/* *A << CONST */
	STR	r12, [r2]	/* *A <<= CONST (signed) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* SHIFT_REG(A, SHIFTBY_REG) - param order in table: SHIFTBY_REG, A */
DoShiftReg:
	LDR	r2, [r2]	/* *SHIFTBY_REG */
	LDR	r3, [r0], #4	/* Read A */
	LDR	r12, [r3]	/* *A */
	CMP	r2, #0
	ITTE	MI
	RSBMI	r2, r2, #0	/* For a negative shift, shift right 0 - SHIFT */
	LSRMI	r12, r12, r2	/* *A >> -*SHIFTBY_REG */
	LSLPL	r12, r12, r2	/* *A << *SHIFTBY_REG */
	STR	r12, [r3]	/* *A <<= *SHIFTBY_REG */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* ADD_CONST_REGOFF(A, REGOFF, CONST) */
DoAddConstRegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r3, [r0], #4	/* Read CONST */
	LDR	r12, [r2]	/* *A */
	ADD	r3, r3, r12	/* *A + CONST */
	STR	r3, [r2]	/* *A += CONST */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* ADD_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFFF)
 * - param order in table: SRC, SRC_REGOFF, DST, DST_REGOFF
 */
DoAddRegRegOff:
	LDR	r3, [r0], #4	/* Read SRC_REGOFF */
	CBZ	r3, 1f		/* Only use SRC_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r2, [r2]	/* *(SRC + *SRC_REGOFF) */
	LDR	r12, [r0], #4	/* Read DST */
	LDR	r3, [r0], #4	/* Read DST_REGOFF */
	CBZ	r3, 1f		/* Only use DST_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r12, r12, r3
1:	LDR	r3, [r12]	/* *(DST + *DST_REGOFF) */
	ADD	r2, r2, r3
	STR	r2, [r12]	/* *(DST + *DST_REGOFF) += *(SRC + *SRC_REGOFF) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* MULT_CONST_REGOFF(A, REGOFF, CONST) */
DoMultConstRegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r3, [r0], #4	/* Read CONST */
	LDR	r12, [r2]	/* *A */
	MUL	r3, r3, r12	/* *A * CONST */
	STR	r3, [r2]	/* *A *= CONST */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* MULT_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF, SRC2)
 * - param order in table: SRC, SRC_REGOFF, DST, DST_REGOFF
 */
DoMultRegRegOff:
	LDR	r3, [r0], #4	/* Read SRC_REGOFF */
	CBZ	r3, 1f		/* Only use SRC_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r2, [r2]	/* *(SRC + *SRC_REGOFF) */
	LDR	r12, [r0], #4	/* Read DST */
	LDR	r3, [r0], #4	/* Read DST_REGOFF */
	CBZ	r3, 1f		/* Only use DST_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r12, r12, r3
1:	LDR	r3, [r12]	/* *(DST + *DST_REGOFF) */
	MUL	r2, r2, r3
	STR	r2, [r12]	/* *(DST + *DST_REGOFF) *= *(SRC + *SRC_REGOFF) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* SHIFT_CONST_REGOFF(A, REGOFF, CONST) */
DoShiftConstRegOff:
	LDR	r3, [r0], #4	/* Read REGOFF */
	CBZ	r3, 1f		/* Only use REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r3, [r0], #4	/* Read CONST */
	LDR	r12, [r2]	/* *A */
	CMP	r3, #0
	ITTE	MI
	RSBMI	r3, r3, #0	/* For a negative shift, shift right 0 - SHIFT */
	LSRMI	r12, r12, r3	/* *A >> -CONST */
	LSLPL	r12, r12, r3	/* *A << CONST */
	STR	r12, [r2]	/* *A <<= CONST (signed) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* SHIFT_REG_REGOFF(A, A_REGOFF, SHIFTBY_REG, SHIFTBY_REG_REGOFFF)
 * - param order in table: SHIFTBY_REG, SHIFTBY_REG_REGOFF, A, A_REGOFF
 */
DoShiftRegRegOff:
	LDR	r3, [r0], #4	/* Read SHIFTBY_REG_REGOFF */
	CBZ	r3, 1f		/* Only use SHIFTBY_REG_REGOFF if it's not 0 */
	LDR	r3, [r3]
	ADD	r2, r2, r3
1:	LDR	r2, [r2]	/* *(SHIFTBY_REG + *SHIFTBY_REG_REGOFF) */
	LDR	r12, [r0], #4	/* Read A */
	LDR	r3, [r0], #4	/* Read A_REGOFF */
	CBZ	r3, 1f		/* Only use A_REGOFF if it's not 0 */
JUMPTHROUGH_POINT_EXEC
	LDR	r3, [r3]
	ADD	r12, r12, r3
1:	LDR	r3, [r12]	/* *(A + *A_REGOFF) */
	CMP	r2, #0
	ITTE	MI
	RSBMI	r2, r2, #0	/* For a negative shift, shift right 0 - SHIFT */
	LSRMI	r3, r3, r2	/* *(A + *A_REGOFF) >> -*(SHIFTBY_REG + *SHIFTBY_REG_REGOFF) */
	LSLPL	r3, r3, r2	/* *(A + *A_REGOFF) << *(SHIFTBY_REG + *SHIFTBY_REG_REGOFF) */
	STR	r3, [r12]	/* *(A + *A_REGOFF) <<= *(SHIFTBY_REG + *SHIFTBY_REG_REGOFF) */
	B	DoPoking

JUMPTHROUGH_POINT_NO_EXEC

/* SET_DEVID_CONST(VAL) */
DoSetDevIDConst:
	MOV	r1, r2
	B	DoPoking

/* SET_DEVID_REG(ADDR, AND, SHIFT) */
DoSetDevIDReg:
	LDR	r1, [r2]
	LDR	r2, [r0], #4	/* Read AND */
	AND	r1, r1, r2
	LDR	r2, [r0], #4	/* Read SHIFT */
	CMP	r2, #0
	ITTE	MI
	RSBMI	r2, r2, #0	/* For a negative shift, shift right 0 - SHIFT */
	LSRMI	r1, r1, r2
	LSLPL	r1, r1, r2
	B	DoPoking

JUMPTHROUGH_END

#if defined(POKELOOPSECTION)
	.popsection
#endif /* !POKELOOPSECTION */
