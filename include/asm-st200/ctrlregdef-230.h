/*
 * include/asm-st200/ctrlregdef-230.h
 *
 * Copyright (C) 2002 STMicroelectronics Limited
 *	Author: Stuart Menefy <stuart.menefy@st.com>
 *
 * Control register defintions for ST230
 */

#ifndef _ASM_ST200_CTRLREGDEFS_230_H
#define _ASM_ST200_CTRLREGDEFS_230_H

/* Exception cause as an integer, indicating the cause of the last trap. */
#define EXCAUSENO	CTRL_ADDR(0xff88)


/* TLB related control registers */
#define TLB_INDEX	CTRL_ADDR(0xff80)	/* Index of the TLB entry pointed to by TLB_ENTRYx */
#define TLB_ENTRY0	CTRL_ADDR(0xff78)	/* TLB entry bits  [31:00] */
#define TLB_ENTRY1	CTRL_ADDR(0xff70)	/* TLB entry bits  [63:32] */
#define TLB_ENTRY2	CTRL_ADDR(0xff68)	/* TLB entry bits  [95:64] */
#define TLB_ENTRY3	CTRL_ADDR(0xff60)	/* TLB entry bits [127:96] */
#define TLB_EXCAUSE	CTRL_ADDR(0xff58)	/* Cause of the TLB related exception */
#define TLB_CONTROL	CTRL_ADDR(0xff50)	/* Control bits for TLB */
#define TLB_REPLACE	CTRL_ADDR(0xff48)	/* Replacement pointer */
#define TLB_ASID	CTRL_ADDR(0xff40)	/* Current address space ID */

#define SCU_BASE0	CTRL_ADDR(0xd000)	/* SCU base 0. */
#define SCU_LIMIT0	CTRL_ADDR(0xd008)	/* SCU limit 0. */
#define SCU_BASE1	CTRL_ADDR(0xd010)	/* SCU base 1. */
#define SCU_LIMIT1	CTRL_ADDR(0xd018)	/* SCU limit 1. */
#define SCU_BASE2	CTRL_ADDR(0xd020)	/* SCU base 2. */
#define SCU_LIMIT2	CTRL_ADDR(0xd028)	/* SCU limit 2. */
#define SCU_BASE3	CTRL_ADDR(0xd030)	/* SCU base 3. */
#define SCU_LIMIT3	CTRL_ADDR(0xd038)	/* SCU limit 3. */
#define SCU_BASE(n)     CTRL_ADDR(0xd000+(0x10*(n)))	/* SCU base n */
#define SCU_LIMIT(n)    CTRL_ADDR(0xd008+(0x10*(n)))	/* SCU limit n */
#define SCU_NUM_ENTRIES 4

#define SCU_SHIFT	13

/************ PSW bit definitions ************/
#define PSW_USER_MODE			(1<< 0)	/* When 1 the core is in user mode, otherwise supervisor mode. */
#define PSW_INT_ENABLE			(1<< 1)	/* When 1 external interrupts are enabled. */
#define PSW_TLB_ENABLE			(1<< 2)	/* When 1 address translation is enabled. */
#define PSW_TLB_DYNAMIC			(1<< 3)	/* When 1 purges and speculative loads which miss the TLB cause TLB_NO_MAPPING exceptions. */
#define PSW_SPECLOAD_MALIGNTRAP_EN	(1<< 4)	/* When 1 enables exceptions on speculative load misalignment errors. */
#define PSW_DBREAK_ENABLE		(1<< 8)	/* When 1 data breakpoints are enabled. */
#define PSW_IBREAK_ENABLE		(1<< 9)	/* When 1 instruction breakpoints are enabled. */
#define PSW_DEBUG_MODE			(1<<12)	/* When 1 the core is in debug mode. */

/* These are the default things we always stick in the PSW, separated out for
 * user and kernel. Done like this for uclinux, which doesn't appreciate
 * having the TLB turned on!
 */
#define PSW_KERNEL_NOSPEC	(PSW_TLB_ENABLE|PSW_SPECLOAD_MALIGNTRAP_EN)
#define PSW_KERNEL_DEFAULT	(PSW_TLB_ENABLE|PSW_SPECLOAD_MALIGNTRAP_EN|PSW_TLB_DYNAMIC)
#define PSW_USER_DEFAULT	(PSW_TLB_ENABLE|PSW_TLB_DYNAMIC|PSW_USER_MODE)

/* TLB bit definitions */
#define TLB_ENTRY0_ASID_MASK		0xff
#define TLB_ENTRY0_SHARED		(1<< 8)	/* Page shared by multiple processes (ASID ignored). */
#define TLB_ENTRY0_PROT_SUPER_X		(1<< 9)
#define TLB_ENTRY0_PROT_SUPER_R		(2<< 9)
#define TLB_ENTRY0_PROT_SUPER_W		(4<< 9)
#define TLB_ENTRY0_PROT_SUPER_RWX	\
	(TLB_ENTRY0_PROT_SUPER_X | TLB_ENTRY0_PROT_SUPER_R | TLB_ENTRY0_PROT_SUPER_W)
#define TLB_ENTRY0_PROT_USER_X		(1<<12)
#define TLB_ENTRY0_PROT_USER_R		(2<<12)
#define TLB_ENTRY0_PROT_USER_W		(4<<12)
#define TLB_ENTRY0_PROT_USER_RWX	\
	(TLB_ENTRY0_PROT_USER_X | TLB_ENTRY0_PROT_USER_R | TLB_ENTRY0_PROT_USER_W)
#define TLB_ENTRY0_DIRTY		(1<<15)	/* Page is dirty. When this bit is 0 write accesses to this page (when write permission is allowed) cause a TLB_WRITE_TO_CLEAN exception. When this bit is 1 writes to this page (when write permission is allowed) are permitted. */
#define TLB_ENTRY0_POLICY_MASK		(15<<16)
#define TLB_ENTRY0_POLICY_UNCACHED	(0<<16)	/* Non cached */
#define TLB_ENTRY0_POLICY_CACHED	(1<<16)	/* Cached */
#define TLB_ENTRY0_POLICY_WCUNCACHED	(2<<16)	/* Write combining */
#define TLB_ENTRY0_SIZE_MASK		(7<<20)
#define TLB_ENTRY0_SIZE_DISABLED	(0<<20)	/* Disabled */
#define TLB_ENTRY0_SIZE_8K		(1<<20)	/* 8Kb */
#define TLB_ENTRY0_SIZE_4M		(2<<20)	/* 4MB */
#define TLB_ENTRY0_SIZE_256M		(3<<20)	/* 256MB */

#define TLB_ENTRY1_VADDR_MASK		0x0007ffff	/* For 8K pages */
#define TLB_ENTRY1_VADDR_SHIFT		13

#define TLB_ENTRY2_PADDR_MASK		0x0007ffff	/* For 8K pages */
#define TLB_ENTRY2_PADDR_SHIFT		13

/* TLB_REPLACE masks */
#define TLB_REPLACE_LFSR_MASK		0xffff
#define TLB_REPLACE_LIMIT_SHIFT		16

/* TLB_CONTROL masks */
#define TLB_CONTROL_ITLB_FLUSH		(1<<0)
#define TLB_CONTROL_DTLB_FLUSH		(1<<1)

/* TLB_ASID masks */
#define TLB_ASID_ASID_MASK		0xff

/* TLB_EXCAUSE bit defintions */
#define TLB_EXCAUSE_INDEX_MASK		0x3f
#define TLB_EXCAUSE_CAUSE_MASK			(3<<16)
#define TLB_EXCAUSE_CAUSE_NO_MAPPING		(0<<16)
#define TLB_EXCAUSE_CAUSE_PROT_VIOLATION	(1<<16)
#define TLB_EXCAUSE_CAUSE_WRITE_TO_CLEAN	(2<<16)
#define TLB_EXCAUSE_CAUSE_MULTI_MAPPING		(3<<16)
#define TLB_EXCAUSE_SPEC	(1<<18)
#define TLB_EXCAUSE_WRITE	(1<<19)
#define TLB_EXCAUSE_IN_UTLB	(1<<20)

/* EXCAUSENO definitions */
#define EXCAUSENO_STBUS_IC_ERROR	 0	/* The Instruction Cache caused a bus error. */
#define EXCAUSENO_STBUS_DC_ERROR	 1	/* The Data Cache caused a bus error. */
#define EXCAUSENO_EXTERN_INT		 2	/* There was an external interrupt. */
#define EXCAUSENO_IBREAK		 3	/* An instruction address breakpoint has occured. */
#define EXCAUSENO_ITLB			 4	/* Instruction side TLB exception */
#define EXCAUSENO_SBREAK		 5	/* A software breakpoint was found. */
#define EXCAUSENO_ILL_INST		 6	/* The bundle could not be decoded into legal sequence of operations or a privileged operation is being issued in user mode. */
#define EXCAUSENO_SYSCALL		 7	/* A syscall instruction was found */
#define EXCAUSENO_DBREAK		 8	/* The DPU has triggered a breakpoint on a data address. */
#define EXCAUSENO_MISALIGNED_TRAP	 9	/* The address is misaligned and misaligned accesses are not supported. */
#define EXCAUSENO_CREG_NO_MAPPING	10	/* The load or store address was in control register space, but no control register exists at that exact address. */
#define EXCAUSENO_CREG_ACCESS_VIOLATION	11	/* A store to a control register was attempted whilst in user mode. */
#define EXCAUSENO_DTLB			12	/* Data side TLB exception */
#define EXCAUSENO_SDI_TIMEOUT		14	/* One of the SDI interfaces timed out while being accessed. */

/* EXCAUSE bit definitions */
#define EXCAUSE_STBUS_IC_ERROR		(1<<EXCAUSENO_STBUS_IC_ERROR)
#define EXCAUSE_STBUS_DC_ERROR		(1<<EXCAUSENO_STBUS_DC_ERROR)
#define EXCAUSE_EXTERN_INT		(1<<EXCAUSENO_EXTERN_INT)
#define EXCAUSE_IBREAK			(1<<EXCAUSENO_IBREAK)
#define EXCAUSE_ITLB			(1<<EXCAUSENO_ITLB)
#define EXCAUSE_SBREAK			(1<<EXCAUSENO_SBREAK)
#define EXCAUSE_ILL_INST		(1<<EXCAUSENO_ILL_INST)
#define EXCAUSE_SYSCALL			(1<<EXCAUSENO_SYSCALL)
#define EXCAUSE_DBREAK			(1<<EXCAUSENO_DBREAK)
#define EXCAUSE_MISALIGNED_TRAP		(1<<EXCAUSENO_MISALIGNED_TRAP)
#define EXCAUSE_CREG_NO_MAPPING		(1<<EXCAUSENO_CREG_NO_MAPPING)
#define EXCAUSE_CREG_ACCESS_VIOLATION	(1<<EXCAUSENO_CREG_ACCESS_VIOLATION)
#define EXCAUSE_DTLB			(1<<EXCAUSENO_DTLB)
#define EXCAUSE_SDI_TIMEOUT		(1<<EXCAUSENO_SDI_TIMEOUT)

/* Additional performance monitoring events */
#define PM_EVENT_LONGIMM		21	/* Number of long immediates in completed bundles. */
#define PM_EVENT_ITLBMISS		22	/* The number of instruction cache fills that missed the ITLB. */
#define PM_EVENT_DTLBMISS		23	/* The number of load/store operations that missed the DTLB when the TLB is enabled. */
#define PM_EVENT_UTLBHIT		24	/* The number of accesses to the UTLB which were hits. */
#define PM_EVENT_ITLBWAITCYCLES		25	/* Number of cycles the instruction cache spends waiting for the ITLB to fill. */
#define PM_EVENT_DTLBWAITCYCLES		26	/* Number of cycles the data cache spends waiting for the ITLB to fill. */
#define PM_EVENT_UTLBARBITRATIONCYCLES	27	/* Number of cycles where the ITLB or DTLB was waiting for access to the UTLB because the UTLB was busy servicing a request. */

#endif /* _ASM_ST200_CTRLREGDEFS_230_H */
