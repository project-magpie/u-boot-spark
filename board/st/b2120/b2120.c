/*
 * (C) Copyright 2008-2014 STMicroelectronics.
 *
 * Sean McGoogan <Sean.McGoogan@st.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <stm/soc.h>
#include <stm/socregs.h>
#include <asm/io.h>
#include <stm/pio.h>
#include <stm/sysconf.h>


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


#define PIOALT(port, pin, alt, dir)			\
do							\
{							\
	stxh407_pioalt_select((port), (pin), (alt));	\
	stxh407_pioalt_pad((port), (pin), (dir));	\
} while(0)
#define SYSCONF(_reg)	((unsigned long*)STXH407_SYSCFG(_reg))


/*
 *	MII1: PIO0[7] = M_RGMII1_notRESET
 */
#define GMII_PHY_NOT_RESET	0, 7


extern int board_early_init_f(void)
{
	/* Setup PIOs for ASC device */

#if CONFIG_SYS_STM_ASC_BASE == STXH407_SBC_ASC0_BASE	/* a.k.a. UART #10 */

	/* Route SBC_UART0 via PIO3 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(3, 4, 1, stm_pad_direction_output);	/* SBC_UART0-TX */
	PIOALT(3, 5, 1, stm_pad_direction_input);	/* SBC_UART0-RX */
//	PIOALT(3, 6, 1, stm_pad_direction_input);	/* SBC_UART0-CTS */
//	PIOALT(3, 7, 1, stm_pad_direction_output);	/* SBC_UART0-RTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH407_SBC_ASC1_BASE	/* a.k.a. UART #11 */

	/* Route SBC_UART1 via PIO2,3 for TX, RX, CTS & RTS (Alternative #3) */
	PIOALT(2, 6, 3, stm_pad_direction_output);	/* SBC_UART1-TX */
	PIOALT(2, 7, 3, stm_pad_direction_input);	/* SBC_UART1-RX */
//	PIOALT(3, 0, 3, stm_pad_direction_input);	/* SBC_UART1-CTS */
//	PIOALT(3, 1, 3, stm_pad_direction_output);	/* SBC_UART1-RTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH407_ASC0_BASE	/* a.k.a. UART #0 */

	/* Route UART0 via PIO17 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(17, 0, 1, stm_pad_direction_output);	/* UART0-TX */
	PIOALT(17, 1, 1, stm_pad_direction_input);	/* UART0-RX */
//	PIOALT(17, 2, 1, stm_pad_direction_input);	/* UART0-CTS */
//	PIOALT(17, 3, 1, stm_pad_direction_output);	/* UART0-RTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH407_ASC1_BASE	/* a.k.a. UART #1 */

	/* Route UART1 via PIO16 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(16, 0, 1, stm_pad_direction_output);	/* UART1-TX */
	PIOALT(16, 1, 1, stm_pad_direction_input);	/* UART1-RX */
//	PIOALT(16, 2, 1, stm_pad_direction_input);	/* UART1-CTS */
//	PIOALT(16, 3, 1, stm_pad_direction_output);	/* UART1-RTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH407_ASC3_BASE	/* a.k.a. UART #3 */

	/* Route UART3 via PIO31 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(31, 3, 1, stm_pad_direction_output);	/* UART3-TX */
	PIOALT(31, 4, 1, stm_pad_direction_input);	/* UART3-RX */
//	PIOALT(31, 5, 1, stm_pad_direction_input);	/* UART3-CTS */
//	PIOALT(31, 6, 1, stm_pad_direction_output);	/* UART3-RTS */

#else
#error Unknown ASC port selected!
#endif	/* CONFIG_SYS_STM_ASC_BASE == STXH407_ASCx_REGS_BASE */

	return 0;
}


#ifdef CONFIG_DRIVER_NET_STM_GMAC
extern void stmac_phy_reset(void)
{
	/*
	 * Reset the Ethernet PHY/Switch.
	 */
	STPIO_SET_PIN2(GMII_PHY_NOT_RESET, 1);
	udelay(10000);				/* 10 ms */
	STPIO_SET_PIN2(GMII_PHY_NOT_RESET, 0);
	udelay(10000);				/* 10 ms */
	STPIO_SET_PIN2(GMII_PHY_NOT_RESET, 1);
	udelay(10000);				/* 10 ms */
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */


extern int board_init(void)
{
#ifdef CONFIG_DRIVER_NET_STM_GMAC
	/*
	 * Configure the Ethernet PHY/Switch Reset signal
	 */
	SET_PIO_PIN2(GMII_PHY_NOT_RESET, STPIO_OUT);
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

#if 0	/* QQQ - TO IMPLEMENT */
#if defined(CONFIG_STM_SATA)
	stx7105_configure_sata ();
#endif	/* CONFIG_STM_SATA */
#endif	/* QQQ - TO IMPLEMENT */

#ifdef CONFIG_DRIVER_NET_STM_GMAC
#if defined(CONFIG_STMAC_RTL8367RB)	/* Realtek RTL8367RB */
	stxh407_configure_ethernet(&(struct stxh407_ethernet_config) {
			.mode = stxh407_ethernet_mode_rgmii,
			.ext_clk = 0,
			.phy_bus = 1, });
#else
#error Unknown PHY type associated with STM GMAC #1
#endif	/* CONFIG_STMAC_RTL8367RB */
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

#if defined(CONFIG_CMD_I2C)
	stxh407_configure_i2c();
#endif	/* CONFIG_CMD_I2C */

#if defined(CONFIG_CMD_NAND)
	/*
	 * Configure the PIO signals (in the FlashSS) for NAND.
	 */
	stxh407_configure_nand();
#endif	/* CONFIG_CMD_NAND */

	return 0;
}


int checkboard (void)
{
#if defined(CONFIG_MACH_STM_STXH407_A9SS_VCORE_HACK)
	const unsigned long pwm1_1100 = STXH407_PWM1_VOLTS(1100);	/* value for 1.10V */
	const unsigned long pwm1 = readl(STXH407_SBC_PWM1_REG);		/* current value */

	/* S/W Workaround: Decrease Vcore for the CA9 cores, if Vcore > 1.10V */
	if (pwm1 < pwm1_1100)			/* greater than 1.10V ? */
	{
		writel(pwm1_1100, STXH407_SBC_PWM1_REG);	/* lower Vcore to 1.10V */
	}
#endif	/* CONFIG_MACH_STM_STXH407_A9SS_VCORE_HACK */

	printf("\n\nBoard: "
#if defined(CONFIG_STM_B2120)			/* B2120 ? */
	"B2120"
#elif defined(CONFIG_STM_B2089)			/* B2089 ? */
	"B2089"
#else
#	error Unknown BOARD Variant for B2120!
#endif	/* CONFIG_STM_B2120 */
#if defined(CONFIG_STM_STXH410)
	"-STxH410"
#elif defined(CONFIG_STM_STXH407)
	"-STxH407"
#endif	/* CONFIG_STM_STXH410 */
#if defined(CONFIG_ARM)
		"  [ARM]"
#endif
		"\n");

#if defined(CONFIG_SPI)
	/*
	 * Configure for the SPI Serial Flash.
	 * Note: for CONFIG_SYS_BOOT_FROM_SPI + CONFIG_ENV_IS_IN_EEPROM, this
	 * needs to be done after env_init(), hence it is done
	 * here, and not in board_init().
	 */
	stxh407_configure_spi();
#endif	/* CONFIG_SPI */

#if defined(CONFIG_DRIVER_NET_STM_GMAC)
	/* Hard Reset the PHY -- do after we have configured the MAC */
	stmac_phy_reset();
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

	return 0;
}