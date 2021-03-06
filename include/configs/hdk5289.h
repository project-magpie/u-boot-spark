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

#define CONFIG_STM	1		/* The SoC is from STMicroelectronics */
#define CONFIG_ST40	1		/* This is an SH4 CPU		*/
#define CONFIG_CPU_SUBTYPE_SH4_3XX	/* it is an SH4-300		*/


/*-----------------------------------------------------------------------
 *	Switch settings to select between the SoC's main 3 boot-modes:
 *		a) boot from 16-bit NOR flash
 *		b) boot from 8-bit NAND flash, large-page, long address
 *		c) boot from SPI serial flash
 *
 *	Switch	NOR	NAND	SPI	Mode
 *	------	---	----	---	----
 *	JE1-2	X	ON	X	MODE[11]
 *	JE1-1	X	off	X	MODE[10]
 *
 *	JE2-2	ON	off	X	MODE[13]
 *	JE2-1	X	X	off	MODE[12]
 *
 *	JE3-2	ON	off	ON	MODE[16]
 *	JE3-1	ON	ON	off	MODE[15]
 *
 *	JH1	NORTH	SOUTH	SOUTH	Swap CSA and CSB
 *
 *	Note: "X" denotes don't care (i.e. either ON or OFF)
 *
 *	Note: "NORTH" and "SOUTH" denotes board orientation with LEDs facing user.
 */


/*-----------------------------------------------------------------------
 * Are we booting directly from a NAND Flash device ?
 * If so, then define the "CONFIG_SYS_BOOT_FROM_NAND" macro,
 * otherwise (e.g. NOR/SPI Flash booting), do not define it.
 */
#undef CONFIG_SYS_BOOT_FROM_NAND	/* define to build a NAND-bootable image */


/*-----------------------------------------------------------------------
 * Are we booting directly from a SPI Serial Flash device ?
 * If so, then define the "CONFIG_SYS_BOOT_FROM_SPI" macro,
 * otherwise (e.g. for NOR/NAND Flash booting), do not define it.
 */
#undef CONFIG_SYS_BOOT_FROM_SPI		/* define to build a SPI-bootable image */


/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * Assume we run out of uncached memory for the moment
 */

#if defined(CONFIG_SYS_BOOT_FROM_SPI)		/* we are booting from SPI, so *DO* swap CSA and CSB */
#define CONFIG_SYS_EMI_SPI_BASE		0xA0000000	/* CSA: SPI Flash,  Physical 0x00000000 (64MiB) */
#define CONFIG_SYS_EMI_NAND_BASE	0xA0000000	/* CSA: NAND Flash, Physical 0x00000000 (64MiB) */
#define CONFIG_SYS_EMI_NOR_BASE		0xA4000000	/* CSB: NOR Flash,  Physical 0x04000000 (8MiB) */
#define CONFIG_SYS_NAND_FLEX_CSn_MAP	{ 0 }		/* NAND is on Chip Select CSB */
#elif defined(CONFIG_SYS_BOOT_FROM_NAND)	/* we are booting from NAND, so *DO* swap CSA and CSB */
#define CONFIG_SYS_EMI_NAND_BASE	0xA0000000	/* CSA: NAND Flash, Physical 0x00000000 (64MiB) */
#define CONFIG_SYS_EMI_NOR_BASE		0xA4000000	/* CSB: NOR Flash,  Physical 0x04000000 (8MiB) */
#define CONFIG_SYS_NAND_FLEX_CSn_MAP	{ 0 }		/* NAND is on Chip Select CSA */
#else						/* else, do *NOT* swap CSA and CSB in EPLD */
#define CONFIG_SYS_EMI_NOR_BASE		0xA0000000	/* CSA: NOR Flash,  Physical 0x00000000 (64MiB) */
#define CONFIG_SYS_EMI_NAND_BASE	0xA4000000	/* CSB: NAND Flash, Physical 0x04000000 (8MiB) */
#define CONFIG_SYS_NAND_FLEX_CSn_MAP	{ 1 }		/* NAND is on Chip Select CSB */
#endif /* CONFIG_SYS_BOOT_FROM_NAND */

#ifdef CONFIG_ST40_SE_MODE
#define CONFIG_SYS_FLASH_BASE		CONFIG_SYS_EMI_NOR_BASE/* NOR FLASH (uncached) via PMB */
#define CONFIG_SYS_SE_PHYSICAL_BASE	0x40000000	/* LMI Physical Address */
#define CONFIG_SYS_SDRAM_BASE		0x80000000	/* LMI Cached addr via PMB */
#else
#define CONFIG_SYS_FLASH_BASE		CONFIG_SYS_EMI_NOR_BASE/* NOR FLASH in P2 region */
#define CONFIG_SYS_SDRAM_BASE		0x8C000000	/* SDRAM in P1 region */
#endif

	/* in 32-bit mode, default TargetPack's PMB setup is only 128 MiB of RAM! */
#if defined(CONFIG_ST40_SE_MODE) && (TEXT_BASE < 0x8FF00000)
#define CONFIG_SYS_SDRAM_SIZE		0x08000000	/* 128 MiB of LMI SDRAM */
#else
#define CONFIG_SYS_SDRAM_SIZE		0x08000000	/* 128 MiB of LMI SDRAM */
#endif

#define CONFIG_SYS_MONITOR_LEN		0x00040000	/* Reserve 256 KiB for Monitor */
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_MALLOC_LEN		(1 << 20)	/* Reserve 1 MiB for malloc */
#define CONFIG_SYS_GBL_DATA_SIZE	1024		/* Global data structures */

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(TEXT_BASE - CONFIG_SYS_MALLOC_LEN - (1 << 20))

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define XSTR(s) STR(s)
#define STR(s) #s

#define BOARD hdk5289

#if CONFIG_SYS_MONITOR_LEN == 0x00008000	/* 32 KiB */
#	define MONITOR_SECTORS	"1:0"		/* 1 sector */
#elif CONFIG_SYS_MONITOR_LEN == 0x00010000	/* 64 KiB */
#	define MONITOR_SECTORS	"1:0-1"		/* 2 sectors */
#elif CONFIG_SYS_MONITOR_LEN == 0x00018000	/* 96 KiB */
#	define MONITOR_SECTORS	"1:0-2"		/* 3 sectors */
#elif CONFIG_SYS_MONITOR_LEN == 0x00020000	/* 128 KiB */
#	define MONITOR_SECTORS	"1:0-3"		/* 4 sectors */
#elif CONFIG_SYS_MONITOR_LEN == 0x00040000	/* 256 KiB */
#	define MONITOR_SECTORS	"1:0-4"		/* 5 sectors */
#else						/* unknown */
#	error "Unable to determine sectors for monitor"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
		"board=" XSTR(BOARD) "\0" \
		"monitor_base=" XSTR(CONFIG_SYS_MONITOR_BASE) "\0" \
		"monitor_len=" XSTR(CONFIG_SYS_MONITOR_LEN) "\0" \
		"monitor_sec=" MONITOR_SECTORS "\0" \
		"load_addr=" XSTR(CONFIG_SYS_LOAD_ADDR) "\0" \
		"unprot=" \
		  "protect off $monitor_sec\0" \
		"update=" \
		  "erase $monitor_sec;" \
		  "cp.b $load_addr $monitor_base $monitor_len;" \
		  "protect on $monitor_sec\0"

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

/* UART #2, on JI3 to daughter board */
#define CONFIG_SYS_STM_ASC_BASE	STM_ASC2_REGS_BASE	/* UART2 = JI3 */

/*---------------------------------------------------------------
 * Ethernet driver config
 */

/*
 * There are 2 options for ethernet, all use the on-chip ST-GMAC.
 * The choice in PHYs are:
 *    The on-board Micrel KSZ8041 (UP1)
 *    External PHY connected via the MII off-board connector (JN8).
 */

/* are we using the internal ST GMAC device ? */
#define CONFIG_DRIVER_NET_STM_GMAC

/*
 * Select the appropriate base address for the GMAC.
 * Also, choose which PHY to use.
 */
#ifdef CONFIG_DRIVER_NET_STM_GMAC
#	define CONFIG_SYS_STM_STMAC_BASE	 0xfd110000ul	/* MAC = STM GMAC0 */
#if 1
#	define CONFIG_STMAC_KSZ8041				/* PHY = Micrel KSZ8041 */
#else
	/* QQQ define for JN8 ... */
#endif
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

/*  If this board does not have eeprom for ethernet address so allow the user
 *  to set it in the environment
 */
#define CONFIG_ENV_OVERWRITE


/*---------------------------------------------------------------
 * USB driver config
 */

/* Choose if we want USB Mass-Storage Support */
#define CONFIG_STM_USB

#ifdef CONFIG_STM_USB
#	define CONFIG_CMD_USB
#	define CONFIG_CMD_FAT
#	define CONFIG_USB_STORAGE
#	define CONFIG_SYS_USB_BASE			0xfe100000
#	if 0	/* use OHCI (USB 1.x) ? */
#		define CONFIG_USB_OHCI_NEW				/* enable USB 1.x, via OHCI */
#		define CONFIG_SYS_USB_OHCI_CPU_INIT
#		define CONFIG_SYS_USB_OHCI_REGS_BASE		AHB2STBUS_OHCI_BASE
#		define CONFIG_SYS_USB_OHCI_SLOT_NAME		"ohci"
#		define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	2	/* Note: TWO root ports */
#	else	/* else, use EHCI (USB 2.0) */
#		define CONFIG_USB_EHCI					/* enable USB 2.0, via EHCI */
#		define CONFIG_USB_EHCI_STM				/* use EHCI for STMicroelectronics */
#	endif	/* use OHCI/EHCI */
#endif	/* ifdef CONFIG_STM_USB */

#if defined(CONFIG_STM_USB)
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
#define CONFIG_SYS_PROMPT		"HDK5289> "		/* Monitor Command Prompt	*/
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_HZ			1000			/* HZ for timer ticks	*/
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_SDRAM_BASE	/* default load address		*/
#define CONFIG_BOOTDELAY		10			/* default delay before executing bootcmd */
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_CMDLINE_EDITING

/*-----------------------------------------------------------------------
 * FLASH organization
 */

/* Choose if we want FLASH Support (NAND, NOR & SPI devices),
 * all three, or none, or any other combination.
 *
 * Note: by default CONFIG_CMD_FLASH is defined in config_cmd_default.h
 */
#undef CONFIG_CMD_FLASH		/* undefine it, define only if needed */
#define CONFIG_CMD_FLASH	/* define for NOR flash */
#define CONFIG_CMD_NAND		/* define for NAND flash */
#define CONFIG_SPI_FLASH	/* define for SPI serial flash */

/*-----------------------------------------------------------------------
 * NOR FLASH organization
 */

/* M58LT256: 32MiB 259 blocks, 128 KiB block size */
#ifdef CONFIG_CMD_FLASH				/* NOR flash present ? */
#	define CONFIG_FLASH_CFI_DRIVER
#	define CONFIG_SYS_FLASH_CFI
#	define CONFIG_FLASH_PROTECT_SINGLE_CELL
#	define CONFIG_FLASH_SHOW_PROGRESS	45	/* count down from 45/5: 9..1		*/
#	define CONFIG_SYS_FLASH_PROTECTION	1	/* use hardware flash protection	*/
#	define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#	define CONFIG_SYS_MAX_FLASH_SECT	259	/* max number of sectors on one chip	*/
#	define CONFIG_SYS_FLASH_EMPTY_INFO		/* test if each sector is empty		*/
#	define MTDPARTS_NOR						\
	"physmap-flash:"	/* First NOR flash device */		\
		"256k(U-Boot)"		/* first partition */		\
		",128k(Environment)"					\
		",4M(Kernel)"						\
		",-(RestOfNor0)"	/* last partition */
#	define MTDIDS_NOR						\
	"nor0=physmap-flash"	/* First NOR flash device */
#else
#	undef CONFIG_CMD_IMLS			/* NOR-flash specific */
#	define CONFIG_SYS_NO_FLASH		/* NOR-flash specific */
#endif	/* CONFIG_CMD_FLASH */

/*-----------------------------------------------------------------------
 * NAND FLASH organization
 */

/* NAND512W3A: 64MiB  8-bit, 4096 Blocks (16KiB+512B) of 32 Pages (512+16) */
#ifdef CONFIG_CMD_NAND				/* NAND flash present ? */
#	define CONFIG_SYS_MAX_NAND_DEVICE	1
#	define CONFIG_SYS_NAND0_BASE		CONFIG_SYS_EMI_NAND_BASE
#	define CONFIG_SYS_NAND_BASE_LIST	{ CONFIG_SYS_NAND0_BASE }
#	define MTDPARTS_NAND						\
	"gen_nand.1:"		/* First NAND flash device */		\
		"128k(env-nand0)"	/* first partition */		\
		",4M(kernel-nand0)"					\
		",32M(root-nand0)"					\
		",-(RestOfNand0)"	/* last partition */
#	define MTDIDS_NAND						\
	"nand0=gen_nand.1"	/* First NAND flash device */

	/*
	 * Currently, there are 2 main modes to read/write from/to
	 * NAND devices on STM SoCs:
	 *	1) using a S/W "bit-banging" driver
	 *	   (can NOT be used with boot-from-NAND)
	 *	2) using the H/W Hamming controller (flex-mode) driver
	 *	   (only supported means for boot-from-NAND)
	 * Either CONFIG_SYS_STM_NAND_USE_BIT_BANGING or CONFIG_SYS_STM_NAND_USE_HAMMING
	 * should be defined, to select a single NAND driver.
	 * If we are using FLEX-mode, we still need to #define the
	 * address CONFIG_SYS_EMI_NAND_BASE, although the value is ignored!
	 */
//#	define CONFIG_SYS_STM_NAND_USE_BIT_BANGING	/* use S/W "bit-banging" driver */
#	define CONFIG_SYS_STM_NAND_USE_HAMMING		/* use H/W Hamming ("flex") driver */

	/*
	 * Do we want to read/write NAND Flash compatible with the ST40's
	 * NAND Controller H/W IP block for "boot-mode"? If we want
	 * to read/write NAND flash that is meant to support booting
	 * from NAND, then we need to use 3 bytes of ECC per 128 byte
	 * record.  If so, then define the "CONFIG_SYS_NAND_ECC_HW3_128" macro.
	 */
#	define CONFIG_SYS_NAND_ECC_HW3_128	/* define for "boot-from-NAND" compatibility */

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
#	define CONFIG_SYS_NAND_ENV_OFFSET (CONFIG_SYS_MONITOR_LEN + 0x0)	/* immediately after u-boot.bin */
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
	 * - see "arch/st40/cpu/start.S" for details.
	 * Also, CONFIG_SYS_NAND_SKIP_BLOCK_COUNT defines the number of blocks
	 * stored in the abridged copy of the master BBT.
	 */
#	define CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING	/* define for skipping */
#	define CONFIG_SYS_NAND_SKIP_BLOCK_SIZE		(16<<10)/* Block Size = 16 KiB */
#	define CONFIG_SYS_NAND_SKIP_BLOCK_COUNT		16	/* entries in the array */
#endif /* CONFIG_SYS_BOOT_FROM_NAND */

/*-----------------------------------------------------------------------
 * SPI SERIAL FLASH organization
 */

/*
 *	Name	Manuf	Device
 *	-----	-----	------
 *	UG1	Winbond	W25Q64V
 */
#if defined(CONFIG_SPI_FLASH)				/* SPI serial flash present ? */
#	define CONFIG_SPI_FLASH_WINBOND			/* Winbond W25Q64V (UG1) */
#	define CONFIG_SPI				/* enable the SPI driver */
#	define CONFIG_CMD_EEPROM			/* enable the "eeprom" command set */
#	define CONFIG_SYS_I2C_FRAM			/* to minimize performance degradation */
#	undef  CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	/* to minimize performance degradation */

	/* Can only use H/W FSM SPI Controller (not H/W SSC, nor S/W "bit-banging") */
#	define CONFIG_STM_FSM_SPI				/* Use the H/W FSM for SPI */
#	define CONFIG_SYS_STM_SPI_FSM_BASE	0xfe702000	/* FSM SPI Controller Base */
#	undef CONFIG_CMD_SPI					/* SPI serial bus command support - NOT with FSM! */
#endif	/* CONFIG_SPI_FLASH */

/*-----------------------------------------------------------------------
 * Address, size, & location of U-boot's Environment Sector
 */

#define CONFIG_ENV_SIZE			0x4000	/* 16 KiB of environment data */

#if 1 && defined(CONFIG_CMD_FLASH)		/* NOR flash present ? */
#	define CONFIG_ENV_IS_IN_FLASH		/* environment in NOR flash */
#	define CONFIG_ENV_OFFSET	CONFIG_SYS_MONITOR_LEN	/* immediately after u-boot.bin */
#	define CONFIG_ENV_SECT_SIZE	0x20000	/* 128 KiB Sector size */
#elif 1 && defined(CONFIG_CMD_NAND)		/* NAND flash present ? */
#	define CONFIG_ENV_IS_IN_NAND		/* environment in NAND flash */
#	define CONFIG_ENV_OFFSET	CONFIG_SYS_NAND_ENV_OFFSET
#	if CONFIG_ENV_SIZE < 0x20000		/* needs to be a multiple of block-size */
#		undef CONFIG_ENV_SIZE		/* give it just one large-page block */
#		define CONFIG_ENV_SIZE	0x20000	/* 128 KiB of environment data */
#	endif /* if CONFIG_ENV_SIZE < 0x20000 */
#elif 1 && defined(CONFIG_SPI_FLASH)		/* SPI serial flash present ? */
#	define CONFIG_ENV_IS_IN_EEPROM		/* ENV is stored in SPI Serial Flash */
#	define CONFIG_ENV_OFFSET	CONFIG_SYS_MONITOR_LEN	/* immediately after u-boot.bin */
#else
#	define CONFIG_ENV_IS_NOWHERE		/* ENV is stored in volatile RAM */
#	undef  CONFIG_CMD_SAVEENV		/* no need for "saveenv" */
#endif	/* CONFIG_CMD_NAND */

/*----------------------------------------------------------------------
 * JFFS2 + MTD Partition support
 */

#if 1 && (defined(CONFIG_CMD_FLASH) || defined(CONFIG_CMD_NAND))
#	define CONFIG_CMD_JFFS2			/* enable JFFS2 support */
#endif

#if defined(CONFIG_CMD_JFFS2)
#	define CONFIG_CMD_MTDPARTS		/* mtdparts command line support */
#	define CONFIG_MTD_DEVICE		/* needed for mtdparts commands */
#	define CONFIG_JFFS2_NAND		/* JFFS2 support on NAND Flash */
#	if defined(CONFIG_CMD_FLASH) && defined(CONFIG_CMD_NAND) /* Both NOR + NAND */
#		define MTDPARTS_DEFAULT						\
		"mtdparts="							\
			MTDPARTS_NOR	/* NOR flash devices */			\
			";"		/* delimiter */				\
			MTDPARTS_NAND	/* NAND flash devices */
#		define MTDIDS_DEFAULT						\
			MTDIDS_NOR	/* NOR flash devices */			\
			","		/* delimiter */				\
			MTDIDS_NAND	/* NAND flash devices */
#	elif defined(CONFIG_CMD_FLASH)		/* Only NOR flash devices */
#		define MTDPARTS_DEFAULT	"mtdparts=" MTDPARTS_NOR
#		define MTDIDS_DEFAULT	MTDIDS_NOR
#	elif defined(CONFIG_CMD_NAND)		/* Only NAND flash devices */
#		define MTDPARTS_DEFAULT	"mtdparts=" MTDPARTS_NAND
#		define MTDIDS_DEFAULT	MTDIDS_NAND
#	endif	/* defined(CONFIG_CMD_FLASH) && defined(CONFIG_CMD_NAND) */
#endif	/* CONFIG_CMD_JFFS2 */

#endif	/* __CONFIG_H */
