/*
 * Copyright 2009 Freescale Semiconductor, Inc.
 * Copyright 2013 STMicroelectronics Ltd.
 *	Sean McGoogan <Sean.McGoogan@st.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#ifndef _ASM_CONFIG_H_
#define _ASM_CONFIG_H_

/* common configuration for all STMicroelectronics' systems */
#if defined(CONFIG_STM)
#  include <stm/config.h>
#endif

#define CONFIG_LMB
#define CONFIG_SYS_BOOT_RAMDISK_HIGH

#ifdef CONFIG_ARM64
#define CONFIG_PHYS_64BIT
#define CONFIG_STATIC_RELA
#endif

#endif
