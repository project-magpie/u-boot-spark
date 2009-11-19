/*
 * (C) Copyright 2009 STMicroelectronics.
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
#include <asm/fli7510reg.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <asm/soc.h>


extern void flashWriteEnable (void)
{
	/* Enable Vpp for writing to flash */
	/* Nothing to do! */
}

extern void flashWriteDisable (void)
{
	/* Disable Vpp for writing to flash */
	/* Nothing to do! */
}


#ifdef CONFIG_STM_ASC_SERIAL
static void configSerial (void)
{
#if (CFG_STM_ASC_BASE == ST40_ASC0_REGS_BASE)	/* UART #1 */
	/* Route UART #1 via PIO9 for TX, RX, CTS & RTS */
	SET_PIO_ASC(PIO_PORT(9), 3, 2, 1, 0);
#elif (CFG_STM_ASC_BASE == ST40_ASC1_REGS_BASE)	/* UART #2 */
	/* Route UART #2 via PIO25 for TX, RX, CTS & RTS */
	SET_PIO_ASC(PIO_PORT(25), 5, 4, 3, 2);
#elif (CFG_STM_ASC_BASE == ST40_ASC2_REGS_BASE)	/* UART #3 */
	/* Route UART #3 via PIO25 for TX & RX */
	SET_PIO_ASC(PIO_PORT(25), 7, 6, STPIO_NO_PIN, STPIO_NO_PIN);
#else
#error Unknown serial port configuration!
#endif
}
#endif /* CONFIG_STM_ASC_SERIAL */


#if defined(CONFIG_SPI)
static void configSpi(void)
{
	unsigned long sysconf;

	/*
	 * CFG_COMMS_CONFIG_2[13] = spi_enable = 0
	 * i.e. disable the SPI boot-controller.
	 */
	sysconf = readl(CFG_COMMS_CONFIG_2);
	sysconf &= ~(1ul<<13);
	writel(sysconf, CFG_COMMS_CONFIG_2);

#if defined(CONFIG_SOFT_SPI)
	/* Configure SPI Serial Flash for PIO "bit-banging" */

	/* SPI is on PIO17[5:0] */
	SET_PIO_PIN(PIO_PORT(17),5,STPIO_IN);	/* SPI_MISO */
	SET_PIO_PIN(PIO_PORT(17),2,STPIO_OUT);	/* SPI_CLK */
	SET_PIO_PIN(PIO_PORT(17),3,STPIO_OUT);	/* SPI_MOSI */
	SET_PIO_PIN(PIO_PORT(17),4,STPIO_OUT);	/* SPI_CSN */

	/* drive outputs with sensible initial values */
	STPIO_SET_PIN(PIO_PORT(17), 4, 1);	/* deassert SPI_CSN */
	STPIO_SET_PIN(PIO_PORT(17), 2, 1);	/* assert SPI_CLK */
	STPIO_SET_PIN(PIO_PORT(17), 3, 0);	/* deassert SPI_MOSI */
#endif	/* CONFIG_SOFT_SPI */
}
#endif	/* CONFIG_SPI */


extern int board_init (void)
{
#ifdef CONFIG_STM_ASC_SERIAL
	configSerial ();
#endif /* CONFIG_STM_ASC_SERIAL */

#ifdef CONFIG_DRIVER_NET_STM_GMAC
	fli7510_configure_ethernet (fli7510_ethernet_mii, 0, 0);
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

#if defined(CONFIG_SPI)
	/* Configure for SPI Serial Flash */
	configSpi();
#endif	/* CONFIG_SPI */

	return 0;
}


extern int checkboard (void)
{
	printf ("\n\nBoard: FLI7510 Development Board"
#ifdef CONFIG_SH_SE_MODE
		"  [32-bit mode]"
#else
		"  [29-bit mode]"
#endif
		"\n");

	return 0;
}
