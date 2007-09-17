/*
 *  linux/include/asm-st200/setup.h
 *
 * Shamelessly stolen from the ARM for the ST200 by David McKay
 *
 *  Copyright (C) 1997-1999 Russell King
 *  Copyright (C) 2004 Dave McKay <david.mckay@st.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Structure passed to kernel to tell it about the
 *  hardware it's running on.  See Documentation/st200/Setup
 *  for more info.
 */

#ifndef __ASM_ST200_SETUP_H
#define __ASM_ST200_SETUP_H

/* The list ends with an ATAG_NONE node. */
#define ATAG_NONE	0x00000000

struct tag_header {
	u32 size; /* Size IN WORDS */
	u32 tag;
};

#define ATAG_MKTAG(x) ((0x5441<<16)|(x))

/* The list must start with an ATAG_CORE node */
#define ATAG_CORE	ATAG_MKTAG(1)
#define ATAG_CORE_MAGIC 0xabcd1234
struct tag_core {
	u32 magic;
	u32 flags;		/* bit 0 = read-only root filesystem */
	u32 rootdev;
};

/* On the ST200 we can only have one of these.
 * We cannot cope with discontiguous memory at the moment,
 * and the kernel image is not relocatable, so  the start
 * is a bit of a waste of time.
 */
#define ATAG_MEM	ATAG_MKTAG(2)

struct tag_mem32 {
	u32	start;	/* physical start address */
	u32	size;
};

/* describes where the compressed ramdisk image lives (physical address) */
#define ATAG_INITRD	ATAG_MKTAG(5)

struct tag_initrd {
	u32 start;	/* physical start address */
	u32 size;	/* size of compressed ramdisk image in bytes */
};


#define COMMAND_LINE_SIZE	1024

/* command line: \0 terminated string */
#define ATAG_CMDLINE	ATAG_MKTAG(9)

struct tag_cmdline {
	char	cmdline[1];	/* this is the minimum size */
};

struct tag {
	struct tag_header hdr;
	union {
		struct tag_core		core;
		struct tag_mem32	mem;
		struct tag_initrd	initrd;
		struct tag_cmdline	cmdline;
	} u;
};

struct tagtable {
	u32 tag;
	int (*parse)(const struct tag *);
};

#define __tag __attribute_used__ __attribute__((__section__(".taglist")))
#define __tagtable(tag, fn) \
static struct tagtable __tagtable_##fn __tag = { tag, fn }

#define tag_member_present(tag,member)				\
	((unsigned long)(&((struct tag *)0L)->member + 1)	\
		<= (tag)->hdr.size * 4)

#define tag_next(t)	((struct tag *)((u32 *)(t) + (t)->hdr.size))
#define tag_size(type)	((sizeof(struct tag_header) + sizeof(struct type)) >> 2)

#define for_each_tag(t,base)		\
	for (t = base; t->hdr.size; t = tag_next(t))

/*
 * Early command line parameters.
 */
struct early_params {
	const char *arg;
	void (*fn)(char **p);
};

#define __early_param(name,fn)					\
static struct early_params __early_##fn __attribute_used__	\
__attribute__((__section__("__early_param"))) = { name, fn }



#endif /*  __ASM_ST200_SETUP_H */
