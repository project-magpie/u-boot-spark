/*
 * (C) Copyright 2014 STMicroelectronics.
 *
 * Imran Khan <Imran.Khan@st.com>
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
	stxh301_pioalt_select((port), (pin), (alt));	\
	stxh301_pioalt_pad((port), (pin), (dir));	\
} while(0)
#define SYSCONF(_reg)	((unsigned long*)STXH301_SYSCFG(_reg))


/*
 * Define Phy reset PIO
 */
#define GMII_PHY_NOT_RESET 0, 7

/*
 * Define spi write protection PIO
 */ 
#define SPI_WR_PROTECT 12, 7

extern int board_early_init_f(void)
{
	/* Setup PIOs for ASC device */

#if CONFIG_SYS_STM_ASC_BASE == STXH301_SBC_ASC0_BASE	/* a.k.a. UART #10 */

	/* Route SBC_UART0 via PIO3 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(3, 4, 1, stm_pad_direction_output);	/* SBC_UART0-TX */
	PIOALT(3, 5, 1, stm_pad_direction_input);	/* SBC_UART0-RX */

#elif CONFIG_SYS_STM_ASC_BASE == STXH301_SBC_ASC1_BASE	/* a.k.a. UART #11 */

	/* Route SBC_UART1 via PIO2,3 for TX, RX, CTS & RTS (Alternative #3) */
	PIOALT(2, 6, 3, stm_pad_direction_output);	/* SBC_UART1-TX */
	PIOALT(2, 7, 3, stm_pad_direction_input);	/* SBC_UART1-RX */

#elif CONFIG_SYS_STM_ASC_BASE == STXH301_ASC0_BASE	/* a.k.a. UART #0 */

	/* Route UART0 via PIO17 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(17, 0, 1, stm_pad_direction_output);	/* UART0-TX */
	PIOALT(17, 1, 1, stm_pad_direction_input);	/* UART0-RX */
//	PIOALT(17, 2, 1, stm_pad_direction_input);	/* UART0-CTS */
//	PIOALT(17, 3, 1, stm_pad_direction_output);	/* UART0-RTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH301_ASC1_BASE	/* a.k.a. UART #1 */

	/* Route UART1 via PIO16 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(16, 0, 1, stm_pad_direction_output);	/* UART1-TX */
	PIOALT(16, 1, 1, stm_pad_direction_input);	/* UART1-RX */

#elif CONFIG_SYS_STM_ASC_BASE == STXH301_ASC2_BASE

	/* Route UART2 via PIO17 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(17, 4, 2, stm_pad_direction_output);	/* UART2-TX */
	PIOALT(17, 5, 2, stm_pad_direction_input);	/* UART2-RX */
#elif CONFIG_SYS_STM_ASC_BASE == STXH301_ASC3_BASE	/* a.k.a. UART #3 */

	/* Route UART3 via PIO31 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(31, 3, 1, stm_pad_direction_output);	/* UART3-TX */
	PIOALT(31, 4, 1, stm_pad_direction_input);	/* UART3-RX */
//	PIOALT(31, 5, 1, stm_pad_direction_input);	/* UART3-CTS */
//	PIOALT(31, 6, 1, stm_pad_direction_output);	/* UART3-RTS */

#else
#error Unknown ASC port selected!
#endif	/* CONFIG_SYS_STM_ASC_BASE == STXH301_ASCx_REGS_BASE */

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
#if defined(CONFIG_STMAC_RTL8211E)	/* Realtek RTL8211E */
	stxh301_configure_ethernet(&(struct stxh301_ethernet_config) {
			.mode = stxh301_ethernet_mode_rgmii,
			.ext_clk = 0,
			.phy_bus = 1, });
#else
#error Unknown PHY type associated with STM GMAC #1
#endif	/* CONFIG_STMAC_RTL8367RB */
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

#if defined(CONFIG_CMD_I2C)
	stxh301_configure_i2c();
#endif	/* CONFIG_CMD_I2C */

#if defined(CONFIG_CMD_NAND)
	/*
	 * Configure the PIO signals (in the FlashSS) for NAND.
	 */
	stxh301_configure_nand();
#endif	/* CONFIG_CMD_NAND */

	return 0;
}


int checkboard (void)
{
#if defined(CONFIG_MACH_STM_STXH301_A9SS_VCORE_HACK)
	const unsigned long pwm1_1100 = STXH301_PWM1_VOLTS(1100);	/* value for 1.10V */
	const unsigned long pwm1 = readl(STXH301_SBC_PWM1_REG);		/* current value */

	/* S/W Workaround: Decrease Vcore for the CA9 cores, if Vcore > 1.10V */
	if (pwm1 < pwm1_1100)			/* greater than 1.10V ? */
	{
		writel(pwm1_1100, STXH301_SBC_PWM1_REG);	/* lower Vcore to 1.10V */
	}
#endif	/* CONFIG_MACH_STM_STXH301_A9SS_VCORE_HACK */

	printf("\n\nBoard: "
#if defined(CONFIG_STM_B2173)			/* B2173 ? */
	"B2173"
#elif defined(CONFIG_STM_B2169)			/* B2169 ? */
	"B2169"
#else
#	error Unknown BOARD Variant for B2173!
#endif	/* CONFIG_STM_B2173 */
#if defined(CONFIG_STM_STXH301)
	"-STxH301"
#elif defined(CONFIG_STM_STXH303)
	"-STxH303"
#endif	/* CONFIG_STM_STXH303 */
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

	/* SPI Write protection */
	SET_PIO_PIN2(SPI_WR_PROTECT, STPIO_OUT);
	STPIO_SET_PIN2(SPI_WR_PROTECT, 0);
	stxh301_configure_spi();
#if defined(CONFIG_STM_FSM_SPI_FLASH)
	stxh301_configure_spifsm();
#endif
#endif	/* CONFIG_SPI */

#if defined(CONFIG_DRIVER_NET_STM_GMAC)
	/* Hard Reset the PHY -- do after we have configured the MAC */
	stmac_phy_reset();
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

	return 0;
}

#if defined(CONFIG_DRIVER_NET_STM_GMAC)
extern void stmac_set_mac_speed(int speed)
{
	unsigned int val;
	/*
	 * Manage the MAC speed
	 */
	val=readl(0x09103310);
	val=val&0xFE000000;
	if (speed==1000)
	{
	    /* 125Mhz */
	    val=val|0x01251eb8;
  
	}
	else
	{
	    /* 25Mhz */
	    val=val|0x014a6666;
	}
	writel(val,0x09103310);
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

