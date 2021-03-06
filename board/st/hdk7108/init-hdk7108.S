/*
 * The poke table is a series of long words, in the format
 *
 *	opcode, address, operand, ...
 *
 * An opcode of 0 marks the table end
 */


#include <stm/stx7108reg.h>
#include "../../../arch/st40/cpu/init_ram.S"


	.section .data.init, "a"

	.balign 32

__memory_setup_table:

	/*
	 * include the appropriate set of "GDB pokes", (usually) generated
	 * using the "romgen" utility, from a known good target-pack.
	 *
	 * This file may be regenerated by executing the following
	 * command (redirecting the output), *without* needing any
	 * access to either a MicroConnect or a target board:
	 *
	 * host% romgen --show-comment dummy:hdk7108stx7108:host
	 *
	 * Where "dummy" need not correspond to any real IP address on
	 * your network.  In addition, note that "hdk7108stx7108" should
	 * substituted if you have a custom target-pack connect command.
	 *
	 * If you wish to boot from SPI Serial Flash in "Quad-Mode" (i.e.
	 * using 4 data wires), then you should also post-pend the "romgen"
	 * command with the "spi_4x=1" option. e.g.
	 *
	 * host% romgen --show-comment dummy:hdk7108stx7108:host,spi_4x=1
	 *
	 * If you wish to have the 2 LMIs being physically contiguous,
	 * then you should also post-pend the "romgen" command with
	 * the "lmi_contig=1" option. e.g.
	 *
	 * host% romgen --show-comment dummy:hdk7108stx7108:host,lmi_contig=1
	 *
	 * At the time of writing this (July 2010) the output from "romgen"
	 * needs to be edited slightly.  The edit is just two add two lines
	 * to comment out all the PMB + 32-bit mode configuration. The lines
	 * to comment out start with the pokes that setup the PMB arrays,
	 * and end with pokes that configure the CCN registers.
	 * That is, the following two lines should be added:
	 *		#if 0
	 *		#endif
	 * See the supplied version of the ".romgen" file, to see where
	 * these have been added, if there is any doubt.
	 *
	 * Update (November 2010), an additional modification is required,
	 * such that the changes to the SPI registers are *only*
	 * to be performed if we are actively booting from SPI.
	 * Please note the lines with "QQQ - ... BY HAND"
	 *
	 * Update (May 2011), recent TargetPacks now include boot-time
	 * checking to configure the SPI only if we are in boot-from-SPI
	 * mode. Hence the additional modification in the previous
	 * paragraph may now be ignored if now using newer TargetPacks.
	 *
	 * Update (April 2012), when using a revision V2.2 board, then
	 * it is essential that you also post-pend the "romgen" command
	 * with the ",boardrev=3" option. e.g.:
	 *
	 * host% romgen --show-comment dummy:hdk7108stx7108:host,boardrev=3
	 *
	 * Update (April 2012), when using recent versions of the TargetPacks,
	 * it is now possible to get romgen *not* to configure the 32-bit mode
	 * + PMB register settings, by post-pending the "romgen" command with the
	 * ",se=0" option. This then removes the need to hand-edit the "*.romgen"
	 * file to add the "#if 0" and "#endif" lines mentioned above. e.g.:
	 *
	 * host% romgen --show-comment dummy:hdk7108stx7108:host,boardrev=3,se=0
	 */
#include "hdk7108-stx7108.romgen"

	END_MARKER

__memory_setup_table_end:

	.end
