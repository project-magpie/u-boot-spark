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

#ifdef CONFIG_SH_SE_MODE
#define EPLD_BASE		0xb4000000	/* Phys 0x04000000 */
#else
#define EPLD_BASE		0xa4000000
#endif	/* CONFIG_SH_SE_MODE */

static inline void epld_write(unsigned long value, unsigned long offset)
{
	/* 8-bit write to EPLD registers */
	writeb(value, EPLD_BASE + offset);
}

static inline unsigned long epld_read(unsigned long offset)
{
	/* 8-bit read from EPLD registers */
	return readb(EPLD_BASE + offset);
}

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

extern int board_init(void)
{
	configPIO();

	return 0;
}

#ifdef CONFIG_DRIVER_NET_STM_GMAC
static void mb618_phy_reset05(void)
{
	/* Bring the PHY out of reset in MII mode */
	epld_write(0x4 | 0, 0);
	epld_write(0x4 | 1, 0);
}
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

/*
 * We have several EPLD versions to cope with, with slightly different memory
 * maps and features:
 *
 * version 04:
 * off  read        reset
 *  0   Status      undef  (unused)
 *  4   Ctrl        20     (unused)
 *  8   Test        33
 *  c   Ident       0      (should be 1 but broken)
 * (note writes are broken)
 *
 * version 05:
 * off  read     write       reset
 *  0   Ident    Ctrl        45
 *  4   Test     Test        55
 *  8   IntStat  IntMaskSet  -
 *  c   IntMask  IntMaskClr  0
 */
static int mb618_init_epld(void)
{
	unsigned char epld_reg;
	int test_offset = -1;
	int version_offset = -1;
	int version = -1;

	epld_reg = epld_read(0x4);
	switch (epld_reg) {
	case 0x20:
		/*
		 * Probably the Ctrl reg of a 04 EPLD. Look for the default
		 * value in the test reg (we can't do a test as it is broken).
		 */
		epld_reg = epld_read(0x8);
		if (epld_reg == 0x33)
			version = 4;
		break;
	case 0x55:
		/* Probably the Test reg of the 05 or later EPLD */
		test_offset = 4;
		version_offset = 0;
		break;
	}

	if (test_offset > 0) {
		epld_write(0x63, test_offset);
		epld_reg = epld_read(test_offset);
		if (epld_reg != (unsigned char)(~0x63)) {
			printf("Failed mb618 EPLD test (off %02x, res %02x)\n",
			       test_offset, epld_reg);
			return 1;
		}

		/* Assume we can trust the version register */
		version = epld_read(version_offset) & 0xf;
	}

	if (version < 0) {
		printf("Unable to determine mb618 EPLD version\n");
		return 1;
	}

	printf("mb618 EPLD version %02d\n", version);

	switch (version) {
	case 5:
		/* We need to control the PHY reset in software */
#ifdef CONFIG_DRIVER_NET_STM_GMAC
		mb618_phy_reset05();
#endif
		break;
	}

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

	/*
	 * initialize the EPLD.
	 */
	return mb618_init_epld();
}
