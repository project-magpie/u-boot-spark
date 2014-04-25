/*
 * (C) Copyright 2008, Michael Trimarchi <trimarchimichael@yahoo.it>
 * Copyright (C) 2012-2014 STMicroelectronics Limited
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
#include <../drivers/usb/host/ehci.h>
#include <stm/stbus.h>


#define unlikely(x)		__builtin_expect(!!(x), 0)

#define STM_HC_LENGTH		0x10	/* should always be 16 for ST's SoCs */


/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
extern int ehci_hcd_init(
	const int index, enum usb_init_type init,
	struct ehci_hccr ** const hccr,
	struct ehci_hcor ** const hcor)
{
	uint32_t hc_length;

	/*
	 * First, we call the board- and CPU-specific initialization functions,
	 * which will enable the controller, and configure all the USB PIOs.
	 * We will also call the function STM_start_host_control(),
	 * which configures the AMBA interface for the IP block.
	 */
	usb_cpu_init();

	/*
	 * Save address for both the HCCR and HCOR registers in the EHCI
	 * H/W controller, into the 2 indirect-pointers "hccr", and "hcor".
	 * NOTE: this limits us to only ONE host-controller (QQQ: allow 1+)
	 */
	*hccr = (struct ehci_hccr *)AHB2STBUS_EHCI_BASE;

	hc_length = HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase));
	BUG_ON(hc_length != STM_HC_LENGTH);

	*hcor = (struct ehci_hcor *)((uint32_t)*hccr + STM_HC_LENGTH);

#if 1
	printf("STM EHCI HCD initialization HCCR=0x%08x, HCOR=0x%08x, hc_length=%u\n",
		(uint32_t)*hccr, (uint32_t)*hcor, hc_length);
#endif

	return 0;	/* indicate success */
}


/*
 * Properly shutdown the EHCI host controller (HC).
 * This is a mandatory requirement before we can safely transition
 * from U-Boot to any OS (e.g. linux) … otherwise madness ensues!
 */
extern int ehci_hcd_stop(const int index)
{
	struct ehci_hccr * const hccr =
		(struct ehci_hccr *)AHB2STBUS_EHCI_BASE;
	struct ehci_hcor * const hcor =
		(struct ehci_hcor *)((uint32_t)hccr + STM_HC_LENGTH);
	uint32_t reg;
	const int numPorts = HCS_N_PORTS(ehci_readl(&hccr->cr_hcsparams));
	int port;

	/*
	 * Complete any current transactions, and HALT the HC.
	 */
	reg = ehci_readl(&hcor->or_usbcmd);
	reg &= ~(CMD_RUN);
	ehci_writel(&hcor->or_usbcmd, reg);

	/*
	 * Wait for the HC to be halted.
	 * This can take up to 16 micro-frames (i.e. 2ms).
	 */
	while (!(ehci_readl(&hcor->or_usbsts) & STS_HALT))
		;	/* wait until STS_HALT is asserted */

	/*
	 * power-down all the root ports.
	 */
	for (port=0; port<numPorts; port++) {
		ehci_writel(&hcor->or_portsc[port],
			EHCI_PS_OCC | EHCI_PS_PEC | EHCI_PS_CSC);
	}

	/*
	 * Finally, reset the controller (just in case)!
	 */
	reg = ehci_readl(&hcor->or_usbcmd);
	reg |= CMD_RESET;
	ehci_writel(&hcor->or_usbcmd, reg);

	/*
	 * … and wait for the reset to self-clear…
	 */
	while ((ehci_readl(&hcor->or_usbcmd) & CMD_RESET))
		;	/* wait until CMD_RESET is de-asserted */

	return 0;
}
