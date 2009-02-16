/*
 * (C) Copyright 2008-2009 STMicroelectronics.
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
#include <asm/stx7105reg.h>
#include <asm/io.h>
#include <asm/pio.h>


#define PIO_BASE  0xfd020000	/* Base of PIO block in COMMs block */


void flashWriteEnable(void)
{
	/* Enable Vpp for writing to flash */
	/* FLASH_WP# = PIO6[4] = 1 */
	STPIO_SET_PIN(PIO_PORT(6), 4, 1);
}

void flashWriteDisable(void)
{
	/* Disable Vpp for writing to flash */
	/* FLASH_WP# = PIO6[4] = 0 */
	STPIO_SET_PIN(PIO_PORT(6), 4, 0);
}

static void configEthernet(void)
{
	unsigned long sysconf;

	/* Configure & Reset the Ethernet PHY */

	/* Set the GMAC in MII mode */
	sysconf = *STX7105_SYSCONF_SYS_CFG07;
	sysconf &= ~0x060f0000ul;
	sysconf |=  0x08010000ul;
	*STX7105_SYSCONF_SYS_CFG07 = sysconf;

	sysconf = *STX7105_SYSCONF_SYS_CFG37;
	/* PIO7[4] CFG37[12,4]  AltFunction = 1 */
	/* PIO7[5] CFG37[13,5]  AltFunction = 1 */
	/* PIO7[6] CFG37[14,6]  AltFunction = 1 */
	/* PIO7[7] CFG37[15,7]  AltFunction = 1 */
	sysconf &= ~0xf0f0ul;	/* 3,3,3,3,0,0,0,0 */
	*STX7105_SYSCONF_SYS_CFG37 = sysconf;

	sysconf = *STX7105_SYSCONF_SYS_CFG46;
	/* PIO8[0] CFG46[8,0]   AltFunction = 1 */
	/* PIO8[1] CFG46[9,1]   AltFunction = 1 */
	/* PIO8[2] CFG46[10,2]  AltFunction = 1 */
	/* PIO8[3] CFG46[11,3]  AltFunction = 1 */
	/* PIO8[4] CFG46[12,4]  AltFunction = 1 */
	/* PIO8[5] CFG46[13,5]  AltFunction = 1 */
	/* PIO8[6] CFG46[14,6]  AltFunction = 1 */
	/* PIO8[7] CFG46[15,7]  AltFunction = 1 */
	sysconf &= ~0xfffful;	/* 3,3,3,3,3,3,3,3 */
	*STX7105_SYSCONF_SYS_CFG46 = sysconf;

	sysconf = *STX7105_SYSCONF_SYS_CFG47;
	/* PIO9[0] CFG47[8,0]   AltFunction = 1 */
	/* PIO9[1] CFG47[9,1]   AltFunction = 1 */
	/* PIO9[2] CFG47[10,2]  AltFunction = 1 */
	/* PIO9[3] CFG47[11,3]  AltFunction = 1 */
	/* PIO9[4] CFG47[12,4]  AltFunction = 1 */
	/* PIO9[5] CFG47[13,5]  AltFunction = 1 */
	/* PIO9[6] CFG47[14,6]  AltFunction = 1 */
	sysconf &= ~0x7f7ful;	/* 0,3,3,3,3,3,3,3 */
	*STX7105_SYSCONF_SYS_CFG47 = sysconf;

	/* Setup PIO for the Ethernet's MII bus */
	SET_PIO_PIN(PIO_PORT(7),4,STPIO_IN);
	SET_PIO_PIN(PIO_PORT(7),5,STPIO_IN);
	SET_PIO_PIN(PIO_PORT(7),6,STPIO_ALT_OUT);
	SET_PIO_PIN(PIO_PORT(7),7,STPIO_ALT_OUT);
	SET_PIO_PIN(PIO_PORT(8),0,STPIO_ALT_OUT);
	SET_PIO_PIN(PIO_PORT(8),1,STPIO_ALT_OUT);
	SET_PIO_PIN(PIO_PORT(8),2,STPIO_ALT_OUT);
	SET_PIO_PIN(PIO_PORT(8),3,STPIO_ALT_BIDIR);
	SET_PIO_PIN(PIO_PORT(8),4,STPIO_ALT_OUT);
	SET_PIO_PIN(PIO_PORT(8),5,STPIO_IN);
	SET_PIO_PIN(PIO_PORT(8),6,STPIO_IN);
	SET_PIO_PIN(PIO_PORT(8),7,STPIO_IN);
	SET_PIO_PIN(PIO_PORT(9),0,STPIO_IN);
	SET_PIO_PIN(PIO_PORT(9),1,STPIO_IN);
	SET_PIO_PIN(PIO_PORT(9),2,STPIO_IN);
	SET_PIO_PIN(PIO_PORT(9),3,STPIO_IN);
	SET_PIO_PIN(PIO_PORT(9),4,STPIO_IN);
	SET_PIO_PIN(PIO_PORT(9),5,STPIO_ALT_OUT);
	SET_PIO_PIN(PIO_PORT(9),6,STPIO_IN);

	/* Setup PIO for the PHY's reset */
	SET_PIO_PIN(PIO_PORT(15), 5, STPIO_OUT);

	/* Finally, toggle the PHY Reset pin ("RST#") */
	STPIO_SET_PIN(PIO_PORT(15), 5, 0);
	udelay(100);	/* small delay */
	STPIO_SET_PIN(PIO_PORT(15), 5, 1);
}

static void configPIO(void)
{
	unsigned long sysconf;

	/* Setup PIO of ASC device */
	SET_PIO_ASC(PIO_PORT(4), 0, 1, 2, 3);  /* UART2 - AS0 */
	SET_PIO_ASC(PIO_PORT(5), 0, 1, 3, 2);  /* UART3 - AS1 */

	/* Setup PIO for FLASH_WP# (Active-Low WriteProtect) */
	SET_PIO_PIN(PIO_PORT(6), 4, STPIO_OUT);

	/* Select UART2 via PIO4 */
	sysconf = *STX7105_SYSCONF_SYS_CFG07;
	/* CFG07[1] = UART2_RXD_SRC_SELECT = 0 */
	/* CFG07[2] = UART2_CTS_SRC_SELECT = 0 */
	sysconf &= ~(1ul<<2 | 1ul<<1);
	*STX7105_SYSCONF_SYS_CFG07 = sysconf;

	/* Route UART2 via PIO4 for TX, RX, CTS & RTS */
	sysconf = *STX7105_SYSCONF_SYS_CFG34;
	/* PIO4[0] CFG34[8,0]   AltFunction = 3 */
	/* PIO4[1] CFG34[9,1]   AltFunction = 3 */
	/* PIO4[2] CFG34[10,2]  AltFunction = 3 */
	/* PIO4[3] CFG34[11,3]  AltFunction = 3 */
	sysconf &= ~0x0f0ful;	/* 3,3,3,3 */
	sysconf |=  0x0f00ul;	/* 2,2,2,2 */
	*STX7105_SYSCONF_SYS_CFG34 = sysconf;

	/* Route UART3 via PIO5 for TX, RX, CTS & RTS */
	sysconf = *STX7105_SYSCONF_SYS_CFG35;
	/* PIO5[0] CFG35[8,0]   AltFunction = 3 */
	/* PIO5[1] CFG35[9,1]   AltFunction = 3 */
	/* PIO5[2] CFG35[10,2]  AltFunction = 3 */
	/* PIO5[3] CFG35[11,3]  AltFunction = 3 */
	sysconf &= ~0x0f0ful;	/* 3,3,3,3 */
	sysconf |=  0x000ful;	/* 1,1,1,1 */
	*STX7105_SYSCONF_SYS_CFG35 = sysconf;

	/* Configure & Reset the Ethernet PHY */
	configEthernet();
}

extern int board_init(void)
{
	configPIO();

	return 0;
}

int checkboard (void)
{
	printf ("\n\nBoard: STx7105-PDK"
#ifdef CONFIG_SH_SE_MODE
		"  [32-bit mode]"
#else
		"  [29-bit mode]"
#endif
		"\n");

	return 0;
}
