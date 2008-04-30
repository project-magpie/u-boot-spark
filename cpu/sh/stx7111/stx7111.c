/*
 * (C) Copyright 2008 STMicroelectronics.
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
#include <asm/stx7111reg.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <asm/stbus.h>
#include <ata.h>

#define PIO_BASE  ST40_PIO0_REGS_BASE

static void stx7111_clocks(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	/*
	 * FIXME
	 * Gross hack to get the serial port working.
	 * See the defintion of PCLK in drivers/stm-asc.c
	 * for where this is used.
	 */
	bd->bi_emifrq = 100;
}

#ifdef CONFIG_DRIVER_NETSTMAC

#define MII_MODE		(1<<0)
#define PHY_CLK_EXT             (1<<2)
#define MAC_SPEED               (1<<4)
#define VCI_ACK_SOURCE          (1<<6)
#define RESET                   (1<<8)
#define DISABLE_MSG_READ        (1<<12)
#define DISABLE_MSG_WRITE       (1<<14)
/* Remaining bits define pad functions, default appears to work */

int stmac_default_pbl(void)
{
	return 32;
}

#ifdef CONFIG_STMAC_STE101P_RMII
void stb7109_mac_speed(int speed)
{
#ifdef QQQ	/* QQQ - DELETE */
	unsigned long sysconf = *STX7200_SYSCONF_SYS_CFG41;

	if (speed == 100)
		sysconf |= MAC_SPEED;
	else if (speed == 10)
		sysconf &= ~MAC_SPEED;

	*STX7200_SYSCONF_SYS_CFG41 = sysconf;
#endif		/* QQQ - DELETE */
}
#endif

/* ETH MAC pad configuration */
static void stmac_eth_hw_setup(void)
{
#ifdef QQQ	/* QQQ - DELETE */
	unsigned long sysconf;
#if defined(CONFIG_STMAC_MAC0)
	const int shift = 0;	/* First MAC */
#elif defined(CONFIG_STMAC_MAC1)
	const int shift = 1;	/* Second MAC */
#endif

	sysconf = *STX7200_SYSCONF_SYS_CFG41;
        sysconf &= ~(DISABLE_MSG_READ << shift);
        sysconf &= ~(DISABLE_MSG_WRITE << shift);
        //sysconf |=  (VCI_ACK_SOURCE << shift);
        sysconf &= ~(VCI_ACK_SOURCE << shift);
        sysconf |=  (RESET << shift);

#ifdef CONFIG_STMAC_STE101P_RMII
        sysconf |= (MII_MODE << shift);
        sysconf &= ~(PHY_CLK_EXT << shift);
#else
        sysconf &= ~(MII_MODE << shift);
        sysconf &= ~(PHY_CLK_EXT << shift);
#endif
	*STX7200_SYSCONF_SYS_CFG41 = sysconf;
#endif		/* QQQ - DELETE */
}
#endif

int soc_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	stx7111_clocks();

#ifdef CONFIG_DRIVER_NETSTMAC
	stmac_eth_hw_setup();
#endif

	bd->bi_devid = *STX7111_SYSCONF_DEVICEID_0;

	/*  Make sure reset period is shorter than WDT timeout */
	*STX7111_SYSCONF_SYS_CFG09 = (*STX7111_SYSCONF_SYS_CFG09 & 0xFF000000) | 0x000A8C;

	return 0;
}

void stx7111_reset(void)
{
	ulong sr;
	asm ("stc sr, %0":"=r" (sr));
	sr |= (1 << 28);	/* set block bit */
	asm ("ldc %0, sr": :"r" (sr));
	asm volatile ("trapa #0");
}


#if defined(CONFIG_USB_OHCI_NEW)

extern void stx7111_usb_init(void)
{
	unsigned long reg, req_reg;

	/* Power on the USB */
	reg = readl(STX7111_SYSCONF_SYS_CFG32);
	reg &= ~(1ul<<4); /* USB_POWER_DOWN_REQ = 0 */
	writel(reg, STX7111_SYSCONF_SYS_CFG32);

	/* Work around for USB over-current detection chip being
	 * active low, and the 7111 being active high.
	 * Note this is an undocumented bit, which apparently enables
	 * an inverter on the overcurrent signal.
	 */
	reg = readl(STX7111_SYSCONF_SYS_CFG06);
	reg |= 1ul<<29;
	writel(reg, STX7111_SYSCONF_SYS_CFG06);

	/* USB oc */
	SET_PIO_PIN(PIO_PORT(5), 6, STPIO_IN);
	/* USB power */
	SET_PIO_PIN(PIO_PORT(5), 7, STPIO_ALT_OUT);
	STPIO_SET_PIN(PIO_PORT(5), 7, 1);

	/* Set strap mode */
#define STRAP_MODE	AHB2STBUS_STRAP_16_BIT
	reg = readl(AHB2STBUS_STRAP);
#if STRAP_MODE == 0
	reg &= ~AHB2STBUS_STRAP_16_BIT;
#else
	reg |= STRAP_MODE;
#endif
	writel(reg, AHB2STBUS_STRAP);

	/* Start PLL */
	reg = readl(AHB2STBUS_STRAP);
	writel(reg | AHB2STBUS_STRAP_PLL, AHB2STBUS_STRAP);
	udelay(100000);	/* QQQ: can this delay be shorter ? */
	writel(reg & (~AHB2STBUS_STRAP_PLL), AHB2STBUS_STRAP);
	udelay(100000);	/* QQQ: can this delay be shorter ? */

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

#endif /* defined(CONFIG_USB_OHCI_NEW) */

