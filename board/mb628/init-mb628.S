#include "asm/stx7141reg.h"
#include "../../cpu/sh/init_ram.S"

/*
 * This is derived from STMicroelectronics gnu toolchain example:
 *   sh-superh-elf/examples/os21/romdynamic/memory_mb628.S
 */

/*
 * The poke table is a series of long words, in the format
 *
 *	opcode, address, operand, ...
 *
 * An opcode of 0 marks the table end
 */

/*
 * For compatibility with old poke table code, we define some of the
 * new names, to map onto the old names. Ultimately, the old poke table
 * code will be updated to the "new order", and we can just delete
 * the following mappings.
 */
#define POKE8(A, VAL)				POKE_CHAR(A, VAL)
#define POKE16(A, VAL)				POKE_SHORT(A, VAL)
#define POKE32(A, VAL)				POKE_LONG(A, VAL)
#define OR32(A, VAL)				OR_LONG(A, VAL)
#define UPDATE32(A, AND, OR)			UPDATE_LONG(A, AND, OR)
#define POKE_UPDATE32(A1, A2, AND, SHIFT, OR)	POKE_UPDATE_LONG(A1, A2, AND, SHIFT, OR)
#define WHILE_NE32(A, AND, VAL)			WHILE_NE(A, AND, VAL)
#define DELAY(VAL)				/* do nothing */

.balign 32


__memory_setup_table:

/* QQQ - TO DO */

#ifdef CONFIG_SH_SE_MODE
	/*
	 * Note that we also manually need to move the LMI0 and LMI1 base addresses to
	 * their 32-bit SE mode locations as defined in the datasheet and change the
	 * 'upper bound addresses' (in row attribute registers) for the LMIs.
	 */
#if 0	/* QQQ - DELETE */
	UPDATE32(STX7141_SYSCONF_SYS_CFG38, 0xFFFFFF00, 0x00000040)
	UPDATE32(STX7141_SYSCONF_SYS_CFG39, 0xFFFFFF00, 0x00000080)
	UPDATE32(ST40_LMI_SDRA0_0(0), 0x001FFFFF, 0x48000000)
	UPDATE32(ST40_LMI_SDRA1_0(0), 0x001FFFFF, 0x48000000)
	UPDATE32(ST40_LMI_SDRA0_0(1), 0x001FFFFF, 0x88000000)
	UPDATE32(ST40_LMI_SDRA1_0(1), 0x001FFFFF, 0x88000000)
#endif	/* QQQ - DELETE */
#endif	/* CONFIG_SH_SE_MODE */

	END_MARKER

__memory_setup_table_end:

	.end
