/*
 * (C) Copyright 2008, Michael Trimarchi <trimarchimichael@yahoo.it>
 * Copyright (C) 2012 STMicroelectronics Limited
 *	Sean McGoogan <Sean.McGoogan@st.com>
 *
 * Author: Michael Trimarchi <trimarchimichael@yahoo.it>
 * This code is based on ehci freescale driver, updated specifically
 * for the STMicroelectronics' EHCI controller, as found on ST40 SoCs.
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
#include <common.h>
#include <usb.h>
#include "ehci.h"
#include "ehci-core.h"
#include <asm/stbus.h>


#define unlikely(x)		__builtin_expect(!!(x), 0)


/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
extern int ehci_hcd_init(void)
{
	uint32_t hc_length;

	/*
	 * First, we call the board- and CPU-specific initialization functions,
	 * which will enable the controller, and configure all the USB PIOs.
	 * We will also call the function ST40_start_host_control(),
	 * which configures the AMBA interface for the IP block.
	 */
	usb_cpu_init();

	/*
	 * Save address for both the HCCR and HCOR registers in the EHCI
	 * H/W controller, into the 2 global variables "hccr", and "hcor".
	 */
	hccr = (struct ehci_hccr *)AHB2STBUS_EHCI_BASE;

	hc_length = HC_LENGTH(ehci_readl(&hccr->cr_capbase));
	BUG_ON(hc_length != 0x10);	/* should always be 16 for ST's SoCs */

	hcor = (struct ehci_hcor *)((uint32_t)hccr + hc_length);

#if 1
	printf("STM EHCI HCD initialization HCCR=0x%08x, HCOR=0x%08x, hc_length=%u\n",
		(uint32_t)hccr, (uint32_t)hcor, hc_length);
#endif

	return 0;	/* indicate success */
}


/*
 * Destroy the appropriate control structures corresponding
 * to the EHCI host controller.
 */
extern int ehci_hcd_stop(void)
{
	return 0;
}
