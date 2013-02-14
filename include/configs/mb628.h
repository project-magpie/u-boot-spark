/*
 * (C) Copyright 2008-2012 STMicroelectronics.
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
 * Start addresses for the final memory configuration
 * Assume we run out of uncached memory for the moment
 *
 * See board/mb628/config.mk for details of the memory map.
 */

#ifdef CONFIG_ST40_SE_MODE
#define CONFIG_SYS_FLASH_BASE		0xA0000000	/* FLASH (uncached) via PMB */
#define CONFIG_SYS_SE_PHYSICAL_BASE	0x40000000	/* LMI Physical Address */
#define CONFIG_SYS_SDRAM_BASE		0x80800000	/* LMI Cached addr via PMB */
#else
#define CONFIG_SYS_FLASH_BASE		0xA0000000	/* FLASH in P2 region */
#define CONFIG_SYS_SDRAM_BASE		0x8C800000	/* SDRAM in P1 region */
#endif

#define CONFIG_SYS_SDRAM_SIZE		0x03000000	/* 48 MiB of LMI SDRAM */

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

#define BOARD mb628

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

/* choose which ST ASC UART to use */
#if 1
	/* ASC1	(left-most)	"RS232 1" */
#	define CONFIG_SYS_STM_ASC_BASE	0xfd031000
#else
	/* ASC2	(right-most)	"RS232 2" */
#	define CONFIG_SYS_STM_ASC_BASE	0xfd032000
#endif

/*---------------------------------------------------------------
 * Ethernet driver config
 */

/*
 * There are 2 options for ethernet, both use the on-chip ST-GMAC.
 * The choice in PHYs are:
 *    The on-board SMSC LAN8700
 *    External PHY connected via the MII off-board connector.
 */

/* are we using the internal ST GMAC device ? */
//#define CONFIG_DRIVER_NET_STM_GMAC

/*
 * Select the appropriate base address for the GMAC.
 * Also, choose which PHY to use.
 */
#ifdef CONFIG_DRIVER_NET_STM_GMAC
#	define CONFIG_SYS_STM_STMAC_BASE	 0xfd110000ul	/* MAC = STM GMAC0 */
#	define CONFIG_STMAC_LAN8700				/* PHY = SMSC LAN8700 */
#else
#	undef CONFIG_CMD_NET		/* No networking! */
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
#	define CONFIG_SYS_USB0_BASE			0xfe100000 /* USB2.0-A CN12 upper of stack */
#	define CONFIG_SYS_USB1_BASE			0xfea00000 /* USB2.0-B CN21 */
//#	define CONFIG_SYS_USB2_BASE	/* Broken! */	0xfeb00000 /* USB1.1-A */
//#	define CONFIG_SYS_USB3_BASE	/* Broken! */	0xfec00000 /* USB1.1-B */
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

/* SATA works on cut 2.x of the STx7141 (just one port) */
/* Choose if we want to use a SATA HDD */
//#define CONFIG_ST40_STM_SATA

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
#define CONFIG_SYS_PROMPT		"MB628> "		/* Monitor Command Prompt	*/
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_HZ			1000			/* HZ for timer ticks	*/
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_SDRAM_BASE	/* default load address		*/
#define CONFIG_BOOTDELAY		10			/* default delay before executing bootcmd */
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_CMDLINE_EDITING

/*-----------------------------------------------------------------------
 * FLASH organization
 */

/* Whether the hardware supports NOR or NAND Flash depends on J70.
 * Only ONE of these may be present at any one time. Each hides the other.
 * In position 1-2 CSA selects NAND, in position 2-3 is selects NOR.
 * Note that J69 must also be in position 2-3 to select the
 * on-board Flash (for either the on-board NOR or NAND flash).
 *
 * i.e.		ON-board NOR FLASH:	J69:2-3, J70:2-3
 *		ON-board NAND FLASH:	J69:2-3, J70:1-2
 */
//#define CONFIG_CMD_NAND		/* define for NAND flash */

/*-----------------------------------------------------------------------
 * NOR FLASH organization
 */

/* M58LT256JSB: 32MiB 259 blocks, 128 KiB block size */
#ifndef CONFIG_CMD_NAND				/* NOR flash present ? */
#	define CONFIG_CMD_JFFS2			/* enable JFFS2 support */
#	define CONFIG_FLASH_CFI_DRIVER
#	define CONFIG_SYS_FLASH_CFI
#	define CONFIG_FLASH_PROTECT_SINGLE_CELL
#	define CONFIG_FLASH_SHOW_PROGRESS	45	/* count down from 45/5: 9..1		*/
#	define CONFIG_SYS_FLASH_PROTECTION	1	/* use hardware flash protection	*/
#	define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#	define CONFIG_SYS_MAX_FLASH_SECT	259	/* max number of sectors on one chip	*/
#	define CONFIG_SYS_FLASH_EMPTY_INFO		/* test if each sector is empty		*/
#define MTDPARTS_DEFAULT						\
	"mtdparts="							\
	"physmap-flash:"	/* First NOR flash device */		\
		"256k(U-Boot)"		/* first partition */		\
		",128k(Environment)"					\
		",4M(Kernel)"						\
		",-(RestOfNor)"		/* last partition */
#define MTDIDS_DEFAULT							\
	"nor0=physmap-flash"	/* First NOR flash device */
#endif	/* CONFIG_CMD_NAND */

/*-----------------------------------------------------------------------
 * NAND FLASH organization
 */

/* NAND512W3A: 64MiB  8-bit, 4096 Blocks (16KiB+512B) of 32 Pages (512+16) */
/* NAND512W4A: 64MiB 16-bit, 4096 Blocks (16KiB+512B) of 32 Pages (512+16) */
#ifdef CONFIG_CMD_NAND				/* NAND flash present ? */
#	define CONFIG_CMD_JFFS2			/* enable JFFS2 support */
#	define CONFIG_SYS_MAX_NAND_DEVICE	1
#	define CONFIG_SYS_NAND0_BASE		CONFIG_SYS_FLASH_BASE	/* Occludes NOR flash */
#	define CONFIG_SYS_NAND_BASE_LIST	{ CONFIG_SYS_NAND0_BASE }
#	undef CONFIG_CMD_FLASH			/* NOR-flash specific */
#	undef CONFIG_CMD_IMLS			/* NOR-flash specific */
#	define CONFIG_SYS_NO_FLASH		/* no NOR-flash when using NAND-flash */
#define MTDPARTS_DEFAULT						\
	"mtdparts="							\
	"gen_nand.1:"		/* First NAND flash device */		\
		"128k(Environment)"	/* first partition */		\
		",4M(Kernel)"						\
		",32M(rootfs)"						\
		",-(RestOfNand)"	/* last partition */
#define MTDIDS_DEFAULT							\
	"nand0=gen_nand.1"	/* First NAND flash device */
#endif	/* CONFIG_CMD_NAND */

/*-----------------------------------------------------------------------
 * Addresss, size, & location of U-boot's Environment Sector
 */

#if 1 && defined(CONFIG_CMD_NAND)		/* NAND flash present ? */
#	define CONFIG_ENV_IS_IN_NAND		/* enviroment in NAND flash */
#	define CONFIG_ENV_OFFSET	0	/* begining of NAND flash */
#elif 1 && defined(CONFIG_CMD_FLASH)
#	define CONFIG_ENV_IS_IN_FLASH		/* environment in NOR flash */
#	define CONFIG_ENV_OFFSET	CONFIG_SYS_MONITOR_LEN	/* After u-boot.bin */
#else
#	define CONFIG_ENV_IS_NOWHERE		/* ENV is stored in volatile RAM */
#	undef  CONFIG_CMD_SAVEENV		/* no need for "saveenv" */
#endif	/* CONFIG_CMD_NAND */
#define CONFIG_ENV_ADDR		(CONFIG_SYS_FLASH_BASE + CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SIZE		0x10000
#define CONFIG_ENV_SECT_SIZE	0x20000

/*----------------------------------------------------------------------
 * JFFS2 support
 */

#if defined(CONFIG_CMD_JFFS2)
#	ifdef CONFIG_CMD_NAND			/* NAND flash present ? */
#		define CONFIG_JFFS2_NAND	/* JFFS2 support on NAND Flash */
#	endif	/* CONFIG_CMD_NAND */
#	define CONFIG_CMD_MTDPARTS		/* mtdparts command line support */
#	define CONFIG_MTD_DEVICE		/* needed for mtdparts commands */
#endif	/* CONFIG_CMD_JFFS2 */


#endif	/* __CONFIG_H */
