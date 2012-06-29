#ifndef __ASM_ST40_IO_H
#define __ASM_ST40_IO_H

/*
 * Convention:
 *    read{b,w,l}/write{b,w,l} are for PCI,
 *    while in{b,w,l}/out{b,w,l} are for ISA
 * These may (will) be platform specific function.
 * In addition we have 'string' versions: ins{b,w,l}/outs{b,w,l}
 * For read{b,w,l} and write{b,w,l} there are also __raw versions, which
 * do not have a memory barrier after them.
 *
 * In addition, we have
 *   ctrl_in{b,w,l}/ctrl_out{b,w,l} for SuperH specific I/O.
 *   which are processor specific.
 */

/*
 * We follow the Alpha convention here:
 *  __inb expands to an inline function call (which either calls via the
 *        mach_vec if generic, or a machine specific implementation)
 *  _inb  is a real function call (note ___raw fns are _ version of __raw)
 *  inb   by default expands to _inb, but the machine specific code may
 *        define it to __inb if it chooses.
 */


#include "common.h"

#include "asm/cache.h"
#include "asm/system.h"
#include "linux/config.h"



/********************************************************
 *	__raw_read{b,w,l}, __raw_write{b,w,l}
 ********************************************************/

#define __raw_readb(addr)	(*(volatile unsigned char  *)(addr))
#define __raw_readw(addr)	(*(volatile unsigned short *)(addr))
#define __raw_readl(addr)	(*(volatile unsigned int   *)(addr))

#define __raw_writeb(v,addr)	(*(volatile unsigned char  *)(addr) = (v))
#define __raw_writew(v,addr)	(*(volatile unsigned short *)(addr) = (v))
#define __raw_writel(v,addr)	(*(volatile unsigned int   *)(addr) = (v))



/********************************************************
 *	read{b,w,l}, write{b,w,l}
 ********************************************************/

#define readb(addr)		__raw_readb((addr))
#define readw(addr)		__raw_readw((addr))
#define readl(addr)		__raw_readl((addr))

#define writeb(v,addr)		__raw_writeb((v),(addr))
#define writew(v,addr)		__raw_writew((v),(addr))
#define writel(v,addr)		__raw_writel((v),(addr))



/********************************************************
 *	_in{b,w,l}, _out{b,w,l}
 ********************************************************/

static __inline__ unsigned char _inb (unsigned long addr)
{
	return *(volatile unsigned char *) addr;
}
static __inline__ unsigned short _inw (unsigned long addr)
{
	return *(volatile unsigned short *) addr;
}
static __inline__ unsigned int _inl (unsigned long addr)
{
	return *(volatile unsigned long *) addr;
}

static __inline__ void _outb (unsigned char v, unsigned long addr)
{
	*(volatile unsigned char *) addr = v;
}
static __inline__ void _outw (unsigned short v, unsigned long addr)
{
	*(volatile unsigned short *) addr = v;
}
static __inline__ void _outl (unsigned int v, unsigned long addr)
{
	*(volatile unsigned long *) addr = v;
}

/********************************************************
 *	in{b,w,l}, out{b,w,l}
 ********************************************************/

#define inb(p)		_inb((p))
#define inw(p)		_inw((p))
#define inl(p)		_inl((p))

#define outb(v,p)	_outb((v),(p))
#define outw(v,p)	_outw((v),(p))
#define outl(v,p)	_outl((v),(p))



/********************************************************
 *	_ins{b,w,l}, _outs{b,w,l}
 ********************************************************/

extern void _insb (unsigned long port, void *dst, unsigned long count);
extern void _insw (unsigned long port, void *dst, unsigned long count);
extern void _insl (unsigned long port, void *dst, unsigned long count);

extern void _outsb (unsigned long port, const void *src, unsigned long count);
extern void _outsw (unsigned long port, const void *src, unsigned long count);
extern void _outsl (unsigned long port, const void *src, unsigned long count);

/********************************************************
 *	ins{b,w,l}, outs{b,w,l}
 ********************************************************/

#define insb(p,d,c)	_insb((p),(d),(c))
#define insw(p,d,c)	_insw((p),(d),(c))
#define insl(p,d,c)	_insl((p),(d),(c))

#define outsb(p,s,c)	_outsb((p),(s),(c))
#define outsw(p,s,c)	_outsw((p),(s),(c))
#define outsl(p,s,c)	_outsl((p),(s),(c))



static inline void sync(void)
{
	/* do nothing */
	/* Note: may need to include a "synco" instruction here, if we
	 * have silicon with aggressive write-combiners on the SH4-300
	 * series cores in the future */
}


/*
 * Given a physical address and a length, return a virtual address
 * that can be used to access the memory range with the caching
 * properties specified by "flags".
 */
#define MAP_NOCACHE     (0)
#define MAP_WRCOMBINE   (0)
#define MAP_WRBACK      (0)
#define MAP_WRTHROUGH   (0)

static inline void *
map_physmem(phys_addr_t paddr, unsigned long len, unsigned long flags)
{
	return (void *)paddr;
}

/*
 * Take down a mapping set up by map_physmem().
 */
static inline void unmap_physmem(void *vaddr, unsigned long flags)
{
	/* do nothing */
}

static inline phys_addr_t virt_to_phys(void * const vaddr)
{
	return (phys_addr_t)(vaddr);
}

#endif /* __ASM_ST40_IO_H */
