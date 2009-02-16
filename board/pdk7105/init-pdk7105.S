#include "asm/stx7105reg.h"
#include "../../cpu/sh/init_ram.S"

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

	/*
	 * include the set of pokes automatically generated from
	 * the ROMGEN utility, specifically for this board.
	 */
#include "romgen-pdk7105.inc"

#ifdef CONFIG_SH_SE_MODE
	/*
	 * Note that we also manually need to move the LMI base addresses to
	 * their 32-bit SE mode locations as defined in the datasheet and change the
	 * 'upper bound addresses' (in row attribute registers) for the LMIs.
	 */
	UPDATE32(STX7105_SYSCONF_SYS_CFG38, 0xFFFFFF00, 0x00000040)
	/* Change LMI upper bound addresses
	 * Upper LMI addr=0x40000000 + 512MiB=0x60000000
	 */
	UPDATE32(ST40_LMI_SDRA0_0(), 0x001FFFFF, 0x60000000)
	UPDATE32(ST40_LMI_SDRA1_0(), 0x001FFFFF, 0x60000000)
#endif	/* CONFIG_SH_SE_MODE */

	END_MARKER

__memory_setup_table_end:

	.end
