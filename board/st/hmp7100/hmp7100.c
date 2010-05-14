/*
 * (C) Copyright 2004,2010 STMicroelectronics.
 *
 * Andy Sturges <andy.sturges@st.com>
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
#include <asm/stb7100reg.h>
#include <asm/io.h>
#include <asm/pio.h>

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

#define PIO_BASE  0xb8020000	/* Phys 0x18020000 */

static void configPIO(void)
{
  /*  Setup PIO of ASC device */
  SET_PIO_ASC(PIO_PORT(4), 3, 2, 4, 5);  /* UART2 - AS0 */

  /*  Setup up ethernet reset */
#if defined(CONFIG_SMC911X_BASE) && 0	/* QQQ */
  SET_PIO_PIN(PIO_PORT(2), 7, STPIO_OUT);
#endif
#if defined(CONFIG_DRIVER_NETSTMAC)
  SET_PIO_PIN(PIO_PORT(5), 3, STPIO_OUT);
#endif
}

extern int board_init(void)
{
	unsigned long sysconf;
	/* Route UART2 instead of SCI to PIO4 */
	/* Set ssc2_mux_sel = 0 */
	sysconf = *STB7100_SYSCONF_SYS_CFG07;
	sysconf &= ~(1<<3);
	*STB7100_SYSCONF_SYS_CFG07 = sysconf;

	configPIO();

#if defined(CONFIG_SMC911X_BASE) && 0	/* QQQ */
	/*  Reset ethernet chip */
	STPIO_SET_PIN(PIO_PORT(2), 7, 0);
	udelay(1000);
	STPIO_SET_PIN(PIO_PORT(2), 7, 1);
	udelay(1000);
	STPIO_SET_PIN(PIO_PORT(2), 7, 0);
#endif

#if defined(CONFIG_DRIVER_NETSTMAC)
	/*  Reset ethernet chip */
	STPIO_SET_PIN(PIO_PORT(5), 3, 1);
	udelay(1000);
	STPIO_SET_PIN(PIO_PORT(5), 3, 0);
	udelay(2000);
	STPIO_SET_PIN(PIO_PORT(5), 3, 1);
#endif

#if defined(CONFIG_CMD_IDE)
#ifdef CONFIG_SH_STB7100_SATA
	stb7100_sata_init();
#endif
#endif

	return 0;
}

extern int checkboard (void)
{
	printf ("\n\nBoard: HMP7100"
#ifdef CONFIG_SH_SE_MODE
		"  [32-bit mode]"
#else
		"  [29-bit mode]"
#endif
		"\n");

	return 0;
}
