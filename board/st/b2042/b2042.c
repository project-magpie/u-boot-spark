/*
 * (C) Copyright 2008-2012 STMicroelectronics.
 *
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
#include <asm/stx7141reg.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <spi.h>


extern void flashWriteEnable(void)
{
	/* Enable Vpp for writing to flash */
	/* Nothing to do! */
}

extern void flashWriteDisable(void)
{
	/* Disable Vpp for writing to flash */
	/* Nothing to do! */
}

#ifdef CONFIG_DRIVER_NET_STM_GMAC
extern void stmac_phy_reset(void)
{
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* MAC = STM GMAC#1 */
	/* Reset the on-board IC+ IP1001 PHY (U51) */
	SET_PIO_PIN(PIO_PORT(15), 7, STPIO_OUT);

	STPIO_SET_PIN(PIO_PORT(15), 7, 1);
	udelay(10);

	STPIO_SET_PIN(PIO_PORT(15), 7, 0);
	udelay(50);

	STPIO_SET_PIN(PIO_PORT(15), 7, 1);
	udelay(10);
#else
	/* Reset the on-board IC+ IP1001 PHY (U51) */
	SET_PIO_PIN(PIO_PORT(5), 3, STPIO_OUT);

	STPIO_SET_PIN(PIO_PORT(5), 3, 1);
	udelay(10);

	STPIO_SET_PIN(PIO_PORT(5), 3, 0);
	udelay(50);

	STPIO_SET_PIN(PIO_PORT(5), 3, 1);
	udelay(10);
#endif
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */


#if defined(CONFIG_SPI)

#define SPI_CLK		2, 0	/* PIO2[0] = SPI_CLK */
#define SPI_DOUT	2, 1	/* PIO2[1] = SPI_DOUT */
#define SPI_DIN		2, 2	/* PIO2[2] = SPI_DIN */
#define SPI_notCS	2, 5	/* PIO2[5] = SPI_notCS */

#if defined(CONFIG_SOFT_SPI)			/* Use "bit-banging" for SPI */
extern void stx7141_spi_scl(const int val)
{
	STPIO_SET_PIN2(SPI_CLK, val ? 1 : 0);
}

extern void stx7141_spi_sda(const int val)
{
	STPIO_SET_PIN2(SPI_DOUT, val ? 1 : 0);
}

extern unsigned char stx7141_spi_read(void)
{
	return STPIO_GET_PIN2(SPI_DIN);
}
#endif	/* CONFIG_SOFT_SPI */

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
	STPIO_SET_PIN2(SPI_notCS, 0);

	/* wait 1us for CSn assert to propagate  */
	udelay(1);
}
extern void spi_cs_deactivate(struct spi_slave * const slave)
{
	/* DE-assert SPI CSn */
	STPIO_SET_PIN2(SPI_notCS, 1);
}
#endif	/* defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI) */


static void configSpi(void)
{
#if defined(CONFIG_STM_SSC_SPI)		/* Use the H/W SSC for SPI */
	unsigned long sysconf;
#endif	/* CONFIG_STM_SSC_SPI */

	/* Use PIO2[5] for SPI CSn */
	SET_PIO_PIN2(SPI_notCS, STPIO_OUT);	/* SPI_notCS */
	STPIO_SET_PIN2(SPI_notCS, 1);		/* deassert SPI_notCS */

#if defined(CONFIG_SOFT_SPI)	/* Configure SPI Serial Flash for PIO "bit-banging" */
	SET_PIO_PIN2(SPI_DIN, STPIO_IN);	/* SPI_DIN */
	SET_PIO_PIN2(SPI_CLK, STPIO_OUT);	/* SPI_CLK */
	SET_PIO_PIN2(SPI_DOUT, STPIO_OUT);	/* SPI_DOUT */

	/* drive outputs with sensible initial values */
	STPIO_SET_PIN2(SPI_CLK, 1);		/* assert SPI_CLK */
	STPIO_SET_PIN2(SPI_DOUT, 0)	;	/* deassert SPI_DOUT */
#elif defined(CONFIG_STM_SSC_SPI)		/* Use the H/W SSC for SPI */
	/* Set PIO2[2:0]_SELECTOR to AltFunction #1 (SSC #0) */
	sysconf = *STX7141_SYSCONF_SYS_CFG19;
	/* PIO2[0] CFG19[12]  AltFunction = 1 */
	/* PIO2[1] CFG19[13]  AltFunction = 1 */
	/* PIO2[2] CFG19[14]  AltFunction = 1 */
	sysconf |= 1u<<12;	/* PIO2[0] */
	sysconf |= 1u<<13;	/* PIO2[1] */
	sysconf |= 1u<<14;	/* PIO2[2] */
	*STX7141_SYSCONF_SYS_CFG19 = sysconf;

	/* SSC #0 is on PIO2[2:0] */
	SET_PIO_PIN2(SPI_DIN, STPIO_IN);	/* SPI_DIN */
	SET_PIO_PIN2(SPI_CLK, STPIO_ALT_OUT);	/* SPI_CLK */
	SET_PIO_PIN2(SPI_DOUT, STPIO_ALT_OUT);	/* SPI_DOUT */
#endif	/* CONFIG_SOFT_SPI */
}
#endif	/* CONFIG_SPI */


static void configPIO(void)
{
	unsigned long sysconf;

	/* Setup PIO of ASC device */
	SET_PIO_ASC(PIO_PORT(1), 0, 1, STPIO_NO_PIN, STPIO_NO_PIN);  /* ASC2 */

	/* Enable ASC UART */
	sysconf = *STX7141_SYSCONF_SYS_CFG36;
	/* CFG36[31] = 0 = UART2_RXD_SEL = PIO1[1] */
	sysconf &= ~(1ul << 31);
	*STX7141_SYSCONF_SYS_CFG36 = sysconf;

	/* Route ASC2 via PIO[1] for TX & RX */
	sysconf = *STX7141_SYSCONF_SYS_CFG19;
	/* PIO1[0] Selector: CFG19[1:0] = 3 TX */
	/* PIO1[1] Selector: CFG19[3:2] = 3 RX */
	sysconf |= 3ul << 0 | 3ul << 2;
	*STX7141_SYSCONF_SYS_CFG19 = sysconf;

#if defined(CONFIG_SPI)
	/* Configure for SPI Serial Flash */
	configSpi();
#endif	/* CONFIG_SPI */
}

extern int board_init(void)
{
	configPIO();

#ifdef CONFIG_DRIVER_NET_STM_GMAC
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* MAC = STM GMAC#0 */
	stx7141_configure_ethernet(0, 0, 0, 0);
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* MAC = STM GMAC#1 */
	stx7141_configure_ethernet(1, 0, 0, 1);
#else
#error Unknown GMAC Base address encountered!
#endif
	/* Hard Reset the PHY -- do after we have configured the MAC */
	stmac_phy_reset();
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

#if defined(CONFIG_ST40_STM_SATA)
	stx7141_configure_sata ();
#endif	/* CONFIG_ST40_STM_SATA */

	return 0;
}

extern int checkboard (void)
{
	printf ("\n\nBoard: STiH225-HDK (B2042)"
#ifdef CONFIG_ST40_SE_MODE
		"  [32-bit mode]"
#else
		"  [29-bit mode]"
#endif
		"\n");

	return 0;
}
