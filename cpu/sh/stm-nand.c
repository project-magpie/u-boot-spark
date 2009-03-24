/*
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
#include <asm/stm-nand.h>
#include <asm/ecc.h>


#ifdef CFG_NAND_ECC_HW3_128	/* for STM "boot-mode" */

struct nand_oobinfo stm_nand_oobinfo = {
	.useecc = MTD_NANDECC_AUTOPLACE,
#if defined(CFG_NAND_SMALLPAGE)
	.eccbytes = 12,
	.eccpos = {
		 0,  1,  2,	/* ECC for 1st 128-byte record */
		 4,  5,  6,	/* ECC for 2nd 128-byte record */
		 8,  9, 10,	/* ECC for 3rd 128-byte record */
		12, 13, 14},	/* ECC for 4th 128-byte record */
	.oobfree = { {3, 1}, {7, 1}, {11, 1}, {15, 1} }
#elif defined(CFG_NAND_LARGEPAGE)
	.eccbytes = 48,
	.eccpos = {
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
	.oobfree = {
		{ 3, 1}, { 7, 1}, {11, 1}, {15, 1},
		{19, 1}, {23, 1}, {27, 1}, {31, 1},
		{35, 1}, {39, 1}, {43, 1}, {47, 1},
		{51, 1}, {55, 1}, {59, 1}, {63, 1} },
#else
#error "Either CFG_NAND_LARGEPAGE or CFG_NAND_SMALLPAGE must be defined!"
#endif
};


#define isprint(x)	( ((x)>=0x20u) && ((x)<0x7fu) )			/* QQQ - DELETE */
	/* QQQ move following to ST40-specific (not board) file */
extern int stm_nand_calculate_ecc (
	struct mtd_info * const mtd,
	const u_char * const dat,
	u_char * const ecc_code)
{
	const struct nand_chip const * this = mtd->priv;

//QQQ	printf("QQQ: %s(mtd=%p, dat=%p, ecc_code=%p)\n", __FUNCTION__, mtd, dat, ecc_code);	/* QQQ - DELETE */

	if (this->eccmode!=NAND_ECC_HW3_128)
	{
		printf("ERROR: Can not calculate ECC: Internal Error (eccmode=%u)\n",
			this->eccmode);
		ecc_code[0] = 'B';
		ecc_code[1] = 'A';
		ecc_code[2] = 'D';
		return 1;	/* Note: caller ignores this value! */
	}
	else if ((((unsigned long)dat)%4)!=0)	/* data *must* be 4-bytes aligned */
	{
		printf("ERROR: Can not calculate ECC: data (%08lx) must be 4-byte aligned!\n",
			(unsigned long)dat);
		ecc_code[0] = 'B';
		ecc_code[1] = 'A';
		ecc_code[2] = 'D';
		return 2;	/* Note: caller ignores this value! */
	}
	else	/* calculate 3 ECC bytes per 128 bytes of data */
	{
		const ecc_t computed_ecc = ecc_gen(dat, ECC_128);
		/* poke them into the right place */
		ecc_code[0] = computed_ecc.byte[0];
		ecc_code[1] = computed_ecc.byte[1];
		ecc_code[2] = computed_ecc.byte[2];
#if 0									/* QQQ - DELETE */
		printf("QQQ: calculated[0:2] = %02x %02x %02x  (%c%c%c)\n",
			(unsigned)ecc_code[0],
			(unsigned)ecc_code[1],
			(unsigned)ecc_code[2],
			isprint(ecc_code[0]) ? ecc_code[0] : '.',
			isprint(ecc_code[1]) ? ecc_code[1] : '.',
			isprint(ecc_code[2]) ? ecc_code[2] : '.');	/* QQQ - DELETE */
#endif									/* QQQ - DELETE */
	}

	return 0;
}


extern int stm_nand_correct_data (
	struct mtd_info *mtd,
	u_char *dat,
	u_char *read_ecc,
	u_char *calc_ecc)
{
#if 0									/* QQQ - DELETE */
	printf("QQQ: %s(mtd=%p, dat=%p, read_ecc=%p, calc_ecc=%p)\n",
		__FUNCTION__, mtd, dat, read_ecc, calc_ecc);		/* QQQ - DELETE */
#endif									/* QQQ - DELETE */

	if (    (read_ecc[0] == calc_ecc[0]) &&
		(read_ecc[1] == calc_ecc[1]) &&
		(read_ecc[2] == calc_ecc[2])    )
	{
		return 0;		/* ECCs agree, nothing to do */
	}

#if 0									/* QQQ - DELETE */
	printf("ERROR: ECC detected - NOT corrected!\n");		/* QQQ - NEED TO FIX THIS PROPERLY */
	printf("QQQ: read_ecc[0:2]   = %02x %02x %02x  (%c%c%c)\n",
		(unsigned)read_ecc[0],
		(unsigned)read_ecc[1],
		(unsigned)read_ecc[2],
		isprint(read_ecc[0]) ? read_ecc[0] : '.',
		isprint(read_ecc[1]) ? read_ecc[1] : '.',
		isprint(read_ecc[2]) ? read_ecc[2] : '.');		/* QQQ - DELETE */
	printf("QQQ: calc_ecc[0:2]   = %02x %02x %02x  (%c%c%c)\n",
		(unsigned)calc_ecc[0],
		(unsigned)calc_ecc[1],
		(unsigned)calc_ecc[2],
		isprint(calc_ecc[0]) ? calc_ecc[0] : '.',
		isprint(calc_ecc[1]) ? calc_ecc[1] : '.',
		isprint(calc_ecc[2]) ? calc_ecc[2] : '.');		/* QQQ - DELETE */
#endif									/* QQQ - DELETE */
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
	return 0;
}


extern void stm_nand_enable_hwecc (
	struct mtd_info *mtd,
	int mode)
{
	/* do nothing - we are only emulating HW in SW */
}


#endif	/* CFG_NAND_ECC_HW3_128 */


#endif	/* CONFIG_CMD_NAND */
