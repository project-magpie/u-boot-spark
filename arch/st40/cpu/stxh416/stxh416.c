/*
 * (C) Copyright 2008-2013 STMicroelectronics.
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
#include <asm/soc.h>
#include <asm/stxh416reg.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <asm/stbus.h>
#include <asm/sysconf.h>
#include <asm/pad.h>
#include <asm/pio-control.h>
#include <ata.h>
#include <spi.h>


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
	 * The STxH416 is only supported in 32-bit (SE) mode,
	 * and is not supported in 29-bit (legacy) mode.
	 * We refuse to build, if this assertion is invalid.
	 */
#if !defined(CONFIG_ST40_SE_MODE)
#error This SoC is not supported in 29-bit mode, please enable SE-mode!
#endif	/* !CONFIG_ST40_SE_MODE */


static void stxh416_clocks(void)
{
	bd_t * const bd = gd->bd;

	/*
	 * Ideally, we should probe to determine all the clock frequencies.
	 * However, for simplicity, we will simply hard-wire the values
	 * that U-Boot will use for computing the clock dividers later.
	 * WARNING: Getting these values wrong may result in strange behaviour!
	 *
	 * Note: for the ASC in the SBC, we expect this will always be 30MHz,
	 *       otherwise we expect the ASC to be 100MHz.
	 */
#if (CONFIG_SYS_STM_ASC_BASE==STXH416_SBC_ASC0_BASE) || (CONFIG_SYS_STM_ASC_BASE==STXH416_SBC_ASC1_BASE)
	bd->bi_uart_frq =  30ul * 1000000ul;	/*  30 MHz */
#else
	bd->bi_uart_frq = 100ul * 1000000ul;	/* 100 MHz */
#endif
	bd->bi_tmu_frq  = 100ul * 1000000ul;	/* 100 MHz */
	bd->bi_ssc_frq  = bd->bi_tmu_frq;
}


/*
 * PIO alternative Function selector
 */
extern void stxh416_pioalt_select(int port, const int pin, const int alt)
{
	unsigned long sysconf, *sysconfReg;

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("%s(port=%d, pin=%d, alt=%d)\n", __func__, port, pin, alt);
	BUG_ON(pin < 0 || pin > 7);
	BUG_ON(alt < 0 || alt > 5);
#endif

	switch (port)
	{
	case 40:		/* in the "SBC Bank", port #40 is logically port #5 */
		port = 5;	/* fallthru */
	case 0 ... 4:		/* in "SBC Bank" */
		sysconfReg = (unsigned long*)STXH416_SYSCFG(0);
		sysconfReg += port;
		break;
	case 5 ... 12:		/* in "FRONT Bank" */
		sysconfReg = (unsigned long*)STXH416_SYSCFG(1000);
		sysconfReg += port - 5;
		break;
	case 13 ... 18:		/* in "REAR Bank" */
		sysconfReg = (unsigned long*)STXH416_SYSCFG(2000);
		sysconfReg += port - 13;
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

extern void stxh416_pioalt_pad(int port, const int pin,
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
	case 40:		/* in the "SBC Bank", port #40 is logically port #5 */
		port = 5;	/* fallthru */
	case 0 ... 4:		/* in "SBC Bank" */
		sysconfReg = (unsigned long*)STXH416_SYSCFG(40);
		sysconfReg += port / 4;
		break;
	case 5 ... 12:		/* in "FRONT Bank" */
		port -= 5;
		sysconfReg = (unsigned long*)STXH416_SYSCFG(1040);
		sysconfReg += port / 4;
		break;
	case 13 ... 18:		/* in "REAR Bank" */
		port -= 13;
		sysconfReg = (unsigned long*)STXH416_SYSCFG(2040);
		sysconfReg += port / 4;
		break;
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

extern void stxh416_pioalt_retime(int port, const int pin,
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
	case 40:		/* in the "SBC Bank", port #40 is logically port #5 */
		port = 5;	/* fallthru */
	case 0 ... 4:		/* in "SBC Bank" */
		sysconfReg = (unsigned long*)STXH416_SYSCFG(100);
		sysconfReg += (port-0) * 8;
		break;
	case 5 ... 12:		/* in "FRONT Bank" */
		sysconfReg = (unsigned long*)STXH416_SYSCFG(1100);
		sysconfReg += (port-5) * 8;
		break;
	case 13 ... 18:		/* in "REAR Bank" */
		sysconfReg = (unsigned long*)STXH416_SYSCFG(2100);
		sysconfReg += (port-13) * 8;
		break;
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


#define SYSCONF(_reg)	((unsigned long*)STXH416_SYSCFG(_reg))


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
		.phy_clock = 1, \
		.direction = stm_pad_direction_unknown, \
		.retime    = _retiming, \
	}


#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
static struct stm_gmac_pin stxh416_ethernet_mii_pad_configs[] = {
			DATA_OUT(14, 0, 2, RET_SE_NICLK_IO(0, 0)),/* TXD[0] */
			DATA_OUT(14, 1, 2, RET_SE_NICLK_IO(0, 0)),/* TXD[1] */
			DATA_OUT(14, 2, 2, RET_SE_NICLK_IO(0, 1)),/* TXD[2] */
			DATA_OUT(14, 3, 2, RET_SE_NICLK_IO(0, 1)),/* TXD[3] */
			DATA_OUT(15, 1, 2, RET_SE_NICLK_IO(0, 0)),/* TXER */
			DATA_OUT(13, 7, 2, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			CLOCK_IN(15, 0, 2, RET_NICLK(0)),/* TXCLK */
			DATA_IN(15, 3, 2, RET_BYPASS(2)),/* COL */
			DATA_OUT(15, 4, 2, RET_BYPASS(3)),/* MDIO*/
			CLOCK_OUT(15, 5, 2, RET_NICLK(1)),/* MDC */
			DATA_IN(15, 2, 2, RET_BYPASS(2)),/* CRS */
			DATA_IN(13, 6, 2, RET_BYPASS(0)),/* MDINT */
			DATA_IN(16, 0, 2, RET_SE_NICLK_IO(0, 0)),/* 5 RXD[0] */
			DATA_IN(16, 1, 2, RET_SE_NICLK_IO(0, 0)),/* RXD[1] */
			DATA_IN(16, 2, 2, RET_SE_NICLK_IO(0, 0)),/* RXD[2] */
			DATA_IN(16, 3, 2, RET_SE_NICLK_IO(0, 0)),/* RXD[3] */
			DATA_IN(15, 6, 2, RET_SE_NICLK_IO(0, 0)),/* RXDV */
			DATA_IN(15, 7, 2, RET_SE_NICLK_IO(0, 0)),/* RX_ER */
			CLOCK_IN(17, 0, 2, RET_NICLK(0)),/* RXCLK */
			PHY_CLOCK(13, 5, 2, RET_NICLK(1)),/* PHYCLK */
};
static struct stm_pad_sysconf stxh416_ethernet_mii_pad_sysconfs[] = {
			/* ETH0_GMAC_EN */
			STM_PAD_SYSCONF(SYSCONF(1539), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(2559), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(2559), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(2559), 6, 6, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(2559), 8, 8, 0),
};
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
static struct stm_gmac_pin stxh416_ethernet_mii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[1] */
			DATA_OUT(0, 2, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[2] */
			DATA_OUT(0, 3, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[3] */
			DATA_OUT(0, 4, 1, RET_SE_NICLK_IO(0, 0)),/* TXER */
			DATA_OUT(0, 5, 1, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			CLOCK_IN(0, 6, 1, RET_NICLK(0)),/* TXCLK */
			DATA_IN(0, 7, 1, RET_BYPASS(2)),/* COL */
			DATA_OUT(1, 0, 1, RET_BYPASS(0)),/* MDIO */
			CLOCK_OUT(1, 1, 1, RET_NICLK(0)),/* MDC */
			DATA_IN(1, 2, 1, RET_BYPASS(2)),/* CRS */
			DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
			DATA_IN(1, 4, 1, RET_SE_NICLK_IO(0, 0)),/* RXD[0] */
			DATA_IN(1, 5, 1, RET_SE_NICLK_IO(0, 0)),/* RXD[1] */
			DATA_IN(1, 6, 1, RET_SE_NICLK_IO(0, 0)),/* RXD[2] */
			DATA_IN(1, 7, 1, RET_SE_NICLK_IO(0, 0)),/* RXD[3] */
			DATA_IN(2, 0, 1, RET_SE_NICLK_IO(0, 0)),/* RXDV */
			DATA_IN(2, 1, 1, RET_SE_NICLK_IO(0, 0)),/* RX_ER */
			CLOCK_IN(2, 2, 1, RET_NICLK(0)),/* RXCLK */
			PHY_CLOCK(2, 3, 1, RET_NICLK(1)),/* PHYCLK */
};
static struct stm_pad_sysconf stxh416_ethernet_mii_pad_sysconfs[] = {
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
#endif				/* CONFIG_SYS_STM_STMAC_BASE == GMAC #1 */

#if 0
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
static struct stm_gmac_pin stxh416_ethernet_gmii_pad_configs[] = {
			PHY_CLOCK(13, 5, 4, RET_NICLK(1)),/* GTXCLK */
			DATA_IN(13, 6, 2, RET_BYPASS(0)),/* MDINT */
			DATA_OUT(13, 7, 2, RET_SE_NICLK_IO(3, 0)),/* TXEN */
			DATA_OUT(14, 0, 2, RET_SE_NICLK_IO(3, 0)),/* TXD[0] */
			DATA_OUT(14, 1, 2, RET_SE_NICLK_IO(3, 0)),/* TXD[1] */
			DATA_OUT(14, 2, 2, RET_SE_NICLK_IO(3, 1)),/* TXD[2] */
			DATA_OUT(14, 3, 2, RET_SE_NICLK_IO(3, 1)),/* TXD[3] */
			DATA_OUT(14, 4, 2, RET_SE_NICLK_IO(3, 1)),/* TXD[4] */
			DATA_OUT(14, 5, 2, RET_SE_NICLK_IO(3, 1)),/* TXD[5] */
			DATA_OUT(14, 6, 2, RET_SE_NICLK_IO(3, 1)),/* TXD[6] */
			DATA_OUT(14, 7, 2, RET_SE_NICLK_IO(3, 1)),/* TXD[7] */
			CLOCK_IN(15, 0, 2, RET_NICLK(0)),/* TXCLK */
			DATA_OUT(15, 1, 2, RET_SE_NICLK_IO(3, 0)),/* TXER */
			DATA_IN(15, 2, 2, RET_BYPASS(2)),/* CRS */
			DATA_IN(15, 3, 2, RET_BYPASS(2)),/* COL */
			DATA_OUT_PU(15, 4, 2, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(15, 5, 2, RET_NICLK(1)),/* MDC */
			DATA_IN(15, 6, 2, RET_SE_NICLK_IO(3, 0)),/* RXDV */
			DATA_IN(15, 7, 2, RET_SE_NICLK_IO(3, 0)),/* RX_ER */
			DATA_IN(16, 0, 2, RET_SE_NICLK_IO(3, 0)),/* RXD[0] */
			DATA_IN(16, 1, 2, RET_SE_NICLK_IO(3, 0)),/* RXD[1] */
			DATA_IN(16, 2, 2, RET_SE_NICLK_IO(3, 0)),/* RXD[2] */
			DATA_IN(16, 3, 2, RET_SE_NICLK_IO(3, 0)),/* RXD[3] */
			DATA_IN(16, 4, 2, RET_SE_NICLK_IO(3, 0)),/* RXD[4] */
			DATA_IN(16, 5, 2, RET_SE_NICLK_IO(3, 0)),/* RXD[5] */
			DATA_IN(16, 6, 2, RET_SE_NICLK_IO(3, 0)),/* RXD[6] */
			DATA_IN(16, 7, 2, RET_SE_NICLK_IO(3, 0)),/* RXD[7] */
			CLOCK_IN(17, 0, 2, RET_NICLK(0)),/* RXCLK */
			CLOCK_IN(17, 6, 1, RET_NICLK(0)),/* 125MHZ i/p clk */
};
static struct stm_pad_sysconf stxh416_ethernet_gmii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYSCONF(SYSCONF(1539), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(2559), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(2559), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(2559), 6, 6, 0),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(2559), 8, 8, 0),
};
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
static struct stm_gmac_pin stxh416_ethernet_gmii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_SE_NICLK_IO(3, 0)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_SE_NICLK_IO(3, 0)),/* TXD[1] */
			DATA_OUT(0, 2, 1, RET_SE_NICLK_IO(3, 0)),/* TXD[2] */
			DATA_OUT(0, 3, 1, RET_SE_NICLK_IO(3, 0)),/* TXD[3] */
			DATA_OUT(0, 4, 1, RET_SE_NICLK_IO(3, 0)),/* TXER */
			DATA_OUT(0, 5, 1, RET_SE_NICLK_IO(3, 0)),/* TXEN */
			CLOCK_IN(0, 6, 1, RET_NICLK(0)),/* TXCLK  FIXME*/
			DATA_IN(0, 7, 1, RET_BYPASS(2)),/* COL */
			DATA_OUT_PU(1, 0, 1, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(1, 1, 1, RET_NICLK(0)),/* MDC */
			DATA_IN(1, 2, 1, RET_BYPASS(2)),/* CRS */
			DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
			DATA_IN(1, 4, 1, RET_SE_NICLK_IO(3, 0)),/* RXD[0] */
			DATA_IN(1, 5, 1, RET_SE_NICLK_IO(3, 0)),/* RXD[1] */
			DATA_IN(1, 6, 1, RET_SE_NICLK_IO(3, 0)),/* RXD[2] */
			DATA_IN(1, 7, 1, RET_SE_NICLK_IO(3, 0)),/* RXD[3] */
			DATA_IN(2, 0, 1, RET_SE_NICLK_IO(3, 0)),/* RXDV */
			DATA_IN(2, 1, 1, RET_SE_NICLK_IO(3, 0)),/* RX_ER */
			CLOCK_IN(2, 2, 1, RET_NICLK(0)),/* RXCLK */
			PHY_CLOCK(2, 3, 4, RET_NICLK(1)), /* GTXCLK */
			DATA_OUT(2, 6, 4, RET_SE_NICLK_IO(3, 0)),/* TXD[4] */
			DATA_OUT(2, 7, 4, RET_SE_NICLK_IO(3, 0)),/* TXD[5] */
			DATA_IN(3, 0, 4, RET_SE_NICLK_IO(3, 0)),/* RXD[4] */
			DATA_IN(3, 1, 4, RET_SE_NICLK_IO(3, 0)),/* RXD[5] */
			DATA_IN(3, 2, 4, RET_SE_NICLK_IO(3, 0)),/* RXD[6] */
			DATA_IN(3, 3, 4, RET_SE_NICLK_IO(3, 0)),/* RXD[7] */
			CLOCK_IN(3, 7, 4, RET_NICLK(0)),/* 125MHZ input clock */
			DATA_OUT(4, 1, 4, RET_SE_NICLK_IO(3, 1)),/* TXD[6] */
			DATA_OUT(4, 2, 4, RET_SE_NICLK_IO(3, 1)),/* TXD[7] */
};
static struct stm_pad_sysconf stxh416_ethernet_gmii_pad_sysconfs[] = {
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
#endif				/* CONFIG_SYS_STM_STMAC_BASE == GMAC #1 */
#endif	/* 0 */

#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
static struct stm_gmac_pin stxh416_ethernet_rgmii_pad_configs[] = {
			PHY_CLOCK(13, 5, 2, RET_NICLK(1)),/* GTXCLK */
			DATA_OUT(13, 7, 2, RET_DE_IO(0, 0)),/* TXEN */
			DATA_IN(13, 6, 2, RET_BYPASS(0)), /* MDINT */
			DATA_OUT(14, 0, 2, RET_DE_IO(1, 0)),/* TXD[0] */
			DATA_OUT(14, 1, 2, RET_DE_IO(1, 0)),/* TXD[1] */
			DATA_OUT(14, 2, 2, RET_DE_IO(1, 1)),/* TXD[2] */
			DATA_OUT(14, 3, 2, RET_DE_IO(1, 1)),/* TXD[3] */
			/* TX Clock inversion is not set for 1000Mbps */
			CLOCK_IN(15, 0, 2, RET_ICLK(0)),/* TXCLK */
			MDIO(15, 4, 2, RET_BYPASS(3)),/* MDIO */
			MDC(15, 5, 2, RET_NICLK(1)),/* MDC */
			DATA_IN(15, 6, 2, RET_DE_IO(1, 0)),/* RXDV */
			DATA_IN(16, 0, 2, RET_DE_IO(1, 0)),/* RXD[0] */
			DATA_IN(16, 1, 2, RET_DE_IO(1, 0)),/* RXD[1] */
			DATA_IN(16, 2, 2, RET_DE_IO(1, 0)),/* RXD[2] */
			DATA_IN(16, 3, 2, RET_DE_IO(1, 0)),/* RXD[3] */
			CLOCK_IN(17, 0, 2, RET_NICLK(0)),/* RXCLK */
			CLOCK_IN(17, 6, 1, RET_NICLK(0)),/* 125MHZ i/p clk */
};
static struct stm_pad_sysconf stxh416_ethernet_rgmii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYSCONF(SYSCONF(1539), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(2559), 2, 4, 1),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(2559), 5, 5, 1),
			/* Extra retime config base on speed */
};
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
static struct stm_gmac_pin stxh416_ethernet_rgmii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_DE_IO(1, 0)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_DE_IO(1, 0)),/* TXD[1] */
			DATA_OUT(0, 2, 1, RET_DE_IO(1, 0)),/* TXD[2] */
			DATA_OUT(0, 3, 1, RET_DE_IO(1, 0)),/* TXD[3] */
			DATA_OUT(0, 5, 1, RET_DE_IO(0, 0)),/* TXEN */
			CLOCK_IN(0, 6, 1, RET_NICLK(0)),/* TXCLK */
			MDIO(1, 0, 1, RET_BYPASS(0)),/* MDIO */
			MDC(1, 1, 1, RET_NICLK(0)),/* MDC */
			DATA_IN(1, 4, 1, RET_DE_IO(0, 0)),/* RXD[0] */
			DATA_IN(1, 5, 1, RET_DE_IO(0, 0)),/* RXD[1] */
			DATA_IN(1, 6, 1, RET_DE_IO(0, 0)),/* RXD[2] */
			DATA_IN(1, 7, 1, RET_DE_IO(0, 0)),/* RXD[3] */
			DATA_IN(2, 0, 1, RET_DE_IO(1, 0)),/* RXDV */
			CLOCK_IN(2, 2, 1, RET_NICLK(0)),/* RXCLK */
			PHY_CLOCK(2, 3, 4, RET_NICLK(1)), /* GTXCLK */
			CLOCK_IN(3, 7, 4, RET_NICLK(0)),/* 125MHZ input clock */
};
static struct stm_pad_sysconf stxh416_ethernet_rgmii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(510), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(508), 2, 4, 1),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(508), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(508), 6, 8, 0),
};
#endif				/* CONFIG_SYS_STM_STMAC_BASE == GMAC #1 */

#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
static struct stm_gmac_pin stxh416_ethernet_rmii_pad_configs[] = {
			PHY_CLOCK(13, 5, 2, RET_NICLK(1)),/* PHYCLK */
			DATA_IN(13, 6, 2, RET_BYPASS(0)),/* MDINT */
			DATA_OUT(13, 7, 2, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			DATA_OUT(14, 0, 2, RET_SE_NICLK_IO(0, 0)),/* TXD[0] */
			DATA_OUT(14, 1, 2, RET_SE_NICLK_IO(0, 0)),/* TXD[1] */
			DATA_OUT(15, 4, 2, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(15, 5, 2, RET_NICLK(1)),/* MDC */
			/* NOTE:
			 * retime settings for RX pins is
			 * incorrect in App note */
			DATA_IN(15, 6, 2, RET_SE_NICLK_IO(0, 1)),/* RXDV */
			DATA_IN(15, 7, 2, RET_SE_NICLK_IO(0, 0)),/* RX_ER */
			DATA_IN(16, 0, 2, RET_SE_NICLK_IO(0, 1)),/* RXD.0 */
			DATA_IN(16, 1, 2, RET_SE_NICLK_IO(0, 1)),/* RXD.1 */
};
static struct stm_pad_sysconf stxh416_ethernet_rmii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYSCONF(SYSCONF(1539), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(2559), 2, 4, 4),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(2559), 5, 5, 1),
			/* ETH0_SEL_INTERNAL_NOT_EXT_PHYCLK */
			STM_PAD_SYSCONF(SYSCONF(2559), 7, 7, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(2559), 8, 8, 1),
};
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
static struct stm_gmac_pin stxh416_ethernet_rmii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[1] */
			DATA_OUT(0, 5, 1, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			DATA_OUT(1, 0, 1, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(1, 1, 1, RET_NICLK(0)),/* MDC */
			DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
			DATA_IN(1, 4, 1, RET_SE_NICLK_IO(2, 1)),/* RXD[0] */
			DATA_IN(1, 5, 1, RET_SE_NICLK_IO(2, 1)),/* RXD[1] */
			DATA_IN(2, 0, 1, RET_SE_NICLK_IO(2, 1)),/* RXDV */
			DATA_IN(2, 1, 1, RET_SE_NICLK_IO(2, 0)),/* RX_ER */
			PHY_CLOCK(2, 3, 1, RET_NICLK(1)),/* PHYCLK */
};
static struct stm_pad_sysconf stxh416_ethernet_rmii_pad_sysconfs[] = {
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
#endif				/* CONFIG_SYS_STM_STMAC_BASE == GMAC #1 */

#if 0
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
static struct stm_gmac_pin stxh416_ethernet_reverse_mii_pad_configs[] = {
			DATA_OUT(14, 0, 2, RET_BYPASS(0)),/* TXD[0] */
			DATA_OUT(14, 1, 2, RET_BYPASS(0)),/* TXD[1] */
			DATA_OUT(14, 2, 2, RET_BYPASS(0)),/* TXD[2] */
			DATA_OUT(14, 3, 2, RET_BYPASS(0)),/* TXD[3] */
			DATA_OUT(15, 1, 2, RET_SE_NICLK_IO(0, 0)),/* TXER */
			DATA_OUT(13, 7, 2, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			CLOCK_IN(15, 0, 2, RET_NICLK(0)),/* TXCLK */
			DATA_OUT(15, 3, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(15, 4, 2, RET_BYPASS(2)),/* MDIO*/
			CLOCK_IN(15, 5, 3, RET_NICLK(0)),/* MDC */
			DATA_OUT(15, 2, 3, RET_BYPASS(0)),/* CRS */
			DATA_IN(13, 6, 2, RET_BYPASS(0)),/* MDINT */
			DATA_IN(16, 0, 2, RET_SE_NICLK_IO(2, 0)),/* RXD[0] */
			DATA_IN(16, 1, 2, RET_SE_NICLK_IO(2, 0)),/* RXD[1] */
			DATA_IN(16, 2, 2, RET_SE_NICLK_IO(2, 0)),/* RXD[2] */
			DATA_IN(16, 3, 2, RET_SE_NICLK_IO(2, 0)),/* RXD[3] */
			DATA_IN(15, 6, 2, RET_SE_NICLK_IO(2, 0)),/* RXDV */
			DATA_IN(15, 7, 2, RET_SE_NICLK_IO(2, 0)),/* RX_ER */
			CLOCK_IN(17, 0, 2, RET_NICLK(0)),/* RXCLK */
			PHY_CLOCK(13, 5, 2, RET_NICLK(0)),/* PHYCLK */
};
static struct stm_pad_sysconf stxh416_ethernet_reverse_mii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYSCONF(SYSCONF(1539), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(2559), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(2559), 5, 5, 0),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(2559), 6, 6, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(2559), 8, 8, 0),
};
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
static struct stm_gmac_pin stxh416_ethernet_reverse_mii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[1] */
			DATA_OUT(0, 2, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[2] */
			DATA_OUT(0, 3, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[3] */
			DATA_OUT(0, 4, 1, RET_SE_NICLK_IO(0, 1)),/* TXER */
			DATA_OUT(0, 5, 1, RET_SE_NICLK_IO(0, 1)),/* TXEN */
			CLOCK_IN(0, 6, 1, RET_NICLK(0)),/* TXCLK */
			DATA_OUT(0, 7, 2, RET_BYPASS(0)),/* COL */
			DATA_IN(2, 0, 1, RET_SE_NICLK_IO(2, 1)),/* RXDV */
			DATA_IN(2, 1, 1, RET_SE_NICLK_IO(2, 1)),/* RX_ER */
			CLOCK_IN(2, 2, 1, RET_NICLK(0)),/* RXCLK */
			PHY_CLOCK(2, 3, 1, RET_NICLK(0)),/* PHYCLK */
			DATA_OUT_PU(1, 0, 1, RET_BYPASS(2)),/* MDIO */
			CLOCK_IN(1, 1, 2, RET_NICLK(1)),/* MDC */
			DATA_OUT(1, 2, 2, RET_BYPASS(0)),/* CRS */
			DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
			DATA_IN(1, 4, 1, RET_SE_NICLK_IO(2, 1)),/* RXD[0] */
			DATA_IN(1, 5, 1, RET_SE_NICLK_IO(2, 1)),/* RXD[1] */
			DATA_IN(1, 6, 1, RET_SE_NICLK_IO(2, 1)),/* RXD[2] */
			DATA_IN(1, 7, 1, RET_SE_NICLK_IO(2, 1)),/* RXD[3] */
};
static struct stm_pad_sysconf stxh416_ethernet_reverse_mii_pad_sysconfs[] = {
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
#endif				/* CONFIG_SYS_STM_STMAC_BASE == GMAC #1 */
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
static void stxh416_ethernet_bus_setup(void)
{
	/* Configure the bridge to generate more efficient STBus traffic.
	 *
	 * Cut Version	| Ethernet AD_CONFIG[21:0]
	 *	1.0	|	0x00264207
	 */
	writel(0x00264207, CONFIG_SYS_STM_STMAC_BASE + GMAC_AHB_CONFIG);
}


/* ETH MAC pad configuration */
extern void stxh416_configure_ethernet(
	const int port,
	const struct stxh416_ethernet_config * const config)
{
	struct stm_gmac_pin * pad_config;
	struct stm_gmac_pin * phy_clock;
	const struct stm_pad_sysconf * sys_configs;
	size_t num_pads, num_sys, i;

	BUG_ON(!config);
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
	BUG_ON(port != 0);
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
	BUG_ON(port != 1);
#else
#error Unknown base address for the STM GMAC
#endif

	switch (config->mode) {

	case stxh416_ethernet_mode_mii:
		pad_config = stxh416_ethernet_mii_pad_configs;
		num_pads = ARRAY_SIZE(stxh416_ethernet_mii_pad_configs);
		sys_configs = stxh416_ethernet_mii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh416_ethernet_mii_pad_sysconfs);
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);
		if (config->ext_clk)
			stm_pad_set_pio_ignored(phy_clock);
		else
			stm_pad_set_pio_out(phy_clock, 2 - port);
		break;

	case stxh416_ethernet_mode_rmii:  {
		pad_config = stxh416_ethernet_rmii_pad_configs;
		num_pads = ARRAY_SIZE(stxh416_ethernet_rmii_pad_configs);
		sys_configs = stxh416_ethernet_rmii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh416_ethernet_rmii_pad_sysconfs);

		/* we assume we *always* use the internal clock for RMII! */
		BUG_ON(config->ext_clk!= 0);

		/*
		 * Configure the PHY Clock:
		 * CLKS_GMAC0_PHY (PIO13[5]) or CLKS_ETH1_PHY (PIO2[3]) should
		 * be 50MHz for RMII mode (as appropriate).
		 * Note: We rely on the GDB "pokes" to set the frequency for us ...
		 */
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);
		stm_pad_set_pio_out(phy_clock, 2 - port);
	} break;

	case stxh416_ethernet_mode_rgmii:
		/* This mode is similar to GMII (GTX) except the data
		 * buses are reduced down to 4 bits and the 2 error
		 * signals are removed. The data rate is maintained by
		 * using both edges of the clock. This also explains
		 * the different retiming configuration for this mode.
		 */
		pad_config = stxh416_ethernet_rgmii_pad_configs;
		num_pads = ARRAY_SIZE(stxh416_ethernet_rgmii_pad_configs);
		sys_configs = stxh416_ethernet_rgmii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh416_ethernet_rgmii_pad_sysconfs);
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);
		stm_pad_set_pio_out(phy_clock, 4);
		break;

	case stxh416_ethernet_mode_reverse_mii:
		BUG();		/* assume not required in U-Boot */
		return;

	case stxh416_ethernet_mode_gmii:
		BUG();		/* assume not required in U-Boot */
		return;

	case stxh416_ethernet_mode_gmii_gtx:
		BUG();		/* assume not required in U-Boot */
		return;

	case stxh416_ethernet_mode_rgmii_gtx:
		BUG();		/* assume not required in U-Boot */
		return;

	default:
		BUG();
		return;
	}

		/* now configure all the PIOs */
	for (i = 0; i < num_pads; i++)
	{
		const struct stm_gmac_pin * const pad = &pad_config[i];
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

		stxh416_pioalt_select(portno, pinno, pad->pio.alt);
		stxh416_pioalt_pad(portno, pinno, pad->direction);
		stxh416_pioalt_retime(portno, pinno, pad->retime, pad->direction);
	}

		/* now configure the relevant SYS_CONFIGs */
	stm_configure_sysconfs(sys_configs, num_sys);

		/* need to do this *after* the SYSCONF initialization! */
	stxh416_ethernet_bus_setup();
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */


extern int soc_init(void)
{
	bd_t * const bd = gd->bd;

	stxh416_clocks();

	bd->bi_devid = *STXH416_SYSCONF_DEVICEID;

#if 0	/* QQQ - TO IMPLEMENT */
	/* Make sure reset period is shorter than WDT time-out */
	*STX7108_BANK0_SYSCFG(14) = 3000;	/* about 100 us */
#endif	/* QQQ - TO IMPLEMENT */

	return 0;
}


#if defined(CONFIG_USB_OHCI_NEW) || defined(CONFIG_USB_EHCI)
extern int stxh416_usb_init(const int port)
{
	static int initialized = 0;
	unsigned int flags;
	const struct {
		struct {
			unsigned char port, pin, alt;
		} oc, pwr;
	} usb_pins[] = {
		{ .oc = {  9, 4, 1 }, .pwr = {  9, 5, 1 } },
		{ .oc = { 18, 0, 1 }, .pwr = { 18, 1, 1 } },
		{ .oc = { 18, 2, 1 }, .pwr = { 18, 3, 1 } },
		{ .oc = { 40, 0, 1 }, .pwr = { 40, 1, 1 } },
	};

	if (port < 0 || port >= 4)	/* invalid port number ? */
		return -1;		/* failed to initialize! */

		/* Set the USB clock to 48MHz */
		/* QQQ - this probably should be done in the TargetPacks! */
	*STXH416_SYSCFG(1557) = (1<<10)|(1<<14)|(1<<18);/* CKGB_FS0_FSYN_CFG */
	*STXH416_SYSCFG(1558) = 0x00000013;		/* CKGB_FS0_MD0 */
	*STXH416_SYSCFG(1559) = 0x00000000;		/* CKGB_FS0_PE0 */
	*STXH416_SYSCFG(1561) = 0x00000000;		/* CKGB_FS0_EN_PRG0 */
	*STXH416_SYSCFG(1560) = 0x00000002;		/* CKGB_FS0_SDIV0 */

	if (!initialized)		/* first time ? */
	{	/* set USB2TRIPPHY_OSCIOK = 1 */
		unsigned long * const sysconfReg = (unsigned long*)STXH416_SYSCFG(2520);
		unsigned long sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, TRUE, 6);
		writel(sysconf, sysconfReg);
		initialized = 1;	/* do this just once */
	}

	/* Power up the USB port, i.e. set USB_x_POWERDOWN_REQ = 0 */
	{
		unsigned long * const sysconfReg = (unsigned long*)STXH416_SYSCFG(2525);
		unsigned long sysconf = readl(sysconfReg);
		unsigned int const req = (port==3) ? 6 : port;
		SET_SYSCONF_BIT(sysconf, FALSE, req);
		writel(sysconf, sysconfReg);
	}

	/* wait until USB[x]_POWERDOWN_ACK == 0 */
	{
		unsigned int const ack = (port==3) ? 5 : port;
		while (*STXH416_SYSCFG(2583) & (1u<<ack))
			;	/* just wait ... */
	}

	/* Configure USB: in DC shift, and in edge control */
	{
		unsigned long * const sysconfReg = (unsigned long*)STXH416_SYSCFG(2520);
		unsigned long sysconf = readl(sysconfReg);
		unsigned int const dc = (port==3) ? 8 : port;
		unsigned int const ec = (port==3) ? 9 : port + 3;
		SET_SYSCONF_BIT(sysconf, FALSE, dc);	/* in DC shift */
		SET_SYSCONF_BIT(sysconf, TRUE,  ec);	/* in edge control */
		writel(sysconf, sysconfReg);
	}

	/* route the USB power enable (output) signal */
	stxh416_pioalt_select(usb_pins[port].pwr.port,
			    usb_pins[port].pwr.pin,
			    usb_pins[port].pwr.alt);
	stxh416_pioalt_pad(usb_pins[port].pwr.port,
			  usb_pins[port].pwr.pin, stm_pad_direction_output);

	/* route the USB over-current (input) signal */
	stxh416_pioalt_select(usb_pins[port].oc.port,
			    usb_pins[port].oc.pin,
			    usb_pins[port].oc.alt);
	stxh416_pioalt_pad(usb_pins[port].oc.port,
			  usb_pins[port].oc.pin, stm_pad_direction_input);

	/* start the USB Wrapper Host Controller */
	flags = USB_FLAGS_STRAP_8BIT | USB_FLAGS_STBUS_CONFIG_THRESHOLD128;
	ST40_start_host_control(flags);

	return 0;
}
#endif /* defined(CONFIG_USB_OHCI_NEW) || defined(CONFIG_USB_EHCI) */


#if defined(CONFIG_ST40_STM_SATA)
extern void stxh416_configure_sata(void)
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
#endif	/* CONFIG_ST40_STM_SATA */


#if defined(CONFIG_CMD_I2C) && defined(CONFIG_SOFT_I2C)
struct ssc_pios
{
	struct
	{
		char port;
		char pin;
	}	pio[3];
};
static const struct ssc_pios ssc_pios[7+3] =
{
	{ { /* SSC0 */
		{ 9, 2 }, /* SCLK */
		{ 9, 3 }, /* MTSR */
		{ 9, 6 }, /* MRST */
	} }, { { /* SSC1 */
		{ 12, 0 }, /* SCLK */
		{ 12, 1 }, /* MTSR */
		{ 11, 7 }, /* MRST */
	} }, { { /* SSC2 */
		{ 8, 6 }, /* SCLK */
		{ 8, 7 }, /* MTSR */
		{ 8, 5 }, /* MRST */
	} }, { { /* SSC3 */
		{ 13, 0 }, /* SCLK */
		{ 13, 1 }, /* MTSR */
		{ 13, 2 }, /* MRST */
	} }, { { /* SSC4 */
		{ 10, 5 }, /* SCLK */
		{ 10, 6 }, /* MTSR */
		{ 10, 7 }, /* MRST */
	} }, { { /* SSC5 */
		{ 14, 4 }, /* SCLK */
		{ 14, 5 }, /* MTSR */
		{ 14, 6 }, /* MRST */
	} }, { { /* SSC6 */
		{ 17, 1 }, /* SCLK */
		{ 17, 2 }, /* MTSR */
		{ 17, 3 }, /* MRST */
	} }, { { /* SBC_SSC0 */
		{ 4, 5 }, /* SCLK */
		{ 4, 6 }, /* MTSR */
		{ 4, 7 }, /* MRST */
	} }, { { /* SBC_SSC1 */
		{ 3, 2 }, /* SCLK */
		{ 3, 1 }, /* MTSR */
		{ 3, 0 }, /* MRST */
	} }, { { /* SBC_SSC2 */
		{ 3, 7 }, /* SCLK */
		{ 3, 6 }, /* MTSR */
		{ 3, 4 }, /* MRST */
	} },
};

extern void stxh416_configure_i2c(void)
{
	/*
	 * The I2C buses are routed as follows:
	 *
	 *	Bus	  SCL		  SDA
	 *	---	  ---		  ---
	 *	 0	PIO9[2]		PIO9[3]		SSC #0
	 *	 1	PIO12[0]	PIO12[1]	SSC #1
	 *	 2	PIO8[6]		PIO8[7]		SSC #2
	 *	 3	PIO13[0]	PIO13[1]	SSC #3
	 *	 4	PIO10[5]	PIO10[6]	SSC #4
	 *	 5	PIO14[4]	PIO14[5]	SSC #5
	 *	 6	PIO17[1]	PIO17[2]	SSC #6
	 *	 7	PIO4[5]		PIO4[6]		SBC_SSC #0
	 *	 8	PIO3[2]		PIO3[1]		SBC_SSC #1
	 *	 9	PIO3[7]		PIO3[6]		SBC_SSC #2
	 */
	const int scl_port = ssc_pios[CONFIG_I2C_BUS].pio[0].port;
	const int scl_pin  = ssc_pios[CONFIG_I2C_BUS].pio[0].pin;
	const int sda_port = ssc_pios[CONFIG_I2C_BUS].pio[1].port;
	const int sda_pin  = ssc_pios[CONFIG_I2C_BUS].pio[1].pin;

	if (CONFIG_I2C_BUS >= ARRAY_SIZE(ssc_pios)) BUG();

	/* route PIO (explicitly alternate #0) */
	stxh416_pioalt_select(scl_port, scl_pin, 0);			/* I2C_SCL */
	stxh416_pioalt_select(sda_port, sda_pin, 0);			/* I2C_SDA */

	/* set up directionality appropriately */
	SET_PIO_PIN(ST40_PIO_BASE(scl_port), scl_pin, STPIO_BIDIR);	/* I2C_SCL */
	SET_PIO_PIN(ST40_PIO_BASE(sda_port), sda_pin, STPIO_BIDIR);	/* I2C_SDA */
}

extern void stxh416_i2c_scl(const int val)
{
	/* SSC's SCLK == I2C's SCL */
	const int port = ssc_pios[CONFIG_I2C_BUS].pio[0].port;
	const int pin  = ssc_pios[CONFIG_I2C_BUS].pio[0].pin;
	STPIO_SET_PIN(ST40_PIO_BASE(port), pin, (val) ? 1 : 0);
}

extern void stxh416_i2c_sda(const int val)
{
	/* SSC's MTSR == I2C's SDA */
	const int port = ssc_pios[CONFIG_I2C_BUS].pio[1].port;
	const int pin  = ssc_pios[CONFIG_I2C_BUS].pio[1].pin;
	STPIO_SET_PIN(ST40_PIO_BASE(port), pin, (val) ? 1 : 0);
}

extern int stxh416_i2c_read(void)
{
	/* SSC's MTSR == I2C's SDA */
	const int port = ssc_pios[CONFIG_I2C_BUS].pio[1].port;
	const int pin  = ssc_pios[CONFIG_I2C_BUS].pio[1].pin;
	return STPIO_GET_PIN(ST40_PIO_BASE(port), pin);
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
#define	SPI_CLK		12, 2		/* SPI Clock */
#define	SPI_nCS		12, 3		/* SPI not-Chip-Select */
#define	SPI_MISO	12, 4		/* Data: Master In, Slave Out */
#define	SPI_MOSI	12, 5		/* Data: Master Out, Slave In */

extern void stxh416_configure_spi(void)
{
	/*
	 *	We set up the PIO pins correctly for SPI
	 */

	/* route PIO (alternate #0) */
	STXH416_PIOALT_SELECT(SPI_MISO, 0);	/* SPI_MISO */
	STXH416_PIOALT_SELECT(SPI_MOSI, 0);	/* SPI_MOSI */
	STXH416_PIOALT_SELECT(SPI_nCS,  0);	/* SPI_nCS */
	STXH416_PIOALT_SELECT(SPI_CLK,  0);	/* SPI_CLK */

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

extern void stxh416_spi_scl(const int val)
{
	STPIO_SET_PIN2(SPI_CLK, val ? 1 : 0);
}

extern void stxh416_spi_sda(const int val)
{
	STPIO_SET_PIN2(SPI_MOSI, val ? 1 : 0);
}

extern unsigned char stxh416_spi_read(void)
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

