/*
 * include/asm-st200/cache.h
 *
 *  Copyright (C) 2002 STMicroelectronics Limited
 *	Author: Stuart Menefy <stuart.menefy@st.com>
 *
 */
#ifndef _ASM_ST200_CACHE_H
#define _ASM_ST200_CACHE_H

/* L1 cache line size */
#define L1_CACHE_SHIFT	5
#define L1_CACHE_BYTES	(1 << L1_CACHE_SHIFT)

#define L1_CACHE_SHIFT_MAX L1_CACHE_SHIFT	/* largest L1 which this arch supports */

#endif /* _ASM_ST200_CACHE_H */
