	/*
	 * Add TLB regions:
	 *  - LMI, cached
	 *  - Flash, cached
	 */
	POKE_LONG(0xFFFFFF50, 0x3) /* Flush instruction and data TLBs */

	/* Set up identity RAM region as cached, rwx, 256MB */
	POKE_LONG(0xFFFFFF80, 0x0) /* Set TLB index to 0 */
	POKE_LONG(0xFFFFFF70, (0x80000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x80000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x7000 |    /* User rwx */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x10000 |   /* Cached policy */
			      0x300000)   /* 256MB page size */


	/* Set up identity flash region 1 as uncached, rwx, 4MB */
	POKE_LONG(0xFFFFFF80, 0x1) /* Set TLB index to 1 */
	POKE_LONG(0xFFFFFF70, (0x00000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x00000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x7000 |    /* User rwx */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x00000 |   /* Uncached policy */
			      0x200000)   /* 4MB page size */

	/* Set up identity flash region 2 as uncached, rwx, 4MB */
	POKE_LONG(0xFFFFFF80, 0x2) /* Set TLB index to 2 */
	POKE_LONG(0xFFFFFF70, (0x00400000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x00400000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x7000 |    /* User rwx */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x00000 |   /* Uncached policy */
			      0x200000)   /* 4MB page size */


	POKE_LONG(0xFFFFFF80, 0x3) /* Set TLB index to 4 */
	POKE_LONG(0xFFFFFF70, (0x40000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x40000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x6000 |    /* User rw */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |       /* Uncached policy */
			      0x300000)   /* 256MB page size */

	POKE_LONG(0xFFFFFF80, 0x4) /* Set TLB index to 5 */
	POKE_LONG(0xFFFFFF70, (0x50000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x50000000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x6000 |    /* User rw */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |       /* Uncached policy */
			      0x300000)   /* 256MB page size */
/* Ethernet */

	POKE_LONG(0xFFFFFF80, 0x5) /* Set TLB index to 6 */
	POKE_LONG(0xFFFFFF70, (0x800000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x800000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x6000 |    /* User rw */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |       /* Uncached policy */
			      0x100000)   /* 8K page size */

/* ATAPI/IDE */
	POKE_LONG(0xFFFFFF80, 0x8) /* Set TLB index to 7 */
	POKE_LONG(0xFFFFFF70, (0x05100000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set virtual address to use */
	POKE_LONG(0xFFFFFF68, (0x05100000 >> LOG2_LXTLB_MIN_PAGE_SIZE)) /* Set physical address to use */
	POKE_LONG(0xFFFFFF78, 0xe00 |     /* Supervisor rwx */
			      0x6000 |    /* User rw */
			      0x100 |     /* Shared on */
			      0x8000 |    /* Page is dirty bit (writes allowed?) */
			      0x0 |       /* Uncached policy */
			      0x200000)   /* 4M page size */

	POKE_LONG(0xFFFFFF50, 0x3) /* Flush instruction and data TLBs */

