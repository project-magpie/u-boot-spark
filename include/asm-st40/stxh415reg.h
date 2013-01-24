/*
 * Copyright (C) STMicroelectronics Ltd. 2008,2010-2013
 *
 * All rights reserved.
 */

/*
 * This is derived from STMicroelectronics gnu toolchain example:
 *   sh-superh-elf/examples/bare/sh4reg/sti7105.h
 */


#ifndef __ASM_ST40_STXH415REG_H
#define __ASM_ST40_STXH415REG_H

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

/* Clock Generator (CLOCKGENB) */
#ifndef STXH415_CKGB_REGS_BASE
#define STXH415_CKGB_REGS_BASE			0xfe81c000
#endif

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

/*
 * Does the FSM SPI Serial Flash Controller support 32-bit addresses on this SoC?
 */
#if !defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
#define CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES	/* yes, it is supported */
#endif

/*----------------------------------------------------------------------------*/

#include "st40reg.h"

/*----------------------------------------------------------------------------*/


/* Clock Generator "B" control registers */
#define STXH415_CKGB_LOCK		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x10)
#define STXH415_CKGB_FS0_CTRL		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x14)
#define STXH415_CKGB_FS0_MD0		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x18)
#define STXH415_CKGB_FS0_PE0		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x1c)
#define STXH415_CKGB_FS0_EN_PRG0	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x20)
#define STXH415_CKGB_FS0_SDIV0		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x24)
#define STXH415_CKGB_FS0_MD1		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x28)
#define STXH415_CKGB_FS0_PE1		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x2c)
#define STXH415_CKGB_FS0_EN_PRG1	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x30)
#define STXH415_CKGB_FS0_SDIV1		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x34)
#define STXH415_CKGB_FS0_MD2		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x38)
#define STXH415_CKGB_FS0_PE2		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x3c)
#define STXH415_CKGB_FS0_EN_PRG2	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x40)
#define STXH415_CKGB_FS0_SDIV2		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x44)
#define STXH415_CKGB_FS0_MD3		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x48)
#define STXH415_CKGB_FS0_PE3		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x4c)
#define STXH415_CKGB_FS0_EN_PRG3	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x50)
#define STXH415_CKGB_FS0_SDIV3		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x54)
#define STXH415_CKGB_FS0_CLKOUT_CTRL	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x58)
#define STXH415_CKGB_FS1_CTRL		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x5c)
#define STXH415_CKGB_FS1_MD0		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x60)
#define STXH415_CKGB_FS1_PE0		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x64)
#define STXH415_CKGB_FS1_EN_PRG0	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x68)
#define STXH415_CKGB_FS1_SDIV0		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x6c)
#define STXH415_CKGB_FS1_MD1		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x70)
#define STXH415_CKGB_FS1_PE1		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x74)
#define STXH415_CKGB_FS1_EN_PRG1	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x78)
#define STXH415_CKGB_FS1_SDIV1		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x7c)
#define STXH415_CKGB_FS1_MD2		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x80)
#define STXH415_CKGB_FS1_PE2		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x84)
#define STXH415_CKGB_FS1_EN_PRG2	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x88)
#define STXH415_CKGB_FS1_SDIV2		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x8c)
#define STXH415_CKGB_FS1_MD3		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x90)
#define STXH415_CKGB_FS1_PE3		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x94)
#define STXH415_CKGB_FS1_EN_PRG3	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x98)
#define STXH415_CKGB_FS1_SDIV3		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0x9c)
#define STXH415_CKGB_FS1_CLKOUT_CTRL	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0xa0)
#define STXH415_CKGB_FS_CFG0		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0xa4)
#define STXH415_CKGB_FS_CFG1		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0xa8)
#define STXH415_CKGB_FS_CFG2		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0xac)
#define STXH415_CKGB_CLOCK_ENABLE	SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0xb0)
#define STXH415_CKGB_OUT_CTRL		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0xb4)
#define STXH415_CKGB_REFCLK_SEL		SH4_DWORD_REG(STXH415_CKGB_REGS_BASE + 0xb8)


/*----------------------------------------------------------------------------*/


/*
 * STxP70 registers to configure the ARM boot zone (ABAP through xP70)
 */
#define STXH415_LPM_STXP70_BASE			STXH415_SBC_LPM_BASE
#define STXH415_LPM_STXP70_IMEM_BASE		SH4_DWORD_REG(STXH415_LPM_STXP70_BASE + 0x00018000)
#define STXH415_LPM_STXP70_DMEM_BASE		SH4_DWORD_REG(STXH415_LPM_STXP70_BASE + 0x00010000)
#define STXH415_LPM_STXP70_DMEM_04		SH4_DWORD_REG((unsigned int)(STXH415_LPM_STXP70_DMEM_BASE) + 0x04)
#define STXH415_LPM_STXP70_CONFIG_REGS_BASE	SH4_DWORD_REG(STXH415_LPM_STXP70_BASE + 0x000b5100)


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

#endif /* __ASM_ST40_STXH415REG_H */
