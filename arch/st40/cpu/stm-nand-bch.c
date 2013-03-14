/*
 *  Copyright (c) 2011-2012 STMicroelectronics Limited
 *  Authors: Angus Clark <Angus.Clark@st.com>
 *           Sean McGoogan <Sean.McGoogan@st.com>
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
#include <linux/ctype.h>

#if defined(CONFIG_CMD_NAND) && defined(CONFIG_SYS_ST40_NAND_USE_BCH)

#include <nand.h>
#include <asm/stm-nand.h>
#include <asm/ecc.h>
#include <asm/errno.h>
#include <asm/st40reg.h>
#include <asm/io.h>
#include <asm/socregs.h>
#include <asm/cache.h>
#include <asm/addrspace.h>


	/* define DEBUG_BCH to enable verbose debugging of BCH */
//#define DEBUG_BCH


#define readsl(port, buffer, count) insl((unsigned long)(port), (buffer), (count))			/* QQQ - move ??? */
#define writesl(port, buffer, count) outsl((unsigned long)(port), (buffer), (count))			/* QQQ - move ??? */


/* ECC Modes */
#define BCH_18BIT_ECC				0
#define BCH_30BIT_ECC				1
#define BCH_NO_ECC				2
#define BCH_ECC_RSRV				3

/* NANDBCH_BOOTBANK_CFG */
#define BOOT_CFG_RESET				(0x1 << 3)

/* NANDBCH_CONTROLLER_CFG/NANDHAM_FLEXMODE_CFG */
#define CONFIG_SYS_ENABLE_FLEX			(0x1)
#define CONFIG_SYS_ENABLE_AFM			(0x2)
#define CONFIG_SYS_RESET			(0x1 << 3)
#define CONFIG_SYS_RESET_ECC(x)			(0x1 << (7 + (x)))
#define CONFIG_SYS_RESET_ECC_ALL		(0xff << 7)

/* NANDi BCH Controller properties */
#define NANDI_BCH_MAX_BUF_LIST			8u	/* H/W supports 8 buffers */
#define NANDI_BCH_BUF_LIST_SIZE			( NANDI_BCH_MAX_BUF_LIST * sizeof(u32) )
#define NANDI_BCH_SECTOR_SIZE			1024u
#define NANDI_BCH_DMA_ALIGNMENT			64u

/* helper to force BCH alignment constraints with the DMA */
#define NANDI_BCH_IS_ALIGNED(x)			( (((unsigned long)(x)) & (NANDI_BCH_DMA_ALIGNMENT-1u)) ? 0 : 1 )
#define NANDI_BCH_ALIGN_BUFFER(x)		( (((unsigned long)(x))+NANDI_BCH_DMA_ALIGNMENT-1u) & ~(NANDI_BCH_DMA_ALIGNMENT-1u) )


/*
 * Hamming/BCH controller interrupts
 */

/* NANDxxx_INT_EN/NANDxxx_INT_STA */
/*      Common */
#define NAND_INT_ENABLE				(0x1 << 0)
#define NAND_INT_RBN				(0x1 << 2)
#define NAND_INT_SEQCHECK			(0x1 << 5)
/*      Hamming only */
#define NANDHAM_INT_DATA_DREQ			(0x1 << 3)
#define NANDHAM_INT_SEQ_DREQ			(0x1 << 4)
#define NANDHAM_INT_ECC_FIX_REQ			(0x1 << 6)
/*      BCH only */
#define NANDBCH_INT_SEQNODESOVER		(0x1 << 7)
#define NANDBCH_INT_ECCTHRESHOLD		(0x1 << 8)

/* NANDxxx_INT_CLR */
/*      Common */
#define NAND_INT_CLR_RBN			(0x1 << 2)
#define NAND_INT_CLR_SEQCHECK			(0x1 << 3)
/*      Hamming only */
#define NANDHAM_INT_CLR_ECC_FIX_REQ		(0x1 << 4)
#define NANDHAM_INT_CLR_DATA_DREQ		(0x1 << 5)
#define NANDHAM_INT_CLR_SEQ_DREQ		(0x1 << 6)
/*      BCH only */
#define NANDBCH_INT_CLR_SEQNODESOVER		(0x1 << 5)
#define NANDBCH_INT_CLR_ECCTHRESHOLD		(0x1 << 6)

/* NANDxxx_INT_EDGE_CFG */
#define NAND_EDGE_CFG_RBN_RISING		0x1
#define NAND_EDGE_CFG_RBN_FALLING		0x2
#define NAND_EDGE_CFG_RBN_ANY			0x3


/*************************************************************************
 *******************	static (global) variables	******************
 *************************************************************************/


static u32 * dma_buffer_list = NULL;
static void * dma_buffer = NULL;
static int bch_ecc_mode = BCH_NO_ECC;


/*************************************************************************
 *******************	Initialization Functions	******************
 *************************************************************************/


enum nandi_controllers {STM_NANDI_UNCONFIGURED,
			STM_NANDI_HAMMING,
			STM_NANDI_BCH};


static void emiss_nandi_select(const enum nandi_controllers controller)
{
	unsigned v;

	v = readl(ST40_EMISS_CONFIG);

	if (controller == STM_NANDI_HAMMING) {
		if (v & ST40_EMISS_NAND_HAMMING_NOT_BCH)
			return;		/* already selected Hamming */
		v |= ST40_EMISS_NAND_HAMMING_NOT_BCH;
	} else {
		if (!(v & ST40_EMISS_NAND_HAMMING_NOT_BCH))
			return;		/* already selected BCH */
		v &= ~ST40_EMISS_NAND_HAMMING_NOT_BCH;
	}

	writel(v, ST40_EMISS_CONFIG);	/* select the chosen controller */

	/*
	 * Ensure the writel() has completed before we attempt to make
	 * any further accesses to the newly selected NAND controller,
	 * by simply reading it back again.
	 * This was empirically required by Linux on the Freeman Ultra.
	 * We will do this on all ST40 systems -- best to be safe!
	 */
	readl(ST40_EMISS_CONFIG);
}


static void nandi_init_hamming(const int emi_bank)
{
	/* Initialise the Hamming Controller */
	emiss_nandi_select(STM_NANDI_HAMMING);

	/* Reset and disable boot-mode controller */
	writel(BOOT_CFG_RESET, ST40_EMI_NAND_HAM_BOOTBANK_CFG);
	udelay(1);
	writel(0x00000000, ST40_EMI_NAND_HAM_BOOTBANK_CFG);

	/* Reset controller */
	writel(CONFIG_SYS_RESET, ST40_EMI_NAND_HAM_FLEXMODE_CFG);
	udelay(1);
	writel(0x00000000, ST40_EMI_NAND_HAM_FLEXMODE_CFG);

	/* Disable all interrupts in FLEX mode */
	writel(0, ST40_EMI_NAND_HAM_INT_EN);

	/* Set EMI Bank */
	writel(0x1 << emi_bank, ST40_EMI_NAND_HAM_FLEX_MUXCTRL);

	/* Enable FLEX mode */
	writel(CONFIG_SYS_ENABLE_FLEX, ST40_EMI_NAND_HAM_FLEXMODE_CFG);

	/* Configure pervading FLEX_DATA_READ/WRITE as 1-byte accesses */
	writel(STM_NAND_FLEX_BEAT_COUNT_1 | STM_NAND_FLEX_1_BYTE_PER_BEAT | STM_NAND_FLEX_CSn_STATUS,
	       ST40_EMI_NAND_HAM_FLEX_DATA_RD_CFG);
	writel(STM_NAND_FLEX_BEAT_COUNT_1 | STM_NAND_FLEX_1_BYTE_PER_BEAT | STM_NAND_FLEX_CSn_STATUS,
	       ST40_EMI_NAND_HAM_FLEX_DATAWRT_CFG);
}


static void nandi_init_bch(const int emi_bank)
{
	/* Initialise the BCH Controller */
	emiss_nandi_select(STM_NANDI_BCH);

	/* Reset and disable boot-mode controller */
	writel(BOOT_CFG_RESET, ST40_EMI_NAND_BCH_BOOTBANK_CFG);
	udelay(1);
	writel(0x00000000, ST40_EMI_NAND_BCH_BOOTBANK_CFG);

	/* Reset AFM controller */
	writel(CONFIG_SYS_RESET, ST40_EMI_NAND_BCH_CONTROLLER_CFG);
	udelay(1);
	writel(0x00000000, ST40_EMI_NAND_BCH_CONTROLLER_CFG);

	/* Set EMI Bank */
	writel(0x1 << emi_bank, ST40_EMI_NAND_BCH_FLEX_MUXCTRL);

	/* Reset ECC stats */
	writel(0x7f0, ST40_EMI_NAND_BCH_CONTROLLER_CFG);
	udelay(1);

	/* Enable AFM */
	writel(CONFIG_SYS_ENABLE_AFM, ST40_EMI_NAND_BCH_CONTROLLER_CFG);

	/* Timing parameters */
	/* Values from validation found not to work on some parts.  Awaiting
	 * clarification.  Use reset values for the time being.
	 *
	 * writel(0x14000205, ST40_EMI_NAND_BCH_CTL_TIMING);
	 * writel(0x00020304, ST40_EMI_NAND_BCH_WEN_TIMING);
	 * writel(0x060b0304, ST40_EMI_NAND_BCH_REN_TIMING);
	 */

	/* Configure Read DMA Plugs (values supplied by Validation)*/
	writel(0x00000005, ST40_EMISS_NAND_RD_DMA_PAGE_SIZE);
	writel(0x00000005, ST40_EMISS_NAND_RD_DMA_MAX_OPCODE_SIZE);
	writel(0x00000002, ST40_EMISS_NAND_RD_DMA_MIN_OPCODE_SIZE);
	writel(0x00000001, ST40_EMISS_NAND_RD_DMA_MAX_CHUNK_SIZE);
	writel(0x00000000, ST40_EMISS_NAND_RD_DMA_MAX_MESSAGE_SIZE);

	/* Configure Write DMA Plugs (values supplied by Validation) */
	writel(0x00000005, ST40_EMISS_NAND_WR_DMA_PAGE_SIZE);
	writel(0x00000005, ST40_EMISS_NAND_WR_DMA_MAX_OPCODE_SIZE);
	writel(0x00000002, ST40_EMISS_NAND_WR_DMA_MIN_OPCODE_SIZE);
	writel(0x00000001, ST40_EMISS_NAND_WR_DMA_MAX_CHUNK_SIZE);
	writel(0x00000000, ST40_EMISS_NAND_WR_DMA_MAX_MESSAGE_SIZE);

	/* Enable the SEQ_NODES_OVER interrupt for the BCH (we will poll on it!) */
	writel(NAND_INT_ENABLE | NANDBCH_INT_SEQNODESOVER,
		ST40_EMI_NAND_BCH_INT_EN);
}


/*************************************************************************
 *******************	Hamming-FLEX Functions		******************
 *************************************************************************/


/*
 * hardware specific access to the Ready/not_Busy signal.
 * Signal is routed through the EMI NAND Controller block.
 */
static int stm_flex_device_ready(struct mtd_info * const mtd)
{
	/* Apply a small delay before sampling the RBn signal */
	ndelay(500);

	/* extract bit 2: status of RBn pin on the FLEX bank */
	return ((*ST40_EMI_NAND_HAM_RBN_STA) & (1ul<<2)) ? 1 : 0;
}


static int flex_wait_rbn(struct mtd_info * const mtd)
{
	while(!stm_flex_device_ready(mtd))
		;	/* no nothing */

	return 1;	/* ready now */
}


static void flex_cmd(const uint8_t cmd)
{
	const uint32_t val = (STM_NAND_FLEX_CSn_STATUS | STM_NAND_FLEX_BEAT_COUNT_1 | cmd);
	writel(val, ST40_EMI_NAND_HAM_FLEX_CMD);
}


static void flex_addr(uint32_t addr, const int cycles)
{
	addr &= 0x00ffffffu;

	BUG_ON(cycles < 1);
	BUG_ON(cycles > 3);

	addr |= (STM_NAND_FLEX_CSn_STATUS | STM_NAND_FLEX_ADDR_ADD8_VALID);
	addr |= (cycles & 0x3) << 28;

	writel(addr, ST40_EMI_NAND_HAM_FLEX_ADD_REG);
}


/*
 * Partial implementation of MTD/NAND Interface, based on Hamming-FLEX
 * operation.
 *
 * Allows us to make use of nand_base.c functions where possible
 * (e.g. nand_scan_ident() and friends).
 */
static void flex_command_lp(
	struct mtd_info * const mtd,
	unsigned int command,
	int column,
	const int page)
{
	struct nand_chip * const chip = mtd->priv;

#if defined(DEBUG_BCH)
	printf("flex_command_lp(mtd=%p, command=0x%02x, column=%d, page=%d) ... %s\n", mtd, command, column, page,
		(command==NAND_CMD_READOOB) ? "NAND_CMD_READOOB" :
		(command==NAND_CMD_READ0) ? "NAND_CMD_READ0" :
		(command==NAND_CMD_ERASE1) ? "NAND_CMD_ERASE1" :
		(command==NAND_CMD_SEQIN) ? "NAND_CMD_SEQIN" :
		(command==NAND_CMD_RESET) ? "NAND_CMD_RESET" :
		(command==NAND_CMD_PARAM) ? "NAND_CMD_PARAM" :
		(command==NAND_CMD_ERASE2) ? "NAND_CMD_ERASE2" :
		(command==NAND_CMD_STATUS) ? "NAND_CMD_STATUS" :
		(command==NAND_CMD_READID) ? "NAND_CMD_READID" :
		(command==NAND_CMD_PAGEPROG) ? "NAND_CMD_PAGEPROG" :
		"***UNKNOWN***");
#endif /* DEBUG_BCH */

	emiss_nandi_select(STM_NANDI_HAMMING);

	/* Emulate NAND_CMD_READOOB */
	if (command == NAND_CMD_READOOB) {
		column += mtd->writesize;
		command = NAND_CMD_READ0;
	}

	switch (command) {

	case NAND_CMD_READ0:
	case NAND_CMD_RESET:
	case NAND_CMD_PARAM:
	case NAND_CMD_STATUS:
	case NAND_CMD_READID:
	case NAND_CMD_SEQIN:	/* used by "nand write.oob" */
	case NAND_CMD_PAGEPROG:	/* used by "nand write.oob" */
	case NAND_CMD_ERASE1:	/* used by "nand erase"     */
	case NAND_CMD_ERASE2:	/* used by "nand erase"     */
		break;		/* okay, safe to process here ... */

	default:
		/* Catch all unexpected commands! */
		BUG();
	}

	/*
	 * Command Cycle
	 */
	flex_cmd(command);

	/*
	 * Address Cycles
	 */
	if (column != -1)
		flex_addr(column, (command == NAND_CMD_READID) ? 1 : 2);
	if (page != -1)
		flex_addr(page, (chip->chipsize > (128 << 20)) ? 3 : 2);

	/* Complete 'READ0' command */
	if (command == NAND_CMD_READ0)
		flex_cmd(NAND_CMD_READSTART);

	/* Wait for RBn, if required. */
	if (command == NAND_CMD_RESET ||
	    command == NAND_CMD_READ0 ||
	    command == NAND_CMD_PARAM) {
		flex_wait_rbn(mtd);
	}
}


static uint8_t flex_read_byte(struct mtd_info * const mtd)
{
	emiss_nandi_select(STM_NANDI_HAMMING);

	return (uint8_t)(readl(ST40_EMI_NAND_HAM_FLEX_DATA) & 0xff);
}


static int flex_wait_func(
	struct mtd_info * const mtd,
	struct nand_chip * const chip)
{
	emiss_nandi_select(STM_NANDI_HAMMING);

	flex_wait_rbn(mtd);

	flex_cmd(NAND_CMD_STATUS);

	return (int)(readl(ST40_EMI_NAND_HAM_FLEX_DATA) & 0xff);
}


/* Multi-CS devices not supported */
static void flex_select_chip(
	struct mtd_info * const mtd,
	const int chipnr)
{
	/* no multi-CS support, ... nothing to do yet! */
}


static void flex_read_buf(
	struct mtd_info * const mtd,
	uint8_t *buf,
	int len)
{
	emiss_nandi_select(STM_NANDI_HAMMING);

	/* Read bytes until buf is 4-byte aligned */
	while (len && ((unsigned int)buf & 0x3)) {
		*buf++ = (uint8_t)(readl(ST40_EMI_NAND_HAM_FLEX_DATA) & 0xff);
		len--;
	}

	/* Use 'BEATS_4'/readsl */
	if (len >= 8) {	/* big enough to benefit from cost of readsl() overhead ? */
		const unsigned int aligned = len & ~0x3;

		writel(STM_NAND_FLEX_BEAT_COUNT_4 | STM_NAND_FLEX_1_BYTE_PER_BEAT | STM_NAND_FLEX_CSn_STATUS,
		       ST40_EMI_NAND_HAM_FLEX_DATA_RD_CFG);

		readsl(ST40_EMI_NAND_HAM_FLEX_DATA, buf, aligned >> 2);

		buf += aligned;
		len -= aligned;
			/* restore pervading configuration */
		writel(STM_NAND_FLEX_BEAT_COUNT_1 | STM_NAND_FLEX_1_BYTE_PER_BEAT | STM_NAND_FLEX_CSn_STATUS,
		       ST40_EMI_NAND_HAM_FLEX_DATA_RD_CFG);
	}

	/* Mop up remaining bytes */
	while (len--) {
		*buf++ = (uint8_t)(readl(ST40_EMI_NAND_HAM_FLEX_DATA) & 0xff);
	}
}


static void flex_write_buf(
	struct mtd_info * const mtd,
	const uint8_t *buf,
	int len)
{
	emiss_nandi_select(STM_NANDI_HAMMING);

	/* Write bytes until buf is 4-byte aligned */
	while (len && ((unsigned int)buf & 0x3)) {
		writel(*buf++, ST40_EMI_NAND_HAM_FLEX_DATA);
		len--;
	};

	/* USE 'BEATS_4/writesl */
	if (len >= 8) {	/* big enough to benefit from cost of writesl() overhead ? */
		const unsigned int aligned = len & ~0x3;

		writel(STM_NAND_FLEX_BEAT_COUNT_4 | STM_NAND_FLEX_1_BYTE_PER_BEAT | STM_NAND_FLEX_CSn_STATUS,
		       ST40_EMI_NAND_HAM_FLEX_DATAWRT_CFG);

		writesl(ST40_EMI_NAND_HAM_FLEX_DATA, buf, aligned >> 2);

		buf += aligned;
		len -= aligned;
			/* restore pervading configuration */
		writel(STM_NAND_FLEX_BEAT_COUNT_1 | STM_NAND_FLEX_1_BYTE_PER_BEAT | STM_NAND_FLEX_CSn_STATUS,
		       ST40_EMI_NAND_HAM_FLEX_DATAWRT_CFG);
	}

	/* Mop up remaining bytes */
	while (len--) {
		writel(*buf++, ST40_EMI_NAND_HAM_FLEX_DATA);
	}
}


/*************************************************************************
 *******************		BCH Functions		******************
 *************************************************************************/


/* NANDBCH_GEN_CFG */
#define GEN_CFG_DATA_8_NOT_16		(0x1 << 16)
#define GEN_CFG_EXTRA_ADD_CYCLE		(0x1 << 18)
#define GEN_CFG_2X8_MODE		(0x1 << 19)
#define GEN_CFG_ECC_SHIFT		20
#define GEN_CFG_18BIT_ECC		(BCH_18BIT_ECC << GEN_CFG_ECC_SHIFT)
#define GEN_CFG_30BIT_ECC		(BCH_30BIT_ECC << GEN_CFG_ECC_SHIFT)
#define GEN_CFG_NO_ECC			(BCH_NO_ECC << GEN_CFG_ECC_SHIFT)
#define GEN_CFG_LAST_SEQ_NODE		(0x1 << 22)

/* NANDBCH_SEQ_CFG */
#define SEQ_CFG_REPEAT_COUNTER(x)	((x) & 0xffff)
#define SEQ_CFG_SEQ_IDENT(x)		(((x) & 0xff) << 16)
#define SEQ_CFG_DATA_WRITE		(0x1 << 24)
#define SEQ_CFG_ERASE			(0x1 << 25)
#define SEQ_CFG_GO_STOP			(0x1 << 26)

/* NANDBCH_SEQ_STA */
#define SEQ_STA_RUN			(0x1 << 4)

/*
 * BCH Commands
 */
#define BCH_OPC_STOP			0x0
#define BCH_OPC_CMD			0x1
#define BCH_OPC_INC			0x2
#define BCH_OPC_DEC_JUMP		0x3
#define BCH_OPC_DATA			0x4
#define BCH_OPC_DELAY			0x5
#define BCH_OPC_CHECK			0x6
#define BCH_OPC_ADDR			0x7
#define BCH_OPC_NEXT_CHIP_ON		0x8
#define BCH_OPC_DEC_JMP_MCS		0x9
#define BCH_OPC_ECC_SCORE		0xA

#define BCH_INSTR(opc, opr)		((opc) | ((opr) << 4))

#define BCH_CMD_ADDR			BCH_INSTR(BCH_OPC_CMD, 0)
#define BCH_CL_CMD_1			BCH_INSTR(BCH_OPC_CMD, 1)
#define BCH_CL_CMD_2			BCH_INSTR(BCH_OPC_CMD, 2)
#define BCH_CL_CMD_3			BCH_INSTR(BCH_OPC_CMD, 3)
#define BCH_CL_EX_0			BCH_INSTR(BCH_OPC_CMD, 4)
#define BCH_CL_EX_1			BCH_INSTR(BCH_OPC_CMD, 5)
#define BCH_CL_EX_2			BCH_INSTR(BCH_OPC_CMD, 6)
#define BCH_CL_EX_3			BCH_INSTR(BCH_OPC_CMD, 7)
#define BCH_INC(x)			BCH_INSTR(BCH_OPC_INC, (x))
#define BCH_DEC_JUMP(x)			BCH_INSTR(BCH_OPC_DEC_JUMP, (x))
#define BCH_STOP			BCH_INSTR(BCH_OPC_STOP, 0)
#define BCH_DATA_1_SECTOR		BCH_INSTR(BCH_OPC_DATA, 0)
#define BCH_DATA_2_SECTOR		BCH_INSTR(BCH_OPC_DATA, 1)
#define BCH_DATA_4_SECTOR		BCH_INSTR(BCH_OPC_DATA, 2)
#define BCH_DATA_8_SECTOR		BCH_INSTR(BCH_OPC_DATA, 3)
#define BCH_DATA_16_SECTOR		BCH_INSTR(BCH_OPC_DATA, 4)
#define BCH_DATA_32_SECTOR		BCH_INSTR(BCH_OPC_DATA, 5)
#define BCH_DELAY_0			BCH_INSTR(BCH_OPC_DELAY, 0)
#define BCH_DELAY_1			BCH_INSTR(BCH_OPC_DELAY, 1)
#define BCH_OP_ERR			BCH_INSTR(BCH_OPC_CHECK, 0)
#define BCH_CACHE_ERR			BCH_INSTR(BCH_OPC_CHECK, 1)
#define BCH_ERROR			BCH_INSTR(BCH_OPC_CHECK, 2)
#define BCH_AL_EX_0			BCH_INSTR(BCH_OPC_ADDR, 0)
#define BCH_AL_EX_1			BCH_INSTR(BCH_OPC_ADDR, 1)
#define BCH_AL_EX_2			BCH_INSTR(BCH_OPC_ADDR, 2)
#define BCH_AL_EX_3			BCH_INSTR(BCH_OPC_ADDR, 3)
#define BCH_AL_AD_0			BCH_INSTR(BCH_OPC_ADDR, 4)
#define BCH_AL_AD_1			BCH_INSTR(BCH_OPC_ADDR, 5)
#define BCH_AL_AD_2			BCH_INSTR(BCH_OPC_ADDR, 6)
#define BCH_AL_AD_3			BCH_INSTR(BCH_OPC_ADDR, 7)
#define BCH_NEXT_CHIP_ON		BCH_INSTR(BCH_OPC_NEXT_CHIP_ON, 0)
#define BCH_DEC_JMP_MCS(x)		BCH_INSTR(BCH_OPC_DEC_JMP_MCS, (x))
#define BCH_ECC_SCORE(x)		BCH_INSTR(BCH_OPC_ECC_SCORE, (x))


/* BCH 'program' structure */
struct bch_prog {
	u32	multi_cs_addr[3];
	u32	multi_cs_config;
	u8	seq[16];
	u32	addr;
	u32	extra;
	u8	cmd[4];
	u32	reserved1;
	u32	gen_cfg;
	u32	delay;
	u32	reserved2;
	u32	seq_cfg;
} __attribute__((__packed__, aligned(32)));


/* BCH template programs (modified on-the-fly) */
static struct bch_prog bch_prog_read_page = {
	.cmd = {
		NAND_CMD_READ0,
		NAND_CMD_READSTART,
	},
	.seq = {
		BCH_ECC_SCORE(0),
		BCH_CMD_ADDR,
		BCH_CL_CMD_1,
		BCH_DATA_2_SECTOR,
		BCH_STOP,
	},
	.gen_cfg = (GEN_CFG_DATA_8_NOT_16 |
		    GEN_CFG_LAST_SEQ_NODE),
	.seq_cfg = SEQ_CFG_GO_STOP,
};

static struct bch_prog bch_prog_write_page = {
	.cmd = {
		NAND_CMD_SEQIN,
		NAND_CMD_PAGEPROG,
		NAND_CMD_STATUS,
	},
	.seq = {
		BCH_CMD_ADDR,
		BCH_DATA_4_SECTOR,
		BCH_CL_CMD_1,
		BCH_CL_CMD_2,
		BCH_OP_ERR,
		BCH_STOP,
	},
	.gen_cfg = (GEN_CFG_DATA_8_NOT_16 |
		    GEN_CFG_LAST_SEQ_NODE),
	.seq_cfg = (SEQ_CFG_GO_STOP |
		    SEQ_CFG_DATA_WRITE),
};


struct nand_bbt_inband_base_header
{
	uint8_t signature[4];		/* the traditional "Bbt0", or "1tbB" signature */
	uint8_t version;		/* BBT version ("age") */
	uint8_t reserved[3];		/* padding */
	uint8_t schema[4];		/* "base" schema version (quadruplicated) */
} __attribute__((__packed__, aligned(4)));

struct bch_nand_bbt_inband_header
{
	struct nand_bbt_inband_base_header base;
	uint8_t schema[4];		/* "vendor" schema version (quadruplicated) */
	uint8_t ecc_size[4];		/* ECC bytes (0, 32, or 54) per 1KiB section (quadruplicated) */
	uint8_t author[64];		/* arbitrary string for creating S/W to use */
} __attribute__((__packed__, aligned(64)));


/* Configure BCH read/write/erase programs */
static void bch_configure_progs(struct mtd_info * const mtd)
{
	const struct nand_chip * const chip = mtd->priv;
	const uint32_t page_size = mtd->writesize;
	const uint32_t sectors_per_page = page_size / NANDI_BCH_SECTOR_SIZE;
	const uint8_t data_opa = ffs(sectors_per_page) - 1;
	const uint8_t data_instr = BCH_INSTR(BCH_OPC_DATA, data_opa);
	const uint32_t gen_cfg_ecc = bch_ecc_mode << GEN_CFG_ECC_SHIFT;

	/* Set 'DATA' instruction */
	bch_prog_read_page.seq[3]  = data_instr;
	bch_prog_write_page.seq[1] = data_instr;

	/* Set ECC mode */
	bch_prog_read_page.gen_cfg  |= gen_cfg_ecc;
	bch_prog_write_page.gen_cfg |= gen_cfg_ecc;

	/* Need one extra address cycle for devices > 128MiB */
	if (chip->chipsize > (128 << 20))
	{
		bch_prog_read_page.gen_cfg  |= GEN_CFG_EXTRA_ADD_CYCLE;
		bch_prog_write_page.gen_cfg |= GEN_CFG_EXTRA_ADD_CYCLE;
	}
}


/* BCH operations */
static inline void bch_load_prog_cpu(const struct bch_prog * const prog)
{
	uint32_t       * to   = (uint32_t *)ST40_EMI_NAND_BCH_ADDRESS_REG_1;
	const uint32_t * from = (const uint32_t *)prog;
	int i = sizeof(struct bch_prog) / sizeof(uint32_t);

		/* load + execute the sequence by writing to the BCH registers */
	while(i--) {
		writel(*from++, to++);
	}
}

/* wail till BCH+DMA has finished with the current running program */
static inline void bch_wait_till_completion(void)
{
	/*
	 * Poll on the SEQ_NODES_OVER interrupt bit, as this is safer than
	 * just using the Sequence Status RUN_STATUS bit, as the DMA of page
	 * reads complete *after* the BCH engine has actually finished!
	 * We simply wait for SEQ_NODES_OVER to be asserted, and clear it afterwards.
	 */
	while ((readl(ST40_EMI_NAND_BCH_INT_STA) & NANDBCH_INT_SEQNODESOVER) == 0)
		;	/* do nothing */

	/* finally, clear the SEQ_NODES_OVER interrupt status bit, for next time */
	writel(NANDBCH_INT_CLR_SEQNODESOVER, ST40_EMI_NAND_BCH_INT_CLR);
}


/* Attempt to establish if a page is empty (likely to have been erased), while
 * tolerating a number of bits stuck at, or drifted to, 0.
 */
static int is_page_empty(
	const uint8_t * data,
	uint32_t page_size,
	const int max_bit_errors)
{
	int e = 0;

	while (page_size--) {
		e += hweight8(~*data++);
		if (e > max_bit_errors)
			return 0;	/* too many bytes not 0xFF */
	}

	return 1;			/* erased, with <= max_bit_errors zero-bits */
}


static void bch_init_dma_buffers(struct mtd_info * const mtd)
{
	struct nand_chip * const chip = mtd->priv;
	int bits = 0;

		/* set BCH "potency", based on number of ECC bytes/sector we have been given */
	if (chip->ecc.bytes == 32)
	{
		bch_ecc_mode = BCH_18BIT_ECC;	/* correct 18-bit/1KiB sector, uses 32 bytes */
		bits = 18;
	}
	else if (chip->ecc.bytes == 54)
	{
		bch_ecc_mode = BCH_30BIT_ECC;	/* correct 30-bit/1KiB sector, uses 54 bytes */
		bits = 30;
	}
	else if (chip->ecc.bytes == 0)
	{
		bch_ecc_mode = BCH_NO_ECC;	/* disables all ECC - not recommended! */
	}
	else
		BUG();
	printf("info: BCH controller initializing with ECC of %u-bits per 1KiB sector\n", bits);


	/* Tune BCH programs according to device found and ECC mode */
	bch_configure_progs(mtd);


	if (!dma_buffer)
	{
		/* allocate for both in-band and out-of-band data, for ONE single page */
		dma_buffer = (void*)NANDI_BCH_ALIGN_BUFFER(malloc(mtd->writesize + mtd->oobsize + NANDI_BCH_DMA_ALIGNMENT));
	}

	if (!dma_buffer_list)
	{
		/* allocate memory for a single list of NANDI_BCH_MAX_BUF_LIST DMA pointers */
		dma_buffer_list = (void*)NANDI_BCH_ALIGN_BUFFER(malloc(NANDI_BCH_BUF_LIST_SIZE + NANDI_BCH_DMA_ALIGNMENT));

		/* explicitly set all list items to zero */
		memset(dma_buffer_list, 0x00, NANDI_BCH_BUF_LIST_SIZE);

		/* write physical address of "buffer list" to BCH DMA controller */
		writel(PHYSADDR(dma_buffer_list), ST40_EMI_NAND_BCH_BUFFER_LIST_PTR);
	}

#if defined(DEBUG_BCH)
	printf("info: dma_buffer      = %p\t\t(%u+%u)\n", dma_buffer, mtd->writesize, mtd->oobsize);
	printf("info: dma_buffer_list = %p\t\t(%u)\n", dma_buffer_list, NANDI_BCH_BUF_LIST_SIZE);
#endif /* DEBUG_BCH */
}


static int bch_ecc_read_page (
	struct mtd_info * const mtd,
	struct nand_chip * const chip,
	uint8_t * const buf,
	int page)
{
	const uint32_t page_size = mtd->writesize;
	const uint32_t sectors_per_page = chip->ecc.steps;
	struct bch_prog * const prog = &bch_prog_read_page;
	int status = 0;
	uint8_t * buffer;
	uint8_t * const oobbuf = buf + page_size;
	uint8_t * bbt_buf;
	uint32_t ecc_err;
	struct bch_nand_bbt_inband_header * header;
		/* calculate the range of pages, corresponding to all the blocks reserved for the BBTs */
	const struct nand_bbt_descr * const descr = chip->bbt_td;	/* assume main BBT descriptor */
	const int pages_per_block = 1u << (chip->bbt_erase_shift - chip->page_shift);
	const int bbt_last_page  = (mtd->size >> chip->page_shift) - 1;
	const int bbt_first_page = bbt_last_page - (descr->maxblocks * pages_per_block) + 1;

#if defined(DEBUG_BCH)
	printf("info: bch_ecc_read_page(mtd=%p, chip=%p, buf=%p, page=%u) ... page_size = %u\n", mtd, chip, buf, page, page_size);
#endif /* DEBUG_BCH */

	/* Activate the BCH controller */
	emiss_nandi_select(STM_NANDI_BCH);

	if (!dma_buffer)			/* no buffers yet ? */
		bch_init_dma_buffers(mtd);	/* then allocate them ... */

	/* use a bounce buffer if we need to honour the DMA's alignment constraints */
	if (NANDI_BCH_IS_ALIGNED(buf))		/* correctly aligned ? */
		buffer = (void*)buf;
	else					/* use bounce buffer */
		buffer = dma_buffer;

	/* write the (shifted) page to the "read-page" program */
	prog->addr = page << 8;			/* shift "page" by 8-bits (irrespective of page_size) */

	/* put physical address of buffer into the "buffer list" (zeroth-item) */
	dma_buffer_list[0] = PHYSADDR(buffer) | (sectors_per_page - 1);

	/* Reset ECC stats */
	writel(CONFIG_SYS_RESET_ECC_ALL | CONFIG_SYS_ENABLE_AFM, ST40_EMI_NAND_BCH_CONTROLLER_CFG);
	writel(CONFIG_SYS_ENABLE_AFM, ST40_EMI_NAND_BCH_CONTROLLER_CFG);

	/* Make sure buffer (and buffer list) agrees with real memory */
	flush_cache((ulong)buffer, page_size);
	flush_cache((ulong)dma_buffer_list, NANDI_BCH_BUF_LIST_SIZE);

	/* now load + execute the BCH "read-page" program */
	bch_load_prog_cpu(prog);

#if defined(DEBUG_BCH)
	printf("info: BCH is reading page %u (0x%x) ...\n", page, page*page_size);
#endif /* DEBUG_BCH */

	/* and wait for it to finish ... */
	bch_wait_till_completion();

	/* finally, copy it out from the bounce buffer, if used */
	if (buffer != buf)
		memcpy(buf, buffer, page_size);

	/* Use the maximum per-sector ECC count! */
	ecc_err = readl(ST40_EMI_NAND_BCH_ECC_SCORE_REG_A) & 0xffu;
	if (ecc_err == 0xff)
	{
		/* Do we have a genuine uncorrectable ECC error, or is it just
		 * an erased page?
		 */
		if (is_page_empty(buf, page_size, sectors_per_page))
		{
			/* empty/erased page (probably) */
#if defined(DEBUG_BCH)
			printf("Warning: page %u (0x%x) has un-correctable errors -- looks to be erased!\n",
				page, page*page_size);
#endif /* DEBUG_BCH */
		}
		else
		{
			/* un-correctable */
			printf("ERROR: page %u (0x%x) has un-correctable errors!\n",
				page, page*page_size);
			status = ecc_err;
		}
		return status;		/* nothing more to do! */
	}
	else if (ecc_err == 0x00)
	{
		/* no errors at all! */
#if defined(DEBUG_BCH)
		printf("info: page %u (0x%x) has NO errors\n",
			page, page*page_size);
#endif /* DEBUG_BCH */
	}
	else
	{
		/* some bit errors, but are all correctable */
		printf("info: page %u (0x%x) has %u correctable errors\n",
			page, page*page_size, ecc_err);
	}


	/*
	 * If we are reading from a page in the region reserved for NAND-resident
	 * BBTs, then we may want do additional work ... otherwise exit
	 */
	if ( (page < bbt_first_page) || (page > bbt_last_page) )
		return status;			/* all done! */

	/* if it is the last page in the BBT region, then we have nothing more to do either */
	if ( (page & (pages_per_block-1)) == pages_per_block-1 )	/* last page */
	{
		return status;			/* all done! */
	}

	/*
	 * In fact, we really only expect to get here for each of the *first* pages
	 * in the last <chip->bbt_td->maxblocks> blocks!
	 * So, we will just BUG if it is some other page ...
	 */
	if ( (page & (pages_per_block-1)) != 0 )	/* first page in a block ? */
	{
		printf("Error: attempting to read from unexpected page (%u) in region reserved for BBTs!\n", page);
		BUG();
	}


	BUG_ON (descr->len       != chip->bbt_md->len);		/* paranoia! */
	BUG_ON (descr->offs      != chip->bbt_md->offs);	/* paranoia! */
	BUG_ON (descr->veroffs   != chip->bbt_md->veroffs);	/* paranoia! */
	BUG_ON (descr->maxblocks != chip->bbt_md->maxblocks);	/* paranoia! */

	/* allocate memory for one whole page */
	bbt_buf = malloc(page_size + NANDI_BCH_DMA_ALIGNMENT);
	BUG_ON (!bbt_buf);

	/* convenience pointer to DMA-aligned buffer */
	header = (struct bch_nand_bbt_inband_header*)NANDI_BCH_ALIGN_BUFFER(bbt_buf);

	page |= pages_per_block - 1;		/* last page in the block */

#if defined(DEBUG_BCH)
	printf("info: going to try and read BBT signature from page %u (0x%x) ...\n", page, page*page_size);
#endif /* DEBUG_BCH */

	/* now read what he hope is the BBT header from the *last* page in the same block */
	status = bch_ecc_read_page (mtd, chip, (void*)header, page);

	/*
	 * Copy a sub-set of info from "inband" into the OOB region in the caller's buffer
	 * i.e. fake-up the BBT "signature" by copying it from the real in-band one.
	 */
	memcpy(&oobbuf[descr->offs], header->base.signature, sizeof(header->base.signature));
	oobbuf[descr->veroffs] = header->base.version;

#if defined(DEBUG_BCH) && 0
{
	int i;
	for(i=0; i<64; i++)
		printf("read[%2u]  =  0x%02x  '%c'\t\t-> 0x%02x  '%c'\n",
			i,
			((unsigned char*)header)[i], isprint(((unsigned char*)header)[i]) ? ((unsigned char*)header)[i] : '.',
			oobbuf[i], isprint(oobbuf[i]) ? oobbuf[i] : '.');
}
#endif /* DEBUG_BCH */

	free(bbt_buf);

	return status;
}

extern int stm_bch_scan_read_raw(
	struct mtd_info * const mtd,
	uint8_t * const buf,
	const loff_t offs,
	size_t len)
{
	struct nand_chip * const chip = mtd->priv;
	const uint32_t page_size = mtd->writesize;
	const int page = offs >> chip->page_shift;

#if defined(DEBUG_BCH)
	printf("info: stm_bch_scan_read_raw(mtd=%p, buf=%p, offs=0x%x, len=%u) ...\n", mtd, buf, (unsigned)offs, len);
#endif /* DEBUG_BCH */

	BUG_ON(len != page_size);		/* paranoia */
	BUG_ON((offs & (page_size-1)));		/* paranoia */

	/* now read the page with the BCH controller ... */
	return bch_ecc_read_page (mtd, chip, buf, page);
}


static void bch_ecc_write_page (
	struct mtd_info * const mtd,
	struct nand_chip * const chip,
	const uint8_t * const buf)
{
#if defined(DEBUG_BCH)
	printf("info: bch_ecc_write_page(mtd=%p, chip=%p, buf=%p)\n", mtd, chip, buf);
#endif /* DEBUG_BCH */
		/*
		 * Note: I do not expect this function *ever* to be called.
		 * However, in theory we need chip->ecc.write_page to point to it!
		 * Probably bch_write_page() could/should call it!
		 * So, just in case, lets BUG() if we get here ...
		 */
	BUG();
}


/* write a single complete page, and let the H/W take care of all the ECC's needs */
static int bch_write_page (
	struct mtd_info * const mtd,
	struct nand_chip * const chip,
	const uint8_t * const buf,
	int page,
	int cached,
	int raw)
{
	const uint32_t page_size = mtd->writesize;
	const uint32_t sectors_per_page = chip->ecc.steps;
	struct bch_prog * const prog = &bch_prog_write_page;
	uint8_t status = 0;
	uint8_t * buffer;
	const uint8_t * const oobbuf = buf + page_size;
	int i;
	int is_main = 1, is_mirror = 1;		/* assume a good match */
	uint8_t * bbt_buf;
	struct bch_nand_bbt_inband_header * header;
		/* calculate the range of pages, corresponding to all the blocks reserved for the BBTs */
	const struct nand_bbt_descr * descr = chip->bbt_td	/* assume main BBT descriptor */;
	const int pages_per_block = 1u << (chip->bbt_erase_shift - chip->page_shift);
	const int bbt_last_page  = (mtd->size >> chip->page_shift) - 1;
	const int bbt_first_page = bbt_last_page - (descr->maxblocks * pages_per_block) + 1;

	const char author[] = "STMicroelectronics U-Boot (" CONFIG_IDENT_STRING ")";

#if defined(DEBUG_BCH)
	printf("info: bch_write_page(mtd=%p, chip=%p, buf=%p, page=%d, cached=%d, raw=%d) ... page_size = %u\n",
		mtd, chip, buf, page, cached, raw, page_size);
#endif /* DEBUG_BCH */

	BUG_ON(raw);

	/* Activate the BCH controller */
	emiss_nandi_select(STM_NANDI_BCH);

	if (!dma_buffer)			/* no buffers yet ? */
		bch_init_dma_buffers(mtd);	/* then allocate them ... */

	/* use a bounce buffer if we need to honour the DMA's alignment constraints */
	if (NANDI_BCH_IS_ALIGNED(buf))		/* correctly aligned ? */
		buffer = (void*)buf;
	else					/* use bounce buffer */
		buffer = memcpy(dma_buffer, buf, page_size);


	/* write the (shifted) page to the "write-page" program */
	prog->addr = page << 8;		/* shift "page" by 8-bits (irrespective of page_size) */

	/* put physical address of buffer into the "buffer list" (zeroth-item) */
	dma_buffer_list[0] = PHYSADDR(buffer) | (sectors_per_page - 1);

	/* Make sure buffer (and buffer list) agrees with real memory */
	flush_cache((ulong)buffer, page_size);
	flush_cache((ulong)dma_buffer_list, NANDI_BCH_BUF_LIST_SIZE);

	/* now load + execute the BCH "write-page" program */
	bch_load_prog_cpu(prog);

#if defined(DEBUG_BCH)
	printf("info: BCH is writing page %u (0x%x) ...\n", page, page*page_size);
#endif /* DEBUG_BCH */

	/* and wait for it to finish ... */
	bch_wait_till_completion();


	/*
	 * If we are writing to a page in the region reserved for NAND-resident
	 * BBTs, then we may want do additional work ... otherwise exit
	 */
	if ( (page < bbt_first_page) || (page > bbt_last_page) )
		return status;			/* all done! */

	/* if it is the last page in the BBT region, then we have nothing more to do either */
	if ( (page & (pages_per_block-1)) == pages_per_block-1 )	/* last page */
	{
		return status;			/* all done! */
	}

	/*
	 * In fact, we really only expect to get here for each of the *first* pages
	 * in the last <chip->bbt_td->maxblocks> blocks!
	 * So, we will just BUG if it is some other page ...
	 */
	if ( (page & (pages_per_block-1)) != 0 )	/* first page in a block ? */
	{
#if defined(DEBUG_BCH)
		printf("Error: attempting to read from unexpected page (%u) in region reserved for BBTs!\n", page);
#endif /* DEBUG_BCH */
		BUG();
	}


	/* need to determine if are we using the "main" or the "mirror" descriptor ...  */
	BUG_ON (descr->len       != chip->bbt_md->len);		/* paranoia! */
	BUG_ON (descr->offs      != chip->bbt_md->offs);	/* paranoia! */
	BUG_ON (descr->veroffs   != chip->bbt_md->veroffs);	/* paranoia! */
	BUG_ON (descr->maxblocks != chip->bbt_md->maxblocks);	/* paranoia! */
	for(i=0; i<descr->len; i++)
	{
		const uint8_t c = oobbuf[ descr->offs + i ];
		if (c != chip->bbt_td->pattern[i])
			is_main = 0;		/* not main BBT */
		if (c != chip->bbt_md->pattern[i])
			is_mirror = 0;		/* not mirror BBT */
#if defined(DEBUG_BCH)
		printf("debug: OOB[offs+%u]  =  0x%02x  :  0x%02x  :  0x%02x\tmain = %u  mirror = %u\n",
			i,
			oobbuf[ descr->offs + i ],
			chip->bbt_td->pattern[i],
			chip->bbt_md->pattern[i],
			is_main, is_mirror);
#endif /* DEBUG_BCH */
	}
	BUG_ON (is_main == is_mirror);		/* exactly ONE should be matched */

	/* so, are we using the "main" or the "mirror" descriptor ? */
	if (is_mirror)
		descr = chip->bbt_md;		/* use mirror descriptor instead */

	/* allocate memory for one whole page */
	bbt_buf = malloc(page_size + NANDI_BCH_DMA_ALIGNMENT);
	BUG_ON (!bbt_buf);

	/* convenience pointer to DMA-aligned buffer */
	header = (struct bch_nand_bbt_inband_header*)NANDI_BCH_ALIGN_BUFFER(bbt_buf);

	page |= pages_per_block - 1;		/* last page in the block */

#if defined(DEBUG_BCH)
	printf("info: going to write BBT signature to page %u (0x%x) ...\n", page, page*page_size);
#endif /* DEBUG_BCH */

	/* fill in the BBT descriptor's fields, prior to writing it */
	memset(header, 0xff, page_size);	/* first, set it to all ones */
	memcpy(header->base.signature, descr->pattern, sizeof(header->base.signature));
	header->base.version = oobbuf[ descr->veroffs ];
	header->base.schema[0] = header->base.schema[1] = header->base.schema[2] = header->base.schema[3] = 0x10;	/* Version 1.0 */
	header->schema[0] = header->schema[1] = header->schema[2] = header->schema[3] = 0x10;				/* Version 1.0 */
	header->ecc_size[0] = header->ecc_size[1] = header->ecc_size[2] = header->ecc_size[3] = chip->ecc.bytes;
	memcpy(header->author, author, sizeof(author));

	/* now write the new BBT descriptor to the *last* page in the same block */
	status = bch_write_page(mtd, chip, (void*)header, page, cached, raw);

	free(bbt_buf);

	return status;				/* finally, all done! */
}


/*************************************************************************
 *******************	Configure MTD/NAND Interface	******************
 *************************************************************************/


static void nandi_set_mtd_defaults(
	struct mtd_info * const mtd,
	struct nand_chip * const chip)
{
	/*
	 * Over-write the function pointers in the 'nand_chip'
	 * structure, to use our NANDi H/W-specific ones instead.
	 */

	/* Hamming FLEX functions */
	chip->dev_ready          = stm_flex_device_ready;
	chip->cmdfunc            = flex_command_lp;
	chip->read_byte          = flex_read_byte;
	chip->select_chip        = flex_select_chip;
	chip->waitfunc           = flex_wait_func;
	chip->read_buf           = flex_read_buf;
	chip->write_buf          = flex_write_buf;

	/* BCH functions */
	chip->ecc.read_page      = bch_ecc_read_page;
	chip->ecc.write_page     = bch_ecc_write_page;
	chip->write_page         = bch_write_page;
}


extern void stm_bch_init_nand(
	struct mtd_info * const mtd,
	struct nand_chip * const chip)
{
	int emi_bank = 0;		/* use first CSn */
#if defined(CONFIG_SYS_NAND_FLEX_CSn_MAP)
	const int csn_map[CONFIG_SYS_MAX_NAND_DEVICE] = CONFIG_SYS_NAND_FLEX_CSn_MAP;
#endif	/* CONFIG_SYS_NAND_FLEX_CSn_MAP */

#if defined(CONFIG_SYS_NAND_FLEX_CSn_MAP)
	/* Re-map to different CSn if needed */
	emi_bank = csn_map[emi_bank];
#endif	/* CONFIG_SYS_NAND_FLEX_CSn_MAP */

	/* initialize the BCH controller H/W */
	nandi_init_bch(emi_bank);

	/* initialize the Hamming controller H/W */
	nandi_init_hamming(emi_bank);

	/* Configure MTD/NAND interface */
	nandi_set_mtd_defaults(mtd, chip);

}

#endif	/* CONFIG_CMD_NAND && CONFIG_SYS_ST40_NAND_USE_BCH */


