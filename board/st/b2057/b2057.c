/*
 * (C) Copyright 2008-2011 STMicroelectronics.
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
#include <asm/stxh205reg.h>
#include <asm/io.h>
#include <asm/pio.h>


void flashWriteEnable(void)
{
	/* Enable Vpp for writing to flash */
	/* Nothing to do */
}

void flashWriteDisable(void)
{
	/* Disable Vpp for writing to flash */
	/* Nothing to do */
}


#define PIOALT(port, pin, alt, dir)			\
do							\
{							\
	stxh205_pioalt_select((port), (pin), (alt));	\
	stxh205_pioalt_pad((port), (pin), (dir));	\
} while(0)

static void configPIO(void)
{
	/* Setup PIOs for ASC device */

#if CFG_STM_ASC_BASE == STXH205_ASC1_BASE

	/* Route UART #1 via PIO11 for TX & RX (JK1, 4-pin header) */
	PIOALT(11, 0, 3, stm_pad_direction_output);	/* UART1-TX */
	PIOALT(11, 1, 3, stm_pad_direction_input);	/* UART1-RX */
//	PIOALT(11, 2, 3, stm_pad_direction_output);	/* UART1-RTS */
//	PIOALT(11, 4, 3, stm_pad_direction_input);	/* UART1-CTS */

#elif CFG_STM_ASC_BASE == STXH205_ASC10_BASE

	/* Route UART #10 via PIO3 for TX & RX (JM5, DB9) */
	PIOALT(3, 5, 2, stm_pad_direction_output);	/* UART10-TX */
	PIOALT(3, 6, 2, stm_pad_direction_input);	/* UART10-RX */
//	PIOALT(3, 4, 2, stm_pad_direction_output);	/* UART10-RTS */
//	PIOALT(3, 7, 2, stm_pad_direction_input);	/* UART10-CTS */

#else
#error Unknown ASC port selected!
#endif	/* CFG_STM_ASC_BASE == STXH205_ASCx_BASE */

#if 0	/* QQQ - TO DO */
#ifdef CONFIG_DRIVER_NET_STM_GMAC
	/*
	 * Configure the Ethernet PHY Reset signal
	 *	PIO3[6] == POWER_ON_ETH (a.k.a. ETH_RESET)
	 */
	SET_PIO_PIN(ST40_PIO_BASE(3), 6, STPIO_OUT);
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */
#endif	/* QQQ - TO DO */
}

#if 0	/* QQQ - TO DO */
#ifdef CONFIG_DRIVER_NET_STM_GMAC
extern void stmac_phy_reset(void)
{
	/*
	 * Reset the Ethernet PHY.
	 *
	 *	PIO3[6] = POWER_ON_ETH (a.k.a. ETH_RESET)
	 */
	STPIO_SET_PIN(ST40_PIO_BASE(3), 6, 0);
	udelay(10000);				/* 10 ms */
	STPIO_SET_PIN(ST40_PIO_BASE(3), 6, 1);
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */
#endif	/* QQQ - TO DO */

extern int board_init(void)
{
	configPIO();

#if 0	/* QQQ - TO DO */
#ifdef CONFIG_DRIVER_NET_STM_GMAC
	/* Reset the PHY */
	stmac_phy_reset();
#if CFG_STM_STMAC_BASE == CFG_STM_STMAC0_BASE
		/* MII0: RMII with 25MHz External Clock  */
	stxh205_configure_ethernet(0, &(struct stxh205_ethernet_config) {
			.mode = stxh205_ethernet_mode_rmii,
			.ext_clk = 1,
			.phy_bus = 0, });
#else
#error Unknown base address for the STM GMAC
#endif
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */
#endif	/* QQQ - TO DO */

#if defined(CONFIG_CMD_I2C)
	stxh205_configure_i2c();
#endif	/* CONFIG_CMD_I2C */

	return 0;
}

int checkboard (void)
{
	printf ("\n\nBoard: STiH207-HDK (B2057)"
#ifdef CONFIG_SH_SE_MODE
		"  [32-bit mode]"
#else
		"  [29-bit mode]"
#endif
		"\n");

#if defined(CONFIG_SOFT_SPI)
	/*
	 * Configure for the SPI Serial Flash.
	 * Note: for CFG_BOOT_FROM_SPI + CFG_ENV_IS_IN_EEPROM, this
	 * needs to be done after env_init(), hence it is done
	 * here, and not in board_init().
	 */
	stxh205_configure_spi();
#endif	/* CONFIG_SPI */

	return 0;
}
