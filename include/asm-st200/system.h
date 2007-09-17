#ifndef __ASM_ST200_SYSTEM_H
#define __ASM_ST200_SYSTEM_H

/* #include <linux/kernel.h> */

/* #include <asm/segment.h> */
#include <asm/ptrace.h>
#include <asm/ctrlregdef.h>

#ifdef __KERNEL__

#define SWITCH_DEBUG_FLAGS (ST200_THREAD_DEBUG_VALID | ST200_THREAD_TRACE_VALID)
#define switch_to(prev, next, last) do {			\
	extern struct task_struct* __switch_to(struct thread_struct* prev_ts,	\
		struct thread_struct* next_ts, unsigned long ssp,	\
		task_t* prev);						\
	extern void clear_debugregs(struct task_struct*);		\
	extern void set_debugregs(struct task_struct*);			\
	if (prev->thread.flags & SWITCH_DEBUG_FLAGS)			\
		clear_debugregs(prev);					\
	if (next->thread.flags & SWITCH_DEBUG_FLAGS)			\
		set_debugregs(next);					\
	last = __switch_to(&prev->thread, &next->thread,		\
			(unsigned long)next->thread_info + THREAD_SIZE, prev);	\
} while (0)

#endif	/* __KERNEL__ */


/*
 * Force strict CPU ordering.
 * And yes, this is required on UP too when we're talking
 * to devices.
 *
 */

/* FIXME : We need to issue a sync() call somehow. There is an intrinsic to
 * do this, alternatively we call to assembler. I've stuck in a function
 * call for now.
 */
extern void __sync(void);

#define mb()		__sync()
#define rmb()		mb()
#define wmb()		mb()
#define smp_mb()	mb()
#define smp_rmb()	mb()
#define smp_wmb()	mb()
#define smp_read_barrier_depends()	do { } while(0)
#define read_barrier_depends()		do { } while(0)

#define set_mb(var, value)		do { var = value; mb(); } while (0)
#define set_wmb(var, value)		do { var = value; wmb(); } while (0)

/* interrupt control.. */
#define local_irq_disable() __asm__ volatile ("pswclr %0\n"         \
					      : /* No output */     \
					      : "r" (PSW_INT_ENABLE)\
					      : "memory"         \
					     );


#define local_irq_enable() __asm__ volatile ("pswset %0\n"         \
					      : /* No output */     \
					      : "r" (PSW_INT_ENABLE)\
					      : "memory"         \
					     );


#define local_save_flags(x) do { __asm__ volatile ("ldw %0 = %1[$r0]\n" \
						   :"=r" (x)            \
						   :"i" (PSW)           \
						  ); } while(0)


#define local_irq_restore(x) do { __asm__ volatile ("pswset %0 \n"   \
						    ";;\n"           \
						    "pswclr %1 \n"   \
						   : /* No output */ \
						   :"r" (x), "r" (~x)\
						   : "memory"        \
						  ); } while(0)


#define irqs_disabled()			\
({					\
	unsigned long flags;		\
	local_save_flags(flags);	\
	!(flags & PSW_INT_ENABLE);	\
})

/* For spinlocks etc */
#define local_irq_save(x)  do { local_save_flags(x); local_irq_disable(); } while (0)

#define xchg(ptr,v) ((__typeof__(*(ptr)))__xchg((unsigned long)(v),(ptr),sizeof(*(ptr))))

extern void __xchg_called_with_bad_pointer(void);

static __inline__ unsigned long __xchg(unsigned long x, volatile void * ptr, int size)
{
	unsigned long retval;

	switch (size) {
	case 4:
		asm volatile("1: ldw %0    = 0[%1]\n"
			     "   mov $r62  = 1b\n"
			     "   or  $r12  = $r12,1\n"
			     "   ;;\n"
			     "   stw 0[%1] = %2\n"
			     "   and $r12  = $r12,~1\n"
			     : "=&r" (retval)
			     : "r" (ptr), "r" (x)
			     : "memory","r62" );
		break;
	case 2:
		asm volatile("1: ldhu %0    = 0[%1]\n"
			     "   mov $r62  = 1b\n"
			     "   or  $r12  = $r12,1\n"
			     "   ;;\n"
			     "   sth 0[%1] = %2\n"
			     "   and $r12  = $r12,~1\n"
			     : "=&r" (retval)
			     : "r" (ptr), "r" (x)
			     : "memory","r62" );
		break;
	case 1:
		asm volatile("1: ldbu %0    = 0[%1]\n"
			     "   mov $r62  = 1b\n"
			     "   or  $r12  = $r12,1\n"
			     "   ;;\n"
			     "   stb 0[%1] = %2\n"
			     "   and $r12  = $r12,~1\n"
			     : "=&r" (retval)
			     : "r" (ptr), "r" (x)
			     : "memory","r62" );
		break;
	default:
		break;
	}

	return retval;
}

extern void die_if_kernel(const char *str, struct pt_regs *regs) __attribute__ ((noreturn));

#endif /* __ASM_ST200_SYSTEM_H */
