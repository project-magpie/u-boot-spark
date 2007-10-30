/*
 * (C) Copyright 2007 STMicroelectronics.
 *
 * Stuart Menefy <stuart.menefy@st.com>
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
#include <asm/sti7200reg.h>
#include <asm/io.h>
#include <asm/pio.h>

void stx7200_reset(void);

#ifdef CONFIG_SH_SE_MODE
#define EPLD_BASE		0xb5000000	/* Phys 0x05000000 */
#else
#define EPLD_BASE		0xa5000000
#endif	/* CONFIG_SH_SE_MODE */

#define EPLD_FlashCtrlAddr	SH4_WORD_REG(EPLD_BASE + 0x400000)

void flashWriteEnable(void)
{
	/*  Enable vpp for writing to flash */
	*EPLD_FlashCtrlAddr = 3;
}

void flashWriteDisable(void)
{
	/*  Disable vpp for writing to flash */
	*EPLD_FlashCtrlAddr = 2;
}

#define PIO_BASE  0xfd020000

static void configPIO(void)
{
  /*  Setup PIO of ASC device */
  SET_PIO_ASC(PIO_PORT(4), 3, 2, 4, 5);  /* UART2 - AS0 */
  SET_PIO_ASC(PIO_PORT(5), 4, 3, 5, 6);  /* UART3 - AS1 */
}

int board_init(void)
{
	unsigned long sysconf;

	/* Serial port set up */
	/* Route UART2&3 or SCI inputs instead of DVP to pins: conf_pad_dvp = 0 */
	sysconf = *STI7200_SYSCONF_SYS_CFG40;
	sysconf &= ~(1<<16);
	*STI7200_SYSCONF_SYS_CFG40 = sysconf;

	/* Route UART2&3/SCI outputs instead of DVP to pins: conf_pad_pio[1]=0 */
	sysconf = *STI7200_SYSCONF_SYS_CFG07;
	sysconf &= ~(1<<25);
	*STI7200_SYSCONF_SYS_CFG07 = sysconf;

	/* No idea, more routing: conf_pad_pio[0] = 0 */
	sysconf = *STI7200_SYSCONF_SYS_CFG07;
	sysconf &= ~(1<<24);
	*STI7200_SYSCONF_SYS_CFG07 = sysconf;

	/* Route UART2 (inputs and outputs) instead of SCI to pins: ssc2_mux_sel = 0 */
	sysconf = *STI7200_SYSCONF_SYS_CFG07;
	sysconf &= ~(1<<2);
	*STI7200_SYSCONF_SYS_CFG07 = sysconf;

	/* conf_pad_pio[4] = 0 */
	sysconf = *STI7200_SYSCONF_SYS_CFG07;
	sysconf &= ~(1<<28);
	*STI7200_SYSCONF_SYS_CFG07 = sysconf;

	/* Route UART3 (inputs and outputs) instead of SCI to pins: ssc3_mux_sel = 0 */
	sysconf = *STI7200_SYSCONF_SYS_CFG07;
	sysconf &= ~(1<<3);
	*STI7200_SYSCONF_SYS_CFG07 = sysconf;

	/* conf_pad_clkobs = 1 */
	sysconf = *STI7200_SYSCONF_SYS_CFG07;
	sysconf |= (1<<14);
	*STI7200_SYSCONF_SYS_CFG07 = sysconf;

	/* I2C and USB related routing */
	/* bit4: ssc4_mux_sel = 0 (treat SSC4 as I2C) */
	/* bit26: conf_pad_pio[2] = 0 route USB etc instead of DVO */
	/* bit27: conf_pad_pio[3] = 0 DVO output selection (probably ignored) */
	sysconf = *STI7200_SYSCONF_SYS_CFG07;
	sysconf &= ~((1<<27)|(1<<26)|(1<<4));
	*STI7200_SYSCONF_SYS_CFG07 = sysconf;

	/* Enable SOFT_JTAG mode.
	 * Taken from OS21, but is this correct?
	 */
	sysconf = *STI7200_SYSCONF_SYS_CFG33;
	sysconf |= (1<<6);
	sysconf &= ~((1<<0)|(1<<1)|(1<<2)|(1<<3));
	*STI7200_SYSCONF_SYS_CFG33 = sysconf;

	/* Route Ethernet pins to output */
	/* bit26-16: conf_pad_eth(10:0) */
	sysconf = *STI7200_SYSCONF_SYS_CFG41;
	/* MII0: conf_pad_eth(0) = 0 (ethernet) */
	sysconf &= ~(1<<16);
	/* MII1: conf_pad_eth(2) = 0, (3)=0, (4)=0, (9)=0, (10)=0 (ethernet)
	 * MII1: conf_pad_eth(6) = 0 (MII1TXD[0] = output) */
	sysconf &= ~( (1<<(16+2)) | (1<<(16+3)) | (1<<(16+4)) | (1<<(16+6)) |
		      (1<<(16+9)) | (1<<(16+10)));
	*STI7200_SYSCONF_SYS_CFG41 = sysconf;

	configPIO();

	return 0;
}

int checkboard (void)
{
	printf ("\n\nBoard: mb519\n");

	return 0;
}

int do_reset (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	stx7200_reset();
	/*NOTREACHED*/ return (0);
}
