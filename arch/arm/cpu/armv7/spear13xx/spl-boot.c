/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/boot.h>
#include <linux/mtd/st_smi.h>

ulong spl_boot(void)
{
	ulong ret;

#if defined(CONFIG_SPEAR_USBTTY)
	return 0;
#endif

	switch (get_boot_type()) {
	case BOOT_TYPE_BYPASS:
	case BOOT_TYPE_SMI:
		/* SNOR-SMI initialization */
		smi_init();
		ret = (ulong)CONFIG_SYS_SNOR_BOOT_BASE;
		break;

	case BOOT_TYPE_NAND:
		ret = (ulong)CONFIG_SYS_NAND_BOOT_BASE;
		break;

	case BOOT_TYPE_PNOR8:
	case BOOT_TYPE_PNOR16:
	case BOOT_TYPE_PNOR32:
		ret = (ulong)CONFIG_SYS_PNOR_BOOT_BASE;
		break;

	case BOOT_TYPE_USBD:
	case BOOT_TYPE_TFTP:
	case BOOT_TYPE_PCIE:
	case BOOT_TYPE_UART:
		ret = 0;
		break;

	case BOOT_TYPE_MMC:
		ret = (ulong)CONFIG_SYS_MMC_BOOT_FILE;
		break;

	case BOOT_TYPE_I2C:
	case BOOT_TYPE_SPI:
	case BOOT_TYPE_RESERVED:
	case BOOT_TYPE_UNSUPPORTED:
	default:
		ret = (ulong)-1;
		break;
	}

	return ret;
}
