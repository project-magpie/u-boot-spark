/*
 * (C) Copyright 2006 DENX Software Engineering
 * (C) Copyright 2008-2009 STMicroelectronics, Sean McGoogan <Sean.McGoogan@st.com>
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


#if defined(CONFIG_CMD_NAND)

#include <nand.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <asm/stx7105reg.h>
#include <asm/stm-nand.h>


/*
 * hardware specific access to control-lines
 *	CL -> Emi_Addr(17)
 *	AL -> Emi_Addr(18)
 *	nCE is handled by EMI (not s/w controlable)
 */
static void pdk7105_hwcontrol(struct mtd_info *mtdinfo, int cmd)
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


/*
 * hardware specific access to the Ready/not_Busy signal.
 * Signal is routed through the EMI NAND Controller block.
 */
static int pdk7105_device_ready(struct mtd_info *mtd)
{
	/* extract bit 1: status of RBn pin on boot bank */
	return ((*ST40_EMI_NAND_RBN_STA) & (1ul<<1)) ? 1 : 0;
}


/*
 * Board-specific NAND initialization. The following members of the
 * argument are board-specific (per include/linux/mtd/nand.h):
 * - IO_ADDR_R?: address to read the 8 I/O lines of the flash device
 * - IO_ADDR_W?: address to write the 8 I/O lines of the flash device
 * - hwcontrol: hardwarespecific function for accesing control-lines
 * - dev_ready: hardwarespecific function for  accesing device ready/busy line
 * - enable_hwecc?: function to enable (reset)  hardware ecc generator. Must
 *   only be provided if a hardware ECC is available
 * - eccmode: mode of ecc, see defines
 * - chip_delay: chip dependent delay for transfering data from array to
 *   read regs (tR)
 * - options: various chip options. They can partly be set to inform
 *   nand_scan about special functionality. See the defines for further
 *   explanation
 * Members with a "?" were not set in the merged testing-NAND branch,
 * so they are not set here either.
 */
extern int board_nand_init(struct nand_chip *nand)
{
	struct mtd_info * const mtd = nand->priv;

	nand->hwcontrol     = pdk7105_hwcontrol;
	nand->dev_ready     = pdk7105_device_ready;
	nand->eccmode       = NAND_ECC_SOFT;
	nand->options       = NAND_NO_AUTOINCR;
#if 1
	nand->options      |= NAND_USE_FLASH_BBT;
	nand->badblock_pattern = &stm_nand_badblock_pattern;
#endif

#ifdef CFG_NAND_ECC_HW3_128
	mtd->read           = stm_nand_read;
	mtd->write          = stm_nand_write;
	mtd->read_ecc       = stm_nand_read_ecc;
	mtd->write_ecc      = stm_nand_write_ecc;
	mtd->read_oob       = stm_nand_read_oob;
	mtd->write_oob      = stm_nand_write_oob;
	nand->enable_hwecc  = stm_nand_enable_hwecc;
#endif /* CFG_NAND_ECC_HW3_128 */

	return 0;
}
#endif	/* CONFIG_CMD_NAND */

