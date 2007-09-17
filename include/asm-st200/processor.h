/*
 * include/asm-st200/processor.h
 *
 * Copyright (C) 2002 STMicroelectronics Limited
 *	Author: Stuart Menefy <stuart.menefy@st.com>
 *
 */

#ifndef _ASM_ST200_PROCESSOR_H
#define _ASM_ST200_PROCESSOR_H

#include <asm/ptrace.h>
#include <asm/page.h>

/*
 * Default implementation of macro that returns current
 * instruction pointer ("program counter").
 */
#define current_text_addr() ({ __label__ _l; _l: &&_l;})

#ifdef UNKNOWN
struct cpuinfo_st200 {

};

/*
 * capabilities of CPUs
 */

extern struct cpuinfo_st200 boot_cpu_data;
#endif

#ifdef CONFIG_SMP
extern struct cpuinfo_st200 cpu_data[];
#define current_cpu_data cpu_data[smp_processor_id()]
#else
#define cpu_data (&boot_cpu_data)
#define current_cpu_data boot_cpu_data
#endif

#if defined(CONFIG_EISA) || defined(CONFIG_MCA)
#error Bus handling need updating
#endif
#define EISA_bus (0)
#define EISA_bus__is_a_macro /* for versions in ksyms.c */
#define MCA_bus  (0)
#define MCA_bus__is_a_macro /* for versions in ksyms.c */


#ifdef CONFIG_MMU
/*
 * User space process size.
 */

#define TASK_SIZE	        (PAGE_OFFSET)
/* This decides where the kernel will search for a free chunk of vm
 * space during mmap's.
 */
#define TASK_UNMAPPED_BASE	(PAGE_ALIGN(TASK_SIZE / 3))
#else
#define TASK_SIZE               0xffff0000UL
#define TASK_UNMAPPED_BASE	0
#endif


typedef struct {
	unsigned long seg;
} mm_segment_t;

/* Flags for struct thread_struct flags field */
#define ST200_THREAD_DEBUG_VALID	(1<<0)
#define ST200_THREAD_TRACE_VALID	(1<<1)

/* Offsets into this struct are used in __switchto */
struct thread_struct {
	unsigned long sp;
	unsigned long pc;

	unsigned long flags;

	/* Debug registers */
	struct {
		unsigned long dbreak_control;
		unsigned long dbreak_lower;
		unsigned long dbreak_upper;
		unsigned long ibreak_control;
		unsigned long ibreak_lower;
		unsigned long ibreak_upper;
	} debug;

	/* Trace registers */
	struct {
		unsigned long trace;
	} trace;
};


#define KSTK_EIP(tsk)  ((tsk)->thread.pc)
#define KSTK_ESP(tsk)  ((tsk)->thread.sp)

#define INIT_THREAD  { .flags = 0 }

/* Forward declaration, a strange C thing */
struct task_struct;
struct mm_struct;

/* Do necessary setup to start up a newly executed thread.  */
extern void start_thread(struct pt_regs *, unsigned long, unsigned long);

/* Free all resources held by a thread. */
extern void release_thread(struct task_struct *);

/* Prepare to copy thread state - unlazy all lazy status */
#define prepare_to_copy(tsk)	do { } while (0)

/* create a kernel thread without removing it from tasklists */
extern int kernel_thread(int (*fn)(void *), void * arg, unsigned long flags);

/* Return saved PC of a blocked thread.  */
/* This is probably not 100% correct */

static inline unsigned long thread_saved_pc(struct thread_struct *t)
{
        return t->pc;
}


unsigned long get_wchan(struct task_struct *p);

#define cpu_relax()	barrier()

#ifdef UNKNOWN

#define ARCH_HAS_PREFETCH
#define ARCH_HAS_PREFETCHW
#define ARCH_HAS_SPINLOCK_PREFETCH

extern inline void prefetch(const void *x)
{
	 __asm__ __volatile__ ("prefetch (%0)" : : "r"(x));
}

extern inline void prefetchw(const void *x)
{
	 __asm__ __volatile__ ("prefetchw (%0)" : : "r"(x));
}

#define spin_lock_prefetch(x)	prefetchw(x)

#endif

#endif /* _ASM_ST200_PROCESSOR_H */
