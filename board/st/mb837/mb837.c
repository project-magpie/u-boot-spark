/*
 * (C) Copyright 2008-2010 STMicroelectronics.
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
#include <asm/stx7108reg.h>
#include <asm/io.h>
#include <asm/pio.h>


#if defined(CONFIG_SH_MB705)
	/*
	 * More recent EPLD versions have the EPLD in EMI space,
	 * using CSCn (EMI Bank #2), nominally at physical 0x04800000.
	 */
#if !defined(CFG_EPLD_PHYSICAL_BASE)
#	define CFG_EPLD_PHYSICAL_BASE	0x04800000	/* CSCn (EMI Bank #2) */
#endif /* CFG_EPLD_PHYSICAL_BASE */
	/* map the physical address to UN-cached virtual address */
#if !defined(CFG_EPLD_BASE)
#	define CFG_EPLD_BASE		( 0xa0000000 | (CFG_EPLD_PHYSICAL_BASE) )
#endif /* CFG_EPLD_BASE */
	/*
	 * following are the offsets within the EMI EPLD (IC21),
	 * for the MB705 Peripheral board.
	 */
#define EPLD_IDENT		0x00	/* EPLD Identifier Register */
#define EPLD_TEST		0x02	/* EPLD Test Register */
#define EPLD_SWITCH		0x04	/* EPLD Switch Register */
#define EPLD_MISC		0x0a	/* Miscellaneous Control Register */
#endif	/* CONFIG_SH_MB705 */


#if defined(CONFIG_SH_MB705)
static inline void epld_write(unsigned long value, unsigned long offset)
{
	/* 16-bit write to EPLD registers */
	writew(value, CFG_EPLD_BASE + offset);
}

static inline unsigned long epld_read(unsigned long offset)
{
	/* 16-bit read from EPLD registers */
	return readw(CFG_EPLD_BASE + offset);
}

static int mb705_init_epld(void)
{
	const unsigned short test_value = 0x1234u;
	unsigned short epld_reg;
	unsigned short epld_version, board_version;

	/* write (anything) to the test register */
	epld_write(test_value, EPLD_TEST);
	/* verify we got back an inverted result */
	epld_reg = epld_read(EPLD_TEST);
	if (epld_reg != (test_value ^ 0xffffu)) {
		printf("Failed EPLD test (offset=%02x, result=%04x)\n",
			EPLD_TEST, epld_reg);
		return 1;
		}

	/* Assume we can trust the version register */
	epld_reg = epld_read(EPLD_IDENT);
	board_version = epld_reg >> 4 & 0xfu;
	epld_version = epld_reg & 0xfu;

	/* display the board revision, and EPLD version */
	printf("MB705: revision %c, EPLD version %02d\n",
		board_version + 'A' - 1,
		epld_version);

	/* return a "success" result */
	return 0;
}
#endif	/* CONFIG_SH_MB705 */

void flashWriteEnable(void)
{
#if defined(CONFIG_SH_MB705)
	unsigned short epld_reg;

	/* Enable Vpp for writing to flash */
	epld_reg = epld_read(EPLD_MISC);
	epld_reg |= 1u << 3;	/* NandFlashWP = MISC[3] = 1 */
	epld_reg |= 1u << 2;	/* NorFlashVpp = MISC[2] = 1 */
	epld_write(epld_reg, EPLD_MISC);
#endif	/* CONFIG_SH_MB705 */
}

void flashWriteDisable(void)
{
#if defined(CONFIG_SH_MB705)
	unsigned short epld_reg;

	/* Disable Vpp for writing to flash */
	epld_reg = epld_read(EPLD_MISC);
	epld_reg &= ~(1u << 3);	/* NandFlashWP = MISC[3] = 0 */
	epld_reg &= ~(1u << 2);	/* NorFlashVpp = MISC[2] = 0 */
	epld_write(epld_reg, EPLD_MISC);
#endif	/* CONFIG_SH_MB705 */
}

#ifdef QQQ	/* QQQ - DELETE */
#ifdef CONFIG_STMAC_LAN8700
static void phy_reset(void)
{
	/* Reset the SMSC LAN8700 PHY */
	STPIO_SET_PIN(PIO_PORT(5), 5, 1);
	STPIO_SET_PIN(PIO_PORT(11), 2, 1);
	udelay(1);
	STPIO_SET_PIN(PIO_PORT(5), 5, 0);
	udelay(100);
	STPIO_SET_PIN(PIO_PORT(5), 5, 1);
	udelay(1);
	STPIO_SET_PIN(PIO_PORT(11), 2, 0);
}
#endif	/* CONFIG_STMAC_LAN8700 */
#endif		/* QQQ - DELETE */


static void configPIO(void)
{
	unsigned long sysconf;

/* QQQ move the following somewhere better! */
#define STX7108_BANK2_SYSGFG(x)	SH4_DWORD_REG(STX7108_SYSCONF_BANK2_BASE + (x)*0x4)

	/* Setup PIOs for ASC device */

#if CFG_STM_ASC_BASE == ST40_ASC2_REGS_BASE
	/* Route UART2 via PIO14 for TX, RX, CTS & RTS */
	sysconf = *STX7108_BANK2_SYSGFG(14);
	/* PIO14[4] CFG14[17,16] AltFunction = 1 = UART2-TX */
	/* PIO14[5] CFG14[21,20] AltFunction = 1 = UART2-RX */
	/* PIO14[6] CFG14[25,24] AltFunction = 1 = UART2-RTS */
	/* PIO14[7] CFG14[28,28] AltFunction = 1 = UART2-CTS */
	sysconf &= ~0x13330000ul;	/* 1,3,3,3,0,0,0,0 */
	sysconf |=  0x11110000ul;	/* 1,1,1,1,0,0,0,0 */
	*STX7108_BANK2_SYSGFG(14)= sysconf;

	sysconf = *STX7108_BANK2_SYSGFG(18);
	/* PIO14[4] CFG18[20]=1 Output Enable for UART2-TX */
	/* PIO14[5] CFG18[21]=0 Output Enable for UART2-RX */
	/* PIO14[6] CFG18[22]=1 Output Enable for UART2-RTS */
	/* PIO14[7] CFG18[23]=0 Output Enable for UART2-CTS */
	sysconf &= ~( 1ul<<23 | 1ul<<22 | 1ul<<21 | 1ul<<20 );
	sysconf |=  ( 0ul<<23 | 1ul<<22 | 0ul<<21 | 1ul<<20 );
	*STX7108_BANK2_SYSGFG(18) = sysconf;

	sysconf = *STX7108_BANK2_SYSGFG(22);
	/* PIO14[4] CFG22[20]=0 Pull Up Pad for UART2-TX */
	/* PIO14[5] CFG22[21]=0 Pull Up Pad for UART2-RX */
	/* PIO14[6] CFG22[22]=0 Pull Up Pad for UART2-RTS */
	/* PIO14[7] CFG22[23]=0 Pull Up Pad for UART2-CTS */
	sysconf &= ~( 1ul<<23 | 1ul<<22 | 1ul<<21 | 1ul<<20 );
	*STX7108_BANK2_SYSGFG(22) = sysconf;

	sysconf = *STX7108_BANK2_SYSGFG(26);
	/* PIO14[4] CFG26[20]=0 Open Drain pad for UART2-TX */
	/* PIO14[5] CFG26[21]=0 Open Drain pad for UART2-RX */
	/* PIO14[6] CFG26[22]=0 Open Drain pad for UART2-RTS */
	/* PIO14[7] CFG26[23]=0 Open Drain pad for UART2-CTS */
	sysconf &= ~( 1ul<<23 | 1ul<<22 | 1ul<<21 | 1ul<<20 );
	*STX7108_BANK2_SYSGFG(26) = sysconf;
#elif CFG_STM_ASC_BASE == ST40_ASC2_REGS_BASE

#error QQQ - TO DO!

#else	/* CFG_STM_ASC_BASE == ST40_ASC2_REGS_BASE */
#error Unknown ASC port selected!
#endif	/* CFG_STM_ASC_BASE == ST40_ASC2_REGS_BASE */

#ifdef QQQ	/* QQQ - DELETE */
#ifdef CONFIG_STMAC_LAN8700
	/* Configure SMSC LAN8700 PHY Reset signals */
	SET_PIO_PIN(PIO_PORT(5), 5, STPIO_OUT);
	SET_PIO_PIN(PIO_PORT(11), 2, STPIO_OUT);
#endif	/* CONFIG_STMAC_LAN8700 */
#endif		/* QQQ - DELETE */
}

extern int board_init(void)
{
	configPIO();

	/* Reset the PHY */
#ifdef QQQ	/* QQQ - DELETE */
#ifdef CONFIG_STMAC_LAN8700
	phy_reset();
#endif	/* CONFIG_STMAC_LAN8700 */
#endif		/* QQQ - DELETE */

#ifdef QQQ	/* QQQ - DELETE */
#if defined(CONFIG_SH_STM_SATA)
	stx7105_configure_sata ();
#endif	/* CONFIG_SH_STM_SATA */
#endif		/* QQQ - DELETE */

	return 0;
}

int checkboard (void)
{
	printf ("\n\nBoard: STx7108-Mboard (MB837)"
#ifdef CONFIG_SH_SE_MODE
		"  [32-bit mode]"
#else
		"  [29-bit mode]"
#endif
		"\n");

#if defined(CONFIG_SH_MB705)
	/*
	 * initialize the EPLD on the MB705.
	 */
	mb705_init_epld();
#endif	/* CONFIG_SH_MB705 */

#if 0	/* QQQ - DELETE */
{
const unsigned long nand_reg = *ST40_EMI_NAND_VERSION_REG;
const unsigned long epld_reg = epld_read(EPLD_SWITCH);
	printf ("*ST40_EMI_NAND_VERSION_REG = %u.%u.%u\n",
		(nand_reg>>8)&0x0ful,
		(nand_reg>>4)&0x0ful,
		(nand_reg>>0)&0x0ful);
	printf("*EPLD_SWITCH = 0x%08x  -->  boot-from-%s\n",
		epld_reg,
		(epld_reg & (1ul<<8)) ? "NAND" : "NOR");
}
#endif	/* QQQ - DELETE */

	return 0;
}
