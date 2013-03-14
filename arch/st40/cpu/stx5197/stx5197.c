/*
 * (C) Copyright 2008-2012 STMicroelectronics.
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
#include <asm/sysconf.h>
#include <ata.h>
#include <spi.h>


DECLARE_GLOBAL_DATA_PTR;


static void stx5197_clocks(void)
{
	bd_t * const bd = gd->bd;

	/*
	 * Ideally, we should probe to determine all the clock frequencies.
	 * However, for simplicity, we will simply hard-wire the values
	 * that U-Boot will use for computing the clock dividers later.
	 * WARNING: Getting these values wrong may result in strange behaviour!
	 */
	bd->bi_uart_frq = 140ul * 1000000ul;	/* 140 MHz */
	bd->bi_tmu_frq  = 200ul * 1000000ul;	/* 200 MHz */
	bd->bi_ssc_frq  = bd->bi_uart_frq;
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
	/* CONFIG_SYS_CTRL_E.MAC_SPEED_SEL = speed==100 ? 1 : 0 [1] */
	SET_SYSCONF_BIT(sysconf, speed==100, 1);

	*STX5197_HD_CONF_MON_CONFIG_CONTROL_E = sysconf;
}

/* ETH MAC pad configuration */
static void stmac_eth_hw_setup(const int rmii, const int ext_clk, const int phy_bus)
{
	unsigned long sysconf;

	sysconf = *STX5197_HD_CONF_MON_CONFIG_CONTROL_E;

	/* Ethernet interface on */
	/* CONFIG_SYS_CTRL_E.ETHERNET_INTERFACE_ON = 1 [0] */
	SET_SYSCONF_BIT(sysconf, 1, 0);

	/* MII plyclk out enable: 0=output, 1=input */
	/* CONFIG_SYS_CTRL_E.MII_PHYCLK_OUT_EN = ext_clk ? 1 : 0 [6] */
	SET_SYSCONF_BIT(sysconf, ext_clk, 6);

	/* RMII/MII pin mode */
	/* CONFIG_SYS_CTRL_E.MII_ETHERNET_SEL = rmii ? 2 : 3 [8:7] */
	SET_SYSCONF_BITS(sysconf, rmii, 7, 8, 0x2, 0x3);

	/* MII mode */
	/* CONFIG_SYS_CTRL_E.MII_MODE = rmii ? 0 : 1 [2] */
	SET_SYSCONF_BIT(sysconf, !rmii, 2);

	*STX5197_HD_CONF_MON_CONFIG_CONTROL_E = sysconf;
}
#endif	/* CONFIG_DRIVER_NETSTMAC */


#if defined(CONFIG_USB_OHCI_NEW) || defined(CONFIG_USB_EHCI)
extern void stx5197_usb_init(void)
{
	unsigned long sysconf;

	/* USB power down */
	sysconf = *STX5197_HD_CONF_MON_CONFIG_CONTROL_H;
	sysconf &= ~(1ul<<8);	/* CONFIG_SYS_CTRL_H.USB_POWERDOWN_REQ = 0 [8] */
	*STX5197_HD_CONF_MON_CONFIG_CONTROL_H = sysconf;

	/* DDR enable for ULPI */
	sysconf = *STX5197_HD_CONF_MON_CONFIG_CONTROL_M;
	sysconf &= ~(1ul<<12);	/* CONFIG_SYS_CTRL_M.ULPI_DDR_EN_I = 0 [12] */
				/* 0=8-bit SDR ULPI, 1=4-bit DDR ULPI */
	*STX5197_HD_CONF_MON_CONFIG_CONTROL_M = sysconf;

	/* start the USB Wrapper Host Controller */
	ST40_start_host_control(
		USB_FLAGS_STRAP_16BIT		|
		USB_FLAGS_STRAP_PLL		|
		USB_FLAGS_STBUS_CONFIG_THRESHOLD256);
}
#endif /* defined(CONFIG_USB_OHCI_NEW) || defined(CONFIG_USB_EHCI) */


/**********************************************************************/


#if defined(CONFIG_SPI)

#if defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI)
/*
 * A pair of functions to assert and de-assert the SPI
 * chip select line, for the given SPI "slave" device.
 *
 * This is used by both the S/W "bit-banging" and the
 * H/W SSC drivers (but not the H/W FSM driver).
 *
 * We only support *one* SPI device, so we just ignore
 * the "slave" parameter. This may change later...
 */
extern void spi_cs_activate(struct spi_slave * const slave)
{
	unsigned long reg = *STX5197_HD_CONF_MON_CONFIG_CONTROL_M;

	reg &= ~(1ul<<13);	/* CONFIG_SYS_CTRL_M.SPI_CS_WHEN_SSC_USED = 0 [13] */

	/* assert SPI CSn */
	*STX5197_HD_CONF_MON_CONFIG_CONTROL_M = reg;

	/* wait 1us for CSn assert to propagate  */
	udelay(1);
}
extern void spi_cs_deactivate(struct spi_slave * const slave)
{
	unsigned long reg = *STX5197_HD_CONF_MON_CONFIG_CONTROL_M;

	reg |= 1ul<<13;		/* CONFIG_SYS_CTRL_M.SPI_CS_WHEN_SSC_USED = 1 [13] */

	/* DE-assert SPI CSn */
	*STX5197_HD_CONF_MON_CONFIG_CONTROL_M = reg;
}
#endif	/* defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI) */

#endif	/* CONFIG_SPI */


/**********************************************************************/


extern int soc_init(void)
{
	bd_t * const bd = gd->bd;

	stx5197_clocks();

	/* obtain the chip cut + device id */
	bd->bi_devid = *STX5197_HD_CONF_MON_CONFIG_MONITOR_H;

#ifdef CONFIG_DRIVER_NETSTMAC
	stmac_eth_hw_setup(0, 1, 0);
#endif	/* CONFIG_DRIVER_NETSTMAC */

	return 0;
}

