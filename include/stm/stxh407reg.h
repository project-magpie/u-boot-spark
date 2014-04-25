/*
 * Copyright (C) STMicroelectronics Ltd. 2008,2010-2013
 *
 * All rights reserved.
 */

/*
 * This is derived from STMicroelectronics gnu toolchain example:
 *   sh-superh-elf/examples/bare/sh4reg/sti7105.h
 */


#ifndef __INCLUDE_STM_STXH407REG_H
#define __INCLUDE_STM_STXH407REG_H

#include <stm/regtype.h>

/*----------------------------------------------------------------------------*/

/*
 * Base addresses for control register banks.
 */

/* STxH407 control registers */
#ifndef STXH407_COMMS_BASE
#define STXH407_COMMS_BASE			0x09800000
#endif

/* EMISS (aka EMIPCISS) Register Configuration Space */
//QQQ #define STM_EMISS_REGS_BASE			0xfef00000

/* Flash Interface, Control Registers (port 0) */
#ifndef STXH407_FLASH_IF_REG0_BASE
#define STXH407_FLASH_IF_REG0_BASE		0x09020000
#endif

/* Flash Interface, Control Registers (port 1) */
#ifndef STXH407_FLASH_IF_REG1_BASE
#define STXH407_FLASH_IF_REG1_BASE		0x09060000
#endif

/* Low-Power Block */
#ifndef STXH407_SBC_LPM_BASE
#define STXH407_SBC_LPM_BASE			0x09400000
#endif

/* ARM Cortex-A9 Configuration Registers */
#ifndef STM_A9_CONFIG_BASE
#define STM_A9_CONFIG_BASE			0x08760000
#endif

/*----------------------------------------------------------------------------*/

/* Recommended STBus Bridge Values for GMAC */
#ifndef STM_GMAC_AHB2STBUS_BASE
#define STM_GMAC_AHB2STBUS_BASE			0x2000		/* offset */
#endif
#ifndef STM_GMAC_AHB2STBUS_CONFIG
#define STM_GMAC_AHB2STBUS_CONFIG		0x26c209	/* from validation */
#endif

/*----------------------------------------------------------------------------*/

/* ASC UARTs */
/* in the main "COMMs" block */
#ifndef STXH407_ASC0_BASE
#define STXH407_ASC0_BASE (STXH407_COMMS_BASE + 0x00030000)
#endif
#ifndef STXH407_ASC1_BASE
#define STXH407_ASC1_BASE (STXH407_COMMS_BASE + 0x00031000)
#endif
#ifndef STXH407_ASC2_BASE
#define STXH407_ASC2_BASE (STXH407_COMMS_BASE + 0x00032000)
#endif
#ifndef STXH407_ASC3_BASE
#define STXH407_ASC3_BASE (STXH407_COMMS_BASE + 0x00033000)
#endif
#ifndef STXH407_ASC4_BASE
#define STXH407_ASC4_BASE (STXH407_COMMS_BASE + 0x00034000)
#endif
/* in the "SBC" block */
#ifndef STXH407_SBC_ASC0_BASE
#define STXH407_SBC_ASC0_BASE (STXH407_SBC_LPM_BASE + 0x00130000)
#endif
#ifndef STXH407_SBC_ASC1_BASE
#define STXH407_SBC_ASC1_BASE (STXH407_SBC_LPM_BASE + 0x00131000)
#endif


/* SSCs */
/* in the main "COMMs" block */
#ifndef STXH407_SSC0_BASE
#define STXH407_SSC0_BASE (STXH407_COMMS_BASE + 0x00040000)
#endif
#ifndef STXH407_SSC1_BASE
#define STXH407_SSC1_BASE (STXH407_COMMS_BASE + 0x00041000)
#endif
#ifndef STXH407_SSC2_BASE
#define STXH407_SSC2_BASE (STXH407_COMMS_BASE + 0x00042000)
#endif
#ifndef STXH407_SSC3_BASE
#define STXH407_SSC3_BASE (STXH407_COMMS_BASE + 0x00043000)
#endif
#ifndef STXH407_SSC4_BASE
#define STXH407_SSC4_BASE (STXH407_COMMS_BASE + 0x00044000)
#endif
#ifndef STXH407_SSC5_BASE
#define STXH407_SSC5_BASE (STXH407_COMMS_BASE + 0x00045000)
#endif
#ifndef STXH407_SSC6_BASE
#define STXH407_SSC6_BASE (STXH407_COMMS_BASE + 0x00046000)
#endif
#ifndef STXH407_SSC7_BASE
#define STXH407_SSC7_BASE (STXH407_COMMS_BASE + 0x00047000)
#endif
/* in the "SBC" block */
#ifndef STXH407_SBC_SSC0_BASE
#define STXH407_SBC_SSC0_BASE (STXH407_SBC_LPM_BASE + 0x00140000)
#endif
#ifndef STXH407_SBC_SSC1_BASE
#define STXH407_SBC_SSC1_BASE (STXH407_SBC_LPM_BASE + 0x00141000)
#endif
#ifndef STXH407_SBC_SSC2_BASE
#define STXH407_SBC_SSC2_BASE (STXH407_SBC_LPM_BASE + 0x00142000)
#endif

/*----------------------------------------------------------------------------*/

/* PIOs */
#ifndef STXH407_PIO_SBC_BASE
#define STXH407_PIO_SBC_BASE		0x09610000 /* 0-5 */
#endif
#ifndef STXH407_PIO_FRONT0_BASE
#define STXH407_PIO_FRONT0_BASE		0x09200000 /* 10-19 */
#endif
#ifndef STXH407_PIO_FRONT1_BASE
#define STXH407_PIO_FRONT1_BASE		0x09210000 /* 20 */
#endif
#ifndef STXH407_PIO_REAR_BASE
#define STXH407_PIO_REAR_BASE		0x09220000 /* 30-35 */
#endif
#ifndef STXH407_PIO_FLASH_BASE
#define STXH407_PIO_FLASH_BASE		0x09230000 /* 40-42 */
#endif
#define STM_PIO_BASE(x)							\
	(								\
		((x) < 6)						\
		? (STXH407_PIO_SBC_BASE +(0x1000*((x)-0)))		\
		:							\
		((x) < 20)						\
		? (STXH407_PIO_FRONT0_BASE +(0x1000*((x)-10)))		\
		:							\
		((x) < 21)						\
		? (STXH407_PIO_FRONT1_BASE +(0x1000*((x)-20)))		\
		:							\
		((x) < 36)						\
		? (STXH407_PIO_REAR_BASE+(0x1000*((x)-30)))		\
		:							\
		(STXH407_PIO_FLASH_BASE+(0x1000*((x)-40)))		\
	)

/*----------------------------------------------------------------------------*/

/* Configuration/Status Registers */
#ifndef STXH407_SYSCONF0_BASE
#define STXH407_SYSCONF0_BASE		0x09620000 /* 0-999 */
#endif
#ifndef STXH407_SYSCONF1_BASE
#define STXH407_SYSCONF1_BASE		0x09280000 /* 1000-1999 */
#endif
#ifndef STXH407_SYSCONF2_BASE
#define STXH407_SYSCONF2_BASE		0x09290000 /* 2000-2999 */
#endif
#ifndef STXH407_SYSCONF3_BASE
#define STXH407_SYSCONF3_BASE		0x092a0000 /* 3000-3999 */
#endif
#ifndef STXH407_SYSCONF4_BASE
#define STXH407_SYSCONF4_BASE		0x09600000 /* 4000-4999 */
#endif
#ifndef STXH407_SYSCONF5_BASE
#define STXH407_SYSCONF5_BASE		0x092b0000 /* 5000-5999 */
#endif
#ifndef STXH407_SYSCONF6_BASE
#define STXH407_SYSCONF6_BASE		0x092c0000 /* 6000-6999 */
#endif


/*
 * STxH407 System Configuration "accessors"
 */
#if defined(__ASSEMBLER__)
#define STXH407_SYSCFG(x)	STXH407_SYSCFG x
	.macro STXH407_SYSCFG x:req
		.if ((\x) < 1000)
			.long (STXH407_SYSCONF0_BASE + ((\x)-0)*0x4)
		.elseif ((\x) < 2000)
			.long (STXH407_SYSCONF1_BASE + ((\x)-1000)*0x4)
		.elseif ((\x) < 3000)
			.long (STXH407_SYSCONF2_BASE + ((\x)-2000)*0x4)
		.elseif ((\x) < 4000)
			.long (STXH407_SYSCONF3_BASE + ((\x)-3000)*0x4)
		.elseif ((\x) < 5000)
			.long (STXH407_SYSCONF4_BASE + ((\x)-4000)*0x4)
		.elseif ((\x) < 6000)
			.long (STXH407_SYSCONF5_BASE + ((\x)-5000)*0x4)
		.else
			.long (STXH407_SYSCONF6_BASE + ((\x)-6000)*0x4)
		.endif
	.endm
#else	/* __ASSEMBLER__ */
#define STXH407_SYSCFG(x)							\
	(									\
		((x) < 1000)							\
		? STM_U32_REG(STXH407_SYSCONF0_BASE + ((x)-0)*0x4)		\
		:								\
		((x) < 2000)							\
		? STM_U32_REG(STXH407_SYSCONF1_BASE + ((x)-1000)*0x4)		\
		:								\
		((x) < 3000)							\
		? STM_U32_REG(STXH407_SYSCONF2_BASE + ((x)-2000)*0x4)		\
		:								\
		((x) < 4000)							\
		? STM_U32_REG(STXH407_SYSCONF3_BASE + ((x)-3000)*0x4)		\
		:								\
		((x) < 5000)							\
		? STM_U32_REG(STXH407_SYSCONF4_BASE + ((x)-4000)*0x4)		\
		:								\
		((x) < 6000)							\
		? STM_U32_REG(STXH407_SYSCONF5_BASE + ((x)-5000)*0x4)		\
		:								\
		STM_U32_REG(STXH407_SYSCONF6_BASE + ((x)-6000)*0x4)		\
	)
#endif	/* __ASSEMBLER__ */

/*
 * STxH407 System Status "accessors"
 */
#define STXH407_SYSSTS(x)	STXH407_SYSCFG(x)


/*----------------------------------------------------------------------------*/

/*
 * Does the FSM SPI Serial Flash Controller support 32-bit addresses on this SoC?
 */
#if !defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
#define CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES	/* yes, it is supported */
#endif

/*
 * The STxH407 does not have a traditional "banked" EMI.
 */
#if !defined(CONFIG_CMD_BDI_DUMP_EMI_BANKS)
#define CONFIG_CMD_BDI_DUMP_EMI_BANKS		0	/* do not dump EMI banks */
#endif

/*----------------------------------------------------------------------------*/

/*
 * Pulse-Width Modulator in SBC.
 */
#ifndef STXH407_SBC_PWM_BASE
#define STXH407_SBC_PWM_BASE		(STXH407_SBC_LPM_BASE + 0x00110000)
#endif

#ifndef STXH407_SBC_PWM1_REG
#define STXH407_SBC_PWM1_REG		(STXH407_SBC_PWM_BASE + 0x04)
#endif

#define STXH407_PWM1_VOLTS(vcore)	((1135740-823*(vcore))/1000)	/* 1070=>255, 1380=>0 */

/*----------------------------------------------------------------------------*/

#include <stm/stxxxxxreg.h>

/*----------------------------------------------------------------------------*/


/*
 * Device ID register & bitfields
 */

#define STXH407_SYSCONF_DEVICEID	STXH407_SYSSTS(5568)

/* Device ID values, masks & predicates */
#define STXH407_DEVID_VAL		0x050
#define STXH407_DEVID_ID_SHIFT		12
#define STXH407_DEVID_ID_MASK		0x3ff
#define STXH407_DEVID_CUT_SHIFT		28
#define STXH407_DEVID_CUT_MASK		0xf

#define STXH407_DEVICEID_407(ID)	((((ID) >> STXH407_DEVID_ID_SHIFT) & STXH407_DEVID_ID_MASK) == STXH407_DEVID_VAL)
#define STXH407_DEVICEID_CUT(ID)	((((ID) >> STXH407_DEVID_CUT_SHIFT) & STXH407_DEVID_CUT_MASK) + 1)

#endif /* __INCLUDE_STM_STXH407REG_H */
