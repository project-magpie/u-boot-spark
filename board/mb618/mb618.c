/*
 * (C) Copyright 2008 STMicroelectronics.
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
#include <asm/stx7111reg.h>
#include <asm/io.h>
#include <asm/pio.h>

#define PIO_BASE  0xfd020000	/* Base of PIO block in COMMs block */

void flashWriteEnable(void)
{
	/* Enable Vpp for writing to flash */
	STPIO_SET_PIN(PIO_PORT(3), 4, 1);
}

void flashWriteDisable(void)
{
	/* Disable Vpp for writing to flash */
	STPIO_SET_PIN(PIO_PORT(3), 4, 0);
}

static void configPIO(void)
{
	/* Setup PIO of ASC device */
	SET_PIO_ASC(PIO_PORT(4), 3, 2, 4, 5);  /* UART2 - AS0 */
	SET_PIO_ASC(PIO_PORT(5), 0, 1, 2, 3);  /* UART3 - AS1 */

	/* Setup up Vpp for NOR FLASH */
	SET_PIO_PIN(PIO_PORT(3), 4, STPIO_OUT);
}

int board_init(void)
{
#ifdef QQQ	/* QQQ - DELETE */
	unsigned long sysconf;

	/* I2C and USB related routing */
	/* bit4: ssc4_mux_sel = 0 (treat SSC4 as I2C) */
	/* bit26: conf_pad_pio[2] = 0 route USB etc instead of DVO */
	/* bit27: conf_pad_pio[3] = 0 DVO output selection (probably ignored) */
	sysconf = *STX7111_SYSCONF_SYS_CFG07;
	sysconf &= ~((1<<27)|(1<<26)|(1<<4));
	*STX7111_SYSCONF_SYS_CFG07 = sysconf;

	/* Enable SOFT_JTAG mode.
	 * Taken from OS21, but is this correct?
	 */
	sysconf = *STX7111_SYSCONF_SYS_CFG33;
	sysconf |= (1<<6);
	sysconf &= ~((1<<0)|(1<<1)|(1<<2)|(1<<3));
	*STX7111_SYSCONF_SYS_CFG33 = sysconf;
#endif		/* QQQ - DELETE */

	configPIO();

	return 0;
}

int checkboard (void)
{
	printf ("\n\nBoard: STx7111-Mboard (MB618)"
#ifdef CONFIG_SH_SE_MODE
		"  [32-bit mode]"
#else
		"  [29-bit mode]"
#endif
		"\n");
	return 0;
}
