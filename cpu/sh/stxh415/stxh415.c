/*
 * (C) Copyright 2008-2011 STMicroelectronics.
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
#include <asm/stxh415reg.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <asm/stbus.h>
#include <asm/sysconf.h>
#include <asm/pio-control.h>
#include <ata.h>
#include <spi.h>

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
#undef DEBUG_PAD_CONFIGS
#ifdef DEBUG_PAD_CONFIGS
volatile int debug_pad_configs = 0;
#endif

	/*
	 * The STxH415 is only supported in 32-bit (SE) mode,
	 * and is not supported in 29-bit (legacy) mode.
	 * We refuse to build, if this assertion is invalid.
	 */
#if !defined(CONFIG_SH_SE_MODE)
#error This SoC is not supported in 29-bit mode, please enable SE-mode!
#endif	/* !CONFIG_SH_SE_MODE */


static void stxh415_clocks(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	/*
	 * FIXME
	 * Gross hack to get the serial port working.
	 * See the definition of PCLK in drivers/stm-asc.c
	 * for where this is used.
	 *
	 * Note: for the SBC, we expect this will always be 30MHz, so this
	 * is *less* than a gross hack! The "gross hack" is for the other cases!
	 */
#if (CFG_STM_ASC_BASE==STXH415_SBC_ASC0_BASE) || (CFG_STM_ASC_BASE==STXH415_SBC_ASC1_BASE)
	bd->bi_emifrq = 30;	/* use 30 MHz */
#else
	bd->bi_emifrq = 100;	/* use 100 MHz */
#endif
}


/*
 * PIO alternative Function selector
 */
extern void stxh415_pioalt_select(const int port, const int pin, const int alt)
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
	case 0 ... 4:		/* in "SBC Bank" */
		sysconfReg = (unsigned long*)STXH415_SYSCFG(0);
		sysconfReg += port;
		break;
	case 5 ... 12:		/* in "FRONT Bank" */
		sysconfReg = (unsigned long*)STXH415_SYSCFG(100);
		sysconfReg += port - 5;
		break;
	case 13 ... 18:		/* in "REAR Bank" */
		sysconfReg = (unsigned long*)STXH415_SYSCFG(300);
		sysconfReg += port - 13;
		break;
	default:
		BUG();
		return;
	}

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("ante: *%p = 0x%08x\n", sysconfReg, *sysconfReg);
#endif
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BITS(sysconf, TRUE, pin*4,(pin*4)+3, alt,alt);
	writel(sysconf, sysconfReg);
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("post: *%p = 0x%08x\n", sysconfReg, *sysconfReg);
#endif
}

/* Pad configuration */

#define IN		stm_pad_direction_input
#define IN_WITH_PU	stm_pad_direction_input_with_pullup
#define OUT		stm_pad_direction_output
#define BIDIR		stm_pad_direction_bidir_no_pullup
#define BIDIR_WITH_PU	stm_pad_direction_bidir_with_pullup
#define IGNORED		stm_pad_direction_ignored
#define UNKNOWN		stm_pad_direction_unknown

void stxh415_pioalt_pad(int port, const int pin,
		const enum stm_pad_gpio_direction direction)
{
	int bit;
	int oe=0, pu=0, od=0;
	unsigned long sysconf, *sysconfReg;

	switch (direction) {
	case IN:
		oe = 0; pu = 0; od = 0;
		break;
	case IN_WITH_PU:
		oe = 0; pu = 1; od = 0;
		break;
	case OUT:
		oe = 1; pu = 0; od = 0;
		break;
	case BIDIR:
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
	case 0 ... 4:		/* in "SBC Bank" */
		sysconfReg = (unsigned long*)STXH415_SYSCFG(5);
		sysconfReg += port / 4;
		break;
	case 5 ... 12:		/* in "FRONT Bank" */
		port -= 5;
		sysconfReg = (unsigned long*)STXH415_SYSCFG(108);
		sysconfReg += port / 4;
		break;
	case 13 ... 18:		/* in "REAR Bank" */
		port -= 13;
		sysconfReg = (unsigned long*)STXH415_SYSCFG(306);
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
		printf("ante: *%p = 0x%08x\n", sysconfReg, *sysconfReg);
#endif
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BIT(sysconf, oe, bit);
	writel(sysconf, sysconfReg);
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("post: *%p = 0x%08x\n", sysconfReg, *sysconfReg);
#endif

	sysconfReg += 2;	/* skip to next set of syscfg registers */

		/* set the "Pull Up" pad control */
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("ante: *%p = 0x%08x\n", sysconfReg, *sysconfReg);
#endif
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BIT(sysconf, pu, bit);
	writel(sysconf, sysconfReg);
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("post: *%p = 0x%08x\n", sysconfReg, *sysconfReg);
#endif

	sysconfReg += 2;	/* skip to next set of syscfg registers */

		/* set the "Open Drain Enable" pad control */
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("ante: *%p = 0x%08x\n", sysconfReg, *sysconfReg);
#endif
	sysconf = readl(sysconfReg);
	SET_SYSCONF_BIT(sysconf, od, bit);
	writel(sysconf, sysconfReg);
#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("post: *%p = 0x%08x\n", sysconfReg, *sysconfReg);
#endif
}

/* PIO retiming setup */

#ifdef CONFIG_DRIVER_NET_STM_GMAC

static void stxh415_pioalt_retime(const int port, const int pin,
		const struct stm_pio_control_retime_config * const cfg)
{
	unsigned long sysconf, *sysconfReg;

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("%s(port=%d, pin=%d, retime=%d, clk1notclk0=%d, "
				"clknotdata=%d, double_edge=%d, invertclk=%d, "
				"delay_input=%d)\n", __func__, port, pin,
				cfg->retime, cfg->clk1notclk0, cfg->clknotdata,
				cfg->double_edge, cfg->invertclk, cfg->delay_input
				);
	BUG_ON(pin < 0 || pin > 7);
	BUG_ON(!cfg);
#endif

	switch (port)
	{
	case 0 ... 4:		/* in "SBC Bank" */
		sysconfReg = (unsigned long*)STXH415_SYSCFG(16);
		sysconfReg += (port-0) * 2;
		break;
	case 5 ... 12:		/* in "FRONT Bank" */
		sysconfReg = (unsigned long*)STXH415_SYSCFG(116);
		sysconfReg += (port-5) * 2;
		break;
	case 13 ... 18:		/* in "REAR Bank" */
		sysconfReg = (unsigned long*)STXH415_SYSCFG(338);
		sysconfReg += (port-13) * 2;
		break;
	default:
		BUG();
		return;
	}

	sysconfReg += 0;	/* use retime configuration register #0 */

	if (cfg->clk1notclk0 >= 0)
	{
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, cfg->clk1notclk0, 0 + pin);
		writel(sysconf, sysconfReg);
	}

	if (cfg->delay_input >= 0)
	{			/* map value to 2 adjacent bitfields */
		sysconf = readl(sysconfReg);
		SET_SYSCONF_BIT(sysconf, (cfg->delay_input >> 0) & 0x1, 16 + pin);
		SET_SYSCONF_BIT(sysconf, (cfg->delay_input >> 1) & 0x1, 24 + pin);
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
}


/* --------------------------------------------------------------------
 *           Ethernet MAC resources (PAD and Retiming)
 * --------------------------------------------------------------------*/


struct stm_pad_sysconf {
	volatile unsigned long * const address;
	const int lsb;
	const int msb;
	const unsigned long value;
};

#define STM_PAD_SYSCONF(_reg, _lsb, _msb, _value) \
	{ \
		.address = _reg, \
		.lsb     = _lsb, \
		.msb     = _msb, \
		.value   = _value, \
	}
#define SYSCONF(_reg)	((unsigned long*)STXH415_SYSCFG(_reg))


struct stxh415_gmac_pin {
	struct {
		const unsigned char port, pin;
		      unsigned char alt;
	} pio;
	const char phy_clock:1;
	enum stm_pad_gpio_direction direction;
	const struct stm_pio_control_retime_config * const retime;
};

#define stm_pad_set_pio_ignored(_pin)				\
	do {							\
		(_pin)->direction = IGNORED;			\
	} while(0)

#define stm_pad_set_pio_out(_pin, _func)			\
	do {							\
		(_pin)->direction = OUT;			\
		(_pin)->pio.alt = (_func);			\
	} while(0)

#define stm_pad_set_pio_in(_pin, _func)				\
	do {							\
		(_pin)->direction = IN;				\
		(_pin)->pio.alt = (_func);			\
	} while(0)


#define DATA_IN(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = IN, \
		.retime    = _retiming, \
	}

#define DATA_OUT(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = OUT, \
		.retime    = _retiming, \
	}

/* On some boards MDIO line is missing Pull-up resistor, Enabling weak
 * internal PULL-UP overcomes the issue */
#define DATA_OUT_PU(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = IN_WITH_PU, \
		.retime    = _retiming, \
	}

#define CLOCK_IN(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = IN, \
		.retime    = _retiming, \
	}

#define CLOCK_OUT(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.direction = OUT, \
		.retime    = _retiming, \
	}

#define PHY_CLOCK(_port, _pin, _func, _retiming) \
	{ \
		.pio       = { _port, _pin, _func, }, \
		.phy_clock = 1, \
		.direction = UNKNOWN, \
		.retime    = _retiming, \
	}

/*
 * Find first pin which is tagged as being a "PHY CLOCK", and return it.
 * Otherwise return NULL, if none found!
 */
static struct stxh415_gmac_pin * find_phy_clock(
	struct stxh415_gmac_pin * const array,
	const size_t count)
{
	size_t i;

	for(i=0; i<count; i++)
	{
		if (array[i].phy_clock)
			return &array[i];	/* found it */
	}

	BUG();
	return NULL;				/* not found! */
}

#if CFG_STM_STMAC_BASE == CFG_STM_STMAC0_BASE		/* GMAC #0 */
static struct stxh415_gmac_pin stxh415_ethernet_mii_pad_configs[] = {
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
static struct stm_pad_sysconf stxh415_ethernet_mii_pad_sysconfs[] = {
			/* ETH0_GMAC_EN */
			STM_PAD_SYSCONF(SYSCONF(166), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(382), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(382), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(382), 6, 6, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(382), 8, 8, 0),
};
#elif CFG_STM_STMAC_BASE == CFG_STM_STMAC1_BASE		/* GMAC #1 */
static struct stxh415_gmac_pin stxh415_ethernet_mii_pad_configs[] = {
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
static struct stm_pad_sysconf stxh415_ethernet_mii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(31), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(29), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(29), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(29), 6, 6, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(29), 8, 8, 0),
};
#endif				/* CFG_STM_STMAC_BASE == GMAC #1 */

#if 0
#if CFG_STM_STMAC_BASE == CFG_STM_STMAC0_BASE		/* GMAC #0 */
static struct stxh415_gmac_pin stxh415_ethernet_gmii_pad_configs[] = {
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
static struct stm_pad_sysconf stxh415_ethernet_gmii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYSCONF(SYSCONF(166), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(382), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(382), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(382), 6, 6, 0),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(382), 8, 8, 0),
};
#elif CFG_STM_STMAC_BASE == CFG_STM_STMAC1_BASE		/* GMAC #1 */
static struct stxh415_gmac_pin stxh415_ethernet_gmii_pad_configs[] = {
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
static struct stm_pad_sysconf stxh415_ethernet_gmii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(31), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(29), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(29), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(29), 6, 6, 0),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(29), 8, 8, 0),
};
#endif				/* CFG_STM_STMAC_BASE == GMAC #1 */
#endif	/* 0 */

#if 0
#if CFG_STM_STMAC_BASE == CFG_STM_STMAC0_BASE		/* GMAC #0 */
static struct stxh415_gmac_pin stxh415_ethernet_rgmii_pad_configs[] = {
			PHY_CLOCK(13, 5, 2, RET_NICLK(1)),/* GTXCLK */
			DATA_IN(13, 6, 2, RET_BYPASS(0)), /* MDINT */
			DATA_OUT(13, 7, 2, RET_DE_IO(0, 0)),/* TXEN */
			DATA_OUT(14, 0, 2, RET_DE_IO(0, 0)),/* TXD[0] */
			DATA_OUT(14, 1, 2, RET_DE_IO(0, 0)),/* TXD[1] */
			DATA_OUT(14, 2, 2, RET_DE_IO(0, 0)),/* TXD[2] */
			DATA_OUT(14, 3, 2, RET_DE_IO(0, 0)),/* TXD[3] */
			/* TX Clock inversion is not set for 1000Mbps */
			CLOCK_IN(15, 0, 2, RET_ICLK(-1)),/* TXCLK */
			DATA_IN(15, 2, 2, RET_BYPASS(0)), /* CRS */
			DATA_IN(15, 3, 2, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(15, 4, 2, RET_BYPASS(3)),/* MDIO */
			CLOCK_OUT(15, 5, 2, RET_NICLK(0)),/* MDC */
			DATA_IN(15, 6, 2, RET_DE_IO(0, 0)),/* RXDV */
			DATA_IN(16, 0, 2, RET_DE_IO(0, 0)),/* RXD[0] */
			DATA_IN(16, 1, 2, RET_DE_IO(0, 0)),/* RXD[1] */
			DATA_IN(16, 2, 2, RET_DE_IO(0, 0)),/* RXD[2] */
			DATA_IN(16, 3, 2, RET_DE_IO(0, 0)),/* RXD[3] */
			CLOCK_IN(17, 0, 2, RET_NICLK(-1)),/* RXCLK */
};
static struct stm_pad_sysconf stxh415_ethernet_rgmii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYSCONF(SYSCONF(166), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(382), 2, 4, 1),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(382), 5, 5, 1),
			/* Extra retime config base on speed */
};
#elif CFG_STM_STMAC_BASE == CFG_STM_STMAC1_BASE		/* GMAC #1 */
static struct stxh415_gmac_pin stxh415_ethernet_rgmii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_DE_IO(0, 1)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_DE_IO(0, 1)),/* TXD[1] */
			DATA_OUT(0, 2, 1, RET_DE_IO(0, 1)),/* TXD[2] */
			DATA_OUT(0, 3, 1, RET_DE_IO(0, 1)),/* TXD[3] */
			DATA_OUT(0, 5, 1, RET_DE_IO(0, 1)),/* TXEN */
			/* TX Clock inversion is not set for 1000Mbps */
			CLOCK_IN(0, 6, 1, RET_ICLK(-1)),/* TXCLK */
			DATA_IN(0, 7, 1, RET_BYPASS(0)),/* COL */
			DATA_OUT_PU(1, 0, 1, RET_BYPASS(2)),/* MDIO */
			CLOCK_OUT(1, 1, 1, RET_NICLK(1)),/* MDC */
			DATA_IN(1, 2, 1, RET_BYPASS(0)),/* CRS */
			DATA_IN(1, 3, 1, RET_BYPASS(0)),/* MDINT */
			DATA_IN(2, 0, 1, RET_DE_IO(0, 1)),/* RXDV */
			CLOCK_IN(2, 2, 1, RET_NICLK(-1)),/* RXCLK */
			PHY_CLOCK(2, 3, 1, RET_NICLK(1)), /* GTXCLK*/
			DATA_IN(3, 0, 1, RET_DE_IO(0, 1)),/* RXD[0] */
			DATA_IN(3, 1, 1, RET_DE_IO(0, 1)),/* RXD[1] */
			DATA_IN(3, 2, 1, RET_DE_IO(0, 1)),/* RXD[2] */
			DATA_IN(3, 3, 1, RET_DE_IO(0, 1)),/* RXD[3] */
};
static struct stm_pad_sysconf stxh415_ethernet_rgmii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(31), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(29), 2, 4, 1),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(29), 5, 5, 1),
			/* Extra retime config base on speed */
};
#endif				/* CFG_STM_STMAC_BASE == GMAC #1 */
#endif	/* 0 */

#if CFG_STM_STMAC_BASE == CFG_STM_STMAC0_BASE		/* GMAC #0 */
static struct stxh415_gmac_pin stxh415_ethernet_rmii_pad_configs[] = {
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
static struct stm_pad_sysconf stxh415_ethernet_rmii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYSCONF(SYSCONF(166), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(382), 2, 4, 4),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(382), 5, 5, 1),
			/* ETH0_SEL_INTERNAL_NOT_EXT_PHYCLK */
			STM_PAD_SYSCONF(SYSCONF(382), 7, 7, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(382), 8, 8, 1),
};
#elif CFG_STM_STMAC_BASE == CFG_STM_STMAC1_BASE		/* GMAC #1 */
static struct stxh415_gmac_pin stxh415_ethernet_rmii_pad_configs[] = {
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
static struct stm_pad_sysconf stxh415_ethernet_rmii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(31), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(29), 2, 4, 4),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(29), 5, 5, 1),
			/* ETH1_SEL_INTERNAL_NOT_EXT_PHYCLK */
			STM_PAD_SYSCONF(SYSCONF(29), 7, 7, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(29), 8, 8, 1),
};
#endif				/* CFG_STM_STMAC_BASE == GMAC #1 */

#if 0
#if CFG_STM_STMAC_BASE == CFG_STM_STMAC0_BASE		/* GMAC #0 */
static struct stxh415_gmac_pin stxh415_ethernet_reverse_mii_pad_configs[] = {
			DATA_OUT(14, 0, 2, RET_BYPASS(0)),/* TXD[0] */
			DATA_OUT(14, 1, 2, RET_BYPASS(0)),/* TXD[1] */
			DATA_OUT(14, 2, 2, RET_BYPASS(0)),/* TXD[2] */
			DATA_OUT(14, 3, 2, RET_BYPASS(0)),/* TXD[3] */
			DATA_OUT(15, 1, 2, RET_SE_NICLK_IO(0, 0)),/* TXER */
			DATA_OUT(13, 7, 2, RET_SE_NICLK_IO(0, 0)),/* TXEN */
			CLOCK_IN(15, 0, 2, RET_NICLK(-1)),/* TXCLK */
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
			CLOCK_IN(17, 0, 2, RET_NICLK(-1)),/* RXCLK */
			PHY_CLOCK(13, 5, 2, RET_NICLK(0)),/* PHYCLK */
};
static struct stm_pad_sysconf stxh415_ethernet_reverse_mii_pad_sysconfs[] = {
			/* EN_GMAC0 */
			STM_PAD_SYSCONF(SYSCONF(166), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(382), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(382), 5, 5, 0),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(382), 6, 6, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(382), 8, 8, 0),
};
#elif CFG_STM_STMAC_BASE == CFG_STM_STMAC1_BASE		/* GMAC #1 */
static struct stxh415_gmac_pin stxh415_ethernet_reverse_mii_pad_configs[] = {
			DATA_OUT(0, 0, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[0] */
			DATA_OUT(0, 1, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[1] */
			DATA_OUT(0, 2, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[2] */
			DATA_OUT(0, 3, 1, RET_SE_NICLK_IO(0, 1)),/* TXD[3] */
			DATA_OUT(0, 4, 1, RET_SE_NICLK_IO(0, 1)),/* TXER */
			DATA_OUT(0, 5, 1, RET_SE_NICLK_IO(0, 1)),/* TXEN */
			CLOCK_IN(0, 6, 1, RET_NICLK(-1)),/* TXCLK */
			DATA_OUT(0, 7, 2, RET_BYPASS(0)),/* COL */
			DATA_IN(2, 0, 1, RET_SE_NICLK_IO(2, 1)),/* RXDV */
			DATA_IN(2, 1, 1, RET_SE_NICLK_IO(2, 1)),/* RX_ER */
			CLOCK_IN(2, 2, 1, RET_NICLK(-1)),/* RXCLK */
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
static struct stm_pad_sysconf stxh415_ethernet_reverse_mii_pad_sysconfs[] = {
			/* EN_GMAC1 */
			STM_PAD_SYSCONF(SYSCONF(31), 0, 0, 1),
			/* MIIx_PHY_SEL */
			STM_PAD_SYSCONF(SYSCONF(29), 2, 4, 0),
			/* ENMIIx */
			STM_PAD_SYSCONF(SYSCONF(29), 5, 5, 1),
			/* TXCLK_NOT_CLK125 */
			STM_PAD_SYSCONF(SYSCONF(29), 6, 6, 1),
			/* TX_RETIMING_CLK */
			STM_PAD_SYSCONF(SYSCONF(29), 8, 8, 0),
};
#endif				/* CFG_STM_STMAC_BASE == GMAC #1 */
#endif	/* 0 */


/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/


#define MAC_SPEED_SEL		1	/* [1:1] */

extern int stmac_default_pbl(void)
{
	return 32;
}

#define GMAC_AHB_CONFIG         0x2000
static void stxh415_ethernet_bus_setup(void)
{
	/* Configure the bridge to generate more efficient STBus traffic.
	 *
	 * Cut Version	| Ethernet AD_CONFIG[21:0]
	 *	1.0	|	0x00264207
	 */
	writel(0x00264207, CFG_STM_STMAC_BASE + GMAC_AHB_CONFIG);
}

extern void stmac_set_mac_speed(int speed)
{
#if 0	/* QQQ - TO IMPLEMENT */
#if CFG_STM_STMAC_BASE == CFG_STM_STMAC0_BASE		/* MII0 */
	unsigned long * const sysconfReg = (void*)STX7108_MII_SYSGFG(27);
#elif CFG_STM_STMAC_BASE == CFG_STM_STMAC1_BASE		/* MII1 */
	unsigned long * const sysconfReg = (void*)STX7108_MII_SYSGFG(23);
#else
#error Unknown base address for the STM GMAC
#endif
	unsigned long sysconf = *sysconfReg;

	/* MIIx_MAC_SPEED_SEL = 0|1 */
	SET_SYSCONF_BIT(sysconf, (speed==100), MAC_SPEED_SEL);

	*sysconfReg = sysconf;
#endif	/* QQQ - TO IMPLEMENT */
}

/* ETH MAC pad configuration */
extern void stxh415_configure_ethernet(
	const int port,
	const struct stxh415_ethernet_config * const config)
{
	struct stxh415_gmac_pin * pad_config;
	struct stxh415_gmac_pin * phy_clock;
	const struct stm_pad_sysconf * sys_config;
	size_t num_pads, num_sys, i;

	BUG_ON(!config);
#if CFG_STM_STMAC_BASE == CFG_STM_STMAC0_BASE		/* GMAC #0 */
	BUG_ON(port != 0);
#elif CFG_STM_STMAC_BASE == CFG_STM_STMAC1_BASE		/* GMAC #1 */
	BUG_ON(port != 1);
#else
#error Unknown base address for the STM GMAC
#endif

	switch (config->mode) {

	case stxh415_ethernet_mode_mii:
		pad_config = stxh415_ethernet_mii_pad_configs;
		num_pads = ARRAY_SIZE(stxh415_ethernet_mii_pad_configs);
		sys_config = stxh415_ethernet_mii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh415_ethernet_mii_pad_sysconfs);
		phy_clock = find_phy_clock(pad_config, num_pads);
		if (config->ext_clk)
			stm_pad_set_pio_ignored(phy_clock);
		else
			stm_pad_set_pio_out(phy_clock, 2 - port);
		break;

	case stxh415_ethernet_mode_rmii:  {
		pad_config = stxh415_ethernet_rmii_pad_configs;
		num_pads = ARRAY_SIZE(stxh415_ethernet_rmii_pad_configs);
		sys_config = stxh415_ethernet_rmii_pad_sysconfs;
		num_sys = ARRAY_SIZE(stxh415_ethernet_rmii_pad_sysconfs);

		/* we assume we *always* use the internal clock for RMII! */
		BUG_ON(config->ext_clk!= 0);

		/*
		 * Configure the PHY Clock:
		 * CLKS_GMAC0_PHY (PIO13[5]) or CLKS_ETH1_PHY (PIO2[3]) should
		 * be 50MHz for RMII mode (as appropriate).
		 * Note: We rely on the GDB "pokes" to set the frequency for us ...
		 */
		phy_clock = find_phy_clock(pad_config, num_pads);
		stm_pad_set_pio_out(phy_clock, 2 - port);
	} break;

	case stxh415_ethernet_mode_reverse_mii:
		BUG();		/* assume not required in U-Boot */
		return;

	case stxh415_ethernet_mode_gmii:
		BUG();		/* assume not required in U-Boot */
		return;

	case stxh415_ethernet_mode_gmii_gtx:
		BUG();		/* assume not required in U-Boot */
		return;

	case stxh415_ethernet_mode_rgmii_gtx:
		BUG();		/* assume not required in U-Boot */
		return;

	default:
		BUG();
		return;
	}

		/* now configure all the PIOs */
	for (i = 0; i < num_pads; i++)
	{
		const struct stxh415_gmac_pin * const pad = &pad_config[i];
		const int portno = pad->pio.port;
		const int pinno = pad->pio.pin;

#ifdef DEBUG_PAD_CONFIGS
	if (debug_pad_configs)
		printf("%2u: PIO%03u[%u] %-7s, alt=%u, retime=%p\n",
			i+1,
			portno, pinno,
			(pad->direction==IN) ? "in" :
				(pad->direction==IN_WITH_PU) ? "in+pu" :
				(pad->direction==OUT) ? "out" :
				(pad->direction==BIDIR) ? "bidir" :
				(pad->direction==IGNORED) ? "ignore" :
				"BAD-BAD",
			pad->pio.alt,
			pad->retime
		);
#endif

		if (pad->direction == IGNORED)
			continue;	/* skip all "ignored" pads */

		stxh415_pioalt_select(portno, pinno, pad->pio.alt);
		stxh415_pioalt_pad(portno, pinno, pad->direction);
		stxh415_pioalt_retime(portno, pinno, pad->retime);
	}

		/* now configure the relevant SYS_CONFIGs */
	for (i = 0; i < num_sys; i++)
	{
		const struct stm_pad_sysconf * const sys = &sys_config[i];
		unsigned long sysconf;

#ifdef DEBUG_PAD_CONFIGS
		if (debug_pad_configs) {
			printf("%2u: SYSCFG=%p,  [%u:%u]\t0x%08x\n",
				i+1,
				sys->address,
				sys->msb, sys->lsb,
				sys->value
			);
			printf("ante: *%p = 0x%08x\n", sys->address, *sys->address);
		}
#endif
		sysconf = readl(sys->address);
		SET_SYSCONF_BITS(sysconf, TRUE, sys->lsb, sys->msb, sys->value,sys->value);
		writel(sysconf, sys->address);
#ifdef DEBUG_PAD_CONFIGS
		if (debug_pad_configs)
			printf("post: *%p = 0x%08x\n", sys->address, *sys->address);
#endif
	}

		/* need to do this *after* the SYSCONF initialization! */
	stxh415_ethernet_bus_setup();
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */


extern int soc_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	stxh415_clocks();

	bd->bi_devid = *STXH415_SYSCONF_DEVICEID;

#if 0	/* QQQ - TO IMPLEMENT */
	/* Make sure reset period is shorter than WDT time-out */
	*STX7108_BANK0_SYSCFG(14) = 3000;	/* about 100 us */
#endif	/* QQQ - TO IMPLEMENT */

	return 0;
}


#if defined(CONFIG_USB_OHCI_NEW)
extern int stxh415_usb_init(const int port)
{
#if 0	/* QQQ - TO IMPLEMENT */
//QQQ	DECLARE_GLOBAL_DATA_PTR;
//QQQ	bd_t *bd = gd->bd;
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
	stxh415_pioalt_select(usb_pins[port].pwr.port,
			    usb_pins[port].pwr.pin,
			    usb_pins[port].pwr.alt);
	stxh415_pioalt_pad(usb_pins[port].pwr.port,
			  usb_pins[port].pwr.pin, OUT);

	/* route the USB over-current (input) signal */
	stxh415_pioalt_select(usb_pins[port].oc.port,
			    usb_pins[port].oc.pin,
			    usb_pins[port].oc.alt);
	stxh415_pioalt_pad(usb_pins[port].oc.port,
			  usb_pins[port].oc.pin, IN);

	/* start the USB Wrapper Host Controller */
#if 1	/* QQQ - DELETE */
	flags = USB_FLAGS_STRAP_8BIT | USB_FLAGS_STBUS_CONFIG_THRESHOLD128;
#else	/* QQQ - DELETE */
	if (STX7108_DEVICEID_CUT(bd->bi_devid) < 2)
	{		/* for cut 1.x */
		flags = USB_FLAGS_STRAP_8BIT |
			USB_FLAGS_STBUS_CONFIG_OPCODE_LD64_ST64 |
			USB_FLAGS_STBUS_CONFIG_PKTS_PER_CHUNK_1 |
			USB_FLAGS_STBUS_CONFIG_THRESHOLD_64;
	}
	else
	{		/* for cut 2.x */
		flags = USB_FLAGS_STRAP_8BIT |
			USB_FLAGS_STBUS_CONFIG_OPCODE_LD64_ST64 |
			USB_FLAGS_STBUS_CONFIG_PKTS_PER_CHUNK_2 |
			USB_FLAGS_STBUS_CONFIG_THRESHOLD_128;
	}
#endif	/* QQQ - DELETE */
	ST40_start_host_control(flags);

	return 0;
#endif	/* QQQ - TO IMPLEMENT */
}

#endif /* defined(CONFIG_USB_OHCI_NEW) */


#if defined(CONFIG_SH_STM_SATA)
extern void stxh415_configure_sata(void)
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
#endif	/* CONFIG_SH_STM_SATA */


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

extern void stxh415_configure_i2c(void)
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
	stxh415_pioalt_select(scl_port, scl_pin, 0);			/* I2C_SCL */
	stxh415_pioalt_select(sda_port, sda_pin, 0);			/* I2C_SDA */

	/* set up directionality appropriately */
	SET_PIO_PIN(ST40_PIO_BASE(scl_port), scl_pin, STPIO_BIDIR);	/* I2C_SCL */
	SET_PIO_PIN(ST40_PIO_BASE(sda_port), sda_pin, STPIO_BIDIR);	/* I2C_SDA */
}

extern void stxh415_i2c_scl(const int val)
{
	/* SSC's SCLK == I2C's SCL */
	const int port = ssc_pios[CONFIG_I2C_BUS].pio[0].port;
	const int pin  = ssc_pios[CONFIG_I2C_BUS].pio[0].pin;
	STPIO_SET_PIN(ST40_PIO_BASE(port), pin, (val) ? 1 : 0);
}

extern void stxh415_i2c_sda(const int val)
{
	/* SSC's MTSR == I2C's SDA */
	const int port = ssc_pios[CONFIG_I2C_BUS].pio[1].port;
	const int pin  = ssc_pios[CONFIG_I2C_BUS].pio[1].pin;
	STPIO_SET_PIN(ST40_PIO_BASE(port), pin, (val) ? 1 : 0);
}

extern int stxh415_i2c_read(void)
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
	return CFG_I2C_SPEED;
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

extern void stxh415_configure_spi(void)
{
	/*
	 *	We set up the PIO pins correctly for SPI
	 */

	/* route PIO (alternate #0) */
	STXH415_PIOALT_SELECT(SPI_MISO, 0);	/* SPI_MISO */
	STXH415_PIOALT_SELECT(SPI_MOSI, 0);	/* SPI_MOSI */
	STXH415_PIOALT_SELECT(SPI_nCS,  0);	/* SPI_nCS */
	STXH415_PIOALT_SELECT(SPI_CLK,  0);	/* SPI_CLK */

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

extern void stxh415_spi_scl(const int val)
{
	STPIO_SET_PIN2(SPI_CLK, val ? 1 : 0);
}

extern void stxh415_spi_sda(const int val)
{
	STPIO_SET_PIN2(SPI_MOSI, val ? 1 : 0);
}

extern unsigned char stxh415_spi_read(void)
{
	return STPIO_GET_PIN2(SPI_MISO);
}

/*
 * assert or de-assert the SPI Chip Select line.
 *
 *	input: cs == true, assert CS, else deassert CS
 */
static void spi_chip_select(const int cs)
{
	if (cs)
	{	/* assert SPI CSn */
		STPIO_SET_PIN2(SPI_nCS, 0);
	}
	else
	{	/* DE-assert SPI CSn */
		STPIO_SET_PIN2(SPI_nCS, 1);
	}

	if (cs)
	{	/* wait 250ns for CSn assert to propagate  */
		udelay(1);	/* QQQ: can we make this shorter ? */
	}
}

/*
 * The SPI command uses this table of functions for controlling the SPI
 * chip selects: it calls the appropriate function to control the SPI
 * chip selects.
 */
spi_chipsel_type spi_chipsel[] =
{
	spi_chip_select
};
int spi_chipsel_cnt = sizeof(spi_chipsel) / sizeof(spi_chipsel[0]);

#endif	/* CONFIG_SPI && CONFIG_SOFT_SPI */

