/*
 * Copyright (C) 2007-2008 STMicroelectronics Limited
 * Stuart Menefy <stuart.menefy@st.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/stb7100reg.h>
#include <asm/io.h>
#include <asm/pio.h>
#include "stbus.h"

#if defined(CONFIG_USB_OHCI_NEW) && defined(CFG_USB_OHCI_CPU_INIT)

#define mdelay(x)	do { udelay((x)*1000); } while(0)


	/* QQQ: following line should be in configPIO() */
#define PIO_BASE  0xb8020000	/* Phys 0x18020000 */

extern int usb_cpu_init(void)
{
	unsigned long reg;
	DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	/* Work around for USB over-current detection chip being
	 * active low, and the 710x being active high.
	 *
	 * This test is wrong for 7100 cut 3.0 (which needs the work
	 * around), but as we can't reliably determine the minor
	 * revision number, hard luck, this works for most people.
	 */
	if ( ( (STB7100_DEVICEID_7109(bd->bi_devid)) &&
	       (STB7100_DEVICEID_CUT(bd->bi_devid) < 2) ) ||
	     ( (STB7100_DEVICEID_7100(bd->bi_devid)) &&
	       (STB7100_DEVICEID_CUT(bd->bi_devid) < 3) ) )
	{
		/* QQQ: following line should be in configPIO() */
		/* Setup PIO for USB over-current */
		SET_PIO_PIN(PIO_PORT(5), 6, STPIO_OUT);	/* QQQ CHIP specific */
		STPIO_SET_PIN(PIO_PORT(5), 6, 0);
	}

	/*
	 * There have been two changes to the USB power enable signal:
	 *
	 * - 7100 upto and including cut 3.0 and 7109 1.0 generated an
	 *   active high enables signal. From 7100 cut 3.1 and 7109 cut 2.0
	 *   the signal changed to active low.
	 *
	 * - The 710x ref board (mb442) has always used power distribution
	 *   chips which have active high enables signals (on rev A and B
	 *   this was a TI TPS2052, rev C used the ST equivalent a ST2052).
	 *   However rev A and B had a pull up on the enables signal, while
	 *   rev C changed this to a pull down.
	 *
	 * The net effect of all this is that the easiest way to drive
	 * this signal is ignore the USB hardware and drive it as a PIO
	 * pin.
	 *
	 * (Note the USB over current input on the 710x changed from active
	 * high to low at the same cuts, but board revs A and B had a resistor
	 * option to select an inverted output from the TPS2052, so no
	 * software work around is required.)
	 */
	/* QQQ: following line should be in configPIO() */
	/* Setup PIO for USB power */
	SET_PIO_PIN(PIO_PORT(5), 7, STPIO_OUT);	/* QQQ CHIP specific */
	STPIO_SET_PIN(PIO_PORT(5), 7, 1);

	/****************************************************************
	 ****************************************************************
	 *		NOTE: ABOVE is ONLY for the STb710x		*
	 ***************************************************************
	 ****************************************************************/

	/* QQQ: The following needs to be re-done for STx7200 */
	/* Make sure PLL is on */
	reg = readl(SYS_CFG2);
	if (reg & SYS_CFG2_PLL_POWER_DOWN_BIT)
	{
		writel(reg & (~SYS_CFG2_PLL_POWER_DOWN_BIT), SYS_CFG2);
		mdelay(100);	/* QQQ: can this delay be shorter ? */
	}

	/****************************************************************
	 ****************************************************************
	 *		NOTE: BELOW is COMMON to the STb710x/STx7200	*
	 ****************************************************************
	 ****************************************************************/


	/* Set strap mode */
#define STRAP_MODE	AHB2STBUS_STRAP_16_BIT
	reg = readl(AHB2STBUS_STRAP);
#if STRAP_MODE == 0
	reg &= ~AHB2STBUS_STRAP_16_BIT;
#else
	reg |= STRAP_MODE;
#endif
	writel(reg, AHB2STBUS_STRAP);

	/* Start PLL */
	reg = readl(AHB2STBUS_STRAP);
	writel(reg | AHB2STBUS_STRAP_PLL, AHB2STBUS_STRAP);
	mdelay(100);	/* QQQ: can this delay be shorter ? */
	writel(reg & (~AHB2STBUS_STRAP_PLL), AHB2STBUS_STRAP);
	mdelay(100);	/* QQQ: can this delay be shorter ? */

	/* Set the STBus Opcode Config for 32-bit access */
	writel(AHB2STBUS_STBUS_OPC_32BIT, AHB2STBUS_STBUS_OPC);

	/* Set the Message Size Config to 4 packets per message */
	writel(AHB2STBUS_MSGSIZE_4, AHB2STBUS_MSGSIZE);

	/* Set the Chunk Size Config to 4 packets per chunk */
	writel(AHB2STBUS_CHUNKSIZE_4, AHB2STBUS_CHUNKSIZE);

	return 0;
}

extern int usb_cpu_stop(void)
{
	return 0;
}

extern int usb_cpu_init_fail(void)
{
	return 0;
}

#endif /* defined(CONFIG_USB_OHCI) && defined(CFG_USB_OHCI_CPU_INIT) */

