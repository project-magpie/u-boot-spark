/*
 * (C) Copyright 2007 STMicroelectronics.
 *
 * Stuart Menefy <stuart.menefy@st.com>
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
#include <asm/sti7200reg.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <ata.h>

static void stx7200_clocks(void)
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
	unsigned long sysconf = *STI7200_SYSCONF_SYS_CFG41;

	if (speed == 100)
		sysconf |= MAC_SPEED;
	else if (speed == 10)
		sysconf &= ~MAC_SPEED;

	*STI7200_SYSCONF_SYS_CFG41 = sysconf;
}
#endif

/* ETH MAC pad configuration */
static void stb7109eth_hw_setup(void)
{
	unsigned long sysconf;
#if defined(CONFIG_STMAC_MAC0)
	const int shift = 0;	/* First MAC */
#elif defined(CONFIG_STMAC_MAC1)
	const int shift = 1;	/* Second MAC */
#endif

	sysconf = *STI7200_SYSCONF_SYS_CFG41;
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
	*STI7200_SYSCONF_SYS_CFG41 = sysconf;
}
#endif

int soc_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	stx7200_clocks();

#ifdef CONFIG_DRIVER_NETSTMAC
	stb7109eth_hw_setup();
#endif

	bd->bi_devid = *STI7200_SYSCONF_DEVICEID_0;

	/*  Make sure reset period is shorter than WDT timeout */
	*STI7200_SYSCONF_SYS_CFG09 = (*STI7200_SYSCONF_SYS_CFG09 & 0xFF000000) | 0x000A8C;

	return 0;
}

void stx7200_reset(void)
{
	ulong sr;
	asm ("stc sr, %0":"=r" (sr));
	sr |= (1 << 28);	/* set block bit */
	asm ("ldc %0, sr": :"r" (sr));
	asm volatile ("trapa #0");
}
