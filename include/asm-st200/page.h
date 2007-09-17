/*
 *  include/asm-st200/io.h
 *
 *  Copyright (C) 2002 STMicroelectronics Limited
 *	Author: Stuart Menefy <stuart.menefy@st.com>
 *
 */

#ifndef _ASM_ST200_PAGE_H
#define _ASM_ST200_PAGE_H

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT	13
#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#define PAGE_SHIFT_256M		28
#define PAGE_SIZE_256M		(1<<PAGE_SHIFT_256M)
#define PAGE_MASK_256M		(~(PAGE_SIZE_256M-1))

#define clear_page(page)	memset((void *)(page), 0, PAGE_SIZE)
#define copy_page(to,from)	memcpy((void *)(to), (void *)(from), PAGE_SIZE)

#define clear_user_page(to, vaddr, page)	\
do {						\
	clear_page(to);				\
	flush_dcache_page(page);		\
} while (0)
#define copy_user_page(to, from, vaddr, page)	\
do {						\
	copy_page(to, from);			\
	flush_dcache_page(page);		\
} while (0)

/*
 * These are used to make use of C type-checking..
 */
typedef struct { unsigned long pte; } pte_t;
typedef struct { unsigned long pmd; } pmd_t;
typedef struct { unsigned long pgd; } pgd_t;
typedef struct { unsigned long pgprot; } pgprot_t;

#define pte_val(x)	((x).pte)
#define pmd_val(x)	((x).pmd)
#define pgd_val(x)	((x).pgd)
#define pgprot_val(x)	((x).pgprot)

#define __pte(x) ((pte_t) { (x) } )
#define __pmd(x) ((pmd_t) { (x) } )
#define __pgd(x) ((pgd_t) { (x) } )
#define __pgprot(x)	((pgprot_t) { (x) } )

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)	(((addr)+PAGE_SIZE-1)&PAGE_MASK)

/* Start of kernel address space */
#ifdef CONFIG_MMU
#define __PAGE_OFFSET		(0x80000000)
#else
#define __PAGE_OFFSET		(0x0)
#endif

#define CONTROL_START		(0xffff0000)

#define _MEMORY_START		CONFIG_MEMORY_START
#define _MEMORY_SIZE		CONFIG_MEMORY_SIZE

/*
 * PFNs start at physical 0, while pages start at _MEMORY_START
 */
#define PFN_OFFSET		(_MEMORY_START >> PAGE_SHIFT)


extern unsigned long last_user_utlb_entry;
#define UTLB_USER_START		0
#define UTLB_USER_LAST		last_user_utlb_entry
#define UTLB_USER_ENTRIES	(UTLB_USER_LAST - UTLB_USER_START + 1)
#define UTLB_KERNEL_ENTRY	(UTLB_USER_LAST + 1)

/* Pure 2^n version of get_order */
static __inline__ int get_order(unsigned long size)
{
	int order;

	size = (size-1) >> (PAGE_SHIFT-1);
	order = -1;
	do {
		size >>= 1;
		order++;
	} while (size);
	return order;
}


#define PAGE_OFFSET		((unsigned long)__PAGE_OFFSET)

#define __pa(x)			((unsigned long)(x)-PAGE_OFFSET)
#define __va(x)			((void *)((unsigned long)(x)+PAGE_OFFSET))

#define pfn_to_page(pfn)	(mem_map + ((pfn) - PFN_OFFSET))
#define page_to_pfn(page)	((unsigned long)((page)-mem_map) + PFN_OFFSET)
#define virt_to_page(kaddr)	pfn_to_page(__pa(kaddr) >> PAGE_SHIFT)
#define pfn_valid(pfn)		(((pfn) - PFN_OFFSET) < max_mapnr)
#define virt_addr_valid(kaddr)	pfn_valid(__pa(kaddr) >> PAGE_SHIFT)

/*
 * Default vma flags for stack and anonymous pages
 * Note we do not have execute permission.
 */
#define VM_DATA_DEFAULT_FLAGS	(VM_READ | VM_WRITE | \
				 VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC)

/* Is this physical address RAM or not? */
int phys_is_ram(unsigned long physaddr);

#endif /* _ASM_ST200_PAGE_H */
