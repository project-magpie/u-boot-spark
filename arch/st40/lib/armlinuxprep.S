/* Synopsis: ARM boot stub code for STLinux kernel preparation when booted as
 *           an ST40 slave.
 *
 * Copyright (c) 2011-2013 STMicroelectronics Limited.
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * #include <armv7reg.h>
 * #include <asmdefs.h>
 * #include <cache_ops.h>
 *
 *   .section    .text
 *   .global     __armBootStub
 *   .type       __armBootStub, %function
 *   .syntax     unified
 *   .arch       armv7
 *   .cpu        cortex-a9
 *   .arm
 *   .balign     64
 *
 * __armBootStub:
 *    CACHE_INVALIDATE_ICACHE_ALL
 *
 *    // Code to invalidate data cache (copied from cache.S)
 *    READ_CP15     r0, CP15_CLIDR                          // Cache Level ID Register
 *    ANDS          r3, r0, #CLIDR_LOC_MASK                 // Extract Level of Coherency
 *    MOV           r3, r3, LSR #(CLIDR_LOC_SHIFT - 1)      // Cache level value (naturally aligned; 1 in field means level 2)
 *    BEQ           5f
 *    LDR           r10, =0                                 // R10 = Current cache level minus 1 from bit 1 upwards (ready for use in CSSELR)
 * 1: ADD           r2, r10, r10, LSR #1                    // R2 = 3 * cache level
 *    MOV           r1, r0, LSR r2                          // Shift cache type to LSBs
 *    AND           r1, r1, #(CLIDR_CTYPE_MASK(1))
 *    CMP           r1, #CLIDR_CTYPE_DCACHE                 // Has a D-cache?
 *    BLT           4f                                      // Skip this level if no D-cache
 *
 *    WRITE_CP15    r10, CP15_CSSELR                        // Cache Size Selection Register
 *    ISB                                                   // ISB to sync the change to the CSSELR
 *    READ_CP15     r1, CP15_CSIDR                          // Cache Size ID Register
 *    AND           r2, r1, #CSIDR_LINE_SIZE_MASK
 *    ADD           r2, r2, #4                              // Add 4 for the line length offset (log2 16 bytes)
 *    LDR           r4, =0x3FF
 *    ANDS          r4, r4, r1, LSR #CSIDR_NUM_WAYS_SHIFT   // R4 is the max number of the way size (right aligned)
 *    CLZ           r8, r4                                  // R8 is the bit position of the way size increment
 *    LDR           r7, =0x00007FFF
 *    ANDS          r7, r7, r1, LSR #CSIDR_NUM_SETS_SHIFT   // R7 is the max number of the index size (right aligned)
 * 2: MOV           r9, r4                                  // R9 working copy of the max way size (right aligned)
 * 3: ORR           r11, r10, r9, LSL r8                    // Factor in the way number and cache number into R11
 *    ORR           r11, r11, r7, LSL r2                    // Factor in the index number
 *    CACHE_INVALIDATE_DCACHE_SET_WAY(r11)                  // Invalidate by set/way
 *    SUBS          r9, r9, #1                              // Decrement the way number
 *    BGE           3b
 *    SUBS          r7, r7, #1                              // Decrement the index
 *    BGE           2b
 * 4: ADD           r10, r10, #2                            // Increment the cache level number (CSSELR usage)
 *    CMP           r3, r10
 *    BGT           1b
 * 5: DSB                                                   // Ensure everything is complete
 *    ISB
 *
 *    // Detect CPU ID within cluster and put in holding pen if not 0 (copied from pbl_start.S)
 *    // At this stage we can send slave ARM cores to a holding pen where they
 *    // can wait until we're ready to do something with them.
 *    READ_CP15     r1, CP15_MPIDR
 *    TST           r1, #(1 << 31)                          // Register has MPCore format?  0 means no
 *    BEQ           3f
 *    TST           r1, #(1 << 30)                          // 1 means this is a uni-processor ARM
 *    BNE           3f
 *    ANDS          r1, #0xFF                               // Is CPU 0 (in affinity level 0)?
 *    BEQ           3f
 *
 *    // Capture slave(s) in a holding pen.
 *    // Our slave(s) will use wait-for-event to wait, and when woken will
 *    // check to see if a secondary entrypoint pointer and location have been
 *    // updated.  If the pointer is updated, it updates it's reference copies
 *    // and checks again, if the location changes alone, that's our release
 *    // address.
 *
 *    ADR           r0, 4f                                  // Will be &armSecondaryStartupAddr in the host-CPU PBL
 *    LDR           r0, [r0]                                // armSecondaryStartupAddr is a pointer to where to look...
 * 1: LDR           r1, [r0]                                // r1 = reference location to read for start address
 *    LDR           r2, [r1]                                // r2 = reference 'start address'
 *
 *    // We have our reference addresses...
 *    // Poll for changes...
 *    // Data cache is still off on this core, so no need to invalidate cache
 *    // lines.
 *
 * 2: LDR           r3, [r0]                                // r3 = current location to read for start address
 *    CMP           r1, r3
 *    BNE           1b                                      // Location for start address changed - update references
 *    LDR           r4, [r1]                                // r4 = current 'start address'
 *    CMP           r2, r4                                  // Is it different from our first read of this location?
 *    IT            NE
 *    BLXNE         r4                                      // If different, release the slave
 *    WFE
 *    B             2b
 *
 * 3: // Boot master CPU continues here
 *
 *    // Set r0 to 0, r1 to the machine type, and r2 to the ATAGs address
 *    LDR           r0, =0
 *    LDR           r1, 1f
 *    LDR           r2, 2f
 *
 *    // Jump to kernel entry point
 *    LDR           r3, 3f
 *    BX            r3
 *
 *    // Should never get here, but just in case
 * 6: B         6b
 *
 *    // Inject real addresses here to replace these placeholders
 *    .balign 4
 * 1: .long         0x8ac417e7                              // Machine type
 * 2: .long         0xa7a95add                              // ATAGs address pointer
 * 3: .long         0x57a77add                              // Kernel start address pointer
 * 4: .long         0x5ece7711                              // Secondary core kernel start address pointer - armSecondaryStartupAddr in main PBL
 *
 *    .end
 *
 * ARM Linux preparation code.
 * ---------------------------
 *      Conversion process: (Replace <.> by nothing)
 *          armv7-linux-gcc -fpic -c _armlinuxprep.S -mcpu=cortex-a9 -I $BOOTROOT/include/armv7
 *          armv7-linux-objcopy -O binary -j .text _armlinuxprep.o d.bin
 *          xxd -i d.bin | grep -v unsigned |grep -v "}" | sed -e "s/,$//" -e "s/^ *<.>/\t.byte\t/" > d.inc
 *          cat d.inc | sed -e "s/0xe7, 0x17, 0xc4, 0x8a/\n\t.long\t\ARM_MACHTYPE_NUM\n\t.byte\t/" > d2.inc
 *          cat d2.inc | sed -e "s/0xdd, 0x5a, 0xa9, 0xa7/\natagsAddrPtr:\n\t.byte\t0xdd, 0x5a, 0xa9, 0xa7\n\t.byte\t/" > d3.inc
 *          cat d3.inc | sed -e "s/0xdd, 0x7a, 0xa7, 0x57/\narmKernelEntryAddrPtr:\n\t.byte\t0xdd, 0x7a, 0xa7, 0x57\n\t.byte\t/" > d4.inc
 *          cat d4.inc | sed -e "s/0x11, 0x77, 0xce, 0x5e/\narmKernelSecondaryEntryAddrPtr:\n\t.byte\t0x11, 0x77, 0xce, 0x5e\n\t.byte\t/" > d5.inc
 *          cat d5.inc | sed -e "s/, *$//" -e "s/[ \t]*.byte[ \t]*$//" -e "s/\.byte[ \t]*,[ \t]*<.>/.byte\t/" | grep .
 */

       #define ARM_MACHTYPE_NUM 3612
       .section .data
       .global  armLinuxPrep
       .global  atagsAddrPtr
       .global  armKernelEntryAddrPtr
       .global  armKernelSecondaryEntryAddrPtr
       .balign 64

armLinuxPrep:
       .byte   0x15, 0x0f, 0x07, 0xee, 0x30, 0x0f, 0x30, 0xee, 0x07, 0x34, 0x10, 0xe2
       .byte   0xa3, 0x3b, 0xa0, 0xe1, 0x1a, 0x00, 0x00, 0x0a, 0x00, 0xa0, 0xa0, 0xe3
       .byte   0xaa, 0x20, 0x8a, 0xe0, 0x30, 0x12, 0xa0, 0xe1, 0x07, 0x10, 0x01, 0xe2
       .byte   0x02, 0x00, 0x51, 0xe3, 0x11, 0x00, 0x00, 0xba, 0x10, 0xaf, 0x40, 0xee
       .byte   0x6f, 0xf0, 0x7f, 0xf5, 0x10, 0x1f, 0x30, 0xee, 0x07, 0x20, 0x01, 0xe2
       .byte   0x04, 0x20, 0x82, 0xe2, 0xb4, 0x40, 0x9f, 0xe5, 0xa1, 0x41, 0x14, 0xe0
       .byte   0x14, 0x8f, 0x6f, 0xe1, 0xac, 0x70, 0x9f, 0xe5, 0xa1, 0x76, 0x17, 0xe0
       .byte   0x04, 0x90, 0xa0, 0xe1, 0x19, 0xb8, 0x8a, 0xe1, 0x17, 0xb2, 0x8b, 0xe1
       .byte   0x56, 0xbf, 0x07, 0xee, 0x01, 0x90, 0x59, 0xe2, 0xfa, 0xff, 0xff, 0xaa
       .byte   0x01, 0x70, 0x57, 0xe2, 0xf7, 0xff, 0xff, 0xaa, 0x02, 0xa0, 0x8a, 0xe2
       .byte   0x0a, 0x00, 0x53, 0xe1, 0xe5, 0xff, 0xff, 0xca, 0x4f, 0xf0, 0x7f, 0xf5
       .byte   0x6f, 0xf0, 0x7f, 0xf5, 0xb0, 0x1f, 0x10, 0xee, 0x02, 0x01, 0x11, 0xe3
       .byte   0x0f, 0x00, 0x00, 0x0a, 0x01, 0x01, 0x11, 0xe3, 0x0d, 0x00, 0x00, 0x1a
       .byte   0xff, 0x10, 0x11, 0xe2, 0x0b, 0x00, 0x00, 0x0a, 0x4c, 0x00, 0x8f, 0xe2
       .byte   0x00, 0x00, 0x90, 0xe5, 0x00, 0x10, 0x90, 0xe5, 0x00, 0x20, 0x91, 0xe5
       .byte   0x00, 0x30, 0x90, 0xe5, 0x03, 0x00, 0x51, 0xe1, 0xfa, 0xff, 0xff, 0x1a
       .byte   0x00, 0x40, 0x91, 0xe5, 0x04, 0x00, 0x52, 0xe1, 0x34, 0xff, 0x2f, 0x11
       .byte   0x02, 0xf0, 0x20, 0xe3, 0xf7, 0xff, 0xff, 0xea, 0x00, 0x00, 0xa0, 0xe3
       .byte   0x0c, 0x10, 0x9f, 0xe5, 0x0c, 0x20, 0x9f, 0xe5, 0x0c, 0x30, 0x9f, 0xe5
       .byte   0x13, 0xff, 0x2f, 0xe1, 0xfe, 0xff, 0xff, 0xea
       .long   ARM_MACHTYPE_NUM

atagsAddrPtr:
       .byte   0xdd, 0x5a, 0xa9, 0xa7

armKernelEntryAddrPtr:
       .byte   0xdd, 0x7a, 0xa7, 0x57

armKernelSecondaryEntryAddrPtr:
       .byte   0x11, 0x77, 0xce, 0x5e
       .byte   0xff, 0x03, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xf0, 0x20, 0xe3
       .byte   0x00, 0xf0, 0x20, 0xe3, 0x00, 0xf0, 0x20, 0xe3, 0x00, 0xf0, 0x20, 0xe3
       .byte   0x00, 0xf0, 0x20, 0xe3, 0x00, 0xf0, 0x20, 0xe3, 0x00, 0xf0, 0x20, 0xe3
       .byte   0x00, 0xf0, 0x20, 0xe3, 0x00, 0xf0, 0x20, 0xe3, 0x00, 0xf0, 0x20, 0xe3
       .byte   0x00, 0xf0, 0x20, 0xe3, 0x00, 0xf0, 0x20, 0xe3, 0x00, 0xf0, 0x20, 0xe3
       .byte   0x00, 0xf0, 0x20, 0xe3, 0x00, 0xf0, 0x20, 0xe3
       .end
