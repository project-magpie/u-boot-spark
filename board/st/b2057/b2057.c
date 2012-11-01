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
#include <asm/stxh205reg.h>
#include <asm/io.h>
#include <asm/pio.h>


#define TX_ER_FXSD		0, 4	/* PIO0[4] == TX_ER_FXSD (or MII_TXER or ISOL) */
#if defined(CONFIG_ST40_B2075)		/* B2075 ? */
#define POWER_ON_ETH		3, 3	/* PIO3[3] == POWER_ON_ETH (a.k.a. ETH_RESET) */
#else
#define POWER_ON_ETH		2, 5	/* PIO2[5] == POWER_ON_ETH (a.k.a. ETH_RESET) */
#endif	/* CONFIG_ST40_B2075 */
#define FLASH_notWP		6, 2	/* PIO6[2] == FLASH_WP# (or FLASH_notWP) */


void flashWriteEnable(void)
{
	/* Enable Vpp for writing to flash */
#if defined(CONFIG_CMD_NAND)
	/* set FLASH_WP# = 1 */
	STPIO_SET_PIN2(FLASH_notWP, 1);
#endif	/* CONFIG_CMD_NAND */
}

void flashWriteDisable(void)
{
	/* Disable Vpp for writing to flash */
#if defined(CONFIG_CMD_NAND)
	/* set FLASH_WP# = 0 */
#if 0
	/*
	 * NOTE: currently linux does *not* explicitly enable FLASH_WP#.
	 * Hence, if we were to do the following in U-Boot, and then chain-
	 * load linux, then that linux would not be able to write to the
	 * FLASH device. So, as a temporary workaround, we do not disable
	 * the ability to write to the FLASH when passing control to linux.
	 * QQQ - to fix, once linux has been updated, and promulgated.
	 */
	STPIO_SET_PIN2(FLASH_notWP, 0);
#endif
#endif	/* CONFIG_CMD_NAND */
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

#if CONFIG_SYS_STM_ASC_BASE == STXH205_ASC1_BASE

	/* Route UART #1 via PIO11 for TX & RX (JK1, 4-pin header) */
	PIOALT(11, 0, 3, stm_pad_direction_output);	/* UART1-TX */
	PIOALT(11, 1, 3, stm_pad_direction_input);	/* UART1-RX */
//	PIOALT(11, 2, 3, stm_pad_direction_output);	/* UART1-RTS */
//	PIOALT(11, 4, 3, stm_pad_direction_input);	/* UART1-CTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH205_ASC10_BASE

	/* Route UART #10 via PIO3 for TX & RX (JM5, DB9) */
	PIOALT(3, 5, 2, stm_pad_direction_output);	/* UART10-TX */
	PIOALT(3, 6, 2, stm_pad_direction_input);	/* UART10-RX */
//	PIOALT(3, 4, 2, stm_pad_direction_output);	/* UART10-RTS */
//	PIOALT(3, 7, 2, stm_pad_direction_input);	/* UART10-CTS */

#else
#error Unknown ASC port selected!
#endif	/* CONFIG_SYS_STM_ASC_BASE == STXH205_ASCx_BASE */

#ifdef CONFIG_DRIVER_NET_STM_GMAC
	/*
	 * Configure the Ethernet PHY Reset signal
	 */
	SET_PIO_PIN2(POWER_ON_ETH, STPIO_OUT);
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

#if defined(CONFIG_CMD_NAND)
	/*
	 * Configure FLASH_WP# (Active-Low, FLASH Write-Protect)
	 */
	SET_PIO_PIN2(FLASH_notWP, STPIO_OUT);
#endif	/* CONFIG_CMD_NAND */
}

#ifdef CONFIG_DRIVER_NET_STM_GMAC
extern void stmac_phy_reset(void)
{
	/*
	 * Reset the Ethernet PHY.
	 */
	STPIO_SET_PIN2(POWER_ON_ETH, 0);
	udelay(10000);				/* 10 ms */
	STPIO_SET_PIN2(POWER_ON_ETH, 1);
	udelay(10000);				/* 10 ms */
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

extern int board_init(void)
{
	configPIO();

#ifdef CONFIG_DRIVER_NET_STM_GMAC
#if defined(CONFIG_STMAC_IP101A)
	/*
	 * This is a work around for the problem that on parts with an
	 * IC+ IP101A, the pin marked as TX_ER_FXSD (AA23) is actually
	 * ISOL, which appears to be driven high at boot time despite the
	 * internal pull down in the IC+ IP101A.
	 * In MII mode this doesn't appear to be a problem because the
	 * STxH207 is driving the pin, and so it remains low, however
	 * just in case the GMAC were to assert this signal for whatever
	 * reason, we still treat it as a gpio pin.
	 * This w/a is *not* required on parts that use a IC+ IP101G.
	 */
	SET_PIO_PIN2(TX_ER_FXSD, STPIO_OUT);
	STPIO_SET_PIN2(TX_ER_FXSD, 0);		/* deassert ISOL */
#endif /* CONFIG_STMAC_IP101A */
	/*
	 * Normally, we will use MII (or RMII) with an External Clock.
	 */
	stxh205_configure_ethernet(0, &(struct stxh205_ethernet_config) {
#if defined(CONFIG_STM_USE_RMII_MODE)
			.mode = stxh205_ethernet_mode_rmii,	/* RMII */
#else
			.mode = stxh205_ethernet_mode_mii,	/* MII */
#endif	/* CONFIG_STM_USE_RMII_MODE */
			.ext_clk = 1,				/* External Clock */
#if defined(CONFIG_STMAC_IP101A)
			.no_txer = 1,				/* NO TXER from MAC */
#endif /* CONFIG_STMAC_IP101A */
			.phy_bus = 0, });
	/* Hard Reset the PHY -- do after we have configured the MAC */
	stmac_phy_reset();
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

#if defined(CONFIG_CMD_I2C)
	stxh205_configure_i2c();
#endif	/* CONFIG_CMD_I2C */

	return 0;
}

int checkboard (void)
{
	printf ("\n\nBoard: "
#if defined(CONFIG_ST40_B2057)			/* B2057 ? */
		"STiH207-HDK (B2057)"
#elif defined(CONFIG_ST40_B2064)		/* B2064 ? */
		"STiH239-HDK (B2064)"
#elif defined(CONFIG_ST40_B2067)		/* B2067 ? */
		"STiH238-HDK (B2067)"
#elif defined(CONFIG_ST40_B2075)		/* B2075 ? */
		"STiH205-HDK (B2075)"
#else
#	error Unknown BOARD Variant for B2057!
#endif	/* CONFIG_ST40_B2057 */
#ifdef CONFIG_ST40_SE_MODE
		"  [32-bit mode]"
#else
		"  [29-bit mode]"
#endif
		"\n");

#if defined(CONFIG_SOFT_SPI)
	/*
	 * Configure for the SPI Serial Flash.
	 * Note: for CONFIG_SYS_BOOT_FROM_SPI + CONFIG_ENV_IS_IN_EEPROM, this
	 * needs to be done after env_init(), hence it is done
	 * here, and not in board_init().
	 */
	stxh205_configure_spi();
#endif	/* CONFIG_SPI */

	return 0;
}
