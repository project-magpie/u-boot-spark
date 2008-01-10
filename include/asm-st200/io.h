/*
 * include/asm-st200/io.h
 *
 * Copyright (C) 2002 STMicroelectronics Limited
 *	Author: Stuart Menefy <stuart.menefy@st.com>
 *
 */

#ifndef _ASM_ST200_IO_H
#define _ASM_ST200_IO_H

#include <linux/config.h>

/*
 * This file contains the definitions for the x86 IO instructions
 * inb/inw/inl/outb/outw/outl and the "string versions" of the same
 * (insb/insw/insl/outsb/outsw/outsl). You can also use "pausing"
 * versions of the single-IO instructions (inb_p/inw_p/..).
 */

#ifdef __KERNEL__

/* #include <asm/pgtable.h> */

#define IO_SPACE_LIMIT 0xffff

/**
 *	virt_to_phys	-	map virtual addresses to physical
 *	@address: address to remap
 *
 *	The returned physical address is the physical (CPU) mapping for
 *	the memory address given. It is only valid to use this function on
 *	addresses directly mapped or allocated via kmalloc.
 *
 *	This function does not give bus mappings for DMA transfers. In
 *	almost all conceivable cases a device driver should not be using
 *	this function
 */
static inline unsigned long virt_to_phys(volatile void * address)
{
	return (unsigned long)address;
}

/**
 *	phys_to_virt	-	map physical address to virtual
 *	@address: address to remap
 *
 *	The returned virtual address is a current CPU mapping for
 *	the memory address given. It is only valid to use this function on
 *	addresses that have a kernel mapping
 * page_to_phys
 *	This function does not handle bus mappings for DMA transfers. In
 *	almost all conceivable cases a device driver should not be using
 *	this function
 */
static inline void * phys_to_virt(unsigned long address)
{
	return (void*)address;
}

/*
 * Change "struct page" to physical address.
 */
#ifndef CONFIG_DISCONTIGMEM
#define page_to_phys(page) ((dma_addr_t)page_to_pfn(page) << PAGE_SHIFT)
#endif

#ifdef CONFIG_MMU
extern void * __ioremap (unsigned long offset, unsigned long size,
			 pgprot_t prot);
extern void iounmap(void *addr);
#else
#define __ioremap(__offset, __size, __flags)	((void *)(__offset))
#define iounmap(addr)				do { } while(0)
#endif

/**
 * ioremap     -   map bus memory into CPU space
 * @offset:    bus address of the memory
 * @size:      size of the resource to map
 *
 * ioremap performs a platform specific sequence of operations to
 * make bus memory CPU accessible via the readb/readw/readl/writeb/
 * writew/writel functions and the other mmio helpers. The returned
 * address is not guaranteed to be usable directly as a virtual
 * address.
 */

static inline void * ioremap (unsigned long offset, unsigned long size)
{
	return __ioremap(offset, size, PAGE_KERNEL_NC);
}

/**
 * ioremap_...     -   map bus memory into CPU space
 * @offset:    bus address of the memory
 * @size:      size of the resource to map
 *
 * These functions behave in the same way as ioremap, but with specified
 * cache attributes.
 *
 * Must be freed with iounmap.
 */
static inline void * ioremap_nocache (unsigned long offset, unsigned long size)
{
	return __ioremap(offset, size, PAGE_KERNEL_NC);
}

static inline void * ioremap_writecombine (unsigned long offset, unsigned long size)
{
	return __ioremap(offset, size, PAGE_KERNEL_NCWC);
}

static inline void * ioremap_cache (unsigned long offset, unsigned long size)
{
	return __ioremap(offset, size, PAGE_KERNEL);
}

/*
 * bt_ioremap() is for early boot-time mappings.
 * It combines the functions of request_mem_resource and ioremap.
 */
extern void *bt_ioremap(unsigned long phys, unsigned long size,
			const char* name);

/*
 * readX/writeX() are used to access memory mapped devices. On some
 * architectures the memory mapped IO stuff needs to be accessed
 * differently. On the ST200 architecture, we just read/write the
 * memory location directly.
 */
#define readb(addr) (*(volatile unsigned char *)(addr))
#define readw(addr) (*(volatile unsigned short *)(addr))
#define readl(addr) (*(volatile unsigned int *)(addr))
#define readsl(addr,data,len)  __raw_readsl(addr,data,len)
#define __raw_readb readb
#define __raw_readw readw
#define __raw_readl readl


#define writeb(b,addr) (*(volatile unsigned char *)(addr) = (b))
#define writew(b,addr) (*(volatile unsigned short *)(addr) = (b))
#define writel(b,addr) (*(volatile unsigned int *)(addr) = (b))
#define writesl(addr,data,len)  __raw_writesl(addr,data,len)
#define __raw_writeb writeb
#define __raw_writew writew
#define __raw_writel writel




#if 0
#define memset_io(a,b,c)	memset((a),(b),(c))
#define memcpy_fromio(a,b,c)	memcpy((a),(b),(c))
#define memcpy_toio(a,b,c)	memcpy((a),(b),(c))
#else
/* We need to do word accesses in memcpy_fromio() in order
 * to get the 64 bit wide flash to work
 */
void memcpy_fromio(void *a,unsigned long addr,int len);
void memcpy_toio(unsigned long addr,void *a,int len);
void memset_io(unsigned long addr,int c,int len);

#endif

/*
 * inX/outX() are used to access IO ports. As there are no IO
 * instructions on the ST200, this is simply a special memory region.
 */
extern long PORT_TO_ADDR_OFFSET_UNKNOWN(void);


#if defined(CONFIG_ST200_MB388_FPGA) && defined(CONFIG_PCI)
/* This is a temporary bodge until we get the proper board support
 * infrastructure working
 */
extern volatile char * v320_iomap; /* Pointer to io region of vs320 */
#define PORT_TO_ADDR(x) ((x)+ v320_iomap)
#else
#define PORT_TO_ADDR(x) (x)
#endif

#define inb(port) (*(volatile unsigned char*)PORT_TO_ADDR(port))
#define inw(port) (*(volatile unsigned short*)PORT_TO_ADDR(port))
#define inl(port) (*(volatile unsigned int*)PORT_TO_ADDR(port))

#define outb(b, port) (*(volatile unsigned char*)PORT_TO_ADDR(port) = (b))
#define outw(b, port) (*(volatile unsigned short*)PORT_TO_ADDR(port) = (b))
#define outl(b, port) (*(volatile unsigned int*)PORT_TO_ADDR(port) = (b))

#define inb_p inb
#define outb_p outb
#define inw_p inw
#define outw_p outw
#define inl_p inl
#define outl_p outl

extern void outsb(unsigned long addr, const void *src, unsigned long count);
extern void outsw(unsigned long addr, const void *src, unsigned long count);
extern void outsl(unsigned long addr, const void *src, unsigned long count);
extern void insb(unsigned long addr, void *dst, unsigned long count);
extern void insw(unsigned long addr, void *dst, unsigned long count);
extern void insl(unsigned long addr, void *dst, unsigned long count);

/**
 *	check_signature		-	find BIOS signatures
 *	@io_addr: mmio address to check
 *	@signature:  signature block
 *	@length: length of signature
 *
 *	Perform a signature comparison with the mmio address io_addr. This
 *	address should have been obtained by ioremap.
 *	Returns 1 on a match.
 */

static inline int check_signature(unsigned long io_addr,
	const unsigned char *signature, int length)
{
	int retval = 0;
	do {
		if (readb(io_addr) != *signature)
			goto out;
		io_addr++;
		signature++;
		length--;
	} while (length);
	retval = 1;
out:
	return retval;
}

/* ST200 caches are not DMA coherent */
extern void dma_cache_wback_inv(unsigned long start, unsigned long size);
extern void dma_cache_wback(unsigned long start, unsigned long size);
extern void dma_cache_inv(unsigned long start, unsigned long size);


/* The ctrl_inl() etc macros are used to acccess the control
 * registers of the ST200. These are mapped to the top 64K of
 * addess space. They are *not* translated, so we pull them out
 * here in case we have to do wierd things with readl() and writel()
 * later on.
 */
#define ctrl_inl(addr) (*(volatile unsigned long *)(addr))
#define ctrl_outl(b,addr) (*(volatile unsigned long *)(addr) = (b))

#endif /* __KERNEL__ */


#endif /* _ASM_ST200_IO_H */
