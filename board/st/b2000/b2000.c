/*
 * (C) Copyright 2008-2013 STMicroelectronics.
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
#include <asm/socregs.h>
#include <asm/io.h>
#include <asm/pio.h>


void flashWriteEnable(void)
{
	/* Enable Vpp for writing to flash */
	/* Nothing to do! */
}

void flashWriteDisable(void)
{
	/* Disable Vpp for writing to flash */
	/* Nothing to do! */
}


#if defined(CONFIG_ST40_STXH415)
#define PIOALT(port, pin, alt, dir)			\
do							\
{							\
	stxh415_pioalt_select((port), (pin), (alt));	\
	stxh415_pioalt_pad((port), (pin), (dir));	\
} while(0)
#elif defined(CONFIG_ST40_STXH416)
#define PIOALT(port, pin, alt, dir)			\
do							\
{							\
	stxh416_pioalt_select((port), (pin), (alt));	\
	stxh416_pioalt_pad((port), (pin), (dir));	\
} while(0)
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */


/*
 *	MII0: PIO106[2] = GMII0_notRESET
 *	MII0: PIO13[4]  = PIO6_CLKSEL (DVO_CK)
 *
 *	MII1: PIO4[7]   = GMII1_notRESET	(needs J39 fitted)
 *	MII1: PIO2[5]   = PIO6_CLKSEL (PIO_HDMI_TX_HPD)
 */
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* MII0, on CN22 */
#	define GMII_PHY_NOT_RESET		106, 2
#	define GMII_PHY_CLKOUT_NOT_TXCLK_SEL	13, 4
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* MII1, on CN23 */
#	define GMII_PHY_NOT_RESET		4, 7
#	define GMII_PHY_CLKOUT_NOT_TXCLK_SEL	2, 5
#endif


static void configPIO(void)
{
	/* Setup PIOs for ASC device */

#if defined(CONFIG_ST40_STXH415)
#if CONFIG_SYS_STM_ASC_BASE == STXH415_ASC2_BASE

	/* Route UART2 via PIO17 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(17, 4, 2, stm_pad_direction_output);	/* UART2-TX */
	PIOALT(17, 5, 2, stm_pad_direction_input);	/* UART2-RX */
	PIOALT(17, 7, 2, stm_pad_direction_output);	/* UART2-RTS */
	PIOALT(17, 6, 2, stm_pad_direction_input);	/* UART2-CTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH415_SBC_ASC0_BASE

	/* Route SBC_UART0 via PIO3 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(3, 4, 1, stm_pad_direction_output);	/* SBC_UART0-TX */
	PIOALT(3, 5, 1, stm_pad_direction_input);	/* SBC_UART0-RX */
	PIOALT(3, 7, 1, stm_pad_direction_output);	/* SBC_UART0-RTS */
	PIOALT(3, 6, 1, stm_pad_direction_input);	/* SBC_UART0-CTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH415_SBC_ASC1_BASE

	/* Route SBC_UART1 via PIO2,3 for TX, RX, CTS & RTS (Alternative #3) */
	PIOALT(2, 6, 3, stm_pad_direction_output);	/* SBC_UART1-TX */
	PIOALT(2, 7, 3, stm_pad_direction_input);	/* SBC_UART1-RX */
	PIOALT(3, 1, 3, stm_pad_direction_output);	/* SBC_UART1-RTS */
	PIOALT(3, 0, 3, stm_pad_direction_input);	/* SBC_UART1-CTS */

#else
#error Unknown ASC port selected!
#endif	/* CONFIG_SYS_STM_ASC_BASE == STXH415_ASCx_REGS_BASE */

#elif defined(CONFIG_ST40_STXH416)
#if CONFIG_SYS_STM_ASC_BASE == STXH416_ASC2_BASE

	/* Route UART2 via PIO17 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(17, 4, 2, stm_pad_direction_output);	/* UART2-TX */
	PIOALT(17, 5, 2, stm_pad_direction_input);	/* UART2-RX */
	PIOALT(17, 7, 2, stm_pad_direction_output);	/* UART2-RTS */
	PIOALT(17, 6, 2, stm_pad_direction_input);	/* UART2-CTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH416_SBC_ASC0_BASE

	/* Route SBC_UART0 via PIO3 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(3, 4, 1, stm_pad_direction_output);	/* SBC_UART0-TX */
	PIOALT(3, 5, 1, stm_pad_direction_input);	/* SBC_UART0-RX */
	PIOALT(3, 7, 1, stm_pad_direction_output);	/* SBC_UART0-RTS */
	PIOALT(3, 6, 1, stm_pad_direction_input);	/* SBC_UART0-CTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH416_SBC_ASC1_BASE

	/* Route SBC_UART1 via PIO2,3 for TX, RX, CTS & RTS (Alternative #3) */
	PIOALT(2, 6, 3, stm_pad_direction_output);	/* SBC_UART1-TX */
	PIOALT(2, 7, 3, stm_pad_direction_input);	/* SBC_UART1-RX */
	PIOALT(3, 1, 3, stm_pad_direction_output);	/* SBC_UART1-RTS */
	PIOALT(3, 0, 3, stm_pad_direction_input);	/* SBC_UART1-CTS */

#else
#error Unknown ASC port selected!
#endif	/* CONFIG_SYS_STM_ASC_BASE == STXH416_ASCx_REGS_BASE */
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */

#ifdef CONFIG_DRIVER_NET_STM_GMAC
	/*
	 * Configure the Ethernet PHY Reset signal
	 */
	SET_PIO_PIN2(GMII_PHY_NOT_RESET, STPIO_OUT);

	/*
	 * Configure the Ethernet PHY Mux PIO clock signal,
	 * as an output, which controls the speed of the MAC.
	 */
	SET_PIO_PIN2(GMII_PHY_CLKOUT_NOT_TXCLK_SEL, STPIO_OUT);
	STPIO_SET_PIN2(GMII_PHY_CLKOUT_NOT_TXCLK_SEL, 1);

#endif	/* CONFIG_DRIVER_NET_STM_GMAC */
}


#ifdef CONFIG_DRIVER_NET_STM_GMAC
extern void stmac_phy_reset(void)
{
	/*
	 * Reset the Ethernet PHY.
	 */
	STPIO_SET_PIN2(GMII_PHY_NOT_RESET, 0);
	udelay(10000);				/* 10 ms */
	STPIO_SET_PIN2(GMII_PHY_NOT_RESET, 1);
	udelay(10000);				/* 10 ms */
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */


#ifdef CONFIG_DRIVER_NET_STM_GMAC
extern void stmac_set_mac_speed(int speed)
{
	/* Manage the MAC speed */
	STPIO_SET_PIN2(GMII_PHY_CLKOUT_NOT_TXCLK_SEL,
		(speed==1000)?1:0);
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */


extern int board_init(void)
{
	configPIO();

#if 0	/* QQQ - TO IMPLEMENT */
#if defined(CONFIG_ST40_STM_SATA)
	stx7105_configure_sata ();
#endif	/* CONFIG_ST40_STM_SATA */
#endif	/* QQQ - TO IMPLEMENT */

/*
 * B2032A (MII) Ethernet card (*not* GMII)
 * On B2000B board, to get GMAC0 working make sure that jumper
 * on PIN 9-10 on CN35 and CN36 are removed.
 *
 *******************************************************************
 *
 * B2035A (RMII + MMC(on CN22)) Ethernet + MMC card
 * B2035A board has IP101ALF PHY connected in RMII mode
 * and an MMC card
 * It is designed to be connected to GMAC0 (CN22) to get MMC working,
 * however we can connect it to GMAC1 for RMII testing.
 *
 *******************************************************************
 *
 * Note: The following (default) configuration assumes we are using
 * the B2032 daughter board, in MII mode (not GMII). To use other
 * configurations, then please have a look in the STLinux kernel
 * distribution source trees for: arch/sh/boards/mach-b2000/setup.c
 */
#ifdef CONFIG_DRIVER_NET_STM_GMAC
#if CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE	/* MII0, on CN22 */
#	if defined(CONFIG_STMAC_IP1001)		/* IC+ IP1001 (B2032) */
#if defined(CONFIG_ST40_STXH415)
	stxh415_configure_ethernet(0, &(struct stxh415_ethernet_config) {
			.mode = stxh415_ethernet_mode_mii,
			.ext_clk = 1,
			.phy_bus = 0, });
#elif defined(CONFIG_ST40_STXH416)
	stxh416_configure_ethernet(0, &(struct stxh416_ethernet_config) {
			.mode = stxh416_ethernet_mode_mii,
			.ext_clk = 1,
			.phy_bus = 0, });
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */
#	elif defined(CONFIG_STMAC_IP101A)	/* IC+ IP101A (B2035) */
#if defined(CONFIG_ST40_STXH415)
	stxh415_configure_ethernet(0, &(struct stxh415_ethernet_config) {
			.mode = stxh415_ethernet_mode_rmii,
			.ext_clk = 0,
			.phy_bus = 0, });
#elif defined(CONFIG_ST40_STXH416)
	stxh416_configure_ethernet(0, &(struct stxh416_ethernet_config) {
			.mode = stxh416_ethernet_mode_rmii,
			.ext_clk = 0,
			.phy_bus = 0, });
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */
#	else
#	error Unknown PHY type associated with STM GMAC #0
#	endif	/* CONFIG_STMAC_IP1001 || CONFIG_STMAC_IP101A */
#elif CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC1_BASE	/* MII1, on CN23 */
#	if defined(CONFIG_STMAC_IP1001)		/* IC+ IP1001 (B2032) */
#if defined(CONFIG_ST40_STXH415)
	stxh415_configure_ethernet(1, &(struct stxh415_ethernet_config) {
			.mode = stxh415_ethernet_mode_mii,
			.ext_clk = 1,
			.phy_bus = 1, });
#elif defined(CONFIG_ST40_STXH416)
	stxh416_configure_ethernet(1, &(struct stxh416_ethernet_config) {
			.mode = stxh416_ethernet_mode_mii,
			.ext_clk = 1,
			.phy_bus = 1, });
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */
#	elif defined(CONFIG_STMAC_IP101A)	/* IC+ IP101A (B2035) */
#if defined(CONFIG_ST40_STXH415)
	stxh415_configure_ethernet(1, &(struct stxh415_ethernet_config) {
			.mode = stxh415_ethernet_mode_rmii,
			.ext_clk = 0,
			.phy_bus = 1, });
#elif defined(CONFIG_ST40_STXH416)
	stxh416_configure_ethernet(1, &(struct stxh416_ethernet_config) {
			.mode = stxh416_ethernet_mode_rmii,
			.ext_clk = 0,
			.phy_bus = 1, });
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */
#	else
#	error Unknown PHY type associated with STM GMAC #1
#	endif	/* CONFIG_STMAC_IP1001 || CONFIG_STMAC_IP101A */
#else
#error Unknown base address for the STM GMAC
#endif
	/* Hard Reset the PHY -- do after we have configured the MAC */
	stmac_phy_reset();
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

#if defined(CONFIG_CMD_I2C)
#if defined(CONFIG_ST40_STXH415)
	stxh415_configure_i2c();
#elif defined(CONFIG_ST40_STXH416)
	stxh416_configure_i2c();
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */
#endif	/* CONFIG_CMD_I2C */

	return 0;
}


int checkboard (void)
{
	printf ("\n\nBoard: B2000"
#if defined(CONFIG_ST40_STXH415)
		"-STxH415"
#elif defined(CONFIG_ST40_STXH416)
		"-STxH416"
#endif
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
#if defined(CONFIG_ST40_STXH415)
	stxh415_configure_spi();
#elif defined(CONFIG_ST40_STXH416)
	stxh416_configure_spi();
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */
#endif	/* CONFIG_SPI */

	return 0;
}


