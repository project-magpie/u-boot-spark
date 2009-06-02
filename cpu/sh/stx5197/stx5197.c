/*
 * (C) Copyright 2008-2009 STMicroelectronics.
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/soc.h>
#include <asm/stx5197reg.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <asm/stbus.h>
#include <ata.h>


#define SET_SYSCONF_BIT(reg,flag,bit)			\
	do {						\
		if (flag)				\
		{	/* set bit 'bit' */		\
			reg |= (1ul<<(bit));		\
		}					\
		else					\
		{	/* clear bit 'bit' */		\
			reg &= ~(1ul<<(bit));		\
		}					\
	} while (0)

#define SET_SYSCONF_BITS(reg,flag,first,last,yes,no)	\
	do {						\
		const unsigned long _mask = 		\
			(1ul<<((last)-(first)+1))-1ul;	\
		/* clear all bits in 'first':'last' */	\
		reg &= ~(_mask<<(first));		\
		if (flag)				\
		{	/* set 'yes' in first:last */	\
			reg |= ((yes)<<(first));	\
		}					\
		else					\
		{	/* set 'no' in first:last */	\
			reg |= ((no)<<(first));		\
		}					\
	} while (0)


static void stx5197_clocks(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	/*
	 * FIXME
	 * Gross hack to get the serial port working.
	 * See the defintion of PCLK in drivers/stm-asc.c
	 * for where this is used.
	 */
	bd->bi_emifrq = 140;	/* comms_clk = 140 MHz */
}


#ifdef CONFIG_DRIVER_NETSTMAC
extern int stmac_default_pbl(void)
{
	return 32;
}

extern void stmac_set_mac_speed(const int speed)
{
	unsigned long sysconf;

	sysconf = *STX5197_HD_CONF_MON_CONFIG_CONTROL_E;

	/* MAC speed*/
	/* CFG_CTRL_E.MAC_SPEED_SEL = speed==100 ? 1 : 0 [1] */
	SET_SYSCONF_BIT(sysconf, speed==100, 1);

	*STX5197_HD_CONF_MON_CONFIG_CONTROL_E = sysconf;
}

/* ETH MAC pad configuration */
static void stmac_eth_hw_setup(const int rmii, const int ext_clk, const int phy_bus)
{
	unsigned long sysconf;

	sysconf = *STX5197_HD_CONF_MON_CONFIG_CONTROL_E;

	/* Ethernet interface on */
	/* CFG_CTRL_E.ETHERNET_INTERFACE_ON = 1 [0] */
	SET_SYSCONF_BIT(sysconf, 1, 0);

	/* MII plyclk out enable: 0=output, 1=input */
	/* CFG_CTRL_E.MII_PHYCLK_OUT_EN = ext_clk ? 1 : 0 [6] */
	SET_SYSCONF_BIT(sysconf, ext_clk, 6);

	/* RMII/MII pin mode */
	/* CFG_CTRL_E.MII_ETHERNET_SEL = rmii ? 2 : 3 [8:7] */
	SET_SYSCONF_BITS(sysconf, rmii, 7, 8, 0x2, 0x3);

	/* MII mode */
	/* CFG_CTRL_E.MII_MODE = rmii ? 0 : 1 [2] */
	SET_SYSCONF_BIT(sysconf, !rmii, 2);

	*STX5197_HD_CONF_MON_CONFIG_CONTROL_E = sysconf;
}
#endif	/* CONFIG_DRIVER_NETSTMAC */


#if defined(CONFIG_USB_OHCI_NEW)
extern void stx5197_usb_init(void)
{
	unsigned long sysconf;

	/* USB power down */
	sysconf = *STX5197_HD_CONF_MON_CONFIG_CONTROL_H;
	sysconf &= ~(1ul<<8);	/* CFG_CTRL_H.USB_POWERDOWN_REQ = 0 [8] */
	*STX5197_HD_CONF_MON_CONFIG_CONTROL_H = sysconf;

	/* DDR enable for ULPI */
	sysconf = *STX5197_HD_CONF_MON_CONFIG_CONTROL_M;
	sysconf &= ~(1ul<<12);	/* CFG_CTRL_M.ULPI_DDR_EN_I = 0 [12] */
				/* 0=8-bit SDR ULPI, 1=4-bit DDR ULPI */
	*STX5197_HD_CONF_MON_CONFIG_CONTROL_M = sysconf;

	/* start the USB Wrapper Host Controller */
	ST40_start_host_control(
		USB_FLAGS_STRAP_16BIT		|
		USB_FLAGS_STRAP_PLL		|
		USB_FLAGS_STBUS_CONFIG_THRESHOLD256);
}
#endif /* defined(CONFIG_USB_OHCI_NEW) */


extern int soc_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	stx5197_clocks();

	/* obtain the chip cut + device id */
	bd->bi_devid = *STX5197_HD_CONF_MON_CONFIG_MONITOR_H;

#ifdef CONFIG_DRIVER_NETSTMAC
	stmac_eth_hw_setup(0, 1, 0);
#endif	/* CONFIG_DRIVER_NETSTMAC */

	return 0;
}

