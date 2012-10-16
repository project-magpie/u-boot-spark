/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Microelectronics, vipin.kumar@st.com
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/generic.h>
#include <asm/arch/misc.h>

int get_socrev(void)
{
#if defined(CONFIG_SOC_SPEAR300)
	return SOC_SPEAR300;
#elif defined(CONFIG_SOC_SPEAR310)
	return SOC_SPEAR310;
#elif defined(CONFIG_SOC_SPEAR320)
	return SOC_SPEAR320;
#endif
}

void spear3xx_ddr_comp_init(void)
{
	struct misc_regs *misc_p = (struct misc_regs *)CONFIG_SPEAR_MISCBASE;
	u32 ddrpad;
	u32 core3v3, ddr1v8;

	/* DDR pad register configurations */
	ddrpad = readl(&misc_p->ddr_pad);
	ddrpad &= ~MISC_DDR_PAD_CNF_MSK;
	ddrpad |= 0x3AA4;
	writel(ddrpad, &misc_p->ddr_pad);

	/* Compensation register configurations */
	core3v3 = readl(&misc_p->core_3v3_compensation);
	core3v3 &= 0x02fffff0;
	core3v3 |= 0x78000008;
	writel(core3v3, &misc_p->core_3v3_compensation);

	ddr1v8 = readl(&misc_p->ddr_1v8_compensation);
	ddr1v8 &= 0x02fffff0;
	ddr1v8 |= 0x78000008;
	writel(ddr1v8, &misc_p->ddr_1v8_compensation);
}

/* getboottype() implementation for spear300 */
#if defined(CONFIG_SOC_SPEAR300)
u32 getboottype(void)
{
	u32 bootmask = 0;
	u32 bootstrap = (readl(CONFIG_SYS_TELECOM_BASE) >> SPEAR300_BOOTSHFT) &
			SPEAR300_BOOTMASK;

	switch (bootstrap) {
	case SPEAR300_USBBOOT:
		bootmask |= BOOT_TYPE_USBD;
		break;
	case SPEAR300_TFTPI2CBOOT:
	case SPEAR300_TFTPSPIBOOT:
		bootmask |= BOOT_TYPE_TFTP;
		break;
	case SPEAR300_SNORBOOT:
		bootmask |= BOOT_TYPE_SMI;
		break;
	case SPEAR300_PNOR8BOOT:
		bootmask |= BOOT_TYPE_PNOR8;
		break;
	case SPEAR300_PNOR16BOOT:
		bootmask |= BOOT_TYPE_PNOR16;
		break;
	case SPEAR300_NAND8BOOT:
	case SPEAR300_NAND16BOOT:
		bootmask |= BOOT_TYPE_NAND;
		break;
	case SPEAR300_UARTBOOT:
		bootmask |= BOOT_TYPE_UART;
		break;
	default:
		bootmask |= BOOT_TYPE_UNSUPPORTED;
		break;
	}

	return bootmask;
}
#elif defined(CONFIG_SOC_SPEAR310)
u32 getboottype(void)
{
	u32 bootmask = 0;
	u32 bootstrap = (readl(SPEAR310_BOOT_REG) >> SPEAR310_BOOTSHFT) &
			SPEAR310_BOOTMASK;

	switch (bootstrap) {
	case SPEAR310_USBBOOT:
		bootmask |= BOOT_TYPE_USBD;
		break;
	case SPEAR310_SNORBOOT:
		bootmask |= BOOT_TYPE_SMI;
		break;
	case SPEAR310_PNORBOOT:
		bootmask |= BOOT_TYPE_PNOR8 | BOOT_TYPE_PNOR16;
		break;
	case SPEAR310_NANDBOOT:
		bootmask |= BOOT_TYPE_NAND;
		break;
	default:
		bootmask |= BOOT_TYPE_UNSUPPORTED;
		break;
	}

	return bootmask;
}
#elif defined(CONFIG_SOC_SPEAR320)
u32 getboottype(void)
{
	u32 bootmask = 0;
	u32 bootstrap = (readl(SPEAR320_BOOT_REG) >> SPEAR320_BOOTSHFT) &
			SPEAR320_BOOTMASK;

	switch (bootstrap) {
	case SPEAR320_USBBOOT:
		bootmask |= BOOT_TYPE_USBD;
		break;
	case SPEAR320_TFTPI2CBOOT:
	case SPEAR320_TFTPSPIBOOT:
		bootmask |= BOOT_TYPE_TFTP;
		break;
	case SPEAR320_SNORBOOT:
		bootmask |= BOOT_TYPE_SMI;
		break;
	case SPEAR320_PNOR8BOOT:
	case SPEAR320_PNOR8NOACKBOOT:
		bootmask |= BOOT_TYPE_PNOR8;
		break;
	case SPEAR320_PNOR16BOOT:
	case SPEAR320_PNOR16NOACKBOOT:
		bootmask |= BOOT_TYPE_PNOR16;
		break;
	case SPEAR320_NAND8BOOT:
	case SPEAR320_NAND16BOOT:
		bootmask |= BOOT_TYPE_NAND;
		break;
	case SPEAR320_UARTBOOT:
		bootmask |= BOOT_TYPE_UART;
		break;
	default:
		bootmask |= BOOT_TYPE_UNSUPPORTED;
		break;
	}

	return bootmask;
}
#endif
