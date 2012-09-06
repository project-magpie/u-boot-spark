/*
 * (C) Copyright 2012
 * Vipin Kumar, STMicroelectronics, <vipin.kumar@st.com>
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

#define CONFIG_MACH_SPEAR1340LCAD
#define CONFIG_MACH_TYPE			3395

/* Timer configurations */
#define CONFIG_ARMV7_CA9LTIMER
#define CONFIG_ARMV7_LTMR_CLK			250000000

/* ARASAN SD MMC configuration */
#if !defined(CONFIG_SPEAR_USBTTY)
	#define CONFIG_SPEAR_SDHCI
#endif


/* Designware UDC configurations */
#if defined(CONFIG_SPEAR_USBTTY)
	#define CONFIG_DW_OTG
	#define CONFIG_DW_OTG_PHYINIT
#endif

/* ST SMI (Serial flash) configurations */
#define CONFIG_ST_SMI

/* GPIO configurations */
#define CONFIG_SPEAR_GPIO

/* AMBA PL011 configurations */
#define CONFIG_PL011_SERIAL
#define CONFIG_CONS_INDEX			0

/* USB EHCI configurations */
#if !defined(CONFIG_SPEAR_USBTTY)
	#define CONFIG_USB_EHCI_SPEAR
#endif

/* FSMC NAND configurations */
#define CONFIG_NAND_FSMC
#define CONFIG_SYS_FSMC_NAND_8BIT

/* Environment Variable configs */
/* Environment is in NAND */
#define CONFIG_ENV_IS_IN_NAND

#define CONFIG_ENV_OFFSET		0x00480000

#define CONFIG_BOOTCOMMAND	"run bootusb; run bootupg;" \
	"nand read.jffs2 0x800000 0x280000 0x800000; bootm 0x800000"

#define CONFIG_BOOTARGS		"console=ttyAMA0,115200 " \
	"mem=960M noinitrd no_console_suspend androidboot.console=ttyAMA0 " \
	"android.checkjni=0 root=/dev/mtdblock3 rw rootfstype=yaffs2 " \
	"rootflags=inband-tags,tags-ecc-off rootdelay=3" \
	"video=db9000:800x480-32@0 init=/init"

#define CONFIG_BOARD_EXTRA_ENV		"" \
	"bootusb=mw 0x0 0x0; usb start; fatload usb 0:2 0x0 run.img; " \
	"source 0x0\0" \
	"bootupg=mw 0x0 0x0; fatload usb 0:1 0x0 vInstaller/upgrade.img; " \
	"source 0x0\0" \
	"loados=tftpboot 0x900000 $(rootpath)/spear13xx_uImage\0"	\
	"loaddtb=tftpboot 0x800000 $(rootpath)/spear1340-lcad.dtb\0"

#include <configs/spear1340.h>
#endif
