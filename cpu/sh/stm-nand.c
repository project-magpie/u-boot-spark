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
#include <malloc.h>


#if defined(CONFIG_CMD_NAND)

#include <nand.h>
#include <asm/stm-nand.h>
#include <asm/ecc.h>
#include <asm/errno.h>
#include <asm/st40reg.h>
#include <asm/io.h>
#include <asm/socregs.h>
#include <asm/cache.h>


#define isprint(x)	( ((x)>=0x20u) && ((x)<0x7fu) )


#define DEBUG_FLEX		0	/* Enable additional debugging of the FLEX controller */
#define VERBOSE_ECC		0	/* Enable for verbose ECC information  */


#ifdef CFG_NAND_ECC_HW3_128	/* for STM "boot-mode" */
#if !defined(CFG_STM_NAND_BOOT_MODE_ECC_WITH_B)
#	define CFG_STM_NAND_BOOT_MODE_ECC_WITH_B	1	/* Enable 'B' tagging */
#endif	/* CFG_STM_NAND_BOOT_MODE_ECC_WITH_B */
#endif	/* CFG_NAND_ECC_HW3_128 */


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

struct nand_bbt_descr stm_nand_badblock_pattern_16 = {
	.pattern = scan_pattern,
	.options = NAND_BBT_SCANEMPTY /* | NAND_BBT_SCANALLPAGES */ | NAND_BBT_SCAN2NDPAGE,
	.offs = 5,	/* Byte 5 */
	.len = 1
};

struct nand_bbt_descr stm_nand_badblock_pattern_64 = {
	.pattern = scan_pattern,
	.options = NAND_BBT_SCANEMPTY /* | NAND_BBT_SCANALLPAGES */ | NAND_BBT_SCAN2NDPAGE,
	.offs = 0,	/* Bytes 0-1 */
	.len = 2
};


#ifdef CFG_NAND_ECC_HW3_128	/* for STM "boot-mode" */

	/* for SMALL-page devices */
static struct nand_oobinfo stm_nand_oobinfo_16 = {
	.useecc = MTD_NANDECC_AUTOPLACE,
#if CFG_STM_NAND_BOOT_MODE_ECC_WITH_B
	.eccbytes = 16,			/* 16 out of 16 bytes = 100% of OOB */
	.eccpos = {
		 0,  1,  2,  3,		/* 128-byte Record 0: ECC0, ECC1, ECC2, 'B' */
		 4,  5,  6,  7,		/* 128-byte Record 1: ECC0, ECC1, ECC2, 'B' */
		 8,  9, 10, 11,		/* 128-byte Record 2: ECC0, ECC1, ECC2, 'B' */
		12, 13, 14, 15},	/* 128-byte Record 3: ECC0, ECC1, ECC2, 'B' */
	.oobfree = { {0, 0} }		/* No free space in OOB! */
#else	/* CFG_STM_NAND_BOOT_MODE_ECC_WITH_B */
	.eccbytes = 12,			/* 12 out of 16 bytes = 75% of OOB */
	.eccpos = {
		 0,  1,  2,		/* 128-byte Record 0: ECC0, ECC1, ECC2 */
		 4,  5,  6,		/* 128-byte Record 1: ECC0, ECC1, ECC2 */
		 8,  9, 10,		/* 128-byte Record 2: ECC0, ECC1, ECC2 */
		12, 13, 14}		/* 128-byte Record 3: ECC0, ECC1, ECC2 */
	.oobfree = { {3, 1}, {7, 1}, {11, 1}, {15, 1} }	/* 4 free bytes in the OOB */
#endif	/* CFG_STM_NAND_BOOT_MODE_ECC_WITH_B */
};

	/* for LARGE-page devices */
static struct nand_oobinfo stm_nand_oobinfo_64 = {
	.useecc = MTD_NANDECC_AUTOPLACE,
#if CFG_STM_NAND_BOOT_MODE_ECC_WITH_B
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
#else	/* CFG_STM_NAND_BOOT_MODE_ECC_WITH_B */
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
#endif	/* CFG_STM_NAND_BOOT_MODE_ECC_WITH_B */
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
	struct	/* holds differences in "struct nand_chip" */
	{
		int			eccmode;
		int			eccsize;
		int			eccbytes;	/* for ECC + ID tags */
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


#endif	/* CFG_NAND_ECC_HW3_128 */


#ifdef CFG_NAND_FLEX_MODE	/* for STM "flex-mode" (c.f. "bit-banging") */

/* Flex-Mode Data {Read,Write} Config Registers & Flex-Mode {Command,Address} Registers */
#define FLEX_WAIT_RBn			( 1u << 27 )	/* wait for RBn to be asserted (i.e. ready) */
#define FLEX_BEAT_COUNT_1		( 1u << 28 )	/* One Beat */
#define FLEX_BEAT_COUNT_2		( 2u << 28 )	/* Two Beats */
#define FLEX_BEAT_COUNT_3		( 3u << 28 )	/* Three Beats */
#define FLEX_BEAT_COUNT_4		( 0u << 28 )	/* Four Beats */
#define FLEX_CSn_STATUS			( 1u << 31 )	/* Deasserts CSn after current operation completes */

/* Flex-Mode Data-{Read,Write} Config Registers */
#define FLEX_1_BYTE_PER_BEAT		( 0u << 30 )	/* One Byte per Beat */
#define FLEX_2_BYTES_PER_BEAT		( 1u << 30 )	/* Two Bytes per Beat */

/* Flex-Mode Configuration Register */
#define FLEX_CFG_ENABLE_FLEX_MODE	( 1u <<  0 )	/* Enable Flex-Mode operations */
#define FLEX_CFG_ENABLE_AFM		( 2u <<  0 )	/* Enable Advanced-Flex-Mode operations */
#define FLEX_CFG_SW_RESET		( 1u <<  3 )	/* Enable Software Reset */
#define FLEX_CFG_CSn_STATUS		( 1u <<  4 )	/* Deasserts CSn in current Flex bank */


enum stm_nand_flex_mode {
	flex_quiecent,		/* next byte_write is *UNEXPECTED* */
	flex_command,		/* next byte_write is a COMMAND */
	flex_address		/* next byte_write is a ADDRESS */
};


/*
 * NAND device connected to STM NAND Controller operating in FLEX mode.
 * There may be several NAND device connected to the NAND controller.
 */
struct stm_nand_flex_device {
	int			csn;
	struct nand_chip	*chip;
	struct mtd_info		*mtd;
	struct nand_timing_data *timing_data;
};


/*
 * STM NAND Controller operating in FLEX mode.
 * There is only a single one of these.
 */
static struct stm_nand_flex_controller {
	int			initialized;	/* is the FLEX controller initialized ? */
	int			current_csn;	/* Currently Selected Device (CSn) */
	int			next_csn;	/* First free NAND Device (CSn) */
	enum stm_nand_flex_mode mode;
	struct stm_nand_flex_device device[CFG_MAX_NAND_DEVICE];
	uint8_t			*buf;		/* Bounce buffer for non-aligned xfers */
} flex;


/*
 * In FLEX-mode, we can either read from the Flex-mode data
 * register (ST40_EMI_NAND_FLEX_DATA), over the STBus with either
 * a 4-byte read (LD4) or a 32-byte read (LD32) bus opcode.
 * Using the LD32 bus opcode amortises the cost of the bus
 * latency over 32-bytes, instead of only 4 bytes, so one can
 * achieve a higher throughput, using the LD32 bus opcode.
 *
 * However, in order to realize this potential improvement, one
 * needs to access the ST40_EMI_NAND_FLEX_DATA via the CPU's operand
 * (data) caches. This in turn requires a TLB (or PMB) address
 * translation to be configured and enabled.
 *
 * The following macros are used to configure the TLB to map
 * the ST40_EMI_NAND_FLEX_DATA into a P3 (cachable + translatable)
 * virtual address, so we can utilize the LD32 opcode.
 *
 * The implementation will create a single (read-only) 1KiB TLB
 * mapping, including ST40_EMI_NAND_FLEX_DATA to 0xC0000000.
 */
#if defined(CONFIG_SH_NAND_USES_CACHED_READS)
#define ST40_MMU_PTEH	0xFF000000	/* Page Table Entry High register */
#define ST40_MMU_PTEL	0xFF000004	/* Page Table Entry Low register */
#define ST40_MMU_MMUCR	0xFF000010	/* MMU Control Register */

#define PTEH_ASID	0		/* ASID to use (any 8-bit number) */

#define PTEL_WT		(1ul<<0)	/* Write-through bit */
#define PTEL_SH		(1ul<<1)	/* Private page or Shared page */
#define PTEL_D		(1ul<<2)	/* Dirty bit */
#define PTEL_C		(1ul<<3)	/* Cacheability bit */
#define PTEL_SZ_1K	(0ul<<4)	/* Page size of 1 KiB */
#define PTEL_SZ_4K	(1ul<<4)	/* Page size of 4 KiB */
#define PTEL_SZ_64K	(8ul<<4)	/* Page size of 64 KiB */
#define PTEL_SZ_1M	(9ul<<4)	/* Page size of 1 MiB */
#define PTEL_PR_READ	(0ul<<5)	/* Privileged-mode can READ */
#define PTEL_PR_WRITE	(1ul<<5)	/* Privileged-mode can READ+WRITE */
#define PTEL_V		(1ul<<8)	/* Validity Bit */
#define PTEL_UB		(1ul<<9)	/* Unbuffered write control bit */

#define MMUCR_AT	(1ul<<0)	/* Address Translation bit */
#define MMUCR_TI	(1ul<<2)	/* TLB Invalidate bit */

#define PAGE_MASK	(~0x3fful)	/* 1 KiB Page */
#define ASID_MASK	(0xfful)	/* 8 bit ASID */

static volatile u32 * const mmucr_p = (u32*)ST40_MMU_MMUCR;

static volatile u32 * const cache =
	(u32*)(0xC0000000ul | ((u32)ST40_EMI_NAND_FLEX_DATA & ~PAGE_MASK));
#endif	/* CONFIG_SH_NAND_USES_CACHED_READS */

#endif /* CFG_NAND_FLEX_MODE */


extern int stm_nand_default_bbt (struct mtd_info *mtd)
{
	struct nand_chip * const this = (struct nand_chip *)(mtd->priv);

	/* over-write the default "badblock_pattern", with our one */
	/* choose the correct pattern struct, depending on the OOB size */
	if (mtd->oobsize > 16)
		this->badblock_pattern = &stm_nand_badblock_pattern_64;	/* LARGE-page */
	else
		this->badblock_pattern = &stm_nand_badblock_pattern_16;	/* SMALL-page */

	/*
	 * For the "boot-mode+B" ECC (i.e. 3+1/128), then we wish to
	 * be compatible with the way linux scans NAND devices.
	 * So, we do not want to scan all pages, nor all the in-band data!
	 * Play with the options to make it so...
	 */
#ifdef CFG_NAND_ECC_HW3_128	/* for STM "boot-mode" */
#if CFG_STM_NAND_BOOT_MODE_ECC_WITH_B
	this->badblock_pattern->options &= ~(NAND_BBT_SCANEMPTY|NAND_BBT_SCANALLPAGES);
	this->badblock_pattern->options |= NAND_BBT_SCANSTMBOOTECC;
#endif	/* CFG_STM_NAND_BOOT_MODE_ECC_WITH_B */
#endif	/* CFG_NAND_ECC_HW3_128 */

	/* now call the generic BBT function */
	return nand_default_bbt (mtd);
}


#ifdef CFG_NAND_ECC_HW3_128	/* for STM "boot-mode" */


extern int stm_nand_calculate_ecc (
	struct mtd_info * const mtd,
	const u_char * const dat,
	u_char * const ecc_code)
{
	const struct nand_chip const * this = mtd->priv;

	if (this->eccmode!=NAND_ECC_HW3_128)
	{
		printf("ERROR: Can not calculate ECC: Internal Error (eccmode=%u)\n",
			this->eccmode);
		BUG();
		return -1;	/* Note: caller ignores this value! */
	}
	else if ((((unsigned long)dat)%4)!=0)	/* data *must* be 4-bytes aligned */
	{
		/* QQQ: change this case to use a properly aligned bounce buffer */
		printf("ERROR: Can not calculate ECC: data (%08lx) must be 4-byte aligned!\n",
			(unsigned long)dat);
		ecc_code[0] = 'B';
		ecc_code[1] = 'A';
		ecc_code[2] = 'D';
#if CFG_STM_NAND_BOOT_MODE_ECC_WITH_B
		ecc_code[3] = 'B';	/* append ASCII 'B', for Boot-mode */
#endif	/* CFG_STM_NAND_BOOT_MODE_ECC_WITH_B */
		return -1;	/* Note: caller ignores this value! */
	}
	else
	{	/* calculate 3 ECC bytes per 128 bytes of data */
		const ecc_t computed_ecc = ecc_gen(dat, ECC_128);
		/* poke them into the right place */
		ecc_code[0] = computed_ecc.byte[0];
		ecc_code[1] = computed_ecc.byte[1];
		ecc_code[2] = computed_ecc.byte[2];
#if CFG_STM_NAND_BOOT_MODE_ECC_WITH_B
		ecc_code[3] = 'B';	/* append ASCII 'B', for Boot-mode */
#endif	/* CFG_STM_NAND_BOOT_MODE_ECC_WITH_B */
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

	if (this->eccmode!=NAND_ECC_HW3_128)
	{
		printf("ERROR: Can not correct ECC: Internal Error (eccmode=%u)\n",
			this->eccmode);
		BUG();
		return -1;
	}

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

	/* correct a 1-bit error (if we can) */
	result = ecc_correct(dat, read, calc, ECC_128);

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


/*
 * fill in the "default_ecc" and "special_ecc" structures.
 */
static void initialize_ecc_diffs (
	const struct mtd_info * const mtd)
{
	const struct nand_chip * const this = (struct nand_chip *)(mtd->priv);
	struct nand_oobinfo * autooob;

	/* choose the correct OOB info struct, depending on the OOB size */
	if (mtd->oobsize > 16)
		autooob = &stm_nand_oobinfo_64;	/* LARGE-page */
	else
		autooob = &stm_nand_oobinfo_16;	/* SMALL-page */

	/* fill in "default_ecc" from the current "live" (default) structures */
	default_ecc.nand.eccmode	= this->eccmode;
	default_ecc.nand.eccsize	= this->eccsize;
	default_ecc.nand.eccbytes	= this->eccbytes;
	default_ecc.nand.eccsteps	= this->eccsteps;
	default_ecc.nand.autooob	= this->autooob;
	default_ecc.nand.calculate_ecc	= this->calculate_ecc;
	default_ecc.nand.correct_data	= this->correct_data;
	default_ecc.mtd.oobavail	= mtd->oobavail;
	default_ecc.mtd.eccsize		= mtd->eccsize;
	memcpy(&default_ecc.mtd.oobinfo, &mtd->oobinfo, sizeof(struct nand_oobinfo));

	/* fill in "special_ecc" for our special "hybrid" ECC paradigm */
	special_ecc.nand.eccmode	= NAND_ECC_HW3_128;
	special_ecc.nand.eccsize	= 128;
#if CFG_STM_NAND_BOOT_MODE_ECC_WITH_B
	special_ecc.nand.eccbytes	= 3 + 1;	/* ECC 3+1/128, '+1' for 'B' */
#else	/* CFG_STM_NAND_BOOT_MODE_ECC_WITH_B */
	special_ecc.nand.eccbytes	= 3;		/* ECC 3/128 */
#endif	/* CFG_STM_NAND_BOOT_MODE_ECC_WITH_B */
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
	this->eccbytes		= diffs->nand.eccbytes;
	this->eccsteps		= diffs->nand.eccsteps;
	this->autooob		= diffs->nand.autooob;
	this->calculate_ecc	= diffs->nand.calculate_ecc;
	this->correct_data	= diffs->nand.correct_data;

	mtd->oobavail		= diffs->mtd.oobavail;
	mtd->eccsize		= diffs->mtd.eccsize;
	memcpy(&mtd->oobinfo, &diffs->mtd.oobinfo, sizeof(struct nand_oobinfo));

	/* also, we need to reinitialize oob_buf */
	this->oobdirty		= 1;

#if VERBOSE_ECC
	printf("info: switching to NAND \"%s\" ECC (%u/%u)\n",
		(diffs==&special_ecc) ? "BOOT-mode" : "NON-boot-mode",
		this->eccbytes,
		this->eccsize);
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

	result = set_ecc_mode (mtd, from, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_read_ecc (mtd, from, len, retlen, buf, NULL, NULL);
	}

	return result;
}


extern int stm_nand_read_ecc (struct mtd_info *mtd, loff_t from, size_t len,
	size_t * retlen, u_char * buf, u_char * eccbuf, struct nand_oobinfo *oobsel)
{
	int result;

	result = set_ecc_mode (mtd, from, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_read_ecc (mtd, from, len, retlen, buf, eccbuf, oobsel);
	}

	return result;
}


extern int stm_nand_read_oob (struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, u_char * buf)
{
	int result;

	result = set_ecc_mode (mtd, from, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_read_oob (mtd, from, len, retlen, buf);
	}

	return result;
}


extern int stm_nand_write (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char * buf)
{
	int result;

	result = set_ecc_mode (mtd, to, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_write_ecc (mtd, to, len, retlen, buf, NULL, NULL);
	}

	return result;
}


extern int stm_nand_write_ecc (struct mtd_info *mtd, loff_t to, size_t len,
	size_t * retlen, const u_char * buf, u_char * eccbuf, struct nand_oobinfo *oobsel)
{
	int result;

	result = set_ecc_mode (mtd, to, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_write_ecc (mtd, to, len, retlen, buf, eccbuf, oobsel);
	}

	return result;
}


extern int stm_nand_write_oob (struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const u_char *buf)
{
	int result;

	result = set_ecc_mode (mtd, to, len);
	if (result != 0)
	{
		*retlen = 0;
	}
	else
	{
		result = nand_write_oob (mtd, to, len, retlen, buf);
	}

	return result;
}


#endif	/* CFG_NAND_ECC_HW3_128 */


#ifdef CFG_NAND_FLEX_MODE	/* for STM "flex-mode" (c.f. "bit-banging") */


/* Configure NAND controller timing registers */
/* QQQ: to write & use this function (for performance reasons!) */
#ifdef QQQ
static void flex_set_timings(struct nand_timing_data * const tm)
{
	uint32_t n;
	uint32_t reg;
	uint32_t emi_clk;
	uint32_t emi_t_ns;

	/* Timings are set in units of EMI clock cycles */
	emi_clk = clk_get_rate(clk_get(NULL, "emi_master"));
	emi_t_ns = 1000000000UL / emi_clk;

	/* CONTROL_TIMING */
	n = (tm->sig_setup + emi_t_ns - 1u)/emi_t_ns;
	reg = (n & 0xffu) << 0;

	n = (tm->sig_hold + emi_t_ns - 1u)/emi_t_ns;
	reg |= (n & 0xffu) << 8;

	n = (tm->CE_deassert + emi_t_ns - 1u)/emi_t_ns;
	reg |= (n & 0xffu) << 16;

	n = (tm->WE_to_RBn + emi_t_ns - 1u)/emi_t_ns;
	reg |= (n & 0xffu) << 24;

#if DEBUG_FLEX
	printf("info: CONTROL_TIMING = 0x%08x\n", reg);
#endif
	*ST40_EMI_NAND_CTL_TIMING = reg;

	/* WEN_TIMING */
	n = (tm->wr_on + emi_t_ns - 1u)/emi_t_ns;
	reg = (n & 0xffu) << 0;

	n = (tm->wr_off + emi_t_ns - 1u)/emi_t_ns;
	reg |= (n & 0xffu) << 8;

#if DEBUG_FLEX
	printf("info: WEN_TIMING = 0x%08x\n", reg);
#endif
	*ST40_EMI_NAND_WEN_TIMING = reg;

	/* REN_TIMING */
	n = (tm->rd_on + emi_t_ns - 1u)/emi_t_ns;
	reg = (n & 0xffu) << 0;

	n = (tm->rd_off + emi_t_ns - 1u)/emi_t_ns;
	reg |= (n & 0xffu) << 8;

#if DEBUG_FLEX
	printf("info: REN_TIMING = 0x%08x\n", reg);
#endif
	*ST40_EMI_NAND_REN_TIMING = reg;
}
#endif


/*
 * hardware specific access to the Ready/not_Busy signal.
 * Signal is routed through the EMI NAND Controller block.
 */
extern int stm_flex_device_ready(struct mtd_info * const mtd)
{
	/* Apply a small delay before sampling the RBn signal */
#if 1
	ndelay(500);	/* QQQ: do we really need this ??? */
#endif
	/* extract bit 2: status of RBn pin on the FLEX bank */
	return ((*ST40_EMI_NAND_RBN_STA) & (1ul<<2)) ? 1 : 0;
}


static void init_flex_mode(void)
{
	u_int32_t reg;

	/* Disable the BOOT-mode controller */
	*ST40_EMI_NAND_BOOTBANK_CFG = 0;

	/* Perform a S/W reset the FLEX-mode controller */
	/* need to assert it for at least one (EMI) clock cycle. */
	*ST40_EMI_NAND_FLEXMODE_CFG = FLEX_CFG_SW_RESET;
	udelay(1);	/* QQQ: can we do something shorter ??? */
	*ST40_EMI_NAND_FLEXMODE_CFG = 0;

	/* Disable all interrupts in FLEX mode */
	*ST40_EMI_NAND_INT_EN = 0;

	/* Set FLEX-mode controller to enable FLEX-mode */
	*ST40_EMI_NAND_FLEXMODE_CFG = FLEX_CFG_ENABLE_FLEX_MODE;

	/*
	 * Configure (pervading) FLEX_DATA to write 4-bytes at a time.
	 * DATA is only written by write_buf(), not write_byte().
	 * Hence, we only need to configure this once (ever)!
	 * As we may be copying directly from NOR flash to NAND flash,
	 * we need to deassert the CSn after *each* access, as we
	 * can not guarantee the buffer is in RAM (or not in the EMI).
	 * Note: we could run memcpy() in write_buf() instead.
	 */
	reg = FLEX_BEAT_COUNT_4 | FLEX_1_BYTE_PER_BEAT;
	reg |= FLEX_CSn_STATUS;		/* deassert CSn after each flex data write */
#if 0
	reg |= FLEX_WAIT_RBn;		/* QQQ: do we want this ??? */
#endif
	*ST40_EMI_NAND_FLEX_DATAWRT_CFG = reg;
}


/* FLEX mode chip select: For now we only support 1 chip per
 * 'stm_nand_flex_device' so chipnr will be 0 for select, -1 for deselect.
 *
 * So, if we change device:
 *   - Set bank in mux_control_reg to data->csn
 *   - Update read/write timings (to do)
 */
extern void stm_flex_select_chip(
	struct mtd_info * const mtd,
	const int chipnr)
{
	struct nand_chip * const chip = mtd->priv;
	struct stm_nand_flex_device * data = chip->priv;
#if defined(CONFIG_SH_NAND_USES_CACHED_READS)
	volatile u32 * const pteh_p  = (u32*)ST40_MMU_PTEH;
	volatile u32 * const ptel_p  = (u32*)ST40_MMU_PTEL;
	const u32 pteh =
		((u32)cache & PAGE_MASK)			|
		(PTEH_ASID & ASID_MASK);
	const u32 ptel =
		((u32)ST40_EMI_NAND_FLEX_DATA & PAGE_MASK)	|
		PTEL_V						|
#if 0
		PTEL_PR_WRITE | PTEL_D	/* if we use OCBI */	|
#endif
		PTEL_SZ_1K					|
		PTEL_C						|
		PTEL_SH;
#endif	/* CONFIG_SH_NAND_USES_CACHED_READS */

#if DEBUG_FLEX
	printf("\t\t\t\t---- SELECT = %2d ----\n", chipnr);
#endif

	if (!flex.initialized)		/* is the H/W yet to be initialized ? */
	{
		/* initialize the FLEX mode controller H/W */
		init_flex_mode();
		/* initialize the "flex" software structure */
		flex.mode          = flex_quiecent;	/* nothing pending */
		flex.next_csn      = 0;			/* start with first EMI CSn */
		flex.current_csn   = -1;		/* no NAND device selected */
							/* allocate a bounce buffer */
		flex.buf = malloc(NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE);
		if (flex.buf==NULL)
		{
			printf("ERROR: Unable to allocate memory for a bounce buffer\n");
			BUG();
		}
		/* initialize the TLB mapping if configured */
#if defined(CONFIG_SH_NAND_USES_CACHED_READS)
		*mmucr_p |= MMUCR_TI;	/* invalidate the TLBs */
		*pteh_p = pteh;
		*ptel_p = ptel;
		asm volatile ("ldtlb");	/* define 1 TLB mapping */
#endif	/* CONFIG_SH_NAND_USES_CACHED_READS */
		flex.initialized   = 1;			/* initialization done */
	}

	if (data == NULL)		/* device not yet scanned ? */
	{
#ifdef CFG_NAND_FLEX_CSn_MAP
		const int csn_map[CFG_MAX_NAND_DEVICE] = CFG_NAND_FLEX_CSn_MAP;
#endif	/* CFG_NAND_FLEX_CSn_MAP */
		int csn            = flex.next_csn++;		/* first free CSn */
		chip->priv = data  = &(flex.device[csn]);	/* first free "private" structure */
		if (csn >= CFG_MAX_NAND_DEVICE) BUG();
#ifdef CFG_NAND_FLEX_CSn_MAP
		csn                = csn_map[csn];		/* Re-map to different CSn if needed */
#endif	/* CFG_NAND_FLEX_CSn_MAP */
#if DEBUG_FLEX
		printf("info: stm_nand_flex_device.csn = %u\n", csn);
#endif

		data->csn          = csn;			/* fill in the private structure ... */
		data->mtd          = mtd;
		data->chip         = chip;
		data->timing_data  = NULL;			/* QQQ: to do */
#ifdef CFG_NAND_ECC_HW3_128
		mtd->read          = stm_nand_read;
		mtd->write         = stm_nand_write;
		mtd->read_ecc      = stm_nand_read_ecc;
		mtd->write_ecc     = stm_nand_write_ecc;
		mtd->read_oob      = stm_nand_read_oob;
		mtd->write_oob     = stm_nand_write_oob;
		chip->enable_hwecc = stm_nand_enable_hwecc;
#endif /* CFG_NAND_ECC_HW3_128 */
	}

	/* Deselect, do nothing */
	if (chipnr == -1) {
		return;

	} else if (chipnr == 0) {
		/* If same chip as last time, no need to change anything */
		if (data->csn == flex.current_csn)
			return;

		/* Set correct EMI Chip Select (CSn) on FLEX controller */
		flex.current_csn = data->csn;
		*ST40_EMI_NAND_FLEX_MUXCTRL = 1ul << data->csn;

		/* Set up timing parameters */
#if 0
		/* The default times will work for 200MHz (or slower) */
		/* QQQ: to do - BUT this is also the WRONG place to do this! */
		flex_set_timings(data->timing_data);
#endif

	} else {
		printf("ERROR: In %s() attempted to select chipnr = %d\n",
			__FUNCTION__,
			chipnr);
	}
}


extern void stm_flex_hwcontrol (
	struct mtd_info * const mtd,
	int control)
{
	switch(control) {

	case NAND_CTL_SETCLE:
#if DEBUG_FLEX
		printf("\t\t\t\t\t\t----START COMMAND----\n");
		if (flex.mode != flex_quiecent) BUG();
#endif
		flex.mode = flex_command;
		break;

#if DEBUG_FLEX
	case NAND_CTL_CLRCLE:
		printf("\t\t\t\t\t\t---- end  command----\n");
		if (flex.mode != flex_command) BUG();
		flex.mode = flex_quiecent;
		break;
#endif

	case NAND_CTL_SETALE:
#if DEBUG_FLEX
		printf("\t\t\t\t\t\t----START ADDRESS----\n");
		if (flex.mode != flex_quiecent) BUG();
#endif
		flex.mode = flex_address;
		break;

#if DEBUG_FLEX
	case NAND_CTL_CLRALE:
		printf("\t\t\t\t\t\t---- end  address----\n");
		if (flex.mode != flex_address) BUG();
		flex.mode = flex_quiecent;
		break;
#endif

#if DEBUG_FLEX
	default:
		printf("ERROR: Unexpected parameter (control=0x%x) in %s()\n",
			control,
			__FUNCTION__);
		BUG();
#endif
	}
}


/**
 * nand_read_byte - [DEFAULT] read one byte from the chip
 * @mtd:	MTD device structure
 */
extern u_char stm_flex_read_byte(
	struct mtd_info * const mtd)
{
	u_char byte;
	u_int32_t reg;

	/* read 1-byte at a time */
	reg = FLEX_BEAT_COUNT_1 | FLEX_1_BYTE_PER_BEAT;
	reg |= FLEX_CSn_STATUS;		/* deassert CSn after each flex data read */
#if 0
	reg |= FLEX_WAIT_RBn;		/* QQQ: do we want this ??? */
#endif
	*ST40_EMI_NAND_FLEX_DATA_RD_CFG = reg;

	/* read it */
	byte = (u_char)*ST40_EMI_NAND_FLEX_DATA;

#if DEBUG_FLEX
	printf("\t\t\t\t\t\t\t\t\t READ = 0x%02x\n", byte);
#endif

	/* return it */
	return byte;
}


/**
 * nand_write_byte - [DEFAULT] write one byte to the chip
 * @mtd:	MTD device structure
 * @byte:	pointer to data byte to write
 */
extern void stm_flex_write_byte(
	struct mtd_info * const mtd,
	u_char byte)
{
	u_int32_t reg;

#if DEBUG_FLEX
	printf("\t\t\t\t\t\t\t\t\tWRITE = 0x%02x\t%s\n", byte,
		(flex.mode==flex_command) ? "command" :
		((flex.mode==flex_address) ? "address" : "*UNKNOWN*"));
#endif

	switch (flex.mode)
	{
		case flex_command:
			reg = byte | FLEX_BEAT_COUNT_1;
			reg |= FLEX_CSn_STATUS;	/* deassert CSn after each flex command write */
#if 0
			reg |= FLEX_WAIT_RBn;		/* QQQ: do we want this ??? */
#endif
			*ST40_EMI_NAND_FLEX_CMD = reg;
			break;

		case flex_address:
			reg = byte | FLEX_BEAT_COUNT_1;
			reg |= FLEX_CSn_STATUS;	/* deassert CSn after each flex address write */
#if 0
			reg |= FLEX_WAIT_RBn;		/* QQQ: do we want this ??? */
#endif
			*ST40_EMI_NAND_FLEX_ADD_REG = reg;
#if 0			/* QQQ: do we need this - I think not! */
			while (!nand_device_ready()) ;	/* wait till NAND is ready */
#endif
			break;

		default:
			BUG();
	}
}


/**
 * nand_read_buf - [DEFAULT] read chip data into buffer
 * @mtd:	MTD device structure
 * @buf:	buffer to store data
 * @len:	number of bytes to read
 */
extern void stm_flex_read_buf(
	struct mtd_info * const mtd,
	u_char * const buf,
	const int len)
{
	int i;
	uint32_t *p;
	u_int32_t reg;

	/* our buffer needs to be 4-byte aligned, for the FLEX controller */
	p = ((uint32_t)buf & 0x3) ? (void*)flex.buf : (void*)buf;

#if DEBUG_FLEX
	printf("info: stm_flex_read_buf( buf=%p, len=0x%x )\t\tp=%p%s\n",
		buf, len, p,
		((uint32_t)buf & 0x3) ? "\t\t**** UN-ALIGNED *****" : "");
#endif

	/* configure to read 4-bytes at a time */
	reg = FLEX_BEAT_COUNT_4 | FLEX_1_BYTE_PER_BEAT;
	reg |= FLEX_CSn_STATUS;		/* deassert CSn after each flex data read */
#if 0
	reg |= FLEX_WAIT_RBn;		/* QQQ: do we want this ??? */
#endif
	*ST40_EMI_NAND_FLEX_DATA_RD_CFG = reg;

#if defined(CONFIG_SH_NAND_USES_CACHED_READS)
	/*
	 * Note, we only use the optimized cached TLB mapping,
	 * if the amount of data to be copied is an exact
	 * multiple of the length of a cache line.
	 */
	if ((len % DCACHE_LINESZ) == 0)	/* whole multiples of cache lines ? */
	{
		*mmucr_p |= MMUCR_AT;	/* enable Address Translation */
		asm volatile ("nop");	/* wait a few cycles after enabling AT */
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");

#if 0	/* QQQ - DELETE */
		/* copy the data (from NAND), one CACHE-LINE at a time ... */
		if (((u32)p) % DCACHE_LINESZ == 0ul)	/* cache aligned ? */
		{
			register u32 src32;
			register u32 dst32 = (u32)p;
			register double temp0 asm("dr0");
			register double temp1 asm("dr2");
			register double temp2 asm("dr4");
			register double temp3 asm("dr6");

#define PUT_FPSCR(F)	asm volatile ("lds %0, fpscr"	: : "r"(F))
#define OCBP(LINE)	asm volatile ("ocbp @%0"	: : "r"(LINE))
#define PREF(LINE)	asm volatile ("pref @%0"	: : "r"(LINE))
#define ALLOC(LINE)	asm volatile ("movca.l r0, @%0"	: : "r"(LINE))

			OCBP(cache);
			PREF(cache);
			PUT_FPSCR(1<<20);	/* FPSCR.SZ=1 */

			for(i=0; i<len; )
			{
				src32 = (u32)cache;
				asm volatile ("fmov @%0+,%1"	: "+r"(src32),"=d"(temp0));
				asm volatile ("fmov @%0+,%1"	: "+r"(src32),"=d"(temp1));
				asm volatile ("fmov @%0+,%1"	: "+r"(src32),"=d"(temp2));
				asm volatile ("fmov @%0+,%1"	: "+r"(src32),"=d"(temp3));

				OCBP(cache);

				i += DCACHE_LINESZ;

				if (i<len)
				{
//QQQ					PREF(cache);
				}

				ALLOC(dst32);
				asm volatile ("fmov %1,@%0"	: : "r"(dst32),"d"(temp0) : "memory" );
				dst32+=8;
				asm volatile ("fmov %1,@%0"	: : "r"(dst32),"d"(temp1) : "memory" );
				dst32+=8;
				asm volatile ("fmov %1,@%0"	: : "r"(dst32),"d"(temp2) : "memory" );
				dst32+=8;
				asm volatile ("fmov %1,@%0"	: : "r"(dst32),"d"(temp3) : "memory" );
				dst32+=8;
			}
		}
		else	/* destination is *not* cache-aligned */
#endif	/* QQQ - DELETE */
		{
			for(i=0; i<len/4; i+=DCACHE_LINESZ/4)
			{
				asm volatile ("ocbp @%0" : : "r"(cache));
				p[i+0] = cache[0];
				p[i+1] = cache[1];
				p[i+2] = cache[2];
				p[i+3] = cache[3];
				p[i+4] = cache[4];
				p[i+5] = cache[5];
				p[i+6] = cache[6];
				p[i+7] = cache[7];
			}
		}
#if 0					/* QQQ - DELETE */
		if (++done_init < 8)
		{
			int j;
			printf("READ BUF\tlen=%u,\tpass=%u\n",len, done_init);
			for (i=0; i<len; i+=16)
			{
				for (j=0; j<16; j++)
					printf("%02x ", buf[i+j]);
				printf("\n");
			}
			printf("\n");
		}
#endif					/* QQQ - DELETE */
		/* finally, disable Address Translation */
		*mmucr_p &= ~MMUCR_AT;
	}
	else
	{
		/* let the user know we are *not* using the TLB */
		printf("warning: Not using cached copy in %s() for len=%u\n",
			__FUNCTION__,
			len);
		/* copy the data (from NAND) as 4-byte words ... */
		for(i=0; i<len/4; i++)
		{
			p[i] = *ST40_EMI_NAND_FLEX_DATA;
		}
	}
#else	/* CONFIG_SH_NAND_USES_CACHED_READS */
	/* copy the data (from NAND) as 4-byte words ... */
	for(i=0; i<len/4; i++)
	{
		p[i] = *ST40_EMI_NAND_FLEX_DATA;
	}
#endif	/* CONFIG_SH_NAND_USES_CACHED_READS */

	/* copy back into user-supplied buffer, if it was unaligned */
	if ((void*)p != (void*)buf)
		memcpy(buf, p, len);

#if DEBUG_FLEX
	printf("READ BUF\t\t\t\t");
	for (i=0; i<16; i++)
		printf("%02x ", buf[i]);
	printf("...\n");
#endif
}


/**
 * nand_write_buf - [DEFAULT] write buffer to chip
 * @mtd:	MTD device structure
 * @buf:	data buffer
 * @len:	number of bytes to write
 */
extern void stm_flex_write_buf(
	struct mtd_info * const mtd,
	const u_char * const buf,
	const int len)
{
	int i;
	uint32_t *p;

#if DEBUG_FLEX
	printf("WRITE BUF\t\t");
	for (i=0; i<16; i++)
		printf("%02x ", buf[i]);
	printf("...\n");
#endif

	/* our buffer needs to be 4-byte aligned, for the FLEX controller */
	p = ((uint32_t)buf & 0x3) ? (void*)flex.buf : (void*)buf;

	/* copy from user-supplied buffer, if it is unaligned */
	if ((void*)p != (void*)buf)
		memcpy(p, buf, len);

	/* configured to write 4-bytes at a time */
	/* copy the data (to NAND) as 32-bit words ... */
	for(i=0; i<len/4; i++)
	{
		*ST40_EMI_NAND_FLEX_DATA = p[i];
	}
}


#endif /* CFG_NAND_FLEX_MODE */


#endif	/* CONFIG_CMD_NAND */
