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
#include <asm/stx7108reg.h>
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
	 * The STx7108 is only supported in 32-bit (SE) mode,
	 * and is not supported in 29-bit (legacy) mode.
	 * We refuse to build, if this assertion is invalid.
	 */
#if !defined(CONFIG_ST40_SE_MODE)
#error This SoC is not supported in 29-bit mode, please enable SE-mode!
#endif	/* !CONFIG_ST40_SE_MODE */


static void stx7108_clocks(void)
{
	bd_t * const bd = gd->bd;

	/*
	 * Ideally, we should probe to determine all the clock frequencies.
	 * However, for simplicity, we will simply hard-wire the values
	 * that U-Boot will use for computing the clock dividers later.
	 * WARNING: Getting these values wrong may result in strange behaviour!
	 */
	bd->bi_uart_frq = 100ul * 1000000ul;	/* 100 MHz */
	bd->bi_tmu_frq  = bd->bi_uart_frq;
	bd->bi_ssc_frq  = bd->bi_uart_frq;
}


/*
 * PIO alternative Function selector
 */
extern void stx7108_pioalt_select(const int port, const int pin, const int alt)
{
	int num;
	unsigned long sysconf, *sysconfReg;

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("%s(port=%d, pin=%d, alt=%d)\n", __func__, port, pin, alt);
	BUG_ON(pin < 0 || pin > 7);
	BUG_ON(alt < 0 || alt > 5);
#endif

	switch (port)
	{
	case 0 ... 14:
		num = port;		/* in Bank #2 */
		sysconfReg = (unsigned long*)STX7108_BANK2_SYSCFG(num);
		break;
	case 15 ... 26:
		num = port - 15;	/* in Bank #4 */
		sysconfReg = (unsigned long*)STX7108_BANK4_SYSCFG(num);
		break;
	default:
		BUG();
		return;
	}

	sysconf = readl(sysconfReg);
	SET_SYSCONF_BITS(sysconf, TRUE, pin*4,(pin*4)+3, alt,alt);
	writel(sysconf, sysconfReg);
}

/* Pad configuration */

void stx7108_pioalt_pad(int port, const int pin,
		const enum stm_pad_gpio_direction direction)
{
	int num, bit;
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
	case 0 ... 14:
		num = 15 + (port / 4);	/* in Bank #2 */
		sysconfReg = (unsigned long*)STX7108_BANK2_SYSCFG(num);
		break;
	case 15 ... 26:
		port -= 15;
		num = 12 + (port / 4);	/* in Bank #4 */
		sysconfReg = (unsigned long*)STX7108_BANK4_SYSCFG(num);
		break;
	default:
		BUG();
		return;
	}

	bit = ((port * 8) + pin) % 32;

		/* set the "Output Enable" pad control */
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BIT(sysconf, oe, bit);
	writel(sysconf, sysconfReg);

	sysconfReg += 4;	/* skip 4 syscfg registers */

		/* set the "Pull Up" pad control */
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BIT(sysconf, pu, bit);
	writel(sysconf, sysconfReg);

	sysconfReg += 4;	/* skip another 4 syscfg registers */

		/* set the "Open Drain Enable" pad control */
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BIT(sysconf, od, bit);
	writel(sysconf, sysconfReg);
}

/* PIO retiming setup */

#ifdef CONFIG_DRIVER_NET_STM_GMAC

static void stx7108_pioalt_retime(const int port, const int pin,
		const struct stm_pio_control_retime_config * const cfg)
{
	int num;
	unsigned long sysconf, *sysconfReg;

#ifdef DEBUG_PAD_CONFIGS
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
	case 0 ... 14:
		switch (port)
		{
		case 1:
			num = 32;
			break;
		case 6 ... 14:
			num = 34 + ((port - 6) * 2);
			break;
		default:
			BUG();
			return;
		}
		sysconfReg = (unsigned long*)STX7108_BANK2_SYSCFG(num);
		break;
	case 15 ... 23:
		num = 48 + ((port - 15) * 2);
		sysconfReg = (unsigned long*)STX7108_BANK4_SYSCFG(num);
		break;
	default:
		BUG();
		return;
	}

	sysconfReg += 0;	/* use retime configuration register #0 */

	if (cfg->clk >= 0)
	{
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, cfg->clk, 0 + pin);
		writel(sysconf, sysconfReg);
	}

	if (cfg->clknotdata >= 0)
	{
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, cfg->clknotdata, 8 + pin);
		writel(sysconf, sysconfReg);
	}

	if (cfg->delay >= 0)
	{
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, cfg->delay, 16 + pin);
		writel(sysconf, sysconfReg);
	}

	if (cfg->double_edge >= 0)
	{
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, cfg->double_edge, 24+ pin);
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
}


/* --------------------------------------------------------------------
 *		Ethernet MAC resources (PAD and Retiming)
 * --------------------------------------------------------------------*/


#define SYSCONF(_bank,_reg)	((unsigned long*)STX7108_BANK##_bank##_SYSCFG(_reg))


#define DATA_IN(_gmac, _port, _pin, _retiming) \
	{ \
		.pio       = { _port, _pin, _gmac + 1, }, \
		.direction = stm_pad_direction_input, \
		.retime    = _retiming, \
	}

#define DATA_OUT(_gmac, _port, _pin, _retiming) \
	{ \
		.pio       = { _port, _pin, _gmac + 1, }, \
		.direction = stm_pad_direction_output, \
		.retime    = _retiming, \
	}

/*
 * On some boards the MDIO line is missing a pull-up resistor. Enabling
 * weak internal pull-up overcomes the issue.
 */
#define DATA_OUT_PU(_gmac, _port, _pin, _retiming) \
	{ \
		.pio       = { _port, _pin, _gmac + 1, }, \
		.direction = stm_pad_direction_input_with_pullup, \
		.retime    = _retiming, \
	}

#define CLOCK_IN(_gmac, _port, _pin, _retiming) \
	{ \
		.pio       = { _port, _pin, _gmac + 1, }, \
		.direction = stm_pad_direction_input, \
		.retime    = _retiming, \
	}

#define CLOCK_OUT(_gmac, _port, _pin, _retiming) \
	{ \
		.pio       = { _port, _pin, _gmac + 1, }, \
		.direction = stm_pad_direction_output, \
		.retime    = _retiming, \
	}

/* Give PHY_CLOCK an additional "flag" so we can refer to it later. */
#define PHY_CLOCK(_gmac, _port, _pin, _retiming) \
	{ \
		.pio       = { _port, _pin, 0 /* filled in later */, }, \
		.phy_clock = 1, \
		.direction = stm_pad_direction_unknown, \
		.retime    = _retiming, \
	}

/* Give TX_CLOCK an additional "flag" so we can refer to it later. */
#define TX_CLOCK(_gmac, _port, _pin, _retiming) \
	{ \
		.pio       = { _port, _pin, _gmac + 1, }, \
		.tx_clock  = 1, \
		.direction = stm_pad_direction_input, \
		.retime    = _retiming, \
	}


#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
static struct stm_gmac_pin stx7108_ethernet_mii_pad_configs[] = {
			DATA_OUT(0, 6, 0, RET_SE_NICLK_IO(0, 0)),/* TXD[0] */
			DATA_OUT(0, 6, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[1] */
			DATA_OUT(0, 6, 2, RET_SE_NICLK_IO(0, 0)),/* TXD[2] */
			DATA_OUT(0, 6, 3, RET_SE_NICLK_IO(0, 0)),/* TXD[3] */
			DATA_OUT(0, 7, 0, RET_SE_NICLK_IO(0, 0)),/* TXER */
			DATA_OUT(0, 7, 1, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			CLOCK_IN(0, 7, 2, RET_NICLK(0)),/* TXCLK */
			DATA_IN(0, 7, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(0, 7, 4, RET_BYPASS(3)),/* MDIO*/
			CLOCK_OUT(0, 7, 5, RET_NICLK(0)),/* MDC */
			DATA_IN(0, 7, 6, RET_BYPASS(0)),/* CRS */
			DATA_IN(0, 7, 7, RET_BYPASS(0)),/* MDINT */
			DATA_IN(0, 8, 0, RET_SE_NICLK_IO(2, 0)),/* RXD[0] */
			DATA_IN(0, 8, 1, RET_SE_NICLK_IO(2, 0)),/* RXD[1] */
			DATA_IN(0, 8, 2, RET_SE_NICLK_IO(2, 0)),/* RXD[2] */
			DATA_IN(0, 8, 3, RET_SE_NICLK_IO(2, 0)),/* RXD[3] */
			DATA_IN(0, 9, 0, RET_SE_NICLK_IO(2, 0)),/* RXDV */
			DATA_IN(0, 9, 1, RET_SE_NICLK_IO(2, 0)),/* RX_ER */
			CLOCK_IN(0, 9, 2, RET_NICLK(0)),/* RXCLK */
			PHY_CLOCK(0, 9, 3, RET_NICLK(0)),/* PHYCLK */
};
static struct stm_pad_sysconf stx7108_ethernet_mii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYS_CFG_BANK(2, 53, 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYS_CFG_BANK(2, 27, 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYS_CFG_BANK(2, 27, 5, 5, 1),
};
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
static struct stm_gmac_pin stx7108_ethernet_mii_pad_configs[] = {
			PHY_CLOCK(1, 15, 5, RET_NICLK(0)),/* PHYCLK */
			DATA_IN(1, 15, 6, RET_BYPASS(0)),/* MDINT */
			DATA_OUT(1, 15, 7, RET_SE_NICLK_IO(0, 1)),/* TXEN */
			DATA_OUT(1, 16, 0, RET_SE_NICLK_IO(0, 1)),/* TXD[0] */
			DATA_OUT(1, 16, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[1] */
			DATA_OUT(1, 16, 2, RET_SE_NICLK_IO(0, 1)),/* TXD[2] */
			DATA_OUT(1, 16, 3, RET_SE_NICLK_IO(0, 1)),/* TXD[3] */
			CLOCK_IN(1, 17, 0, RET_NICLK(0)),/* TXCLK */
			DATA_OUT(1, 17, 1, RET_SE_NICLK_IO(0, 1)),/* TXER */
			DATA_IN(1, 17, 2, RET_BYPASS(0)),/* CRS */
			DATA_IN(1, 17, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(1, 17, 4, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(1, 17, 5, RET_NICLK(1)),/* MDC */
			DATA_IN(1, 17, 6, RET_SE_NICLK_IO(2, 1)),/* RXDV */
			DATA_IN(1, 17, 7, RET_SE_NICLK_IO(2, 1)),/* RX_ER */
			DATA_IN(1, 18, 0, RET_SE_NICLK_IO(2, 1)),/* RXD[0] */
			DATA_IN(1, 18, 1, RET_SE_NICLK_IO(2, 1)),/* RXD[1] */
			DATA_IN(1, 18, 2, RET_SE_NICLK_IO(2, 1)),/* RXD[2] */
			DATA_IN(1, 18, 3, RET_SE_NICLK_IO(2, 1)),/* RXD[3] */
			CLOCK_IN(1, 19, 0, RET_NICLK(0)),/* RXCLK */
};
static struct stm_pad_sysconf stx7108_ethernet_mii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYS_CFG_BANK(4, 67, 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYS_CFG_BANK(4, 23, 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYS_CFG_BANK(4, 23, 5, 5, 1),
};
#endif				/* CONFIG_SYS_STM_STMAC_BASE == GMAC #1 */

#if 0	/* assume not required in U-Boot */
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
static struct stm_gmac_pin stx7108_ethernet_gmii_pad_configs[] = {
			DATA_OUT(0, 6, 0, RET_SE_NICLK_IO(3, 0)),/* TXD[0] */
			DATA_OUT(0, 6, 1, RET_SE_NICLK_IO(3, 0)),/* TXD[1] */
			DATA_OUT(0, 6, 2, RET_SE_NICLK_IO(3, 0)),/* TXD[2] */
			DATA_OUT(0, 6, 3, RET_SE_NICLK_IO(3, 0)),/* TXD[3] */
			DATA_OUT(0, 6, 4, RET_SE_NICLK_IO(3, 0)),/* TXD[4] */
			DATA_OUT(0, 6, 5, RET_SE_NICLK_IO(3, 0)),/* TXD[5] */
			DATA_OUT(0, 6, 6, RET_SE_NICLK_IO(3, 0)),/* TXD[6] */
			DATA_OUT(0, 6, 7, RET_SE_NICLK_IO(3, 0)),/* TXD[7] */
			DATA_OUT(0, 7, 0, RET_SE_NICLK_IO(3, 0)),/* TXER */
			DATA_OUT(0, 7, 1, RET_SE_NICLK_IO(3, 0)),/* TXEN */
			CLOCK_IN(0, 7, 2, RET_NICLK(0)),/* TXCLK */
			DATA_IN(0, 7, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(0, 7, 4, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(0, 7, 5, RET_NICLK(0)),/* MDC */
			DATA_IN(0, 7, 6, RET_BYPASS(0)),/* CRS */
			DATA_IN(0, 7, 7, RET_BYPASS(0)),/* MDINT */
			DATA_IN(0, 8, 0, RET_SE_NICLK_IO(2, 0)),/* RXD[0] */
			DATA_IN(0, 8, 1, RET_SE_NICLK_IO(2, 0)),/* RXD[1] */
			DATA_IN(0, 8, 2, RET_SE_NICLK_IO(2, 0)),/* RXD[2] */
			DATA_IN(0, 8, 3, RET_SE_NICLK_IO(2, 0)),/* RXD[3] */
			DATA_IN(0, 8, 4, RET_SE_NICLK_IO(2, 0)),/* RXD[4] */
			DATA_IN(0, 8, 5, RET_SE_NICLK_IO(2, 0)),/* RXD[5] */
			DATA_IN(0, 8, 6, RET_SE_NICLK_IO(2, 0)),/* RXD[6] */
			DATA_IN(0, 8, 7, RET_SE_NICLK_IO(2, 0)),/* RXD[7] */
			DATA_IN(0, 9, 0, RET_SE_NICLK_IO(2, 0)),/* RXDV */
			DATA_IN(0, 9, 1, RET_SE_NICLK_IO(2, 0)),/* RX_ER */
			CLOCK_IN(0, 9, 2, RET_NICLK(0)),/* RXCLK */
			PHY_CLOCK(0, 9, 3, RET_NICLK(1)),/* GTXCLK */
};
static struct stm_pad_sysconf stx7108_ethernet_gmii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYS_CFG_BANK(2, 53, 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYS_CFG_BANK(2, 27, 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYS_CFG_BANK(2, 27, 5, 5, 1),
};
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
static struct stm_gmac_pin stx7108_ethernet_gmii_pad_configs[] = {
			PHY_CLOCK(1, 15, 5, RET_NICLK(1)), /* GTXCLK */
			DATA_IN(1, 15, 6, RET_BYPASS(0)),/* MDINT */
			DATA_OUT(1, 15, 7, RET_SE_NICLK_IO(3, 1)),/* TXEN */
			DATA_OUT(1, 16, 0, RET_SE_NICLK_IO(3, 1)),/* TXD[0] */
			DATA_OUT(1, 16, 1, RET_SE_NICLK_IO(3, 1)),/* TXD[1] */
			DATA_OUT(1, 16, 2, RET_SE_NICLK_IO(3, 1)),/* TXD[2] */
			DATA_OUT(1, 16, 3, RET_SE_NICLK_IO(3, 1)),/* TXD[3] */
			DATA_OUT(1, 16, 4, RET_SE_NICLK_IO(3, 1)),/* TXD[4] */
			DATA_OUT(1, 16, 5, RET_SE_NICLK_IO(3, 1)),/* TXD[5] */
			DATA_OUT(1, 16, 6, RET_SE_NICLK_IO(3, 1)),/* TXD[6] */
			DATA_OUT(1, 16, 7, RET_SE_NICLK_IO(3, 1)),/* TXD[7] */
			CLOCK_IN(1, 17, 0, RET_NICLK(0)),/* TXCLK */
			DATA_OUT(1, 17, 1, RET_SE_NICLK_IO(3, 1)),/* TXER */
			DATA_IN(1, 17, 2, RET_BYPASS(0)),/* CRS */
			DATA_IN(1, 17, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(1, 17, 4, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(1, 17, 5, RET_NICLK(1)),/* MDC */
			DATA_IN(1, 17, 6, RET_SE_NICLK_IO(2, 1)),/* RXDV */
			DATA_IN(1, 17, 7, RET_SE_NICLK_IO(2, 1)),/* RX_ER */
			DATA_IN(1, 18, 0, RET_SE_NICLK_IO(2, 1)),/* RXD[0] */
			DATA_IN(1, 18, 1, RET_SE_NICLK_IO(2, 1)),/* RXD[1] */
			DATA_IN(1, 18, 2, RET_SE_NICLK_IO(2, 1)),/* RXD[2] */
			DATA_IN(1, 18, 3, RET_SE_NICLK_IO(2, 1)),/* RXD[3] */
			DATA_IN(1, 18, 4, RET_SE_NICLK_IO(2, 1)),/* RXD[4] */
			DATA_IN(1, 18, 5, RET_SE_NICLK_IO(2, 1)),/* RXD[5] */
			DATA_IN(1, 18, 6, RET_SE_NICLK_IO(2, 1)),/* RXD[6] */
			DATA_IN(1, 18, 7, RET_SE_NICLK_IO(2, 1)),/* RXD[7] */
			CLOCK_IN(1, 19, 0, RET_NICLK(0)),/* RXCLK */
};
static struct stm_pad_sysconf stx7108_ethernet_gmii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYS_CFG_BANK(4, 67, 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYS_CFG_BANK(4, 23, 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYS_CFG_BANK(4, 23, 5, 5, 1),
};
#endif				/* CONFIG_SYS_STM_STMAC_BASE == GMAC #1 */
#endif	/* if 0 */

#if 0	/* assume not required in U-Boot */
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
static struct stm_gmac_pin stx7108_ethernet_rgmii_pad_configs[] = {
			DATA_OUT(0, 6, 0, RET_DE_IO(0, 0)),/* TXD[0] */
			DATA_OUT(0, 6, 1, RET_DE_IO(0, 0)),/* TXD[1] */
			DATA_OUT(0, 6, 2, RET_DE_IO(0, 0)),/* TXD[2] */
			DATA_OUT(0, 6, 3, RET_DE_IO(0, 0)),/* TXD[3] */
			DATA_OUT(0, 7, 1, RET_DE_IO(0, 0)),/* TXEN */
			/* TX Clock inversion is not set for 1000Mbps */
			TX_CLOCK(0, 7, 2, RET_NICLK(0)),/* TXCLK */
			DATA_IN(0, 7, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(0, 7, 4, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(0, 7, 5, RET_NICLK(0)),/* MDC */
			DATA_IN(0, 7, 7, RET_BYPASS(0)), /* MDINT */
			DATA_IN(0, 8, 0, RET_DE_IO(0, 0)),/* RXD[0] */
			DATA_IN(0, 8, 1, RET_DE_IO(0, 0)),/* RXD[1] */
			DATA_IN(0, 8, 2, RET_DE_IO(0, 0)),/* RXD[2] */
			DATA_IN(0, 8, 3, RET_DE_IO(0, 0)),/* RXD[3] */
			DATA_IN(0, 9, 0, RET_DE_IO(0, 0)),/* RXDV */
			CLOCK_IN(0, 9, 2, RET_NICLK(0)),/* RXCLK */
			PHY_CLOCK(0, 9, 3, RET_NICLK(1)),/* PHYCLK */
};
static struct stm_pad_sysconf stx7108_ethernet_rgmii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYS_CFG_BANK(2, 53, 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYS_CFG_BANK(2, 27, 2, 4, 1),
			/* ENMIIx */
			STM_PAD_SYS_CFG_BANK(2, 27, 5, 5, 1),
};
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
static struct stm_gmac_pin stx7108_ethernet_rgmii_pad_configs[] = {
			PHY_CLOCK(1, 15, 5, RET_NICLK(1)), /* GTXCLK*/
			DATA_IN(1, 15, 6, RET_BYPASS(0)),/* MDINT */
			DATA_OUT(1, 15, 7, RET_DE_IO(0, 1)),/* TXEN */
			DATA_OUT(1, 16, 0, RET_DE_IO(0, 1)),/* TXD[0] */
			DATA_OUT(1, 16, 1, RET_DE_IO(0, 1)),/* TXD[1] */
			DATA_OUT(1, 16, 2, RET_DE_IO(0, 1)),/* TXD[2] */
			DATA_OUT(1, 16, 3, RET_DE_IO(0, 1)),/* TXD[3] */
			/* TX Clock inversion is not set for 1000Mbps */
			TX_CLOCK(1, 17, 0, RET_NICLK(0)),/* TXCLK */
			DATA_IN(1, 17, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(1, 17, 4, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(1, 17, 5, RET_NICLK(1)),/* MDC */
			DATA_IN(1, 17, 6, RET_DE_IO(0, 1)),/* RXDV */
			DATA_IN(1, 18, 0, RET_DE_IO(0, 1)),/* RXD[0] */
			DATA_IN(1, 18, 1, RET_DE_IO(0, 1)),/* RXD[1] */
			DATA_IN(1, 18, 2, RET_DE_IO(0, 1)),/* RXD[2] */
			DATA_IN(1, 18, 3, RET_DE_IO(0, 1)),/* RXD[3] */
			CLOCK_IN(1, 19, 0, RET_NICLK(0)),/* RXCLK */
};
static struct stm_pad_sysconf stx7108_ethernet_rgmii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYS_CFG_BANK(4, 67, 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYS_CFG_BANK(4, 23, 2, 4, 1),
			/* ENMIIx */
			STM_PAD_SYS_CFG_BANK(4, 23, 5, 5, 1),
};
#endif				/* CONFIG_SYS_STM_STMAC_BASE == GMAC #1 */
#endif	/* if 0 */

#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
static struct stm_gmac_pin stx7108_ethernet_rmii_pad_configs[] = {
			DATA_OUT(0, 6, 0, RET_SE_NICLK_IO(0, 1)),/* TXD[0] */
			DATA_OUT(0, 6, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[1] */
			DATA_OUT(0, 7, 0, RET_SE_NICLK_IO(0, 1)),/* TXER */
			DATA_OUT(0, 7, 1, RET_SE_NICLK_IO(0, 1)),/* TXEN */
			DATA_OUT(0, 7, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(0, 7, 4, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(0, 7, 5, RET_NICLK(0)),/* MDC */
			DATA_IN(0, 7, 6, RET_BYPASS(0)),/* CRS */
			DATA_IN(0, 7, 7, RET_BYPASS(0)),/* MDINT */
			DATA_IN(0, 8, 0, RET_SE_NICLK_IO(2, 1)),/* RXD.0 */
			DATA_IN(0, 8, 1, RET_SE_NICLK_IO(2, 1)),/* RXD.1 */
			DATA_IN(0, 9, 0, RET_SE_NICLK_IO(2, 1)),/* RXDV */
			DATA_IN(0, 9, 1, RET_SE_NICLK_IO(2, 1)),/* RX_ER */
			PHY_CLOCK(0, 9, 3, RET_NICLK(0)),/* PHYCLK */
};
static struct stm_pad_sysconf stx7108_ethernet_rmii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYS_CFG_BANK(2, 53, 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYS_CFG_BANK(2, 27, 2, 4, 4),
			/* ENMIIx */
			STM_PAD_SYS_CFG_BANK(2, 27, 5, 5, 1),
};
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
static struct stm_gmac_pin stx7108_ethernet_rmii_pad_configs[] = {
			PHY_CLOCK(1, 15, 5, RET_NICLK(0)),/* PHYCLK */
			DATA_IN(1, 15, 6, RET_BYPASS(0)),/* MDINT */
			DATA_OUT(1, 15, 7, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			DATA_OUT(1, 16, 0, RET_SE_NICLK_IO(0, 0)),/* TXD[0] */
			DATA_OUT(1, 16, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[1] */
			DATA_OUT(1, 17, 1, RET_SE_NICLK_IO(0, 0)),/* TXER */
			DATA_OUT(1, 17, 2, RET_BYPASS(0)),/* CRS */
			DATA_OUT(1, 17, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(1, 17, 4, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(1, 17, 5, RET_NICLK(1)),/* MDC */
			DATA_IN(1, 17, 6, RET_SE_NICLK_IO(2, 0)),/* RXDV */
			DATA_IN(1, 17, 7, RET_SE_NICLK_IO(2, 0)),/* RX_ER */
			DATA_IN(1, 18, 0, RET_SE_NICLK_IO(2, 0)),/* RXD[0] */
			DATA_IN(1, 18, 1, RET_SE_NICLK_IO(2, 0)),/* RXD[1] */
};
static struct stm_pad_sysconf stx7108_ethernet_rmii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYS_CFG_BANK(4, 67, 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYS_CFG_BANK(4, 23, 2, 4, 4),
			/* ENMIIx */
			STM_PAD_SYS_CFG_BANK(4, 23, 5, 5, 1),
};
#endif				/* CONFIG_SYS_STM_STMAC_BASE == GMAC #1 */

#if 0	/* assume not required in U-Boot */
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
static struct stm_gmac_pin stx7108_ethernet_reverse_mii_pad_configs[] = {
			DATA_OUT(0, 6, 0, RET_SE_NICLK_IO(0, 0)),/* TXD[0] */
			DATA_OUT(0, 6, 1, RET_SE_NICLK_IO(0, 0)),/* TXD[1] */
			DATA_OUT(0, 6, 2, RET_SE_NICLK_IO(0, 0)),/* TXD[2] */
			DATA_OUT(0, 6, 3, RET_SE_NICLK_IO(0, 0)),/* TXD[3] */
			DATA_OUT(0, 7, 0, RET_SE_NICLK_IO(0, 0)),/* TXER */
			DATA_OUT(0, 7, 1, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			CLOCK_IN(0, 7, 2, RET_NICLK(0)),/* TXCLK */
			DATA_OUT(0, 7, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(0, 7, 4, RET_BYPASS(3)),/* MDIO*/
			CLOCK_IN(0, 7, 5, RET_NICLK(0)),/* MDC */
			DATA_OUT(0, 7, 6, RET_BYPASS(0)),/* CRS */
			DATA_IN(0, 7, 7, RET_BYPASS(0)),/* MDINT */
			DATA_IN(0, 8, 0, RET_SE_NICLK_IO(2, 0)),/* RXD[0] */
			DATA_IN(0, 8, 1, RET_SE_NICLK_IO(2, 0)),/* RXD[1] */
			DATA_IN(0, 8, 2, RET_SE_NICLK_IO(2, 0)),/* RXD[2] */
			DATA_IN(0, 8, 3, RET_SE_NICLK_IO(2, 0)),/* RXD[3] */
			DATA_IN(0, 9, 0, RET_SE_NICLK_IO(2, 0)),/* RXDV */
			DATA_IN(0, 9, 1, RET_SE_NICLK_IO(2, 0)),/* RX_ER */
			CLOCK_IN(0, 9, 2, RET_NICLK(0)),/* RXCLK */
			PHY_CLOCK(0, 9, 3, RET_NICLK(0)),/* PHYCLK */
};
static struct stm_pad_sysconf stx7108_ethernet_reverse_mii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYS_CFG_BANK(2, 53, 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYS_CFG_BANK(2, 27, 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYS_CFG_BANK(2, 27, 5, 5, 0),
};
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
static struct stm_gmac_pin stx7108_ethernet_reverse_mii_pad_configs[] = {
			PHY_CLOCK(1, 15, 5, RET_NICLK(0)),/* PHYCLK */
			DATA_IN(1, 15, 6, RET_BYPASS(0)),/* MDINT */
			DATA_OUT(1, 15, 7, RET_SE_NICLK_IO(0, 1)),/* TXEN */
			DATA_OUT(1, 16, 0, RET_SE_NICLK_IO(0, 1)),/* TXD[0] */
			DATA_OUT(1, 16, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[1] */
			DATA_OUT(1, 16, 2, RET_SE_NICLK_IO(0, 1)),/* TXD[2] */
			DATA_OUT(1, 16, 3, RET_SE_NICLK_IO(0, 1)),/* TXD[3] */
			CLOCK_IN(1, 17, 0, RET_NICLK(0)),/* TXCLK */
			DATA_OUT(1, 17, 1, RET_SE_NICLK_IO(0, 1)),/* TXER */
			DATA_OUT(1, 17, 2, RET_BYPASS(0)),/* CRS */
			DATA_OUT(1, 17, 3, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(1, 17, 4, RET_BYPASS(3)),/* MDIO */
			CLOCK_IN(1, 17, 5, RET_NICLK(1)),/* MDC */
			DATA_IN(1, 17, 6, RET_SE_NICLK_IO(2, 1)),/* RXDV */
			DATA_IN(1, 17, 7, RET_SE_NICLK_IO(2, 1)),/* RX_ER */
			DATA_IN(1, 18, 0, RET_SE_NICLK_IO(2, 1)),/* RXD[0] */
			DATA_IN(1, 18, 1, RET_SE_NICLK_IO(2, 1)),/* RXD[1] */
			DATA_IN(1, 18, 2, RET_SE_NICLK_IO(2, 1)),/* RXD[2] */
			DATA_IN(1, 18, 3, RET_SE_NICLK_IO(2, 1)),/* RXD[3] */
			CLOCK_IN(1, 19, 0, RET_NICLK(0)),/* RXCLK */
};
static struct stm_pad_sysconf stx7108_ethernet_reverse_mii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYS_CFG_BANK(4, 67, 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYS_CFG_BANK(4, 23, 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYS_CFG_BANK(4, 23, 5, 5, 1),
};
#endif				/* CONFIG_SYS_STM_STMAC_BASE == GMAC #1 */
#endif	/* if 0 */


#define MAC_SPEED_SEL		1	/* [1:1] */


extern int stmac_default_pbl(void)
{
	return 32;
}

#define GMAC_AHB_CONFIG		0x7000
static void stx7108_ethernet_bus_setup(void)
{
	const bd_t * const bd = gd->bd;

	/* Configure the bridge to generate more efficient STBus traffic.
	 *
	 * Cut Version	| Ethernet AD_CONFIG[21:0]
	 * ---------------------------------------
	 *	1.1	|	0x00264006
	 *	2.0	|	0x00264207
	 */
	if (STX7108_DEVICEID_CUT(bd->bi_devid) < 2)	/* for cut 1.x */
		writel(0x00264006, CONFIG_SYS_STM_STMAC_BASE + GMAC_AHB_CONFIG);
	else						/* for cut 2.x */
		writel(0x00264207, CONFIG_SYS_STM_STMAC_BASE + GMAC_AHB_CONFIG);
}

extern void stmac_set_mac_speed(int speed)
{
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* MII0 */
	unsigned long * const sysconfReg = (void*)STX7108_BANK2_SYSCFG(27);
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* MII1 */
	unsigned long * const sysconfReg = (void*)STX7108_BANK4_SYSCFG(23);
#else
#error Unknown base address for the STM GMAC
#endif
	unsigned long sysconf = *sysconfReg;

	/* MIIx_MAC_SPEED_SEL = 0|1 */
	SET_SYSCONF_BIT(sysconf, (speed==100), MAC_SPEED_SEL);

	*sysconfReg = sysconf;
}

/* ETH MAC pad configuration */
extern void stx7108_configure_ethernet(
	const int port,
	const struct stx7108_ethernet_config * const config)
{
	struct stm_gmac_pin * pad_config;
	struct stm_gmac_pin * phy_clock;
	const struct stm_pad_sysconf * sys_configs;
	size_t num_pads, num_sys, i;

#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* GMAC #0 */
		/* CLK_ETH_PHY_0 divider is channel #10 in "A1", using PLL1 */
	const unsigned long divider_reg = 0xfdab8800 + 0x300 + 0x4*10;
	BUG_ON(port != 0);
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* GMAC #1 */
		/* CLK_ETH_PHY_1 divider is channel #14 in "A0", using PLL1 */
	const unsigned long divider_reg = 0xfde98800 + 0x300 + 0x4*14;
	BUG_ON(port != 1);
#else
#error Unknown base address for the STM GMAC
#endif
	BUG_ON(!config);

	switch (config->mode) {

	case stx7108_ethernet_mode_mii:
		pad_config = stx7108_ethernet_mii_pad_configs;
		num_pads = ARRAY_SIZE(stx7108_ethernet_mii_pad_configs);
		sys_configs = stx7108_ethernet_mii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stx7108_ethernet_mii_pad_sysconfs);
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);
		if (config->ext_clk)
			stm_pad_set_pio_ignored(phy_clock);
		else
			stm_pad_set_pio_out(phy_clock, port + 1);
		break;

	case stx7108_ethernet_mode_rmii: /* only GMAC #1 tested */
		pad_config = stx7108_ethernet_rmii_pad_configs;
		num_pads = ARRAY_SIZE(stx7108_ethernet_rmii_pad_configs);
		sys_configs = stx7108_ethernet_rmii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stx7108_ethernet_rmii_pad_sysconfs);
		phy_clock = stm_gmac_find_phy_clock(pad_config, num_pads);
		if (config->ext_clk)
			stm_pad_set_pio_in(phy_clock, port + 2);
		else
		{
			/*
			 * Use the internal PHY_CLK, so configure the PHY Clock.
			 * i.e. CLK_ETH_PHY_0 (PIO9[3]) or CLK_ETH_PHY_1 (PIO15[5])
			 * (as appropriate) should be 50MHz for RMII mode.
			 *
			 * NOTE: We assume the TargetPack has already set the frequency
			 * to 25MHz (for MII), and hence we simply just half
			 * the divider, to double the frequency to 50MHz for RMII.
			 * WARNING: RMII will fail, if this assumption is untrue!
			 */
			unsigned long divider = readl(divider_reg);
			divider  += 1;	/* canonical to numerical form */
			divider >>= 1;	/* half the divider, double the frequency */
			divider  -= 1;	/* numerical to canonical form */
			writel(divider, divider_reg);
			stm_pad_set_pio_out(phy_clock, port + 1);
		}
		break;

	case stx7108_ethernet_mode_reverse_mii:
		BUG();		/* assume not required in U-Boot */
		return;

	case stx7108_ethernet_mode_gmii:
		BUG();		/* assume not required in U-Boot */
		return;

	case stx7108_ethernet_mode_gmii_gtx:
		BUG();		/* assume not required in U-Boot */
		return;

	case stx7108_ethernet_mode_rgmii_gtx:
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

		stx7108_pioalt_select(portno, pinno, pad->pio.alt);
		stx7108_pioalt_pad(portno, pinno, pad->direction);
		stx7108_pioalt_retime(portno, pinno, pad->retime);
	}

		/* now configure the relevant SYS_CONFIGs */
	stm_configure_sysconfs(sys_configs, num_sys);

		/* need to do this *after* the SYSCONF initialization! */
	stx7108_ethernet_bus_setup();
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

extern int soc_init(void)
{
	bd_t * const bd = gd->bd;

	stx7108_clocks();

	bd->bi_devid = *STX7108_SYSCONF_DEVICEID_0;

	/* Make sure reset period is shorter than WDT time-out */
	*STX7108_BANK0_SYSCFG(14) = 3000;	/* about 100 us */

	return 0;
}


#if defined(CONFIG_USB_OHCI_NEW) || defined(CONFIG_USB_EHCI)
extern int stx7108_usb_init(const int port)
{
	static int initialized = 0;
	unsigned int flags;
	const struct {
		struct {
			unsigned char port, pin, alt;
		} oc, pwr;
	} usb_pins[] = {
		{ .oc = { 23, 6, 1 }, .pwr = { 23, 7, 1 } },
		{ .oc = { 24, 0, 1 }, .pwr = { 24, 1, 1 } },
		{ .oc = { 24, 2, 1 }, .pwr = { 24, 3, 1 } },
	};

	if (port < 0 || port >= 3)	/* invalid port number ? */
		return -1;		/* failed to initialize! */

	if (!initialized)		/* first time ? */
	{	/* set USB2TRIPPHY_OSCIOK = 1 */
		unsigned long * const sysconfReg = (unsigned long*)STX7108_BANK4_SYSCFG(44);
		unsigned long sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, TRUE, 6);
		writel(sysconf, sysconfReg);
		initialized = 1;	/* do this just once */
	}

	/* Power up the USB port, i.e. set USB_x_POWERDOWN_REQ = 0 */
	{
		unsigned long * const sysconfReg = (unsigned long*)STX7108_BANK4_SYSCFG(46);
		unsigned long sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, FALSE, port);
		writel(sysconf, sysconfReg);
	}
	/* wait until USBx_PWR_DWN_GRANT == 0 */
	while (*STX7108_BANK4_SYSSTS(2) & (1u<<port))
		;	/* just wait ... */

	/* route the USB power enable (output) signal */
	stx7108_pioalt_select(usb_pins[port].pwr.port,
			    usb_pins[port].pwr.pin,
			    usb_pins[port].pwr.alt);
	stx7108_pioalt_pad(usb_pins[port].pwr.port,
			  usb_pins[port].pwr.pin, stm_pad_direction_output);

	/* route the USB over-current (input) signal */
	stx7108_pioalt_select(usb_pins[port].oc.port,
			    usb_pins[port].oc.pin,
			    usb_pins[port].oc.alt);
	stx7108_pioalt_pad(usb_pins[port].oc.port,
			  usb_pins[port].oc.pin, stm_pad_direction_input);

	/* start the USB Wrapper Host Controller */
	flags = USB_FLAGS_STRAP_8BIT | USB_FLAGS_STBUS_CONFIG_THRESHOLD128;
	ST40_start_host_control(flags);

	return 0;
}
#endif /* defined(CONFIG_USB_OHCI_NEW) || defined(CONFIG_USB_EHCI) */


#if defined(CONFIG_ST40_STM_SATA)
extern void stx7108_configure_sata(void)
{
#ifdef QQQ	/* QQQ - DELETE */
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
#endif		/* QQQ - DELETE */
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
static const struct ssc_pios ssc_pios[7] =
{
	{ { /* SSC0 */
		{ 1, 6 }, /* SCLK */
		{ 1, 7 }, /* MTSR */
		{ 2, 0 }, /* MRST */
	} }, { { /* SSC1 */
		{ 9, 6 }, /* SCLK */
		{ 9, 7 }, /* MTSR */
		{ 9, 5 }, /* MRST */
	} }, { { /* SSC2 */
#if 1
		{ 1, 3 }, /* SCLK */	/* Variant A */
		{ 1, 4 }, /* MTSR */
		{ 1, 5 }, /* MRST */
#else
		{ 14, 4 }, /* SCLK */	/* Variant B */
		{ 14, 5 }, /* MTSR */
		{ 14, 6 }, /* MRST */
#endif
	} }, { { /* SSC3 */
		{ 5, 2 }, /* SCLK */
		{ 5, 3 }, /* MTSR */
		{ 5, 4 }, /* MRST */
	} }, { { /* SSC4 */
		{ 13, 6 }, /* SCLK */
		{ 13, 7 }, /* MTSR */
		{ 14, 0 }, /* MRST */
	} }, { { /* SSC5 */
		{ 5, 6 }, /* SCLK */
		{ 5, 7 }, /* MTSR */
		{ 5, 5 }, /* MRST */
	} }, { { /* SSC6 */
		{ 15, 2 }, /* SCLK */
		{ 15, 3 }, /* MTSR */
		{ 15, 4 }, /* MRST */
	} },
};
// static const int stx7108_ssc_alt_funcs[] = { 2, 1, 2, 2, 1, 2, 1 };


extern void stx7108_configure_i2c(void)
{
	/*
	 * The I2C busses are routed as follows:
	 *
	 *	Bus	  SCL		  SDA
	 *	---	  ---		  ---
	 *	 0	PIO1[6]		PIO1[7]		SSC #0
	 *	 1	PIO9[6]		PIO9[7]		SSC #1
	 *	 2	PIO1[3]		PIO1[4]		SSC #2, Variant A
	 *	 2	PIO14[4]	PIO14[5]	SSC #2, Variant B
	 *	 3	PIO5[2]		PIO5[3]		SSC #3
	 *	 4	PIO13[6]	PIO13[7]	SSC #4
	 *	 5	PIO5[6]		PIO5[7]		SSC #5
	 *	 6	PIO15[2]	PIO15[3]	SSC #6
	 */
	const int scl_port = ssc_pios[CONFIG_I2C_BUS].pio[0].port;
	const int scl_pin  = ssc_pios[CONFIG_I2C_BUS].pio[0].pin;
	const int sda_port = ssc_pios[CONFIG_I2C_BUS].pio[1].port;
	const int sda_pin  = ssc_pios[CONFIG_I2C_BUS].pio[1].pin;

	if (CONFIG_I2C_BUS >= ARRAY_SIZE(ssc_pios)) BUG();

	SET_PIO_PIN(ST40_PIO_BASE(scl_port), scl_pin, STPIO_BIDIR);	/* I2C_SCL */
	SET_PIO_PIN(ST40_PIO_BASE(sda_port), sda_pin, STPIO_BIDIR);	/* I2C_SDA */
}

extern void stx7108_i2c_scl(const int val)
{
	/* SSC's SCLK == I2C's SCL */
	const int port = ssc_pios[CONFIG_I2C_BUS].pio[0].port;
	const int pin  = ssc_pios[CONFIG_I2C_BUS].pio[0].pin;
	STPIO_SET_PIN(ST40_PIO_BASE(port), pin, (val) ? 1 : 0);
}

extern void stx7108_i2c_sda(const int val)
{
	/* SSC's MTSR == I2C's SDA */
	const int port = ssc_pios[CONFIG_I2C_BUS].pio[1].port;
	const int pin  = ssc_pios[CONFIG_I2C_BUS].pio[1].pin;
	STPIO_SET_PIN(ST40_PIO_BASE(port), pin, (val) ? 1 : 0);
}

extern int stx7108_i2c_read(void)
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
extern void stx7108_configure_spi(void)
{
	/*
	 *	We set up the PIO pins correctly for SPI
	 */

	/* route PIO (alternate #0) */
	stx7108_pioalt_select(2, 1, 0);			/* SPI_MISO */
	stx7108_pioalt_select(2, 0, 0);			/* SPI_MOSI */
	stx7108_pioalt_select(1, 7, 0);			/* SPI_notCS */
	stx7108_pioalt_select(1, 6, 0);			/* SPI_CLK */

	/* set PIO directionality */
	SET_PIO_PIN(ST40_PIO_BASE(2), 1, STPIO_IN);	/* SPI_MISO */
	SET_PIO_PIN(ST40_PIO_BASE(2), 0, STPIO_OUT);	/* SPI_MOSI */
	SET_PIO_PIN(ST40_PIO_BASE(1), 7, STPIO_OUT);	/* SPI_notCS */
	SET_PIO_PIN(ST40_PIO_BASE(1), 6, STPIO_OUT);	/* SPI_CLK */

	/* drive outputs with sensible initial values */
	STPIO_SET_PIN(ST40_PIO_BASE(2), 0, 0);		/* deassert SPI_MOSI */
	STPIO_SET_PIN(ST40_PIO_BASE(1), 7, 1);		/* deassert SPI_notCS */
	STPIO_SET_PIN(ST40_PIO_BASE(1), 6, 1);		/* assert SPI_CLK */
}

extern void stx7108_spi_scl(const int val)
{
	const int pin = 6;	/* PIO1[6] = SPI_CLK */
	STPIO_SET_PIN(ST40_PIO_BASE(1), pin, val ? 1 : 0);
}

extern void stx7108_spi_sda(const int val)
{
	const int pin = 0;	/* PIO2[0] = SPI_MOSI */
	STPIO_SET_PIN(ST40_PIO_BASE(2), pin, val ? 1 : 0);
}

extern unsigned char stx7108_spi_read(void)
{
	const int pin = 1;	/* PIO2[1] = SPI_MISO */
	return STPIO_GET_PIN(ST40_PIO_BASE(2), pin);
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
	const int pin = 7;	/* PIO1[7] = SPI_NOTCS */

	/* assert SPI CSn */
	STPIO_SET_PIN(ST40_PIO_BASE(1), pin, 0);

	/* wait 1us for CSn assert to propagate  */
	udelay(1);
}
extern void spi_cs_deactivate(struct spi_slave * const slave)
{
	const int pin = 7;	/* PIO1[7] = SPI_NOTCS */

	/* DE-assert SPI CSn */
	STPIO_SET_PIN(ST40_PIO_BASE(1), pin, 1);
}
#endif	/* defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI) */

#endif	/* CONFIG_SPI && CONFIG_SOFT_SPI */

