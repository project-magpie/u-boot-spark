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
#include <stm/soc.h>
#include <stm/socregs.h>
#include <asm/io.h>
#include <stm/pio.h>
#include <stm/sysconf.h>
#include <stm/stm-sdhci.h>
#include <mmc.h>


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

#if CONFIG_SYS_STM_ASC_BASE == STXH407_SBC_ASC0_BASE

	/* Route SBC_UART0 via PIO3 for TX, RX, CTS & RTS (Alternative #1) */
	PIOALT(3, 4, 1, stm_pad_direction_output);	/* SBC_UART0-TX */
	PIOALT(3, 5, 1, stm_pad_direction_input);	/* SBC_UART0-RX */
//	PIOALT(3, 7, 1, stm_pad_direction_output);	/* SBC_UART0-RTS */
//	PIOALT(3, 6, 1, stm_pad_direction_input);	/* SBC_UART0-CTS */

#elif CONFIG_SYS_STM_ASC_BASE == STXH407_SBC_ASC1_BASE

	/* Route SBC_UART1 via PIO2,3 for TX, RX, CTS & RTS (Alternative #3) */
	PIOALT(2, 6, 3, stm_pad_direction_output);	/* SBC_UART1-TX */
	PIOALT(2, 7, 3, stm_pad_direction_input);	/* SBC_UART1-RX */
//	PIOALT(3, 1, 3, stm_pad_direction_output);	/* SBC_UART1-RTS */
//	PIOALT(3, 0, 3, stm_pad_direction_input);	/* SBC_UART1-CTS */

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


#ifdef CONFIG_GENERIC_MMC
/**
 * stxh407_mmc_core_config: configure the Arasan HC
 * @ioaddr: base address
 * Description: this function is to configure the arasan MMC HC.
 * This should be called when the system starts in case of, on the SoC,
 * it is needed to configure the host controller.
 * This happens on some SoCs, i.e. StiH407, where the MMC0 inside the flashSS
 * needs to be configured as MMC 4.5 to have full capabilities.
 * W/o these settings the SDHCI could configure and use the embedded controller
 * with limited features.
 */
extern void stxh407_mmc_core_config(int port, u32 regbase)
{
#ifdef MMC_CORE_DEBUG
	printf("mmc%d: mmc%d core at reset ...\n", port, port);
	printf("cfg1 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_1));
	printf("cfg2 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_2));
	printf("cfg3 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_3));
	printf("cfg4 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_4));
	printf("cfg5 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_5));
	printf("cfg6 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_6));
	printf("cfg7 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_7));
	printf("cfg8 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_8));
#endif /*MMC_CORE_DEBUG*/

	writel(STM_FLASHSS_MMC_CORE_CONFIG_1,
	       regbase + FLASHSS_MMC_CORE_CONFIG_1);
	(port)? writel(STM_FLASHSS_SD20_CORE_CONFIG2,
	       		regbase + FLASHSS_MMC_CORE_CONFIG_2):/*STM_FLASHSS_SD20_CORE_CONFIG3*/
		writel(STM_FLASHSS_MMC43_CORE_CONFIG2,
	       		regbase + FLASHSS_MMC_CORE_CONFIG_2);

	(port)? writel(STM_FLASHSS_SD20_CORE_CONFIG3,
	        	regbase + FLASHSS_MMC_CORE_CONFIG_3):/*STM_FLASHSS_SD20_CORE_CONFIG3*/
	       	writel(STM_FLASHSS_MMC43_CORE_CONFIG3,
	       		regbase + FLASHSS_MMC_CORE_CONFIG_3); 
	writel(STM_FLASHSS_MMC43_CORE_CONFIG4,
	       regbase + FLASHSS_MMC_CORE_CONFIG_4);
	writel(STM_FLASHSS_MMC43_CORE_CONFIG5,
	       regbase + FLASHSS_MMC_CORE_CONFIG_5);
	writel(FLASHSS_MMC_CORE_CONFIG_6,
	       regbase + FLASHSS_MMC_CORE_CONFIG_6);
	writel(FLASHSS_MMC_CORE_CONFIG_7,
	       regbase + FLASHSS_MMC_CORE_CONFIG_7);

#ifdef MMC_CORE_DEBUG
	printf("mmc%d: mmc%d core set SD2.0 ...\n", port, port);
	printf("cfg1 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_1));
	printf("cfg2 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_2));
	printf("cfg3 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_3));
	printf("cfg4 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_4));
	printf("cfg5 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_5));
	printf("cfg6 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_6));
	printf("cfg7 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_7));
	printf("cfg8 0x%x\n", readl(regbase + FLASHSS_MMC_CORE_CONFIG_8));
#endif /*MMC_CORE_DEBUG*/
}		

struct stm_pad_pin stxh407_mmc0_pad_configs[] = {
					MMC_DATA_IN_PU(41, 0, 1, NULL),/* DATA[0] */
					MMC_DATA_IN_PU(41, 1, 1, NULL),/* DATA[1] */
					MMC_DATA_IN_PU(41, 2, 1, NULL),/* DATA[2] */
					MMC_DATA_IN_PU(41, 3, 1, NULL),/* DATA[3] */
					MMC_DATA_IN_PU(41, 4, 1, NULL),/* DATA[4] */
					MMC_DATA_IN_PU(41, 5, 1, NULL),/* DATA[5] */
					MMC_DATA_IN_PU(41, 6, 1, NULL),/* DATA[6] */
					MMC_DATA_IN_PU(41, 7, 1, NULL),/* DATA[7] */
					MMC_DATA_IN_PU(40, 7, 1, NULL),/* CMD */
					MMC_CLOCK_OUT(40, 6, 1, NULL),/*Clock*/
					//MMC_OUT(42,3,2),/*V_SEL for SD config*/
};

struct stm_pad_pin stxh407_mmc1_pad_configs[] = {
					MMC_DATA_IN_PU(19, 4, 5, RET_BYPASS(15)),/* DATA[0] */
					MMC_DATA_IN_PU(19, 5, 5, RET_BYPASS(15)),/* DATA[1] */
					MMC_DATA_IN_PU(19, 6, 5, RET_BYPASS(15)),/* DATA[2] */
					MMC_DATA_IN_PU(19, 7, 5, RET_BYPASS(15)),/* DATA[3] */
					MMC_DATA_IN_PU(19, 2, 5, RET_BYPASS(15)),/* CMD */
					MMC_CLOCK_OUT(19, 3, 5, RET_NICLK2(15,1)),/*Clock*/
					MMC_OUT(16, 7, 6),/*MMC Card PWR*/
					MMC_OUT(16, 6, 6),/*MMC LED ON*/
					MMC_DATA_IN_PU(19, 0, 6, NULL),/*MMC Card Detect*/
					MMC_IN(19, 1, 6),/*MMC Write Protect*/
};

int board_mmc_init(bd_t *bis)
{
   	int ret=0;
	
#if defined(CONFIG_STM_SDHCI)
#if defined(CONFIG_STM_SDHCI_0)
   	stxh407_configure_mmc(0,CONFIG_SYS_MMC0_BASE);
   	ret = stxh407_mmc_init(0);
#endif
#if defined (CONFIG_STM_SDHCI_1) 
 	stxh407_configure_mmc(1,CONFIG_SYS_MMC1_BASE);
 	ret |= stxh407_mmc_init(1);
#endif  
#endif  
   	return ret;
}

int board_mmc_getcd(struct mmc *mmc)
{
	if (strcmp(mmc->name,"stm-sdhci0")==0)
	{
	return 1;
	}
	if (strcmp(mmc->name,"stm-sdhci1")==0)
	{
	return (stxh407_mmc_getcd(1)==0)?1:0;	
	}
	return 0;
}
#endif /*CONFIG_GENERIC_MMC*/


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

	printf ("\n\nBoard: B2120-STxH407"
#if defined(CONFIG_ARM)
		"  [ARM]"
#endif
		"\n");

#if defined(CONFIG_SOFT_SPI)
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
