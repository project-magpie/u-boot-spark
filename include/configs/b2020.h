/*
 * (C) Copyright 2008-2013 STMicroelectronics.
 *
 * Sean McGoogan <Sean.McGoogan@st.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_ST40	1		/* This is an SH4 CPU		*/
#define CONFIG_CPU_SUBTYPE_SH4_3XX	/* it is an SH4-300		*/


/*-----------------------------------------------------------------------
 *	Switch settings to select between the SoC's main boot-modes:
 *		1) boot from 8-bit NAND, 2KiB page, long address, Hamming controller
 *		2) boot from SPI serial flash
 *
 *	Jumper	NAND	SPI
 *	------	----	---
 *	n/a	10100	11010		MODE[6:2]
 *	n/a	0x14	0x1A		MODE[6:2]
 *	J13	2-3	2-3		MODE[6]
 *	J22	1-2	2-3		MODE[5]
 *	J16	2-3	1-2		MODE[4]
 *	J32	1-2	2-3		MODE[3]
 *	J37	1-2	1-2		MODE[2]
 */


/*-----------------------------------------------------------------------
 * Do we allow the (primary) ST40 core to boot ARM images on the (secondary) ARM core ?
 * If so, then define the "CONFIG_SYS_BOOT_ARM_IMAGE" macro,
 * otherwise (ST40-only booting), do not define it.
 */
#define CONFIG_SYS_BOOT_ARM_IMAGE		/* define to allow ARM-bootable image */


/*-----------------------------------------------------------------------
 * Are we booting directly from a NAND Flash device ?
 * If so, then define the "CONFIG_SYS_BOOT_FROM_NAND" macro,
 * otherwise (e.g. SPI Flash booting), do not define it.
 * Note: this will be implicitly defined if "make b2020se_nand_config" is run.
 */
//#define CONFIG_SYS_BOOT_FROM_NAND		/* define to build a NAND-bootable image */


/*-----------------------------------------------------------------------
 * Are we booting directly from a SPI Serial Flash device ?
 * If so, then define the "CONFIG_SYS_BOOT_FROM_SPI" macro,
 * otherwise (e.g. for NAND Flash booting), do not define it.
 * Note: this will be implicitly defined if "make b2020se_spi_config" is run.
 */
//#define CONFIG_SYS_BOOT_FROM_SPI		/* define to build a SPI-bootable image */


/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * Assume we run out of uncached memory for the moment
 */

#if defined(CONFIG_SYS_BOOT_FROM_SPI)			/* we are booting from SPI */
#	define CONFIG_SYS_EMI_SPI_BASE		0xB0000000	/* CSA: SPI Flash,  Physical 0x00000000 (4MiB) */
#	define CONFIG_SYS_NAND_FLEX_CSn_MAP	{ 0 }		/* NAND is on Chip Select CSA */
#elif defined(CONFIG_SYS_BOOT_FROM_NAND)		/* we are booting from NAND */
#	define CONFIG_SYS_NAND_FLEX_CSn_MAP	{ 0 }		/* NAND is on Chip Select CSA */
#else
#	error Either CONFIG_SYS_BOOT_FROM_SPI or CONFIG_SYS_BOOT_FROM_NAND must be defined!
#endif /* CONFIG_SYS_BOOT_FROM_SPI || CONFIG_SYS_BOOT_FROM_NAND */
#if defined(CONFIG_SYS_BOOT_FROM_SPI) && defined(CONFIG_SYS_BOOT_FROM_NAND)
#	error Only one of CONFIG_SYS_BOOT_FROM_SPI or CONFIG_SYS_BOOT_FROM_NAND must be defined!
#endif /* CONFIG_SYS_BOOT_FROM_SPI && CONFIG_SYS_BOOT_FROM_NAND */

#ifdef CONFIG_ST40_SE_MODE
#if defined(CONFIG_ST40_CONTIG_MODE)
#define CONFIG_SYS_SE_PHYSICAL_BASE	0x40000000	/* LMI Physical Address */
#else
#define CONFIG_SYS_SE_PHYSICAL_BASE	0x40000000	/* LMI Physical Address */
#endif	/* CONFIG_ST40_CONTIG_MODE */
#define CONFIG_SYS_SDRAM_BASE		0x80000000	/* LMI Cached addr via PMB */
#else
#error This SoC is not supported in 29-bit mode, please enable SE-mode!
#endif

#define CONFIG_SYS_SDRAM_SIZE		0x10000000	/* 256 MiB of LMI SDRAM */

#define CONFIG_SYS_MONITOR_LEN		0x00080000	/* Reserve 512 KiB for Monitor */
#define CONFIG_SYS_MALLOC_LEN		(4 << 20)	/* Reserve 4 MiB for malloc */
#define CONFIG_SYS_GBL_DATA_SIZE	1024		/* Global data structures */

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(TEXT_BASE - CONFIG_SYS_MALLOC_LEN - (1 << 20))

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define XSTR(s) STR(s)
#define STR(s) #s

#define BOARD b2020

#define CONFIG_EXTRA_ENV_SETTINGS \
		"board=" XSTR(BOARD) "\0" \
		"load_addr=" XSTR(CONFIG_SYS_LOAD_ADDR) "\0"

/*--------------------------------------------------------------
 * Command line configuration.
 */

#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_NFS
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII

/*--------------------------------------------------------------
 * Serial console info
 */

/*
 * We can use one of two methods for the "serial" console.
 * We can either use the (normal hardware) internal ST ASC UART;
 * OR we can use STMicroelectronics' DTF (Data Transfer Format)
 * mechanism over a JTAG link to a remote GDB debugger.
 */
#if 1
#	define CONFIG_STM_ASC_SERIAL	/* use a ST ASC UART */
#else
#	define CONFIG_STM_DTF_SERIAL	/* use DTF over JTAG */
#endif

/* choose which ST ASC UART to use */
#if defined(CONFIG_ST40_STXH415)
#if 0
#	define CONFIG_SYS_STM_ASC_BASE	STXH415_ASC2_BASE	/* UART2 on CN14 (COM0) */
#elif 1
#	define CONFIG_SYS_STM_ASC_BASE	STXH415_SBC_ASC1_BASE	/* SBC_UART1 on CN28 (COM1) */
#else
#	define CONFIG_SYS_STM_ASC_BASE	STXH415_SBC_ASC0_BASE	/* SBC_UART0 on CN19 */
#endif
#elif defined(CONFIG_ST40_STXH416)
#if 0
#	define CONFIG_SYS_STM_ASC_BASE	STXH416_ASC2_BASE	/* UART2 on CN14 (COM0) */
#elif 1
#	define CONFIG_SYS_STM_ASC_BASE	STXH416_SBC_ASC1_BASE	/* SBC_UART1 on CN28 (COM1) */
#else
#	define CONFIG_SYS_STM_ASC_BASE	STXH416_SBC_ASC0_BASE	/* SBC_UART0 on CN19 */
#endif
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */

/*---------------------------------------------------------------
 * Ethernet driver config
 */

/*
 * There are 2 on-chip ST-GMACs, however only one of them
 * (GMAC #1) is connected. This MAC is connected to a IC+
 * IP1001 on-board PHY (U77).
 */

/* are we using the internal ST GMAC device ? */
#define CONFIG_DRIVER_NET_STM_GMAC

/*
 * Select the appropriate base address for the GMAC.
 * Also, choose which PHY to use.
 */
#ifdef CONFIG_DRIVER_NET_STM_GMAC
#	define CONFIG_SYS_STM_STMAC0_BASE	0xfe810000ul	/* GMAC #0 */
#	define CONFIG_SYS_STM_STMAC1_BASE	0xfef08000ul	/* GMAC #1 */
#	define CONFIG_SYS_STM_STMAC_BASE	CONFIG_SYS_STM_STMAC1_BASE
#	define CONFIG_STMAC_IP1001		/* IC+ IP1001 (U77) */
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

/*  If this board does not have eeprom for ethernet address so allow the user
 *  to set it in the environment
 */
#define CONFIG_ENV_OVERWRITE


/*---------------------------------------------------------------
 * USB driver config
 */

/* Choose if we want USB Mass-Storage Support */
#define CONFIG_ST40_STM_USB

#ifdef CONFIG_ST40_STM_USB
#	define CONFIG_CMD_USB
#	define CONFIG_CMD_FAT
#	define CONFIG_USB_STORAGE
#	define CONFIG_SYS_USB0_BASE			0xfe100000	/* #0 is rear, next to E-SATA */
#	define CONFIG_SYS_USB1_BASE			0xfe200000	/* #1 is rear, next to HDMI */
#	define CONFIG_SYS_USB2_BASE			0xfe300000	/* #2 is front port */
#	define CONFIG_SYS_USB3_BASE			0xfe340000	/* #3 is another usb - QQQ to check! */
#	define CONFIG_SYS_USB_BASE			CONFIG_SYS_USB0_BASE
#	if 0	/* use OHCI (USB 1.x) ? */
#		define CONFIG_USB_OHCI_NEW				/* enable USB 1.x, via OHCI */
#		define CONFIG_SYS_USB_OHCI_CPU_INIT
#		define CONFIG_SYS_USB_OHCI_REGS_BASE		AHB2STBUS_OHCI_BASE
#		define CONFIG_SYS_USB_OHCI_SLOT_NAME		"ohci"
#		define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	1
#	else	/* else, use EHCI (USB 2.0) */
#		define CONFIG_USB_EHCI					/* enable USB 2.0, via EHCI */
#		define CONFIG_USB_EHCI_STM				/* use EHCI for STMicroelectronics */
#	endif	/* use OHCI/EHCI */
#endif	/* ifdef CONFIG_ST40_STM_USB */

/*---------------------------------------------------------------
 * SATA driver config
 */

/* SATA works on cut 3.x of the STx7105 (just one port) */
/* Choose if we want to use a SATA HDD */
//#define CONFIG_ST40_STM_SATA	/* QQQ - TO TEST SATA */

#ifdef CONFIG_ST40_STM_SATA
#	define CONFIG_CMD_IDE					/* enable "ide" command set */
#	define CONFIG_SYS_ATA_BASE_ADDR		0xfe209000	/* E-SATA panel connector */
#	define CONFIG_SYS_ATA_IDE0_OFFSET	0x800		/* Host Controller */
#	define CONFIG_SYS_ATA_REG_OFFSET	0x0
#	define CONFIG_SYS_ATA_DATA_OFFSET	0x0
#	define CONFIG_SYS_ATA_STRIDE		0x4
#	define CONFIG_SYS_IDE_MAXBUS		1
#	define CONFIG_SYS_IDE_MAXDEVICE		1
#endif	/* CONFIG_ST40_STM_SATA */

#if defined(CONFIG_ST40_STM_SATA) ||	\
    defined(CONFIG_ST40_STM_USB)
#	define CONFIG_SYS_64BIT_LBA
#	define CONFIG_LBA48
#	define CONFIG_DOS_PARTITION
#	define CONFIG_CMD_EXT2
#endif

/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */

#define CONFIG_SYS_HUSH_PARSER		1
#define CONFIG_AUTO_COMPLETE		1
#define CONFIG_SYS_LONGHELP		1			/* undef to save memory		*/
#define CONFIG_SYS_PROMPT		"B2020> "		/* Monitor Command Prompt	*/
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_HZ			1000			/* HZ for timer ticks	*/
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_SDRAM_BASE	/* default load address		*/
#define CONFIG_BOOTDELAY		10			/* default delay before executing bootcmd */
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_CMDLINE_EDITING

/*-----------------------------------------------------------------------
 * FLASH organization
 */

/*
 * Choose if we want FLASH Support (NAND and/or SPI devices).
 * Note: by default CONFIG_CMD_FLASH is defined in config_cmd_default.h
 */
#undef CONFIG_CMD_FLASH		/* undefine it, define only if needed */
#define CONFIG_CMD_NAND		/* define for NAND flash */
#define CONFIG_SPI_FLASH	/* define for SPI serial flash */

/*-----------------------------------------------------------------------
 * NOR FLASH organization
 */

#ifdef CONFIG_CMD_FLASH				/* NOR flash present ? */
	/* There is no support for parallel NOR flashes on this board. */
#else
#	undef CONFIG_CMD_IMLS			/* NOR-flash specific */
#	define CONFIG_SYS_NO_FLASH		/* NOR-flash specific */
#endif	/* CONFIG_CMD_FLASH */

/*-----------------------------------------------------------------------
 * NAND FLASH organization
 */

#ifdef CONFIG_CMD_NAND				/* NAND flash present ? */
#	define CONFIG_SYS_MAX_NAND_DEVICE	1
#	define CONFIG_SYS_NAND0_BASE		CONFIG_SYS_EMI_NAND_BASE
#	define CONFIG_SYS_NAND_BASE_LIST	{ CONFIG_SYS_NAND0_BASE }

#if defined(CONFIG_SYS_BOOT_FROM_NAND)	/* booting from NAND ? */
#	define MTDPARTS_NAND						\
	"gen_nand.1:"		/* First NAND flash device */		\
	"1M(uboot)"		/* reserve some space for uboot */	\
	",8M(kernel)"		/* kernel space */			\
	",-(rootfs)"		/* rootfs */
#else
#	define MTDPARTS_NAND						\
	"gen_nand.1:"		/* First NAND flash device */		\
	"8M(kernel)"		/* kernel space */			\
	",-(rootfs)"		/* rootfs */
#endif

#	define MTDIDS_NAND						\
	"nand0=gen_nand.1"	/* First NAND flash device */

	/*
	 * Enable this, if support for probing of ONFI complaint NAND
	 * devices is also required (typically recommended).
	 */
#	define CONFIG_SYS_NAND_ONFI_DETECTION	/* define for probing ONFI devices */

	/*
	 * With modern large NAND devices, we need to ensure that (if the
	 * environment is stored in NAND) that it is in its own dedicated "block".
	 * Hence, we need to define the erase size of the NAND device being used,
	 * so that the environment section always starts on an erase-block boundary,
	 * AND is a multiple of erase blocks.
	 * In practice for a NAND device with an erase size of >= 256KiB,
	 * a single dedicated block should always be sufficient!
	 */
#	define CONFIG_SYS_NAND_ERASE_SIZE	(512 << 10)	/* NAND erase block size is 512 KiB */

	/*
	 * Currently, there are 3 main modes to read/write from/to
	 * NAND devices on STM SoCs:
	 *	1) using a S/W "bit-banging" driver
	 *	   (can NOT be used with boot-from-NAND)
	 *	2) using the H/W Hamming controller (flex-mode) driver
	 *	   (also supports boot-from-NAND capability)
	 *	3) using the H/W BCH controller (multi-bit ECC) driver
	 *	   (also supports boot-from-NAND capability)
	 * Either CONFIG_SYS_ST40_NAND_USE_BIT_BANGING, or CONFIG_SYS_ST40_NAND_USE_HAMMING,
	 * or CONFIG_SYS_ST40_NAND_USE_BCH should be defined, to select a single NAND driver.
	 * If we are using FLEX or BCH, we still need to #define the
	 * address CONFIG_SYS_EMI_NAND_BASE, although the value is ignored!
	 */
//#	define CONFIG_SYS_ST40_NAND_USE_BIT_BANGING		/* use S/W "bit-banging" driver */
//#	define CONFIG_SYS_ST40_NAND_USE_HAMMING			/* use H/W Hamming ("flex") driver */
#	define CONFIG_SYS_ST40_NAND_USE_BCH			/* use H/W BCH ("multi-bit") driver */
#	define CONFIG_SYS_EMI_NAND_BASE		0xDEADBEEF	/* for FLEX/BCH, we do not care! */

	/*
	 * If using BCH, then we also need choose the "potency" of the ECC
	 * scheme to use. The BCH controller can correct up to a maximum of
	 * either 18-bits, or 30-bits, (both per 1024 byte sector).
	 * 18-bits of correction requires 32 bytes of OOB per 1KiB of data.
	 * 30-bits of correction requires 54 bytes of OOB per 1KiB of data.
	 * For BCH, please choose *only* ONE of the following ECC schemes.
	 */
//#	define CONFIG_SYS_ST40_NAND_USE_BCH_NO_ECC		/* use BCH with-OUT ECC -- not recommended! */
//#	define CONFIG_SYS_ST40_NAND_USE_BCH_18_BIT_ECC		/* use BCH with 18-bit/1KiB sector ECC */
#	define CONFIG_SYS_ST40_NAND_USE_BCH_30_BIT_ECC		/* use BCH with 30-bit/1KiB sector ECC */

	/*
	 * Do we want to read/write NAND Flash compatible with the ST40's
	 * NAND Hamming H/W IP block for "boot-mode"? If we want
	 * to read/write NAND flash that is meant to support booting
	 * from NAND, then we need to use 3 bytes of ECC per 128 byte
	 * record.  If so, then define the "CONFIG_SYS_NAND_ECC_HW3_128" macro.
	 * Note: do *not* define this if CONFIG_SYS_ST40_NAND_USE_BCH is defined,
	 * as the Hamming boot-mode ECC is different to that of the BCH.
	 */
#	define CONFIG_SYS_NAND_ECC_HW3_128	/* define for "boot-from-NAND" compatibility */
#	if defined(CONFIG_SYS_ST40_NAND_USE_BCH)
#	undef CONFIG_SYS_NAND_ECC_HW3_128	/* explicitly un-define if using BCH */
#	endif /* CONFIG_SYS_NAND_ECC_HW3_128 */

	/*
	 * Do we want to use STMicroelectronics' proprietary AFM4 (4+3/512)
	 * ECC format, instead of Linux's traditional S/W 3/256 ECC?
	 * Note: This does *not* enable H/W AFM - you can use either
	 * "bit-banging" or STM's "FLEX-mode", it is simply the addition
	 * of the AFM4 ECC algorithm+layout that is being supported here.
	 * Note: We *can* use this H/W AFM4 (4+3/512) ECC in addition to
	 * the H/W "boot-mode" (3+1/128) ECC, on the same NAND device,
	 * to partition it, set CONFIG_SYS_NAND_STM_BOOT_MODE_BOUNDARY appropriately.
	 */
#	undef CONFIG_SYS_NAND_ECC_AFM4		/* define for AFM4 (4+3/512) ECC compatibility */

	/*
	 * If using CONFIG_SYS_NAND_ECC_HW3_128, then we must also define
	 * where the (high watermark) boundary is. That is, the
	 * NAND offset, below which we are in "boot-mode", and
	 * must use 3 bytes of ECC for each 128 byte record.
	 * For this offset (and above) we can use any supported
	 * ECC configuration (e.g 3/256 S/W, or 3/512 H/W).
	 */
#	define CONFIG_SYS_NAND_STM_BOOT_MODE_BOUNDARY (1ul << 20)	/* 1 MiB */

	/*
	 * If we want to store the U-boot environment variables in
	 * the NAND device, then we also need to specify *where* the
	 * environment variables will be stored. Typically this
	 * would be immediately after the U-boot monitor itself.
	 * However, that *may* be a bad block. Define the following
	 * to place the environment in an appropriate good block.
	 */
#	define CONFIG_SYS_NAND_ENV_OFFSET	roundup(CONFIG_SYS_MONITOR_LEN, CONFIG_SYS_NAND_ERASE_SIZE)
#endif	/* CONFIG_CMD_NAND */

#if 0 && defined(CONFIG_SYS_BOOT_FROM_NAND)		/* we are booting from NAND */
	/*
	 * If we want to store "u-boot.bin" in NAND flash starting at
	 * physical block #0, but there are Bad Blocks in the first
	 * few blocks that we need to "skip" over, then we need
	 * to define CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING to allow
	 * skipping of these bad blocks for u-boot to relocate itself.
	 * In addition, we also need to tell U-boot the block size,
	 * and provide it a local abridged copy of the master Bad Block
	 * Table (BBT), which must also be stored in physical block #0
	 * - see "cpu/sh/start.S" for details.
	 * Also, CONFIG_SYS_NAND_SKIP_BLOCK_COUNT defines the number of blocks
	 * stored in the abridged copy of the master BBT.
	 */
#	define CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING	/* define for skipping */
#	define CONFIG_SYS_NAND_SKIP_BLOCK_SIZE		(128<<10)/* Block Size = 128 KiB */
#	define CONFIG_SYS_NAND_SKIP_BLOCK_COUNT		8	/* entries in the array */
#endif /* CONFIG_SYS_BOOT_FROM_NAND */

/*-----------------------------------------------------------------------
 * SPI SERIAL FLASH organization
 */

/*
 *	Name	Manuf		Device
 *	-----	-----		------
 *	U22	Spansion	S25FL129
 */
#if defined(CONFIG_SPI_FLASH)				/* SPI serial flash present ? */
#	define CONFIG_SPI_FLASH_SPANSION		/* Spansion S25FL129PIL */
#	define CONFIG_SPI				/* enable the SPI driver */
#	define CONFIG_CMD_EEPROM			/* enable the "eeprom" command set */
#	define CONFIG_SYS_I2C_FRAM			/* to minimize performance degradation */
#	undef  CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	/* to minimize performance degradation */

#if 1
	/* Use H/W FSM SPI Controller (not H/W SSC, nor S/W "bit-banging") */
#	define CONFIG_STM_FSM_SPI			/* Use the H/W FSM for SPI */
#	define CONFIG_SYS_STM_SPI_FSM_BASE	0xfe902000	/* FSM SPI Controller Base */
#	define CONFIG_SYS_STM_SPI_CLOCKDIV	4	/* set SPI_CLOCKDIV = 4 */
#	undef CONFIG_CMD_SPI				/* SPI serial bus command support - NOT with FSM! */
#else
	/* Use S/W "bit-banging", (not H/W FSM, nor H/W SSC) */
#	define CONFIG_SOFT_SPI			/* Use "bit-banging" PIO */
#if defined(CONFIG_ST40_STXH415)
#	define SPI_SCL(val)	do { stxh415_spi_scl((val)); } while (0)
#	define SPI_SDA(val)	do { stxh415_spi_sda((val)); } while (0)
#	define SPI_READ		stxh415_spi_read()
#elif defined(CONFIG_ST40_STXH416)
#	define SPI_SCL(val)	do { stxh416_spi_scl((val)); } while (0)
#	define SPI_SDA(val)	do { stxh416_spi_sda((val)); } while (0)
#	define SPI_READ		stxh416_spi_read()
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */
#	define SPI_DELAY	do { udelay(1); } while (0)	/* Note: only 500 kHz! */
#endif
#endif	/* CONFIG_SPI_FLASH */

/*-----------------------------------------------------------------------
 * Address, size, & location of U-boot's Environment Sector
 */

#define CONFIG_ENV_SIZE_REQUIRED	0x4000	/* 16 KiB of environment data required */

#if 1 && defined(CONFIG_SYS_BOOT_FROM_NAND) &&	/* booting from NAND ? */ \
	defined(CONFIG_CMD_NAND)		/* NAND flash present ? */
#	define CONFIG_ENV_IS_IN_NAND		/* environment in NAND flash */
#	define CONFIG_ENV_OFFSET	CONFIG_SYS_NAND_ENV_OFFSET
#	define CONFIG_ENV_SIZE		roundup(CONFIG_ENV_SIZE_REQUIRED,CONFIG_SYS_NAND_ERASE_SIZE)
#elif 1 && defined(CONFIG_SPI_FLASH)		/* SPI serial flash present ? */
#	define CONFIG_ENV_IS_IN_EEPROM		/* ENV is stored in SPI Serial Flash */
#	define CONFIG_ENV_OFFSET	CONFIG_SYS_MONITOR_LEN	/* immediately after u-boot.bin */
#	define CONFIG_ENV_SIZE		CONFIG_ENV_SIZE_REQUIRED
#else
#	define CONFIG_ENV_IS_NOWHERE		/* ENV is stored in volatile RAM */
#	undef  CONFIG_CMD_SAVEENV		/* no need for "saveenv" */
#endif	/* CONFIG_CMD_NAND */

/*----------------------------------------------------------------------
 * JFFS2 + MTD Partition support
 */

#if 0 && defined(CONFIG_CMD_NAND)
#	define CONFIG_CMD_JFFS2			/* enable JFFS2 support */
#	define CONFIG_CMD_UBI
#	define CONFIG_RBTREE
#	define CONFIG_MTD_PARTITIONS
#endif

#if defined(CONFIG_CMD_JFFS2)
#	define CONFIG_CMD_MTDPARTS		/* mtdparts command line support */
#	define CONFIG_MTD_DEVICE		/* needed for mtdparts commands */
#	define CONFIG_JFFS2_NAND		/* JFFS2 support on NAND Flash */
#	if defined(CONFIG_CMD_NAND)		/* Only NAND flash devices */
#		define MTDPARTS_DEFAULT	"mtdparts=" MTDPARTS_NAND
#		define MTDIDS_DEFAULT	MTDIDS_NAND
#	endif	/* defined(CONFIG_CMD_NAND) */
#endif	/* CONFIG_CMD_JFFS2 */


/*----------------------------------------------------------------------
 * I2C configuration
 */

#define CONFIG_CMD_I2C				/* do we want I2C support ? */

#if defined(CONFIG_CMD_I2C)
	/* Note: I2C Bus #1 "probes" devices when a suitable HDMI device is plugged in to CN5. */
#	define CONFIG_I2C_BUS		1	/* Use I2C Bus associated with SSC #1 */
#	define CONFIG_I2C_CMD_TREE		/* use a "i2c" root command */
#	define CONFIG_SYS_I2C_SLAVE	0x7F	/* I2C slave address - Bogus: master-only in U-Boot */
#	define CONFIG_SOFT_I2C			/* I2C with S/W bit-banging	*/
#	undef  CONFIG_HARD_I2C			/* I2C withOUT hardware support	*/
#	define I2C_ACTIVE			/* open-drain, nothing to do */
#	define I2C_TRISTATE			/* open-drain, nothing to do */
#if defined(CONFIG_ST40_STXH415)
#	define I2C_SCL(val)		do { stxh415_i2c_scl((val)); } while (0)
#	define I2C_SDA(val)		do { stxh415_i2c_sda((val)); } while (0)
#	define I2C_READ			stxh415_i2c_read()
#elif defined(CONFIG_ST40_STXH416)
#	define I2C_SCL(val)		do { stxh416_i2c_scl((val)); } while (0)
#	define I2C_SDA(val)		do { stxh416_i2c_sda((val)); } while (0)
#	define I2C_READ			stxh416_i2c_read()
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */

	/*
	 * The "BOGOS" for NDELAY() may be calibrated using the
	 * following code fragment, and measuring (using an oscilloscope)
	 * the frequency of the I2C SCL pin, and adjusting
	 * NDELAY_BOGOS, until the SCL is approximately 100 kHz.
	 * (100kHz has a period of 5us + 5us).
	 *
	 *	printf("just toggling I2C SCL (100kHz frequency) ...\n");
	 *	while (1)
	 *	{
	 *		I2C_SCL(1); NDELAY(5000);
	 *		I2C_SCL(0); NDELAY(5000);
	 *	}
	 */
#	define NDELAY_BOGOS		20	/* Empirical measurement for 1ns on B2020 */
#	define NDELAY(ns)						\
		do {							\
			const unsigned n_bogo = NDELAY_BOGOS;		\
			const unsigned n_ticks =			\
				((ns)<n_bogo) ? 1u : (ns)/n_bogo;	\
			volatile unsigned n_count;			\
			for(n_count=0; n_count<n_ticks; n_count++)	\
				;	/* do nothing */		\
		} while(0)

	/*
	 * Note there are 4 * I2C_DELAY per I2C clock cycle
	 * So, 100kHz requires an I2C delay of 2,500 ns,
	 * whereas, 400 kHz requires an I2C delay of 625 ns.
	 * However, this calculation only works if the S/W
	 * overhead in I2C bit-banging is negligible - which it is not!
	 * So, in practice, either I2C_DELAY or CONFIG_SYS_I2C_SPEED will be lower.
	 * The higher the clock frequency, the greater the difference.
	 * Empirical measurement/adjustment is recommended.
	 */
#	if 0	/* run the I2C bus clock at 400 kHz ? */
#		define CONFIG_SYS_I2C_SPEED	400000			/* I2C clock speed (400 kHz) */
#		define I2C_DELAY	do { NDELAY(625); } while (0)	/* 625 ns */
#	else	/* else, run the I2C bus clock at 100 kHz */
#		define CONFIG_SYS_I2C_SPEED	100000			/* I2C clock speed (100 kHz) */
#		define I2C_DELAY	do { NDELAY(625*4); } while (0)	/* 2,500 ns */
#	endif	/* 100kHz or 400kHz */

#endif	/* CONFIG_CMD_I2C */


/*----------------------------------------------------------------------
 * Support for Linux Hibernation-on-Memory (HoM)
 */

	/*
	 * Define the following only if we want to use the
	 * Linux Hibernation-on-Memory (HoM) capability.
	 */
#undef CONFIG_HIBERNATION_ON_MEMORY

	/*
	 * If we are using HoM, we need to tell U-Boot where the
	 * special "tag" is located in memory. This must be the
	 * same address as configured for the linux kernel.
	 */
#undef CONFIG_HOM_TAG_VIRTUAL_ADDRESS


#endif	/* __CONFIG_H */
