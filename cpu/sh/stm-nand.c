extern volatile int sean_qqq;			/* QQQ - DELETE */
volatile int sean_qqq = 0;			/* QQQ - DELETE */
#define QQQ_START	{ sean_qqq++; }		/* QQQ - DELETE */
#define QQQ_END		{ sean_qqq--; }		/* QQQ - DELETE */
#define QQQ_TEST	{ if (sean_qqq != 1)						\
				printf("WARNING!!! Unexpected entry in %s() %s:%u\n",	\
					__FUNCTION__,					\
					__FILE__,					\
					__LINE__);					\
			}

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
#include <asm/errno.h>


/*
 * Define the bad/good block scan pattern which are used while scanning
 * the NAND device for any factory marked good/bad blocks.
 *
 * For small page devices, factory bad block markers are in byte 5.
 * For large page devices, factory bad block markers are in bytes 0 & 1.
 * Any bit in those bytes being a zero, implies the block is bad.
 *
 * We only scan the first & 2nd page in each block.
 * However, we do also check the entire data area in those 2 pages,
 * to see if they are properly erased. Improperly erased pages
 * result in the entire bock also being treated as bad.
 *
 * Note: using "SCANALLPAGES" takes a significant performance hit!
 */
static uint8_t scan_pattern[] = { 0xffu, 0xffu };

struct nand_bbt_descr stm_nand_badblock_pattern = {
	.pattern = scan_pattern,
/*	.options = NAND_BBT_SCANEMPTY | NAND_BBT_SCANALLPAGES */
	.options = NAND_BBT_SCANEMPTY | NAND_BBT_SCAN2NDPAGE,
#if defined(CFG_NAND_SMALLPAGE)
	.offs = 5,	/* Byte 5 */
	.len = 1,
#elif defined(CFG_NAND_LARGEPAGE)
	.offs = 0,	/* Bytes 0-1 */
	.len = 2,
#else
#error "Either CFG_NAND_LARGEPAGE or CFG_NAND_SMALLPAGE must be defined!"
#endif
};


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
 * same device, we use 2 instances of this structure:
 *	1) based on the normal (default) configuration (e.g. S/W ECC)
 *	2) the "boot-mode" 'special' with 3/128 H/W compatibility.
 *
 * Various functions are intercepted to ensure that the mtd_info
 * and nand_chip structures always have the correct "view" of the ECC
 * configurations, then the default functions can be safely used as is.
 */
struct stm_mtd_nand_ecc
{
	struct	/* holds differences in "struct nand_chip" */
	{
		int			eccmode;
		int			eccsize;
		int			eccsteps;
		struct nand_oobinfo	*autooob;
		int (*calculate_ecc)(struct mtd_info *, const u_char *, u_char *);
		int (*correct_data)(struct mtd_info *, u_char *, u_char *, u_char *);
	}	nand;
	struct	/* holds differences in "struct mtd_info" */
	{
		u_int32_t		oobavail;
		u_int32_t		eccsize;
		struct nand_oobinfo	oobinfo;
	}	mtd;
};

static struct stm_mtd_nand_ecc default_ecc;	/* ECC diffs for the normal SW case */
static struct stm_mtd_nand_ecc special_ecc;	/* ECC diffs for the "special" hybrid case */
static int done_ecc_info = 0;			/* are the 2 ECC structures initialized ? */


#define isprint(x)	( ((x)>=0x20u) && ((x)<0x7fu) )			/* QQQ - DELETE */
extern int stm_nand_calculate_ecc (
	struct mtd_info * const mtd,
	const u_char * const dat,
	u_char * const ecc_code)
{
	const struct nand_chip const * this = mtd->priv;

#if 0									/* QQQ - DELETE */
	printf("QQQ: %s(mtd=%p, dat=%p, ecc_code=%p)\n", __FUNCTION__, mtd, dat, ecc_code);	/* QQQ - DELETE */
#endif									/* QQQ - DELETE */

#if 1	/* QQQ - DELETE */
	if (this->eccmode!=NAND_ECC_HW3_128)
	{
		printf("ERROR: Can not calculate ECC: Internal Error (eccmode=%u)\n",
			this->eccmode);
		ecc_code[0] = 'B';
		ecc_code[1] = 'A';
		ecc_code[2] = 'D';
		return -1;	/* Note: caller ignores this value! */
	}
	else if ((((unsigned long)dat)%4)!=0)	/* data *must* be 4-bytes aligned */
	{
		printf("ERROR: Can not calculate ECC: data (%08lx) must be 4-byte aligned!\n",
			(unsigned long)dat);
		ecc_code[0] = 'B';
		ecc_code[1] = 'A';
		ecc_code[2] = 'D';
		return -1;	/* Note: caller ignores this value! */
	}
	else
#endif	/* QQQ -DELETE */
	{	/* calculate 3 ECC bytes per 128 bytes of data */
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
	ecc_t read, calc;
	enum ecc_check result;
	const struct nand_chip const * this = mtd->priv;

#if 0									/* QQQ - DELETE */
	printf("QQQ: %s(mtd=%p, dat=%p, read_ecc=%p, calc_ecc=%p)\n",
		__FUNCTION__, mtd, dat, read_ecc, calc_ecc);		/* QQQ - DELETE */
#endif									/* QQQ - DELETE */

#if 1	/* QQQ - DELETE */
	if (this->eccmode!=NAND_ECC_HW3_128)
	{
		printf("ERROR: Can not correct ECC: Internal Error (eccmode=%u)\n",
			this->eccmode);
		return -1;
	}
#endif	/* QQQ -DELETE */

	/* do we need to try and correct anything ? */
	if (    (read_ecc[0] == calc_ecc[0]) &&
		(read_ecc[1] == calc_ecc[1]) &&
		(read_ecc[2] == calc_ecc[2])    )
	{
		return 0;		/* ECCs agree, nothing to do */
	}

#if 1							/* QQQ - DELETE */
	printf("ERROR: ECC detected!\n");		/* QQQ - DELETE */
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

	/* put ECC bytes into required structure */
	read.byte[0] = read_ecc[0];
	read.byte[1] = read_ecc[1];
	read.byte[2] = read_ecc[2];
	calc.byte[0] = calc_ecc[0];
	calc.byte[1] = calc_ecc[1];
	calc.byte[2] = calc_ecc[2];

	/* correct a 1-bit error (if we can) */
	result = ecc_correct(dat, read, calc, ECC_128);

#if 1	/* QQQ - DELETE */
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
			printf("ERROR: uncorrectable ECC error not corrected!\n");
	}
#endif	/* QQQ - DELETE */

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

/*
 * fill in the "default_ecc" and "special_ecc" structures.
 */
static void initialize_ecc_diffs (
	const struct mtd_info * const mtd,
	struct nand_oobinfo * const autooob)
{
	const struct nand_chip * const this = (struct nand_chip *)(mtd->priv);

	/* fill in "default_ecc" from the current "live" (default) structures */
	default_ecc.nand.eccmode	= this->eccmode;
	default_ecc.nand.eccsize	= this->eccsize;
	default_ecc.nand.eccsteps	= this->eccsteps;
	default_ecc.nand.autooob	= this->autooob;
	default_ecc.nand.calculate_ecc	= this->calculate_ecc;
	default_ecc.nand.correct_data	= this->correct_data;
	default_ecc.mtd.oobavail	= mtd->oobavail;
	default_ecc.mtd.eccsize	= mtd->eccsize;
	memcpy(&default_ecc.mtd.oobinfo, &mtd->oobinfo, sizeof(struct nand_oobinfo));

	/* fill in "special_ecc" for our special "hybrid" ECC paradigm */
	special_ecc.nand.eccmode	= NAND_ECC_HW3_128;
	special_ecc.nand.eccsize	= 128;
	special_ecc.nand.eccsteps	= mtd->oobblock / special_ecc.nand.eccsize;
	special_ecc.nand.autooob	= autooob;
	special_ecc.nand.calculate_ecc	= stm_nand_calculate_ecc;
	special_ecc.nand.correct_data	= stm_nand_correct_data;
	if (this->options & NAND_BUSWIDTH_16) {
		special_ecc.mtd.oobavail= mtd->oobsize - (autooob->eccbytes + 2);
		special_ecc.mtd.oobavail= special_ecc.mtd.oobavail & ~0x01;
	} else {
		special_ecc.mtd.oobavail= mtd->oobsize - (autooob->eccbytes + 1);
	}
	special_ecc.mtd.eccsize		= special_ecc.nand.eccsize;
	memcpy(&special_ecc.mtd.oobinfo, autooob, sizeof(struct nand_oobinfo));
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

	this->eccmode		= diffs->nand.eccmode;
	this->eccsize		= diffs->nand.eccsize;
	this->eccsteps		= diffs->nand.eccsteps;
	this->autooob		= diffs->nand.autooob;
	this->calculate_ecc	= diffs->nand.calculate_ecc;
	this->correct_data	= diffs->nand.correct_data;

	mtd->oobavail		= diffs->mtd.oobavail;
	mtd->eccsize		= diffs->mtd.eccsize;
	memcpy(&mtd->oobinfo, &diffs->mtd.oobinfo, sizeof(struct nand_oobinfo));

	/* also, we need to reinitialize oob_buf */
	this->oobdirty		= 1;
#if 1	/* QQQ - DELETE */
	printf("QQQ: switching to NAND \"%s\" ECC (%u/%u)\n",
		(diffs==&special_ecc) ? "BOOT-mode" : "NON-boot-mode",
		this->eccbytes,
		this->eccsize);
#endif	/* QQQ - DELETE */
}

static int set_ecc_mode (
	struct mtd_info * const mtd,
	const loff_t addr,
	const size_t len)
{
	struct nand_chip * const this = (struct nand_chip *)(mtd->priv);

#if 0	/* QQQ - DELETE */
	printf("QQQ: %s(addr=0x%08x, len=0x%x)\n", __FUNCTION__, (unsigned long)addr, len);
#endif	/* QQQ - DELETE */

	if (!done_ecc_info)		/* first time ? */
	{
		initialize_ecc_diffs (mtd, &stm_nand_oobinfo);
		done_ecc_info = 1;	/* do not do this again */
	}

	/* do we need to switch ECC mode ? */
	if ( addr >= CFG_NAND_STM_BOOT_MODE_BOUNDARY )
	{	/* entire range is *not* in "boot-mode" (i.e. default ECC) */
		if (this->eccmode == NAND_ECC_HW3_128)
		{	/* we are in the wrong ECC mode, so change */
			set_ecc_diffs (mtd, &default_ecc);
		}
	}
	else if ( addr + len <= CFG_NAND_STM_BOOT_MODE_BOUNDARY )
	{	/* entire range is in "boot-mode" (i.e. 3 bytes of ECC per 128 record */
		if (this->eccmode != NAND_ECC_HW3_128)
		{	/* we are in the wrong ECC mode, so change */
			set_ecc_diffs (mtd, &special_ecc);
		}
	}
	else
	{	/* the range is split over *both* "boot" and "non-boot" modes! */
		printf("ERROR: NAND range crosses \"boot-mode\" boundary (0x%08x)\n",
			CFG_NAND_STM_BOOT_MODE_BOUNDARY);
		return -EINVAL;
	}

	return 0;	/* success */
}


extern void stm_nand_enable_hwecc (
	struct mtd_info *mtd,
	int mode)
{
	/* do nothing - we are only emulating HW in SW */
}


extern int stm_nand_read (struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, u_char * buf)
{
	int result;
QQQ_START
	result = set_ecc_mode (mtd, from, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_read_ecc (mtd, from, len, retlen, buf, NULL, NULL);
	}
QQQ_END
	return result;
}


extern int stm_nand_read_ecc (struct mtd_info *mtd, loff_t from, size_t len,
	size_t * retlen, u_char * buf, u_char * eccbuf, struct nand_oobinfo *oobsel)
{
	int result;
QQQ_START
	result = set_ecc_mode (mtd, from, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_read_ecc (mtd, from, len, retlen, buf, eccbuf, oobsel);
	}
QQQ_END
	return result;
}


extern int stm_nand_read_oob (struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, u_char * buf)
{
	int result;
QQQ_START
	result = set_ecc_mode (mtd, from, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_read_oob (mtd, from, len, retlen, buf);
	}
QQQ_END
	return result;
}


extern int stm_nand_write (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf)
{
	int result;
QQQ_START
	result = set_ecc_mode (mtd, to, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_write_ecc (mtd, to, len, retlen, buf, NULL, NULL);
	}
QQQ_END
	return result;
}


extern int stm_nand_write_ecc (struct mtd_info *mtd, loff_t to, size_t len,
	size_t * retlen, const u_char * buf, u_char * eccbuf, struct nand_oobinfo *oobsel)
{
	int result;
QQQ_START
	result = set_ecc_mode (mtd, to, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_write_ecc (mtd, to, len, retlen, buf, eccbuf, oobsel);
	}
QQQ_END
	return result;
}


extern int stm_nand_write_oob (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char *buf)
{
	int result;
QQQ_START
	result = set_ecc_mode (mtd, to, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_write_oob (mtd, to, len, retlen, buf);
	}
QQQ_END
	return result;
}


#endif	/* CFG_NAND_ECC_HW3_128 */


#endif	/* CONFIG_CMD_NAND */
