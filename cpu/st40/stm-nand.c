/*
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
#include <malloc.h>


#if defined(CONFIG_CMD_NAND)

#include <nand.h>
#include <asm/stm-nand.h>
#include <asm/ecc.h>
#include <asm/errno.h>
#include <asm/st40reg.h>
#include <asm/io.h>
#include <asm/socregs.h>


	/*
	 * Ensure we have *exactly* ONE NAND driver configured.
	 * Not less than one, and not more than one!
	 */
#if defined(CONFIG_SYS_ST40_NAND_USE_BIT_BANGING) && defined(CONFIG_SYS_ST40_NAND_USE_HAMMING)
#	error You can only enable *one* NAND driver!
#elif defined(CONFIG_SYS_ST40_NAND_USE_BIT_BANGING) && defined(CONFIG_SYS_ST40_NAND_USE_BCH)
#	error You can only enable *one* NAND driver!
#elif defined(CONFIG_SYS_ST40_NAND_USE_BCH) && defined(CONFIG_SYS_ST40_NAND_USE_HAMMING)
#	error You can only enable *one* NAND driver!
#endif
#if !defined(CONFIG_SYS_ST40_NAND_USE_BIT_BANGING) && !defined(CONFIG_SYS_ST40_NAND_USE_HAMMING) && !defined(CONFIG_SYS_ST40_NAND_USE_BCH)
#	error You must enable one NAND driver if CONFIG_CMD_NAND is defined!
#endif

	/*
	 * Also, when using BCH, we can *only* support the specific ECC
	 * schemes that the underlying H/W supports:
	 *
	 *	1) no ECC (not recommended!)
	 *	2) 18/1024 (18-bits per 1KiB of data)
	 *	3) 30/1024 (30-bits per 1KiB of data)
	 *
	 * Specifically, we can not support the previously used H/W
	 * schemes that the Hamming controller used, so we complain
	 * if we are configured for such an invalid combination.
	 */
#if defined(CONFIG_SYS_ST40_NAND_USE_BCH) && defined(CONFIG_SYS_NAND_ECC_HW3_128)
#	error boot-mode ECC (3/128) is not supportable with the BCH controller
#endif
#if defined(CONFIG_SYS_ST40_NAND_USE_BCH) && defined(CONFIG_SYS_NAND_ECC_AFM4)
#	error AFM4 ECC (4+3/512) is not supportable with the BCH controller
#endif

	/*
	 * Also, for BCH we must specify *one* of 3 possible H/W ECC schemes to use.
	 *
	 *	1) no ECC (not recommended!)
	 *	2) 18/1024 (18-bits per 1KiB of data)
	 *	3) 30/1024 (30-bits per 1KiB of data)
	 *
	 * Ensure we do not define more than (or less than) one such scheme!
	 */
#if defined(CONFIG_SYS_ST40_NAND_USE_BCH)
#if defined(CONFIG_SYS_ST40_NAND_USE_BCH_18_BIT_ECC) && defined(CONFIG_SYS_ST40_NAND_USE_BCH_30_BIT_ECC)
#	error You can only enable *one* BCH ECC scheme!
#elif defined(CONFIG_SYS_ST40_NAND_USE_BCH_18_BIT_ECC) && defined(CONFIG_SYS_ST40_NAND_USE_BCH_NO_ECC)
#	error You can only enable *one* BCH ECC scheme!
#elif defined(CONFIG_SYS_ST40_NAND_USE_BCH_NO_ECC) && defined(CONFIG_SYS_ST40_NAND_USE_BCH_30_BIT_ECC)
#	error You can only enable *one* BCH ECC scheme!
#endif
#if !defined(CONFIG_SYS_ST40_NAND_USE_BCH_18_BIT_ECC) && !defined(CONFIG_SYS_ST40_NAND_USE_BCH_30_BIT_ECC) && !defined(CONFIG_SYS_ST40_NAND_USE_BCH_NO_ECC)
#	error You must select only one BCH ECC scheme if CONFIG_SYS_ST40_NAND_USE_BCH is defined!
#endif
#endif /* CONFIG_SYS_ST40_NAND_USE_BCH */


#define isprint(x)	( ((x)>=0x20u) && ((x)<0x7fu) )


#define VERBOSE_ECC		0	/* Enable for verbose ECC information  */


/*
 * STMicroelectronics' H/W ECC layouts:
 *
 * Boot-mode ECC: 128-byte data records, 3 bytes ECC + 1 byte marker ('B')
 *                Hence, Boot-Mode is described as 3+1/128 ECC.
 *
 * AFM3 ECC:      512-byte data records, 3 bytes H/W ECC
 *                Hence, AFM3 is described as 3/512 ECC.
 *
 * AFM4 ECC:      512-byte data records, 3 bytes H/W ECC, 1 byte S/W ECC, 3 bytes marker ('A', 'F', 'M')
 *                Hence, AFM4 is described as 4+3/512 ECC.
 *
 * Note: ECC schemes described here using the form "x+y/z", means:
 * x-bytes of ECC plus y-bytes of "marker bytes" per z-byte record of data.
 *
 * Note: There is a bug in the AFM3 H/W engine that results in two of the
 * parity bits (LP16 + LP17) being calculated incorrectly. The S/W workaround
 * for this issue is to store the correct values for these two bits in a
 * 4th ECC byte (hence AFM4). The S/W will then "fix-up" the LP16+LP17 bits.
 * It is ill-advised to use the redundant top 6-bits in the 4th byte - beware!
 * In addition, a 3-byte tag is also stored along with the ECC, hence 4+3/512 ECC.
 */
#if !defined(CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B)
#	define CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B	1	/* Enable 'B' tagging */
#endif	/* CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B */
#if !defined(CONFIG_SYS_STM_NAND_AFM4_ECC_WITH_AFM)
#	define CONFIG_SYS_STM_NAND_AFM4_ECC_WITH_AFM	1	/* Enable "AFM" tagging */
#endif	/* CONFIG_SYS_STM_NAND_AFM4_ECC_WITH_AFM */

#if CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B
#	define ECC_BYTES				(3 + 1)	/* ECC 3+1/128, '+1' for 'B' tag */
#else	/* CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B */
#	define ECC_BYTES				(3)	/* ECC 3/128 */
#endif	/* CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B */


/*
 * Define the bad/good block scan pattern which are used while scanning
 * the NAND device for any factory marked good/bad blocks.
 *
 * For small page devices, factory bad block markers are in byte 5.
 * For large page devices, factory bad block markers are in bytes 0 & 1.
 * Any bit in those bytes being a zero, implies the entire block is bad.
 *
 * Using "SCANALLPAGES" takes a significant performance hit - tens of seconds!
 * So, we override the default structures (which is to scan *all* pages),
 * and instead, we only scan the first & 2nd pages in each block.
 * However, we do also check the entire data area in those 2 pages,
 * to see if they are properly erased. Improperly erased pages
 * result in the entire bock also being treated as bad.
 */
static uint8_t scan_pattern[] = { 0xffu, 0xffu };

static struct nand_bbt_descr stm_nand_badblock_pattern_16 = {
	.pattern = scan_pattern,
	.options = NAND_BBT_SCANEMPTY /* | NAND_BBT_SCANALLPAGES */ | NAND_BBT_SCAN2NDPAGE,
	.offs = 5,	/* Byte 5 */
	.len = 1
};

static struct nand_bbt_descr stm_nand_badblock_pattern_64 = {
	.pattern = scan_pattern,
	.options = NAND_BBT_SCANEMPTY /* | NAND_BBT_SCANALLPAGES */ | NAND_BBT_SCAN2NDPAGE,
	.offs = 0,	/* Bytes 0-1 */
	.len = 2
};


/*****************************************************************************************
 *****************************************************************************************
 *****************		Common "ECC" functions		**************************
 *****************************************************************************************
 *****************************************************************************************/


#if defined(CONFIG_SYS_NAND_ECC_HW3_128) || defined(CONFIG_SYS_NAND_ECC_AFM4)


static void stm_nand_hwctl (
	struct mtd_info *mtd,
	int mode)
{
	/* do nothing - we are only emulating H/W in S/W */
}


static int stm_nand_calculate (
	struct mtd_info * const mtd,
	const u_char * const dat,
	u_char * const ecc_code)
{
	const struct nand_chip const * this = mtd->priv;

	if ((((unsigned long)dat)%4)!=0)	/* data *must* be 4-bytes aligned */
	{
		printf("ERROR: Can not calculate ECC: data (%08lx) must be 4-byte aligned!\n",
			(unsigned long)dat);
		BUG();
		return -1;	/* Note: caller ignores this value! */
	}
	else
#if defined(CONFIG_SYS_NAND_ECC_HW3_128)	/* for STM "boot-mode" */
	if (this->ecc.mode==NAND_ECC_HW && this->ecc.size==128 && this->ecc.bytes==ECC_BYTES)
	{	/* calculate 3 ECC bytes per 128 bytes of data */
		const ecc_t computed_ecc = ecc_gen(dat, ECC_128);
		/* poke them into the right place */
		ecc_code[0] = computed_ecc.byte[0];
		ecc_code[1] = computed_ecc.byte[1];
		ecc_code[2] = computed_ecc.byte[2];
#if CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B
		ecc_code[3] = 'B';	/* append ASCII 'B', for Boot-mode */
#endif	/* CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B */
	}
	else
#endif /* CONFIG_SYS_NAND_ECC_HW3_128 */
#if defined(CONFIG_SYS_NAND_ECC_AFM4)		/* for STM AFM4 (4+3/512) ECC compatibility */
	if (this->ecc.mode==NAND_ECC_HW && this->ecc.size==512 && this->ecc.bytes==7)
	{	/* calculate 3 ECC bytes per 512 bytes of data */
		const ecc_t computed_ecc = ecc_gen(dat, ECC_512);
		/* poke them into the right place */
		ecc_code[0] = computed_ecc.byte[0];
		ecc_code[1] = computed_ecc.byte[1];
		ecc_code[2] = computed_ecc.byte[2];
		ecc_code[3] = 'A';	/* ASCII 'A', for AFM4 */
		ecc_code[4] = 'F';	/* ASCII 'F', for AFM4 */
		ecc_code[5] = 'M';	/* ASCII 'M', for AFM4 */
			/* also copy the 2 bits LP16 + LP17 in Byte6[1:0] as well! */
		ecc_code[6] = computed_ecc.byte[2] & 0x03;	/* set top 6-bits to zero */
	}
	else
#endif /* CONFIG_SYS_NAND_ECC_AFM4 */
	{
		printf("ERROR: Can not calculate ECC: Internal Error (mode=%u,size=%u,bytes=%u)\n",
			this->ecc.mode, this->ecc.size, this->ecc.bytes);
		BUG();
		return -1;	/* Note: caller ignores this value! */
	}

	return 0;
}


static int stm_nand_correct (
	struct mtd_info *mtd,
	u_char *dat,
	u_char *read_ecc,
	u_char *calc_ecc)
{
	ecc_t read, calc;
	enum ecc_check result;
	const struct nand_chip const * this = mtd->priv;
	enum ecc_size ecc_size;

#if defined(CONFIG_SYS_NAND_ECC_HW3_128)	/* for STM "boot-mode" */
	if (this->ecc.mode==NAND_ECC_HW && this->ecc.size==128 && this->ecc.bytes==ECC_BYTES)
	{
		/* do we need to try and correct anything ? */
		if (    (read_ecc[0] == calc_ecc[0]) &&
			(read_ecc[1] == calc_ecc[1]) &&
			(read_ecc[2] == calc_ecc[2])    )
		{
			return 0;		/* ECCs agree, nothing to do */
		}

	#if VERBOSE_ECC
		printf("warning: ECC error detected!  "
			"read_ecc %02x:%02x:%02x (%c%c%c) != "
			"calc_ecc %02x:%02x:%02x (%c%c%c)\n",
			(unsigned)read_ecc[0],
			(unsigned)read_ecc[1],
			(unsigned)read_ecc[2],
			isprint(read_ecc[0]) ? read_ecc[0] : '.',
			isprint(read_ecc[1]) ? read_ecc[1] : '.',
			isprint(read_ecc[2]) ? read_ecc[2] : '.',
			(unsigned)calc_ecc[0],
			(unsigned)calc_ecc[1],
			(unsigned)calc_ecc[2],
			isprint(calc_ecc[0]) ? calc_ecc[0] : '.',
			isprint(calc_ecc[1]) ? calc_ecc[1] : '.',
			isprint(calc_ecc[2]) ? calc_ecc[2] : '.');
	#endif	/* VERBOSE_ECC */

		/* put ECC bytes into required structure */
		read.byte[0] = read_ecc[0];
		read.byte[1] = read_ecc[1];
		read.byte[2] = read_ecc[2];
		calc.byte[0] = calc_ecc[0];
		calc.byte[1] = calc_ecc[1];
		calc.byte[2] = calc_ecc[2];
		ecc_size = ECC_128;		/* 128 bytes/record */
	}
	else
#endif /* CONFIG_SYS_NAND_ECC_HW3_128 */
#if defined(CONFIG_SYS_NAND_ECC_AFM4)		/* for STM AFM4 (4+3/512) ECC compatibility */
	if (this->ecc.mode==NAND_ECC_HW && this->ecc.size==512 && this->ecc.bytes==7)
	{
		/*
		 * Do we need to try and correct anything ?
		 * We get LP16 and LP17 from byte6[1:0], instead of byte2[1:0] !
		 * We assume that calc_ecc[6] is has the top 6-bits as zero.
		 */
		if (	(read_ecc[0] == calc_ecc[0])			&&
			(read_ecc[1] == calc_ecc[1])			&&
			((read_ecc[2]&0xfc) == (calc_ecc[2]&0xfc))	&&
			((read_ecc[6]&0x03) == calc_ecc[6])		   )
		{
			return 0;		/* ECCs agree, nothing to do */
		}

	#if VERBOSE_ECC
		printf("warning: ECC error detected!  "
			"read_ecc %02x:%02x:%02x:%02x (%c%c%c%c) != "
			"calc_ecc %02x:%02x:%02x:%02x (%c%c%c%c)\n",
			(unsigned)read_ecc[0],
			(unsigned)read_ecc[1],
			(unsigned)read_ecc[2],
			(unsigned)read_ecc[6],
			isprint(read_ecc[0]) ? read_ecc[0] : '.',
			isprint(read_ecc[1]) ? read_ecc[1] : '.',
			isprint(read_ecc[2]) ? read_ecc[2] : '.',
			isprint(read_ecc[6]) ? read_ecc[6] : '.',
			(unsigned)calc_ecc[0],
			(unsigned)calc_ecc[1],
			(unsigned)calc_ecc[2],
			(unsigned)calc_ecc[6],
			isprint(calc_ecc[0]) ? calc_ecc[0] : '.',
			isprint(calc_ecc[1]) ? calc_ecc[1] : '.',
			isprint(calc_ecc[2]) ? calc_ecc[2] : '.',
			isprint(calc_ecc[6]) ? calc_ecc[6] : '.');
	#endif	/* VERBOSE_ECC */

		/*
		 * Put ECC bytes into required structure
		 * We get LP16 and LP17 from byte6[1:0], instead of byte2[1:0] !
		 * We assume that calc_ecc[2] is correct though!
		 */
		read.byte[0] = read_ecc[0];
		read.byte[1] = read_ecc[1];
		read.byte[2] = (read_ecc[2]&0xfc) | (read_ecc[6]&0x03);
		calc.byte[0] = calc_ecc[0];
		calc.byte[1] = calc_ecc[1];
		calc.byte[2] = calc_ecc[2];
		ecc_size = ECC_512;		/* 512 bytes/record */
	}
	else
#endif /* CONFIG_SYS_NAND_ECC_AFM4 */
	{
		printf("ERROR: Can not calculate ECC: Internal Error (mode=%u,size=%u,bytes=%u)\n",
			this->ecc.mode, this->ecc.size, this->ecc.bytes);
		BUG();
		return -1;
	}

	/* correct a 1-bit error (if we can) */
	result = ecc_correct(dat, read, calc, ecc_size);

	/* let the user know if we were able to recover it or not! */
	switch (result)
	{
		case E_D1_CHK:
			printf("info: 1-bit error in data was corrected\n");
			break;
		case E_C1_CHK:
			printf("info: 1-bit error in ECC ignored (data was okay)\n");
			break;
		default:
#if VERBOSE_ECC
			/* QQQ: filter out genuinely ERASED pages - TO DO */
			printf("ERROR: uncorrectable ECC error not corrected!\n");
#endif	/* VERBOSE_ECC */
			break;
	}

	/* return zero if all okay, and -1 if we have an uncorrectable issue */
	if ((result==E_D1_CHK)||(result==E_C1_CHK))
	{
		return 0;	/* okay (correctable) */
	}
	else
	{
		return -1;	/* uncorrectable */
	}
}


#endif /* CONFIG_SYS_NAND_ECC_HW3_128 || CONFIG_SYS_NAND_ECC_AFM4 */


/*****************************************************************************************
 *****************************************************************************************
 *****************		H/W 3/128 Boot-Mode		**************************
 *****************************************************************************************
 *****************************************************************************************/


#ifdef CONFIG_SYS_NAND_ECC_HW3_128	/* for STM "boot-mode" */

	/* for SMALL-page devices */
static struct nand_ecclayout stm_boot_ecclayout_16 = {
#if CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B
	.eccbytes = 16,			/* 16 out of 16 bytes = 100% of OOB */
	.eccpos = {
		 0,  1,  2,  3,		/* 128-byte Record 0: ECC0, ECC1, ECC2, 'B' */
		 4,  5,  6,  7,		/* 128-byte Record 1: ECC0, ECC1, ECC2, 'B' */
		 8,  9, 10, 11,		/* 128-byte Record 2: ECC0, ECC1, ECC2, 'B' */
		12, 13, 14, 15},	/* 128-byte Record 3: ECC0, ECC1, ECC2, 'B' */
	.oobfree = { {0, 0} }		/* No free space in OOB! */
#else	/* CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B */
	.eccbytes = 12,			/* 12 out of 16 bytes = 75% of OOB */
	.eccpos = {
		 0,  1,  2,		/* 128-byte Record 0: ECC0, ECC1, ECC2 */
		 4,  5,  6,		/* 128-byte Record 1: ECC0, ECC1, ECC2 */
		 8,  9, 10,		/* 128-byte Record 2: ECC0, ECC1, ECC2 */
		12, 13, 14}		/* 128-byte Record 3: ECC0, ECC1, ECC2 */
	.oobfree = { {3, 1}, {7, 1}, {11, 1}, {15, 1} }	/* 4 free bytes in the OOB */
#endif	/* CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B */
};

	/* for LARGE-page devices */
static struct nand_ecclayout stm_boot_ecclayout_64 = {
#if CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B
	.eccbytes = 64,			/* 64 out of 64 bytes = 100% of OOB */
	.eccpos = {
		 0,  1,  2,  3,		/* 128-byte Record  0: ECC0, ECC1, ECC2, 'B' */
		 4,  5,  6,  7,		/* 128-byte Record  1: ECC0, ECC1, ECC2, 'B' */
		 8,  9, 10, 11,		/* 128-byte Record  2: ECC0, ECC1, ECC2, 'B' */
		12, 13, 14, 15,		/* 128-byte Record  3: ECC0, ECC1, ECC2, 'B' */
		16, 17, 18, 19,		/* 128-byte Record  4: ECC0, ECC1, ECC2, 'B' */
		20, 21, 22, 23,		/* 128-byte Record  5: ECC0, ECC1, ECC2, 'B' */
		24, 25, 26, 27,		/* 128-byte Record  6: ECC0, ECC1, ECC2, 'B' */
		28, 29, 30, 31,		/* 128-byte Record  7: ECC0, ECC1, ECC2, 'B' */
		32, 33, 34, 35,		/* 128-byte Record  8: ECC0, ECC1, ECC2, 'B' */
		36, 37, 38, 39,		/* 128-byte Record  9: ECC0, ECC1, ECC2, 'B' */
		40, 41, 42, 43,		/* 128-byte Record 10: ECC0, ECC1, ECC2, 'B' */
		44, 45, 46, 47,		/* 128-byte Record 11: ECC0, ECC1, ECC2, 'B' */
		48, 49, 50, 51,		/* 128-byte Record 12: ECC0, ECC1, ECC2, 'B' */
		52, 53, 54, 55,		/* 128-byte Record 13: ECC0, ECC1, ECC2, 'B' */
		56, 57, 58, 59,		/* 128-byte Record 14: ECC0, ECC1, ECC2, 'B' */
		60, 61, 62, 63},	/* 128-byte Record 15: ECC0, ECC1, ECC2, 'B' */
	.oobfree = { {0, 0} }		/* No free space in OOB! */
#else	/* CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B */
	.eccbytes = 48,			/* 48 out of 64 bytes = 75% of OOB */
	.eccpos = {
		 0,  1,  2,		/* 128-byte Record  0: ECC0, ECC1, ECC2 */
		 4,  5,  6,		/* 128-byte Record  1: ECC0, ECC1, ECC2 */
		 8,  9, 10,		/* 128-byte Record  2: ECC0, ECC1, ECC2 */
		12, 13, 14,		/* 128-byte Record  3: ECC0, ECC1, ECC2 */
		16, 17, 18,		/* 128-byte Record  4: ECC0, ECC1, ECC2 */
		20, 21, 22,		/* 128-byte Record  5: ECC0, ECC1, ECC2 */
		24, 25, 26,		/* 128-byte Record  6: ECC0, ECC1, ECC2 */
		28, 29, 30,		/* 128-byte Record  7: ECC0, ECC1, ECC2 */
		32, 33, 34,		/* 128-byte Record  8: ECC0, ECC1, ECC2 */
		36, 37, 38,		/* 128-byte Record  9: ECC0, ECC1, ECC2 */
		40, 41, 42,		/* 128-byte Record 10: ECC0, ECC1, ECC2 */
		44, 45, 46,		/* 128-byte Record 11: ECC0, ECC1, ECC2 */
		48, 49, 50,		/* 128-byte Record 12: ECC0, ECC1, ECC2 */
		52, 53, 54,		/* 128-byte Record 13: ECC0, ECC1, ECC2 */
		56, 57, 58,		/* 128-byte Record 14: ECC0, ECC1, ECC2 */
		60, 61, 62},		/* 128-byte Record 15: ECC0, ECC1, ECC2 */
	.oobfree = {
		{ 3, 1}, { 7, 1}, {11, 1}, {15, 1},
		{19, 1}, {23, 1}, {27, 1}, {31, 1},
		{35, 1}, {39, 1}, {43, 1}, {47, 1},
		{51, 1}, {55, 1}, {59, 1}, {63, 1} }	/* 16 free bytes in the OOB */
#endif	/* CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B */
};


/*
 * define a structure to hold the fields in the "struct mtd_info"
 * and "struct nand_chip" structures that we need to over-write
 * to allow 2 (incompatible) ECC configurations to exist on
 * the same physical NAND flash device at the same time!
 *
 * This is required as some of the STi7xxx devices, have a property
 * that when in NAND "boot-mode" (i.e. boot-from-NAND) that an ECC
 * configuration of 3 ECC bytes per 128 byte record *must* be used.
 * However, when not in "boot-mode", less aggressive ECC configuration
 * of either 3 ECC bytes per 256 or 512 bytes may be used.
 *
 * When using this "hybrid" of 2 different ECC configurations on the
 * same physical device, we use 2 instances of this structure:
 *	1) based on the normal (default) configuration (e.g. S/W ECC)
 *	2) the "boot-mode" 'special' with 3/128 H/W compatibility.
 *
 * Various functions are intercepted to ensure that the mtd_info
 * and nand_chip structures always have the correct "view" of the ECC
 * configurations, then the default functions can be safely used as is.
 */
struct stm_mtd_nand_ecc
{
	struct		/* holds differences in "struct nand_chip" */
	{
		struct	/* holds differences in "struct nand_ecc_ctrl" */
		{
			int			mode;
			int			size;
			int			bytes;		/* for ECC + ID tags */
			int			steps;
			int			total;
			struct nand_ecclayout	*layout;
			void (*hwctl)(struct mtd_info *mtd, int mode);
			int (*calculate)(struct mtd_info *, const u_char *, u_char *);
			int (*correct)(struct mtd_info *, u_char *, u_char *, u_char *);
		} ecc;
	}	nand;
	struct	/* holds differences in "struct mtd_info" */
	{
		u_int32_t		oobavail;
		struct nand_ecclayout	*ecclayout;
	}	mtd;
};


static struct stm_mtd_nand_ecc default_ecc;	/* ECC diffs for the normal SW case */
static struct stm_mtd_nand_ecc special_ecc;	/* ECC diffs for the "special" hybrid case */
static int done_ecc_info = 0;			/* are the 2 ECC structures initialized ? */


/*
 * fill in the "default_ecc" and "special_ecc" structures.
 */
static void initialize_ecc_diffs (
	const struct mtd_info * const mtd)
{
	const struct nand_chip * const this = (struct nand_chip *)(mtd->priv);
	struct nand_ecclayout * layout = NULL;

	/* choose the correct OOB info struct, depending on the OOB size */
	if (mtd->oobsize == 64)				/* large page device ? */
		layout = &stm_boot_ecclayout_64;
	else if (mtd->oobsize == 16)			/* small page device ? */
		layout = &stm_boot_ecclayout_16;
	else						/* unknown ? */
		BUG();

	BUG_ON (!this->ecc.calculate || !this->ecc.correct);

	/* fill in "default_ecc" from the current "live" (default) structures */
	default_ecc.nand.ecc.mode	= this->ecc.mode;
	default_ecc.nand.ecc.size	= this->ecc.size;
	default_ecc.nand.ecc.bytes	= this->ecc.bytes;
	default_ecc.nand.ecc.steps	= this->ecc.steps;
	default_ecc.nand.ecc.total	= this->ecc.total;
	default_ecc.nand.ecc.layout	= this->ecc.layout;
	default_ecc.nand.ecc.hwctl	= this->ecc.hwctl;
	default_ecc.nand.ecc.calculate	= this->ecc.calculate;
	default_ecc.nand.ecc.correct	= this->ecc.correct;
	default_ecc.mtd.oobavail	= mtd->oobavail;
	default_ecc.mtd.ecclayout	= mtd->ecclayout;

	/* fill in "special_ecc" for our special "hybrid" ECC paradigm */
	special_ecc.nand.ecc.mode	= NAND_ECC_HW;
	special_ecc.nand.ecc.size	= 128;
#if CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B
	special_ecc.nand.ecc.bytes	= 3 + 1;	/* ECC 3+1/128, '+1' for 'B' */
#else	/* CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B */
	special_ecc.nand.ecc.bytes	= 3;		/* ECC 3/128 */
#endif	/* CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B */
	special_ecc.nand.ecc.steps	= mtd->writesize / special_ecc.nand.ecc.size;
	special_ecc.nand.ecc.total	= special_ecc.nand.ecc.steps * special_ecc.nand.ecc.bytes;
	special_ecc.nand.ecc.layout	= layout;
	special_ecc.nand.ecc.hwctl	= stm_nand_hwctl;
	special_ecc.nand.ecc.calculate	= stm_nand_calculate;
	special_ecc.nand.ecc.correct	= stm_nand_correct;
	if (this->options & NAND_BUSWIDTH_16) {
		special_ecc.mtd.oobavail= mtd->oobsize - (layout->eccbytes + 2);
		special_ecc.mtd.oobavail= special_ecc.mtd.oobavail & ~0x01;
	} else {
		special_ecc.mtd.oobavail= mtd->oobsize - (layout->eccbytes + 1);
	}
	special_ecc.mtd.ecclayout	= layout;
}


/*
 * Make the "live" MTD structures use the ECC configuration
 * as described in the passed "diffs" structure.
 */
static void set_ecc_diffs (
	struct mtd_info * const mtd,
	const struct stm_mtd_nand_ecc * const diffs)
{
	struct nand_chip * const this = (struct nand_chip *)(mtd->priv);

	this->ecc.mode		= diffs->nand.ecc.mode;
	this->ecc.size		= diffs->nand.ecc.size;
	this->ecc.bytes		= diffs->nand.ecc.bytes;
	this->ecc.steps		= diffs->nand.ecc.steps;
	this->ecc.total		= diffs->nand.ecc.total;
	this->ecc.layout	= diffs->nand.ecc.layout;
	this->ecc.hwctl		= diffs->nand.ecc.hwctl;
	this->ecc.calculate	= diffs->nand.ecc.calculate;
	this->ecc.correct	= diffs->nand.ecc.correct;

	mtd->oobavail		= diffs->mtd.oobavail;
	mtd->ecclayout		= diffs->mtd.ecclayout;

#if VERBOSE_ECC
	printf("info: switching to NAND \"%s\" ECC (%u/%u)\n",
		(diffs==&special_ecc) ? "BOOT-mode" : "NON-boot-mode",
		this->ecc.bytes,
		this->ecc.size);
#endif	/* VERBOSE_ECC */
}


static int set_ecc_mode (
	struct mtd_info * const mtd,
	const loff_t addr,
	const size_t len)
{
	struct nand_chip * const this = (struct nand_chip *)(mtd->priv);

	if (!done_ecc_info)		/* first time ? */
	{
		initialize_ecc_diffs (mtd);
		done_ecc_info = 1;	/* do not do this again */
	}

	/* do we need to switch ECC mode ? */
	if ( addr >= CONFIG_SYS_NAND_STM_BOOT_MODE_BOUNDARY )
	{	/* entire range is *not* in "boot-mode" (i.e. default ECC) */
		if (this->ecc.mode==NAND_ECC_HW && this->ecc.size==128 && this->ecc.bytes==ECC_BYTES)
		{	/* we are in the wrong ECC mode, so change */
			set_ecc_diffs (mtd, &default_ecc);
		}
	}
	else if ( addr + len <= CONFIG_SYS_NAND_STM_BOOT_MODE_BOUNDARY )
	{	/* entire range is in "boot-mode" (i.e. 3 bytes of ECC per 128 record */
		if (!(this->ecc.mode==NAND_ECC_HW && this->ecc.size==128 && this->ecc.bytes==ECC_BYTES))
		{	/* we are in the wrong ECC mode, so change */
			set_ecc_diffs (mtd, &special_ecc);
		}
	}
	else
	{	/* the range is split over *both* "boot" and "non-boot" modes! */
		printf("ERROR: NAND range crosses \"boot-mode\" boundary (0x%08lx)\n",
			CONFIG_SYS_NAND_STM_BOOT_MODE_BOUNDARY);
		return -EINVAL;
	}

	return 0;	/* success */
}


	/*
	 * function pointers to original (low-level) functions in
	 * struct mtd_info, which are not exported from nand_base.c,
	 * but a pointer to them is initialized in nand_scan_tail().
	 */
static int (*fn_mtd_read)
		(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, uint8_t *buf) = NULL;
static int (*fn_mtd_read_oob)
		(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops) = NULL;
static int (*fn_mtd_write)
		(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const uint8_t *buf) = NULL;
static int (*fn_mtd_write_oob)
		(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops) = NULL;


static int stm_boot_read (struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, u_char * buf)
{
	int result;

	result = set_ecc_mode (mtd, from, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = (*fn_mtd_read) (mtd, from, len, retlen, buf);
	}

	return result;
}


static int stm_boot_read_oob (struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops)
{
	int result;

	result = set_ecc_mode (mtd, from, ops->len);
	if (result != 0)
	{
		ops->retlen = 0;
	}
	else
	{
		result = (*fn_mtd_read_oob) (mtd, from, ops);
	}

	return result;
}


static int stm_boot_write (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf)
{
	int result;

	result = set_ecc_mode (mtd, to, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = (*fn_mtd_write) (mtd, to, len, retlen, buf);
	}

	return result;
}


static int stm_boot_write_oob (struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops)
{
	int result;

	result = set_ecc_mode (mtd, to, ops->len);
	if (result != 0)
	{
		ops->retlen = 0;
	}
	else
	{
		result = (*fn_mtd_write_oob) (mtd, to, ops);
	}

	return result;
}


#endif	/* CONFIG_SYS_NAND_ECC_HW3_128 */


/*****************************************************************************************
 *****************************************************************************************
 *****************		H/W 4+3/512 AFM4 ECC		**************************
 *****************************************************************************************
 *****************************************************************************************/


#if defined(CONFIG_SYS_NAND_ECC_AFM4)	/* for STM AFM4 (4+3/512) ECC compatibility */


	/* for SMALL-page devices */
static struct nand_ecclayout stm_afm4_ecclayout_16 = {
	.eccbytes = 7,			/* 7 out of 16 bytes of OOB */
	.eccpos = {			/* { HW_ECC0, HW_ECC1, HW_ECC2, 'A', 'F', 'M', SW_ECC } */
		0, 1, 2, 3, 4, 5, 6,	/* 512-byte Record 0 */
	},
	.oobfree = { { 7, 9} }		/* 9 free bytes in the OOB */
};

	/* for LARGE-page devices */
static struct nand_ecclayout stm_afm4_ecclayout_64 = {
	.eccbytes = 28,			/* 28 out of 64 bytes of OOB */
	.eccpos = {			/* { HW_ECC0, HW_ECC1, HW_ECC2, 'A', 'F', 'M', SW_ECC } */
		0,   1,  2,  3,  4,  5,  6,	/* 512-byte Record 0 */
		16, 17, 18, 19, 20, 21, 22,	/* 512-byte Record 1 */
		32, 33, 34, 35, 36, 37, 38,	/* 512-byte Record 2 */
		48, 49, 50, 51, 52, 53, 54,	/* 512-byte Record 3 */
	},
	.oobfree = { {7, 9}, {23, 9}, {39, 9}, {55, 9} }	/* 9*4 free bytes in the OOB */
};


#endif /* CONFIG_SYS_NAND_ECC_AFM4 */


/*****************************************************************************************
 *****************************************************************************************
 *****************		H/W BCH (multi-bit ECC)		**************************
 *****************************************************************************************
 *****************************************************************************************/


#if defined(CONFIG_SYS_ST40_NAND_USE_BCH)	/* for H/W BCH ("multi-bit ECC") driver */


	/* for LARGE-page devices */
static struct nand_ecclayout stm_bch_ecclayout_64 = {
	.eccbytes = 64,			/* 64 out of 64 bytes = 100% of OOB */
	.eccpos = {
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = { {0, 0} }		/* No free space in OOB! */
};


#endif /* CONFIG_SYS_ST40_NAND_USE_BCH */


/*****************************************************************************************
 *****************************************************************************************
 *****************		Generic "common" code		**************************
 *****************************************************************************************
 *****************************************************************************************/


static int stm_nand_default_bbt (struct mtd_info * const mtd)
{
	struct nand_chip * const this = (struct nand_chip *)(mtd->priv);

	/* over-write the default "badblock_pattern", with our one */
	/* choose the correct pattern struct, depending on the OOB size */
	if (mtd->oobsize > 16)
		this->badblock_pattern = &stm_nand_badblock_pattern_64;	/* LARGE-page */
	else
		this->badblock_pattern = &stm_nand_badblock_pattern_16;	/* SMALL-page */

	/*
	 * For the "boot-mode+B" ECC (i.e. 3+1/128) scheme, and for
	 * the "AFM4" ECC (i.e. 4+3/512) scheme, then we wish to
	 * be compatible with the way linux scans NAND devices.
	 * So, we do not want to scan all pages, nor all the in-band data!
	 * Play with the options to make it so...
	 */
#if CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B || CONFIG_SYS_STM_NAND_AFM4_ECC_WITH_AFM
	this->badblock_pattern->options &= ~(NAND_BBT_SCANEMPTY|NAND_BBT_SCANALLPAGES);
#endif
#if CONFIG_SYS_STM_NAND_BOOT_MODE_ECC_WITH_B	/* Additional "B" tag in OOB ? */
	this->badblock_pattern->options |= NAND_BBT_SCANSTMBOOTECC;
#endif
#if CONFIG_SYS_STM_NAND_AFM4_ECC_WITH_AFM	/* Additional "AFM" tag in OOB ? */
	this->badblock_pattern->options |= NAND_BBT_SCANSTMAFMECC;
#endif

	/* now call the generic BBT function */
	return nand_default_bbt (mtd);
}


	/*
	 * This function is called *before* nand_scan_ident(),
	 * and hence, before nand_scan() (or stm_nand_scan()).
	 * This function is called directly from board_nand_init().
	 */
extern void stm_default_board_nand_init(
	struct nand_chip * const nand,
	void (*cmd_ctrl)(struct mtd_info *mtdinfo, int dat, unsigned int ctrl),
	int (*dev_ready)(struct mtd_info *mtd))
{
#if defined(CONFIG_SYS_ST40_NAND_USE_HAMMING) || defined(CONFIG_SYS_ST40_NAND_USE_BCH)
	struct mtd_info * const mtd = (struct mtd_info *)(nand->priv);
#endif

	/* free up private pointer for the real driver */
	nand->priv = NULL;

	nand->options       = NAND_NO_AUTOINCR;
#if 1	/* Enable to use a NAND-resident (non-volatile) Bad Block Table (BBT) */
	nand->options      |= NAND_USE_FLASH_BBT;
#endif
	/* override scan_bbt(), even if not using a Bad Block Table (BBT) */
	nand->scan_bbt      = stm_nand_default_bbt;

#if defined(CONFIG_SYS_ST40_NAND_USE_BIT_BANGING)	/* use the S/W "bit-banging" driver */
	nand->cmd_ctrl      = cmd_ctrl;
	nand->dev_ready     = dev_ready;
#elif defined(CONFIG_SYS_ST40_NAND_USE_HAMMING)		/* for H/W Hamming ("flex") driver */
	stm_flex_init_nand(mtd, nand);
#elif defined(CONFIG_SYS_ST40_NAND_USE_BCH)		/* for H/W BCH ("multi-bit ECC") driver */
	stm_bch_init_nand(mtd, nand);
#endif /* CONFIG_SYS_ST40_NAND_USE_BIT_BANGING */

#if defined(CONFIG_SYS_NAND_ECC_AFM4)			/* for STM AFM4 ECC compatibility */
	nand->ecc.hwctl     = stm_nand_hwctl;
	nand->ecc.correct   = stm_nand_correct;
	nand->ecc.calculate = stm_nand_calculate;
#endif /* CONFIG_SYS_NAND_ECC_AFM4 */

	/*
	 * Only enable the following to use a (volatile) RAM-based
	 * (not NAND-resident) Bad Block Table (BBT).
	 * This is *not* recommended! A NAND-resident BBT is recommended.
	 */
#if 0
	nand->options      &= ~NAND_USE_FLASH_BBT;
#endif
}


	/*
	 * This function is called *after* nand_scan_ident(),
	 * but *before* nand_scan_tail().
	 */
extern void stm_nand_scan_post_ident(
	struct mtd_info * const mtd)
{
	struct nand_chip * const nand = mtd->priv;

#if defined(CONFIG_SYS_NAND_ECC_AFM4) || defined(CONFIG_SYS_ST40_NAND_USE_BCH)
#if defined(CONFIG_SYS_NAND_ECC_AFM4)	/* for STM AFM4 (4+3/512) ECC compatibility */
	if (mtd->oobsize == 64)				/* large page device ? */
		nand->ecc.layout = &stm_afm4_ecclayout_64;
	else if (mtd->oobsize == 16)			/* small page device ? */
		nand->ecc.layout = &stm_afm4_ecclayout_16;
#else				/* for H/W BCH ("multi-bit ECC") driver */
	/*
	 * Note: for BCH we can just use a single "one-size-fits-all"
	 * structure (stm_bch_ecclayout_64), as nand->ecc.layout, for any NAND
	 * device which has a minimum of 64 bytes of OOB per page.
	 * Only the nand->ecc.layout.oobfree[0] elements *should* be accessed.
	 */
	if (mtd->oobsize >= 64)				/* (at least a) large page device ? */
		nand->ecc.layout = &stm_bch_ecclayout_64;
#endif /* CONFIG_SYS_NAND_ECC_AFM4 */
	else						/* unknown ? */
	{
		printf("Error: unexpected OOB size of %u bytes\n", mtd->oobsize);
		BUG();
	}
#endif /* CONFIG_SYS_NAND_ECC_AFM4 || CONFIG_SYS_ST40_NAND_USE_BCH */

		/*
		 * Ensure sizeof(OOB) is not too big!
		 * Otherwise we will get some horrible buffer bounds
		 * violations, and the terrible madness that follows ...
		 */
	if (mtd->oobsize > NAND_MAX_OOBSIZE)
	{
		printf("Error: OOB size of %u exceeds maximum of %u bytes\n", mtd->oobsize, NAND_MAX_OOBSIZE);
		BUG();
	}
		/* Similarly, ensure sizeof(PAGE) is not too big! */
	if (mtd->writesize > NAND_MAX_PAGESIZE)
	{
		printf("Error: PAGE size of %u exceeds maximum of %u bytes\n", mtd->writesize, NAND_MAX_PAGESIZE);
		BUG();
	}

#if defined(CONFIG_SYS_NAND_ECC_AFM4)		/* for STM AFM4 ECC compatibility */
	nand->ecc.mode      = NAND_ECC_HW;	/* compatible with AFM4 (4+3/512) ECC */
	nand->ecc.size      = 512;
	nand->ecc.bytes     = 7;
#elif defined(CONFIG_SYS_ST40_NAND_USE_BCH)	/* for H/W BCH ("multi-bit ECC") driver */
	nand->ecc.mode      = NAND_ECC_HW;	/* use ST's (off-die) on-SoC H/W ECC engine */
	nand->ecc.size      = 1024;		/* BCH ECC only processes 1KiB sectors at a time */
#	if defined(CONFIG_SYS_ST40_NAND_USE_BCH_18_BIT_ECC)
	nand->ecc.bytes     = 32;		/* 18-bits of BCH ECC needs 32-Bytes/1KiB sector */
#	elif defined(CONFIG_SYS_ST40_NAND_USE_BCH_30_BIT_ECC)
	nand->ecc.bytes     = 54;		/* 30-bits of BCH ECC needs 54-Bytes/1KiB sector */
#	elif defined(CONFIG_SYS_ST40_NAND_USE_BCH_NO_ECC)
	nand->ecc.bytes     = 0;		/* No ECC -- not recommended! */
#	else
#	error Please specific the BCH ECC scheme to use (CONFIG_SYS_ST40_NAND_USE_BCH_xxx_ECC)
#	endif /* CONFIG_SYS_ST40_NAND_USE_BCH_xxx_ECC */
#else
	nand->ecc.mode      = NAND_ECC_SOFT;	/* default is S/W 3/256 ECC */
#endif /* CONFIG_SYS_NAND_ECC_AFM4 */
}


	/*
	 * This function is called *after* nand_scan_tail().
	 */
extern void stm_nand_scan_post_tail(
	struct mtd_info * const mtd)
{
	struct nand_chip * const nand = mtd->priv;

#if defined(CONFIG_SYS_NAND_ECC_HW3_128)	/* for STM "boot-mode" ECC */
	/* first, copy original bottom-level function pointers safely */
	fn_mtd_read      = mtd->read;
	fn_mtd_read_oob  = mtd->read_oob;
	fn_mtd_write     = mtd->write;
	fn_mtd_write_oob = mtd->write_oob;
	/* then, replace them all with encapsulated "wrapper" functions */
	mtd->read        = stm_boot_read;
	mtd->read_oob    = stm_boot_read_oob;
	mtd->write       = stm_boot_write;
	mtd->write_oob   = stm_boot_write_oob;
#endif /* CONFIG_SYS_NAND_ECC_HW3_128 */

		/*
		 * Ensure we do have enough OOB bytes for the chosen ECC scheme!
		 */
	if (nand->ecc.total > mtd->oobsize)
	{
		printf("Error: Chosen ECC needs minimum OOB size of %u, but OOB is only %u bytes/page\n",
			nand->ecc.total, mtd->oobsize);
		BUG();
	}

	/*
	 * BBT bit-maps are stored as 2-bits per block, or equivalently, as 4 blocks/byte.
	 * Currently, we assume in the BCH driver that the entire BBT bit-map will
	 * fit in one single page (the first page) of the block containing the BBT.
	 * Let the user know if this assumption is false.
	 * QQQ - remove this restriction (later), *if* needed!
	 */
#if defined(CONFIG_SYS_ST40_NAND_USE_BCH)	/* for H/W BCH ("multi-bit ECC") driver */
	const int blocks_per_device = mtd->size >> nand->phys_erase_shift;
	if (blocks_per_device/4 > mtd->writesize)
	{
		/* We now know, we need to implement multi-page BBT support... */
		printf("Error: BBT is too big to fit in a single (%u byte) page\n",
			mtd->writesize);
		BUG();
	}
#endif /* CONFIG_SYS_ST40_NAND_USE_BCH */
}


#endif	/* CONFIG_CMD_NAND */
