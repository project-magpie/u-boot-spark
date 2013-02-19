/*
 * Copyright (C) 2007-2013 STMicroelectronics Limited
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
#include <asm/stbus.h>
#include <asm/io.h>
#include <asm/soc.h>


#define mdelay(n) ({unsigned long msec=(n); while (msec--) udelay(1000);})


#if (defined(CONFIG_USB_OHCI_NEW) && defined(CONFIG_SYS_USB_OHCI_CPU_INIT)) || defined(CONFIG_USB_EHCI)
extern int usb_cpu_init(void)
{
#if defined(CONFIG_ST40_STX5197)
	stx5197_usb_init();
#elif defined(CONFIG_ST40_STX5206)
	stx5206_usb_init();
#elif defined(CONFIG_ST40_STX7105)
#ifdef CONFIG_ST40_STX_STX7105_USB_PORT0
	stx7105_usb_init(0,
			 CONFIG_ST40_STX_STX7105_USB_OC,
			 CONFIG_ST40_STX_STX7105_USB_PW);
#endif
#ifdef CONFIG_ST40_STX_STX7105_USB_PORT1
	stx7105_usb_init(1,
			 CONFIG_ST40_STX_STX7105_USB_OC,
			 CONFIG_ST40_STX_STX7105_USB_PW);
#endif
#elif defined(CONFIG_ST40_STX7108)
#if CONFIG_SYS_USB_BASE == CONFIG_SYS_USB0_BASE		/* USB #0 */
	stx7108_usb_init(0);
#elif CONFIG_SYS_USB_BASE == CONFIG_SYS_USB1_BASE	/* USB #1 */
	stx7108_usb_init(1);
#elif CONFIG_SYS_USB_BASE == CONFIG_SYS_USB2_BASE	/* USB #2 */
	stx7108_usb_init(2);
#endif
#elif defined(CONFIG_ST40_STX7111)
	stx7111_usb_init();
#elif defined(CONFIG_ST40_STX7141)
	stx7141_usb_init();
#elif defined(CONFIG_ST40_STX7200)
	stx7200_usb_init();
#elif defined(CONFIG_ST40_FLI7510)
	fli7510_usb_init (fli7510_usb_ovrcur_active_low);
#elif defined(CONFIG_ST40_FLI7540)
#if CONFIG_SYS_USB_BASE == CONFIG_SYS_USB_A_BASE	/* USB #1 */
	fli7540_usb_init (0, fli7540_usb_ovrcur_active_low);
#elif CONFIG_SYS_USB_BASE == CONFIG_SYS_USB_C_BASE	/* USB #2 */
	fli7540_usb_init (1, fli7540_usb_ovrcur_active_low);
#endif	/* CONFIG_SYS_USB_BASE ... */
#elif defined(CONFIG_ST40_STXH205)
#if CONFIG_SYS_USB_BASE == CONFIG_SYS_USB0_BASE		/* USB #0 */
	stxh205_usb_init(0);
#elif CONFIG_SYS_USB_BASE == CONFIG_SYS_USB1_BASE	/* USB #1 */
	stxh205_usb_init(1);
#endif
#elif defined(CONFIG_ST40_STXH415)
#if CONFIG_SYS_USB_BASE == CONFIG_SYS_USB0_BASE		/* USB #0 */
	stxh415_usb_init(0);
#elif CONFIG_SYS_USB_BASE == CONFIG_SYS_USB1_BASE	/* USB #1 */
	stxh415_usb_init(1);
#elif CONFIG_SYS_USB_BASE == CONFIG_SYS_USB2_BASE	/* USB #2 */
	stxh415_usb_init(2);
#endif
#elif defined(CONFIG_ST40_STXH416)
#if CONFIG_SYS_USB_BASE == CONFIG_SYS_USB0_BASE		/* USB #0 */
	stxh416_usb_init(0);
#elif CONFIG_SYS_USB_BASE == CONFIG_SYS_USB1_BASE	/* USB #1 */
	stxh416_usb_init(1);
#elif CONFIG_SYS_USB_BASE == CONFIG_SYS_USB2_BASE	/* USB #2 */
	stxh416_usb_init(2);
#elif CONFIG_SYS_USB_BASE == CONFIG_SYS_USB3_BASE	/* USB #3 */
	stxh416_usb_init(3);
#endif
#else
#error Missing Device Definitions!
#endif
	return 0;
}
#endif /* (CONFIG_USB_OHCI_NEW && CONFIG_SYS_USB_OHCI_CPU_INIT) || CONFIG_USB_EHCI */


#if defined(CONFIG_USB_OHCI_NEW) && defined(CONFIG_SYS_USB_OHCI_CPU_INIT)
extern int usb_cpu_stop(void)
{
	return 0;
}
#endif /* defined(CONFIG_USB_OHCI_NEW) && defined(CONFIG_SYS_USB_OHCI_CPU_INIT) */


#if defined(CONFIG_USB_OHCI_NEW) && defined(CONFIG_SYS_USB_OHCI_CPU_INIT)
extern int usb_cpu_init_fail(void)
{
	return 0;
}
#endif /* defined(CONFIG_USB_OHCI_NEW) && defined(CONFIG_SYS_USB_OHCI_CPU_INIT) */


#if defined(CONFIG_USB_OHCI_NEW) || defined(CONFIG_USB_EHCI)
extern int ST40_start_host_control(unsigned int flags)
{
	static int initialized = 0;
	unsigned long reg, req_reg;

	if (initialized)
		return 0;	/* already initialized */

	if (flags &
	    (USB_FLAGS_STRAP_8BIT | USB_FLAGS_STRAP_16BIT)) {
		/* Set strap mode */
		reg = readl(AHB2STBUS_STRAP);
		if (flags & USB_FLAGS_STRAP_16BIT)
			reg |= AHB2STBUS_STRAP_16_BIT;
		else
			reg &= ~AHB2STBUS_STRAP_16_BIT;
		writel(reg, AHB2STBUS_STRAP);
	}

	if (flags & USB_FLAGS_STRAP_PLL) {
		/* Start PLL */
		reg = readl(AHB2STBUS_STRAP);
		writel(reg | AHB2STBUS_STRAP_PLL, AHB2STBUS_STRAP);
		mdelay(30);
		writel(reg & (~AHB2STBUS_STRAP_PLL), AHB2STBUS_STRAP);
		mdelay(30);
	}

	if (flags & USB_FLAGS_OPC_MSGSIZE_CHUNKSIZE) {
		/* Set the STBus Opcode Config for load/store 32 */
		writel(AHB2STBUS_STBUS_OPC_32BIT, AHB2STBUS_STBUS_OPC);

		/* Set the Message Size Config to n packets per message */
		writel(AHB2STBUS_MSGSIZE_4, AHB2STBUS_MSGSIZE);

		/* Set the chunksize to n packets */
		writel(AHB2STBUS_CHUNKSIZE_4, AHB2STBUS_CHUNKSIZE);
	}

	if (flags &
	    (USB_FLAGS_STBUS_CONFIG_THRESHOLD128 |
	     USB_FLAGS_STBUS_CONFIG_THRESHOLD256)) {

		if (flags & USB_FLAGS_STBUS_CONFIG_THRESHOLD128)
			req_reg =
				(1<<21) |  /* Turn on read-ahead */
				(5<<16) |  /* Opcode is store/load 32 */
				(0<<15) |  /* Turn off write posting */
				(1<<14) |  /* Enable threshold */
				(3<<9)  |  /* 2**3 Packets in a chunk */
				(0<<4)  |  /* No messages */
				7;         /* Threshold is 128 */
		else
			req_reg =
				(1<<21) |  /* Turn on read-ahead */
				(5<<16) |  /* Opcode is store/load 32 */
				(0<<15) |  /* Turn off write posting */
				(1<<14) |  /* Enable threshold */
				(3<<9)  |  /* 2**3 Packets in a chunk */
				(0<<4)  |  /* No messages */
				(8<<0);    /* Threshold is 256 */

		do {
			writel(req_reg, AHB2STBUS_STBUS_CONFIG);
			reg = readl(AHB2STBUS_STBUS_CONFIG);
		} while ((reg & 0x7FFFFFFF) != req_reg);
	}

	initialized = 1;

	return 0;
}
#endif /* defined(CONFIG_USB_OHCI_NEW) || defined(CONFIG_USB_EHCI) */


