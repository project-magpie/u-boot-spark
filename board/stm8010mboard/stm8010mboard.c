/*
 * (C) Copyright 2004 STMicroelectronics.
 *
 * Andy Sturges <andy.sturges@st.com>
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
#include <asm/io.h>
#include <asm/pio.h>


#define EPLD_REG 0x00C00000

void flashWriteEnable(void)
{
  uchar tmp;
  tmp=readb(EPLD_REG);
  tmp|=1;
  writeb(tmp, EPLD_REG);
  ;
}

void flashWriteDisable(void)
{
  uchar tmp;
  tmp=readb(EPLD_REG);
  tmp &= ~1;
  writeb(tmp, EPLD_REG);
}

#define PIO_BASE       0x540A0000
#define SYSCONFIG_BASE 0x50003000

#define SYSCONFIG2_DATA_OUT     0x18

int stsysconfig_set(int regno,unsigned mask,unsigned value)
{
	unsigned int shift_val;
	unsigned int tmp;
	unsigned int p;

	shift_val=value<<(ffs(mask)-1);

	p=SYSCONFIG_BASE+regno;
	tmp=readl(p);
	tmp=(tmp & ~mask)|shift_val;
	writel(tmp,p);

	return 0;
}

static void configPIO(void)
{
  SET_PIO_PIN(PIO_PORT(1), 3, STPIO_ALT_OUT); /* TX */
  SET_PIO_PIN(PIO_PORT(1), 4, STPIO_IN);      /* RX */
  SET_PIO_PIN(PIO_PORT(2), 5, STPIO_IN);      /* CTS */
  SET_PIO_PIN(PIO_PORT(2), 4, STPIO_ALT_OUT); /* RTS */

  /* Set ASC PIO's to alternate function */
  /* RTS */
  stsysconfig_set(SYSCONFIG_REG_MASK(SYSCONFIG2_DATA_OUT, 3:3),1);
  /* TxD */
  stsysconfig_set(SYSCONFIG_REG_MASK(SYSCONFIG2_DATA_OUT, 43:43),1);

  /*  ATAPI interface */
  SET_PIO_PIN(PIO_PORT(0),0, STPIO_ALT_OUT);
  SET_PIO_PIN(PIO_PORT(0),1, STPIO_ALT_OUT);
}

int checkboard (void)
{
	printf ("\n\nBoard: stm8010-Mboard");
	printf ("CPU Speed %d MHz\n", CPU_CLOCK_RATE / 1000000);
	return 0;
}

int board_init(void)
{
	configPIO();
	flashWriteEnable();
	return 0;
}

extern void stm8010_reset(void);

int do_reset (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	stm8010_reset();
	/*NOTREACHED*/
	return (0);
}
