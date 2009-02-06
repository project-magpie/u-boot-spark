#define D { printf("QQQ: got to %s() @ %s:%u\n",__FUNCTION__,__FILE__,__LINE__); }
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
#include "ecc.h"


/*
 * hardware specific access to control-lines
 *	CL -> Emi_Addr(17)
 *	AL -> Emi_Addr(18)
 *	nCE is handled by EMI (not s/w controlable)
 */
static void mb680_hwcontrol(struct mtd_info *mtdinfo, int cmd)
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


#ifdef CFG_NAND_ECC_HW3_128
	/* QQQ move following to ST40-specific (not board) file */
static struct nand_oobinfo mb680_nand_oobinfo = {
	.useecc = MTD_NANDECC_AUTOPLACE,
#if 1	/* QQQ */
	.eccbytes = 12,
	.eccpos = {
		 0,  1,  2,	/* ECC for 1st 128-byte record */
		 4,  5,  6,	/* ECC for 2nd 128-byte record */
		 8,  9, 10,	/* ECC for 3rd 128-byte record */
		12, 13, 14},	/* ECC for 4th 128-byte record */
	.oobfree = { {3, 1}, {7, 1}, {11, 1}, {15, 1} }
#else	/* QQQ */
	.eccbytes = 48,
	.eccpos = {		/* QQQ: need to increase size of array DEFINITION */
		 0,  1,  2,	/* ECC for  1st 128-byte record */
		 4,  5,  6,	/* ECC for  2nd 128-byte record */
		 8,  9, 10,	/* ECC for  3rd 128-byte record */
		12, 13, 14,	/* ECC for  4th 128-byte record */
		16, 17, 18,	/* ECC for  5th 128-byte record */
		20, 21, 22,	/* ECC for  6th 128-byte record */
		24, 25, 26,	/* ECC for  7th 128-byte record */
		28, 29, 30,	/* ECC for  8th 128-byte record */
		32, 33, 34,	/* ECC for  9th 128-byte record */
		36, 37, 38,	/* ECC for 10th 128-byte record */
		40, 41, 42,	/* ECC for 11th 128-byte record */
		44, 45, 46,	/* ECC for 12th 128-byte record */
		48, 49, 50,	/* ECC for 13th 128-byte record */
		52, 53, 54,	/* ECC for 14th 128-byte record */
		56, 57, 58,	/* ECC for 15th 128-byte record */
		60, 61, 62},	/* ECC for 16th 128-byte record */
	.oobfree = {		/* QQQ: need to increase size of array DEFINITION */
		{ 3, 1}, { 7, 1}, {11, 1}, {15, 1},
		{19, 1}, {23, 1}, {27, 1}, {31, 1},
		{35, 1}, {39, 1}, {43, 1}, {47, 1},
		{51, 1}, {55, 1}, {59, 1}, {63, 1} },
#endif	/* QQQ */
};


	/* QQQ move following to ST40-specific (not board) file */
static int nand_mb680_calculate_ecc(struct mtd_info * const mtd, const u_char * const dat, u_char * const ecc_code)
{
	const struct nand_chip const * this = mtd->priv;

	printf("QQQ: nand_mb680_calculate_ecc(mtd=%p, dat=%p, ecc_code=%p)\n", mtd, dat, ecc_code);

	if ( (this->eccmode==NAND_ECC_HW3_128) &&
	     ((((unsigned long)dat)%4)==0) )	/* data *must* be 4-bytes aligned */
	{
		/* calculate the ECC bytes */
		const ecc_t computed_ecc = ecc_gen(dat, ECC_128);
		/* poke them into the right place */
		ecc_code[0] = computed_ecc.byte[0];
		ecc_code[1] = computed_ecc.byte[1];
		ecc_code[2] = computed_ecc.byte[2];
		printf("QQQ: calculated[0:2] = %02x %02x %02x\n", ecc_code[0], ecc_code[1], ecc_code[2]);	/* QQQ - DELETE */
	}
	else
	{
		printf("ERROR: Arghhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh!\n");	/* QQQ - SORT OUT */
		ecc_code[0] = 'B';
		ecc_code[1] = 'A';
		ecc_code[2] = 'D';
		return 1;							/* QQQ - SORT OUT */
	}

	return(0);
}


	/* QQQ move following to ST40-specific (not board) file */
static int nand_mb680_correct_data(struct mtd_info *mtd, u_char *dat, u_char *read_ecc, u_char *calc_ecc)
{
	printf("QQQ: nand_mb680_correct_data(mtd=%p, dat=%p, read_ecc=%p, calc_ecc=%p)\n", mtd, dat, read_ecc, calc_ecc);
#if 0
enum ecc_check
{
  E_UN_CHK = -1, /* uncorrectable error. */
  E_NO_CHK = 0,  /* No Errors. */
  E_D1_CHK = 1,  /* 1-bit data error. */
  E_C1_CHK = 2   /* 1-bit code error. */
};
#endif
#if 0
	struct nand_chip	*this;
	int			block_count = 0, i, rc;

	this = mtd->priv;
	block_count = (this->eccmode == NAND_ECC_HW12_2048) ? 4 : 1;
	for (i = 0; i < block_count; i++) {
		if (memcmp(read_ecc, calc_ecc, 3) != 0) {
			rc = nand_davinci_compare_ecc(read_ecc, calc_ecc, dat);
			if (rc < 0) {
				return(rc);
			}
		}
		read_ecc += 3;
		calc_ecc += 3;
		dat += 512;
	}
#endif
	return(0);
}


	/* QQQ move following to ST40-specific (not board) file */
static void nand_mb680_enable_hwecc(struct mtd_info *mtd, int mode)
{
	/* do nothing - we are only emulating HW in SW */
}
#endif /* CFG_NAND_ECC_HW3_128 */


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
	nand->hwcontrol = mb680_hwcontrol;
	nand->chip_delay = 25;
	nand->options = NAND_NO_AUTOINCR;

#ifdef CFG_NAND_ECC_HW3_128
	nand->eccmode       = NAND_ECC_HW3_128;
	nand->options      |= NAND_USE_FLASH_BBT;
	nand->autooob       = &mb680_nand_oobinfo;	/* QQQ: "mb680" -> "st40" (or similar) */
	nand->calculate_ecc = nand_mb680_calculate_ecc;	/* QQQ: "mb680" -> "st40" (or similar) */
	nand->correct_data  = nand_mb680_correct_data;	/* QQQ: "mb680" -> "st40" (or similar) */
	nand->enable_hwecc  = nand_mb680_enable_hwecc;	/* QQQ: "mb680" -> "st40" (or similar) */
#else
	nand->eccmode       = NAND_ECC_SOFT;
#endif /* CFG_NAND_ECC_HW3_128 */

	return 0;
}
#endif	/* CONFIG_CMD_NAND */

