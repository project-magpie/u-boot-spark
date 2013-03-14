/*
 * File     : pokeloop.S
 * Synopsis : ST40 poke loop/Sysinit Ops performing code.
 * Version  : 4.1
 *
 * Copyright (c) 2004-2012 STMicroelectronics Limited.  All rights reserved.
 */

#include "poketable.h"

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

/*
 * If the code has to run uncached from serial flash, we need to avoid 8 and
 * 16-bit reads within the instruction stream.  When
 * ALLOW_UNCACHED_USE_FROM_SERIAL_FLASH is non-zero, this support will be
 * enabled.
 * Note: 16-bit operations are still allowed as the data table could still be
 * in a cached mapping.
 * The default is to leave this disabled as it makes the code a little larger
 * and slower.
 */
#ifndef ALLOW_UNCACHED_USE_FROM_SERIAL_FLASH
#define ALLOW_UNCACHED_USE_FROM_SERIAL_FLASH	0
#endif

/* The section the pokeLoop code should be put in may be defined */
#ifndef POKELOOPSECTION
#define POKELOOPSECTION				.text.pokesection
#endif /* !POKELOOPSECTION */
	.pushsection	POKELOOPSECTION

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
 * We set the T bit so we can use 1 instruction jumps for the
 * jump-through.
 */
#define JUMPTHROUGH_INIT			sett;			\
						bt	8f
#define JUMPTHROUGH_POINT_NO_EXEC		8:	bt	8f
#define JUMPTHROUGH_POINT_EXEC			bra	9f;		\
						  nop;			\
						8:	bt	8f;	\
						9:
#define JUMPTHROUGH_END				8:	bra	DoPoking; \
						nop
#else /* (MUST_RUN_FROM_ICACHE == 0) */
#define JUMPTHROUGH_INIT
#define JUMPTHROUGH_POINT_NO_EXEC
#define JUMPTHROUGH_POINT_EXEC
#define JUMPTHROUGH_END
#endif /* (MUST_RUN_FROM_ICACHE != 0) */

	.global		POKELOOP_FUNC_NAME_ASM

/*
 * Routine to setup peripheral registers.
 * It can be executed from within the I-cache (if MUST_RUN_FROM_ICACHE is
 * defined), with all its data in the D-cache.  It is the caller's
 * responsibility to ensure the data is all in the D-cache.
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
	mova	SwitchJumpTable, r0	/* Keep this in r0 for use in DoPoking */
	mov	r0, r6
	pref	@r6			/* 4 prefetches as could cover 4 data cache lines */
	add	#32, r6
	pref	@r6
	add	#32, r6
	pref	@r6
	add	#32, r6
	pref	@r6
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
	mov.l	2f, r7
1:	dt	r7
	bf	1b
	bra	1f
	.balign	32
2:	.long	POKELOOP_DELAY
1:
#endif /* DEBUG_SLOW_POKELOOP */
	mov.l	@r4+, r7	/* Opcode */
	shll	r7		/* Convert to 16-bit offset rather than byte offet */

#if (ALLOW_UNCACHED_USE_FROM_SERIAL_FLASH == 0)
	mov.w	@(r0, r7), r7
	extu.w	r7, r7
#else /* ALLOW_UNCACHED_USE_FROM_SERIAL_FLASH */
	/* To cope with uncached serial flash usage... */
	mov	r7, r6
	add	r0, r6		/* Address to read offset short from */
	mov	#3, r1
	not	r1, r3
	and	r6, r3		/* Mask 16-bit aligned address to 32-bit word address */
	mov.l	@r3, r3		/* Read word containing the jump offset short */
	mov	#2, r1
	and	r6, r1		/* Which short in the word do we want? */
	shll2	r1
	shll	r1
	neg	r1, r1
	shld	r1, r3		/* Shift right to get desired short in short 0 */
	extu.w	r3, r7		/* 0 extend short 0 to get distance for braf */
#endif /* ALLOW_UNCACHED_USE_FROM_SERIAL_FLASH */

	mov.l	@r4+, r1	/* Operand 1 */
	braf	r7
	  mov.l	@r4+, r2	/* Operand 2 */

SwitchJumpFrom:
/* The SwitchJumpTable must be in increasing numeric order of opcode (with
 * padding for any missing entries).
 */
	.balign	32
SwitchJumpTable:
	.short	DoRet - SwitchJumpFrom
	.short	DoPoke8 - SwitchJumpFrom
	.short	DoPoke16 - SwitchJumpFrom
	.short	DoPoke32 - SwitchJumpFrom
	.short	DoOr8 - SwitchJumpFrom
	.short	DoOr16 - SwitchJumpFrom
	.short	DoOr32 - SwitchJumpFrom
	.short	DoUpdate8 - SwitchJumpFrom
	.short	DoUpdate16 - SwitchJumpFrom
	.short	DoUpdate32 - SwitchJumpFrom
	.short	DoPokeUpdate32 - SwitchJumpFrom
	.short	DoWhileNe8 - SwitchJumpFrom
	.short	DoWhileNe16 - SwitchJumpFrom
	.short	DoWhileNe32 - SwitchJumpFrom
	.short	DoIfEq32 - SwitchJumpFrom
	.short	DoIfGT32 - SwitchJumpFrom
	.short	DoElse - SwitchJumpFrom
	.short	DoDelay - SwitchJumpFrom
	.short	DoIfDevIDGE - SwitchJumpFrom
	.short	DoIfDevIDLE - SwitchJumpFrom
	.short	DoPoke32Series - SwitchJumpFrom
	.short	DoPoke32SetRange - SwitchJumpFrom

	.short	DoAnd32 - SwitchJumpFrom

	.short	DoPoke32RegOff - SwitchJumpFrom
	.short	DoOr32RegOff - SwitchJumpFrom
	.short	DoAnd32RegOff - SwitchJumpFrom
	.short	DoUpdate32RegOff - SwitchJumpFrom
	.short	DoPokeUpdate32RegOff - SwitchJumpFrom
	.short	DoWhileNe32RegOff - SwitchJumpFrom
	.short	DoIfEq32RegOff - SwitchJumpFrom
	.short	DoIfGT32RegOff - SwitchJumpFrom
	.short	DoPoke32SeriesRegOff - SwitchJumpFrom
	.short	DoPoke32SetRangeRegOff - SwitchJumpFrom

	.short	DoCopyReg - SwitchJumpFrom
	.short	DoOrReg - SwitchJumpFrom
	.short	DoAndReg - SwitchJumpFrom
	.short	DoNotReg - SwitchJumpFrom
	.short	DoIfEqReg - SwitchJumpFrom
	.short	DoIfGTReg - SwitchJumpFrom

	.short	DoCopyRegRegOff - SwitchJumpFrom
	.short	DoOrRegRegOff - SwitchJumpFrom
	.short	DoAndRegRegOff - SwitchJumpFrom
	.short	DoNotRegRegOff - SwitchJumpFrom
	.short	DoIfEqRegRegOff - SwitchJumpFrom
	.short	DoIfGTRegRegOff - SwitchJumpFrom

	.short	DoAddConst - SwitchJumpFrom
	.short	DoAddReg - SwitchJumpFrom
	.short	DoMultConst - SwitchJumpFrom
	.short	DoMultReg - SwitchJumpFrom
	.short	DoShiftConst - SwitchJumpFrom
	.short	DoShiftReg - SwitchJumpFrom

	.short	DoAddConstRegOff - SwitchJumpFrom
	.short	DoAddRegRegOff - SwitchJumpFrom
	.short	DoMultConstRegOff - SwitchJumpFrom
	.short	DoMultRegRegOff - SwitchJumpFrom
	.short	DoShiftConstRegOff - SwitchJumpFrom
	.short	DoShiftRegRegOff - SwitchJumpFrom

	.short	DoSetDevIDConst - SwitchJumpFrom
	.short	DoSetDevIDReg - SwitchJumpFrom

	.balign	32

JUMPTHROUGH_POINT_NO_EXEC

/* END_MARKER/RETURN(x) */
DoRet:
	rts			/* Return point */
	  mov	r1, r0		/* Exit code */

/* POKE8(A, VAL) */
DoPoke8:
	bra	DoPoking
	  mov.b	r2, @r1		/* *A = VAL */

/* POKE16(A, VAL) */
DoPoke16:
	bra	DoPoking
	  mov.w	r2, @r1		/* *A = VAL */

/* POKE32(A, VAL) */
DoPoke32:
	bra	DoPoking
	  mov.l	r2, @r1		/* *A = VAL */

/* OR8(A, VAL) */
DoOr8:
	mov.b	@r1, r3		/* *A */
	or	r2, r3		/* *A | VAL */
	bra	DoPoking
	  mov.b	r3, @r1		/* *A |= VAL */

/* OR16(A, VAL) */
DoOr16:
	mov.w	@r1, r3		/* *A */
	or	r2, r3		/* *A | VAL */
	bra	DoPoking
	  mov.w	r3, @r1		/* *A |= VAL */

JUMPTHROUGH_POINT_NO_EXEC

/* OR32(A, VAL) */
DoOr32:
	mov.l	@r1, r3		/* *A */
	or	r2, r3		/* *A | VAL */
	bra	DoPoking
	  mov.l	r3, @r1		/* *A |= VAL */

/* UPDATE8(A, AND, OR) */
DoUpdate8:
	mov.b	@r1, r3		/* *A */
	and	r2, r3		/* *A & AND */
	mov.l	@r4+, r2	/* Read OR */
	or	r2, r3		/* (*A & AND) | OR */
	bra	DoPoking
	  mov.b	r3, @r1		/* *A = ((*A & AND) | OR) */

/* UPDATE16(A, AND, OR) */
DoUpdate16:
	mov.w	@r1, r3		/* *A */
	and	r2, r3		/* *A & AND */
	mov.l	@r4+, r2	/* Read OR */
	or	r2, r3		/* (*A & AND) | OR */
	bra	DoPoking
	  mov.w	r3, @r1		/* *A = ((*A & AND) | OR) */

JUMPTHROUGH_POINT_NO_EXEC

/* UPDATE32(A, AND, OR) */
DoUpdate32:
	mov.l	@r1, r3		/* *A */
	and	r2, r3		/* *A & AND */
	mov.l	@r4+, r2	/* Read OR */
	or	r2, r3		/* (*A & AND) | OR */
	bra	DoPoking
	  mov.l	r3, @r1		/* *A = ((*A & AND) | OR) */

/* POKE_UPDATE32(A1, A2, AND, SHIFT, OR) */
DoPokeUpdate32:
	mov.l	@r2, r3		/* *A2 */
	mov.l	@r4+, r2	/* Read AND */
	and	r2, r3		/* *A2 & AND */
	mov.l	@r4+, r2	/* Read SHIFT */
	shld	r2, r3		/* (*A2 & AND) << SHIFT */
	mov.l	@r4+, r2	/* Read OR */
	or	r2, r3		/* ((*A2 & AND) << SHIFT) | OR */
	bra	DoPoking
	  mov.l	r3, @r1		/* *A1 = (((*A2 & AND) << SHIFT) | OR) */

JUMPTHROUGH_POINT_NO_EXEC

/* WHILE_NE8(A, AND, VAL) */
DoWhileNe8:
	mov.l	@r4+, r7	/* Read VAL */
1:	mov.b	@r1, r3		/* *A */
	extu.b	r3, r3		/* 32bit(*A) */
	and	r2, r3		/* *A & AND */
	cmp/eq	r3, r7		/* If ((*A & AND) == VAL) */
	bf	1b		/* Loop if false */
	bt	DoPoking2

/* WHILE_NE16(A, AND, VAL) */
DoWhileNe16:
	mov.l	@r4+, r7	/* Read VAL */
1:	mov.w	@r1, r3		/* *A */
	extu.w	r3, r3		/* 32bit(*A) */
	and	r2, r3		/* *A & AND */
	cmp/eq	r3, r7		/* If ((*A & AND) == VAL) */
	bf	1b		/* Loop if false */
	bra	DoPoking
	  nop

JUMPTHROUGH_POINT_NO_EXEC

/* WHILE_NE32(A, AND, VAL) */
DoWhileNe32:
	mov.l	@r4+, r7	/* Read VAL */
1:	mov.l	@r1, r3		/* *A */
	and	r2, r3		/* *A & AND */
	cmp/eq	r3, r7		/* If ((*A & AND) == VAL) */
	bf	1b		/* Loop if false */
	bra	DoPoking
	  nop

/* IF_EQ32(NESTLEVEL, A, AND, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfEq32:
	mov.l	@r1, r1		/* *A */
	and	r2, r1		/* *A & AND */
	mov.l	@r4+, r2	/* Read VAL */
	mov.l	@r4+, r3	/* Read distance to ELSE/ENDIF */
	cmp/eq	r2, r1		/* If ((*A & AND) == VAL) */
	bt	DoPoking2	/* Go ahead with these pokes */
	bra	DoPoking
	  add	r3, r4		/* Skip forward through pokes to ELSE or ENDIF */

JUMPTHROUGH_POINT_NO_EXEC

/* IF_GT32(NESTLEVEL, A, AND, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfGT32:
	mov.l	@r1, r1		/* *A */
	and	r2, r1		/* *A & AND */
	mov.l	@r4+, r2	/* Read VAL */
	mov.l	@r4+, r3	/* Read distance to ELSE/ENDIF */
	cmp/hi	r2, r1		/* If ((*A & AND) > VAL) */
	bt	DoPoking2	/* Go ahead with these pokes if true*/
	bra	DoPoking
	  add	r3, r4		/* Skip forward through pokes to ELSE or ENDIF */

/* ELSE(NESTLEVEL)
   ENDIF
   GOTO(LABEL)
   Note that NESTLEVEL/LABEL is not in the actual table, but there is a
   distance field following the opcode.
 */
DoElse:
	add	#-4, r4		/* We took 1 arg too many from r4 for this operation */
	bra	DoPoking
	  add	r1, r4		/* Skip through to ENDIF */

/* DELAY(ITERATIONS) */
DoDelay:
1:	dt	r1		/* If (--OPERATIONS == 0) */
	bf	1b
	bra	DoPoking
	  add	#-4, r4		/* We took 1 arg too many from r4 for a delay */

/* IF_DEVID_GE(NESTLEVEL, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfDevIDGE:
	cmp/hs	r1, r5		/* If (device ID >= VAL) */
	bt	DoPoking2	/* Go ahead with these pokes if true */
	bra	DoPoking
	  add	r2, r4

/* IF_DEVID_LE(NESTLEVEL, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfDevIDLE:
	cmp/hs	r5, r1		/* If (device ID <= VAL) */
	bt	1f		/* Go ahead with these pokes if true */
	add	r2, r4
1:	bra	DoPoking
	  nop

JUMPTHROUGH_POINT_NO_EXEC

/* POKE32_SERIES(ADDR, «NUM_VALS», VAL_LIST...)
   Note that NUM_VALS is not in the operation the user creates; it is
   automatically inserted in the data table by the poketable.h macro.
 */
DoPoke32Series:
1:	mov.l	@r4+, r7	/* Read a VAL */
	mov.l	r7, @r1 	/* Write VAL */
	add	#4, r1		/* ADDR++ */
	dt	r2		/* NUM_VALS-- */
	bf	1b		/* Loop if NUM_VALS != 0 */
	bra	DoPoking
	  nop

JUMPTHROUGH_POINT_NO_EXEC

/* POKE32_SET_RANGE(A, VAL, WORDS) */
DoPoke32SetRange:
	mov.l	@r4+, r7	/* Read WORDS */
	tst	r7, r7
	bt	2f
1:	mov.l	r2, @r1		/* *ADDR = VAL */
	add	#4, r1		/* ADDR++ */
	dt	r7		/* WORDS-- */
	bf	1b		/* Loop if NUM_VALS != 0 */
2:	/* Fall-through */
DoPoking2:
	bra	DoPoking
	  nop

/* AND32(A, VAL) */
DoAnd32:
	mov.l	@r1, r3
	and	r2, r3
	bra DoPoking
	  mov.l	r3, @r1

/* POKE32_REGOFF(A, REGOFF, VAL) */
DoPoke32RegOff:
	mov.l	@r4+, r7	/* Read VAL */
	mov	#0, r3
	cmp/eq	r3, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	bra	DoPoking
	  mov.l	r7, @r1		/* *(A + *REGOFF) = VAL */

JUMPTHROUGH_POINT_NO_EXEC

/* OR32_REGOFF(A, REGOFF, VAL) */
DoOr32RegOff:
	mov.l	@r4+, r7	/* Read VAL */
	mov	#0, r3
	cmp/eq	r3, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r2
	or	r7, r2
	bra	DoPoking
	  mov.l	r2, @r1		/* *(A + *REGOFF) |= VAL */

JUMPTHROUGH_POINT_NO_EXEC

/* AND32_REGOFF(A, REGOFF, VAL) */
DoAnd32RegOff:
	mov.l	@r4+, r7	/* Read VAL */
	mov	#0, r3
	cmp/eq	r3, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r2
	and	r7, r2
	bra DoPoking
	  mov.l	r2, @r1

JUMPTHROUGH_POINT_NO_EXEC

/* UPDATE32_REGOFF(A, REGOFF, AND, OR) */
DoUpdate32RegOff:
	mov	#0, r3
	cmp/eq	r3, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r4+, r2	/* Read AND */
	mov.l	@r1, r3
	and	r2, r3
	mov.l	@r4+, r2	/* Read OR */
	or	r2, r3
	bra	DoPoking
	  mov.l	r3, @r1		/* *(A + *REGOFF) = ((*(A + *REGOFF) & AND) | OR) */

JUMPTHROUGH_POINT_NO_EXEC

/* POKE_UPDATE32_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF, AND, SHIFT, OR) -
 * - param order in table:
 *   SRC, SRC_REGOFF, AND, SHIFT, OR, DST, DST_REGOFF
 */
DoPokeUpdate32RegOff:
	mov	#0, r7
	cmp/eq	r7, r2		/* Only use SRC_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r3		/* *(SRC + *SRC_REGOFF) */
	mov.l	@r4+, r2	/* Read AND */
	and	r2, r3
	mov.l	@r4+, r2	/* Read SHIFT */
	shld	r2, r3
	mov.l	@r4+, r2	/* Read OR */
	or	r2, r3

JUMPTHROUGH_POINT_EXEC

	mov.l	@r4+, r1	/* Read DST */
	mov.l	@r4+, r2	/* Read DST_REGOFF */
	cmp/eq	r7, r2		/* Only use DST_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	bra	DoPoking
	  mov.l	r3, @r1		/* *(DST + *DST_REGOFF) = (((*(SRC + *SRC_REGOFF) & AND) << SHIFT) | OR) */

JUMPTHROUGH_POINT_NO_EXEC

/* WHILE_NE32_REGOFF(A, REGOFF, AND, VAL) */
DoWhileNe32RegOff:
	mov	#0, r3
	cmp/eq	r3, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r4+, r2	/* Read AND */
	mov.l	@r4+, r7	/* Read VAL */
1:	mov.l	@r1, r3		/* *(A + *REGOFF) */
	and	r2, r3
	cmp/eq	r3, r7		/* If ((*(A + *REGOFF) & AND) == VAL) */
	bf	1b		/* Loop if false */
	bra	DoPoking
	  nop

JUMPTHROUGH_POINT_NO_EXEC

/* IF_EQ32_REGOFF(NESTLEVEL, A, REGOFF, AND, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfEq32RegOff:
	mov	#0, r3
	cmp/eq	r3, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r1		/* *(A + *REGOFF) */
	mov.l	@r4+, r2	/* Read AND */
	and	r2, r1
	mov.l	@r4+, r2	/* Read VAL */
	mov.l	@r4+, r3	/* Read distance to ELSE/ENDIF */
	cmp/eq	r2, r1		/* If ((*(A + *REGOFF) & AND) == VAL) */
	bt	DoPoking2	/* Go ahead with these pokes */
	bra	DoPoking
	  add	r3, r4		/* Skip forward through pokes to ELSE or ENDIF */

JUMPTHROUGH_POINT_NO_EXEC

/* IF_GT32_REGOFF(NESTLEVEL, A, REGOFF, AND, VAL)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following VAL.
 */
DoIfGT32RegOff:
	mov	#0, r3
	cmp/eq	r3, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r1		/* *(A + *REGOFF) */
	mov.l	@r4+, r2	/* Read AND */
	and	r2, r1
	mov.l	@r4+, r2	/* Read VAL */
	mov.l	@r4+, r3	/* Read distance to ELSE/ENDIF */
	cmp/hi	r2, r1		/* If ((*(A + *REGOFF) & AND) > VAL) */
	bt	DoPoking3	/* Go ahead with these pokes if true*/
	bra	DoPoking
	  add	r3, r4		/* Skip forward through pokes to ELSE or ENDIF */

JUMPTHROUGH_POINT_NO_EXEC

/* POKE32_SERIES_REGOFF(ADDR, REGOFF, «NUM_VALS», VAL_LIST...)
   Note that NUM_VALS is not in the operation the user creates; it is
   automatically inserted in the data table by the poketable.h macro.
 */
DoPoke32SeriesRegOff:
	mov	#0, r3
	cmp/eq	r3, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r4+, r2	/* Read NUM_VALS */
1:	mov.l	@r4+, r7	/* Read a VAL */
	mov.l	r7, @r1 	/* Write VAL */
	add	#4, r1		/* ADDR++ */
	dt	r2		/* NUM_VALS-- */
	bf	1b		/* Loop if NUM_VALS != 0 */
	bra	DoPoking
	  nop

	.balign	32

JUMPTHROUGH_POINT_NO_EXEC

/* POKE32_SET_RANGE_REGOFF(A, REGOFF, VAL, WORDS) */
DoPoke32SetRangeRegOff:
	mov	#0, r3
	cmp/eq	r3, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r4+, r2	/* Read VAL */
	mov.l	@r4+, r7	/* Read WORDS */
	tst	r7, r7
	bt	2f
1:	mov.l	r2, @r1		/* *ADDR = VAL */
	add	#4, r1		/* ADDR++ */
	dt	r7		/* WORDS-- */
	bf	1b		/* Loop if NUM_VALS != 0 */
2:	/* Fall-through */
DoPoking3:
	bra	DoPoking
	  nop

JUMPTHROUGH_POINT_NO_EXEC

/* COPY_REG(DST, SRC) - param order in table: SRC, DST */
DoCopyReg:
	mov.l	@r1, r1		/* r1 = *SRC */
	bra	DoPoking
	  mov.l	r1, @r2

/* OR_REG(DST, SRC) - param order in table: SRC, DST */
DoOrReg:
	mov.l	@r1, r1		/* r1 = *SRC */
	mov.l	@r2, r3		/* r3 = *DST */
	or	r1, r3
	bra	DoPoking
	  mov.l	r3, @r2

JUMPTHROUGH_POINT_NO_EXEC

/* AND_REG(DST, SRC) - param order in table: SRC, DST */
DoAndReg:
	mov.l	@r1, r1		/* r1 = *SRC */
	mov.l	@r2, r3		/* r3 = *DST */
	and	r1, r3
	bra	DoPoking
	  mov.l	r3, @r2

/* NOT_REG(A) */
DoNotReg:
	mov.l	@r1, r2
	add	#-4, r4		/* We took 1 arg too many from r4 for a NOT_REG */
	not	r2, r2
	bra	DoPoking
	  mov.l	r2, @r1

/* IF_EQ_REG(NESTLEVEL, R1, R2)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following the stated parameters.
 */
DoIfEqReg:
	mov.l	@r1, r1
	mov.l	@r2, r2
	mov.l	@r4+, r3	/* Read distance to ELSE/ENDIF */
	cmp/eq	r2, r1		/* If (*R1 == *R2) */
	bt	DoPoking3	/* Go ahead with these pokes */
	bra	DoPoking
	  add	r3, r4		/* Skip forward through pokes to ELSE or ENDIF */

JUMPTHROUGH_POINT_NO_EXEC

/* IF_GT_REG(NESTLEVEL, R1, R2)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following the stated parameters.
 */
DoIfGTReg:
	mov.l	@r1, r1
	mov.l	@r2, r2
	mov.l	@r4+, r3	/* Read distance to ELSE/ENDIF */
	cmp/hi	r2, r1		/* If (*R1 > *R2) */
	bt	DoPoking3	/* Go ahead with these pokes if true*/
	bra	DoPoking
	  add	r3, r4		/* Skip forward through pokes to ELSE or ENDIF */

/* COPY_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF)
 * - param order in table:
 *   SRC, SRC_REGOFF, DST, DST_REGOFF
 */
DoCopyRegRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use SRC_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r1		/* *(SRC + *SRC_REGOFF) */
	mov.l	@r4+, r2	/* Read DST */
	mov.l	@r4+, r3	/* Read DST_REGOFF */
	cmp/eq	r3, r6		/* Only use DST_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r3, r3
	add	r3, r2
1:	bra	DoPoking
	  mov.l	r1, @r2

JUMPTHROUGH_POINT_NO_EXEC

/* OR_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF)
 * - param order in table:
 *   SRC, SRC_REGOFF, DST, DST_REGOFF
 */
DoOrRegRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use SRC_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r1		/* *(SRC + *SRC_REGOFF) */
	mov.l	@r4+, r2	/* Read DST */
	mov.l	@r4+, r3	/* Read DST_REGOFF */
	cmp/eq	r3, r6		/* Only use DST_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r3, r3
	add	r3, r2
1:	mov.l	@r2, r3
	or	r1, r3
	bra	DoPoking
	  mov.l	r3, @r2

JUMPTHROUGH_POINT_NO_EXEC

/* AND_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF)
 * - param order in table:
 *   SRC, SRC_REGOFF, DST, DST_REGOFF
 */
DoAndRegRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use SRC_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r1		/* *(SRC + *SRC_REGOFF) */
	mov.l	@r4+, r2	/* Read DST */
	mov.l	@r4+, r3	/* Read DST_REGOFF */
	cmp/eq	r6, r3		/* Only use DST_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r3, r3
	add	r3, r2
1:	mov.l	@r2, r3		/* *(DST + *DST_REGOFF) */
	and	r1, r3
	bra	DoPoking
	  mov.l	r3, @r2

JUMPTHROUGH_POINT_NO_EXEC
	.balign	32
JUMPTHROUGH_POINT_NO_EXEC

/* NOT_REG_REGOFF(A, A_REGOFF) */
DoNotRegRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use A1_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r2		/* *(A + *A_REGOFF) */
	not	r2, r2
	bra	DoPoking
	  mov.l	r2, @r1

JUMPTHROUGH_POINT_NO_EXEC

/* IF_EQ_REG_REGOFF(NESTLEVEL, A1, A1_REGOFF, A2, A2_REGOFF)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following the last parameter.
 */
DoIfEqRegRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use A1_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r1
	mov.l	@r4+, r7	/* Read A2 */
	mov.l	@r4+, r3	/* Read A2_REGOFF */
	cmp/eq	r6, r3		/* Only use A2_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r3, r3
	add	r3, r7
1:	mov.l	@r7, r7
	mov.l	@r4+, r3	/* Read distance to ELSE/ENDIF */
	cmp/eq	r7, r1		/* If (*A1 == *A2) */
	bt	DoPoking3	/* Go ahead with these pokes */
	bra	DoPoking
	  add	r3, r4		/* Skip forward through pokes to ELSE or ENDIF */

JUMPTHROUGH_POINT_NO_EXEC

/* IF_GT_REG_REGOFF(NESTLEVEL, A1, A1_REGOFF, A2, A2_REGOFF)
   Note that NESTLEVEL is not in the actual table, but there is a distance
   field following the last parameter.
 */
DoIfGTRegRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use A1_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r1
	mov.l	@r4+, r7	/* Read A2 */
	mov.l	@r4+, r3	/* Read A2_REGOFF */
	cmp/eq	r6, r3		/* Only use A2_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r3, r3
	add	r3, r7
1:	mov.l	@r7, r7
	mov.l	@r4+, r3	/* Read distance to ELSE/ENDIF */
	cmp/hi	r7, r1		/* If (*A1 > *A2) */
	bt	DoPoking4	/* Go ahead with these pokes */
	bra	DoPoking
	  add	r3, r4		/* Skip forward through pokes to ELSE or ENDIF */

JUMPTHROUGH_POINT_NO_EXEC

DoPoking4:
	bra	DoPoking
	  nop

/* ADD_CONST(A, CONST) */
DoAddConst:
	mov.l	@r1, r3
	add	r2, r3
	bra	DoPoking
	  mov.l	r3, @r1		/* *A += CONST */

/* ADD_REG(DST, SRC) - param order in table: SRC, DST */
DoAddReg:
	mov.l	@r1, r1
	mov.l	@r2, r3
	add	r3, r1
	bra	DoPoking
	  mov.l	r1, @r2		/* *DST += *SRC */

/* MULT_CONST(A, CONST) */
DoMultConst:
	mov.l	@r1, r3
	mul.l	r2, r3		/* MACL = Signed multiplication result */
	sts	macl, r3
	bra	DoPoking
	  mov.l	r3, @r1		/* *A *= CONST */

JUMPTHROUGH_POINT_NO_EXEC

/* MULT_REG(DST, SRC) - param order in table: SRC, DST */
DoMultReg:
	mov.l	@r1, r1
	mov.l	@r2, r3
	mul.l	r1, r3		/* MACL = Signed multiplication result */
	sts	macl, r3
	bra	DoPoking
	  mov.l	r3, @r2		/* *DST *= *SRC */

/* SHIFT_CONST(A, CONST) */
DoShiftConst:
	mov.l	@r1, r3
	shld	r2, r3		/* r3 <<= r2 (signed) */
	bra	DoPoking
	  mov.l	r3, @r1		/* *A <<= CONST */

/* SHIFT_REG(A, SHIFTBY_REG) - param order in table: SHIFTBY_REG, A */
DoShiftReg:
	mov.l	@r1, r1
	mov.l	@r2, r3
	shld	r1, r3
	bra	DoPoking
	  mov.l	r3, @r2		/* *A <<= *SHIFTBY_REG */

/* ADD_CONST_REGOFF(A, REGOFF, CONST) */
DoAddConstRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r3
	mov.l	@r4+, r2	/* Read CONST */
	add	r2, r3
	bra	DoPoking
	  mov.l	r3, @r1		/* *A += CONST */

JUMPTHROUGH_POINT_NO_EXEC

/* ADD_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFFF)
 * - param order in table: SRC, SRC_REGOFF, DST, DST_REGOFF
 */
DoAddRegRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use SRC_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r1
	mov.l	@r4+, r2	/* Read DST */
	mov.l	@r4+, r3	/* Read DST_REGOFF */
	cmp/eq	r6, r3		/* Only use DST_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r3, r3
	add	r3, r2
1:	mov.l	@r2, r3
	add	r1, r3
	bra	DoPoking
	  mov.l	r3, @r2		/* *(DST + *DST_REGOFF) += *(SRC + *SRC_REGOFF) */

JUMPTHROUGH_POINT_NO_EXEC

/* MULT_CONST_REGOFF(A, REGOFF, CONST) */
DoMultConstRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r3
	mov.l	@r4+, r2	/* Read CONST */
	mul.l	r2, r3		/* MACL = Signed multiplication result */
	sts	macl, r3
	bra	DoPoking
	  mov.l	r3, @r1		/* *A *= CONST */

JUMPTHROUGH_POINT_NO_EXEC

/* MULT_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF, SRC2)
 * - param order in table: SRC, SRC_REGOFF, DST, DST_REGOFF
 */
DoMultRegRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use SRC_REGOFF if it's not 0 */
	bt	1f

JUMPTHROUGH_POINT_EXEC

	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r1
	mov.l	@r4+, r7	/* Read DST */
	mov.l	@r4+, r3	/* Read DST_REGOFF */
	cmp/eq	r6, r3		/* Only use DST_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r3, r3
	add	r3, r7
1:	mov.l	@r7, r3
	mul.l	r1, r3		/* MACL = Signed multiplication result */
	sts	macl, r3
	bra	DoPoking
	  mov.l	r3, @r7		/* *DST *= *SRC */

/* SHIFT_CONST_REGOFF(A, REGOFF, CONST) */
DoShiftConstRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r3
	mov.l	@r4+, r2	/* Read CONST */
	shld	r2, r3
	bra	DoPoking
	  mov.l	r3, @r1		/* *A <<= CONST */

JUMPTHROUGH_POINT_NO_EXEC

/* SHIFT_REG_REGOFF(A, A_REGOFF, SHIFTBY_REG, SHIFTBY_REG_REGOFF)
 * - param order in table: SHIFTBY_REG, SHIFTBY_REG_REGOFF, A, A_REGOFF
 */
DoShiftRegRegOff:
	mov	#0, r6
	cmp/eq	r6, r2		/* Only use SHIFTBY_REG_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r2, r2
	add	r2, r1
1:	mov.l	@r1, r1
	mov.l	@r4+, r2	/* Read A */
	mov.l	@r4+, r3	/* Read A_REGOFF */
	cmp/eq	r6, r3		/* Only use A_REGOFF if it's not 0 */
	bt	1f
	mov.l	@r3, r3
	add	r3, r2
1:	mov.l	@r2, r3
	shld	r1, r3
	bra	DoPoking
	  mov.l	r3, @r2		/* *(A + *A_REGOFF) <<= *(SHIFTBY_REG + *SHIFTBY_REG_REGOFF) */

JUMPTHROUGH_POINT_NO_EXEC

/* SET_DEVID_CONST(VAL) */
DoSetDevIDConst:
	mov	r1, r5
	bra	DoPoking
	  add	#-4, r4		/* We took 1 arg too many from r4 */

/* SET_DEVID_REG(ADDR, AND, SHIFT) */
DoSetDevIDReg:
	mov.l	@r1, r1
	and	r2, r1
	mov.l	@r4+, r2	/* Read SHIFT */
	shld	r2, r1
	bra	DoPoking
	  mov	r1, r5

JUMPTHROUGH_END

	.popsection
