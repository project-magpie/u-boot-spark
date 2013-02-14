/*
 * (C) Copyright 2007-2012 STMicroelectronics.
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
 */

#ifdef CONFIG_ST40_SE_MODE
#define CONFIG_SYS_FLASH_BASE		0xA0000000	/* FLASH (uncached) via PMB */
#define CONFIG_SYS_SDRAM_BASE		0x80000000	/* LMI0 via PMB */
#define CONFIG_SYS_SE_PHYSICAL_BASE	0x40000000	/* LMI0 Physical Address */
#else
#define CONFIG_SYS_FLASH_BASE		0xA0000000	/* FLASH in P2 region */
#define CONFIG_SYS_SDRAM_BASE		0x88000000	/* SDRAM in P1 region */
#endif

#define CONFIG_SYS_SDRAM_SIZE		0x10000000	/* 256 MiB of LMI0 SDRAM */

#define CONFIG_SYS_MONITOR_LEN		0x00020000	/* Reserve 128 KiB for Monitor */
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_MALLOC_LEN		(1 << 20)	/* Reserve 1 MiB for malloc */
#define CONFIG_SYS_GBL_DATA_SIZE	1024		/* Global data structures */

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(TEXT_BASE - CONFIG_SYS_MALLOC_LEN - (1 << 20))

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define XSTR(s) STR(s)
#define STR(s) #s

#define BOARD mb671

#if CONFIG_SYS_MONITOR_LEN == 0x00020000	/* 128 KiB */
#	define MONITOR_SECTORS	"1:0"		/* 1 sector */
#elif CONFIG_SYS_MONITOR_LEN == 0x00040000	/* 256 KiB */
#	define MONITOR_SECTORS	"1:0-1"		/* 2 sectors */
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
#define CONFIG_SYS_STM_ASC_BASE		0xfd032000ul	/* UART2 (lower) */
//#define CONFIG_SYS_STM_ASC_BASE	0xfd033000ul	/* UART3 (upper) */

/*---------------------------------------------------------------
 * Ethernet driver config
 */

/*
 * There are 2 options for ethernet, both use the on-chip ST-MAC.
 * The choice in PHYs is:
 *    1) The on-board SMSC LAN8700 PHY.
 *    2) External PHY connected via the MII off-board connector (MB539B).
 */

/* are we using the internal ST MAC device ? */
#define CONFIG_DRIVER_NETSTMAC

/* choose which internal MAC to use */
#if 1
#	define CONFIG_STMAC_MAC0		/* Use MAC #0 */
#else
#	define CONFIG_STMAC_MAC1		/* Use MAC #1 */
#endif

/*
 * Select the appropriate base address for the selected MAC.
 * Also, choose which external PHY type to use.
 */
#if defined(CONFIG_STMAC_MAC0)
#	define CONFIG_SYS_STM_STMAC_BASE	0xfd500000ul	/* MAC #0 */
#	define CONFIG_STMAC_LAN8700				/* SMSC LAN8700 */
#elif defined(CONFIG_STMAC_MAC1)
#	define CONFIG_SYS_STM_STMAC_BASE	0xfd510000ul	/* MAC #1 */
#	define CONFIG_STMAC_LAN8700				/* SMSC LAN8700 */
#endif

/*  If this board does not have eeprom for ethernet address so allow the user
 *  to set it in the environment
 */
#define CONFIG_ENV_OVERWRITE


/*---------------------------------------------------------------
 * USB driver config
 */

/* Choose if we want USB Mass-Storage Support */
//#define CONFIG_ST40_STM_USB

#ifdef CONFIG_ST40_STM_USB
#	define CONFIG_CMD_USB
#	define CONFIG_CMD_FAT
#	define CONFIG_USB_STORAGE
#	define CONFIG_SYS_USB0_BASE			0xfd200000	/* middle */
#	define CONFIG_SYS_USB1_BASE			0xfd300000	/* right */
#	define CONFIG_SYS_USB2_BASE			0xfd400000	/* left */
#	define CONFIG_SYS_USB_BASE			CONFIG_SYS_USB0_BASE
#	if 1	/* use OHCI (USB 1.x) ? */
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

/* SATA works on cut 3.x of the STx7200 (just one port) */
/* Choose if we want to use a SATA HDD */
//#define CONFIG_ST40_STM_SATA

#ifdef CONFIG_ST40_STM_SATA
#	define CONFIG_CMD_IDE					/* enable "ide" command set */
#	define CONFIG_SYS_ATA_BASE_ADDR		0xfd520000	/* SATA #0 */
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
#define CONFIG_SYS_PROMPT		"MB671> "		/* Monitor Command Prompt	*/
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_HZ			1000			/* HZ for timer ticks	*/
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_SDRAM_BASE	/* default load address		*/
#define CONFIG_BOOTDELAY		10			/* default delay before executing bootcmd */
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_CMDLINE_EDITING

/*-----------------------------------------------------------------------
 * NOR FLASH organization
 */

/* M58LT256GT: 32MiB 259 blocks, 128 KiB block size plus parameter block */
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_PROTECT_SINGLE_CELL
#define CONFIG_FLASH_SHOW_PROGRESS	45	/* count down from 45/5: 9..1		*/
#define CONFIG_SYS_FLASH_PROTECTION	1	/* use hardware flash protection	*/
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CONFIG_SYS_MAX_FLASH_SECT	259	/* max number of sectors on one chip	*/
#define CONFIG_SYS_FLASH_EMPTY_INFO		/* test if each sector is empty		*/

/*-----------------------------------------------------------------------
 * Addresss, size, & location of U-boot's Environment Sector
 */

#define CONFIG_ENV_IS_IN_FLASH	1
#define CONFIG_ENV_OFFSET	CONFIG_SYS_MONITOR_LEN
#define CONFIG_ENV_ADDR		(CONFIG_SYS_FLASH_BASE + CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SIZE		0x10000
#define CONFIG_ENV_SECT_SIZE	0x20000

#endif	/* __CONFIG_H */
