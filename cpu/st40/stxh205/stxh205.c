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
#include <asm/stxh205reg.h>
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
	 * The STxH205 is only supported in 32-bit (SE) mode,
	 * and is not supported in 29-bit (legacy) mode.
	 * We refuse to build, if this assertion is invalid.
	 */
#if !defined(CONFIG_ST40_SE_MODE)
#error This SoC is not supported in 29-bit mode, please enable SE-mode!
#endif	/* !CONFIG_ST40_SE_MODE */


static void stxh205_clocks(void)
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
#if (CONFIG_SYS_STM_ASC_BASE==STXH205_ASC10_BASE) || (CONFIG_SYS_STM_ASC_BASE==STXH205_ASC11_BASE)
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
extern void stxh205_pioalt_select(const int port, const int pin, const int alt)
{
	unsigned long sysconf, *sysconfReg;

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("%s(port=%d, pin=%d, alt=%d)\n", __func__, port, pin, alt);
	BUG_ON(pin < 0 || pin > 7);
	BUG_ON(alt < 0 || alt > 6);
#endif

	switch (port)
	{
	case 0 ... 3:		/* in Bank #0 */
		sysconfReg = (unsigned long*)STXH205_SYSCFG(0);
		sysconfReg += port;
		break;
	case 4 ... 12:		/* in Bank #1 */
		sysconfReg = (unsigned long*)STXH205_SYSCFG(100);
		sysconfReg += port - 4;
		break;
	case 13 ... 15:		/* in Bank #2 */
		sysconfReg = (unsigned long*)STXH205_SYSCFG(200);
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

void stxh205_pioalt_pad(int port, const int pin,
		const enum stm_pad_gpio_direction direction)
{
	int bit;
	int oe=0, pu=0, od=0;
	int stride = 1;		/* assume all registers are adjacent */
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
	case 0 ... 3:		/* in Bank #0 */
		sysconfReg = (unsigned long*)STXH205_SYSCFG(4);
		break;
	case 4 ... 12:		/* in Bank #1 */
		port -= 4;
		sysconfReg = (unsigned long*)STXH205_SYSCFG(109);
		sysconfReg += port / 4;
		stride = 3;	/* number of registers to skip */
		break;
	case 13 ... 15:		/* in Bank #2 */
		port -= 13;
		sysconfReg = (unsigned long*)STXH205_SYSCFG(203);
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

	sysconfReg += stride;	/* skip to next set of syscfg registers */

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

	sysconfReg += stride;	/* skip to next set of syscfg registers */

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

static void stxh205_pioalt_retime(const int port, const int pin,
		const struct stm_pio_control_retime_config * const cfg)
{
	unsigned long sysconf, *sysconfReg;

#ifdef DEBUG_PAD_CONFIGS
	unsigned long onEntry[2];
	BUG_ON(!cfg);
	if (debug_pad_configs)
		printf("%s(port=%d, pin=%d, retime=%d, clk=%d, "
			"clknotdata=%d, double_edge=%d, invertclk=%d, "
			"delay=%d)\n", __func__, port, pin,
			cfg->retime, cfg->clk, cfg->clknotdata,
			cfg->double_edge, cfg->invertclk, cfg->delay
			);
	BUG_ON(pin < 0 || pin > 7);
	BUG_ON(cfg->retime < 0);	/* the "don't care" semantic is deprecated */
	BUG_ON(cfg->clk < 0);		/* the "don't care" semantic is deprecated */
	BUG_ON(cfg->clknotdata < 0);	/* the "don't care" semantic is deprecated */
	BUG_ON(cfg->double_edge < 0);	/* the "don't care" semantic is deprecated */
	BUG_ON(cfg->invertclk < 0);	/* the "don't care" semantic is deprecated */
	BUG_ON(cfg->delay < 0);		/* the "don't care" semantic is deprecated */
#endif

	switch (port)
	{
	case 0 ... 3:		/* in Bank #0 */
		sysconfReg = (unsigned long*)STXH205_SYSCFG(7);
		sysconfReg += (port-0) * 2;
		break;
	case 4 ... 12:		/* in Bank #1 */
		sysconfReg = (unsigned long*)STXH205_SYSCFG(118);
		sysconfReg += (port-4) * 2;
		break;
	case 13 ... 15:		/* in Bank #2 */
		sysconfReg = (unsigned long*)STXH205_SYSCFG(206);
		sysconfReg += (port-13) * 2;
		break;
	default:
		BUG();
		return;
	}

	sysconfReg += 0;	/* use retime configuration register #0 */
#ifdef DEBUG_PAD_CONFIGS
	onEntry[0] = sysconfReg[0];
	onEntry[1] = sysconfReg[1];
#endif

	if (cfg->clk >= 0)
	{
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, cfg->clk, 0 + pin);
		writel(sysconf, sysconfReg);
	}

	if (cfg->delay >= 0)
	{			/* map value to 2 adjacent bitfields */
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, (cfg->delay >> 0) & 0x1, 16 + pin);
		SET_SYSCONF_BIT(sysconf, (cfg->delay >> 1) & 0x1, 24 + pin);
		writel(sysconf, sysconfReg);
	}

	sysconfReg += 1;	/* use retime configuration register #1 */

	if (cfg->invertclk >= 0)
	{
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, cfg->invertclk, 0 + pin);
		writel(sysconf, sysconfReg);
	}

	if (cfg->retime >= 0)
	{
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, cfg->retime, 8 + pin);
		writel(sysconf, sysconfReg);
	}

	if (cfg->clknotdata >= 0)
	{
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, cfg->clknotdata, 16 + pin);
		writel(sysconf, sysconfReg);
	}

	if (cfg->double_edge >= 0)
	{
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, cfg->double_edge, 24 + pin);
		writel(sysconf, sysconfReg);
	}

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("RETIMING: %p:%p   old = %08lx:%08lx  ->  new = %08x:%08x\n",
			sysconfReg-1, sysconfReg,
			onEntry[0], onEntry[1],
			readl(sysconfReg-1), readl(sysconfReg));
#endif
}


/* --------------------------------------------------------------------
 *		Ethernet MAC resources (PAD and Retiming)
 * --------------------------------------------------------------------*/


#define SYSCONF(_reg)	((unsigned long*)STXH205_SYSCFG(_reg))


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

/* Give TXER an additional "flag" so we can refer to it later. */
#define TXER(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.txer      = 1, \
		.direction = stm_pad_direction_output, \
		.retime    = _retiming, \
	}

/*
 * On some boards the MDIO line is missing a pull-up resistor. Enabling
 * weak internal pull-up overcomes the issue.
 */
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

/* Give PHY_CLOCK an additional "flag" so we can refer to it later. */
#define PHY_CLOCK(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.phy_clock = 1, \
		.direction = stm_pad_direction_unknown, \
		.retime    = _retiming, \
	}


static struct stm_gmac_pin stxh205_ethernet_mii_pad_configs[] = {
		DATA_OUT(0, 0, 1, RET_BYPASS(0)),/* TXD[0] */
		DATA_OUT(0, 1, 1, RET_BYPASS(0)),/* TXD[1] */
		DATA_OUT(0, 2, 1, RET_BYPASS(0)),/* TXD[2] */
		DATA_OUT(0, 3, 1, RET_BYPASS(0)),/* TXD[3] */
		TXER(0, 4, 1, RET_BYPASS(0)),/* TXER */
		DATA_OUT(0, 5, 1, RET_BYPASS(0)),/* TXEN */
		CLOCK_IN(0, 6, 1, RET_NICLK(0)),/* TXCLK */
		DATA_IN(0, 7, 1, RET_BYPASS(0)),/* COL */
		DATA_OUT_PU(1, 0, 1, RET_BYPASS(0)),/* MDIO*/
		CLOCK_OUT(1, 1, 1, RET_NICLK(0)),/* MDC */
		DATA_IN(1, 2, 1, RET_BYPASS(0)),/* CRS */
		DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
		DATA_IN(1, 4, 1, RET_BYPASS(0)),/* RXD[0] */
		DATA_IN(1, 5, 1, RET_BYPASS(0)),/* RXD[1] */
		DATA_IN(1, 6, 1, RET_BYPASS(0)),/* RXD[2] */
		DATA_IN(1, 7, 1, RET_BYPASS(0)),/* RXD[3] */
		DATA_IN(2, 0, 1, RET_BYPASS(0)),/* RXDV */
		DATA_IN(2, 1, 1, RET_BYPASS(0)),/* RX_ER */
		CLOCK_IN(2, 2, 1, RET_NICLK(0)),/* RXCLK */
		PHY_CLOCK(2, 3, 1, RET_NICLK(0)),/* PHYCLK */
};
static struct stm_pad_sysconf stxh205_ethernet_mii_pad_sysconfs[] = {
		/* ETH_POWERDOWN_REQ */
		STM_PAD_SYSCONF(SYSCONF(23), 0, 0, 0),
		/* ETH_MII_PHY_SEL */
		STM_PAD_SYSCONF(SYSCONF(23), 2, 4, 0),
		/* ETH_ENMII */
		STM_PAD_SYSCONF(SYSCONF(23), 5, 5, 1),
		/* ETH_SEL_TXCLK_NOT_CLK125 */
		STM_PAD_SYSCONF(SYSCONF(23), 6, 6, 1),
		/* ETH_SEL_INTERNAL_NOTEXT_PHYCLK */
		STM_PAD_SYSCONF(SYSCONF(23), 7, 7, 1),
		/* ETH_SEL_TX_RETIMING_CLK */
		STM_PAD_SYSCONF(SYSCONF(23), 8, 8, 0),
		/* ETH_GMAC_EN */
		STM_PAD_SYSCONF(SYSCONF(23), 9, 9, 1),
};

static struct stm_gmac_pin stxh205_ethernet_rmii_pad_configs[] = {
		DATA_OUT(0, 0, 1, RET_BYPASS(0)),/* TXD[0] */
		DATA_OUT(0, 1, 1, RET_BYPASS(0)),/* TXD[1] */
		DATA_OUT(0, 5, 1, RET_BYPASS(0)),/* TXEN */
		DATA_OUT_PU(1, 0, 1, RET_BYPASS(0)),/* MDIO */
		CLOCK_OUT(1, 1, 1, RET_NICLK(0)),/* MDC */
		DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
		DATA_IN(1, 4, 1, RET_BYPASS(0)),/* RXD.0 */
		DATA_IN(1, 5, 1, RET_BYPASS(0)),/* RXD.1 */
		DATA_IN(2, 0, 1, RET_BYPASS(0)),/* RXDV */
		DATA_IN(2, 1, 1, RET_BYPASS(0)),/* RX_ER */
		PHY_CLOCK(2, 3, 2, RET_NICLK(0)),/* PHYCLK */
};
static struct stm_pad_sysconf stxh205_ethernet_rmii_pad_sysconfs[] = {
		/* ETH_POWERDOWN_REQ */
		STM_PAD_SYSCONF(SYSCONF(23), 0, 0, 0),
		/* ETH_MII_PHY_SEL */
		STM_PAD_SYSCONF(SYSCONF(23), 2, 4, 4),
		/* ETH_ENMII */
		STM_PAD_SYSCONF(SYSCONF(23), 5, 5, 1),
		/* ETH_SEL_TXCLK_NOT_CLK125 */
		STM_PAD_SYSCONF(SYSCONF(23), 6, 6, 1),
		/* ETH_SEL_INTERNAL_NOTEXT_PHYCLK */
		STM_PAD_SYSCONF(SYSCONF(23), 7, 7, 1),
		/* ETH_SEL_TX_RETIMING_CLK */
		STM_PAD_SYSCONF(SYSCONF(23), 8, 8, 1),
		/* ETH_GMAC_EN */
		STM_PAD_SYSCONF(SYSCONF(23), 9, 9, 1),
};

/* TODO */
static struct stm_gmac_pin stxh205_ethernet_reverse_mii_pad_configs[] = {
		DATA_OUT(0, 0, 1, RET_BYPASS(0)),/* TXD[0] */
		DATA_OUT(0, 1, 1, RET_BYPASS(0)),/* TXD[1] */
		DATA_OUT(0, 2, 1, RET_BYPASS(0)),/* TXD[2] */
		DATA_OUT(0, 3, 1, RET_BYPASS(0)),/* TXD[3] */
		TXER(0, 4, 1, RET_BYPASS(0)),/* TXER */
		DATA_OUT(0, 5, 1, RET_BYPASS(0)),/* TXEN */
		CLOCK_IN(0, 6, 1, RET_NICLK(0)),/* TXCLK */
		DATA_OUT(0, 7, 1, RET_BYPASS(0)),/* COL */
		DATA_OUT_PU(1, 0, 1, RET_BYPASS(0)),/* MDIO*/
		CLOCK_IN(1, 1, 1, RET_NICLK(0)),/* MDC */
		DATA_OUT(1, 2, 1, RET_BYPASS(0)),/* CRS */
		DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
		DATA_IN(1, 4, 1, RET_BYPASS(0)),/* RXD[0] */
		DATA_IN(1, 5, 1, RET_BYPASS(0)),/* RXD[1] */
		DATA_IN(1, 6, 1, RET_BYPASS(0)),/* RXD[2] */
		DATA_IN(1, 7, 1, RET_BYPASS(0)),/* RXD[3] */
		DATA_IN(2, 0, 1, RET_BYPASS(0)),/* RXDV */
		DATA_IN(2, 1, 1, RET_BYPASS(0)),/* RX_ER */
		CLOCK_IN(2, 2, 1, RET_NICLK(0)),/* RXCLK */
		PHY_CLOCK(2, 3, 1, RET_NICLK(0)),/* PHYCLK */
};
static struct stm_pad_sysconf stxh205_ethernet_reverse_mii_pad_sysconfs[] = {
		/* ETH_POWERDOWN_REQ */
		STM_PAD_SYSCONF(SYSCONF(23), 0, 0, 0),
		/* ETH_MII_PHY_SEL */
		STM_PAD_SYSCONF(SYSCONF(23), 2, 4, 0),
		/* ETH_ENMII */
		STM_PAD_SYSCONF(SYSCONF(23), 5, 5, 0),
		/* ETH_SEL_TXCLK_NOT_CLK125 */
		STM_PAD_SYSCONF(SYSCONF(23), 6, 6, 1),
		/* ETH_SEL_INTERNAL_NOTEXT_PHYCLK */
		STM_PAD_SYSCONF(SYSCONF(23), 7, 7, 1),
		/* ETH_SEL_TX_RETIMING_CLK */
		STM_PAD_SYSCONF(SYSCONF(23), 8, 8, 0),
		/* ETH_GMAC_EN */
		STM_PAD_SYSCONF(SYSCONF(23), 9, 9, 1),
};


extern int stmac_default_pbl(void)
{
	return 32;
}

extern void stmac_set_mac_speed(int speed)
{
	/*
	 * No Gigabit, so we do not need to worry about switching between 100/1000 speeds.
	 * However, what about 10/100 switching ?
	 * QQQ - need to do something here if we are using a very slow 10Mbps hub!
	 */
}

/* ETH MAC pad configuration */
extern void stxh205_configure_ethernet(
	const int port,
	const struct stxh205_ethernet_config * const config)
{
	struct stm_gmac_pin * pad_config;
	struct stm_gmac_pin * phy_clock;
	struct stm_pad_sysconf * sys_configs;
	size_t num_pads, num_sys, i;

	BUG_ON(!config);
	BUG_ON(port != 0);

	switch (config->mode) {

	case stxh205_ethernet_mode_mii:
		pad_config = stxh205_ethernet_mii_pad_configs;
		num_pads = ARRAY_SIZE(stxh205_ethernet_mii_pad_configs);
		sys_configs = stxh205_ethernet_mii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh205_ethernet_mii_pad_sysconfs);
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);

		if (config->ext_clk)
			stm_pad_set_pio_ignored(phy_clock);
		else
			stm_pad_set_pio_out(phy_clock, 1);
		if (config->no_txer)
			stm_pad_set_pio_ignored(stm_gmac_find_txer(pad_config, num_pads));
		break;

	case stxh205_ethernet_mode_rmii:
		pad_config = stxh205_ethernet_rmii_pad_configs;
		num_pads = ARRAY_SIZE(stxh205_ethernet_rmii_pad_configs);
		sys_configs = stxh205_ethernet_rmii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh205_ethernet_rmii_pad_sysconfs);
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);

		if (config->ext_clk) {
			stm_pad_set_pio_in(phy_clock, 2);
			/* ETH_SEL_INTERNAL_NOTEXT_PHYCLK */
			sys_configs[4].value = 0;
		} else {
			stm_pad_set_pio_out(phy_clock, 1);
			/* ETH_SEL_INTERNAL_NOTEXT_PHYCLK */
			sys_configs[4].value = 1;
		}
		break;

	case stxh205_ethernet_mode_reverse_mii:
		pad_config = stxh205_ethernet_reverse_mii_pad_configs;
		num_pads = ARRAY_SIZE(stxh205_ethernet_reverse_mii_pad_configs);
		sys_configs = stxh205_ethernet_reverse_mii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh205_ethernet_reverse_mii_pad_sysconfs);
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);

		if (config->ext_clk)
			stm_pad_set_pio_ignored(phy_clock);
		else
			stm_pad_set_pio_out(phy_clock, 1);
		if (config->no_txer)
			stm_pad_set_pio_ignored(stm_gmac_find_txer(pad_config, num_pads));
		break;

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

		stxh205_pioalt_select(portno, pinno, pad->pio.alt);
		stxh205_pioalt_pad(portno, pinno, pad->direction);
		stxh205_pioalt_retime(portno, pinno, pad->retime);
	}

		/* now configure the relevant SYS_CONFIGs */
	stm_configure_sysconfs(sys_configs, num_sys);
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */


#define ST40_PWR_SW_RST_MASK		(1u<<5)
#define LPC_WDT_OUT_MASK		(1u<<4)
#define ST40_RST_WDT_OUT_MASK		(1u<<3)	/* ST40 Watchdog reset */
#define ST40_RST_PWR_OUT_MASK		(1u<<2)
#define ST40_RST_UDI_OUT_MASK		(1u<<1)
#define ST40_RST_MAN_OUT_MASK		(1u<<0)	/* ST40 Manual reset, including a "double-fault" */

extern int soc_init(void)
{
	bd_t * const bd = gd->bd;

	stxh205_clocks();

	bd->bi_devid = *STXH205_SYSCONF_DEVICEID_0;

	/*
	 * Allow both the Manual + Watchdog resets *OUT* of the
	 * ST40 core, to propagate to the rest of the system...
	 * Failure to do this results in weird reset conditions!
	 */
	*STXH205_SYSCFG(498) &= ~(ST40_RST_MAN_OUT_MASK | ST40_RST_WDT_OUT_MASK);

	return 0;
}


#if defined(CONFIG_USB_OHCI_NEW) || defined(CONFIG_USB_EHCI)

#define SYSCONF_BITS(lsb,msb,value)					\
do {									\
	SET_SYSCONF_BITS(sysconf,1,(lsb),(msb),(value),(value));	\
} while(0)

extern int stxh205_usb_init(const int port)
{
	static int initialized = 0;
	unsigned int flags;
	const struct {
		struct {
			unsigned char port, pin, alt;
		} oc, pwr;
	} usb_pins[] = {
		{ .oc = { 4, 2, 1 }, .pwr = { 4, 3, 1 } },
		{ .oc = { 4, 4, 1 }, .pwr = { 4, 5, 1 } },
	};

	if (port < 0 || port >= 2)	/* invalid port number ? */
		return -1;		/* failed to initialize! */

	if (!initialized)		/* first time ? */
	{
		unsigned long *sysconfReg, sysconf;

		sysconfReg = (unsigned long*)STXH205_SYSCFG(501);
		sysconf = readl(sysconfReg);
		SYSCONF_BITS(  1,  3, 4);	/* COMPDISTUNE0 */
		SYSCONF_BITS(  4,  6, 4);	/* COMPDISTUNE1 */
		SYSCONF_BITS(  7,  7, 1);	/* OTGDISABLE0 */
		SYSCONF_BITS(  8, 10, 4);	/* OTGTUNE0 */
		SYSCONF_BITS( 11, 11, 1);	/* SLEEPM0 */
		SYSCONF_BITS( 12, 12, 1);	/* SLEEPM1 */
		SYSCONF_BITS( 13, 15, 3);	/* SQRXTUNE0 */
		SYSCONF_BITS( 16, 18, 3);	/* SQRXTUNE1 */
		SYSCONF_BITS( 19, 22, 3);	/* TXFSLSTUNE0 */
		SYSCONF_BITS( 23, 23, 0);	/* TXPREEMPHASISTUNE0 */
		SYSCONF_BITS( 24, 24, 0);	/* TXPREEMPHASISTUNE1 */
		SYSCONF_BITS( 25, 25, 0);	/* TXRISETUNE0 */
		SYSCONF_BITS( 26, 26, 0);	/* TXRISETUNE1 */
		SYSCONF_BITS( 27, 30, 8);	/* TXVREFTUNE0 */
		writel(sysconf, sysconfReg);

		sysconfReg = (unsigned long*)STXH205_SYSCFG(502);
		sysconf = readl(sysconfReg);
		SYSCONF_BITS(  0,  3, 8);	/* TXVREFTUNE1 */
		SYSCONF_BITS(  4,  7, 3);	/* TXFSLSTUNE1 */
		SYSCONF_BITS(  8,  9, 3);	/* TXHSXVTUNE0 */
		SYSCONF_BITS( 10, 11, 3);	/* TXHSXVTUNE1 */
		writel(sysconf, sysconfReg);

		/* QQQ - unclear if this needs to be done *last* or not! */
		sysconfReg = (unsigned long*)STXH205_SYSCFG(501);
		sysconf = readl(sysconfReg);
		SYSCONF_BITS(  0,  0, 0);	/* COMMONONN */
		writel(sysconf, sysconfReg);

		initialized = 1;	/* do this just once */
	}

	/* Power up the USB port, i.e. set USB_x_POWERDOWN_REQ = 0 */
	{
		unsigned long * const sysconfReg = (unsigned long*)STXH205_SYSCFG(449);
		unsigned long sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, FALSE, 0+port);
		writel(sysconf, sysconfReg);
	}
	/* wait until USBx_PWR_DWN_GRANT == 0 */
	while (*STXH205_SYSSTS(423) & (1u<<(3+port)))
		;	/* just wait ... */

	/* route the USB power enable (output) signal */
	stxh205_pioalt_select(usb_pins[port].pwr.port,
			    usb_pins[port].pwr.pin,
			    usb_pins[port].pwr.alt);
	stxh205_pioalt_pad(usb_pins[port].pwr.port,
			  usb_pins[port].pwr.pin, stm_pad_direction_output);

	/* route the USB over-current (input) signal */
	stxh205_pioalt_select(usb_pins[port].oc.port,
			    usb_pins[port].oc.pin,
			    usb_pins[port].oc.alt);
	stxh205_pioalt_pad(usb_pins[port].oc.port,
			  usb_pins[port].oc.pin, stm_pad_direction_input);

	/* start the USB Wrapper Host Controller */
	flags = USB_FLAGS_STRAP_8BIT | USB_FLAGS_STBUS_CONFIG_THRESHOLD128;
	ST40_start_host_control(flags);

	return 0;
}
#endif /* defined(CONFIG_USB_OHCI_NEW) || defined(CONFIG_USB_EHCI) */


#if defined(CONFIG_CMD_I2C) && defined(CONFIG_SOFT_I2C)
extern void stxh205_configure_i2c(void)
{
	SET_PIO_PIN2(STXH205_I2C_SCL, STPIO_BIDIR);	/* I2C_SCL */
	SET_PIO_PIN2(STXH205_I2C_SDA, STPIO_BIDIR);	/* I2C_SDA */
}

extern void stxh205_i2c_scl(const int val)
{
	/* SSC's SCLK == I2C's SCL */
	STPIO_SET_PIN2(STXH205_I2C_SCL, (val) ? 1 : 0);
}

extern void stxh205_i2c_sda(const int val)
{
	/* SSC's MTSR == I2C's SDA */
	STPIO_SET_PIN2(STXH205_I2C_SDA, (val) ? 1 : 0);
}

extern int stxh205_i2c_read(void)
{
	/* SSC's MTSR == I2C's SDA */
	return STPIO_GET_PIN2(STXH205_I2C_SDA);
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

extern void stxh205_configure_spi(void)
{
	/*
	 *	We set up the PIO pins correctly for SPI
	 */

	/* route PIO (alternate #0) */
	STXH205_PIOALT_SELECT(SPI_MISO, 0);	/* SPI_MISO */
	STXH205_PIOALT_SELECT(SPI_MOSI, 0);	/* SPI_MOSI */
	STXH205_PIOALT_SELECT(SPI_nCS,  0);	/* SPI_nCS */
	STXH205_PIOALT_SELECT(SPI_CLK,  0);	/* SPI_CLK */

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

extern void stxh205_spi_scl(const int val)
{
	STPIO_SET_PIN2(SPI_CLK, val ? 1 : 0);
}

extern void stxh205_spi_sda(const int val)
{
	STPIO_SET_PIN2(SPI_MOSI, val ? 1 : 0);
}

extern unsigned char stxh205_spi_read(void)
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

