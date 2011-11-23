/*
 * Copyright (C) STMicroelectronics Ltd. 2008,2010-2011
 *
 * All rights reserved.
 */

/*
 * This is derived from STMicroelectronics gnu toolchain example:
 *   sh-superh-elf/examples/bare/sh4reg/sti7105.h
 */


#ifndef __STXH415REG_H
#define __STXH415REG_H

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

/* STxH415 control registers */
#ifndef STXH415_COMMS_BASE
#define STXH415_COMMS_BASE			0xfed00000
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
#define STXH415_SBC_LPM_BASE			0xfe400000

/*----------------------------------------------------------------------------*/

/* ASC UARTs */
/* in the main "COMMs" block */
#ifndef STXH415_ASC0_BASE
#define STXH415_ASC0_BASE (STXH415_COMMS_BASE + 0x00030000)
#endif
#ifndef STXH415_ASC1_BASE
#define STXH415_ASC1_BASE (STXH415_COMMS_BASE + 0x00031000)
#endif
#ifndef STXH415_ASC2_BASE
#define STXH415_ASC2_BASE (STXH415_COMMS_BASE + 0x00032000)
#endif
#ifndef STXH415_ASC3_BASE
#define STXH415_ASC3_BASE (STXH415_COMMS_BASE + 0x00033000)
#endif
#ifndef STXH415_ASC4_BASE
#define STXH415_ASC4_BASE (STXH415_COMMS_BASE + 0x00034000)
#endif
/* in the "SBC" block */
#ifndef STXH415_SBC_ASC0_BASE
#define STXH415_SBC_ASC0_BASE (STXH415_SBC_LPM_BASE + 0x00130000)
#endif
#ifndef STXH415_SBC_ASC1_BASE
#define STXH415_SBC_ASC1_BASE (STXH415_SBC_LPM_BASE + 0x00131000)
#endif


/* SSCs */
/* in the main "COMMs" block */
#ifndef STXH415_SSC0_BASE
#define STXH415_SSC0_BASE (STXH415_COMMS_BASE + 0x00040000)
#endif
#ifndef STXH415_SSC1_BASE
#define STXH415_SSC1_BASE (STXH415_COMMS_BASE + 0x00041000)
#endif
#ifndef STXH415_SSC2_BASE
#define STXH415_SSC2_BASE (STXH415_COMMS_BASE + 0x00042000)
#endif
#ifndef STXH415_SSC3_BASE
#define STXH415_SSC3_BASE (STXH415_COMMS_BASE + 0x00043000)
#endif
#ifndef STXH415_SSC4_BASE
#define STXH415_SSC4_BASE (STXH415_COMMS_BASE + 0x00044000)
#endif
#ifndef STXH415_SSC5_BASE
#define STXH415_SSC5_BASE (STXH415_COMMS_BASE + 0x00045000)
#endif
#ifndef STXH415_SSC6_BASE
#define STXH415_SSC6_BASE (STXH415_COMMS_BASE + 0x00046000)
#endif
/* in the "SBC" block */
#ifndef STXH415_SBC_SSC0_BASE
#define STXH415_SBC_SSC0_BASE (STXH415_SBC_LPM_BASE + 0x00140000)
#endif
#ifndef STXH415_SBC_SSC1_BASE
#define STXH415_SBC_SSC1_BASE (STXH415_SBC_LPM_BASE + 0x00141000)
#endif
#ifndef STXH415_SBC_SSC2_BASE
#define STXH415_SBC_SSC2_BASE (STXH415_SBC_LPM_BASE + 0x00142000)
#endif

/*----------------------------------------------------------------------------*/

/* PIOs */
#ifndef STXH415_PIO_SAS_SBC_BASE
#define STXH415_PIO_SAS_SBC_BASE		0xfe610000 /* 0-4 */
#endif
#ifndef STXH415_PIO_SAS_FRONT_BASE
#define STXH415_PIO_SAS_FRONT_BASE		0xfee00000 /* 5-12 */
#endif
#ifndef STXH415_PIO_SAS_REAR_BASE
#define STXH415_PIO_SAS_REAR_BASE		0xfe820000 /* 13-18 */
#endif
#ifndef STXH415_PIO_MPE_RIGHT_BASE
#define STXH415_PIO_MPE_RIGHT_BASE		0xfd6b0000 /* 100-102 */
#endif
#ifndef STXH415_PIO_MPE_LEFT_BASE
#define STXH415_PIO_MPE_LEFT_BASE		0xfd330000 /* 103-107 */
#endif
#define ST40_PIO_BASE(x)						\
	(								\
		((x) < 5)						\
		? (STXH415_PIO_SAS_SBC_BASE +(0x1000*((x)-0)))		\
		:							\
		((x) < 13)						\
		? (STXH415_PIO_SAS_FRONT_BASE +(0x1000*((x)-5)))	\
		:							\
		((x) < 100)						\
		? (STXH415_PIO_SAS_REAR_BASE+(0x1000*((x)-13)))		\
		:							\
		((x) < 103)						\
		? (STXH415_PIO_MPE_RIGHT_BASE+(0x1000*((x)-100)))	\
		:							\
		(STXH415_PIO_MPE_LEFT_BASE+(0x1000*((x)-103)))		\
	)

/*----------------------------------------------------------------------------*/

/* Configuration/Status Registers */
#ifndef STXH415_SAS_SBC_SYSCONF_BASE
#define STXH415_SAS_SBC_SYSCONF_BASE		0xfe600000 /* 0-99 */
#endif
#ifndef STXH415_SAS_FRONT_SYSCONF_BASE
#define STXH415_SAS_FRONT_SYSCONF_BASE		0xfee10000 /* 100-299 */
#endif
#ifndef STXH415_SAS_REAR_SYSCONF_BASE
#define STXH415_SAS_REAR_SYSCONF_BASE		0xfe830000 /* 300-399 */
#endif
#ifndef STXH415_MPE_LEFT_SYSCONF_BASE
#define STXH415_MPE_LEFT_SYSCONF_BASE		0xfd690000 /* 400-499 */
#endif
#ifndef STXH415_MPE_RIGHT_SYSCONF_BASE
#define STXH415_MPE_RIGHT_SYSCONF_BASE		0xfd320000 /* 500-599 */
#endif
#ifndef STXH415_MPE_SYSTEM_SYSCONF_BASE
#define STXH415_MPE_SYSTEM_SYSCONF_BASE		0xfdde0000 /* 600-699 */
#endif

/*
 * STxH415 System Configuration "accessors"
 */
#define STXH415_SYSCFG(x)							\
	(									\
		((x) < 100)							\
		? SH4_DWORD_REG(STXH415_SAS_SBC_SYSCONF_BASE + ((x)-0)*0x4)	\
		:								\
		((x) < 300)							\
		? SH4_DWORD_REG(STXH415_SAS_FRONT_SYSCONF_BASE + ((x)-100)*0x4)	\
		:								\
		((x) < 400)							\
		? SH4_DWORD_REG(STXH415_SAS_REAR_SYSCONF_BASE + ((x)-300)*0x4)	\
		:								\
		((x) < 500)							\
		? SH4_DWORD_REG(STXH415_MPE_LEFT_SYSCONF_BASE + ((x)-400)*0x4)	\
		:								\
		((x) < 600)							\
		? SH4_DWORD_REG(STXH415_MPE_RIGHT_SYSCONF_BASE + ((x)-500)*0x4)	\
		:								\
		SH4_DWORD_REG(STXH415_MPE_SYSTEM_SYSCONF_BASE + ((x)-600)*0x4)	\
	)

/*
 * STxH415 System Status "accessors"
 */
#define STXH415_SYSSTS(x)	(STXH415_SYSCFG(x))


/*----------------------------------------------------------------------------*/

#include "st40reg.h"

/*----------------------------------------------------------------------------*/

/*
 * Device ID register & bitfields
 */

#define STXH415_SYSCONF_DEVICEID	STXH415_SYSSTS(427)

/* Device ID values, masks & predicates */
#define STXH415_DEVID_VAL		0x049
#define STXH415_DEVID_ID_SHIFT		12
#define STXH415_DEVID_ID_MASK		0x3ff
#define STXH415_DEVID_CUT_SHIFT		28
#define STXH415_DEVID_CUT_MASK		0xf

#define STXH415_DEVICEID_415(ID)	((((ID) >> STXH415_DEVID_ID_SHIFT) & STXH415_DEVID_ID_MASK) == STXH415_DEVID_VAL)
#define STXH415_DEVICEID_CUT(ID)	((((ID) >> STXH415_DEVID_CUT_SHIFT) & STXH415_DEVID_CUT_MASK) + 1)

#endif /* __STXH415REG_H */
