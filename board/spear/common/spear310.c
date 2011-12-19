/*
 * SPEAr310 machine source file
 *
 * Copyright (C) 2011 ST Microelectronics
 * Shiraz Hashim <shiraz.hashim@st.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <common.h>
#include <linux/types.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/spr_misc.h>
#include <asm/arch/spr_generic.h>
#include <asm/arch/spear320.h>
#include <asm/arch/padmux.h>

/*
 * Function to dnamically set control of shared mdio lines to concerned
 * mac controller
 */
#ifdef CONFIG_ETH_MDIO_HOOK
void arch_get_mdio_control(struct eth_device *netdev)
{
	u32 val;

	val = readl(CONFIG_SPEAR_RASBASE + CONFG_SPEAR_SMII_OFF);
	val &= ~SMII_PHY_MASK;

	if (!strcmp(netdev->name, "macb0")) {
		val |= (0x0 << SMII_PHY_SHIFT);
	} else if (!strcmp(netdev->name, "macb1")) {
		val |= (0x1 << SMII_PHY_SHIFT);
	} else if (!strcmp(netdev->name, "macb2")) {
		val |= (0x2 << SMII_PHY_SHIFT);
	} else if (!strcmp(netdev->name, "macb3")) {
		val |= (0x3 << SMII_PHY_SHIFT);
	} else {
		printf ("no such device:%s\n", netdev->name);
	}

	writel(val, CONFIG_SPEAR_RASBASE + CONFG_SPEAR_SMII_OFF);
}
#endif
