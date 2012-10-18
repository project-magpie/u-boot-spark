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
#include <asm/arch/misc.h>

#if defined(CONFIG_USB_EHCI_SPEAR)
void spear6xx_usbh_stop(void)
{
	struct misc_regs *const misc_p =
	    (struct misc_regs *)CONFIG_SPEAR_MISCBASE;
	u32 periph1_rst = readl(misc_p->periph1_rst);

	periph1_rst |= MISC_USBHENB;
	writel(periph1_rst, misc_p->periph1_rst);

	udelay(1000);
	periph1_rst &= ~MISC_USBHENB;
	writel(periph1_rst, misc_p->periph1_rst);
}
#endif
