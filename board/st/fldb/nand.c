/*
 * (C) Copyright 2006 DENX Software Engineering
 * (C) Copyright 2008-2012 STMicroelectronics, Sean McGoogan <Sean.McGoogan@st.com>
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
#if defined(CONFIG_SYS_ST40_NAND_USE_BIT_BANGING)	/* use the S/W "bit-banging" driver */
static void fldb_cmd_ctrl (
	struct mtd_info * const mtd,
	const int byte,
	const unsigned int ctrl)
{
	struct nand_chip * const this = (struct nand_chip *)(mtd->priv);
	unsigned int addr = (unsigned int)this->IO_ADDR_W;

	if (ctrl & NAND_CTRL_CHANGE)			/* update the "control" lines ? */
	{
		if ( ctrl & NAND_CLE )			/* a COMMAND Cycle ? */
		{
#if defined(DEBUG_NAND_BIT_BANGING_CMD_CTRL)
			printf("\t\t\t\t\t\t----START COMMAND----\n");
			BUG_ON(ctrl & NAND_ALE);	/* just in case ... */
#endif
			addr |= (1u << 17);		/* set CLE */
		}
		else
		{
			addr &= ~(1u << 17);		/* clear CLE */
		}

		if ( ctrl & NAND_ALE )			/* an ADDRESS Cycle ? */
		{
#if defined(DEBUG_NAND_BIT_BANGING_CMD_CTRL)
			printf("\t\t\t\t\t\t----START ADDRESS----\n");
			BUG_ON(ctrl & NAND_CLE);	/* just in case ... */
#endif
			addr |= (1u << 18);		/* set ALE */
		}
		else
		{
			addr &= ~(1u << 18);		/* clear ALE */
		}

		this->IO_ADDR_W = (void *)(addr);	/* now, store updated address back */
	}

	if (byte != NAND_CMD_NONE)			/* write it ? */
	{
#if defined(DEBUG_NAND_BIT_BANGING_CMD_CTRL)
		printf("\t\t\t\t\t\t----WRITING byte 0x%02x to 0x%08x ----\n", byte, addr);
#endif
		writeb(byte, addr);			/* write one byte */
	}
}

/*
 * hardware specific access to the Ready/not_Busy signal.
 * Signal is routed through the EMI NAND Controller block.
 */
static int fldb_device_ready(struct mtd_info *mtd)
{
	/* extract bit 1: status of RBn pin on boot bank */
	return ((*ST40_EMI_NAND_HAM_RBN_STA) & (1ul<<1)) ? 1 : 0;
}
#endif /* CONFIG_SYS_ST40_NAND_USE_BIT_BANGING */


/*
 * Board-specific NAND initialization.
 * We use a "generic" STM function stm_default_board_nand_init() to do it.
 * However, we can easily override anything locally, if required.
 */
extern int board_nand_init(struct nand_chip * const nand)
{
#if defined(CONFIG_SYS_ST40_NAND_USE_BIT_BANGING)	/* use the S/W "bit-banging" driver */
	stm_default_board_nand_init(nand, fldb_cmd_ctrl, fldb_device_ready);
#else							/* else, use a H/W driver */
	stm_default_board_nand_init(nand, NULL, NULL);
#endif /* CONFIG_SYS_ST40_NAND_USE_BIT_BANGING */

	/*
	 * Only enable the following to use a (volatile) RAM-based
	 * (not NAND-resident) Bad Block Table (BBT).
	 * This is *not* recommended! A NAND-resident BBT is recommended.
	 */
#if 0
	nand->options &= ~NAND_USE_FLASH_BBT;
#endif

	return 0;
}
