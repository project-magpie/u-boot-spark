/* ************************************************ */
/* * Copyright STMicroelectronics Limited 2001 */
/* * All rights reserved. */
/* * */
/* * STMicroelectronics Ltd. Confidential */
/* ************************************************ */
/*  FILE AUTOMATICALLY GENERATED -- DO NOT EDIT */

#ifndef _ctrlreg_H_
#define _ctrlreg_H_

#define LX_CREG_BASE LX_CREG_BASE_OFFSET	/*  Memory Mapped Core Control Registers. Internal to the ST200 Core. */
#define LX_CREG_BASE_OFFSET 0xffff0000	/*  Memory Mapped Core Control Registers. Internal to the ST200 Core. */
#define LX_CREG_SIZE 0x00010000	/*  Memory Mapped Core Control Registers. Internal to the ST200 Core. */
#define LOG2_LX_CREG_SIZE 0x10	/*  Memory Mapped Core Control Registers. Internal to the ST200 Core. */
#define LX_C3000_BASE (LX_C3000_BASE_OFFSET + PERIPHERAL_BASE)	/*  Base address of peripherals for ST200 core */
#define LX_C3000_BASE_OFFSET 0x00000	/*  Base address of peripherals for ST200 core */
#define LX_C3000_SIZE 0x05000	/*  Base address of peripherals for ST200 core */
#define LOG2_LX_C3000_SIZE 0xe	/*  Base address of peripherals for ST200 core */
#define LX_INTCR_BASE (LX_INTCR_BASE_OFFSET + PERIPHERAL_BASE)	/*  Interrupt controller & Timer Registers. Part of ST200. */
#define LX_INTCR_BASE_OFFSET 0x00000	/*  Interrupt controller & Timer Registers. Part of ST200. */
#define LX_INTCR_SIZE 0x01000	/*  Interrupt controller & Timer Registers. Part of ST200. */
#define LOG2_LX_INTCR_SIZE 0xc	/*  Interrupt controller & Timer Registers. Part of ST200. */
#define LX_DSU_BASE (LX_DSU_BASE_OFFSET + PERIPHERAL_BASE)	/*  DSU Registers. Part of ST200. */
#define LX_DSU_BASE_OFFSET 0x03000	/*  DSU Registers. Part of ST200. */
#define LX_DSU_SIZE 0x01000	/*  DSU Registers. Part of ST200. */
#define LOG2_LX_DSU_SIZE 0xc	/*  DSU Registers. Part of ST200. */
#define LX_DSU_ROM_BASE (LX_DSU_ROM_BASE_OFFSET + PERIPHERAL_BASE)	/*  DSRom Code. Part of ST200. */
#define LX_DSU_ROM_BASE_OFFSET 0x04000	/*  DSRom Code. Part of ST200. */
#define LX_DSU_ROM_SIZE 0x00400	/*  DSRom Code. Part of ST200. */
#define LOG2_LX_DSU_ROM_SIZE 0xa	/*  DSRom Code. Part of ST200. */
#define ARCHCONST_BASE ARCHCONST_BASE_OFFSET	/*  Architecture constants */
#define ARCHCONST_BASE_OFFSET 0x0	/*  Architecture constants */
#define CORECONST_BASE CORECONST_BASE_OFFSET	/*  Core constants */
#define CORECONST_BASE_OFFSET 0x0	/*  Core constants */
#define C3000CONST_BASE C3000CONST_BASE_OFFSET	/*  C3000 constants */
#define C3000CONST_BASE_OFFSET 0x0	/*  C3000 constants */
#define LXPSW (LX_CREG_BASE + LXPSW_OFFSET)	/*  The Program Status Word. */
#define LXPSW_OFFSET 0xfff8	/*  The Program Status Word. */
#define LXPSW_REL (LX_CREG_BASE_OFFSET + LXPSW_OFFSET)	/*  The Program Status Word. */
#define LXSAVED_PSW (LX_CREG_BASE + LXSAVED_PSW_OFFSET)	/*  Saved PSW, written by hardware on exception. */
#define LXSAVED_PSW_OFFSET 0xfff0	/*  Saved PSW, written by hardware on exception. */
#define LXSAVED_PSW_REL (LX_CREG_BASE_OFFSET + LXSAVED_PSW_OFFSET)	/*  Saved PSW, written by hardware on exception. */
#define LXSAVED_PC (LX_CREG_BASE + LXSAVED_PC_OFFSET)	/*  Saved Program Counter, written by hardware on exception. */
#define LXSAVED_PC_OFFSET 0xffe8	/*  Saved Program Counter, written by hardware on exception. */
#define LXSAVED_PC_REL (LX_CREG_BASE_OFFSET + LXSAVED_PC_OFFSET)	/*  Saved Program Counter, written by hardware on exception. */
#define LXHANDLER_PC (LX_CREG_BASE + LXHANDLER_PC_OFFSET)	/*  The address of the exception handler code. */
#define LXHANDLER_PC_OFFSET 0xffe0	/*  The address of the exception handler code. */
#define LXHANDLER_PC_REL (LX_CREG_BASE_OFFSET + LXHANDLER_PC_OFFSET)	/*  The address of the exception handler code. */
#define LXEXCAUSE (LX_CREG_BASE + LXEXCAUSE_OFFSET)	/*  A one hot vector of trap (exception/interrupt) types, indicating the cause of the last trap. Written by the hardware on a trap. */
#define LXEXCAUSE_OFFSET 0xffd8	/*  A one hot vector of trap (exception/interrupt) types, indicating the cause of the last trap. Written by the hardware on a trap. */
#define LXEXCAUSE_REL (LX_CREG_BASE_OFFSET + LXEXCAUSE_OFFSET)	/*  A one hot vector of trap (exception/interrupt) types, indicating the cause of the last trap. Written by the hardware on a trap. */
#define LXEXADDRESS (LX_CREG_BASE + LXEXADDRESS_OFFSET)	/*  This will be the data effective address in the case of either a DPU, CREG, DBREAK, or MISALIGNED_TRAP exception. For other exception types this register will be zero. */
#define LXEXADDRESS_OFFSET 0xffd0	/*  This will be the data effective address in the case of either a DPU, CREG, DBREAK, or MISALIGNED_TRAP exception. For other exception types this register will be zero. */
#define LXEXADDRESS_REL (LX_CREG_BASE_OFFSET + LXEXADDRESS_OFFSET)	/*  This will be the data effective address in the case of either a DPU, CREG, DBREAK, or MISALIGNED_TRAP exception. For other exception types this register will be zero. */
#define LXSAVED_SAVED_PSW (LX_CREG_BASE + LXSAVED_SAVED_PSW_OFFSET)	/*  PSW saved by debug unit interrupt. */
#define LXSAVED_SAVED_PSW_OFFSET 0xffc0	/*  PSW saved by debug unit interrupt. */
#define LXSAVED_SAVED_PSW_REL (LX_CREG_BASE_OFFSET + LXSAVED_SAVED_PSW_OFFSET)	/*  PSW saved by debug unit interrupt. */
#define LXSAVED_SAVED_PC (LX_CREG_BASE + LXSAVED_SAVED_PC_OFFSET)	/*  PC saved by debug unit interrupt. */
#define LXSAVED_SAVED_PC_OFFSET 0xffb8	/*  PC saved by debug unit interrupt. */
#define LXSAVED_SAVED_PC_REL (LX_CREG_BASE_OFFSET + LXSAVED_SAVED_PC_OFFSET)	/*  PC saved by debug unit interrupt. */
#define LXEXCAUSENO (LX_CREG_BASE + LXEXCAUSENO_OFFSET)	/*  Exception cause as an integer, indicating the cause of the last trap. */
#define LXEXCAUSENO_OFFSET 0xff88	/*  Exception cause as an integer, indicating the cause of the last trap. */
#define LXEXCAUSENO_REL (LX_CREG_BASE_OFFSET + LXEXCAUSENO_OFFSET)	/*  Exception cause as an integer, indicating the cause of the last trap. */
#define LXSTATE1 (LX_CREG_BASE + LXSTATE1_OFFSET)	/*  Global machine state register. Controls cache locking. */
#define LXSTATE1_OFFSET 0xfe00	/*  Global machine state register. Controls cache locking. */
#define LXSTATE1_REL (LX_CREG_BASE_OFFSET + LXSTATE1_OFFSET)	/*  Global machine state register. Controls cache locking. */
#define LX_VERSION (LX_CREG_BASE + LX_VERSION_OFFSET)	/*  The version number of the core. */
#define LX_VERSION_OFFSET 0xffc8	/*  The version number of the core. */
#define LX_VERSION_REL (LX_CREG_BASE_OFFSET + LX_VERSION_OFFSET)	/*  The version number of the core. */
#define LXPERIPHERAL_BASE (LX_CREG_BASE + LXPERIPHERAL_BASE_OFFSET)	/*  Base address of peripheral registers. The top 12 bits of this register are wired to the peripheral base input pins. */
#define LXPERIPHERAL_BASE_OFFSET 0xffb0	/*  Base address of peripheral registers. The top 12 bits of this register are wired to the peripheral base input pins. */
#define LXPERIPHERAL_BASE_REL (LX_CREG_BASE_OFFSET + LXPERIPHERAL_BASE_OFFSET)	/*  Base address of peripheral registers. The top 12 bits of this register are wired to the peripheral base input pins. */
#define LXSCRATCH1 (LX_CREG_BASE + LXSCRATCH1_OFFSET)	/*  Scratch register reserved for use by supervisor software. */
#define LXSCRATCH1_OFFSET 0xffa8	/*  Scratch register reserved for use by supervisor software. */
#define LXSCRATCH1_REL (LX_CREG_BASE_OFFSET + LXSCRATCH1_OFFSET)	/*  Scratch register reserved for use by supervisor software. */
#define LXSCRATCH2 (LX_CREG_BASE + LXSCRATCH2_OFFSET)	/*  Scratch register reserved for use by supervisor software. */
#define LXSCRATCH2_OFFSET 0xffa0	/*  Scratch register reserved for use by supervisor software. */
#define LXSCRATCH2_REL (LX_CREG_BASE_OFFSET + LXSCRATCH2_OFFSET)	/*  Scratch register reserved for use by supervisor software. */
#define LXSCRATCH3 (LX_CREG_BASE + LXSCRATCH3_OFFSET)	/*  Scratch register reserved for use by supervisor software. */
#define LXSCRATCH3_OFFSET 0xff98	/*  Scratch register reserved for use by supervisor software. */
#define LXSCRATCH3_REL (LX_CREG_BASE_OFFSET + LXSCRATCH3_OFFSET)	/*  Scratch register reserved for use by supervisor software. */
#define LXSCRATCH4 (LX_CREG_BASE + LXSCRATCH4_OFFSET)	/*  Scratch register reserved for use by the debug interrupt handler. */
#define LXSCRATCH4_OFFSET 0xff90	/*  Scratch register reserved for use by the debug interrupt handler. */
#define LXSCRATCH4_REL (LX_CREG_BASE_OFFSET + LXSCRATCH4_OFFSET)	/*  Scratch register reserved for use by the debug interrupt handler. */
#define LXTLB_SIZE 0x40		/*  Number of entries in TLB */
#define LOG2_LXTLB_MIN_PAGE_SIZE 0xd	/*  Smallest page size supported by TLB (log2) */
#define LXTLB_MIN_PAGE_SIZE 0x2000	/*  Smallest page size supported by TLB (log2) */
#define LXTLB_MIN_PAGE_SIZE_MASK 0xffffe000	/*  Mask byte address to get min page size aligned */
#define LXTLB_PADDR_SIZE 0x20	/*  Physical address size supported */
#define LXTLB_PADDR_PAGENO_SHIFT 0xd	/*  Physical byte address to page number conversion */
#define LXTLB_VADDR_PAGENO_SHIFT 0xd	/*  Virtual byte address to page number conversion */
#define LXTLB_VADDR_SIZE 0x20	/*  Virtual address size supported */
#define LXTLB_INDEX (LX_CREG_BASE + LXTLB_INDEX_OFFSET)	/*  Index of the TLB entry pointed to by TLB_ENTRY0-3. */
#define LXTLB_INDEX_OFFSET 0xff80	/*  Index of the TLB entry pointed to by TLB_ENTRY0-3. */
#define LXTLB_INDEX_REL (LX_CREG_BASE_OFFSET + LXTLB_INDEX_OFFSET)	/*  Index of the TLB entry pointed to by TLB_ENTRY0-3. */
#define LXTLB_ENTRY0 (LX_CREG_BASE + LXTLB_ENTRY0_OFFSET)	/*  Bits [31:00] of the current TLB entry. */
#define LXTLB_ENTRY0_OFFSET 0xff78	/*  Bits [31:00] of the current TLB entry. */
#define LXTLB_ENTRY0_REL (LX_CREG_BASE_OFFSET + LXTLB_ENTRY0_OFFSET)	/*  Bits [31:00] of the current TLB entry. */
#define LXTLB_ENTRY1 (LX_CREG_BASE + LXTLB_ENTRY1_OFFSET)	/*  Bits [63:32] of the current TLB entry. */
#define LXTLB_ENTRY1_OFFSET 0xff70	/*  Bits [63:32] of the current TLB entry. */
#define LXTLB_ENTRY1_REL (LX_CREG_BASE_OFFSET + LXTLB_ENTRY1_OFFSET)	/*  Bits [63:32] of the current TLB entry. */
#define LXTLB_ENTRY2 (LX_CREG_BASE + LXTLB_ENTRY2_OFFSET)	/*  Bits [95:64] of the current TLB entry. */
#define LXTLB_ENTRY2_OFFSET 0xff68	/*  Bits [95:64] of the current TLB entry. */
#define LXTLB_ENTRY2_REL (LX_CREG_BASE_OFFSET + LXTLB_ENTRY2_OFFSET)	/*  Bits [95:64] of the current TLB entry. */
#define LXTLB_ENTRY3 (LX_CREG_BASE + LXTLB_ENTRY3_OFFSET)	/*  Bits [127:96] of the current TLB entry. */
#define LXTLB_ENTRY3_OFFSET 0xff60	/*  Bits [127:96] of the current TLB entry. */
#define LXTLB_ENTRY3_REL (LX_CREG_BASE_OFFSET + LXTLB_ENTRY3_OFFSET)	/*  Bits [127:96] of the current TLB entry. */
#define LXTLB_EXCAUSE (LX_CREG_BASE + LXTLB_EXCAUSE_OFFSET)	/*  Case of the TLB related exception. */
#define LXTLB_EXCAUSE_OFFSET 0xff58	/*  Case of the TLB related exception. */
#define LXTLB_EXCAUSE_REL (LX_CREG_BASE_OFFSET + LXTLB_EXCAUSE_OFFSET)	/*  Case of the TLB related exception. */
#define LXTLB_CONTROL (LX_CREG_BASE + LXTLB_CONTROL_OFFSET)	/*  Control bits for TLB. */
#define LXTLB_CONTROL_OFFSET 0xff50	/*  Control bits for TLB. */
#define LXTLB_CONTROL_REL (LX_CREG_BASE_OFFSET + LXTLB_CONTROL_OFFSET)	/*  Control bits for TLB. */
#define LXTLB_REPLACE (LX_CREG_BASE + LXTLB_REPLACE_OFFSET)	/*  Replacement pointer. */
#define LXTLB_REPLACE_OFFSET 0xff48	/*  Replacement pointer. */
#define LXTLB_REPLACE_REL (LX_CREG_BASE_OFFSET + LXTLB_REPLACE_OFFSET)	/*  Replacement pointer. */
#define LXTLB_ASID (LX_CREG_BASE + LXTLB_ASID_OFFSET)	/*  Current address space ID. */
#define LXTLB_ASID_OFFSET 0xff40	/*  Current address space ID. */
#define LXTLB_ASID_REL (LX_CREG_BASE_OFFSET + LXTLB_ASID_OFFSET)	/*  Current address space ID. */
#define LXSCU_BASE0 (LX_CREG_BASE + LXSCU_BASE0_OFFSET)	/*  Base address of speculative load region. */
#define LXSCU_BASE0_OFFSET 0xd000	/*  Base address of speculative load region. */
#define LXSCU_BASE0_REL (LX_CREG_BASE_OFFSET + LXSCU_BASE0_OFFSET)	/*  Base address of speculative load region. */
#define LXSCU_LIMIT0 (LX_CREG_BASE + LXSCU_LIMIT0_OFFSET)	/*  Limit addres of speculative load region. */
#define LXSCU_LIMIT0_OFFSET 0xd008	/*  Limit addres of speculative load region. */
#define LXSCU_LIMIT0_REL (LX_CREG_BASE_OFFSET + LXSCU_LIMIT0_OFFSET)	/*  Limit addres of speculative load region. */
#define LXSCU_BASE1 (LX_CREG_BASE + LXSCU_BASE1_OFFSET)	/*  Base address of speculative load region. */
#define LXSCU_BASE1_OFFSET 0xd010	/*  Base address of speculative load region. */
#define LXSCU_BASE1_REL (LX_CREG_BASE_OFFSET + LXSCU_BASE1_OFFSET)	/*  Base address of speculative load region. */
#define LXSCU_LIMIT1 (LX_CREG_BASE + LXSCU_LIMIT1_OFFSET)	/*  Limit addres of speculative load region. */
#define LXSCU_LIMIT1_OFFSET 0xd018	/*  Limit addres of speculative load region. */
#define LXSCU_LIMIT1_REL (LX_CREG_BASE_OFFSET + LXSCU_LIMIT1_OFFSET)	/*  Limit addres of speculative load region. */
#define LXSCU_BASE2 (LX_CREG_BASE + LXSCU_BASE2_OFFSET)	/*  Base address of speculative load region. */
#define LXSCU_BASE2_OFFSET 0xd020	/*  Base address of speculative load region. */
#define LXSCU_BASE2_REL (LX_CREG_BASE_OFFSET + LXSCU_BASE2_OFFSET)	/*  Base address of speculative load region. */
#define LXSCU_LIMIT2 (LX_CREG_BASE + LXSCU_LIMIT2_OFFSET)	/*  Limit addres of speculative load region. */
#define LXSCU_LIMIT2_OFFSET 0xd028	/*  Limit addres of speculative load region. */
#define LXSCU_LIMIT2_REL (LX_CREG_BASE_OFFSET + LXSCU_LIMIT2_OFFSET)	/*  Limit addres of speculative load region. */
#define LXSCU_BASE3 (LX_CREG_BASE + LXSCU_BASE3_OFFSET)	/*  Base address of speculative load region. */
#define LXSCU_BASE3_OFFSET 0xd030	/*  Base address of speculative load region. */
#define LXSCU_BASE3_REL (LX_CREG_BASE_OFFSET + LXSCU_BASE3_OFFSET)	/*  Base address of speculative load region. */
#define LXSCU_LIMIT3 (LX_CREG_BASE + LXSCU_LIMIT3_OFFSET)	/*  Limit addres of speculative load region. */
#define LXSCU_LIMIT3_OFFSET 0xd038	/*  Limit addres of speculative load region. */
#define LXSCU_LIMIT3_REL (LX_CREG_BASE_OFFSET + LXSCU_LIMIT3_OFFSET)	/*  Limit addres of speculative load region. */
#define LXDBREAK_LOWER (LX_CREG_BASE + LXDBREAK_LOWER_OFFSET)	/*  Data breakpoint lower address. */
#define LXDBREAK_LOWER_OFFSET 0xfe80	/*  Data breakpoint lower address. */
#define LXDBREAK_LOWER_REL (LX_CREG_BASE_OFFSET + LXDBREAK_LOWER_OFFSET)	/*  Data breakpoint lower address. */
#define LXDBREAK_UPPER (LX_CREG_BASE + LXDBREAK_UPPER_OFFSET)	/*  Data breakpoint upper address. */
#define LXDBREAK_UPPER_OFFSET 0xfe78	/*  Data breakpoint upper address. */
#define LXDBREAK_UPPER_REL (LX_CREG_BASE_OFFSET + LXDBREAK_UPPER_OFFSET)	/*  Data breakpoint upper address. */
#define LXDBREAK_CONTROL (LX_CREG_BASE + LXDBREAK_CONTROL_OFFSET)	/*  Data breakpoint control. */
#define LXDBREAK_CONTROL_OFFSET 0xfe70	/*  Data breakpoint control. */
#define LXDBREAK_CONTROL_REL (LX_CREG_BASE_OFFSET + LXDBREAK_CONTROL_OFFSET)	/*  Data breakpoint control. */
#define LXIBREAK_LOWER (LX_CREG_BASE + LXIBREAK_LOWER_OFFSET)	/*  Instruction breakpoint lower address. */
#define LXIBREAK_LOWER_OFFSET 0xfdd0	/*  Instruction breakpoint lower address. */
#define LXIBREAK_LOWER_REL (LX_CREG_BASE_OFFSET + LXIBREAK_LOWER_OFFSET)	/*  Instruction breakpoint lower address. */
#define LXIBREAK_UPPER (LX_CREG_BASE + LXIBREAK_UPPER_OFFSET)	/*  Instruction breakpoint upper address. */
#define LXIBREAK_UPPER_OFFSET 0xfdc8	/*  Instruction breakpoint upper address. */
#define LXIBREAK_UPPER_REL (LX_CREG_BASE_OFFSET + LXIBREAK_UPPER_OFFSET)	/*  Instruction breakpoint upper address. */
#define LXIBREAK_CONTROL (LX_CREG_BASE + LXIBREAK_CONTROL_OFFSET)	/*  Instruction breakpoint control. */
#define LXIBREAK_CONTROL_OFFSET 0xfdc0	/*  Instruction breakpoint control. */
#define LXIBREAK_CONTROL_REL (LX_CREG_BASE_OFFSET + LXIBREAK_CONTROL_OFFSET)	/*  Instruction breakpoint control. */
#define LXPM_CR (LX_CREG_BASE + LXPM_CR_OFFSET)	/*  Performance monitoring control. */
#define LXPM_CR_OFFSET 0xf800	/*  Performance monitoring control. */
#define LXPM_CR_REL (LX_CREG_BASE_OFFSET + LXPM_CR_OFFSET)	/*  Performance monitoring control. */
#define LXPM_CNT0 (LX_CREG_BASE + LXPM_CNT0_OFFSET)	/*  Performance monitor counter 0 value. */
#define LXPM_CNT0_OFFSET 0xf808	/*  Performance monitor counter 0 value. */
#define LXPM_CNT0_REL (LX_CREG_BASE_OFFSET + LXPM_CNT0_OFFSET)	/*  Performance monitor counter 0 value. */
#define LXPM_CNT1 (LX_CREG_BASE + LXPM_CNT1_OFFSET)	/*  Performance monitor counter 1 value. */
#define LXPM_CNT1_OFFSET 0xf810	/*  Performance monitor counter 1 value. */
#define LXPM_CNT1_REL (LX_CREG_BASE_OFFSET + LXPM_CNT1_OFFSET)	/*  Performance monitor counter 1 value. */
#define LXPM_CNT2 (LX_CREG_BASE + LXPM_CNT2_OFFSET)	/*  Performance monitor counter 2 value. */
#define LXPM_CNT2_OFFSET 0xf818	/*  Performance monitor counter 2 value. */
#define LXPM_CNT2_REL (LX_CREG_BASE_OFFSET + LXPM_CNT2_OFFSET)	/*  Performance monitor counter 2 value. */
#define LXPM_CNT3 (LX_CREG_BASE + LXPM_CNT3_OFFSET)	/*  Performance monitor counter 3 value. */
#define LXPM_CNT3_OFFSET 0xf820	/*  Performance monitor counter 3 value. */
#define LXPM_CNT3_REL (LX_CREG_BASE_OFFSET + LXPM_CNT3_OFFSET)	/*  Performance monitor counter 3 value. */
#define LXPM_PCLK (LX_CREG_BASE + LXPM_PCLK_OFFSET)	/*  Performance monitor core cycle counter. */
#define LXPM_PCLK_OFFSET 0xf828	/*  Performance monitor core cycle counter. */
#define LXPM_PCLK_REL (LX_CREG_BASE_OFFSET + LXPM_PCLK_OFFSET)	/*  Performance monitor core cycle counter. */
#define LXSDI0_DATA (LX_CREG_BASE + LXSDI0_DATA_OFFSET)	/*  SDI 0 data. */
#define LXSDI0_DATA_OFFSET 0xe000	/*  SDI 0 data. */
#define LXSDI0_DATA_REL (LX_CREG_BASE_OFFSET + LXSDI0_DATA_OFFSET)	/*  SDI 0 data. */
#define LXSDI0_READY (LX_CREG_BASE + LXSDI0_READY_OFFSET)	/*  SDI 0 ready. */
#define LXSDI0_READY_OFFSET 0xe008	/*  SDI 0 ready. */
#define LXSDI0_READY_REL (LX_CREG_BASE_OFFSET + LXSDI0_READY_OFFSET)	/*  SDI 0 ready. */
#define LXSDI0_CONTROL (LX_CREG_BASE + LXSDI0_CONTROL_OFFSET)	/*  SDI 0 control. */
#define LXSDI0_CONTROL_OFFSET 0xe010	/*  SDI 0 control. */
#define LXSDI0_CONTROL_REL (LX_CREG_BASE_OFFSET + LXSDI0_CONTROL_OFFSET)	/*  SDI 0 control. */
#define LXSDI0_COUNT (LX_CREG_BASE + LXSDI0_COUNT_OFFSET)	/*  SDI 0 count. */
#define LXSDI0_COUNT_OFFSET 0xe018	/*  SDI 0 count. */
#define LXSDI0_COUNT_REL (LX_CREG_BASE_OFFSET + LXSDI0_COUNT_OFFSET)	/*  SDI 0 count. */
#define LXSDI0_TIMEOUT (LX_CREG_BASE + LXSDI0_TIMEOUT_OFFSET)	/*  SDI 0 timeout. */
#define LXSDI0_TIMEOUT_OFFSET 0xe020	/*  SDI 0 timeout. */
#define LXSDI0_TIMEOUT_REL (LX_CREG_BASE_OFFSET + LXSDI0_TIMEOUT_OFFSET)	/*  SDI 0 timeout. */
#define LXSDI1_DATA (LX_CREG_BASE + LXSDI1_DATA_OFFSET)	/*  SDI 1 data. */
#define LXSDI1_DATA_OFFSET 0xe400	/*  SDI 1 data. */
#define LXSDI1_DATA_REL (LX_CREG_BASE_OFFSET + LXSDI1_DATA_OFFSET)	/*  SDI 1 data. */
#define LXSDI1_READY (LX_CREG_BASE + LXSDI1_READY_OFFSET)	/*  SDI 1 ready. */
#define LXSDI1_READY_OFFSET 0xe408	/*  SDI 1 ready. */
#define LXSDI1_READY_REL (LX_CREG_BASE_OFFSET + LXSDI1_READY_OFFSET)	/*  SDI 1 ready. */
#define LXSDI1_CONTROL (LX_CREG_BASE + LXSDI1_CONTROL_OFFSET)	/*  SDI 1 control. */
#define LXSDI1_CONTROL_OFFSET 0xe410	/*  SDI 1 control. */
#define LXSDI1_CONTROL_REL (LX_CREG_BASE_OFFSET + LXSDI1_CONTROL_OFFSET)	/*  SDI 1 control. */
#define LXSDI1_COUNT (LX_CREG_BASE + LXSDI1_COUNT_OFFSET)	/*  SDI 1 count. */
#define LXSDI1_COUNT_OFFSET 0xe418	/*  SDI 1 count. */
#define LXSDI1_COUNT_REL (LX_CREG_BASE_OFFSET + LXSDI1_COUNT_OFFSET)	/*  SDI 1 count. */
#define LXSDI1_TIMEOUT (LX_CREG_BASE + LXSDI1_TIMEOUT_OFFSET)	/*  SDI 1 timeout. */
#define LXSDI1_TIMEOUT_OFFSET 0xe420	/*  SDI 1 timeout. */
#define LXSDI1_TIMEOUT_REL (LX_CREG_BASE_OFFSET + LXSDI1_TIMEOUT_OFFSET)	/*  SDI 1 timeout. */
#define LXSDI2_DATA (LX_CREG_BASE + LXSDI2_DATA_OFFSET)	/*  SDI 2 data. */
#define LXSDI2_DATA_OFFSET 0xe800	/*  SDI 2 data. */
#define LXSDI2_DATA_REL (LX_CREG_BASE_OFFSET + LXSDI2_DATA_OFFSET)	/*  SDI 2 data. */
#define LXSDI2_READY (LX_CREG_BASE + LXSDI2_READY_OFFSET)	/*  SDI 2 ready. */
#define LXSDI2_READY_OFFSET 0xe808	/*  SDI 2 ready. */
#define LXSDI2_READY_REL (LX_CREG_BASE_OFFSET + LXSDI2_READY_OFFSET)	/*  SDI 2 ready. */
#define LXSDI2_CONTROL (LX_CREG_BASE + LXSDI2_CONTROL_OFFSET)	/*  SDI 2 control. */
#define LXSDI2_CONTROL_OFFSET 0xe810	/*  SDI 2 control. */
#define LXSDI2_CONTROL_REL (LX_CREG_BASE_OFFSET + LXSDI2_CONTROL_OFFSET)	/*  SDI 2 control. */
#define LXSDI2_COUNT (LX_CREG_BASE + LXSDI2_COUNT_OFFSET)	/*  SDI 2 count. */
#define LXSDI2_COUNT_OFFSET 0xe818	/*  SDI 2 count. */
#define LXSDI2_COUNT_REL (LX_CREG_BASE_OFFSET + LXSDI2_COUNT_OFFSET)	/*  SDI 2 count. */
#define LXSDI2_TIMEOUT (LX_CREG_BASE + LXSDI2_TIMEOUT_OFFSET)	/*  SDI 2 timeout. */
#define LXSDI2_TIMEOUT_OFFSET 0xe820	/*  SDI 2 timeout. */
#define LXSDI2_TIMEOUT_REL (LX_CREG_BASE_OFFSET + LXSDI2_TIMEOUT_OFFSET)	/*  SDI 2 timeout. */
#define LXSDI3_DATA (LX_CREG_BASE + LXSDI3_DATA_OFFSET)	/*  SDI 3 data. */
#define LXSDI3_DATA_OFFSET 0xec00	/*  SDI 3 data. */
#define LXSDI3_DATA_REL (LX_CREG_BASE_OFFSET + LXSDI3_DATA_OFFSET)	/*  SDI 3 data. */
#define LXSDI3_READY (LX_CREG_BASE + LXSDI3_READY_OFFSET)	/*  SDI 3 ready. */
#define LXSDI3_READY_OFFSET 0xec08	/*  SDI 3 ready. */
#define LXSDI3_READY_REL (LX_CREG_BASE_OFFSET + LXSDI3_READY_OFFSET)	/*  SDI 3 ready. */
#define LXSDI3_CONTROL (LX_CREG_BASE + LXSDI3_CONTROL_OFFSET)	/*  SDI 3 control. */
#define LXSDI3_CONTROL_OFFSET 0xec10	/*  SDI 3 control. */
#define LXSDI3_CONTROL_REL (LX_CREG_BASE_OFFSET + LXSDI3_CONTROL_OFFSET)	/*  SDI 3 control. */
#define LXSDI3_COUNT (LX_CREG_BASE + LXSDI3_COUNT_OFFSET)	/*  SDI 3 count. */
#define LXSDI3_COUNT_OFFSET 0xec18	/*  SDI 3 count. */
#define LXSDI3_COUNT_REL (LX_CREG_BASE_OFFSET + LXSDI3_COUNT_OFFSET)	/*  SDI 3 count. */
#define LXSDI3_TIMEOUT (LX_CREG_BASE + LXSDI3_TIMEOUT_OFFSET)	/*  SDI 3 timeout. */
#define LXSDI3_TIMEOUT_OFFSET 0xec20	/*  SDI 3 timeout. */
#define LXSDI3_TIMEOUT_REL (LX_CREG_BASE_OFFSET + LXSDI3_TIMEOUT_OFFSET)	/*  SDI 3 timeout. */
#define LXINTPENDING0 (LX_INTCR_BASE + LXINTPENDING0_OFFSET)	/*  Interrupt pending bits 31:0. */
#define LXINTPENDING0_OFFSET 0x0000	/*  Interrupt pending bits 31:0. */
#define LXINTPENDING0_REL (LX_INTCR_BASE_OFFSET + LXINTPENDING0_OFFSET)	/*  Interrupt pending bits 31:0. */
#define LXINTPENDING1 (LX_INTCR_BASE + LXINTPENDING1_OFFSET)	/*  Interrupt pending bits 63:32. */
#define LXINTPENDING1_OFFSET 0x0008	/*  Interrupt pending bits 63:32. */
#define LXINTPENDING1_REL (LX_INTCR_BASE_OFFSET + LXINTPENDING1_OFFSET)	/*  Interrupt pending bits 63:32. */
#define LXINTMASK0 (LX_INTCR_BASE + LXINTMASK0_OFFSET)	/*  Interrupt mask bits 31:0. */
#define LXINTMASK0_OFFSET 0x0010	/*  Interrupt mask bits 31:0. */
#define LXINTMASK0_REL (LX_INTCR_BASE_OFFSET + LXINTMASK0_OFFSET)	/*  Interrupt mask bits 31:0. */
#define LXINTMASK1 (LX_INTCR_BASE + LXINTMASK1_OFFSET)	/*  Interrupt mask bits 63:32. */
#define LXINTMASK1_OFFSET 0x0018	/*  Interrupt mask bits 63:32. */
#define LXINTMASK1_REL (LX_INTCR_BASE_OFFSET + LXINTMASK1_OFFSET)	/*  Interrupt mask bits 63:32. */
#define LXINTTEST0 (LX_INTCR_BASE + LXINTTEST0_OFFSET)	/*  Interrupt test register bits 31:0. */
#define LXINTTEST0_OFFSET 0x0020	/*  Interrupt test register bits 31:0. */
#define LXINTTEST0_REL (LX_INTCR_BASE_OFFSET + LXINTTEST0_OFFSET)	/*  Interrupt test register bits 31:0. */
#define LXINTTEST1 (LX_INTCR_BASE + LXINTTEST1_OFFSET)	/*  Interrupt test register bits 63:32. */
#define LXINTTEST1_OFFSET 0x0028	/*  Interrupt test register bits 63:32. */
#define LXINTTEST1_REL (LX_INTCR_BASE_OFFSET + LXINTTEST1_OFFSET)	/*  Interrupt test register bits 63:32. */
#define LXINTCLR0 (LX_INTCR_BASE + LXINTCLR0_OFFSET)	/*  Interrupt clear register bits 31:0. */
#define LXINTCLR0_OFFSET 0x0030	/*  Interrupt clear register bits 31:0. */
#define LXINTCLR0_REL (LX_INTCR_BASE_OFFSET + LXINTCLR0_OFFSET)	/*  Interrupt clear register bits 31:0. */
#define LXINTCLR1 (LX_INTCR_BASE + LXINTCLR1_OFFSET)	/*  Interrupt clear register bits 63:32. */
#define LXINTCLR1_OFFSET 0x0038	/*  Interrupt clear register bits 63:32. */
#define LXINTCLR1_REL (LX_INTCR_BASE_OFFSET + LXINTCLR1_OFFSET)	/*  Interrupt clear register bits 63:32. */
#define LXINTSET0 (LX_INTCR_BASE + LXINTSET0_OFFSET)	/*  Interrupt set register bits 31:0. */
#define LXINTSET0_OFFSET 0x0040	/*  Interrupt set register bits 31:0. */
#define LXINTSET0_REL (LX_INTCR_BASE_OFFSET + LXINTSET0_OFFSET)	/*  Interrupt set register bits 31:0. */
#define LXINTSET1 (LX_INTCR_BASE + LXINTSET1_OFFSET)	/*  Interrupt clear register bits 63:32. */
#define LXINTSET1_OFFSET 0x0048	/*  Interrupt clear register bits 63:32. */
#define LXINTSET1_REL (LX_INTCR_BASE_OFFSET + LXINTSET1_OFFSET)	/*  Interrupt clear register bits 63:32. */
#define LXINTMASKCLR0 (LX_INTCR_BASE + LXINTMASKCLR0_OFFSET)	/*  Interrupt mask clear bits 31:0. */
#define LXINTMASKCLR0_OFFSET 0x0108	/*  Interrupt mask clear bits 31:0. */
#define LXINTMASKCLR0_REL (LX_INTCR_BASE_OFFSET + LXINTMASKCLR0_OFFSET)	/*  Interrupt mask clear bits 31:0. */
#define LXINTMASKCLR1 (LX_INTCR_BASE + LXINTMASKCLR1_OFFSET)	/*  Interrupt mask clear bits 63:32. */
#define LXINTMASKCLR1_OFFSET 0x0110	/*  Interrupt mask clear bits 63:32. */
#define LXINTMASKCLR1_REL (LX_INTCR_BASE_OFFSET + LXINTMASKCLR1_OFFSET)	/*  Interrupt mask clear bits 63:32. */
#define LXINTMASKSET0 (LX_INTCR_BASE + LXINTMASKSET0_OFFSET)	/*  Interrupt mask set bits 31:0. */
#define LXINTMASKSET0_OFFSET 0x0118	/*  Interrupt mask set bits 31:0. */
#define LXINTMASKSET0_REL (LX_INTCR_BASE_OFFSET + LXINTMASKSET0_OFFSET)	/*  Interrupt mask set bits 31:0. */
#define LXINTMASKSET1 (LX_INTCR_BASE + LXINTMASKSET1_OFFSET)	/*  Interrupt mask set bits 63:32. */
#define LXINTMASKSET1_OFFSET 0x0120	/*  Interrupt mask set bits 63:32. */
#define LXINTMASKSET1_REL (LX_INTCR_BASE_OFFSET + LXINTMASKSET1_OFFSET)	/*  Interrupt mask set bits 63:32. */
#define LX_C3000_NTIMERS 0x3	/*  Number of timers. */
#define LXTIMECONST0 (LX_INTCR_BASE + LXTIMECONST0_OFFSET)	/*  Timer constant. */
#define LXTIMECONST0_OFFSET 0x0200	/*  Timer constant. */
#define LXTIMECONST0_REL (LX_INTCR_BASE_OFFSET + LXTIMECONST0_OFFSET)	/*  Timer constant. */
#define LXTIMECOUNT0 (LX_INTCR_BASE + LXTIMECOUNT0_OFFSET)	/*  Timer counter. */
#define LXTIMECOUNT0_OFFSET 0x0208	/*  Timer counter. */
#define LXTIMECOUNT0_REL (LX_INTCR_BASE_OFFSET + LXTIMECOUNT0_OFFSET)	/*  Timer counter. */
#define LXTIMECONTROL0 (LX_INTCR_BASE + LXTIMECONTROL0_OFFSET)	/*  Timer control */
#define LXTIMECONTROL0_OFFSET 0x0210	/*  Timer control */
#define LXTIMECONTROL0_REL (LX_INTCR_BASE_OFFSET + LXTIMECONTROL0_OFFSET)	/*  Timer control */
#define LXTIMECONST1 (LX_INTCR_BASE + LXTIMECONST1_OFFSET)	/*  Timer constant. */
#define LXTIMECONST1_OFFSET 0x0218	/*  Timer constant. */
#define LXTIMECONST1_REL (LX_INTCR_BASE_OFFSET + LXTIMECONST1_OFFSET)	/*  Timer constant. */
#define LXTIMECOUNT1 (LX_INTCR_BASE + LXTIMECOUNT1_OFFSET)	/*  Timer counter. */
#define LXTIMECOUNT1_OFFSET 0x0220	/*  Timer counter. */
#define LXTIMECOUNT1_REL (LX_INTCR_BASE_OFFSET + LXTIMECOUNT1_OFFSET)	/*  Timer counter. */
#define LXTIMECONTROL1 (LX_INTCR_BASE + LXTIMECONTROL1_OFFSET)	/*  Timer control */
#define LXTIMECONTROL1_OFFSET 0x0228	/*  Timer control */
#define LXTIMECONTROL1_REL (LX_INTCR_BASE_OFFSET + LXTIMECONTROL1_OFFSET)	/*  Timer control */
#define LXTIMECONST2 (LX_INTCR_BASE + LXTIMECONST2_OFFSET)	/*  Timer constant. */
#define LXTIMECONST2_OFFSET 0x0230	/*  Timer constant. */
#define LXTIMECONST2_REL (LX_INTCR_BASE_OFFSET + LXTIMECONST2_OFFSET)	/*  Timer constant. */
#define LXTIMECOUNT2 (LX_INTCR_BASE + LXTIMECOUNT2_OFFSET)	/*  Timer counter. */
#define LXTIMECOUNT2_OFFSET 0x0238	/*  Timer counter. */
#define LXTIMECOUNT2_REL (LX_INTCR_BASE_OFFSET + LXTIMECOUNT2_OFFSET)	/*  Timer counter. */
#define LXTIMECONTROL2 (LX_INTCR_BASE + LXTIMECONTROL2_OFFSET)	/*  Timer control. */
#define LXTIMECONTROL2_OFFSET 0x0240	/*  Timer control. */
#define LXTIMECONTROL2_REL (LX_INTCR_BASE_OFFSET + LXTIMECONTROL2_OFFSET)	/*  Timer control. */
#define LXTIMEDIVIDE (LX_INTCR_BASE + LXTIMEDIVIDE_OFFSET)	/*  Timer divide. */
#define LXTIMEDIVIDE_OFFSET 0x0248	/*  Timer divide. */
#define LXTIMEDIVIDE_REL (LX_INTCR_BASE_OFFSET + LXTIMEDIVIDE_OFFSET)	/*  Timer divide. */
#define LX_DSU_ROM_HALT (LX_DSU_ROM_BASE + LX_DSU_ROM_HALT_OFFSET)	/*  Address of halt code within DSU Rom. */
#define LX_DSU_ROM_HALT_OFFSET 0x0000	/*  Address of halt code within DSU Rom. */
#define LX_DSU_ROM_HALT_REL (LX_DSU_ROM_BASE_OFFSET + LX_DSU_ROM_HALT_OFFSET)	/*  Address of halt code within DSU Rom. */
#define LX_DSU_ROM_DEBUG (LX_DSU_ROM_BASE + LX_DSU_ROM_DEBUG_OFFSET)	/*  Address of debug code within DSU Rom. */
#define LX_DSU_ROM_DEBUG_OFFSET 0x0004	/*  Address of debug code within DSU Rom. */
#define LX_DSU_ROM_DEBUG_REL (LX_DSU_ROM_BASE_OFFSET + LX_DSU_ROM_DEBUG_OFFSET)	/*  Address of debug code within DSU Rom. */
#define LX_DSU_ROM_EXCEPT (LX_DSU_ROM_BASE + LX_DSU_ROM_EXCEPT_OFFSET)	/*  Address of exception handler within DSU Rom. */
#define LX_DSU_ROM_EXCEPT_OFFSET 0x0008	/*  Address of exception handler within DSU Rom. */
#define LX_DSU_ROM_EXCEPT_REL (LX_DSU_ROM_BASE_OFFSET + LX_DSU_ROM_EXCEPT_OFFSET)	/*  Address of exception handler within DSU Rom. */
#define LOG2_LXDSR_COUNT 0x5	/*  Number of debug registers. */
#define LXDSR_COUNT 0x20	/*  Number of debug registers. */
#define LX_DSU_RES_SIZE 0x1000	/*   */
#define LXDSR0 (LX_DSU_BASE + LXDSR0_OFFSET)	/*  DSU version. */
#define LXDSR0_OFFSET 0x0000	/*  DSU version. */
#define LXDSR0_REL (LX_DSU_BASE_OFFSET + LXDSR0_OFFSET)	/*  DSU version. */
#define LXDSR1 (LX_DSU_BASE + LXDSR1_OFFSET)	/*  DSU status. */
#define LXDSR1_OFFSET 0x0008	/*  DSU status. */
#define LXDSR1_REL (LX_DSU_BASE_OFFSET + LXDSR1_OFFSET)	/*  DSU status. */
#define LXDSR2 (LX_DSU_BASE + LXDSR2_OFFSET)	/*  DSU output. */
#define LXDSR2_OFFSET 0x0010	/*  DSU output. */
#define LXDSR2_REL (LX_DSU_BASE_OFFSET + LXDSR2_OFFSET)	/*  DSU output. */
#define LXDSR3 (LX_DSU_BASE + LXDSR3_OFFSET)	/*  DSU communication. */
#define LXDSR3_OFFSET 0x0018	/*  DSU communication. */
#define LXDSR3_REL (LX_DSU_BASE_OFFSET + LXDSR3_OFFSET)	/*  DSU communication. */
#define LXDSR4 (LX_DSU_BASE + LXDSR4_OFFSET)	/*  DSU communication. */
#define LXDSR4_OFFSET 0x0020	/*  DSU communication. */
#define LXDSR4_REL (LX_DSU_BASE_OFFSET + LXDSR4_OFFSET)	/*  DSU communication. */
#define LXDSR5 (LX_DSU_BASE + LXDSR5_OFFSET)	/*  DSU communication. */
#define LXDSR5_OFFSET 0x0028	/*  DSU communication. */
#define LXDSR5_REL (LX_DSU_BASE_OFFSET + LXDSR5_OFFSET)	/*  DSU communication. */
#define LXDSR6 (LX_DSU_BASE + LXDSR6_OFFSET)	/*  DSU communication. */
#define LXDSR6_OFFSET 0x0030	/*  DSU communication. */
#define LXDSR6_REL (LX_DSU_BASE_OFFSET + LXDSR6_OFFSET)	/*  DSU communication. */
#define LXDSR7 (LX_DSU_BASE + LXDSR7_OFFSET)	/*  DSU communication. */
#define LXDSR7_OFFSET 0x0038	/*  DSU communication. */
#define LXDSR7_REL (LX_DSU_BASE_OFFSET + LXDSR7_OFFSET)	/*  DSU communication. */
#define LXDSR8 (LX_DSU_BASE + LXDSR8_OFFSET)	/*  DSU communication. */
#define LXDSR8_OFFSET 0x0040	/*  DSU communication. */
#define LXDSR8_REL (LX_DSU_BASE_OFFSET + LXDSR8_OFFSET)	/*  DSU communication. */
#define LXDSR9 (LX_DSU_BASE + LXDSR9_OFFSET)	/*  DSU communication. */
#define LXDSR9_OFFSET 0x0048	/*  DSU communication. */
#define LXDSR9_REL (LX_DSU_BASE_OFFSET + LXDSR9_OFFSET)	/*  DSU communication. */
#define LXDSR10 (LX_DSU_BASE + LXDSR10_OFFSET)	/*  DSU communication. */
#define LXDSR10_OFFSET 0x0050	/*  DSU communication. */
#define LXDSR10_REL (LX_DSU_BASE_OFFSET + LXDSR10_OFFSET)	/*  DSU communication. */
#define LXDSR11 (LX_DSU_BASE + LXDSR11_OFFSET)	/*  DSU communication. */
#define LXDSR11_OFFSET 0x0058	/*  DSU communication. */
#define LXDSR11_REL (LX_DSU_BASE_OFFSET + LXDSR11_OFFSET)	/*  DSU communication. */
#define LXDSR12 (LX_DSU_BASE + LXDSR12_OFFSET)	/*  DSU communication. */
#define LXDSR12_OFFSET 0x0060	/*  DSU communication. */
#define LXDSR12_REL (LX_DSU_BASE_OFFSET + LXDSR12_OFFSET)	/*  DSU communication. */
#define LXDSR13 (LX_DSU_BASE + LXDSR13_OFFSET)	/*  DSU communication. */
#define LXDSR13_OFFSET 0x0068	/*  DSU communication. */
#define LXDSR13_REL (LX_DSU_BASE_OFFSET + LXDSR13_OFFSET)	/*  DSU communication. */
#define LXDSR14 (LX_DSU_BASE + LXDSR14_OFFSET)	/*  DSU communication. */
#define LXDSR14_OFFSET 0x0070	/*  DSU communication. */
#define LXDSR14_REL (LX_DSU_BASE_OFFSET + LXDSR14_OFFSET)	/*  DSU communication. */
#define LXDSR15 (LX_DSU_BASE + LXDSR15_OFFSET)	/*  DSU communication. */
#define LXDSR15_OFFSET 0x0078	/*  DSU communication. */
#define LXDSR15_REL (LX_DSU_BASE_OFFSET + LXDSR15_OFFSET)	/*  DSU communication. */
#define LXDSR16 (LX_DSU_BASE + LXDSR16_OFFSET)	/*  DSU communication. */
#define LXDSR16_OFFSET 0x0080	/*  DSU communication. */
#define LXDSR16_REL (LX_DSU_BASE_OFFSET + LXDSR16_OFFSET)	/*  DSU communication. */
#define LXDSR17 (LX_DSU_BASE + LXDSR17_OFFSET)	/*  DSU communication. */
#define LXDSR17_OFFSET 0x0088	/*  DSU communication. */
#define LXDSR17_REL (LX_DSU_BASE_OFFSET + LXDSR17_OFFSET)	/*  DSU communication. */
#define LXDSR18 (LX_DSU_BASE + LXDSR18_OFFSET)	/*  DSU communication. */
#define LXDSR18_OFFSET 0x0090	/*  DSU communication. */
#define LXDSR18_REL (LX_DSU_BASE_OFFSET + LXDSR18_OFFSET)	/*  DSU communication. */
#define LXDSR19 (LX_DSU_BASE + LXDSR19_OFFSET)	/*  DSU communication. */
#define LXDSR19_OFFSET 0x0098	/*  DSU communication. */
#define LXDSR19_REL (LX_DSU_BASE_OFFSET + LXDSR19_OFFSET)	/*  DSU communication. */
#define LXDSR20 (LX_DSU_BASE + LXDSR20_OFFSET)	/*  DSU communication. */
#define LXDSR20_OFFSET 0x00a0	/*  DSU communication. */
#define LXDSR20_REL (LX_DSU_BASE_OFFSET + LXDSR20_OFFSET)	/*  DSU communication. */
#define LXDSR21 (LX_DSU_BASE + LXDSR21_OFFSET)	/*  DSU communication. */
#define LXDSR21_OFFSET 0x00a8	/*  DSU communication. */
#define LXDSR21_REL (LX_DSU_BASE_OFFSET + LXDSR21_OFFSET)	/*  DSU communication. */
#define LXDSR22 (LX_DSU_BASE + LXDSR22_OFFSET)	/*  DSU communication. */
#define LXDSR22_OFFSET 0x00b0	/*  DSU communication. */
#define LXDSR22_REL (LX_DSU_BASE_OFFSET + LXDSR22_OFFSET)	/*  DSU communication. */
#define LXDSR23 (LX_DSU_BASE + LXDSR23_OFFSET)	/*  DSU communication. */
#define LXDSR23_OFFSET 0x00b8	/*  DSU communication. */
#define LXDSR23_REL (LX_DSU_BASE_OFFSET + LXDSR23_OFFSET)	/*  DSU communication. */
#define LXDSR24 (LX_DSU_BASE + LXDSR24_OFFSET)	/*  DSU communication. */
#define LXDSR24_OFFSET 0x00c0	/*  DSU communication. */
#define LXDSR24_REL (LX_DSU_BASE_OFFSET + LXDSR24_OFFSET)	/*  DSU communication. */
#define LXDSR25 (LX_DSU_BASE + LXDSR25_OFFSET)	/*  DSU communication. */
#define LXDSR25_OFFSET 0x00c8	/*  DSU communication. */
#define LXDSR25_REL (LX_DSU_BASE_OFFSET + LXDSR25_OFFSET)	/*  DSU communication. */
#define LXDSR26 (LX_DSU_BASE + LXDSR26_OFFSET)	/*  DSU communication. */
#define LXDSR26_OFFSET 0x00d0	/*  DSU communication. */
#define LXDSR26_REL (LX_DSU_BASE_OFFSET + LXDSR26_OFFSET)	/*  DSU communication. */
#define LXDSR27 (LX_DSU_BASE + LXDSR27_OFFSET)	/*  DSU communication. */
#define LXDSR27_OFFSET 0x00d8	/*  DSU communication. */
#define LXDSR27_REL (LX_DSU_BASE_OFFSET + LXDSR27_OFFSET)	/*  DSU communication. */
#define LXDSR28 (LX_DSU_BASE + LXDSR28_OFFSET)	/*  DSU communication. */
#define LXDSR28_OFFSET 0x00e0	/*  DSU communication. */
#define LXDSR28_REL (LX_DSU_BASE_OFFSET + LXDSR28_OFFSET)	/*  DSU communication. */
#define LXDSR29 (LX_DSU_BASE + LXDSR29_OFFSET)	/*  DSU communication. */
#define LXDSR29_OFFSET 0x00e8	/*  DSU communication. */
#define LXDSR29_REL (LX_DSU_BASE_OFFSET + LXDSR29_OFFSET)	/*  DSU communication. */
#define LXDSR30 (LX_DSU_BASE + LXDSR30_OFFSET)	/*  DSU communication. */
#define LXDSR30_OFFSET 0x00f0	/*  DSU communication. */
#define LXDSR30_REL (LX_DSU_BASE_OFFSET + LXDSR30_OFFSET)	/*  DSU communication. */
#define LXDSR31 (LX_DSU_BASE + LXDSR31_OFFSET)	/*  DSU communication. */
#define LXDSR31_OFFSET 0x00f8	/*  DSU communication. */
#define LXDSR31_REL (LX_DSU_BASE_OFFSET + LXDSR31_OFFSET)	/*  DSU communication. */
#define LOG2_LX_DATA_BITS 0x5	/*  Bits in a word. */
#define LX_DATA_BITS 0x20	/*  Bits in a word. */
#define LOG2_LX_SYL_BITS 0x5	/*  Bits in a syllable. */
#define LX_SYL_BITS 0x20	/*  Bits in a syllable. */
#define LOG2_LX_PTR_BITS 0x5	/*  Bits in a pointer. */
#define LX_PTR_BITS 0x20	/*  Bits in a pointer. */
#define LOG2_LX_DATA_BYTES 0x2	/*  Size of data registers. */
#define LX_DATA_BYTES 0x4	/*  Size of data registers. */
#define LOG2_LX_PTR_BYTES 0x2	/*  Size of pointer. */
#define LX_PTR_BYTES 0x4	/*  Size of pointer. */
#define LOG2_LX_SYL_BYTES 0x2	/*  Size of a syllable. */
#define LX_SYL_BYTES 0x4	/*  Size of a syllable. */
#define LX_DEVICE_REG_ALLOCD_BYTES 0x8	/*  Size unit of device registers. */
#define LOG2_LX_BUNDLE_STOP 0x1f	/*  Bundle Stop Bit Mask. */
#define LX_BUNDLE_STOP 0x80000000	/*  Bundle Stop Bit Mask. */
#define LOG2_LX_CLUSTER_STOP 0x1e	/*  Cluster Stop Bit Mask. */
#define LX_CLUSTER_STOP 0x40000000	/*  Cluster Stop Bit Mask. */
#define LX_CLUSTERS 0x1		/*  Clusters in ST200 core. */
#define LOG2_NUM_SYLLABLES_PER_CLUSTER 0x2	/*   */
#define NUM_SYLLABLES_PER_CLUSTER 0x4	/*   */
#define LOG2_NUM_BYTES_PER_CLUSTER 0x4	/*   */
#define NUM_BYTES_PER_CLUSTER 0x10	/*   */
#define LOG2_MAX_SYLLABLES_PER_BUNDLE 0x4	/*   */
#define MAX_SYLLABLES_PER_BUNDLE 0x10	/*   */
#define LOG2_NUM_SYLLABLES_PER_BUNDLE 0x4	/*   */
#define NUM_SYLLABLES_PER_BUNDLE 0x10	/*   */
#define LOG2_NUM_GEN_REGS_PER_CLUSTER 0x6	/*  General registers per cluster. */
#define NUM_GEN_REGS_PER_CLUSTER 0x40	/*  General registers per cluster. */
#define LOG2_NUM_BRANCH_REGS_PER_CLUSTER 0x3	/*  Branch registers per cluster. */
#define NUM_BRANCH_REGS_PER_CLUSTER 0x8	/*  Branch registers per cluster. */
#define NUM_REGS_PER_CLUSTER 0x48	/*  Total Registers per cluster. */
#define NUM_CLUSTER_INDEP_REGS 0x1	/*   */
#define MAX_GEN_REGS 0x100	/*   */
#define MAX_BRANCH_REGS 0x20	/*   */
#define LOG2_PU_PAGE_BYTES 0xc	/*  Bytes in Page. */
#define PU_PAGE_BYTES 0x1000	/*  Bytes in Page. */
#define LOG2_LX_NUM_PAGES 0x14	/*  Pages in address space. */
#define LX_NUM_PAGES 0x100000	/*  Pages in address space. */
#define LOG2_LX_DCACHE_BYTES 0xf	/*  Bytes in dcache. */
#define LX_DCACHE_BYTES 0x8000	/*  Bytes in dcache. */
#define LOG2_LX_DCACHE_LINE_BYTES 0x5	/*  Bytes in dcache line. */
#define LX_DCACHE_LINE_BYTES 0x20	/*  Bytes in dcache line. */
#define LOG2_LX_DCACHE_ASSOC 0x2	/*  Associativity of dcache. */
#define LX_DCACHE_ASSOC 0x4	/*  Associativity of dcache. */
#define LOG2_LX_DCACHE_TO_WRITEBACK_WIDTH 0x3	/*  data width of the bus from dcache to writeback buffer. */
#define LX_DCACHE_TO_WRITEBACK_WIDTH 0x8	/*  data width of the bus from dcache to writeback buffer. */
#define LOG2_LX_ICACHE_BYTES 0xf	/*  Bytes in icache. */
#define LX_ICACHE_BYTES 0x8000	/*  Bytes in icache. */
#define LOG2_LX_ICACHE_LINE_BYTES 0x6	/*  Bytes in icache line. */
#define LX_ICACHE_LINE_BYTES 0x40	/*  Bytes in icache line. */
#define LOG2_LX_ICACHE_ASSOC 0x0	/*  Associativity of icache. */
#define LX_ICACHE_ASSOC 0x1	/*  Associativity of icache. */
#define LX_DATA_FULL_BYTEENABLE 0xf	/*   */
#define LX_DATA_BYTE_OFFSET_MASK 0x3	/*   */
#define LX_DATA_BYTE_OFFSET_CLEAR_MASK 0xfffffffc	/*   */
#define LX_BITS_PER_BYTE 0x8	/*   */
#define NUM_BYTES_PER_CLUSTER 0x10	/*   */
#define NUM_BYTES_PER_SYLLABLE 0x4	/*   */
#define NUM_REGS 0x121		/*  Registers including pc. */
#define LX_DSU_ROM_RES_SIZE 0x4000
#ifndef LXMAXOPERANDS
#define LXMAXOPERANDS 8
#endif
#ifndef LXMAXDESTOPERANDS
#define LXMAXDESTOPERANDS 4
#endif
/* Icache related */
#define LOG2_LX_NUM_ICACHE_BANKS  LOG2_LX_ICACHE_ASSOC
#define LX_NUM_ICACHE_BANKS   LX_ICACHE_ASSOC
/* Number of Lx syllable-sized data words in a icache line */
#define LOG2_LX_ICACHE_LINE_SYLS (LOG2_LX_ICACHE_LINE_BYTES - LOG2_LX_SYL_BYTES)
#define LX_ICACHE_LINE_SYLS  (1<<(LOG2_LX_ICACHE_LINE_SYLS))
/* Size of a set in bytes */
#define LOG2_LX_ICACHE_SET_BYTES (LOG2_LX_ICACHE_LINE_BYTES + LOG2_LX_NUM_ICACHE_BANKS)
#define LX_ICACHE_SET_BYTES  (1<<(LOG2_LX_ICACHE_SET_BYTES))
#define LOG2_LX_NUM_ICACHE_SETS      (LOG2_LX_ICACHE_BYTES - LOG2_LX_ICACHE_SET_BYTES)
#define LX_NUM_ICACHE_SETS  (1<<(LOG2_LX_NUM_ICACHE_SETS))
/* Size of a bank in bytes */
#define LOG2_LX_ICACHE_BANK_BYTES (LOG2_LX_ICACHE_LINE_BYTES + LOG2_LX_NUM_ICACHE_SETS)
#define LX_ICACHE_BANK_BYTES  (1<<(LOG2_LX_ICACHE_BANK_BYTES))
#define LX_NUM_ICACHE_TAG_BITS  (LX_PTR_BITS - LOG2_LX_ICACHE_BANK_BYTES)
/* Dcache related */
#define LOG2_LX_NUM_DCACHE_BANKS LOG2_LX_DCACHE_ASSOC
#define LX_NUM_DCACHE_BANKS  LX_DCACHE_ASSOC
/* Number of Lx Register-sized data words in a dcache line */
#define LOG2_LX_DCACHE_LINE_WORDS (LOG2_LX_DCACHE_LINE_BYTES - LOG2_LX_DATA_BYTES)
#define LX_DCACHE_LINE_WORDS  (1<<(LOG2_LX_DCACHE_LINE_WORDS))
/* Size of a set in bytes */
#define LOG2_LX_DCACHE_SET_BYTES (LOG2_LX_DCACHE_LINE_BYTES + LOG2_LX_NUM_DCACHE_BANKS)
#define LX_DCACHE_SET_BYTES  (1<<(LOG2_LX_DCACHE_SET_BYTES))
#define LOG2_LX_NUM_DCACHE_SETS      (LOG2_LX_DCACHE_BYTES - LOG2_LX_DCACHE_SET_BYTES)
#define LX_NUM_DCACHE_SETS  (1<<(LOG2_LX_NUM_DCACHE_SETS))
/* Size of a bank in bytes */
#define LOG2_LX_DCACHE_BANK_BYTES (LOG2_LX_DCACHE_LINE_BYTES + LOG2_LX_NUM_DCACHE_SETS)
#define LX_DCACHE_BANK_BYTES  (1<<(LOG2_LX_DCACHE_BANK_BYTES))
#define LX_NUM_DCACHE_TAG_BITS  (LX_PTR_BITS - LOG2_LX_DCACHE_BANK_BYTES)
#define LX_ICACHE_LINE_SELECT_MASK FIELD_MASK(LOG2_LX_ICACHE_LINE_BYTES, LOG2_LX_NUM_ICACHE_SETS)
#define LX_DCACHE_LINE_SELECT_MASK FIELD_MASK(LOG2_LX_DCACHE_LINE_BYTES, LOG2_LX_NUM_DCACHE_SETS)
#define EXCNO_STBUS_IC_ERROR   (LXEXCAUSENO_EXCAUSENO_STBUS_IC_ERROR)
#define EXC_STBUS_IC_ERROR   (1<<(LXEXCAUSENO_EXCAUSENO_STBUS_IC_ERROR))
#define EXCNO_STBUS_DC_ERROR   (LXEXCAUSENO_EXCAUSENO_STBUS_DC_ERROR)
#define EXC_STBUS_DC_ERROR   (1<<(LXEXCAUSENO_EXCAUSENO_STBUS_DC_ERROR))
#define EXCNO_ITLB    (LXEXCAUSENO_EXCAUSENO_ITLB)
#define EXC_ITLB    (1<<(LXEXCAUSENO_EXCAUSENO_ITLB))
#define EXCNO_DTLB    (LXEXCAUSENO_EXCAUSENO_DTLB)
#define EXC_DTLB    (1<<(LXEXCAUSENO_EXCAUSENO_DTLB))
#define EXCNO_EXTERN_INT   (LXEXCAUSENO_EXCAUSENO_EXTERN_INT)
#define EXC_EXTERN_INT    (1<<(LXEXCAUSENO_EXCAUSENO_EXTERN_INT))
#define EXCNO_SBREAK    (LXEXCAUSENO_EXCAUSENO_SBREAK)
#define EXC_SBREAK    (1<<(LXEXCAUSENO_EXCAUSENO_SBREAK))
#define EXCNO_SYSCALL    (LXEXCAUSENO_EXCAUSENO_SYSCALL)
#define EXC_SYSCALL    (1<<(LXEXCAUSENO_EXCAUSENO_SYSCALL))
#define EXCNO_IBREAK    (LXEXCAUSENO_EXCAUSENO_IBREAK)
#define EXC_IBREAK    (1<<(LXEXCAUSENO_EXCAUSENO_IBREAK))
#define EXCNO_ILL_INST    (LXEXCAUSENO_EXCAUSENO_ILL_INST)
#define EXC_ILL_INST    (1<<(LXEXCAUSENO_EXCAUSENO_ILL_INST))
#define EXCNO_DBREAK    (LXEXCAUSENO_EXCAUSENO_DBREAK)
#define EXC_DBREAK    (1<<(LXEXCAUSENO_EXCAUSENO_DBREAK))
#define EXCNO_MISALIGNED_TRAP   (LXEXCAUSENO_EXCAUSENO_MISALIGNED_TRAP)
#define EXC_MISALIGNED_TRAP   (1<<(LXEXCAUSENO_EXCAUSENO_MISALIGNED_TRAP))
#define EXCNO_CREG_NO_MAPPING   (LXEXCAUSENO_EXCAUSENO_CREG_NO_MAPPING)
#define EXC_CREG_NO_MAPPING   (1<<(LXEXCAUSENO_EXCAUSENO_CREG_NO_MAPPING))
#define EXCNO_CREG_ACCESS_VIOLATION  (LXEXCAUSENO_EXCAUSENO_CREG_ACCESS_VIOLATION)
#define EXC_CREG_ACCESS_VIOLATION  (1<<(LXEXCAUSENO_EXCAUSENO_CREG_ACCESS_VIOLATION))
#define EXCNO_SDI_TIMEOUT   (LXEXCAUSENO_EXCAUSENO_SDI_TIMEOUT)
#define EXC_SDI_TIMEOUT   (1<<(LXEXCAUSENO_EXCAUSENO_SDI_TIMEOUT))
#define LXDSU_VERSION    (LXDSR0)
#define LXDSU_STATUS     (LXDSR1)
/*  Define old DSU_SR defines as DSR1 */
#define LXDSU_SR_DEBUG_INT_MODE    LXDSR1_DEBUG_INTERRUPT_TAKEN
#define LXDSU_SR_DEBUG_INT_MODE_LO   LXDSR1_DEBUG_INTERRUPT_TAKEN_LO
#define LXDSU_SR_DEBUG_INT_MODE_SZ   LXDSR1_DEBUG_INTERRUPT_TAKEN_SZ
#define LXDSU_SR_DEBUG_INT_MODE_DEP   LXDSR1_DEBUG_INTERRUPT_TAKEN_DEP
#define LXDSU_SR_SUPERVISOR_WRITE_ENABLE  LXDSR1_SUPERVISOR_WRITE_ENABLE
#define LXDSU_SR_SUPERVISOR_WRITE_ENABLE_LO  LXDSR1_SUPERVISOR_WRITE_ENABLE_LO
#define LXDSU_SR_SUPERVISOR_WRITE_ENABLE_SZ  LXDSR1_SUPERVISOR_WRITE_ENABLE_SZ
#define LXDSU_SR_SUPERVISOR_WRITE_ENABLE_DEP    LXDSR1_SUPERVISOR_WRITE_ENABLE_DEP
#define LXDSU_SR_USER_WRITE_ENABLE   LXDSR1_USER_WRITE_ENABLE
#define LXDSU_SR_USER_WRITE_ENABLE_LO   LXDSR1_USER_WRITE_ENABLE_LO
#define LXDSU_SR_USER_WRITE_ENABLE_SZ   LXDSR1_USER_WRITE_ENABLE_SZ
#define LXDSU_SR_USER_WRITE_ENABLE_DEP   LXDSR1_USER_WRITE_ENABLE_DEP
#define LXDSU_SR_OUTPUT_PENDING    LXDSR1_OUTPUT_PENDING
#define LXDSU_SR_OUTPUT_PENDING_LO   LXDSR1_OUTPUT_PENDING_LO
#define LXDSU_SR_OUTPUT_PENDING_SZ   LXDSR1_OUTPUT_PENDING_SZ
#define LXDSU_SR_OUTPUT_PENDING_DEP   LXDSR1_OUTPUT_PENDING_DEP
#define LXDSU_SR_TRIGGER_IN_LO    LXDSR1_TRIGGER_IN_LO
#define LXDSU_SR_TRIGGER_IN_SZ    LXDSR1_TRIGGER_IN_SZ
#define LXDSU_SR_TRIGGER_IN    LXDSR1_TRIGGER_IN
#define LXDSU_SR_TRIGGER_IN_DEP    LXDSR1_TRIGGER_IN_DEP
#define LXDSU_SR_TRIGGER_OUT_LO    LXDSR1_TRIGGER_OUT_LO
#define LXDSU_SR_TRIGGER_OUT_SZ    LXDSR1_TRIGGER_OUT_SZ
#define LXDSU_SR_TRIGGER_OUT    LXDSR1_TRIGGER_OUT
#define LXDSU_SR_TRIGGER_OUT_DEP   LXDSR1_TRIGGER_OUT_DEP
#define LXDSU_SR_TRIGGER_ENABLE_LO   LXDSR1_TRIGGER_ENABLE_LO
#define LXDSU_SR_TRIGGER_ENABLE_SZ   LXDSR1_TRIGGER_ENABLE_SZ
#define LXDSU_SR_TRIGGER_ENABLE    LXDSR1_TRIGGER_ENABLE
#define LXDSU_SR_TRIGGER_ENABLE_DEP   LXDSR1_TRIGGER_ENABLE_DEP
#define LXRESERVED50 (LX_INTCR_BASE + LXRESERVED50_OFFSET)	/*  Reserved */
#define LXRESERVED50_OFFSET 0x0050	/*  Reserved */
#define LXRESERVED50_REL (LX_INTCR_BASE_OFFSET + LXRESERVED50_OFFSET)	/*  Reserved */
#define LXRESERVED58 (LX_INTCR_BASE + LXRESERVED58_OFFSET)	/*  Reserved */
#define LXRESERVED58_OFFSET 0x0058	/*  Reserved */
#define LXRESERVED58_REL (LX_INTCR_BASE_OFFSET + LXRESERVED58_OFFSET)	/*  Reserved */
#define LXRESERVED60 (LX_INTCR_BASE + LXRESERVED60_OFFSET)	/*  Reserved */
#define LXRESERVED60_OFFSET 0x0060	/*  Reserved */
#define LXRESERVED60_REL (LX_INTCR_BASE_OFFSET + LXRESERVED60_OFFSET)	/*  Reserved */
#define LXRESERVED68 (LX_INTCR_BASE + LXRESERVED68_OFFSET)	/*  RReserved */
#define LXRESERVED68_OFFSET 0x0068	/*  RReserved */
#define LXRESERVED68_REL (LX_INTCR_BASE_OFFSET + LXRESERVED68_OFFSET)	/*  RReserved */
#define LXRESERVED70 (LX_INTCR_BASE + LXRESERVED70_OFFSET)	/*  Reserved */
#define LXRESERVED70_OFFSET 0x0070	/*  Reserved */
#define LXRESERVED70_REL (LX_INTCR_BASE_OFFSET + LXRESERVED70_OFFSET)	/*  Reserved */
#define LXRESERVED78 (LX_INTCR_BASE + LXRESERVED78_OFFSET)	/*  Reserved */
#define LXRESERVED78_OFFSET 0x0078	/*  Reserved */
#define LXRESERVED78_REL (LX_INTCR_BASE_OFFSET + LXRESERVED78_OFFSET)	/*  Reserved */
#define LXRESERVED80 (LX_INTCR_BASE + LXRESERVED80_OFFSET)	/*  Reserved */
#define LXRESERVED80_OFFSET 0x0080	/*  Reserved */
#define LXRESERVED80_REL (LX_INTCR_BASE_OFFSET + LXRESERVED80_OFFSET)	/*  Reserved */
#define LXRESERVED88 (LX_INTCR_BASE + LXRESERVED88_OFFSET)	/*  Reserved */
#define LXRESERVED88_OFFSET 0x0088	/*  Reserved */
#define LXRESERVED88_REL (LX_INTCR_BASE_OFFSET + LXRESERVED88_OFFSET)	/*  Reserved */
#define LXRESERVED90 (LX_INTCR_BASE + LXRESERVED90_OFFSET)	/*  Reserved */
#define LXRESERVED90_OFFSET 0x0090	/*  Reserved */
#define LXRESERVED90_REL (LX_INTCR_BASE_OFFSET + LXRESERVED90_OFFSET)	/*  Reserved */
#define LXRESERVED98 (LX_INTCR_BASE + LXRESERVED98_OFFSET)	/*  Reserved */
#define LXRESERVED98_OFFSET 0x0098	/*  Reserved */
#define LXRESERVED98_REL (LX_INTCR_BASE_OFFSET + LXRESERVED98_OFFSET)	/*  Reserved */
#define LXRESERVED100 (LX_INTCR_BASE + LXRESERVED100_OFFSET)	/*  Reserved */
#define LXRESERVED100_OFFSET 0x0100	/*  Reserved */
#define LXRESERVED100_REL (LX_INTCR_BASE_OFFSET + LXRESERVED100_OFFSET)	/*  Reserved */
#define LXRESERVEDff38 (LX_CREG_BASE + LXRESERVEDff38_OFFSET)	/*  Reserved */
#define LXRESERVEDff38_OFFSET 0xff38	/*  Reserved */
#define LXRESERVEDff38_REL (LX_CREG_BASE_OFFSET + LXRESERVEDff38_OFFSET)	/*  Reserved */
#define LXRESERVEDff30 (LX_CREG_BASE + LXRESERVEDff30_OFFSET)	/*  Reserved */
#define LXRESERVEDff30_OFFSET 0xff30	/*  Reserved */
#define LXRESERVEDff30_REL (LX_CREG_BASE_OFFSET + LXRESERVEDff30_OFFSET)	/*  Reserved */
#define LXRESERVEDff28 (LX_CREG_BASE + LXRESERVEDff28_OFFSET)	/*  Reserved */
#define LXRESERVEDff28_OFFSET 0xff28	/*  Reserved */
#define LXRESERVEDff28_REL (LX_CREG_BASE_OFFSET + LXRESERVEDff28_OFFSET)	/*  Reserved */
#define LXRESERVEDff20 (LX_CREG_BASE + LXRESERVEDff20_OFFSET)	/*  Reserved */
#define LXRESERVEDff20_OFFSET 0xff20	/*  Reserved */
#define LXRESERVEDff20_REL (LX_CREG_BASE_OFFSET + LXRESERVEDff20_OFFSET)	/*  Reserved */
#define LXRESERVEDff18 (LX_CREG_BASE + LXRESERVEDff18_OFFSET)	/*  Reserved */
#define LXRESERVEDff18_OFFSET 0xff18	/*  Reserved */
#define LXRESERVEDff18_REL (LX_CREG_BASE_OFFSET + LXRESERVEDff18_OFFSET)	/*  Reserved */
#define LXRESERVEDff10 (LX_CREG_BASE + LXRESERVEDff10_OFFSET)	/*  Reserved */
#define LXRESERVEDff10_OFFSET 0xff10	/*  Reserved */
#define LXRESERVEDff10_REL (LX_CREG_BASE_OFFSET + LXRESERVEDff10_OFFSET)	/*  Reserved */
#define LXRESERVEDff08 (LX_CREG_BASE + LXRESERVEDff08_OFFSET)	/*  Reserved */
#define LXRESERVEDff08_OFFSET 0xff08	/*  Reserved */
#define LXRESERVEDff08_REL (LX_CREG_BASE_OFFSET + LXRESERVEDff08_OFFSET)	/*  Reserved */
#define LXRESERVEDff00 (LX_CREG_BASE + LXRESERVEDff00_OFFSET)	/*  Reserved */
#define LXRESERVEDff00_OFFSET 0xff00	/*  Reserved */
#define LXRESERVEDff00_REL (LX_CREG_BASE_OFFSET + LXRESERVEDff00_OFFSET)	/*  Reserved */
#define LXRESERVEDfe40 (LX_CREG_BASE + LXRESERVEDfe40_OFFSET)	/*  Reserved */
#define LXRESERVEDfe40_OFFSET 0xfe40	/*  Reserved */
#define LXRESERVEDfe40_REL (LX_CREG_BASE_OFFSET + LXRESERVEDfe40_OFFSET)	/*  Reserved */
#define LXRESERVEDfe38 (LX_CREG_BASE + LXRESERVEDfe38_OFFSET)	/*  Reserved */
#define LXRESERVEDfe38_OFFSET 0xfe38	/*  Reserved */
#define LXRESERVEDfe38_REL (LX_CREG_BASE_OFFSET + LXRESERVEDfe38_OFFSET)	/*  Reserved */
#define LXRESERVEDfe30 (LX_CREG_BASE + LXRESERVEDfe30_OFFSET)	/*  Reserved */
#define LXRESERVEDfe30_OFFSET 0xfe30	/*  Reserved */
#define LXRESERVEDfe30_REL (LX_CREG_BASE_OFFSET + LXRESERVEDfe30_OFFSET)	/*  Reserved */
#define LXRESERVEDfe28 (LX_CREG_BASE + LXRESERVEDfe28_OFFSET)	/*  Reserved */
#define LXRESERVEDfe28_OFFSET 0xfe28	/*  Reserved */
#define LXRESERVEDfe28_REL (LX_CREG_BASE_OFFSET + LXRESERVEDfe28_OFFSET)	/*  Reserved */
#define LXRESERVEDfe20 (LX_CREG_BASE + LXRESERVEDfe20_OFFSET)	/*  Reserved */
#define LXRESERVEDfe20_OFFSET 0xfe20	/*  Reserved */
#define LXRESERVEDfe20_REL (LX_CREG_BASE_OFFSET + LXRESERVEDfe20_OFFSET)	/*  Reserved */
#define LXRESERVEDfe18 (LX_CREG_BASE + LXRESERVEDfe18_OFFSET)	/*  Reserved */
#define LXRESERVEDfe18_OFFSET 0xfe18	/*  Reserved */
#define LXRESERVEDfe18_REL (LX_CREG_BASE_OFFSET + LXRESERVEDfe18_OFFSET)	/*  Reserved */
#define LXRESERVEDfe10 (LX_CREG_BASE + LXRESERVEDfe10_OFFSET)	/*  Reserved */
#define LXRESERVEDfe10_OFFSET 0xfe10	/*  Reserved */
#define LXRESERVEDfe10_REL (LX_CREG_BASE_OFFSET + LXRESERVEDfe10_OFFSET)	/*  Reserved */
#define LXRESERVEDfe08 (LX_CREG_BASE + LXRESERVEDfe08_OFFSET)	/*  Reserved */
#define LXRESERVEDfe08_OFFSET 0xfe08	/*  Reserved */
#define LXRESERVEDfe08_REL (LX_CREG_BASE_OFFSET + LXRESERVEDfe08_OFFSET)	/*  Reserved */
#define LXSTATE1_PARTITION_LO 0	/*  Sets the maximum value for the round robin data cache replacement counters. 00: Replace ways 0-3. 01: Replace ways 0-2. 10: Replace ways 0-1. 11: Replace way 0 only. Changing this field does not alter the current replacement pointers, thus if a replacement pointer is set to 2 and this field is changed to 10 way 2 will still be replaced next time (and then the counter will be set back to 0). */
#define LXSTATE1_PARTITION_SZ 2	/*  Sets the maximum value for the round robin data cache replacement counters. 00: Replace ways 0-3. 01: Replace ways 0-2. 10: Replace ways 0-1. 11: Replace way 0 only. Changing this field does not alter the current replacement pointers, thus if a replacement pointer is set to 2 and this field is changed to 10 way 2 will still be replaced next time (and then the counter will be set back to 0). */
#define LXSTATE1_PARTITION(word)           (((word) & 0x3) >> 0)
#define LXSTATE1_PARTITION_DEP(base, val)  (((base) & 0xfffffffc) | (((val) << 0) & (0x3)))
#define LXSTATE1_RESERVED2_LO 2	/*  Reserved. */
#define LXSTATE1_RESERVED2_SZ 30	/*  Reserved. */
#define LXSTATE1_RESERVED2(word)           (((word) & 0xfffffffc) >> 2)
#define LXSTATE1_RESERVED2_DEP(base, val)  (((base) & 0x3) | (((val) << 2) & (0xfffffffc)))
#define LXEXCAUSENO_EXCAUSENO_LO 0	/*  Specifies the exception number. */
#define LXEXCAUSENO_EXCAUSENO_SZ 5	/*  Specifies the exception number. */
#define LXEXCAUSENO_EXCAUSENO(word)           (((word) & 0x1f) >> 0)
#define LXEXCAUSENO_EXCAUSENO_DEP(base, val)  (((base) & 0xffffffe0) | (((val) << 0) & (0x1f)))
#define LXEXCAUSENO_RESERVED5_LO 5	/*  Reserved. */
#define LXEXCAUSENO_RESERVED5_SZ 27	/*  Reserved. */
#define LXEXCAUSENO_RESERVED5(word)           (((word) & 0xffffffe0) >> 5)
#define LXEXCAUSENO_RESERVED5_DEP(base, val)  (((base) & 0x1f) | (((val) << 5) & (0xffffffe0)))
#define LXDSR0_PRODUCT_ID_LO 0	/*  Chip ID. */
#define LXDSR0_PRODUCT_ID_SZ 16	/*  Chip ID. */
#define LXDSR0_PRODUCT_ID(word)           (((word) & 0xffff) >> 0)
#define LXDSR0_PRODUCT_ID_DEP(base, val)  (((base) & 0xffff0000) | (((val) << 0) & (0xffff)))
#define LXDSR0_CORE_VERSION_LO 16	/*  Core version number. */
#define LXDSR0_CORE_VERSION_SZ 8	/*  Core version number. */
#define LXDSR0_CORE_VERSION(word)           (((word) & 0xff0000) >> 16)
#define LXDSR0_CORE_VERSION_DEP(base, val)  (((base) & 0xff00ffff) | (((val) << 16) & (0xff0000)))
#define LXDSR0_DSU_VERSION_LO 24	/*  DSU design version number. */
#define LXDSR0_DSU_VERSION_SZ 8	/*  DSU design version number. */
#define LXDSR0_DSU_VERSION(word)           (((word) & 0xff000000) >> 24)
#define LXDSR0_DSU_VERSION_DEP(base, val)  (((base) & 0xffffff) | (((val) << 24) & (0xff000000)))
#define LX_VERSION_PRODUCT_ID_LO 0	/*  Chip ID. */
#define LX_VERSION_PRODUCT_ID_SZ 16	/*  Chip ID. */
#define LX_VERSION_PRODUCT_ID(word)           (((word) & 0xffff) >> 0)
#define LX_VERSION_PRODUCT_ID_DEP(base, val)  (((base) & 0xffff0000) | (((val) << 0) & (0xffff)))
#define LX_VERSION_CORE_VERSION_LO 16	/*  Core version number. */
#define LX_VERSION_CORE_VERSION_SZ 8	/*  Core version number. */
#define LX_VERSION_CORE_VERSION(word)           (((word) & 0xff0000) >> 16)
#define LX_VERSION_CORE_VERSION_DEP(base, val)  (((base) & 0xff00ffff) | (((val) << 16) & (0xff0000)))
#define LX_VERSION_DSU_VERSION_LO 24	/*  DSU design version number. */
#define LX_VERSION_DSU_VERSION_SZ 8	/*  DSU design version number. */
#define LX_VERSION_DSU_VERSION(word)           (((word) & 0xff000000) >> 24)
#define LX_VERSION_DSU_VERSION_DEP(base, val)  (((base) & 0xffffff) | (((val) << 24) & (0xff000000)))
#define PSW_USER_MODE_LO 0	/*  When 1 the core is in user mode, otherwise supervisor mode. */
#define PSW_USER_MODE_SZ 1	/*  When 1 the core is in user mode, otherwise supervisor mode. */
#define PSW_USER_MODE(word)           (((word) & 0x1) >> 0)
#define PSW_USER_MODE_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define PSW_INT_ENABLE_LO 1	/*  When 1 external interrupts are enabled. */
#define PSW_INT_ENABLE_SZ 1	/*  When 1 external interrupts are enabled. */
#define PSW_INT_ENABLE(word)           (((word) & 0x2) >> 1)
#define PSW_INT_ENABLE_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define PSW_TLB_ENABLE_LO 2	/*  When 1 address translation is enabled. */
#define PSW_TLB_ENABLE_SZ 1	/*  When 1 address translation is enabled. */
#define PSW_TLB_ENABLE(word)           (((word) & 0x4) >> 2)
#define PSW_TLB_ENABLE_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define PSW_TLB_DYNAMIC_LO 3	/*  When 1 prefetches, speculative loads and purge address cause no mapping violations. When 0 the violations are ignored. */
#define PSW_TLB_DYNAMIC_SZ 1	/*  When 1 prefetches, speculative loads and purge address cause no mapping violations. When 0 the violations are ignored. */
#define PSW_TLB_DYNAMIC(word)           (((word) & 0x8) >> 3)
#define PSW_TLB_DYNAMIC_DEP(base, val)  (((base) & 0xfffffff7) | (((val) << 3) & (0x8)))
#define PSW_SPECLOAD_MALIGNTRAP_EN_LO 4	/*  When 1 enables exceptions on speculative load misalignment errors. */
#define PSW_SPECLOAD_MALIGNTRAP_EN_SZ 1	/*  When 1 enables exceptions on speculative load misalignment errors. */
#define PSW_SPECLOAD_MALIGNTRAP_EN(word)           (((word) & 0x10) >> 4)
#define PSW_SPECLOAD_MALIGNTRAP_EN_DEP(base, val)  (((base) & 0xffffffef) | (((val) << 4) & (0x10)))
#define PSW_RESERVED5_LO 5	/*  Reserved */
#define PSW_RESERVED5_SZ 1	/*  Reserved */
#define PSW_RESERVED5(word)           (((word) & 0x20) >> 5)
#define PSW_RESERVED5_DEP(base, val)  (((base) & 0xffffffdf) | (((val) << 5) & (0x20)))
#define PSW_RESERVED6_LO 6	/*  Reserved */
#define PSW_RESERVED6_SZ 1	/*  Reserved */
#define PSW_RESERVED6(word)           (((word) & 0x40) >> 6)
#define PSW_RESERVED6_DEP(base, val)  (((base) & 0xffffffbf) | (((val) << 6) & (0x40)))
#define PSW_RESERVED7_LO 7	/*  Reserved */
#define PSW_RESERVED7_SZ 1	/*  Reserved */
#define PSW_RESERVED7(word)           (((word) & 0x80) >> 7)
#define PSW_RESERVED7_DEP(base, val)  (((base) & 0xffffff7f) | (((val) << 7) & (0x80)))
#define PSW_DBREAK_ENABLE_LO 8	/*  When 1 data breakpoints are enabled. */
#define PSW_DBREAK_ENABLE_SZ 1	/*  When 1 data breakpoints are enabled. */
#define PSW_DBREAK_ENABLE(word)           (((word) & 0x100) >> 8)
#define PSW_DBREAK_ENABLE_DEP(base, val)  (((base) & 0xfffffeff) | (((val) << 8) & (0x100)))
#define PSW_IBREAK_ENABLE_LO 9	/*  When 1 instruction breakpoints are enabled. */
#define PSW_IBREAK_ENABLE_SZ 1	/*  When 1 instruction breakpoints are enabled. */
#define PSW_IBREAK_ENABLE(word)           (((word) & 0x200) >> 9)
#define PSW_IBREAK_ENABLE_DEP(base, val)  (((base) & 0xfffffdff) | (((val) << 9) & (0x200)))
#define PSW_RESERVED10_LO 10	/*  Reserved */
#define PSW_RESERVED10_SZ 1	/*  Reserved */
#define PSW_RESERVED10(word)           (((word) & 0x400) >> 10)
#define PSW_RESERVED10_DEP(base, val)  (((base) & 0xfffffbff) | (((val) << 10) & (0x400)))
#define PSW_RESERVED11_LO 11	/*  Reserved */
#define PSW_RESERVED11_SZ 1	/*  Reserved */
#define PSW_RESERVED11(word)           (((word) & 0x800) >> 11)
#define PSW_RESERVED11_DEP(base, val)  (((base) & 0xfffff7ff) | (((val) << 11) & (0x800)))
#define PSW_DEBUG_MODE_LO 12	/*  When 1 the core is in debug mode. */
#define PSW_DEBUG_MODE_SZ 1	/*  When 1 the core is in debug mode. */
#define PSW_DEBUG_MODE(word)           (((word) & 0x1000) >> 12)
#define PSW_DEBUG_MODE_DEP(base, val)  (((base) & 0xffffefff) | (((val) << 12) & (0x1000)))
#define PSW_RESERVED13_LO 13	/*  Reserved */
#define PSW_RESERVED13_SZ 19	/*  Reserved */
#define PSW_RESERVED13(word)           (((word) & 0xffffe000) >> 13)
#define PSW_RESERVED13_DEP(base, val)  (((base) & 0x1fff) | (((val) << 13) & (0xffffe000)))
#define LXEXCAUSENO_EXCAUSENO_STBUS_IC_ERROR 0	/*  The Instruction Cache caused a bus error. */
#define LXEXCAUSENO_EXCAUSENO_STBUS_DC_ERROR 1	/*  The Data Cache caused a bus error. */
#define LXEXCAUSENO_EXCAUSENO_EXTERN_INT 2	/*  There was an external interrupt. */
#define LXEXCAUSENO_EXCAUSENO_IBREAK 3	/*  An instruction address breakpoint has occured. */
#define LXEXCAUSENO_EXCAUSENO_ITLB 4	/*  An instruction related TLB exception has occured. */
#define LXEXCAUSENO_EXCAUSENO_SBREAK 5	/*  A software breakpoint was found. */
#define LXEXCAUSENO_EXCAUSENO_ILL_INST 6	/*  The bundle could not be decoded into legal sequence of operations or a privileged operation is being issued in user mode. */
#define LXEXCAUSENO_EXCAUSENO_SYSCALL 7	/*  System call. */
#define LXEXCAUSENO_EXCAUSENO_DBREAK 8	/*  A breakpoint on a data address has been triggered. */
#define LXEXCAUSENO_EXCAUSENO_MISALIGNED_TRAP 9	/*  The address is misaligned and misaligned accesses are not supported. */
#define LXEXCAUSENO_EXCAUSENO_CREG_NO_MAPPING 10	/*  The load or store address was in control register space, but no control register exists at that exact address. */
#define LXEXCAUSENO_EXCAUSENO_CREG_ACCESS_VIOLATION 11	/*  A store to a control register was attempted whilst in user mode. */
#define LXEXCAUSENO_EXCAUSENO_DTLB 12	/*  A data related TLB exception has occured. */
#define LXEXCAUSENO_EXCAUSENO_RESERVED13 13	/*  Reserved */
#define LXEXCAUSENO_EXCAUSENO_SDI_TIMEOUT 14	/*  One of the SDI interfaces timed out while being accessed. */
#define LXTLB_INDEX_ENTRY_LO 0	/*  Determines which of the 64 TLB entries is mapped to the TLB_ENTRYx registers. Writing a value to this register that is greater than the maximum UTLB entry available has no effort (the entry is not updated). */
#define LXTLB_INDEX_ENTRY_SZ 8	/*  Determines which of the 64 TLB entries is mapped to the TLB_ENTRYx registers. Writing a value to this register that is greater than the maximum UTLB entry available has no effort (the entry is not updated). */
#define LXTLB_INDEX_ENTRY(word)           (((word) & 0xff) >> 0)
#define LXTLB_INDEX_ENTRY_DEP(base, val)  (((base) & 0xffffff00) | (((val) << 0) & (0xff)))
#define LXTLB_INDEX_RESERVED8_LO 8	/*  Reserved. */
#define LXTLB_INDEX_RESERVED8_SZ 24	/*  Reserved. */
#define LXTLB_INDEX_RESERVED8(word)           (((word) & 0xffffff00) >> 8)
#define LXTLB_INDEX_RESERVED8_DEP(base, val)  (((base) & 0xff) | (((val) << 8) & (0xffffff00)))
#define LXTLB_ENTRY0_ASID_LO 0	/*  Indicates which process this page belongs to. */
#define LXTLB_ENTRY0_ASID_SZ 8	/*  Indicates which process this page belongs to. */
#define LXTLB_ENTRY0_ASID(word)           (((word) & 0xff) >> 0)
#define LXTLB_ENTRY0_ASID_DEP(base, val)  (((base) & 0xffffff00) | (((val) << 0) & (0xff)))
#define LXTLB_ENTRY0_SHARED_LO 8	/*  Page shared by multiple processes (ASIDs). */
#define LXTLB_ENTRY0_SHARED_SZ 1	/*  Page shared by multiple processes (ASIDs). */
#define LXTLB_ENTRY0_SHARED(word)           (((word) & 0x100) >> 8)
#define LXTLB_ENTRY0_SHARED_DEP(base, val)  (((base) & 0xfffffeff) | (((val) << 8) & (0x100)))
#define LXTLB_ENTRY0_PROT_SUPER_LO 9	/*  A three bit field that defines the protection of this region in supervisor mode. */
#define LXTLB_ENTRY0_PROT_SUPER_SZ 3	/*  A three bit field that defines the protection of this region in supervisor mode. */
#define LXTLB_ENTRY0_PROT_SUPER(word)           (((word) & 0xe00) >> 9)
#define LXTLB_ENTRY0_PROT_SUPER_DEP(base, val)  (((base) & 0xfffff1ff) | (((val) << 9) & (0xe00)))
#define LXTLB_ENTRY0_PROT_USER_LO 12	/*  A three bit field that defines the protection of this region in user mode. */
#define LXTLB_ENTRY0_PROT_USER_SZ 3	/*  A three bit field that defines the protection of this region in user mode. */
#define LXTLB_ENTRY0_PROT_USER(word)           (((word) & 0x7000) >> 12)
#define LXTLB_ENTRY0_PROT_USER_DEP(base, val)  (((base) & 0xffff8fff) | (((val) << 12) & (0x7000)))
#define LXTLB_ENTRY0_DIRTY_LO 15	/*  Page is dirty. When this bit is 0 write accesses to this page (when write permission is allowed) cause a TLB_WRITE_TO_CLEAN exception. When this bit is 1 writes to this page (when write permission is allowed) are permitted. */
#define LXTLB_ENTRY0_DIRTY_SZ 1	/*  Page is dirty. When this bit is 0 write accesses to this page (when write permission is allowed) cause a TLB_WRITE_TO_CLEAN exception. When this bit is 1 writes to this page (when write permission is allowed) are permitted. */
#define LXTLB_ENTRY0_DIRTY(word)           (((word) & 0x8000) >> 15)
#define LXTLB_ENTRY0_DIRTY_DEP(base, val)  (((base) & 0xffff7fff) | (((val) << 15) & (0x8000)))
#define LXTLB_ENTRY0_POLICY_LO 16	/*  Cache policy for this page. */
#define LXTLB_ENTRY0_POLICY_SZ 4	/*  Cache policy for this page. */
#define LXTLB_ENTRY0_POLICY(word)           (((word) & 0xf0000) >> 16)
#define LXTLB_ENTRY0_POLICY_DEP(base, val)  (((base) & 0xfff0ffff) | (((val) << 16) & (0xf0000)))
#define LXTLB_ENTRY0_SIZE_LO 20	/*  Size of this page (also used to disable the page). */
#define LXTLB_ENTRY0_SIZE_SZ 3	/*  Size of this page (also used to disable the page). */
#define LXTLB_ENTRY0_SIZE(word)           (((word) & 0x700000) >> 20)
#define LXTLB_ENTRY0_SIZE_DEP(base, val)  (((base) & 0xff8fffff) | (((val) << 20) & (0x700000)))
#define LXTLB_ENTRY0_PARTITION_LO 23	/*  Data cache partition indicator. */
#define LXTLB_ENTRY0_PARTITION_SZ 2	/*  Data cache partition indicator. */
#define LXTLB_ENTRY0_PARTITION(word)           (((word) & 0x1800000) >> 23)
#define LXTLB_ENTRY0_PARTITION_DEP(base, val)  (((base) & 0xfe7fffff) | (((val) << 23) & (0x1800000)))
#define LXTLB_ENTRY0_RESERVED25_LO 25	/*  Reserved. */
#define LXTLB_ENTRY0_RESERVED25_SZ 7	/*  Reserved. */
#define LXTLB_ENTRY0_RESERVED25(word)           (((word) & 0xfe000000) >> 25)
#define LXTLB_ENTRY0_RESERVED25_DEP(base, val)  (((base) & 0x1ffffff) | (((val) << 25) & (0xfe000000)))
#define LXTLB_ENTRY0_POLICY_UNCACHED 0	/*  Uncached mode. Reads and write that miss the cache are uncached. */
#define LXTLB_ENTRY0_POLICY_CACHED 1	/*  Cached mode. Reads that miss the cache cause the cache to be filled. Writes that hit the cache are written into the cache. Writes that miss the cache are sent to the write buffer. */
#define LXTLB_ENTRY0_POLICY_WCUNCACHED 2	/*  Write combining uncached. Writes that miss the cache are sent to the write buffer. Reads that miss the cache are uncached. */
#define LXTLB_ENTRY0_POLICY_RESERVED3 3	/*  Reserved (On the ST230 reserved cache policies default to uncached). */
#define LXTLB_ENTRY0_POLICY_RESERVED4 4	/*  Reserved (On the ST230 reserved cache policies default to uncached). */
#define LXTLB_ENTRY0_POLICY_RESERVED5 5	/*  Reserved (On the ST230 reserved cache policies default to uncached). */
#define LXTLB_ENTRY0_POLICY_RESERVED6 6	/*  Reserved (On the ST230 reserved cache policies default to uncached). */
#define LXTLB_ENTRY0_POLICY_RESERVED7 7	/*  Reserved (On the ST230 reserved cache policies default to uncached). */
#define LXTLB_ENTRY0_PARTITION_REPLACE 0	/*  Place in the way specified by the replacement counter and increment the counter. */
#define LXTLB_ENTRY0_PARTITION_WAY1 1	/*  Place in the way 1 only. */
#define LXTLB_ENTRY0_PARTITION_WAY2 2	/*  Place in the way 2 only. */
#define LXTLB_ENTRY0_PARTITION_WAY3 3	/*  Place in the way 3 only. */
#define LXTLB_PROT_EXECUTE 1	/*  Execute permission */
#define LXTLB_PROT_READ 2	/*  Read (Prefetch & Purge) permission */
#define LXTLB_PROT_WRITE 4	/*  Write permission */
#define LXTLB_ENTRY0_SIZE_DISABLED 0	/*  Page is disabled. */
#define LXTLB_ENTRY0_SIZE_8K 1	/*  8Kb page. */
#define LXTLB_ENTRY0_SIZE_4MB 2	/*  4MB page. */
#define LXTLB_ENTRY0_SIZE_256MB 3	/*  256MB page. */
#define LXTLB_ENTRY0_SIZE_RESERVED4 4	/*  Reserved (on the ST230 reserved page sizes disable the page). */
#define LXTLB_ENTRY0_SIZE_RESERVED5 5	/*  Reserved (on the ST230 reserved page sizes disable the page). */
#define LXTLB_ENTRY0_SIZE_RESERVED6 6	/*  Reserved (on the ST230 reserved page sizes disable the page). */
#define LXTLB_ENTRY0_SIZE_RESERVED7 7	/*  Reserved (on the ST230 reserved page sizes disable the page). */
#define LXTLB_ENTRY1_VADDR_LO 0	/*  The upper 19 bits of the virtual address. For 4MB pages only the upper 10 bits of this field are significant. For 256MB pages only the upper 4 bits of this field are significant. */
#define LXTLB_ENTRY1_VADDR_SZ 19	/*  The upper 19 bits of the virtual address. For 4MB pages only the upper 10 bits of this field are significant. For 256MB pages only the upper 4 bits of this field are significant. */
#define LXTLB_ENTRY1_VADDR(word)           (((word) & 0x7ffff) >> 0)
#define LXTLB_ENTRY1_VADDR_DEP(base, val)  (((base) & 0xfff80000) | (((val) << 0) & (0x7ffff)))
#define LXTLB_ENTRY1_RESERVED19_LO 19	/*  Reserved. */
#define LXTLB_ENTRY1_RESERVED19_SZ 13	/*  Reserved. */
#define LXTLB_ENTRY1_RESERVED19(word)           (((word) & 0xfff80000) >> 19)
#define LXTLB_ENTRY1_RESERVED19_DEP(base, val)  (((base) & 0x7ffff) | (((val) << 19) & (0xfff80000)))
#define LXTLB_ENTRY2_PADDR_LO 0	/*  The upper 19 bits of the physical address. For 4MB pages only the upper 10 bits of this field are significant. For 256MB pages only the upper 4 bits of this field are significant. */
#define LXTLB_ENTRY2_PADDR_SZ 19	/*  The upper 19 bits of the physical address. For 4MB pages only the upper 10 bits of this field are significant. For 256MB pages only the upper 4 bits of this field are significant. */
#define LXTLB_ENTRY2_PADDR(word)           (((word) & 0x7ffff) >> 0)
#define LXTLB_ENTRY2_PADDR_DEP(base, val)  (((base) & 0xfff80000) | (((val) << 0) & (0x7ffff)))
#define LXTLB_ENTRY2_RESERVED19_LO 19	/*  Reserved. */
#define LXTLB_ENTRY2_RESERVED19_SZ 13	/*  Reserved. */
#define LXTLB_ENTRY2_RESERVED19(word)           (((word) & 0xfff80000) >> 19)
#define LXTLB_ENTRY2_RESERVED19_DEP(base, val)  (((base) & 0x7ffff) | (((val) << 19) & (0xfff80000)))
#define LXTLB_ENTRY3_RESERVED19_LO 0	/*  Reserved. */
#define LXTLB_ENTRY3_RESERVED19_SZ 32	/*  Reserved. */
#define LXTLB_ENTRY3_RESERVED19(word)           (((word) & 0x0) >> 0)
#define LXTLB_ENTRY3_RESERVED19_DEP(base, val)  (((base) & 0xffffffff) | (((val) << 0) & (0x0)))
#define LXTLB_REPLACE_LFSR_LO 0	/*  Random number used to determine which entry to replace next. */
#define LXTLB_REPLACE_LFSR_SZ 16	/*  Random number used to determine which entry to replace next. */
#define LXTLB_REPLACE_LFSR(word)           (((word) & 0xffff) >> 0)
#define LXTLB_REPLACE_LFSR_DEP(base, val)  (((base) & 0xffff0000) | (((val) << 0) & (0xffff)))
#define LXTLB_REPLACE_LIMIT_LO 16	/*  Number of TLB entries that will be replaced. */
#define LXTLB_REPLACE_LIMIT_SZ 8	/*  Number of TLB entries that will be replaced. */
#define LXTLB_REPLACE_LIMIT(word)           (((word) & 0xff0000) >> 16)
#define LXTLB_REPLACE_LIMIT_DEP(base, val)  (((base) & 0xff00ffff) | (((val) << 16) & (0xff0000)))
#define LXTLB_REPLACE_RESERVED24_LO 24	/*  Reserved. */
#define LXTLB_REPLACE_RESERVED24_SZ 8	/*  Reserved. */
#define LXTLB_REPLACE_RESERVED24(word)           (((word) & 0xff000000) >> 24)
#define LXTLB_REPLACE_RESERVED24_DEP(base, val)  (((base) & 0xffffff) | (((val) << 24) & (0xff000000)))
#define LXTLB_ASID_ASID_LO 0	/*  Address space identifier. Indicates the current process ID. */
#define LXTLB_ASID_ASID_SZ 8	/*  Address space identifier. Indicates the current process ID. */
#define LXTLB_ASID_ASID(word)           (((word) & 0xff) >> 0)
#define LXTLB_ASID_ASID_DEP(base, val)  (((base) & 0xffffff00) | (((val) << 0) & (0xff)))
#define LXTLB_ASID_RESERVED_LO 8	/*  Reserved. */
#define LXTLB_ASID_RESERVED_SZ 24	/*  Reserved. */
#define LXTLB_ASID_RESERVED(word)           (((word) & 0xffffff00) >> 8)
#define LXTLB_ASID_RESERVED_DEP(base, val)  (((base) & 0xff) | (((val) << 8) & (0xffffff00)))
#define LXTLB_CONTROL_ITLB_FLUSH_LO 0	/*  Writing a 1 to this bit flushes the entire ITLB. Writing 0 to this bit has no effect. This bit always reads as zero. */
#define LXTLB_CONTROL_ITLB_FLUSH_SZ 1	/*  Writing a 1 to this bit flushes the entire ITLB. Writing 0 to this bit has no effect. This bit always reads as zero. */
#define LXTLB_CONTROL_ITLB_FLUSH(word)           (((word) & 0x1) >> 0)
#define LXTLB_CONTROL_ITLB_FLUSH_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXTLB_CONTROL_DTLB_FLUSH_LO 1	/*  Writing a 1 to this bit flushes the entire DTLB. Writing 0 to this bit has no effect. This bit always reads as zero. */
#define LXTLB_CONTROL_DTLB_FLUSH_SZ 1	/*  Writing a 1 to this bit flushes the entire DTLB. Writing 0 to this bit has no effect. This bit always reads as zero. */
#define LXTLB_CONTROL_DTLB_FLUSH(word)           (((word) & 0x2) >> 1)
#define LXTLB_CONTROL_DTLB_FLUSH_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXTLB_CONTROL_RESERVED1_LO 2	/*  Reserved. */
#define LXTLB_CONTROL_RESERVED1_SZ 30	/*  Reserved. */
#define LXTLB_CONTROL_RESERVED1(word)           (((word) & 0xfffffffc) >> 2)
#define LXTLB_CONTROL_RESERVED1_DEP(base, val)  (((base) & 0x3) | (((val) << 2) & (0xfffffffc)))
#define LXTLB_EXCAUSE_INDEX_LO 0	/*  TLB index of excepting page. */
#define LXTLB_EXCAUSE_INDEX_SZ 8	/*  TLB index of excepting page. */
#define LXTLB_EXCAUSE_INDEX(word)           (((word) & 0xff) >> 0)
#define LXTLB_EXCAUSE_INDEX_DEP(base, val)  (((base) & 0xffffff00) | (((val) << 0) & (0xff)))
#define LXTLB_EXCAUSE_RESERVED8_LO 8	/*  Reserved */
#define LXTLB_EXCAUSE_RESERVED8_SZ 8	/*  Reserved */
#define LXTLB_EXCAUSE_RESERVED8(word)           (((word) & 0xff00) >> 8)
#define LXTLB_EXCAUSE_RESERVED8_DEP(base, val)  (((base) & 0xffff00ff) | (((val) << 8) & (0xff00)))
#define LXTLB_EXCAUSE_CAUSE_LO 16	/*  Cause of current TLB exception. */
#define LXTLB_EXCAUSE_CAUSE_SZ 2	/*  Cause of current TLB exception. */
#define LXTLB_EXCAUSE_CAUSE(word)           (((word) & 0x30000) >> 16)
#define LXTLB_EXCAUSE_CAUSE_DEP(base, val)  (((base) & 0xfffcffff) | (((val) << 16) & (0x30000)))
#define LXTLB_EXCAUSE_SPEC_LO 18	/*  When 1 indicates that this exception was caused by either a purge address, prefetch or speculative load instruction. */
#define LXTLB_EXCAUSE_SPEC_SZ 1	/*  When 1 indicates that this exception was caused by either a purge address, prefetch or speculative load instruction. */
#define LXTLB_EXCAUSE_SPEC(word)           (((word) & 0x40000) >> 18)
#define LXTLB_EXCAUSE_SPEC_DEP(base, val)  (((base) & 0xfffbffff) | (((val) << 18) & (0x40000)))
#define LXTLB_EXCAUSE_WRITE_LO 19	/*  When 1 indicates that this exception was caused by an attempted write to a page (store). When 0 indicates that this exception was caused by an attempted read, prefetch or purge of a page. */
#define LXTLB_EXCAUSE_WRITE_SZ 1	/*  When 1 indicates that this exception was caused by an attempted write to a page (store). When 0 indicates that this exception was caused by an attempted read, prefetch or purge of a page. */
#define LXTLB_EXCAUSE_WRITE(word)           (((word) & 0x80000) >> 19)
#define LXTLB_EXCAUSE_WRITE_DEP(base, val)  (((base) & 0xfff7ffff) | (((val) << 19) & (0x80000)))
#define LXTLB_EXCAUSE_IN_UTLB_LO 20	/*  When 1 the exception address is in the UTLB and the INDEX field is valid. When 0 the exception address was not in the UTLB and the INDEX field is invalid. */
#define LXTLB_EXCAUSE_IN_UTLB_SZ 1	/*  When 1 the exception address is in the UTLB and the INDEX field is valid. When 0 the exception address was not in the UTLB and the INDEX field is invalid. */
#define LXTLB_EXCAUSE_IN_UTLB(word)           (((word) & 0x100000) >> 20)
#define LXTLB_EXCAUSE_IN_UTLB_DEP(base, val)  (((base) & 0xffefffff) | (((val) << 20) & (0x100000)))
#define LXTLB_EXCAUSE_RESERVED21_LO 21	/*  Reserved. */
#define LXTLB_EXCAUSE_RESERVED21_SZ 12	/*  Reserved. */
#define LXTLB_EXCAUSE_RESERVED21(word)           (((word) & 0xffe00000) >> 21)
#define LXTLB_EXCAUSE_RESERVED21_DEP(base, val)  (((base) & 0x1fffff) | (((val) << 21) & (0xffe00000)))
#define LXTLB_EXCAUSE_CAUSE_NO_MAPPING 0	/*  No mapping was found */
#define LXTLB_EXCAUSE_CAUSE_PROT_VIOLATION 1	/*  A protection violation occured. */
#define LXTLB_EXCAUSE_CAUSE_WRITE_TO_CLEAN 2	/*  A write to a clean page. */
#define LXTLB_EXCAUSE_CAUSE_MULTI_MAPPING 3	/*  Multiple mappings were found. */
#define LXSCU_BASE_BASE_LO 0	/*  Upper 19 bits of the base of this region. */
#define LXSCU_BASE_BASE_SZ 19	/*  Upper 19 bits of the base of this region. */
#define LXSCU_BASE_BASE(word)           (((word) & 0x7ffff) >> 0)
#define LXSCU_BASE_BASE_DEP(base, val)  (((base) & 0xfff80000) | (((val) << 0) & (0x7ffff)))
#define LXSCU_BASE_RESERVED19_LO 19	/*  Reserved. */
#define LXSCU_BASE_RESERVED19_SZ 13	/*  Reserved. */
#define LXSCU_BASE_RESERVED19(word)           (((word) & 0xfff80000) >> 19)
#define LXSCU_BASE_RESERVED19_DEP(base, val)  (((base) & 0x7ffff) | (((val) << 19) & (0xfff80000)))
#define LXSCU_LIMIT_LIMIT_LO 0	/*  Upper 19 bits of the limit of this region. */
#define LXSCU_LIMIT_LIMIT_SZ 19	/*  Upper 19 bits of the limit of this region. */
#define LXSCU_LIMIT_LIMIT(word)           (((word) & 0x7ffff) >> 0)
#define LXSCU_LIMIT_LIMIT_DEP(base, val)  (((base) & 0xfff80000) | (((val) << 0) & (0x7ffff)))
#define LXSCU_LIMIT_RESERVED19_LO 19	/*  Reserved. */
#define LXSCU_LIMIT_RESERVED19_SZ 13	/*  Reserved. */
#define LXSCU_LIMIT_RESERVED19(word)           (((word) & 0xfff80000) >> 19)
#define LXSCU_LIMIT_RESERVED19_DEP(base, val)  (((base) & 0x7ffff) | (((val) << 19) & (0xfff80000)))
#define LXTIMECONTROL_ENABLE_LO 0	/*  Enable the timer */
#define LXTIMECONTROL_ENABLE_SZ 1	/*  Enable the timer */
#define LXTIMECONTROL_ENABLE(word)           (((word) & 0x1) >> 0)
#define LXTIMECONTROL_ENABLE_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXTIMECONTROL_INTENABLE_LO 1	/*  Enable the timer interrupt. */
#define LXTIMECONTROL_INTENABLE_SZ 1	/*  Enable the timer interrupt. */
#define LXTIMECONTROL_INTENABLE(word)           (((word) & 0x2) >> 1)
#define LXTIMECONTROL_INTENABLE_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXTIMECONTROL_STATUS_LO 2	/*  Status of the timer interrupt. When 1 a timer has expired. Writing a 0 to this bit has no effect. Writing a 1 to this bit will clear it. */
#define LXTIMECONTROL_STATUS_SZ 1	/*  Status of the timer interrupt. When 1 a timer has expired. Writing a 0 to this bit has no effect. Writing a 1 to this bit will clear it. */
#define LXTIMECONTROL_STATUS(word)           (((word) & 0x4) >> 2)
#define LXTIMECONTROL_STATUS_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXTIMECONTROL_RESERVED_LO 3	/*  Reserved */
#define LXTIMECONTROL_RESERVED_SZ 29	/*  Reserved */
#define LXTIMECONTROL_RESERVED(word)           (((word) & 0xfffffff8) >> 3)
#define LXTIMECONTROL_RESERVED_DEP(base, val)  (((base) & 0x7) | (((val) << 3) & (0xfffffff8)))
#define LXTIMEDIVIDE_DIVIDE_LO 0	/*  Timer divide value, 0 = divide by 1. */
#define LXTIMEDIVIDE_DIVIDE_SZ 16	/*  Timer divide value, 0 = divide by 1. */
#define LXTIMEDIVIDE_DIVIDE(word)           (((word) & 0xffff) >> 0)
#define LXTIMEDIVIDE_DIVIDE_DEP(base, val)  (((base) & 0xffff0000) | (((val) << 0) & (0xffff)))
#define LXTIMEDIVIDE_RESERVED_LO 16	/*  Reserved */
#define LXTIMEDIVIDE_RESERVED_SZ 16	/*  Reserved */
#define LXTIMEDIVIDE_RESERVED(word)           (((word) & 0xffff0000) >> 16)
#define LXTIMEDIVIDE_RESERVED_DEP(base, val)  (((base) & 0xffff) | (((val) << 16) & (0xffff0000)))
#define LXTIMECOUNT_COUNT_LO 0	/*  Current value of timer counter. */
#define LXTIMECOUNT_COUNT_SZ 32	/*  Current value of timer counter. */
#define LXTIMECOUNT_COUNT(word)           (((word) & 0x0) >> 0)
#define LXTIMECOUNT_COUNT_DEP(base, val)  (((base) & 0xffffffff) | (((val) << 0) & (0x0)))
#define LXTIMECOUNT_CONST_LO 0	/*  Value to be reloaded when timer reaches zero. */
#define LXTIMECOUNT_CONST_SZ 32	/*  Value to be reloaded when timer reaches zero. */
#define LXTIMECOUNT_CONST(word)           (((word) & 0x0) >> 0)
#define LXTIMECOUNT_CONST_DEP(base, val)  (((base) & 0xffffffff) | (((val) << 0) & (0x0)))
#define LXINTPENDING0_TIMER0_LO 0	/*  Interrupt is pending from timer 0. */
#define LXINTPENDING0_TIMER0_SZ 1	/*  Interrupt is pending from timer 0. */
#define LXINTPENDING0_TIMER0(word)           (((word) & 0x1) >> 0)
#define LXINTPENDING0_TIMER0_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXINTPENDING0_TIMER1_LO 1	/*  Interrupt is pending from timer 1. */
#define LXINTPENDING0_TIMER1_SZ 1	/*  Interrupt is pending from timer 1. */
#define LXINTPENDING0_TIMER1(word)           (((word) & 0x2) >> 1)
#define LXINTPENDING0_TIMER1_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXINTPENDING0_TIMER2_LO 2	/*  Interrupt is pending from timer 2. */
#define LXINTPENDING0_TIMER2_SZ 1	/*  Interrupt is pending from timer 2. */
#define LXINTPENDING0_TIMER2(word)           (((word) & 0x4) >> 2)
#define LXINTPENDING0_TIMER2_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXINTPENDING0_SYSDEF_LO 3	/*  System defined - refer to data sheet. */
#define LXINTPENDING0_SYSDEF_SZ 29	/*  System defined - refer to data sheet. */
#define LXINTPENDING0_SYSDEF(word)           (((word) & 0xfffffff8) >> 3)
#define LXINTPENDING0_SYSDEF_DEP(base, val)  (((base) & 0x7) | (((val) << 3) & (0xfffffff8)))
#define LXINTPENDING1_SYSDEF_LO 0	/*  System defined - refer to data sheet. */
#define LXINTPENDING1_SYSDEF_SZ 32	/*  System defined - refer to data sheet. */
#define LXINTPENDING1_SYSDEF(word)           (((word) & 0x0) >> 0)
#define LXINTPENDING1_SYSDEF_DEP(base, val)  (((base) & 0xffffffff) | (((val) << 0) & (0x0)))
#define LXINTMASK0_TIMER0_LO 0	/*  Mask bit for timer 0 */
#define LXINTMASK0_TIMER0_SZ 1	/*  Mask bit for timer 0 */
#define LXINTMASK0_TIMER0(word)           (((word) & 0x1) >> 0)
#define LXINTMASK0_TIMER0_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXINTMASK0_TIMER1_LO 1	/*  Mask bit for timer 1 */
#define LXINTMASK0_TIMER1_SZ 1	/*  Mask bit for timer 1 */
#define LXINTMASK0_TIMER1(word)           (((word) & 0x2) >> 1)
#define LXINTMASK0_TIMER1_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXINTMASK0_TIMER2_LO 2	/*  Mask bit for timer 2 */
#define LXINTMASK0_TIMER2_SZ 1	/*  Mask bit for timer 2 */
#define LXINTMASK0_TIMER2(word)           (((word) & 0x4) >> 2)
#define LXINTMASK0_TIMER2_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXINTMASK0_SYSDEF_LO 3	/*  Mask bits for system defined interrupts - refer to data sheet. */
#define LXINTMASK0_SYSDEF_SZ 29	/*  Mask bits for system defined interrupts - refer to data sheet. */
#define LXINTMASK0_SYSDEF(word)           (((word) & 0xfffffff8) >> 3)
#define LXINTMASK0_SYSDEF_DEP(base, val)  (((base) & 0x7) | (((val) << 3) & (0xfffffff8)))
#define LXINTMASK1_SYSDEF_LO 0	/*  Mask bits for system defined interrupts - refer to data sheet. */
#define LXINTMASK1_SYSDEF_SZ 32	/*  Mask bits for system defined interrupts - refer to data sheet. */
#define LXINTMASK1_SYSDEF(word)           (((word) & 0x0) >> 0)
#define LXINTMASK1_SYSDEF_DEP(base, val)  (((base) & 0xffffffff) | (((val) << 0) & (0x0)))
#define LXINTMASKCLR0_TIMER0_LO 0	/*  Mask clear bit for timer 0 */
#define LXINTMASKCLR0_TIMER0_SZ 1	/*  Mask clear bit for timer 0 */
#define LXINTMASKCLR0_TIMER0(word)           (((word) & 0x1) >> 0)
#define LXINTMASKCLR0_TIMER0_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXINTMASKCLR0_TIMER1_LO 1	/*  Mask clear bit for timer 1 */
#define LXINTMASKCLR0_TIMER1_SZ 1	/*  Mask clear bit for timer 1 */
#define LXINTMASKCLR0_TIMER1(word)           (((word) & 0x2) >> 1)
#define LXINTMASKCLR0_TIMER1_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXINTMASKCLR0_TIMER2_LO 2	/*  Mask clear bit for timer 2 */
#define LXINTMASKCLR0_TIMER2_SZ 1	/*  Mask clear bit for timer 2 */
#define LXINTMASKCLR0_TIMER2(word)           (((word) & 0x4) >> 2)
#define LXINTMASKCLR0_TIMER2_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXINTMASKCLR0_SYSDEF_LO 3	/*  Mask clear bits for system defined interrupts - refer to data sheet. */
#define LXINTMASKCLR0_SYSDEF_SZ 29	/*  Mask clear bits for system defined interrupts - refer to data sheet. */
#define LXINTMASKCLR0_SYSDEF(word)           (((word) & 0xfffffff8) >> 3)
#define LXINTMASKCLR0_SYSDEF_DEP(base, val)  (((base) & 0x7) | (((val) << 3) & (0xfffffff8)))
#define LXINTMASKCLR1_SYSDEF_LO 0	/*  Mask clear bits for system defined interrupts - refer to data sheet. */
#define LXINTMASKCLR1_SYSDEF_SZ 32	/*  Mask clear bits for system defined interrupts - refer to data sheet. */
#define LXINTMASKCLR1_SYSDEF(word)           (((word) & 0x0) >> 0)
#define LXINTMASKCLR1_SYSDEF_DEP(base, val)  (((base) & 0xffffffff) | (((val) << 0) & (0x0)))
#define LXINTMASKSET0_TIMER0_LO 0	/*  Mask set bit for timer 0 */
#define LXINTMASKSET0_TIMER0_SZ 1	/*  Mask set bit for timer 0 */
#define LXINTMASKSET0_TIMER0(word)           (((word) & 0x1) >> 0)
#define LXINTMASKSET0_TIMER0_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXINTMASKSET0_TIMER1_LO 1	/*  Mask set bit for timer 1 */
#define LXINTMASKSET0_TIMER1_SZ 1	/*  Mask set bit for timer 1 */
#define LXINTMASKSET0_TIMER1(word)           (((word) & 0x2) >> 1)
#define LXINTMASKSET0_TIMER1_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXINTMASKSET0_TIMER2_LO 2	/*  Mask set bit for timer 2 */
#define LXINTMASKSET0_TIMER2_SZ 1	/*  Mask set bit for timer 2 */
#define LXINTMASKSET0_TIMER2(word)           (((word) & 0x4) >> 2)
#define LXINTMASKSET0_TIMER2_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXINTMASKSET0_SYSDEF_LO 3	/*  Mask set bits for system defined interrupts - refer to data sheet. */
#define LXINTMASKSET0_SYSDEF_SZ 29	/*  Mask set bits for system defined interrupts - refer to data sheet. */
#define LXINTMASKSET0_SYSDEF(word)           (((word) & 0xfffffff8) >> 3)
#define LXINTMASKSET0_SYSDEF_DEP(base, val)  (((base) & 0x7) | (((val) << 3) & (0xfffffff8)))
#define LXINTMASKSET1_SYSDEF_LO 0	/*  Mask set bits for system defined interrupts - refer to data sheet. */
#define LXINTMASKSET1_SYSDEF_SZ 32	/*  Mask set bits for system defined interrupts - refer to data sheet. */
#define LXINTMASKSET1_SYSDEF(word)           (((word) & 0x0) >> 0)
#define LXINTMASKSET1_SYSDEF_DEP(base, val)  (((base) & 0xffffffff) | (((val) << 0) & (0x0)))
#define LXINTTEST0_TIMER0_LO 0	/*  Interrupt test bit for timer 0 */
#define LXINTTEST0_TIMER0_SZ 1	/*  Interrupt test bit for timer 0 */
#define LXINTTEST0_TIMER0(word)           (((word) & 0x1) >> 0)
#define LXINTTEST0_TIMER0_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXINTTEST0_TIMER1_LO 1	/*  Interrupt test bit for timer 1 */
#define LXINTTEST0_TIMER1_SZ 1	/*  Interrupt test bit for timer 1 */
#define LXINTTEST0_TIMER1(word)           (((word) & 0x2) >> 1)
#define LXINTTEST0_TIMER1_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXINTTEST0_TIMER2_LO 2	/*  Interrupt test bit for timer 2 */
#define LXINTTEST0_TIMER2_SZ 1	/*  Interrupt test bit for timer 2 */
#define LXINTTEST0_TIMER2(word)           (((word) & 0x4) >> 2)
#define LXINTTEST0_TIMER2_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXINTTEST0_SYSDEF_LO 3	/*  Interrupt test bits for system defined interrupts - refer to data sheet. */
#define LXINTTEST0_SYSDEF_SZ 29	/*  Interrupt test bits for system defined interrupts - refer to data sheet. */
#define LXINTTEST0_SYSDEF(word)           (((word) & 0xfffffff8) >> 3)
#define LXINTTEST0_SYSDEF_DEP(base, val)  (((base) & 0x7) | (((val) << 3) & (0xfffffff8)))
#define LXINTTEST1_SYSDEF_LO 0	/*  Interrupt test bits for system defined interrupts - refer to data sheet. */
#define LXINTTEST1_SYSDEF_SZ 32	/*  Interrupt test bits for system defined interrupts - refer to data sheet. */
#define LXINTTEST1_SYSDEF(word)           (((word) & 0x0) >> 0)
#define LXINTTEST1_SYSDEF_DEP(base, val)  (((base) & 0xffffffff) | (((val) << 0) & (0x0)))
#define LXINTCLR0_TIMER0_LO 0	/*  Interrupt clear bit for timer 0 */
#define LXINTCLR0_TIMER0_SZ 1	/*  Interrupt clear bit for timer 0 */
#define LXINTCLR0_TIMER0(word)           (((word) & 0x1) >> 0)
#define LXINTCLR0_TIMER0_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXINTCLR0_TIMER1_LO 1	/*  Interrupt clear bit for timer 1 */
#define LXINTCLR0_TIMER1_SZ 1	/*  Interrupt clear bit for timer 1 */
#define LXINTCLR0_TIMER1(word)           (((word) & 0x2) >> 1)
#define LXINTCLR0_TIMER1_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXINTCLR0_TIMER2_LO 2	/*  Interrupt clear bit for timer 2 */
#define LXINTCLR0_TIMER2_SZ 1	/*  Interrupt clear bit for timer 2 */
#define LXINTCLR0_TIMER2(word)           (((word) & 0x4) >> 2)
#define LXINTCLR0_TIMER2_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXINTCLR0_SYSDEF_LO 3	/*  Interrupt clear bits for system defined interrupts - refer to data sheet. */
#define LXINTCLR0_SYSDEF_SZ 29	/*  Interrupt clear bits for system defined interrupts - refer to data sheet. */
#define LXINTCLR0_SYSDEF(word)           (((word) & 0xfffffff8) >> 3)
#define LXINTCLR0_SYSDEF_DEP(base, val)  (((base) & 0x7) | (((val) << 3) & (0xfffffff8)))
#define LXINTCLR1_SYSDEF_LO 0	/*  Interrupt clear bits for system defined interrupts - refer to data sheet. */
#define LXINTCLR1_SYSDEF_SZ 32	/*  Interrupt clear bits for system defined interrupts - refer to data sheet. */
#define LXINTCLR1_SYSDEF(word)           (((word) & 0x0) >> 0)
#define LXINTCLR1_SYSDEF_DEP(base, val)  (((base) & 0xffffffff) | (((val) << 0) & (0x0)))
#define LXINTSET0_TIMER0_LO 0	/*  Interrupt set bit for timer 0 */
#define LXINTSET0_TIMER0_SZ 1	/*  Interrupt set bit for timer 0 */
#define LXINTSET0_TIMER0(word)           (((word) & 0x1) >> 0)
#define LXINTSET0_TIMER0_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXINTSET0_TIMER1_LO 1	/*  Interrupt set bit for timer 1 */
#define LXINTSET0_TIMER1_SZ 1	/*  Interrupt set bit for timer 1 */
#define LXINTSET0_TIMER1(word)           (((word) & 0x2) >> 1)
#define LXINTSET0_TIMER1_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXINTSET0_TIMER2_LO 2	/*  Interrupt set bit for timer 2 */
#define LXINTSET0_TIMER2_SZ 1	/*  Interrupt set bit for timer 2 */
#define LXINTSET0_TIMER2(word)           (((word) & 0x4) >> 2)
#define LXINTSET0_TIMER2_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXINTSET0_SYSDEF_LO 3	/*  Interrupt set bits for system defined interrupts - refer to data sheet. */
#define LXINTSET0_SYSDEF_SZ 29	/*  Interrupt set bits for system defined interrupts - refer to data sheet. */
#define LXINTSET0_SYSDEF(word)           (((word) & 0xfffffff8) >> 3)
#define LXINTSET0_SYSDEF_DEP(base, val)  (((base) & 0x7) | (((val) << 3) & (0xfffffff8)))
#define LXINTSET1_SYSDEF_LO 0	/*  Interrupt set bits for system defined interrupts - refer to data sheet. */
#define LXINTSET1_SYSDEF_SZ 32	/*  Interrupt set bits for system defined interrupts - refer to data sheet. */
#define LXINTSET1_SYSDEF(word)           (((word) & 0x0) >> 0)
#define LXINTSET1_SYSDEF_DEP(base, val)  (((base) & 0xffffffff) | (((val) << 0) & (0x0)))
#define PM_EVENT_DHIT 0		/*  The number of load and stores that hit the cache. This includes uncached accesses that hit the cache. */
#define PM_EVENT_DMISS 1	/*  The number of load and stores that miss the cache. This includes stores that miss the cache and are send to the write buffer. Uncached accesses are not included in this count. */
#define PM_EVENT_DMISSCYCLES 2	/*  The number of cycles the core is stalled due to the data cache being busy (this includes DTLB related stalls). */
#define PM_EVENT_PFTISSUED 3	/*  The number of prefetches that are sent to the bus. */
#define PM_EVENT_PFTHITS 4	/*  The number of cached loads that hit the prefetch buffer. */
#define PM_EVENT_WBHITS 5	/*  The number of cached writes that hit the write buffer. */
#define PM_EVENT_IHIT 6		/*  The number of accesses the instruction buffer made that hit the instruction cache. */
#define PM_EVENT_IMISS 7	/*  The number of accesses the instruction buffer made that missed the instruction cache. */
#define PM_EVENT_IMISSCYCLES 8	/*  The number of cycles the instruction cache was stalled for (this includes ITLB related stalls). */
#define PM_EVENT_IBUFINVALID 9	/*  Duration where IBuffer is not able to issue bundles to the pipeline. */
#define PM_EVENT_BUNDLES 10	/*  Bundles executed. */
#define PM_EVENT_LDST 11	/*  Load/Store instructions executed. These inlcude: stw, sth, stb, pft, prgadd, prgset, prginspg, pswset, pswclr, sync, ldb, ldb.d, ldbu, ldbu.d, ldh, ldh.d, ldhu, ldhu.d, ldw, ldw.d. */
#define PM_EVENT_TAKENBR 12	/*  Number of taken branches, includes br, brf, rfi, goto and call. */
#define PM_EVENT_NOTTAKENBR 13	/*  Number of not taken branches (br and brf). */
#define PM_EVENT_EXCEPTIONS 14	/*  Number of exceptions and Debug interrupts. */
#define PM_EVENT_INTERRUPTS 15	/*  Number of interrupts. */
#define PM_EVENT_BUSREADS 16	/*  Number of architectural read transactions issued to the bus. This is the number of uncached reads, I & D cache refills and prefetches issued to the bus. */
#define PM_EVENT_BUSWRITES 17	/*  Number of architectural write transactions issued to the bus. This is the number of write buffer lines evicted and the number of uncached writes issued to the bus. */
#define PM_EVENT_OPERATIONS 18	/*  Number of completed operations. Includes nops in the instruction stream but not those added dynamically. This counter excludes long immediates. */
#define PM_EVENT_WBMISSES 19	/*  Number of cached writes that missed the cache and misses the write buffer. This excludes cache line evictions. */
#define PM_EVENT_NOPBUNDLES 20	/*  Number of completed bundles that were empty or contained only nops. This includes nop bundles generated by instruction buffer stalls and interlocking stalls. It excludes pipeline stalls due to load/stores and control register/sdi accesses. */
#define PM_EVENT_LONGIMM 21	/*  Number of long immediates in completed bundles. */
#define PM_EVENT_ITLBMISS 22	/*  The number of instruction cache fills that missed the ITLB. */
#define PM_EVENT_DTLBMISS 23	/*  The number of load/store operations that missed the DTLB when the TLB is enabled. */
#define PM_EVENT_UTLBHIT 24	/*  The number of accesses to the UTLB which were hits. */
#define PM_EVENT_ITLBWAITCYCLES 25	/*  Number of cycles the instruction cache spends waiting for the ITLB to fill. */
#define PM_EVENT_DTLBWAITCYCLES 26	/*  Number of cycles the data cache spends waiting for the ITLB to fill. */
#define PM_EVENT_UTLBARBITRATIONCYCLES 27	/*  Number of cycles where the ITLB or DTLB was waiting for access to the UTLB because the UTLB was busy servicing a request. */
#define PM_EVENT_RESERVED28_LO 28	/*  Reserved for future use (on the ST230, counting reserved events has no effect, the counter will not increment). */
#define PM_EVENT_RESERVED28_SZ 4	/*  Reserved for future use (on the ST230, counting reserved events has no effect, the counter will not increment). */
#define LXPM_CR_ENB_LO 0	/*  When 1, counting is enabled. When 0 counting is disabled. */
#define LXPM_CR_ENB_SZ 1	/*  When 1, counting is enabled. When 0 counting is disabled. */
#define LXPM_CR_ENB(word)           (((word) & 0x1) >> 0)
#define LXPM_CR_ENB_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXPM_CR_RST_LO 1	/*  When a 1 is written all the counters (PM_CNT0-3 and PM_PCLK) are set to zero. If a 0 is written it is ignored. This field does not retain its value and so always reads as 0. */
#define LXPM_CR_RST_SZ 1	/*  When a 1 is written all the counters (PM_CNT0-3 and PM_PCLK) are set to zero. If a 0 is written it is ignored. This field does not retain its value and so always reads as 0. */
#define LXPM_CR_RST(word)           (((word) & 0x2) >> 1)
#define LXPM_CR_RST_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXPM_CR_IDLE_LO 2	/*  When the core enters idle mode this bit is set to 1. Writing a 0 to this bit has no effect. Writing a 1 to this bit will clear the bit. */
#define LXPM_CR_IDLE_SZ 1	/*  When the core enters idle mode this bit is set to 1. Writing a 0 to this bit has no effect. Writing a 1 to this bit will clear the bit. */
#define LXPM_CR_IDLE(word)           (((word) & 0x4) >> 2)
#define LXPM_CR_IDLE_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXPM_CR_RESERVED3_LO 3	/*  Reserved */
#define LXPM_CR_RESERVED3_SZ 9	/*  Reserved */
#define LXPM_CR_RESERVED3(word)           (((word) & 0xff8) >> 3)
#define LXPM_CR_RESERVED3_DEP(base, val)  (((base) & 0xfffff007) | (((val) << 3) & (0xff8)))
#define LXPM_CR_EVENT0_LO 12	/*  5-bit field specifying the event being monitored for this counter. */
#define LXPM_CR_EVENT0_SZ 5	/*  5-bit field specifying the event being monitored for this counter. */
#define LXPM_CR_EVENT0(word)           (((word) & 0x1f000) >> 12)
#define LXPM_CR_EVENT0_DEP(base, val)  (((base) & 0xfffe0fff) | (((val) << 12) & (0x1f000)))
#define LXPM_CR_EVENT1_LO 17	/*  5-bit field specifying the event being monitored for this counter. */
#define LXPM_CR_EVENT1_SZ 5	/*  5-bit field specifying the event being monitored for this counter. */
#define LXPM_CR_EVENT1(word)           (((word) & 0x3e0000) >> 17)
#define LXPM_CR_EVENT1_DEP(base, val)  (((base) & 0xffc1ffff) | (((val) << 17) & (0x3e0000)))
#define LXPM_CR_EVENT2_LO 22	/*  5-bit field specifying the event being monitored for this counter. */
#define LXPM_CR_EVENT2_SZ 5	/*  5-bit field specifying the event being monitored for this counter. */
#define LXPM_CR_EVENT2(word)           (((word) & 0x7c00000) >> 22)
#define LXPM_CR_EVENT2_DEP(base, val)  (((base) & 0xf83fffff) | (((val) << 22) & (0x7c00000)))
#define LXPM_CR_EVENT3_LO 27	/*  5-bit field specifying the event being monitored for this counter. */
#define LXPM_CR_EVENT3_SZ 5	/*  5-bit field specifying the event being monitored for this counter. */
#define LXPM_CR_EVENT3(word)           (((word) & 0xf8000000) >> 27)
#define LXPM_CR_EVENT3_DEP(base, val)  (((base) & 0x7ffffff) | (((val) << 27) & (0xf8000000)))
#define DSR_REG0 0		/*  DSU version register, contains version number for DSU, core and chip. */
#define DSR_REG1 1		/*  DSU status register, contains DSU control and status bits. */
#define DSR_REG2 2		/*  DSU output register, supports message transfer from target to HTI. */
#define DSR_REGGEN_LO 3		/*  General purpose registers. */
#define DSR_REGGEN_SZ 28	/*  General purpose registers. */
#define LXDSR1_DEBUG_INTERRUPT_TAKEN_LO 0	/*  Value of DEBUG_INTERRUPT_TAKEN signal, active high. */
#define LXDSR1_DEBUG_INTERRUPT_TAKEN_SZ 1	/*  Value of DEBUG_INTERRUPT_TAKEN signal, active high. */
#define LXDSR1_DEBUG_INTERRUPT_TAKEN(word)           (((word) & 0x1) >> 0)
#define LXDSR1_DEBUG_INTERRUPT_TAKEN_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define LXDSR1_SUPERVISOR_WRITE_ENABLE_LO 1	/*  STBus writes enabled if the core is in supervisor mode (regardless of debug mode). */
#define LXDSR1_SUPERVISOR_WRITE_ENABLE_SZ 1	/*  STBus writes enabled if the core is in supervisor mode (regardless of debug mode). */
#define LXDSR1_SUPERVISOR_WRITE_ENABLE(word)           (((word) & 0x2) >> 1)
#define LXDSR1_SUPERVISOR_WRITE_ENABLE_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define LXDSR1_USER_WRITE_ENABLE_LO 2	/*  STBus writes enabled if the core is in user mode (regardless of debug mode). */
#define LXDSR1_USER_WRITE_ENABLE_SZ 1	/*  STBus writes enabled if the core is in user mode (regardless of debug mode). */
#define LXDSR1_USER_WRITE_ENABLE(word)           (((word) & 0x4) >> 2)
#define LXDSR1_USER_WRITE_ENABLE_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXDSR1_BIGENDIAN_LO 3	/*  When 1 the core is in big endian mode. When 0 the core is in little endian. */
#define LXDSR1_BIGENDIAN_SZ 1	/*  When 1 the core is in big endian mode. When 0 the core is in little endian. */
#define LXDSR1_BIGENDIAN(word)           (((word) & 0x8) >> 3)
#define LXDSR1_BIGENDIAN_DEP(base, val)  (((base) & 0xfffffff7) | (((val) << 3) & (0x8)))
#define LXDSR1_RESERVED4_LO 4	/*  Reserved */
#define LXDSR1_RESERVED4_SZ 1	/*  Reserved */
#define LXDSR1_RESERVED4(word)           (((word) & 0x10) >> 4)
#define LXDSR1_RESERVED4_DEP(base, val)  (((base) & 0xffffffef) | (((val) << 4) & (0x10)))
#define LXDSR1_OUTPUT_PENDING_LO 5	/*  DSR2 contains a byte to be sent to the HTI which has not yet been sent. */
#define LXDSR1_OUTPUT_PENDING_SZ 1	/*  DSR2 contains a byte to be sent to the HTI which has not yet been sent. */
#define LXDSR1_OUTPUT_PENDING(word)           (((word) & 0x20) >> 5)
#define LXDSR1_OUTPUT_PENDING_DEP(base, val)  (((base) & 0xffffffdf) | (((val) << 5) & (0x20)))
#define LXDSR1_TRIGGER_IN_LO 6	/*  Current value of the trigger in pin. */
#define LXDSR1_TRIGGER_IN_SZ 1	/*  Current value of the trigger in pin. */
#define LXDSR1_TRIGGER_IN(word)           (((word) & 0x40) >> 6)
#define LXDSR1_TRIGGER_IN_DEP(base, val)  (((base) & 0xffffffbf) | (((val) << 6) & (0x40)))
#define LXDSR1_TRIGGER_OUT_LO 7	/*  Current value of the trigger out pin. */
#define LXDSR1_TRIGGER_OUT_SZ 1	/*  Current value of the trigger out pin. */
#define LXDSR1_TRIGGER_OUT(word)           (((word) & 0x80) >> 7)
#define LXDSR1_TRIGGER_OUT_DEP(base, val)  (((base) & 0xffffff7f) | (((val) << 7) & (0x80)))
#define LXDSR1_TRIGGER_ENABLE_LO 8	/*  Enables/disables debug interrupts on trigger in. */
#define LXDSR1_TRIGGER_ENABLE_SZ 1	/*  Enables/disables debug interrupts on trigger in. */
#define LXDSR1_TRIGGER_ENABLE(word)           (((word) & 0x100) >> 8)
#define LXDSR1_TRIGGER_ENABLE_DEP(base, val)  (((base) & 0xfffffeff) | (((val) << 8) & (0x100)))
#define LXDSR1_RESERVED9_LO 9	/*  Reserved */
#define LXDSR1_RESERVED9_SZ 7	/*  Reserved */
#define LXDSR1_RESERVED9(word)           (((word) & 0xfe00) >> 9)
#define LXDSR1_RESERVED9_DEP(base, val)  (((base) & 0xffff01ff) | (((val) << 9) & (0xfe00)))
#define LXDSR1_SW_FLAGS_LO 16	/*  Reserved for future software use. */
#define LXDSR1_SW_FLAGS_SZ 16	/*  Reserved for future software use. */
#define LXDSR1_SW_FLAGS(word)           (((word) & 0xffff0000) >> 16)
#define LXDSR1_SW_FLAGS_DEP(base, val)  (((base) & 0xffff) | (((val) << 16) & (0xffff0000)))
#define LXDSR2_DATA_LO 0	/*  Output data. */
#define LXDSR2_DATA_SZ 8	/*  Output data. */
#define LXDSR2_DATA(word)           (((word) & 0xff) >> 0)
#define LXDSR2_DATA_DEP(base, val)  (((base) & 0xffffff00) | (((val) << 0) & (0xff)))
#define LXDSR2_RESERVED8_LO 8	/*  Always zero. */
#define LXDSR2_RESERVED8_SZ 24	/*  Always zero. */
#define LXDSR2_RESERVED8(word)           (((word) & 0xffffff00) >> 8)
#define LXDSR2_RESERVED8_DEP(base, val)  (((base) & 0xff) | (((val) << 8) & (0xffffff00)))
#define LXSDI_CONTROL_PRIV_NOUSER 0	/*  Access only allowed in supervisor mode */
#define LXSDI_CONTROL_PRIV_USER 1	/*  Allow user access to data and ready register */
#define LXSDI_CONTROL_PRIV_RESERVED2 2	/*  Reserved (defaults to priviledge no user). */
#define LXSDI_CONTROL_PRIV_RESERVED3 3	/*  Reserved (defaults to priviledge no user). */
#define LXSDI_CONTROL_PRIV_LO 0	/*  Privilege bits. */
#define LXSDI_CONTROL_PRIV_SZ 2	/*  Privilege bits. */
#define LXSDI_CONTROL_PRIV(word)           (((word) & 0x3) >> 0)
#define LXSDI_CONTROL_PRIV_DEP(base, val)  (((base) & 0xfffffffc) | (((val) << 0) & (0x3)))
#define LXSDI_CONTROL_RESETINPUT_LO 2	/*  RESETINPUT (Read Only) acts as RESETREQUEST when slave, RESETACK when master. */
#define LXSDI_CONTROL_RESETINPUT_SZ 1	/*  RESETINPUT (Read Only) acts as RESETREQUEST when slave, RESETACK when master. */
#define LXSDI_CONTROL_RESETINPUT(word)           (((word) & 0x4) >> 2)
#define LXSDI_CONTROL_RESETINPUT_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define LXSDI_CONTROL_RESETOUTPUT_LO 3	/*  RESETOUTPUT, acts as RESETREQUEST when master, RESETACK when slave. */
#define LXSDI_CONTROL_RESETOUTPUT_SZ 1	/*  RESETOUTPUT, acts as RESETREQUEST when master, RESETACK when slave. */
#define LXSDI_CONTROL_RESETOUTPUT(word)           (((word) & 0x8) >> 3)
#define LXSDI_CONTROL_RESETOUTPUT_DEP(base, val)  (((base) & 0xfffffff7) | (((val) << 3) & (0x8)))
#define LXSDI_CONTROL_INPUTNOTOUTPUT_LO 4	/*  INPUTNOTOUTPUT (Read only). */
#define LXSDI_CONTROL_INPUTNOTOUTPUT_SZ 1	/*  INPUTNOTOUTPUT (Read only). */
#define LXSDI_CONTROL_INPUTNOTOUTPUT(word)           (((word) & 0x10) >> 4)
#define LXSDI_CONTROL_INPUTNOTOUTPUT_DEP(base, val)  (((base) & 0xffffffef) | (((val) << 4) & (0x10)))
#define LXSDI_CONTROL_RESERVED0_LO 5	/*  Reserved */
#define LXSDI_CONTROL_RESERVED0_SZ 1	/*  Reserved */
#define LXSDI_CONTROL_RESERVED0(word)           (((word) & 0x20) >> 5)
#define LXSDI_CONTROL_RESERVED0_DEP(base, val)  (((base) & 0xffffffdf) | (((val) << 5) & (0x20)))
#define LXSDI_CONTROL_MASTERNOTSLAVE_LO 6	/*  MASTERNOTSLAVE (Read only). */
#define LXSDI_CONTROL_MASTERNOTSLAVE_SZ 1	/*  MASTERNOTSLAVE (Read only). */
#define LXSDI_CONTROL_MASTERNOTSLAVE(word)           (((word) & 0x40) >> 6)
#define LXSDI_CONTROL_MASTERNOTSLAVE_DEP(base, val)  (((base) & 0xffffffbf) | (((val) << 6) & (0x40)))
#define LXSDI_CONTROL_TIMEOUTENABLE_LO 7	/*  Timeout Disable (set to 1 to disable timeout interrupts). */
#define LXSDI_CONTROL_TIMEOUTENABLE_SZ 1	/*  Timeout Disable (set to 1 to disable timeout interrupts). */
#define LXSDI_CONTROL_TIMEOUTENABLE(word)           (((word) & 0x80) >> 7)
#define LXSDI_CONTROL_TIMEOUTENABLE_DEP(base, val)  (((base) & 0xffffff7f) | (((val) << 7) & (0x80)))
#define LXSDI_CONTROL_RESERVED1_LO 8	/*  Reserved */
#define LXSDI_CONTROL_RESERVED1_SZ 24	/*  Reserved */
#define LXSDI_CONTROL_RESERVED1(word)           (((word) & 0xffffff00) >> 8)
#define LXSDI_CONTROL_RESERVED1_DEP(base, val)  (((base) & 0xff) | (((val) << 8) & (0xffffff00)))
#define BRK_IN_RANGE_LO 0	/*  Break if address >= lower && address <=upper. */
#define BRK_IN_RANGE_SZ 1	/*  Break if address >= lower && address <=upper. */
#define BRK_IN_RANGE(word)           (((word) & 0x1) >> 0)
#define BRK_IN_RANGE_DEP(base, val)  (((base) & 0xfffffffe) | (((val) << 0) & (0x1)))
#define BRK_OUT_RANGE_LO 1	/*  Break if address < lower || address > upper. */
#define BRK_OUT_RANGE_SZ 1	/*  Break if address < lower || address > upper. */
#define BRK_OUT_RANGE(word)           (((word) & 0x2) >> 1)
#define BRK_OUT_RANGE_DEP(base, val)  (((base) & 0xfffffffd) | (((val) << 1) & (0x2)))
#define BRK_EITHER_LO 2		/*  Break if address == lower || address == upper. */
#define BRK_EITHER_SZ 1		/*  Break if address == lower || address == upper. */
#define BRK_EITHER(word)           (((word) & 0x4) >> 2)
#define BRK_EITHER_DEP(base, val)  (((base) & 0xfffffffb) | (((val) << 2) & (0x4)))
#define BRK_MASKED_LO 3		/*  Break if address & upper == lower. */
#define BRK_MASKED_SZ 1		/*  Break if address & upper == lower. */
#define BRK_MASKED(word)           (((word) & 0x8) >> 3)
#define BRK_MASKED_DEP(base, val)  (((base) & 0xfffffff7) | (((val) << 3) & (0x8)))
#define BRK_RESERVED4_LO 4	/*  Reserved */
#define BRK_RESERVED4_SZ 28	/*  Reserved */
#define BRK_RESERVED4(word)           (((word) & 0xfffffff0) >> 4)
#define BRK_RESERVED4_DEP(base, val)  (((base) & 0xf) | (((val) << 4) & (0xfffffff0)))
#endif /*  _ctrlreg_H_ */
