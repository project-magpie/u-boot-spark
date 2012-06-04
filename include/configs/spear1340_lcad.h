/*
 * (C) Copyright 2012
 * Vincenzo Frascino, STMicroelectronics, <vincenzo.frascino@st.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_SPEAR13XX			1
#define CONFIG_SPEAR1340			1
#define CONFIG_SPEAR1340_LCAD			1
#define CONFIG_MACH_SPEAR1340_LCAD	1

#if defined(CONFIG_MK_usbtty)
#define CONFIG_SPEAR_USBTTY			1
#endif

#define CONFIG_ENV_IS_IN_NAND			1

#define CONFIG_YAFFS2			1

#if !defined(CONFIG_SPEAR_USBTTY)
#if !defined(CONFIG_SPEAR1340)
/* Solve issue #101435 - UHC blocks the BUSMATRIX */
#define CONFIG_SPEAR1300_ISSUE_101435		1
#endif
#endif

#if !defined(CONFIG_SPEAR_USBTTY)
/* MMC configuration */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_SPEAR_MMC
#define CONFIG_CMD_MMC
#endif

/* USBD driver configuration */
#if (defined(CONFIG_SPEAR_USBTTY))
#define CONFIG_USB_DEVICE
#ifdef CONFIG_SPEAR1340
#define CONFIG_DW_OTG
#else
#define CONFIG_DW_UDC
#endif
#define CONFIG_USBD_HS
#define CONFIG_USB_TTY

#define CONFIG_USBD_PRODUCT_NAME		"SPEAr SoC"
#define CONFIG_USBD_MANUFACTURER		"ST Microelectronics"

#endif

#define CONFIG_EXTRA_ENV_USBTTY			"usbtty=cdc_acm\0"

/* Timer, HZ specific defines */
#define CONFIG_SYS_HZ				(1000)

/* Flash configuration */
#define CONFIG_ST_SMI				1
#define CONFIG_SYS_MAX_FLASH_BANKS		2
#define CONFIG_SYS_FLASH_BASE			(0xE6000000)
#define CONFIG_SYS_CS1_FLASH_BASE		(0xE7000000)
#define CONFIG_SYS_FLASH_BANK_SIZE		(0x01000000)
#define CONFIG_SYS_FLASH_ADDR_BASE		{CONFIG_SYS_FLASH_BASE, \
						CONFIG_SYS_CS1_FLASH_BASE}
#define CONFIG_SYS_MAX_FLASH_SECT		128

#define CONFIG_SYS_FLASH_EMPTY_INFO		1
#define CONFIG_SYS_FLASH_ERASE_TOUT		(3 * CONFIG_SYS_HZ)
#define CONFIG_SYS_FLASH_WRITE_TOUT		(3 * CONFIG_SYS_HZ)

/*
 * Serial Configuration (PL011)
 */
#define CONFIG_PL011_SERIAL
#define CONFIG_SYS_SERIAL0			0xE0000000
#define CONFIG_PL011_CLOCK			(48 * 1000 * 1000)
#define CONFIG_CONS_INDEX			0
#define CONFIG_BAUDRATE				115200
#define CONFIG_SYS_BAUDRATE_TABLE		{ 9600, 19200, 38400, \
						57600, 115200 }

#define CONFIG_SYS_LOADS_BAUD_CHANGE
#define CONFIG_PL01x_PORTS			{(void *)CONFIG_SYS_SERIAL0}

/*
 * USB/EHCI
 */
#if !defined(CONFIG_SPEAR_USBTTY)
#define CONFIG_USB_EHCI				/* Enable EHCI USB support */
#define CONFIG_USB_EHCI_SPEAR
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_USB				/* Enable USB Commands */
#endif

/*
 * NAND FLASH Configuration
 */
#define CONFIG_NAND_FSMC			1
#define CONFIG_SYS_FSMC_NAND_8BIT		1
#define CONFIG_SYS_MAX_NAND_DEVICE		1
#if defined(CONFIG_SPEAR1340)
#define CONFIG_SYS_NAND_BASE			(0xB0800000)
#else
#define CONFIG_SYS_NAND_BASE			(0xA0000000)
#endif
#define CONFIG_SYS_NAND_ONFI_DETECTION		1
#define CONFIG_SYS_NAND_QUIET_TEST		1
#define CONFIG_CMD_NAND

/*
 * Command support defines
 */
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_RUN

#if !defined(CONFIG_SPEAR_USBTTY)
#if defined(CONFIG_USB_STORAGE) || defined(CONFIG_MMC)
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#define CONFIG_ISO_PARTITION
#endif
#endif

/* This must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <config_cmd_default.h>

#undef CONFIG_CMD_NET
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_XIMG
#undef CONFIG_CMD_LOADS

/*
 * Default Environment Varible definitions
 */
#if defined(CONFIG_SPEAR_USBTTY)
/* This disbales autobooting and stops at uboot prompt */
#define CONFIG_BOOTDELAY			-1
#else
#define CONFIG_BOOTDELAY			1
#endif

#define CONFIG_ENV_OVERWRITE
/*
 * Environment placing
 */
#if defined(CONFIG_ENV_IS_IN_FLASH)
/*
 * Environment is in serial NOR flash
 */
#define CONFIG_SYS_MONITOR_LEN			0x00040000
#define CONFIG_ENV_SECT_SIZE			0x00010000

#if !defined(CONFIG_SPEAR1340)
#define CONFIG_FSMTDBLK				"/dev/mtdblock3 "
#else
#define CONFIG_FSMTDBLK				"/dev/mtdblock3 "
#endif

#if defined(CONFIG_SPEAR1340)
#define CONFIG_BOOTCOMMAND			"run bootusb; run bootupg; nand read.jffs2 0x800000 0x280000 0x800000; bootm 0x800000"
#else
#define CONFIG_BOOTCOMMAND			"bootm 0xe6050000"
#endif

#define CONFIG_SYS_MONITOR_BASE			CONFIG_SYS_FLASH_BASE
#define CONFIG_ENV_ADDR				(CONFIG_SYS_MONITOR_BASE + \
						CONFIG_SYS_MONITOR_LEN)
#elif defined(CONFIG_ENV_IS_IN_NAND)
/*
 * Environment is in NAND
 */

#define CONFIG_ENV_OFFSET			0x480000
#define CONFIG_ENV_RANGE			0x40000

#if !defined(CONFIG_SPEAR1340)
#define CONFIG_FSMTDBLK				"/dev/mtdblock3 "
#else
#define CONFIG_FSMTDBLK				"/dev/mtdblock3 "
#endif

#define CONFIG_BOOTCOMMAND			"run bootusb; run bootupg; nand read.jffs2 0x800000 0x280000 0x800000; bootm 0x800000"

#endif

#define CONFIG_BOOTARGS_NFS			"root=/dev/nfs ip=dhcp " \
						"console=ttyAMA0,115200 " \
						"init=/bin/sh"
#if defined(CONFIG_SPEAR1340)
#define CONFIG_BOOTARGS	"console=ttyAMA0,115200 " \
						"mem=960M "  \
						"noinitrd no_console_suspend androidboot.console=ttyAMA0 android.checkjni=0 " \
						"root="CONFIG_FSMTDBLK "rw rootfstype=yaffs2 rootflags=inband-tags,tags-ecc-off " \
						"rootdelay=3 video=db9000:800x480-32@0 init=/init"
#else
#define CONFIG_BOOTARGS				"console=ttyAMA0,115200 " \
						"mem=128M "  \
						"root="CONFIG_FSMTDBLK \
						"rootfstype=jffs2"
#endif

#define CONFIG_NFSBOOTCOMMAND						\
	"bootp; "							\
	"setenv bootargs root=/dev/nfs rw "				\
	"nfsroot=$(serverip):$(rootpath) "				\
	"ip=$(ipaddr):$(serverip):$(gatewayip):"			\
			"$(netmask):$(hostname):$(netdev):off "		\
			"console=ttyAMA0,115200 $(othbootargs);"	\
	"bootm; "

#define CONFIG_RAMBOOTCOMMAND						\
	"setenv bootargs root=/dev/ram rw "				\
		"console=ttyAMA0,115200 $(othbootargs);"		\
	CONFIG_BOOTCOMMAND

#define CONFIG_ENV_SIZE				0x02000
#define CONFIG_SYS_MONITOR_BASE			TEXT_BASE
#define CONFIG_MONITOR_IS_IN_RAM		1

/* Miscellaneous configurable options */
#define CONFIG_ARCH_CPU_INIT			1
#define CONFIG_DISPLAY_CPUINFO			1

#define CONFIG_BOOT_PARAMS_ADDR			0x00000100
#define CONFIG_CMDLINE_TAG			1
#define CONFIG_SETUP_MEMORY_TAGS		1
#define CONFIG_MISC_INIT_R			1
#define CONFIG_ZERO_BOOTDELAY_CHECK		1
#define CONFIG_AUTOBOOT_KEYED			1
#define CONFIG_AUTOBOOT_STOP_STR		" "
#define CONFIG_AUTOBOOT_PROMPT			\
		"Hit SPACE in %d seconds to stop autoboot.\n", bootdelay

#define CONFIG_SYS_MEMTEST_START		0x00800000
#define CONFIG_SYS_MEMTEST_END			0x04000000
#define CONFIG_SYS_MALLOC_LEN			(1024*1024)
#define CONFIG_SYS_GBL_DATA_SIZE		128
#define CONFIG_IDENT_STRING			"-SPEAr"
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_PROMPT			"android> "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_CBSIZE			256
#define CONFIG_SYS_PBSIZE			(CONFIG_SYS_CBSIZE + \
						sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS			16
#define CONFIG_SYS_BARGSIZE			CONFIG_SYS_CBSIZE
#define CONFIG_SYS_LOAD_ADDR			0x00800000
#define CONFIG_SYS_CONSOLE_INFO_QUIET		1
#define CONFIG_SYS_64BIT_VSPRINTF		1

#define CONFIG_EXTRA_ENV_SETTINGS		\
					CONFIG_EXTRA_ENV_USBTTY \
					"bootusb=mw 0x0 0x0; usb start; fatload usb 0:2 0x0 run.img; source 0x0\0" \
					"bootupg=mw 0x0 0x0; fatload usb 0:1 0x0 vInstaller/upgrade.img; source 0x0\0" \

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS			1
#define PHYS_SDRAM_1				0x00000000
#define PHYS_SDRAM_1_MAXSIZE			0x40000000

#endif
