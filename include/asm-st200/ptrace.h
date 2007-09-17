/*
 * include/asm-st200/ptrace.h
 *
 * Copyright (C) 2002 STMicroelectronics Limited
 *	Author: Stuart Menefy <stuart.menefy@st.com>
 *
 */

#ifndef _ASM_ST200_PTRACE_H
#define _ASM_ST200_PTRACE_H

/*
 * This struct defines the way the registers are stored on the
 * kernel stack during a system call or other kernel entry
 *
 * To improve performance, this only contains the scratch registers,
 * including the system call parameters (in case we need to restart
 * the system call), and a few other fundamental registers.
 *
 * All preserved registers will be naturally preserved or saved in
 * switch_stack.
 *
 * Note that in the case of a system call, only the argument registers
 * may be present.
 *
 * A few system calls (notably fork and friends) and signal handling
 * require the full register state. This is handled in special stubs,
 * which push a struct switch_stack immediatly below the struct pt_regs.
 *
 * To keep stack alignment simple, both structs should be multiples of 32
 * bytes.
 */

struct pt_regs {
	unsigned long psw;
        unsigned long pc;
	unsigned long br;
	unsigned long lr;	/* Link */

	unsigned long r8;	/* scratch */
	unsigned long r9;	/* scratch */
	unsigned long r10;	/* scratch */
	unsigned long r11;	/* scratch */
	unsigned long r12;	/* SP */
	unsigned long r15;	/* scratch/syscall no */
	unsigned long r16;	/* arg0 */
	unsigned long r17;	/* arg1 */
	unsigned long r18;	/* arg2 */
	unsigned long r19;	/* arg3 */
	unsigned long r20;	/* arg4 */
	unsigned long r21;	/* arg5 */
	unsigned long r22;	/* arg6 */
	unsigned long r23;	/* arg7 */
	unsigned long r24;	/* scratch */
	unsigned long r25;	/* scratch */
	unsigned long r26;	/* scratch */
	unsigned long r27;	/* scratch */
	unsigned long r28;	/* scratch */
	unsigned long r29;	/* scratch */
	unsigned long r30;	/* scratch */
	unsigned long r31;	/* scratch */
	unsigned long r32;	/* scratch */
	unsigned long r33;	/* scratch */
	unsigned long r34;	/* scratch */
	unsigned long r35;	/* scratch */
	unsigned long r36;	/* scratch */
	unsigned long r37;	/* scratch */
	unsigned long r38;	/* scratch */
	unsigned long r39;	/* scratch */
	unsigned long r40;	/* scratch */
	unsigned long r41;	/* scratch */
	unsigned long r42;	/* scratch */
	unsigned long r43;	/* scratch */
	unsigned long r44;	/* scratch */
	unsigned long r45;	/* scratch */
	unsigned long r46;	/* scratch */
	unsigned long r47;	/* scratch */
	unsigned long r48;	/* scratch */
	unsigned long r49;	/* scratch */
	unsigned long r50;	/* scratch */
	unsigned long r51;	/* scratch */
	unsigned long r52;	/* scratch */
	unsigned long r53;	/* scratch */
	unsigned long r54;	/* scratch */
	unsigned long r55;	/* scratch */
	unsigned long r56;	/* scratch */
	unsigned long r57;	/* scratch */
	unsigned long r58;	/* scratch */
	unsigned long r59;	/* scratch */
	unsigned long r60;	/* scratch */
	unsigned long r61;	/* scratch */
	unsigned long r62;	/* scratch */
	unsigned long pad[1];   /* Align to 8 bytes*/
};

/*
 * This is the extended stack used by signal handlers and the context
 * switcher: it's pushed after the normal "struct pt_regs".
 */
struct switch_stack {
	unsigned long r1;
	unsigned long r2;
	unsigned long r3;
	unsigned long r4;
	unsigned long r5;
	unsigned long r6;
	unsigned long r7;
        unsigned long r13;	/* Reserved for TLS (thread local storage) */
	unsigned long r14;	/* GOT pointer */
	unsigned long pad[1];   /* To align to 8 byte boundary */
};

#define _PSW_USER 1
#define user_mode(regs) ((regs)->psw & _PSW_USER)
#define instruction_pointer(regs) ((regs)->pc)

#define force_successful_syscall_return() (st200_task_regs(current_thread_info())->r15 = 0)

/* Dump register state. Pass NULL if ss not available */
void st200_show_regs(struct pt_regs* regs, struct switch_stack *ss);

/* Register numbers in the ptrace system call interface */

#define PT_PC		0
#define PT_R0		1
#define PT_R(x)		((x)+PT_R0)
#define PT_B(x)		((x)+65)
#define PT_PSW		73
#define PT_DEBUG(x)	((x)+74)
#define PT_TRACE	80

#endif /* _ASM_ST200_PTRACE_H */
