/*
 * Copyright (C) STMicroelectronics Ltd. 2008,2010-2012
 *
 * All rights reserved.
 */

/*
 * This is derived from STMicroelectronics gnu toolchain example:
 *   sh-superh-elf/examples/bare/sh4reg/sti7105.h
 */


#ifndef __ASM_ST40_STXH205REG_H
#define __ASM_ST40_STXH205REG_H

#include "sh4regtype.h"

/*----------------------------------------------------------------------------*/

/*
 * Base addresses for control register banks.
 */

/* Generic SH4 control registers */
#ifndef SH4_TMU_REGS_BASE
#define SH4_TMU_REGS_BASE		0xffd80000
#endif

/* Common ST40 control registers */
#ifndef ST40_CPG_REGS_BASE
#define ST40_CPG_REGS_BASE		0xffc00000		/* QQQ - TO CHECK */
#endif

/* STxH205 control registers */
#ifndef STXH205_COMMS_BASE
#define STXH205_COMMS_BASE		0xfd700000
#endif

/* System Architecture Volume 2: Bus Interfaces */
#ifndef ST40_EMI_REGS_BASE
#define ST40_EMI_REGS_BASE		0xfe900000
#endif

#ifndef ST40_EMI_SIZE			/* total addressable size of EMI */
#define ST40_EMI_SIZE			(256 << 20)	/* 256 MiB */
#endif

/* EMISS (aka EMIPCISS) Register Configuration Space */
#define ST40_EMISS_REGS_BASE		0xfdaa8000

/* Low-Power Block */
#define STXH205_LPM_COMMS_BASE		0xfe500000

/*----------------------------------------------------------------------------*/

/* ASC UARTs */
/* in the main "COMMs" block */
#ifndef STXH205_ASC0_BASE
#define STXH205_ASC0_BASE		(STXH205_COMMS_BASE + 0x00030000)
#endif
#ifndef STXH205_ASC1_BASE
#define STXH205_ASC1_BASE		(STXH205_COMMS_BASE + 0x00031000)
#endif
#ifndef STXH205_ASC2_BASE
#define STXH205_ASC2_BASE		(STXH205_COMMS_BASE + 0x00032000)
#endif
/* in the "SBC" block */
#ifndef STXH205_ASC10_BASE	/* ASC10 / UART #10 in the SBC */
#define STXH205_ASC10_BASE		(STXH205_LPM_COMMS_BASE + 0x00030000)
#endif
#ifndef STXH205_ASC11_BASE	/* ASC11 / UART #11 in the SBC */
#define STXH205_ASC11_BASE		(STXH205_LPM_COMMS_BASE + 0x00031000)
#endif

/*----------------------------------------------------------------------------*/

/* SSCs */
/* in the main "COMMs" block */
#ifndef STXH205_SSC0_BASE
#define STXH205_SSC0_BASE		(STXH205_COMMS_BASE + 0x00040000)
#endif
#ifndef STXH205_SSC1_BASE
#define STXH205_SSC1_BASE		(STXH205_COMMS_BASE + 0x00041000)
#endif
#ifndef STXH205_SSC2_BASE
#define STXH205_SSC2_BASE		(STXH205_COMMS_BASE + 0x00042000)
#endif
#ifndef STXH205_SSC3_BASE
#define STXH205_SSC3_BASE		(STXH205_COMMS_BASE + 0x00043000)
#endif
/* in the "SBC" block */
#ifndef STXH205_SSC10_BASE	/* SSC #10 in the SBC */
#define STXH205_SSC10_BASE		(STXH205_LPM_COMMS_BASE + 0x00040000)
#endif
#ifndef STXH205_SSC11_BASE	/* SSC #11 in the SBC */
#define STXH205_SSC11_BASE		(STXH205_LPM_COMMS_BASE + 0x00041000)
#endif
#ifndef STXH205_SSC12_BASE	/* SSC #12 in the SBC */
#define STXH205_SSC12_BASE		(STXH205_LPM_COMMS_BASE + 0x00042000)
#endif

/*----------------------------------------------------------------------------*/

/* PIOs */
#ifndef STXH205_PIO_SBC_BASE
#define STXH205_PIO_SBC_BASE		0xfe610000 /* 0-3 */
#endif
#ifndef STXH205_PIO_BANK_1_0_BASE
#define STXH205_PIO_BANK_1_0_BASE	0xfda60000 /* 4-8 */
#endif
#ifndef STXH205_PIO_BANK_1_1_BASE
#define STXH205_PIO_BANK_1_1_BASE	0xfda70000 /* 9-12 */
#endif
#ifndef STXH205_PIO_BANK_2_BASE
#define STXH205_PIO_BANK_2_BASE		0xfde20000 /* 13-15 */
#endif
#define ST40_PIO_BASE(x)						\
	(								\
		((x) < 4)						\
		? (STXH205_PIO_SBC_BASE +(0x1000*((x)-0)))		\
		:							\
		((x) < 9)						\
		? (STXH205_PIO_BANK_1_0_BASE +(0x1000*((x)-4)))		\
		:							\
		((x) < 13)						\
		? (STXH205_PIO_BANK_1_1_BASE+(0x1000*((x)-9)))		\
		:							\
		(STXH205_PIO_BANK_2_BASE+(0x1000*((x)-13)))		\
	)

/*----------------------------------------------------------------------------*/

/* Configuration/Status Registers */
#ifndef STXH205_SYSCONF_BANK0_BASE
#define STXH205_SYSCONF_BANK0_BASE	0xfe600000	/* 0-42 in the SBC (Standby Controller) */
#endif
#ifndef STXH205_SYSCONF_BANK1_BASE
#define STXH205_SYSCONF_BANK1_BASE	0xfda50000	/* 100-176 */
#endif
#ifndef STXH205_SYSCONF_BANK2_BASE
#define STXH205_SYSCONF_BANK2_BASE	0xfd500000	/* 200-243 */
#endif
#ifndef STXH205_SYSCONF_BANK3_BASE
#define STXH205_SYSCONF_BANK3_BASE	0xfd541000	/* 400-510 */
#endif

/*
 * STxH205 System Configuration "accessors"
 *
 * Note: there are many "holes" in the memory map, however the address of each
 * register is always a simple linear computation from the register number!
 */
#define STXH205_SYSCFG(x)							\
	(									\
		((x) < 100)							\
		? SH4_DWORD_REG(STXH205_SYSCONF_BANK0_BASE + ((x)-0)*0x4)	\
		:								\
		((x) < 200)							\
		? SH4_DWORD_REG(STXH205_SYSCONF_BANK1_BASE + ((x)-100)*0x4)	\
		:								\
		((x) < 400)							\
		? SH4_DWORD_REG(STXH205_SYSCONF_BANK2_BASE + ((x)-200)*0x4)	\
		:								\
		SH4_DWORD_REG(STXH205_SYSCONF_BANK3_BASE + ((x)-400)*0x4)	\
	)

/*
 * STxH205 System Status "accessors"
 */
#define STXH205_SYSSTS(x)	(STXH205_SYSCFG(x))


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
 * Device ID register & bitfields
 */

#define STXH205_SYSCONF_DEVICEID_0	STXH205_SYSSTS(41)

/* Device ID values, masks & predicates */
#define STXH205_DEVID_H205_VAL		0x04A
#define STXH205_DEVID_ID_SHIFT		12
#define STXH205_DEVID_ID_MASK		0x3ff	/* 10 bits */
#define STXH205_DEVID_CUT_SHIFT		28
#define STXH205_DEVID_CUT_MASK		0xf	/* 4 bits */

#define STXH205_DEVICEID_H205(ID) ((((ID) >> STXH205_DEVID_ID_SHIFT) & STXH205_DEVID_ID_MASK) == STXH205_DEVID_H205_VAL)
#define STXH205_DEVICEID_CUT(ID)  ((((ID) >> STXH205_DEVID_CUT_SHIFT) & STXH205_DEVID_CUT_MASK) + 1)

#endif /* __ASM_ST40_STXH205REG_H */
