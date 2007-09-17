/*
 * include/asm-sh4/hardware.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004, STMicroelectronics
 */

#ifndef __ASM_SH4_HARDWARE_H
#define __ASM_SH4_HARDWARE_H

#ifdef __ASSEMBLY__
#define SH4_REG(TYPE,ADDRESS) ADDRESS
#else
#define SH4_REG(TYPE,ADDRESS) TYPE(ADDRESS)
#endif

#ifdef CONFIG_CPU_SUBTYPE_SH4_2XX
#include "sh4_202.h"
#elif defined(CONFIG_SH_STI5528)
#include "sti5528.h"
#elif defined(CONFIG_SH_STM8000)
#include "stm8000.h"
#elif defined(CONFIG_SH_ST40RA166)
#include "st40ra166.h"
#endif

/*  SH4 CCN */
/* Core control registers (common to all SH4 variants) */

#define CCN_PTEH SH4_REG((volatile unsigned int *),0xff000000)
#define CCN_PTEL SH4_REG((volatile unsigned int *),0xff000004)
#define CCN_TTB SH4_REG((volatile unsigned int *),0xff000008)
#define CCN_TEA SH4_REG((volatile unsigned int *),0xff00000c)
#define CCN_MMUCR SH4_REG((volatile unsigned int *),0xff000010)
#define CCN_BASRA SH4_REG((volatile unsigned char *),0xff000014)
#define CCN_BASRB SH4_REG((volatile unsigned char *),0xff000018)
#define CCN_CCR SH4_REG((volatile unsigned int *),0xff00001c)
#define CCN_TRA SH4_REG((volatile unsigned int *),0xff000020)
#define CCN_EXPEVT SH4_REG((volatile unsigned int *),0xff000024)
#define CCN_INTEVT SH4_REG((volatile unsigned int *),0xff000028)
#define CCN_PTEA SH4_REG((volatile unsigned int *),0xff000034)
#define CCN_QACR0 SH4_REG((volatile unsigned int *),0xff000038)
#define CCN_QACR1 SH4_REG((volatile unsigned int *),0xff00003c)
#define CCN_PVR SH4_REG((volatile unsigned int *),0xff000030)
#define CCN_CVR SH4_REG((volatile unsigned int *),0xff000040)
#define CCN_PRR SH4_REG((volatile unsigned int *),0xff000044)

/* Some bits in the CCN.CCR register */
#define CCR_OCE (1<<0)
#define CCR_WT (1<<1)
#define CCR_CB (1<<2)
#define CCR_OCI (1<<3)
#define CCR_ORA (1<<5)
#define CCR_ICE (1<<8)
#define CCR_ICI (1<<11)
#define CCR_EMODE (1<<31)

/****/

/*  SH4 UBC */
/* User Break Controller control registers (common to all SH4) */

#if 0

#define UBC_BARA SH4_REG((volatile unsigned int *),0xff200000)
#define UBC_BAMRA SH4_REG((volatile unsigned char *),0xff200004)
#define UBC_BBRA SH4_REG((volatile unsigned short *),0xff200008)
#define UBC_BASRA SH4_REG((volatile unsigned char *),0xff000014)
#define UBC_BARB SH4_REG((volatile unsigned int *),0xff20000c)
#define UBC_BAMRB SH4_REG((volatile unsigned char *),0xff200010)
#define UBC_BBRB SH4_REG((volatile unsigned short *),0xff200014)
#define UBC_BASRB SH4_REG((volatile unsigned char *),0xff000018)
#define UBC_BDRB SH4_REG((volatile unsigned int *),0xff200018)
#define UBC_BDMRB SH4_REG((volatile unsigned int *),0xff20001c)
#define UBC_BRCR SH4_REG((volatile unsigned short *),0xff200020)

#endif

/****/

/*  SH4 UDI */
/* User Debug Interface control registers (common to all SH4) */

#define UDI_SDIR SH4_REG((volatile unsigned short *),0xfff00000)
#define UDI_SDDR SH4_REG((volatile unsigned int *),0xfff00008)
#define UDI_SDDRH SH4_REG((volatile unsigned short *),0xfff00008)
#define UDI_SDDRL SH4_REG((volatile unsigned short *),0xfff0000a)
#define UDI_SDINT SH4_REG((volatile unsigned short *),0xfff00014)

/****/

/*  SH4 AUD */
/* Advanced User Debugger control registers (common to all SH4) */

#define AUD_AUCSR SH4_REG((volatile unsigned short *),0xff2000cc)
#define AUD_AUWASR SH4_REG((volatile unsigned int *),0xff2000d0)
#define AUD_AUWAER SH4_REG((volatile unsigned int *),0xff2000d4)
#define AUD_AUWBSR SH4_REG((volatile unsigned int *),0xff2000d8)
#define AUD_AUWBER SH4_REG((volatile unsigned int *),0xff2000dc)

/****/

/* Generic SH4 control registers */

/*  SH4 TMU */
/* Timer Unit control registers (common to all SH4 variants) */

#define TMU_TOCR SH4_REG((volatile unsigned char *),(TMU_BASE_ADDRESS+0x00))
#define TMU_TSTR SH4_REG((volatile unsigned char *),(TMU_BASE_ADDRESS+0x04))
#define TMU_TCOR0 SH4_REG((volatile unsigned int *),(TMU_BASE_ADDRESS+0x08))
#define TMU_TCNT0 SH4_REG((volatile unsigned int *),(TMU_BASE_ADDRESS+0x0c))
#define TMU_TCR0 SH4_REG((volatile unsigned short *),(TMU_BASE_ADDRESS+0x10))
#define TMU_TCOR1 SH4_REG((volatile unsigned int *),(TMU_BASE_ADDRESS+0x14))
#define TMU_TCNT1 SH4_REG((volatile unsigned int *),(TMU_BASE_ADDRESS+0x18))
#define TMU_TCR1 SH4_REG((volatile unsigned short *),(TMU_BASE_ADDRESS+0x1c))
#define TMU_TCOR2 SH4_REG((volatile unsigned int *),(TMU_BASE_ADDRESS+0x20))
#define TMU_TCNT2 SH4_REG((volatile unsigned int *),(TMU_BASE_ADDRESS+0x24))
#define TMU_TCR2 SH4_REG((volatile unsigned short *),(TMU_BASE_ADDRESS+0x28))
#define TMU_TCPR2 SH4_REG((volatile unsigned int *),(TMU_BASE_ADDRESS+0x2c))

/****/

/*  SH4 RTC */
/* Real Time Clock control registers (common to all SH4 variants) */

#define RTC_R64CNT SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x00))
#define RTC_RSECCNT SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x04))
#define RTC_RMINCNT SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x08))
#define RTC_RHRCNT SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x0c))
#define RTC_RWKCNT SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x10))
#define RTC_RDAYCNT SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x14))
#define RTC_RMONCNT SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x18))
#define RTC_RYRCNT SH4_REG((volatile unsigned short *),(RTC_BASE_ADDRESS+0x1c))
#define RTC_RSECAR SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x20))
#define RTC_RMINAR SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x24))
#define RTC_RHRAR SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x28))
#define RTC_RWKAR SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x2c))
#define RTC_RDAYAR SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x30))
#define RTC_RMONAR SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x34))
#define RTC_RCR1 SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x38))
#define RTC_RCR2 SH4_REG((volatile unsigned char *),(RTC_BASE_ADDRESS+0x3c))

/****/

#endif
