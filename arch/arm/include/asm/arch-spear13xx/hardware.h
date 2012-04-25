/*
 * (C) Copyright 2010
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

#ifndef _ASM_ARCH_SPEAR13XX_HARDWARE_H
#define _ASM_ARCH_SPEAR13XX_HARDWARE_H

#define CONFIG_SPEAR_CORTEXBASE		(0xEC800000)
#define CONFIG_SPEAR_CA9TMR_BASE	(CONFIG_SPEAR_CORTEXBASE + 0x600)
#define CONFIG_SYS_USBD_BASE		(0xE3800000)
#define CONFIG_SYS_UHC0_EHCI_BASE	(0xE4800000)
#define CONFIG_SYS_UHC1_EHCI_BASE	(0xE5800000)
#if !defined(CONFIG_SPEAR1340) && !defined(CONFIG_SPEAR1310)
#define CONFIG_SYS_PLUG_BASE		(0xE2800000)
#endif
#define CONFIG_SYS_FIFO_BASE		(0xE3000800)
#define CONFIG_SYS_SMI_BASE		(0xEA000000)
#define CONFIG_SPEAR_TIMERBASE		(0xE0380000)
#define CONFIG_SPEAR_MISCBASE		(0xE0700000)
#define CONFIG_SYS_I2C_BASE		(0xE0280000)
#define CONFIG_SPEAR_ETHBASE		(0xE2000000)
#define CONFIG_SYS_FSMC_BASE		(0xB0000000)
#define CONFIG_SYS_MMC_BASE		(0xB3000000)

#define CONFIG_SPEAR_OTPPROGCTR		(0xE0700800)
       #define CONFIG_SPEAR_OTPRDY	0x00000007
#define CONFIG_SYS_OTPBASE		(0xE0700864)

#define CONFIG_SYS_NAND_CLE		(1 << 16)
#define CONFIG_SYS_NAND_ALE		(1 << 17)

#endif /* _ASM_ARCH_SPEAR13XX_HARDWARE_H */
