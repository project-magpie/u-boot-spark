/* $Id: sh4-cache.h,v 1.3 2005/11/30 12:00:31 sturgesa Exp $
 *
 * include/asm-sh/cache.h
 *
 * Copyright 2005 (C) Andy Sturges
 */

#ifndef __ASM_SH4_CACHE_H
#define __ASM_SH4_CACHE_H

#define CACHE_IC_ADDRESS_ARRAY	0xf0000000
#define CACHE_OC_ADDRESS_ARRAY	0xf4000000

#if defined(CONFIG_CPU_SUBTYPE_SH4_1XX)	/* its an SH4-100 (ST40) */
#define	DCACHE_WAY_INCR	    (1 << 14)
#define	DCACHE_ENTRY_SHIFT  5
#define	DCACHE_ENTRY_MASK   0x3fe0
#define	DCACHE_SETS	    512
#define	DCACHE_WAYS	    1
#define	DCACHE_LINESZ	    L1_CACHE_BYTES
#elif defined(CONFIG_CPU_SUBTYPE_SH4_2XX)
#define DCACHE_SIZE         32768
#define	DCACHE_WAY_INCR	    (DCACHE_SIZE >> 1)
#define	DCACHE_ENTRY_SHIFT  5
#define	DCACHE_ENTRY_MASK   (DCACHE_WAY_INCR - (1 << 5))
#define	DCACHE_SETS	    (DCACHE_SIZE >> 6)
#define	DCACHE_WAYS	    2
#define	DCACHE_LINESZ	    L1_CACHE_BYTES
#else
#error Unknown CPU
#endif

#endif /* __ASM_SH4_CACHE_H */
