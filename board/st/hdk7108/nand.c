/*
 * (C) Copyright 2006 DENX Software Engineering
 * (C) Copyright 2008-2011 STMicroelectronics, Sean McGoogan <Sean.McGoogan@st.com>
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
#include <nand.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <asm/socregs.h>
#include <asm/stm-nand.h>


/*
 * hardware specific access to control-lines for "bit-banging".
 *	CL -> Emi_Addr(17)
 *	AL -> Emi_Addr(18)
 *	nCE is handled by EMI (not s/w controllable)
 */
#ifndef CFG_NAND_FLEX_MODE	/* for "bit-banging" (c.f. STM "flex-mode")  */
static void board_hwcontrol(struct mtd_info *mtdinfo, int cmd)
{
	struct nand_chip* this = (struct nand_chip *)(mtdinfo->priv);

	switch(cmd) {

	case NAND_CTL_SETCLE:
		this->IO_ADDR_W = (void *)((unsigned int)this->IO_ADDR_W | (1u << 17));
		break;

	case NAND_CTL_CLRCLE:
		this->IO_ADDR_W = (void *)((unsigned int)this->IO_ADDR_W & ~(1u << 17));
		break;

	case NAND_CTL_SETALE:
		this->IO_ADDR_W = (void *)((unsigned int)this->IO_ADDR_W | (1u << 18));
		break;

	case NAND_CTL_CLRALE:
		this->IO_ADDR_W = (void *)((unsigned int)this->IO_ADDR_W & ~(1u << 18));
		break;
	}
}
#endif /* CFG_NAND_FLEX_MODE */


/*
 * hardware specific access to the Ready/not_Busy signal.
 * Signal is routed through the EMI NAND Controller block.
 */
#ifndef CFG_NAND_FLEX_MODE	/* for "bit-banging" (c.f. STM "flex-mode")  */
static int board_device_ready(struct mtd_info *mtd)
{
	/* extract bit 1: status of RBn pin on boot bank */
	return ((*ST40_EMI_NAND_RBN_STA) & (1ul<<1)) ? 1 : 0;
}
#endif /* CFG_NAND_FLEX_MODE */


/*
 * Board-specific NAND initialization.
 *
 * Please see comment in cpu/sh/stm-nand.c for details ...
 */
extern int board_nand_init(struct nand_chip *nand)
{
	nand->eccmode       = NAND_ECC_SOFT;	/* default is S/W 3/256 ECC */
	nand->options       = NAND_NO_AUTOINCR;

#ifdef CFG_NAND_FLEX_MODE	/* for STM "flex-mode" (c.f. "bit-banging") */
	nand->select_chip   = stm_flex_select_chip;
	nand->dev_ready     = stm_flex_device_ready;
	nand->hwcontrol     = stm_flex_hwcontrol;
	nand->read_byte     = stm_flex_read_byte;
	nand->write_byte    = stm_flex_write_byte;
	nand->read_buf      = stm_flex_read_buf;
	nand->write_buf     = stm_flex_write_buf;
#else				/* for "bit-banging" (c.f. STM "flex-mode")  */
	nand->dev_ready     = board_device_ready;
	nand->hwcontrol     = board_hwcontrol;
#ifdef CFG_NAND_ECC_HW3_128	/* for STM "boot-mode" ECC + "bit-banging"! */
	nand->select_chip   = stm_nand_select_chip;
#endif /* CFG_NAND_ECC_HW3_128 */
#endif /* CFG_NAND_FLEX_MODE */

	/* override scan_bbt(), even if not using a Bad Block Table (BBT) */
	nand->scan_bbt      = stm_nand_default_bbt;

#if 1	/* Enable to use a NAND-resident (non-volatile) Bad Block Table (BBT) */
	nand->options      |= NAND_USE_FLASH_BBT;
#endif

	return 0;
}
