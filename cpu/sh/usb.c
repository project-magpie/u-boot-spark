/*
 * Copyright (C) 2007-2008 STMicroelectronics Limited
 * Stuart Menefy <stuart.menefy@st.com>
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

#include <common.h>
#include <command.h>

#if defined(CONFIG_SH_STB7100)
extern void stb7100_usb_init(void);
#elif defined(CONFIG_SH_STX7200)
extern void stx7200_usb_init(void);
#else
#error Missing Device Defintions!
#endif

#if defined(CONFIG_USB_OHCI_NEW) && defined(CFG_USB_OHCI_CPU_INIT)

extern int usb_cpu_init(void)
{
#if defined(CONFIG_SH_STB7100)
	stb7100_usb_init();
#elif defined(CONFIG_SH_STX7200)
	stx7200_usb_init();
#endif
	return 0;
}

extern int usb_cpu_stop(void)
{
	return 0;
}

extern int usb_cpu_init_fail(void)
{
	return 0;
}

#endif /* defined(CONFIG_USB_OHCI_NEW) && defined(CFG_USB_OHCI_CPU_INIT) */

