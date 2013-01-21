/*
 * File     : poketable.h
 * Synopsis : Header containing macro definitions needed for poke tables/SysInit
 *            Ops and the poke loop code which uses them.
 * Version  : 4.1
 *
 * Copyright (c) 2004-2012 STMicroelectronics Limited.  All rights reserved.
 *
 * Version numbering:
 *   - Encoded in a 32-bit value as 0x00<MAJOR>00<MINOR>, e.g 3.1 == 0x00030001
 *   - POKE_TABLE_VERSION may be defined to restrict available operations
 *     to those suitable to a locked version of the pokeLoop code.
 *   - Major version number is updated where changes to the poke table format
 *     require the pokeLoop code to be updated (e.g. changed OpCode numbers or
 *     new operations).
 *   - Minor version number is updated for bug fixes or backward and forward
 *     compatible operation names (those which can be implemented using an
 *     existing piece of pokeLoop code).
 */

#ifndef __POKETABLE_H_
#define __POKETABLE_H_

/* Opcode values.
   New opcodes index numbers should follow sequentially with no gaps.
 */
#define OP_END_POKES					0
#define OP_POKE8					1
#define OP_POKE16					2
#define OP_POKE32					3
#define OP_OR8						4
#define OP_OR16						5
#define OP_OR32						6
#define OP_UPDATE8					7
#define OP_UPDATE16					8
#define OP_UPDATE32					9
#define OP_POKE_UPDATE32				10
#define OP_WHILE_NE8					11
#define OP_WHILE_NE16					12
#define OP_WHILE_NE32					13
#define OP_IF_EQ32					14
#define OP_IF_GT32					15
#define OP_ELSE						16
#define OP_DELAY					17
#define OP_IF_DEVID_GE					18
#define OP_IF_DEVID_LE					19
#define OP_POKE32_SERIES				20
#define OP_POKE32_SET_RANGE				21

#define OP_AND32					22

#define OP_POKE32_REGOFF				23
#define OP_OR32_REGOFF					24
#define OP_AND32_REGOFF					25
#define OP_UPDATE32_REGOFF				26
#define OP_POKE_UPDATE32_REGOFF				27
#define OP_WHILE_NE32_REGOFF				28
#define OP_IF_EQ32_REGOFF				29
#define OP_IF_GT32_REGOFF				30
#define OP_POKE32_SERIES_REGOFF				31
#define OP_POKE32_SET_RANGE_REGOFF			32

#define OP_COPY_REG					33
#define OP_OR_REG					34
#define OP_AND_REG					35
#define OP_NOT_REG					36
#define OP_IF_EQ_REG					37
#define OP_IF_GT_REG					38

#define OP_COPY_REG_REGOFF				39
#define OP_OR_REG_REGOFF				40
#define OP_AND_REG_REGOFF				41
#define OP_NOT_REG_REGOFF				42
#define OP_IF_EQ_REG_REGOFF				43
#define OP_IF_GT_REG_REGOFF				44

#define OP_ADD_CONST					45
#define OP_ADD_REG					46
#define OP_MULT_CONST					47
#define OP_MULT_REG					48
#define OP_SHIFT_CONST					49
#define OP_SHIFT_REG					50

#define OP_ADD_CONST_REGOFF				51
#define OP_ADD_REG_REGOFF				52
#define OP_MULT_CONST_REGOFF				53
#define OP_MULT_REG_REGOFF				54
#define OP_SHIFT_CONST_REGOFF				55
#define OP_SHIFT_REG_REGOFF				56

#define OP_SET_DEVID_CONST				57
#define OP_SET_DEVID_REG				58

#ifdef __ASSEMBLER__

#ifdef NO_WARNING_OR_ERROR_DIRECTIVES
/* Older versions of GNU binutils do not support the .warning and .error
   directives; .print can be used, but doesn't look as good!
 */
#define ASM_WARNING(STR)	.print "Warning:"; .print STR
#define ASM_ERROR(STR)		.print STR; .err
#else
#define ASM_WARNING(STR)	.warning STR
#define ASM_ERROR(STR)		.error STR
#endif /* NO_WARNING_OR_ERROR_DIRECTIVES */

/* Poke table commands */
#define POKE8(A, VAL)					.long OP_POKE8, A, VAL
#define POKE16(A, VAL)					.long OP_POKE16, A, VAL
#define POKE32(A, VAL)					.long OP_POKE32, A, VAL
#define OR8(A, VAL)					.long OP_OR8, A, VAL
#define OR16(A, VAL)					.long OP_OR16, A, VAL
#define OR32(A, VAL)					.long OP_OR32, A, VAL
#define UPDATE8(A, AND, OR)				.long OP_UPDATE8, A, AND, OR
#define UPDATE16(A, AND, OR)				.long OP_UPDATE16, A, AND, OR
#define UPDATE32(A, AND, OR)				.long OP_UPDATE32, A, AND, OR
#ifdef NO_ASM_MACROS
#define POKE_UPDATE32(DST, SRC, AND, SHIFT, OR)		.long OP_POKE_UPDATE32, DST, SRC, AND, SHIFT, OR
#else /* !NO_ASM_MACROS */
#define POKE_UPDATE32(DST, SRC, AND, SHIFT, OR)		POKE_UPDATE32_M DST, SRC, AND, SHIFT, OR
.macro POKE_UPDATE32_M dst:req, src:req, and:req, shift:req, or:req
	.if \shift < -31 || \shift > 31
		.error "POKE_UPDATE32 with a shift value outside the range -31 to 31 is not allowed as the shift result would be undefined"
	.endif
	.long OP_POKE_UPDATE32, \dst, \src, \and, \shift, \or
.endm
#endif /* NO_ASM_MACROS */
#define WHILE_NE8(A, AND, VAL)				.long OP_WHILE_NE8, A, AND, VAL; .if (VAL > 0xFF); ASM_ERROR("Value VAL in WHILE_NE8 should fit in 8 bits"); .endif
#define WHILE_NE16(A, AND, VAL)				.long OP_WHILE_NE16, A, AND, VAL; .if (VAL > 0xFFFF); ASM_ERROR("Value VAL in WHILE_NE16 should fit in 16 bits"); .endif
#define WHILE_NE32(A, AND, VAL)				.long OP_WHILE_NE32, A, AND, VAL
#define IF_EQ32(NESTLEVEL, A, AND, VAL)			.long OP_IF_EQ32, A, AND, VAL, (NESTLEVEL ## f - .)
#define IF_GT32(NESTLEVEL, A, AND, VAL)			.long OP_IF_GT32, A, AND, VAL, (NESTLEVEL ## f - .)
/* An explicit ELSE will skip the OP_ELSE embedded in the ENDIF to make things faster */
#define ELSE(NESTLEVEL)					.long OP_ELSE; NESTLEVEL: ; .long (NESTLEVEL ## f - .)
/* ENDIF includes an OP_ELSE so that we end up at the correct position regardless of whether there is an explicit ELSE in the IF construct */
#define ENDIF(NESTLEVEL)				.long OP_ELSE; NESTLEVEL: ; .long 0
#define DELAY(ITERATIONS)				.long OP_DELAY, (ITERATIONS + 1)

#if !(defined(POKE_TABLE_VERSION) && defined(__st200__) && (POKE_TABLE_VERSION < 0x00030001))
/* The 2nd argument to the poke loop code (in R5 for ST40, $r0.17 for ST200, or
 * R1 for ARM) must be the device ID to compare against for these operations to
 * work - the poke loop code does not try to retrieve the device ID itself.
 * Note: IF_DEVID_LE() incorrectly performed a less than rather than a less than
 * or equal comparison in version 3.0 of the ST40 assembly pokeLoop.
 * Neither operation worked at all in version 3.0 of the ST200 assembly pokeLoop.
 */
#define IF_DEVID_GE(NESTLEVEL, VAL)			.long OP_IF_DEVID_GE, VAL, (NESTLEVEL ## f - .)
#define IF_DEVID_LE(NESTLEVEL, VAL)			.long OP_IF_DEVID_LE, VAL, (NESTLEVEL ## f - .)
#endif /* !(defined(POKE_TABLE_VERSION) && defined(__st200__) && (POKE_TABLE_VERSION < 0x00030001)) */

/* The end marker needs two extra entries which get read by the code, but are
 * were not used (although the first is now used if wishing to return a result
 * to the pokeLoop caller).
 * Returning a result is also supported below.
 */
#define END_MARKER					.long OP_END_POKES, 0, 0

#if !defined(POKE_TABLE_VERSION) || (POKE_TABLE_VERSION > 0x00030000)
/* Added in version 3.1, but will not cause problems with version 3.0 pokeLoop
 * code, with one exception; backwards GOTO() will not work with the C pokeLoop
 * version 3 (as found in BootROM 1.0 SoCs).
 * LABELs should be unique names which do not clash with others in the
 * compilation unit.  Numeric labels cannot be used in case of clashes with
 * IF* operation NESTLEVELs.
 */
#define GOTO(LABEL)					.long OP_ELSE, (LABEL - 4 - .)
#define RETURN(RESULT)					.long OP_END_POKES, RESULT, 0
#endif /* !defined(POKE_TABLE_VERSION) || (POKE_TABLE_VERSION > 0x00030000) */

/* We hide most warnings beyond this point as there is no other way to hide the
 * warning that named variadic macros are not supported in ISO C.
 */
#pragma GCC system_header
#if !defined(POKE_TABLE_VERSION) || (POKE_TABLE_VERSION >= 0x00030002)
/* Added in version 3.2... */
#define POKE32_SERIES(A, ARGS...)			.long OP_POKE32_SERIES, A, ((9191f - .) >> 2) - 1, ARGS ; 9191:
#define POKE32_SET_RANGE(A, VAL, WORDS)			.long OP_POKE32_SET_RANGE, A, VAL, WORDS
#else
/* Older POKE_TABLE_VERSION; use POKE32() to process the series/range using
 * repeated POKE32() operations...unless NO_ASM_MACROS was requested, if which
 * case we cannot do them.
 */
#ifndef NO_ASM_MACROS
#define POKE32_SERIES(A, ARGS...)			POKE32_SERIES_M A, ARGS
.macro POKE32_SERIES_M addr:req, val_list:vararg
	.ifnb \val_list
9191:
		.irp val,\val_list
			.long	OP_POKE32, \addr + ((. - 9191b - 4) / 3), \val
		.endr
	.endif
.endm
#define POKE32_SET_RANGE(A, VAL, WORDS)			POKE32_SET_RANGE_M A, VAL, WORDS
.macro POKE32_SET_RANGE_M addr:req, val:req, words:req
9191:
	.rept \words
		.long	OP_POKE32, \addr + ((. - 9191b - 4) / 3), \val
	.endr
.endm
#endif /* !NO_ASM_MACROS */
#endif /* !defined(POKE_TABLE_VERSION) || (POKE_TABLE_VERSION >= 0x00030002) */

#if !defined(POKE_TABLE_VERSION) || (POKE_TABLE_VERSION >= 0x00040000)
/* Added in version 4.0 - many new operations that cannot be represented using 3.x operations */

#define AND32(A, VAL)					.long OP_AND32, A, VAL

#define POKE32_REGOFF(A, REGOFF, VAL)			.long OP_POKE32_REGOFF, A, REGOFF, VAL
#define OR32_REGOFF(A, REGOFF, VAL)			.long OP_OR32_REGOFF, A, REGOFF, VAL
#define AND32_REGOFF(A, REGOFF, VAL)			.long OP_AND32_REGOFF, A, REGOFF, VAL
#define UPDATE32_REGOFF(A, REGOFF, AND, OR)		.long OP_UPDATE32_REGOFF, A, REGOFF, AND, OR
#define POKE_UPDATE32_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF, AND, SHIFT, OR) \
							.long OP_POKE_UPDATE32_REGOFF, SRC, SRC_REGOFF, AND, SHIFT, OR, DST, DST_REGOFF
#define WHILE_NE32_REGOFF(A, REGOFF, AND, VAL)		.long OP_WHILE_NE32_REGOFF, A, REGOFF, AND, VAL
#define IF_EQ32_REGOFF(NESTLEVEL, A, REGOFF, AND, VAL)	.long OP_IF_EQ32_REGOFF, A, REGOFF, AND, VAL, (NESTLEVEL ## f - .)
#define IF_GT32_REGOFF(NESTLEVEL, A, REGOFF, AND, VAL)	.long OP_IF_GT32_REGOFF, A, REGOFF, AND, VAL, (NESTLEVEL ## f - .)
#define POKE32_SERIES_REGOFF(A, REGOFF, ARGS...)	.long OP_POKE32_SERIES_REGOFF, A, REGOFF, ((9191f - .) >> 2) - 1, ARGS ; 9191:
#define POKE32_SET_RANGE_REGOFF(A, REGOFF, VAL, WORDS)	.long OP_POKE32_SET_RANGE_REGOFF, A, REGOFF, VAL, WORDS

#define COPY_REG(DST, SRC)				.long OP_COPY_REG, SRC, DST
#define OR_REG(DST, SRC)				.long OP_OR_REG, SRC, DST
#define AND_REG(DST, SRC)				.long OP_AND_REG, SRC, DST
#define NOT_REG(A)					.long OP_NOT_REG, A
#define IF_EQ_REG(NESTLEVEL, A1, A2)			.long OP_IF_EQ_REG, A1, A2, (NESTLEVEL ## f - .)
#define IF_GT_REG(NESTLEVEL, A1, A2)			.long OP_IF_GT_REG, A1, A2, (NESTLEVEL ## f - .)

#define COPY_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF) \
							.long OP_COPY_REG_REGOFF, SRC, SRC_REGOFF, DST, DST_REGOFF
#define OR_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF) \
							.long OP_OR_REG_REGOFF, SRC, SRC_REGOFF, DST, DST_REGOFF
#define AND_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF) \
							.long OP_AND_REG_REGOFF, SRC, SRC_REGOFF, DST, DST_REGOFF
#define NOT_REG_REGOFF(A, A_REGOFF)			.long OP_NOT_REG_REGOFF, A, A_REGOFF
#define IF_EQ_REG_REGOFF(NESTLEVEL, A1, A1_REGOFF, A2, A2_REGOFF) \
							.long OP_IF_EQ_REG_REGOFF, A1, A1_REGOFF, A2, A2_REGOFF, (NESTLEVEL ## f - .)
#define IF_GT_REG_REGOFF(NESTLEVEL, A1, A1_REGOFF, A2, A2_REGOFF) \
							.long OP_IF_GT_REG_REGOFF, A1, A1_REGOFF, A2, A2_REGOFF, (NESTLEVEL ## f - .)

#define ADD_CONST(A, CONST)				.long OP_ADD_CONST, A, CONST
#define ADD_REG(DST, SRC)				.long OP_ADD_REG, SRC, DST
#define MULT_CONST(A, CONST)				.long OP_MULT_CONST, A, CONST
#define MULT_REG(DST, SRC)				.long OP_MULT_REG, SRC, DST
#ifdef NO_ASM_MACROS
#define SHIFT_CONST(A, CONST)				.long OP_SHIFT_CONST, A, CONST
#else /* !NO_ASM_MACROS */
#define SHIFT_CONST(A, CONST)				SHIFT_CONST_M A, CONST
.macro SHIFT_CONST_M addr:req, shift:req
	.if \shift < -31 || \shift > 31
		.error "SHIFT_CONST with a shift value outside the range -31 to 31 is not allowed as the shift result would be undefined"
	.endif
	.long OP_SHIFT_CONST, \addr, \shift
.endm
#endif /* NO_ASM_MACROS */
#define SHIFT_REG(A, SHIFTBY_REG)			.long OP_SHIFT_REG, SHIFTBY_REG, A

#define ADD_CONST_REGOFF(A, REGOFF, CONST)		.long OP_ADD_CONST_REGOFF, A, REGOFF, CONST
#define ADD_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF) \
							.long OP_ADD_REG_REGOFF, SRC, SRC_REGOFF, DST, DST_REGOFF
#define MULT_CONST_REGOFF(A, REGOFF, CONST)		.long OP_MULT_CONST_REGOFF, A, REGOFF, CONST
#define MULT_REG_REGOFF(DST, DST_REGOFF, SRC, SRC_REGOFF) \
							.long OP_MULT_REG_REGOFF, SRC, SRC_REGOFF, DST, DST_REGOFF

#ifdef NO_ASM_MACROS
#define SHIFT_CONST_REGOFF(A, REGOFF, CONST)		.long OP_SHIFT_CONST_REGOFF, A, REGOFF, CONST
#else /* !NO_ASM_MACROS */
#define SHIFT_CONST_REGOFF(A, REGOFF, CONST)		SHIFT_CONST_REGOFF_M A, REGOFF, CONST
.macro SHIFT_CONST_REGOFF_M addr:req, regOff:req, shift:req
	.if \shift < -31 || \shift > 31
		.error "SHIFT_CONST_REGOFF with a shift value outside the range -31 to 31 is not allowed as the shift result would be undefined"
	.endif
	.long OP_SHIFT_CONST_REGOFF, \addr, \regOff, \shift
.endm
#endif /* NO_ASM_MACROS */
#define SHIFT_REG_REGOFF(A, A_REGOFF, SHIFTBY_REG, SHIFTBY_REG_REGOFF) \
							.long OP_SHIFT_REG_REGOFF, SHIFTBY_REG, SHIFTBY_REG_REGOFF, A, A_REGOFF

#define SET_DEVID_CONST(VAL)				.long OP_SET_DEVID_CONST, VAL
#ifdef NO_ASM_MACROS
#define SET_DEVID_REG(ADDR, AND, SHIFT)			.long OP_SET_DEVID_REG, ADDR, AND, SHIFT
#else /* !NO_ASM_MACROS */
#define SET_DEVID_REG(ADDR, AND, SHIFT)			SET_DEVID_REG_M ADDR, AND, SHIFT
.macro SET_DEVID_REG_M addr:req, and:req, shift:req
	.if \shift < -31 || \shift > 31
		.error "SET_DEVID_REG with a shift value outside the range -31 to 31 is not allowed as the shift result would be undefined"
	.endif
	.long OP_SET_DEVID_REG, \addr, \and, \shift
.endm
#endif /* NO_ASM_MACROS */

#else /* POKE_TABLE_VERSION < 0x00040000 */
/* Represent those operations which we can in older poke table syntax */
#define AND32(A, VAL)					UPDATE32(A, VAL, 0)
#define COPY_REG(DST, SRC)				POKE_UPDATE32(DST, SRC, 0xffffffff, 0, 0)
#define SHIFT_CONST(A, CONST)				POKE_UPDATE32(A, A, 0xffffffff, CONST, 0)
#endif /* !defined(POKE_TABLE_VERSION) || (POKE_TABLE_VERSION > 0x00040000) */


#endif /* __ASSEMBLER__ */

#endif /* !__POKETABLE_H_ */
