/*
 * Copyright (C) STMicroelectronics Ltd. 2008,2010-2013
 *
 * All rights reserved.
 */

/*
 * This is derived from STMicroelectronics gnu toolchain example:
 *   sh-superh-elf/examples/bare/sh4reg/sti7105.h
 */


#ifndef __ASM_ST40_STXH416REG_H
#define __ASM_ST40_STXH416REG_H

#include "sh4regtype.h"

/*----------------------------------------------------------------------------*/

/*
 * Base addresses for control register banks.
 */

/* Generic SH4 control registers */
#ifndef SH4_TMU_REGS_BASE
#define SH4_TMU_REGS_BASE			0xffd80000		/* QQQ - TO CHECK */
#endif

/* Common ST40 control registers */
#ifndef ST40_CPG_REGS_BASE
#define ST40_CPG_REGS_BASE			0xffc00000		/* QQQ - TO CHECK */
#endif

/* STxH416 control registers */
#ifndef STXH416_COMMS_BASE
#define STXH416_COMMS_BASE			0xfed00000
#endif

/* System Architecture Volume 2: Bus Interfaces */
#ifndef ST40_EMI_REGS_BASE
#define ST40_EMI_REGS_BASE			0xfe900000
#endif

#ifndef ST40_EMI_SIZE				/* total addressable size of EMI */
#define ST40_EMI_SIZE				(256 << 20)	/* 256 MiB */
#endif

/* EMISS (aka EMIPCISS) Register Configuration Space */
#define ST40_EMISS_REGS_BASE			0xfef00000

/* Low-Power Block */
#define STXH416_SBC_LPM_BASE			0xfe400000

/*----------------------------------------------------------------------------*/

/* ASC UARTs */
/* in the main "COMMs" block */
#ifndef STXH416_ASC0_BASE
#define STXH416_ASC0_BASE (STXH416_COMMS_BASE + 0x00030000)
#endif
#ifndef STXH416_ASC1_BASE
#define STXH416_ASC1_BASE (STXH416_COMMS_BASE + 0x00031000)
#endif
#ifndef STXH416_ASC2_BASE
#define STXH416_ASC2_BASE (STXH416_COMMS_BASE + 0x00032000)
#endif
#ifndef STXH416_ASC3_BASE
#define STXH416_ASC3_BASE (STXH416_COMMS_BASE + 0x00033000)
#endif
#ifndef STXH416_ASC4_BASE
#define STXH416_ASC4_BASE (STXH416_COMMS_BASE + 0x00034000)
#endif
/* in the "SBC" block */
#ifndef STXH416_SBC_ASC0_BASE
#define STXH416_SBC_ASC0_BASE (STXH416_SBC_LPM_BASE + 0x00130000)
#endif
#ifndef STXH416_SBC_ASC1_BASE
#define STXH416_SBC_ASC1_BASE (STXH416_SBC_LPM_BASE + 0x00131000)
#endif


/* SSCs */
/* in the main "COMMs" block */
#ifndef STXH416_SSC0_BASE
#define STXH416_SSC0_BASE (STXH416_COMMS_BASE + 0x00040000)
#endif
#ifndef STXH416_SSC1_BASE
#define STXH416_SSC1_BASE (STXH416_COMMS_BASE + 0x00041000)
#endif
#ifndef STXH416_SSC2_BASE
#define STXH416_SSC2_BASE (STXH416_COMMS_BASE + 0x00042000)
#endif
#ifndef STXH416_SSC3_BASE
#define STXH416_SSC3_BASE (STXH416_COMMS_BASE + 0x00043000)
#endif
#ifndef STXH416_SSC4_BASE
#define STXH416_SSC4_BASE (STXH416_COMMS_BASE + 0x00044000)
#endif
#ifndef STXH416_SSC5_BASE
#define STXH416_SSC5_BASE (STXH416_COMMS_BASE + 0x00045000)
#endif
#ifndef STXH416_SSC6_BASE
#define STXH416_SSC6_BASE (STXH416_COMMS_BASE + 0x00046000)
#endif
#ifndef STXH416_SSC7_BASE
#define STXH416_SSC7_BASE (STXH416_COMMS_BASE + 0x00047000)
#endif
/* in the "SBC" block */
#ifndef STXH416_SBC_SSC0_BASE
#define STXH416_SBC_SSC0_BASE (STXH416_SBC_LPM_BASE + 0x00140000)
#endif
#ifndef STXH416_SBC_SSC1_BASE
#define STXH416_SBC_SSC1_BASE (STXH416_SBC_LPM_BASE + 0x00141000)
#endif
#ifndef STXH416_SBC_SSC2_BASE
#define STXH416_SBC_SSC2_BASE (STXH416_SBC_LPM_BASE + 0x00142000)
#endif

/*----------------------------------------------------------------------------*/

/* PIOs */
#ifndef STXH416_PIO_SAS_SBC_BASE
#define STXH416_PIO_SAS_SBC_BASE		0xfe610000 /* 0-4 */
#endif
#ifndef STXH416_PIO_SAS_FRONT_BASE
#define STXH416_PIO_SAS_FRONT_BASE		0xfee00000 /* 5-12 */
#endif
#ifndef STXH416_PIO_SAS_REAR_BASE
#define STXH416_PIO_SAS_REAR_BASE		0xfe820000 /* 13-18 */
#endif
#ifndef STXH416_PIO_MPE_RIGHT_BASE
#define STXH416_PIO_MPE_RIGHT_BASE		0xfd6b0000 /* 100-102 */
#endif
#ifndef STXH416_PIO_MPE_LEFT_BASE
#define STXH416_PIO_MPE_LEFT_BASE		0xfd330000 /* 103-107 */
#endif
#define ST40_PIO_BASE(x)						\
	(								\
		((x) < 5)						\
		? (STXH416_PIO_SAS_SBC_BASE +(0x1000*((x)-0)))		\
		:							\
		((x) == 40)						\
		? (STXH416_PIO_SAS_SBC_BASE +(0x5000))			\
		:							\
		((x) < 13)						\
		? (STXH416_PIO_SAS_FRONT_BASE +(0x1000*((x)-5)))	\
		:							\
		(((x) ==30) || ((x)==31))				\
		? (STXH416_PIO_SAS_FRONT_BASE +(0x8000*((x)-29)))	\
		:							\
		((x) < 100)						\
		? (STXH416_PIO_SAS_REAR_BASE+(0x1000*((x)-13)))		\
		:							\
		((x) < 103)						\
		? (STXH416_PIO_MPE_RIGHT_BASE+(0x1000*((x)-100)))	\
		:							\
		(STXH416_PIO_MPE_LEFT_BASE+(0x1000*((x)-103)))		\
	)

/*----------------------------------------------------------------------------*/

/* Configuration/Status Registers */
#ifndef STXH416_SYSCONF0_BASE
#define STXH416_SYSCONF0_BASE		0xfe600000 /* 0-999 */
#endif
#ifndef STXH416_SYSCONF1_BASE
#define STXH416_SYSCONF1_BASE		0xfee10000 /* 1000-1999 */
#endif
#ifndef STXH416_SYSCONF2_BASE
#define STXH416_SYSCONF2_BASE		0xfe830000 /* 2000-2999 */
#endif
#ifndef STXH416_SYSCONF3_BASE
#define STXH416_SYSCONF3_BASE		0xfddf0000 /* 5000-5999 */
#endif
#ifndef STXH416_SYSCONF4_BASE
#define STXH416_SYSCONF4_BASE		0xfd6a0000 /* 6000-6999 */
#endif
#ifndef STXH416_SYSCONF5_BASE
#define STXH416_SYSCONF5_BASE		0xfdde0000 /* 7500-7999 */
#endif
#ifndef STXH416_SYSCONF6_BASE
#define STXH416_SYSCONF6_BASE		0xfd320000 /* 8500-8999 */
#endif
#ifndef STXH416_SYSCONF7_BASE
#define STXH416_SYSCONF7_BASE		0xfd690000 /* 9500-9999 */
#endif


/*
 * STxH416 System Configuration "accessors"
 */
#define STXH416_SYSCFG(x)							\
	(									\
		((x) < 1000)							\
		? SH4_DWORD_REG(STXH416_SYSCONF0_BASE + ((x)-0)*0x4)		\
		:								\
		((x) < 2000)							\
		? SH4_DWORD_REG(STXH416_SYSCONF1_BASE + ((x)-1000)*0x4)		\
		:								\
		((x) < 5000)							\
		? SH4_DWORD_REG(STXH416_SYSCONF2_BASE + ((x)-2000)*0x4)		\
		:								\
		((x) < 6000)							\
		? SH4_DWORD_REG(STXH416_SYSCONF3_BASE + ((x)-5000)*0x4)		\
		:								\
		((x) < 7500)							\
		? SH4_DWORD_REG(STXH416_SYSCONF4_BASE + ((x)-6000)*0x4)		\
		:								\
		((x) < 8500)							\
		? SH4_DWORD_REG(STXH416_SYSCONF5_BASE + 0x7D0 + ((x)-7500)*0x4) \
		:								\
		((x) < 9500)							\
		? SH4_DWORD_REG(STXH416_SYSCONF6_BASE + 0x7D0 + ((x)-8500)*0x4) \
		:								\
		SH4_DWORD_REG(STXH416_SYSCONF7_BASE + 0x7D0 + ((x)-9500)*0x4)	\
	)

/*
 * STxH416 System Status "accessors"
 */
#define STXH416_SYSSTS(x)	(STXH416_SYSCFG(x))


/*----------------------------------------------------------------------------*/

/*
 * Does the FSM SPI Serial Flash Controller support 32-bit addresses on this SoC?
 */
#if !defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
#define CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES	/* yes, it is supported */
#endif

/*----------------------------------------------------------------------------*/

#include "st40reg.h"

/*----------------------------------------------------------------------------*/


/*
 * STxP70 registers to configure the ARM boot zone (ABAP through xP70)
 */
#define STXH416_LPM_STXP70_BASE			STXH416_SBC_LPM_BASE
#define STXH416_LPM_STXP70_IMEM_BASE		SH4_DWORD_REG(STXH416_LPM_STXP70_BASE + 0x00018000)
#define STXH416_LPM_STXP70_DMEM_BASE		SH4_DWORD_REG(STXH416_LPM_STXP70_BASE + 0x00010000)
#define STXH416_LPM_STXP70_DMEM_04		SH4_DWORD_REG((unsigned int)(STXH416_LPM_STXP70_DMEM_BASE) + 0x04)
#define STXH416_LPM_STXP70_CONFIG_REGS_BASE	SH4_DWORD_REG(STXH416_LPM_STXP70_BASE + 0x000b5100)


/*----------------------------------------------------------------------------*/


/*
 * ARM reset vectors mirrored in the ST40 address map
 */

#define STXH416_A9_RESET_VECTOR_MIRROR_BASE	SH4_DWORD_REG(0xFD4BF000)
#define STXH416_A9_RESET_VECTOR_MIRROR_BOOTADDR	SH4_DWORD_REG((unsigned int)(STXH416_A9_RESET_VECTOR_MIRROR_BASE)+0x20)


/*----------------------------------------------------------------------------*/

/*
 * Device ID register & bitfields
 */

#define STXH416_SYSCONF_DEVICEID	STXH416_SYSSTS(9516)

/* Device ID values, masks & predicates */
#define STXH416_DEVID_VAL		0x04D
#define STXH416_DEVID_ID_SHIFT		12
#define STXH416_DEVID_ID_MASK		0x3ff
#define STXH416_DEVID_CUT_SHIFT		28
#define STXH416_DEVID_CUT_MASK		0xf

#define STXH416_DEVICEID_416(ID)	((((ID) >> STXH416_DEVID_ID_SHIFT) & STXH416_DEVID_ID_MASK) == STXH416_DEVID_VAL)
#define STXH416_DEVICEID_CUT(ID)	((((ID) >> STXH416_DEVID_CUT_SHIFT) & STXH416_DEVID_CUT_MASK) + 1)

#endif /* __ASM_ST40_STXH416REG_H */
