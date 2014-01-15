/*
 * (C) Copyright 2008-2014 STMicroelectronics.
 *
 * Stuart Menefy <stuart.menefy@st.com>
 * Sean McGoogan <Sean.McGoogan@st.com>
 * Pawel Moll <pawel.moll@st.com>
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
#include <stm/soc.h>
#include <stm/stxh407reg.h>
#include <asm/io.h>
#include <stm/pio.h>
#include <stm/stbus.h>
#include <stm/sysconf.h>
#include <stm/pad.h>
#include <stm/pio-control.h>
#include <ata.h>
#include <spi.h>
#include <netdev.h>


DECLARE_GLOBAL_DATA_PTR;


#undef  BUG_ON
#define BUG_ON(condition) do { if ((condition)!=0) BUG(); } while(0)

#define ARRAY_SIZE(x)		(sizeof(x) / sizeof((x)[0]))


#define TRUE			1
#define FALSE			0

	/*
	 * Define the following only if we need to debug the PIO
	 * configurations, including alternate function usage, and re-timings.
	 * Then set 'debug_pad_configs' to TRUE as appropriate.
	 * NOTE: do *not* enable this before the "console" is up and running!
	 */
//#define DEBUG_PAD_CONFIGS
#ifdef DEBUG_PAD_CONFIGS
volatile int debug_pad_configs = 0;
#endif

	/*
	 * The STxH407 does not have any ST40 CPUs!
	 * We refuse to build, if this assertion is invalid.
	 */
#if defined(CONFIG_ST40)
#error This SoC does have a ST40 CPU!
#endif	/* CONFIG_ST40 */


static void stxh407_clocks(void)
{
	/*
	 * Ideally, we should probe to determine all the clock frequencies.
	 * However, for simplicity, we will simply hard-wire the values
	 * that U-Boot will use for computing the clock dividers later.
	 * WARNING: Getting these values wrong may result in strange behaviour!
	 *
	 * Note: for the ASC in the SBC, we expect this will always be 30MHz,
	 *       otherwise we expect the ASC to be 100MHz.
	 */
#if (CONFIG_SYS_STM_ASC_BASE==STXH407_SBC_ASC0_BASE) || (CONFIG_SYS_STM_ASC_BASE==STXH407_SBC_ASC1_BASE)
	gd->stm_uart_frq =  30ul * 1000000ul;	/*  30 MHz */
#else
	gd->stm_uart_frq = 100ul * 1000000ul;	/* 100 MHz */
#endif
	gd->stm_ssc_frq  = 100ul * 1000000ul;	/* 100 MHz */
}


/*
 * PIO alternative Function selector
 */
extern void stxh407_pioalt_select(int port, const int pin, const int alt)
{
	unsigned long sysconf, *sysconfReg;

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("%s(port=%d, pin=%d, alt=%d)\n", __func__, port, pin, alt);
	BUG_ON(pin < 0 || pin > 7);
	BUG_ON(alt < 0 || alt > 7);
#endif

	switch (port)
	{
	case 0 ... 5:		/* in "SBC Bank" */
		sysconfReg = (unsigned long*)STXH407_SYSCFG(0);
		sysconfReg += port;
		break;
	case 10 ... 20:		/* in "FRONT Bank" */
		sysconfReg = (unsigned long*)STXH407_SYSCFG(1000);
		sysconfReg += port - 10;
		break;
	case 30 ... 35:		/* in "REAR Bank" */
		sysconfReg = (unsigned long*)STXH407_SYSCFG(2000);
		sysconfReg += port - 30;
		break;
	case 40 ... 42:		/* in "FLASH Bank" */
		sysconfReg = (unsigned long*)STXH407_SYSCFG(3000);
		sysconfReg += port - 40;
		break;
	default:
		BUG();
		return;
	}

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("ante: *%p = 0x%08lx\n", sysconfReg, *sysconfReg);
#endif
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BITS(sysconf, TRUE, pin*4,(pin*4)+3, alt,alt);
	writel(sysconf, sysconfReg);
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("post: *%p = 0x%08lx\n", sysconfReg, *sysconfReg);
#endif
}

/* Pad configuration */

extern void stxh407_pioalt_pad(int port, const int pin,
		const enum stm_pad_gpio_direction direction)
{
	int bit;
	int oe=0, pu=0, od=0;
	unsigned long sysconf, *sysconfReg;

	switch (direction) {
	case stm_pad_direction_input:
		oe = 0; pu = 0; od = 0;
		break;
	case stm_pad_direction_input_with_pullup:
		oe = 0; pu = 1; od = 0;
		break;
	case stm_pad_direction_output:
		oe = 1; pu = 0; od = 0;
		break;
	case stm_pad_direction_bidir_no_pullup:
		oe = 1; pu = 0; od = 1;
		break;
	default:
		BUG();
		break;
	}

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("%s(port=%d, pin=%d, oe=%d, pu=%d, od=%d)\n", __func__, port, pin, oe, pu, od);
	BUG_ON(pin < 0 || pin > 7);
#endif

	switch (port)
	{
	case 0 ... 5:		/* in "SBC Bank" */
		sysconfReg = (unsigned long*)STXH407_SYSCFG(40);
		sysconfReg += port / 4;
		break;
	case 10 ... 20:		/* in "FRONT Bank" */
		port -= 10;
		sysconfReg = (unsigned long*)STXH407_SYSCFG(1040);
		sysconfReg += port / 4;
		break;
	case 30 ... 35:		/* in "REAR Bank" */
		port -= 30;
		sysconfReg = (unsigned long*)STXH407_SYSCFG(2040);
		sysconfReg += port / 4;
		break;
	case 40 ... 42:		/* in "FLASH Bank" */
		/* QQQ: for the time being, treat them as BUG()! */
	default:
		BUG();
		return;
	}

	bit = ((port * 8) + pin) % 32;

		/* set the "Output Enable" pad control */
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("ante: *%p = 0x%08lx\n", sysconfReg, *sysconfReg);
#endif
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BIT(sysconf, oe, bit);
	writel(sysconf, sysconfReg);
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("post: *%p = 0x%08lx\n", sysconfReg, *sysconfReg);
#endif

	sysconfReg += 10;	/* skip to next set of syscfg registers */

		/* set the "Pull Up" pad control */
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("ante: *%p = 0x%08lx\n", sysconfReg, *sysconfReg);
#endif
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BIT(sysconf, pu, bit);
	writel(sysconf, sysconfReg);
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("post: *%p = 0x%08lx\n", sysconfReg, *sysconfReg);
#endif

	sysconfReg += 10;	/* skip to next set of syscfg registers */

		/* set the "Open Drain Enable" pad control */
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("ante: *%p = 0x%08lx\n", sysconfReg, *sysconfReg);
#endif
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BIT(sysconf, od, bit);
	writel(sysconf, sysconfReg);
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("post: *%p = 0x%08lx\n", sysconfReg, *sysconfReg);
#endif
}

/* PIO retiming setup */

#ifdef CONFIG_DRIVER_NET_STM_GMAC

extern void stxh407_pioalt_retime(int port, const int pin,
		const struct stm_pio_control_retime_config * const cfg,
		const enum stm_pad_gpio_direction direction)
{
	unsigned long sysconf, *sysconfReg;
	unsigned innotout = 0;

#ifdef DEBUG_PAD_CONFIGS
	BUG_ON(!cfg);
	if (debug_pad_configs)
		printf("%s(port=%d, pin=%d, retime=%d, clk=%d, "
				"clknotdata=%d, double_edge=%d, invertclk=%d, "
				"delay=%d, direction=%s)\n", __func__, port, pin,
				cfg->retime, cfg->clk, cfg->clknotdata,
				cfg->double_edge, cfg->invertclk, cfg->delay,
				(direction==stm_pad_direction_input) ? "in" :
					(direction==stm_pad_direction_input_with_pullup) ? "in+pu" :
					(direction==stm_pad_direction_output) ? "out" :
					(direction==stm_pad_direction_bidir_no_pullup) ? "bidir" :
					(direction==stm_pad_direction_ignored) ? "ignore" :
					"BAD-BAD"
				);
	BUG_ON(pin < 0 || pin > 7);
	BUG_ON(cfg->retime < 0);	/* the "don't care" semantic is deprecated */
	BUG_ON(cfg->clk < 0);		/* the "don't care" semantic is deprecated */
	BUG_ON(cfg->clknotdata < 0);	/* the "don't care" semantic is deprecated */
	BUG_ON(cfg->double_edge < 0);	/* the "don't care" semantic is deprecated */
	BUG_ON(cfg->invertclk < 0);	/* the "don't care" semantic is deprecated */
	BUG_ON(cfg->delay < 0);		/* the "don't care" semantic is deprecated */
#endif

	switch (direction)
	{
	case stm_pad_direction_input:
	case stm_pad_direction_input_with_pullup:
		innotout = 1;
		break;
	case stm_pad_direction_output:
	case stm_pad_direction_bidir_no_pullup:
		innotout = 0;
		break;
	default:
		BUG();
		break;
	}

	switch (port)
	{
	case 0 ... 5:		/* in "SBC Bank" */
		sysconfReg = (unsigned long*)STXH407_SYSCFG(100);
		sysconfReg += (port-0) * 8;
		break;
	case 10 ... 20:		/* in "FRONT Bank" */
		sysconfReg = (unsigned long*)STXH407_SYSCFG(1100);
		sysconfReg += (port-10) * 8;
		break;
	case 30 ... 35:		/* in "REAR Bank" */
		sysconfReg = (unsigned long*)STXH407_SYSCFG(2100);
		sysconfReg += (port-30) * 8;
		break;
	case 40 ... 42:		/* in "FLASH Bank" */
		/* QQQ: for the time being, treat them as BUG()! */
	default:
		BUG();
		return;
	}

	sysconfReg += pin;

	/* read the "old" value from the system configuration register */
	sysconf = readl(sysconfReg);

	if (cfg->clk >= 0)
	{			/* map value to 2 adjacent bits */
		SET_SYSCONF_BITS(sysconf, TRUE, 0,1, cfg->clk,cfg->clk);
	}

	if (cfg->clknotdata >= 0)
	{
		SET_SYSCONF_BIT(sysconf, cfg->clknotdata, 2);
	}

	if (cfg->delay >= 0)
	{			/* map value to 4 adjacent bits */
		SET_SYSCONF_BITS(sysconf, TRUE, 3,6, cfg->delay,cfg->delay);
	}

	SET_SYSCONF_BIT(sysconf, innotout, 7);

	if (cfg->double_edge >= 0)
	{
		SET_SYSCONF_BIT(sysconf, cfg->double_edge, 8);
	}

	if (cfg->invertclk >= 0)
	{
		SET_SYSCONF_BIT(sysconf, cfg->invertclk, 9);
	}

	if (cfg->retime >= 0)
	{
		SET_SYSCONF_BIT(sysconf, cfg->retime, 10);
	}

	/* write the "new" value to the system configuration register */
	writel(sysconf, sysconfReg);
}


/* --------------------------------------------------------------------
 *		Ethernet MAC resources (PAD and Retiming)
 * --------------------------------------------------------------------*/


#define SYSCONF(_reg)	((unsigned long*)STXH407_SYSCFG(_reg))


#define DATA_IN(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = stm_pad_direction_input, \
		.retime    = _retiming, \
	}

#define DATA_OUT(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = stm_pad_direction_output, \
		.retime    = _retiming, \
	}

#define MDIO(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = stm_pad_direction_output, \
		.retime    = _retiming, \
	}

#define MDC(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = stm_pad_direction_output, \
		.retime    = _retiming, \
	}

/* On some boards MDIO line is missing Pull-up resistor, Enabling weak
 * internal PULL-UP overcomes the issue */
#define DATA_OUT_PU(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = stm_pad_direction_input_with_pullup, \
		.retime    = _retiming, \
	}

#define CLOCK_IN(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = stm_pad_direction_input, \
		.retime    = _retiming, \
	}

#define CLOCK_OUT(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = stm_pad_direction_output, \
		.retime    = _retiming, \
	}

#define PHY_CLOCK(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.u.gmac.phy_clock = 1, \
		.direction = stm_pad_direction_unknown, \
		.retime    = _retiming, \
	}


static struct stm_pad_pin stxh407_ethernet_mii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[1] */
			DATA_OUT(0, 2, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[2] */
			DATA_OUT(0, 3, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[3] */
			DATA_OUT(0, 4, 1, RET_SE_NICLK_IO(0, 0)),/* TXER */
			DATA_OUT(0, 5, 1, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			CLOCK_IN(0, 6, 1, RET_NICLK(0, 0)),/* TXCLK */
			DATA_IN(0, 7, 1, RET_BYPASS(500)),/* COL */
			DATA_OUT(1, 0, 1, RET_BYPASS(0)),/* MDIO */
			CLOCK_OUT(1, 1, 1, RET_NICLK(0, 0)),/* MDC */
			DATA_IN(1, 2, 1, RET_BYPASS(500)),/* CRS */
			DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
			DATA_IN(1, 4, 1, RET_SE_NICLK_IO(0, 0)),/* RXD[0] */
			DATA_IN(1, 5, 1, RET_SE_NICLK_IO(0, 0)),/* RXD[1] */
			DATA_IN(1, 6, 1, RET_SE_NICLK_IO(0, 0)),/* RXD[2] */
			DATA_IN(1, 7, 1, RET_SE_NICLK_IO(0, 0)),/* RXD[3] */
			DATA_IN(2, 0, 1, RET_SE_NICLK_IO(0, 0)),/* RXDV */
			DATA_IN(2, 1, 1, RET_SE_NICLK_IO(0, 0)),/* RX_ER */
			CLOCK_IN(2, 2, 1, RET_NICLK(0, 0)),/* RXCLK */
			PHY_CLOCK(2, 3, 1, RET_NICLK(0, 1)),/* PHYCLK */
};
static struct stm_pad_sysconf stxh407_ethernet_mii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(510), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(508), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(508), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(508), 6, 6, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(508), 8, 8, 0),
};

#if 0
static struct stm_pad_pin stxh407_ethernet_gmii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_SE_NICLK_IO(750, 0)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_SE_NICLK_IO(750, 0)),/* TXD[1] */
			DATA_OUT(0, 2, 1, RET_SE_NICLK_IO(750, 0)),/* TXD[2] */
			DATA_OUT(0, 3, 1, RET_SE_NICLK_IO(750, 0)),/* TXD[3] */
			DATA_OUT(0, 4, 1, RET_SE_NICLK_IO(750, 0)),/* TXER */
			DATA_OUT(0, 5, 1, RET_SE_NICLK_IO(750, 0)),/* TXEN */
			CLOCK_IN(0, 6, 1, RET_NICLK(0, 0)),/* TXCLK  FIXME*/
			DATA_IN(0, 7, 1, RET_BYPASS(500)),/* COL */
			DATA_OUT_PU(1, 0, 1, RET_BYPASS(750)),/* MDIO */
			CLOCK_OUT(1, 1, 1, RET_NICLK(0, 0)),/* MDC */
			DATA_IN(1, 2, 1, RET_BYPASS(500)),/* CRS */
			DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
			DATA_IN(1, 4, 1, RET_SE_NICLK_IO(750, 0)),/* RXD[0] */
			DATA_IN(1, 5, 1, RET_SE_NICLK_IO(750, 0)),/* RXD[1] */
			DATA_IN(1, 6, 1, RET_SE_NICLK_IO(750, 0)),/* RXD[2] */
			DATA_IN(1, 7, 1, RET_SE_NICLK_IO(750, 0)),/* RXD[3] */
			DATA_IN(2, 0, 1, RET_SE_NICLK_IO(750, 0)),/* RXDV */
			DATA_IN(2, 1, 1, RET_SE_NICLK_IO(750, 0)),/* RX_ER */
			CLOCK_IN(2, 2, 1, RET_NICLK(0, 0)),/* RXCLK */
			PHY_CLOCK(2, 3, 4, RET_NICLK(0, 1)), /* GTXCLK */
			DATA_OUT(2, 6, 4, RET_SE_NICLK_IO(750, 0)),/* TXD[4] */
			DATA_OUT(2, 7, 4, RET_SE_NICLK_IO(750, 0)),/* TXD[5] */
			DATA_IN(3, 0, 4, RET_SE_NICLK_IO(750, 0)),/* RXD[4] */
			DATA_IN(3, 1, 4, RET_SE_NICLK_IO(750, 0)),/* RXD[5] */
			DATA_IN(3, 2, 4, RET_SE_NICLK_IO(750, 0)),/* RXD[6] */
			DATA_IN(3, 3, 4, RET_SE_NICLK_IO(750, 0)),/* RXD[7] */
			CLOCK_IN(3, 7, 4, RET_NICLK(0, 0)),/* 125MHZ input clock */
			DATA_OUT(4, 1, 4, RET_SE_NICLK_IO(750, 1)),/* TXD[6] */
			DATA_OUT(4, 2, 4, RET_SE_NICLK_IO(750, 1)),/* TXD[7] */
};
static struct stm_pad_sysconf stxh407_ethernet_gmii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(510), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(508), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(508), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(508), 6, 6, 0),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(508), 8, 8, 0),
};
#endif	/* 0 */

static struct stm_pad_pin stxh407_ethernet_rgmii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_DE_IO(0, 0)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_DE_IO(0, 0)),/* TXD[1] */
			DATA_OUT(0, 2, 1, RET_DE_IO(0, 0)),/* TXD[2] */
			DATA_OUT(0, 3, 1, RET_DE_IO(0, 0)),/* TXD[3] */
			DATA_OUT(0, 5, 1, RET_DE_IO(0, 0)),/* TXEN */
			CLOCK_IN(0, 6, 1, RET_NICLK(0, 0)),/* TXCLK */
#if !defined(CONFIG_STMAC_USE_RTL8367_SMI)
	/* With the RealTek RTL8367RB switch, use I2C-type SMI instead */
			MDIO(1, 0, 1, RET_BYPASS(0)),/* MDIO */
			MDC(1, 1, 1, RET_NICLK(0, 0)),/* MDC */
#endif	/* CONFIG_STMAC_USE_RTL8367_SMI */
			DATA_IN(1, 4, 1, RET_DE_IO(0, 0)),/* RXD[0] */
			DATA_IN(1, 5, 1, RET_DE_IO(0, 0)),/* RXD[1] */
			DATA_IN(1, 6, 1, RET_DE_IO(0, 0)),/* RXD[2] */
			DATA_IN(1, 7, 1, RET_DE_IO(0, 0)),/* RXD[3] */
			DATA_IN(2, 0, 1, RET_DE_IO(0, 0)),/* RXDV */
			CLOCK_IN(2, 2, 1, RET_NICLK(500, 0)),/* RXCLK */
			PHY_CLOCK(2, 3, 4, RET_NICLK(1750, 1)), /* PHYCLK */
			CLOCK_IN(3, 7, 4, RET_NICLK(0, 0)),/* 125MHz input clock */
};
static struct stm_pad_sysconf stxh407_ethernet_rgmii_pad_sysconfs[] = {
			/* SBC_RESET_PER */
			STM_PAD_SYSCONF(SYSCONF(4002), 4, 4, 1),
			/* PWR_DWN_REQ */
			STM_PAD_SYSCONF(SYSCONF(4032), 0, 0, 0),
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(4032), 1, 1, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(4032), 2, 4, 1),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(4032), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(4032), 6, 6, 0),
			/* INT_NOEXT_PHYCLK */
			STM_PAD_SYSCONF(SYSCONF(4032), 7, 7, 1),
			/* TX_RETIME_CLK */
			STM_PAD_SYSCONF(SYSCONF(4032), 8, 8, 1),
};

static struct stm_pad_pin stxh407_ethernet_rmii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[1] */
			DATA_OUT(0, 5, 1, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			DATA_OUT(1, 0, 1, RET_BYPASS(750)),/* MDIO */
			CLOCK_OUT(1, 1, 1, RET_NICLK(0, 0)),/* MDC */
			DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
			DATA_IN(1, 4, 1, RET_SE_NICLK_IO(500, 1)),/* RXD[0] */
			DATA_IN(1, 5, 1, RET_SE_NICLK_IO(500, 1)),/* RXD[1] */
			DATA_IN(2, 0, 1, RET_SE_NICLK_IO(500, 1)),/* RXDV */
			DATA_IN(2, 1, 1, RET_SE_NICLK_IO(500, 0)),/* RX_ER */
			PHY_CLOCK(2, 3, 1, RET_NICLK(0, 1)),/* PHYCLK */
};
static struct stm_pad_sysconf stxh407_ethernet_rmii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(510), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(508), 2, 4, 4),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(508), 5, 5, 1),
			/* ETH1_SEL_INTERNAL_NOT_EXT_PHYCLK */
			STM_PAD_SYSCONF(SYSCONF(508), 7, 7, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(508), 8, 8, 1),
};

#if 0
static struct stm_pad_pin stxh407_ethernet_reverse_mii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[1] */
			DATA_OUT(0, 2, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[2] */
			DATA_OUT(0, 3, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[3] */
			DATA_OUT(0, 4, 1, RET_SE_NICLK_IO(0, 1)),/* TXER */
			DATA_OUT(0, 5, 1, RET_SE_NICLK_IO(0, 1)),/* TXEN */
			CLOCK_IN(0, 6, 1, RET_NICLK(0, 0)),/* TXCLK */
			DATA_OUT(0, 7, 2, RET_BYPASS(0)),/* COL */
			DATA_IN(2, 0, 1, RET_SE_NICLK_IO(500, 1)),/* RXDV */
			DATA_IN(2, 1, 1, RET_SE_NICLK_IO(500, 1)),/* RX_ER */
			CLOCK_IN(2, 2, 1, RET_NICLK(0, 0)),/* RXCLK */
			PHY_CLOCK(2, 3, 1, RET_NICLK(0, 0)),/* PHYCLK */
			DATA_OUT_PU(1, 0, 1, RET_BYPASS(500)),/* MDIO */
			CLOCK_IN(1, 1, 2, RET_NICLK(0, 1)),/* MDC */
			DATA_OUT(1, 2, 2, RET_BYPASS(0)),/* CRS */
			DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
			DATA_IN(1, 4, 1, RET_SE_NICLK_IO(500, 1)),/* RXD[0] */
			DATA_IN(1, 5, 1, RET_SE_NICLK_IO(500, 1)),/* RXD[1] */
			DATA_IN(1, 6, 1, RET_SE_NICLK_IO(500, 1)),/* RXD[2] */
			DATA_IN(1, 7, 1, RET_SE_NICLK_IO(500, 1)),/* RXD[3] */
};
static struct stm_pad_sysconf stxh407_ethernet_reverse_mii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(510), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(508), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(508), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(508), 6, 6, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(508), 8, 8, 0),
};
#endif	/* 0 */


/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/


extern int stmac_default_pbl(void)
{
	return 32;
}

#define GMAC_AHB_CONFIG		0x2000
static void stxh407_ethernet_bus_setup(void)
{
	/* Configure the bridge to generate more efficient STBus traffic.
	 *
	 * Cut Version	| Ethernet AD_CONFIG[21:0]
	 *	1.0	|	0x00264207
	 */
	writel(0x00264207, CONFIG_SYS_STM_STMAC_BASE + GMAC_AHB_CONFIG);
}


/* ETH MAC pad configuration */
extern void stxh407_configure_ethernet(
	const struct stxh407_ethernet_config * const config)
{
	struct stm_pad_pin * pad_config;
	struct stm_pad_pin * phy_clock;
	const struct stm_pad_sysconf * sys_configs;
	size_t num_pads, num_sys, i;

	BUG_ON(!config);

	switch (config->mode) {

	case stxh407_ethernet_mode_mii:
		pad_config = stxh407_ethernet_mii_pad_configs;
		num_pads = ARRAY_SIZE(stxh407_ethernet_mii_pad_configs);
		sys_configs = stxh407_ethernet_mii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh407_ethernet_mii_pad_sysconfs);
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);
		if (config->ext_clk)
			stm_pad_set_pio_ignored(phy_clock);
		else
			stm_pad_set_pio_out(phy_clock, 1);
		break;

	case stxh407_ethernet_mode_rmii:  {
		pad_config = stxh407_ethernet_rmii_pad_configs;
		num_pads = ARRAY_SIZE(stxh407_ethernet_rmii_pad_configs);
		sys_configs = stxh407_ethernet_rmii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh407_ethernet_rmii_pad_sysconfs);

		/* we assume we *always* use the internal clock for RMII! */
		BUG_ON(config->ext_clk!= 0);

		/*
		 * Configure the PHY Clock:
		 * CLKS_GMAC0_PHY (PIO13[5]) or CLKS_ETH1_PHY (PIO2[3]) should
		 * be 50MHz for RMII mode (as appropriate).
		 * Note: We rely on the GDB "pokes" to set the frequency for us ...
		 */
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);
		stm_pad_set_pio_out(phy_clock, 1);
	} break;

	case stxh407_ethernet_mode_rgmii:
		/* This mode is similar to GMII (GTX) except the data
		 * buses are reduced down to 4 bits and the 2 error
		 * signals are removed. The data rate is maintained by
		 * using both edges of the clock. This also explains
		 * the different retiming configuration for this mode.
		 */
		pad_config = stxh407_ethernet_rgmii_pad_configs;
		num_pads = ARRAY_SIZE(stxh407_ethernet_rgmii_pad_configs);
		sys_configs = stxh407_ethernet_rgmii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh407_ethernet_rgmii_pad_sysconfs);
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);
		stm_pad_set_pio_out(phy_clock, 4);
		break;

	case stxh407_ethernet_mode_reverse_mii:
		BUG();		/* assume not required in U-Boot */
		return;

	case stxh407_ethernet_mode_gmii:
		BUG();		/* assume not required in U-Boot */
		return;

	default:
		BUG();
		return;
	}

		/* now configure all the PIOs */
	for (i = 0; i < num_pads; i++)
	{
		const struct stm_pad_pin * const pad = &pad_config[i];
		const int portno = pad->pio.port;
		const int pinno = pad->pio.pin;

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("%2u: PIO%03u[%u] %-7s, alt=%u, retime=%p\n",
			i+1,
			portno, pinno,
			(pad->direction==stm_pad_direction_input) ? "in" :
				(pad->direction==stm_pad_direction_input_with_pullup) ? "in+pu" :
				(pad->direction==stm_pad_direction_output) ? "out" :
				(pad->direction==stm_pad_direction_bidir_no_pullup) ? "bidir" :
				(pad->direction==stm_pad_direction_ignored) ? "ignore" :
				"BAD-BAD",
			pad->pio.alt,
			pad->retime
		);
#endif

		if (pad->direction == stm_pad_direction_ignored)
			continue;	/* skip all "ignored" pads */

		stxh407_pioalt_select(portno, pinno, pad->pio.alt);
		stxh407_pioalt_pad(portno, pinno, pad->direction);
		stxh407_pioalt_retime(portno, pinno, pad->retime, pad->direction);
	}

		/* now configure the relevant SYS_CONFIGs */
	stm_configure_sysconfs(sys_configs, num_sys);

		/* need to do this *after* the SYSCONF initialization! */
	stxh407_ethernet_bus_setup();
}


	/*
	 * SoC-specific function to register all ST-MAC/GMAC controllers.
	 * The function "board_eth_init()" should ideally be defined
	 * for *each* board.  However, "cpu_eth_init()" can be used
	 * as a generic fall-back, for all boards of a given CPU type.
	 * In any event, "board_eth_init()" will have a higher priority,
	 * so define it, if you want to override the following SoC function.
	 */
extern int cpu_eth_init(bd_t * const bis)
{
	stmac_eth_register(1, CONFIG_SYS_STM_STMAC_BASE);

	return 0;
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */


extern int arch_cpu_init(void)
{
	stxh407_clocks();

	gd->stm_devid = *STXH407_SYSCONF_DEVICEID;

#if 0	/* QQQ - TO IMPLEMENT */
	/* Make sure reset period is shorter than WDT time-out */
	*STX7108_BANK0_SYSCFG(14) = 3000;	/* about 100 us */
#endif	/* QQQ - TO IMPLEMENT */

	return 0;
}


#if defined(CONFIG_STM_SATA)
extern void stxh407_configure_sata(void)
{
#if 0	/* QQQ - TO IMPLEMENT */
	static int initialised_phy = 0;
	unsigned long sysconf;

	if (!initialised_phy)
	{
		/* Power up the SATA PHY */
		sysconf = *STX7105_SYSCONF_SYS_CFG32;
		sysconf &= ~(1u<<9);	/* [11] SATA1_PHY_POWER_DOWN_REQ */
		*STX7105_SYSCONF_SYS_CFG32 = sysconf;

		/* initialize the SATA PHY */
		stm_sata_miphy_init();

		/* Power up the SATA host */
		sysconf = *STX7105_SYSCONF_SYS_CFG32;
		sysconf &= ~(1u<<11);	/* [9] SATA1_HC_POWER_DOWN_REQ */
		*STX7105_SYSCONF_SYS_CFG32 = sysconf;

		/* configure the SATA host controller */
		stm_sata_probe();

		initialised_phy = 1;
	}
#endif	/* QQQ - TO IMPLEMENT */
}
#endif	/* CONFIG_STM_SATA */


#if defined(CONFIG_CMD_I2C) && defined(CONFIG_SOFT_I2C)
struct ssc_pios
{
	struct
	{
		char port;
		char pin;
	}	pio[2];
};
static const struct ssc_pios ssc_pios[6+3] =
{
	{ { /* SSC0 */
		{ 10, 5 }, /* SCLK */
		{ 10, 6 }, /* MTSR */
	} }, { { /* SSC1 */
		{ 11, 0 }, /* SCLK */
		{ 11, 1 }, /* MTSR */
	} }, { { /* SSC2 */
		{ 15, 5 }, /* SCLK */
		{ 15, 6 }, /* MTSR */
	} }, { { /* SSC3 */
		{ 18, 5 }, /* SCLK */
		{ 18, 6 }, /* MTSR */
	} }, { { /* SSC4 */
		{ 30, 0 }, /* SCLK */
		{ 30, 1 }, /* MTSR */
	} }, { { /* SSC5 */
		{ 34, 3 }, /* SCLK */
		{ 34, 4 }, /* MTSR */
	} }, { { /* SBC_SSC10 */
		{ 4, 5 }, /* SCLK */
		{ 4, 6 }, /* MTSR */
	} }, { { /* SBC_SSC11 */
		{ 5, 0 }, /* SCLK */
		{ 5, 1 }, /* MTSR */
	} }, { { /* SBC_SSC12 */
		{ 3, 7 }, /* SCLK */
		{ 3, 6 }, /* MTSR */
	} },
};

extern void stxh407_configure_i2c(void)
{
	/*
	 * The I2C buses are routed as follows:
	 *
	 *	Bus	  SCL		  SDA
	 *	---	  ---		  ---
	 *	 0	PIO10[5]	PIO10[6]	SSC #0
	 *	 1	PIO11[0]	PIO11[1]	SSC #1
	 *	 2	PIO15[5]	PIO15[6]	SSC #2
	 *	 3	PIO18[5]	PIO18[6]	SSC #3
	 *	 4	PIO30[0]	PIO30[1]	SSC #4
	 *	 5	PIO34[3]	PIO34[4]	SSC #5
	 *	 6	PIO4[5]		PIO4[6]		SBC_SSC #10
	 *	 7	PIO5[0]		PIO5[1]		SBC_SSC #11
	 *	 8	PIO3[7]		PIO3[6]		SBC_SSC #12
	 */
	const int scl_port = ssc_pios[CONFIG_I2C_BUS].pio[0].port;
	const int scl_pin  = ssc_pios[CONFIG_I2C_BUS].pio[0].pin;
	const int sda_port = ssc_pios[CONFIG_I2C_BUS].pio[1].port;
	const int sda_pin  = ssc_pios[CONFIG_I2C_BUS].pio[1].pin;

	if (CONFIG_I2C_BUS >= ARRAY_SIZE(ssc_pios)) BUG();

	/* route PIO (explicitly alternate #0) */
	stxh407_pioalt_select(scl_port, scl_pin, 0);			/* I2C_SCL */
	stxh407_pioalt_select(sda_port, sda_pin, 0);			/* I2C_SDA */

	/* set up directionality appropriately */
	SET_PIO_PIN(STM_PIO_BASE(scl_port), scl_pin, STPIO_BIDIR);	/* I2C_SCL */
	SET_PIO_PIN(STM_PIO_BASE(sda_port), sda_pin, STPIO_BIDIR);	/* I2C_SDA */
}

extern void stxh407_i2c_scl(const int val)
{
	/* SSC's SCLK == I2C's SCL */
	const int port = ssc_pios[CONFIG_I2C_BUS].pio[0].port;
	const int pin  = ssc_pios[CONFIG_I2C_BUS].pio[0].pin;
	STPIO_SET_PIN(STM_PIO_BASE(port), pin, (val) ? 1 : 0);
}

extern void stxh407_i2c_sda(const int val)
{
	/* SSC's MTSR == I2C's SDA */
	const int port = ssc_pios[CONFIG_I2C_BUS].pio[1].port;
	const int pin  = ssc_pios[CONFIG_I2C_BUS].pio[1].pin;
	STPIO_SET_PIN(STM_PIO_BASE(port), pin, (val) ? 1 : 0);
}

extern int stxh407_i2c_read(void)
{
	/* SSC's MTSR == I2C's SDA */
	const int port = ssc_pios[CONFIG_I2C_BUS].pio[1].port;
	const int pin  = ssc_pios[CONFIG_I2C_BUS].pio[1].pin;
	return STPIO_GET_PIN(STM_PIO_BASE(port), pin);
}
#endif	/* defined(CONFIG_CMD_I2C) && defined(CONFIG_SOFT_I2C) */


#if defined(CONFIG_I2C_CMD_TREE)
extern unsigned int i2c_get_bus_speed(void)
{
	return CONFIG_SYS_I2C_SPEED;
}
extern int i2c_set_bus_speed(unsigned int speed)
{
	return -1;
}
#endif	/* CONFIG_I2C_CMD_TREE */


#if defined(CONFIG_SPI) && defined(CONFIG_SOFT_SPI)
	/*
	 * We want to use "bit-banging" for SPI (not SSC, nor FSM).
	 */
#define	SPI_CLK		40, 1		/* SPI Clock */
#define	SPI_nCS		40, 0		/* SPI not-Chip-Select */
#define	SPI_MISO	40, 3		/* Data: Master In, Slave Out */
#define	SPI_MOSI	40, 2		/* Data: Master Out, Slave In */

extern void stxh407_configure_spi(void)
{
	/*
	 *	We set up the PIO pins correctly for SPI
	 */

	/* route PIO (alternate #0) */
	STXH407_PIOALT_SELECT(SPI_MISO, 0);	/* SPI_MISO */
	STXH407_PIOALT_SELECT(SPI_MOSI, 0);	/* SPI_MOSI */
	STXH407_PIOALT_SELECT(SPI_nCS,  0);	/* SPI_nCS */
	STXH407_PIOALT_SELECT(SPI_CLK,  0);	/* SPI_CLK */

	/* set PIO directionality */
	SET_PIO_PIN2(SPI_MISO, STPIO_IN);	/* SPI_MISO */
	SET_PIO_PIN2(SPI_MOSI, STPIO_OUT);	/* SPI_MOSI */
	SET_PIO_PIN2(SPI_nCS,  STPIO_OUT);	/* SPI_nCS */
	SET_PIO_PIN2(SPI_CLK,  STPIO_OUT);	/* SPI_CLK */

	/* drive outputs with sensible initial values */
	STPIO_SET_PIN2(SPI_MOSI, 0);		/* deassert SPI_MOSI */
	STPIO_SET_PIN2(SPI_nCS,  1);		/* deassert SPI_nCS */
	STPIO_SET_PIN2(SPI_CLK,  1);		/* assert SPI_CLK */
}

extern void stxh407_spi_scl(const int val)
{
	STPIO_SET_PIN2(SPI_CLK, val ? 1 : 0);
}

extern void stxh407_spi_sda(const int val)
{
	STPIO_SET_PIN2(SPI_MOSI, val ? 1 : 0);
}

extern unsigned char stxh407_spi_read(void)
{
	return STPIO_GET_PIN2(SPI_MISO);
}

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
	/* assert SPI CSn */
	STPIO_SET_PIN2(SPI_nCS, 0);

	/* wait 1us for CSn assert to propagate  */
	udelay(1);
}
extern void spi_cs_deactivate(struct spi_slave * const slave)
{
	/* DE-assert SPI CSn */
	STPIO_SET_PIN2(SPI_nCS, 1);
}
#endif	/* defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI) */

#endif	/* CONFIG_SPI && CONFIG_SOFT_SPI */

