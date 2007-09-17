/*
 * include/asm-st200/ctrlregdef-220.h
 *
 * Copyright (C) 2002 STMicroelectronics Limited
 *	Author: Stuart Menefy <stuart.menefy@st.com>
 *
 * Control register defintions for ST220
 */

#ifndef _ASM_ST200_CTRLREGDEFS_220_H
#define _ASM_ST200_CTRLREGDEFS_220_H

/************ PSW bit definitions ************/
#define PSW_USER_MODE			(1<< 0)	/* When 1 the core is in user mode, otherwise supervisor mode. */
#define PSW_INT_ENABLE			(1<< 1)	/* When 1 external interrupts are enabled. */
#define PSW_SPECLOAD_MALIGNTRAP_EN	(1<< 4)	/* When 1 enables exceptions on speculative load misalignment errors. */
#define PSW_SPECLOAD_DPUTRAP_EN		(1<< 5)	/* When 1 exceptions on speculative load DPU traps are enabled. */
#define PSW_DPU_ENABLE			(1<< 6)	/* When 1 the DPU is enabled. */
#define PSW_IPU_ENABLE			(1<< 7)	/* When 1 the IPU is enabled. */
#define PSW_DBREAK_ENABLE		(1<< 8)	/* When 1 data breakpoints are enabled. */
#define PSW_IBREAK_ENABLE		(1<< 9)	/* When 1 instruction breakpoints are enabled. */
#define PSW_DEBUG_MODE			(1<<12)	/* When 1 the core is in debug mode. */

/* These are the default things we always stick in the PSW, separated out for
 * user and kernel. Done like this for uclinux, which doesn't appreciate
 * having the TLB turned on!
 */
#define PSW_USER_DEFAULT (PSW_USER_MODE)
#define PSW_KERNEL_DEFAULT (0)
#define PSW_KERNEL_NOSPEC (0)

/* EXCAUSE bit definitions */
#define EXCAUSE_STBUS_IC_ERROR		(1<< 0)	/* The Instruction Cache caused a bus error. */
#define EXCAUSE_STBUS_DC_ERROR		(1<< 1)	/* The Data Cache caused a bus error. */
#define EXCAUSE_EXTERN_INT		(1<< 2)	/* There was an external interrupt. */
#define EXCAUSE_IBREAK			(1<< 3)	/* An instruction address breakpoint has occured. */
#define EXCAUSE_IPU_NO_TRANSLATION	(1<< 4)	/* There was no mapping the given address. */
#define EXCAUSE_IPU_ACCESS_VIOLATION	(1<< 5)	/* Permission to access an address controlled by the IPU was not met. */
#define EXCAUSE_SBREAK			(1<< 6)	/* A software breakpoint was found. */
#define EXCAUSE_ILL_INST		(1<< 7)	/* The bundle could not be decoded into legal sequence of operations or a privileged operation is being issued in user mode. */
#define EXCAUSE_DBREAK			(1<< 8)	/* The DPU has triggered a breakpoint on a data address. */
#define EXCAUSE_MISALIGNED_TRAP		(1<< 9)	/* The address is misaligned and misaligned accesses are not supported. */
#define EXCAUSE_CREG_NO_MAPPING		(1<<10)	/* The load or store address was in control register space, but no control register exists at that exact address. */
#define EXCAUSE_CREG_ACCESS_VIOLATION	(1<<11)	/* A store to a control register was attempted whilst in user mode. */
#define EXCAUSE_DPU_NO_TRANSLATION	(1<<12)	/* There was no mapping in the DPU for the given address. */
#define EXCAUSE_DPU_ACCESS_VIOLATION	(1<<13)	/* Permission to access an address controlled by the DPU was not met. */
#define EXCAUSE_SDI_TIMEOUT		(1<<14)	/* One of the SDI interfaces timed out while being accessed. */

#endif /* _ASM_ST200_CTRLREGDEFS_220_H */
