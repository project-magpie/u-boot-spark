/*
 * (C) Copyright 2000-2009
 * Viresh Kumar, ST Microelectronics, viresh.kumar@st.com
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
#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/arch/misc.h>
#include <asm/arch/generic.h>

#define FALSE				0
#define TRUE				(!FALSE)

int get_socrev(void)
{
	struct misc_regs *misc_p = (struct misc_regs *)CONFIG_SPEAR_MISCBASE;
	u32 soc_id = readl(&misc_p->soc_core_id);
	u32 pri_socid = (soc_id >> MISC_PRISOCID_SHFT) & 0xFF;
	u32 sec_socid = (soc_id >> MISC_SECSOCID_SHFT) & 0xFF;

	if ((pri_socid == 'B') && (sec_socid == 'B'))
		return SOC_SPEAR600_BB;
	else if ((pri_socid == 'B') && (sec_socid == 'C'))
		return SOC_SPEAR600_BC;
	else if ((pri_socid == 'B') && (sec_socid == 'D'))
		return SOC_SPEAR600_BD;
	else if (soc_id == 0)
		return SOC_SPEAR600_BA;
	else
		return SOC_SPEAR_NA;
}

/*
 * soc_init:
 */
void soc_init(void)
{
	/* Nothing to be done for SPEAr600 */
}

/*
 * xxx_boot_selected:
 *
 * return TRUE if the particular booting option is selected
 * return FALSE otherwise
 */
static u32 read_bootstrap(void)
{
	struct misc_regs *misc_p = (struct misc_regs *)CONFIG_SPEAR_MISCBASE;

	return readl(&misc_p->auto_cfg_reg) & MISC_BOOTSTRAPMASK;
}

int snor_boot_selected(void)
{
	u32 bootstrap = read_bootstrap();

	if (SNOR_BOOT_SUPPORTED) {
		/* Check whether SNOR boot is selected */
		if ((bootstrap & MISC_ONLYSNORBOOT) == MISC_ONLYSNORBOOT)
			return TRUE;

		if ((bootstrap & MISC_NORNANDBOOT) == MISC_NORNAND8BOOT)
			return TRUE;

		if ((bootstrap & MISC_NORNANDBOOT) == MISC_NORNAND16BOOT)
			return TRUE;
	}

	return FALSE;
}

int nand_boot_selected(void)
{
	u32 bootstrap = read_bootstrap();

	if (NAND_BOOT_SUPPORTED) {
		/* Check whether NAND boot is selected */
		if ((bootstrap & MISC_NORNANDBOOT) == MISC_NORNAND8BOOT)
			return TRUE;

		if ((bootstrap & MISC_NORNANDBOOT) == MISC_NORNAND16BOOT)
			return TRUE;
	}

	return FALSE;
}

int pnor_boot_selected(void)
{
	/* Parallel NOR boot is not selected in any SPEAr600 revision */
	return FALSE;
}

int usb_boot_selected(void)
{
	u32 bootstrap = read_bootstrap();

	if (USB_BOOT_SUPPORTED) {
		/* Check whether USB boot is selected */
		if (!(bootstrap & MISC_USBBOOT))
			return TRUE;
	}

	return FALSE;
}

int tftp_boot_selected(void)
{
	/* TFTP boot is not selected in any SPEAr600 revision */
	return FALSE;
}

int uart_boot_selected(void)
{
	/* UART boot is not selected in any SPEAr600 revision */
	return FALSE;
}

int spi_boot_selected(void)
{
	/* SPI boot is not selected in any SPEAr600 revision */
	return FALSE;
}

int i2c_boot_selected(void)
{
	/* I2C boot is not selected in any SPEAr600 revision */
	return FALSE;
}

int mmc_boot_selected(void)
{
	return FALSE;
}

void plat_late_init(void)
{
	spear_late_init();
}
