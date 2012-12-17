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

#ifndef _ASM_ARCH_SPEAR600_H
#define _ASM_ARCH_SPEAR600_H

#define CONFIG_SYS_I2C_BASE			0xD0200000
#define CONFIG_GPIO_BASE			0xD8100000
#define CONFIG_SPEAR_ETHBASE			0xE0800000
#define CONFIG_SYS_FIFO_BASE			0xE1000800
#define CONFIG_SYS_USBD_BASE			0xE1100000
#define CONFIG_SYS_PLUG_BASE			0xE1200000
#define CONFIG_SYS_UHC0_EHCI_BASE		0xE1800000
#define CONFIG_SYS_SMI_BASE			0xFC000000

#define CONFIG_SPEAR_MPMCBASE			0xFC600000
#define CONFIG_SPEAR_MPMCREGS			100

#define CONFIG_SPEAR_TIMERBASE			0xFC800000
#define CONFIG_SPEAR_SYSCNTLBASE		0xFCA00000
#define CONFIG_SPEAR_MISCBASE			0xFCA80000

#define CONFIG_SYS_FSMC_BASE			0xD1800000
#define CONFIG_FSMC_NAND_BASE			0xD2000000
#define CONFIG_SYS_NAND_CLE			(1 << 16)
#define CONFIG_SYS_NAND_ALE			(1 << 17)

#define SNOR_BOOT_SUPPORTED			1
#define NAND_BOOT_SUPPORTED			1
#define PNOR_BOOT_SUPPORTED			0
#define USBD_BOOT_SUPPORTED			1
#define TFTP_BOOT_SUPPORTED			0
#define UART_BOOT_SUPPORTED			0
#define MMC_BOOT_SUPPORTED			0
#define SPI_BOOT_SUPPORTED			0
#define I2C_BOOT_SUPPORTED			0

#endif
