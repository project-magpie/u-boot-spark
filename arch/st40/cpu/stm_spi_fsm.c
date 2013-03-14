/*
 * (C) Copyright 2010-2012 STMicroelectronics.
 *
 * Angus Clark   <Angus.Clark@st.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/soc.h>
#include <asm/socregs.h>
#include <asm/io.h>
#include <spi.h>
#include <asm/clk.h>
#include <asm/spi-commands.h>
#include "stm_spi_fsm.h"


/**********************************************************************/


#if defined(CONFIG_STM_FSM_SPI)


/**********************************************************************/


#undef DEBUG
#if 0
#define DEBUG(args...)					\
	do {						\
		printf(args);				\
	} while (0)
#else
#define DEBUG(args...) do {} while (0)
#endif

/*
 * Define the following macro to define the helper function
 * fsm_read_flag_status(), to read the FLAG register.
 * This is usually only for DEBUG purposes.
 */
//#define CONFIG_STM_SPI_READ_FLAG_FUNCTION


/**********************************************************************/


#define ENABLE_ASSERTS

#if defined(ENABLE_ASSERTS)

static const char assert_message[] =
	"ERROR: Assertion failed in %s() @ %s:%u\n";

#define assert(x)					\
	do {						\
		if (!(x)) {				\
			printf(				\
				assert_message,		\
				__FUNCTION__,		\
				__FILE__,		\
				__LINE__);		\
		}					\
	} while (0)
#else	/* ENABLE_ASSERTS */

#define assert(x) do {} while (0)

#endif	/* ENABLE_ASSERTS */


/**********************************************************************/


static const unsigned long base_addr = CONFIG_SYS_STM_SPI_FSM_BASE;	/* SPI base */

#define fsm_write_reg(reg, val)		writel((val), base_addr + (reg))
#define fsm_read_reg(reg)		readl(base_addr + (reg))


#define mdelay(n) ({unsigned long msec=(n); while (msec--) udelay(1000);})


#define FLASH_PAGESIZE		256	/* QQQ - Why not 512 ??? */
static uint8_t	page_buf[FLASH_PAGESIZE] __attribute__((aligned(4)));


/**********************************************************************/


struct fsm_seq {
	      uint32_t data_size;
	      uint32_t addr1;
	      uint32_t addr2;
	      uint32_t addr_cfg;
	const uint32_t seq_opc[5];
	const uint32_t mode;
	const uint32_t dummy;
	      uint32_t status;
	const uint8_t  seq[16];
	const uint32_t seq_cfg;
} __attribute__((__packed__,aligned(4)));

#define FSM_SEQ_SIZE		sizeof(struct fsm_seq)


/**********************************************************************/


static const struct fsm_seq seq_read_jedec = {
	.data_size = TRANSFER_SIZE(8),
	.seq_opc[0] = SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_READ_DEVID),
	.seq = {
		FSM_INST_CMD1,
		FSM_INST_DATA_READ,
		FSM_INST_STOP,
	},
	.seq_cfg = (SEQ_CFG_PADS_1 |
		    SEQ_CFG_READNOTWRITE |
		    SEQ_CFG_CSDEASSERT |
		    SEQ_CFG_STARTSEQ),
};

static const struct fsm_seq seq_read_status = {
	.data_size = TRANSFER_SIZE(4),
	.seq_opc[0] = SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_READ_STATUS),
	.seq = {
		FSM_INST_CMD1,
		FSM_INST_DATA_READ,
		FSM_INST_STOP,
	},
	.seq_cfg = (SEQ_CFG_PADS_1 |
		    SEQ_CFG_READNOTWRITE |
		    SEQ_CFG_CSDEASSERT |
		    SEQ_CFG_STARTSEQ),
};

#if defined(CONFIG_SPI_FLASH_ST) && defined(CONFIG_STM_SPI_READ_FLAG_FUNCTION)
static const struct fsm_seq seq_read_flag_status = {
	.data_size = TRANSFER_SIZE(4),
	.seq_opc[0] = SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_READ_FLAG_STATUS),
	.seq = {
		FSM_INST_CMD1,
		FSM_INST_DATA_READ,
		FSM_INST_STOP,
	},
	.seq_cfg = (SEQ_CFG_PADS_1 |
		    SEQ_CFG_READNOTWRITE |
		    SEQ_CFG_CSDEASSERT |
		    SEQ_CFG_STARTSEQ),
};
#endif	/* CONFIG_SPI_FLASH_ST && CONFIG_STM_SPI_READ_FLAG_FUNCTION */

#if defined(CONFIG_SPI_FLASH_ST)
static struct fsm_seq seq_enter_4byte_mode = {
	.seq_opc = {
		/* Note: for seq_opc[0], "| SEQ_OPC_OPCODE(cmd)" is done in fsm_enter_4byte_mode() */
		SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8)                           | SEQ_OPC_CSDEASSERT,
		SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_WREN) | SEQ_OPC_CSDEASSERT,
	},
	.seq = {
		FSM_INST_CMD2,			/* Note the order: seq_opc[1], before seq_opc[0] */
		FSM_INST_CMD1,
		FSM_INST_WAIT,
		FSM_INST_STOP,
	},
	.seq_cfg = (SEQ_CFG_PADS_1 |
		    SEQ_CFG_ERASE |		/* Needed by FSM controller! */
		    SEQ_CFG_READNOTWRITE |
		    SEQ_CFG_CSDEASSERT |
		    SEQ_CFG_STARTSEQ),
};
#endif	/* CONFIG_SPI_FLASH_ST */

#if defined(CONFIG_SPI_FLASH_SPANSION)
static struct fsm_seq seq_read_dyb = {
	.data_size = TRANSFER_SIZE(4),
	.seq_opc[0] = SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_DYB_READ),
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
	.addr_cfg = ADR_CFG_PADS_1_ADD1 | ADR_CFG_CYCLES_ADD1(32) | ADR_CFG_ADDR1_32_BIT,
#else
	.addr_cfg = ADR_CFG_PADS_1_ADD1 | ADR_CFG_CYCLES_ADD1(16)
		  | ADR_CFG_PADS_1_ADD2 | ADR_CFG_CYCLES_ADD2(16),
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
	.seq = {
		FSM_INST_CMD1,
		FSM_INST_ADD1,
#if !defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
		FSM_INST_ADD2,
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
		FSM_INST_DATA_READ,
		FSM_INST_STOP,
	},
	.seq_cfg = (SEQ_CFG_PADS_1 |
		    SEQ_CFG_READNOTWRITE |
		    SEQ_CFG_CSDEASSERT |
		    SEQ_CFG_STARTSEQ),
};
#endif	/* CONFIG_SPI_FLASH_SPANSION */

#if defined(CONFIG_SPI_FLASH_SPANSION)
static struct fsm_seq seq_write_dyb = {
	.seq_opc = {
		(SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_WREN) | SEQ_OPC_CSDEASSERT),
		(SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_DYB_WRITE)),
	},
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
	.addr_cfg = ADR_CFG_PADS_1_ADD1 | ADR_CFG_CYCLES_ADD1(32) | ADR_CFG_ADDR1_32_BIT,
#else
	.addr_cfg = ADR_CFG_PADS_1_ADD1 | ADR_CFG_CYCLES_ADD1(16)
		  | ADR_CFG_PADS_1_ADD2 | ADR_CFG_CYCLES_ADD2(16),
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
	.status = STA_PADS_1 | STA_CSDEASSERT /* | STA_DATA_BYTE1(dyb) */,
	.seq = {
		FSM_INST_CMD1,
		FSM_INST_CMD2,
		FSM_INST_ADD1,
#if !defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
		FSM_INST_ADD2,
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
		FSM_INST_STA_WR1,
		FSM_INST_STOP,
	},
	.seq_cfg = (SEQ_CFG_PADS_1 |
		    SEQ_CFG_READNOTWRITE |
		    SEQ_CFG_CSDEASSERT |
		    SEQ_CFG_STARTSEQ),
};
#endif	/* CONFIG_SPI_FLASH_SPANSION */

static struct fsm_seq seq_read_data = {
	.seq_opc[0] = SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_READ_ARRAY),
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
	.addr_cfg = ADR_CFG_PADS_1_ADD1 | ADR_CFG_CYCLES_ADD1(24),
#else
	.addr_cfg = ADR_CFG_PADS_1_ADD1 | ADR_CFG_CYCLES_ADD1(16)
		  | ADR_CFG_PADS_1_ADD2 | ADR_CFG_CYCLES_ADD2(8),
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
	.seq = {
		FSM_INST_CMD1,
		FSM_INST_ADD1,
#if !defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
		FSM_INST_ADD2,
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
		FSM_INST_DATA_READ,
		FSM_INST_STOP,
	},
	.seq_cfg = (SEQ_CFG_PADS_1 |
		    SEQ_CFG_READNOTWRITE |
		    SEQ_CFG_CSDEASSERT |
		    SEQ_CFG_STARTSEQ),
};

static struct fsm_seq seq_write_data = {
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
	.addr_cfg = ADR_CFG_PADS_1_ADD1 | ADR_CFG_CYCLES_ADD1(24),
#else
	.addr_cfg = ADR_CFG_PADS_1_ADD1 | ADR_CFG_CYCLES_ADD1(16)
		  | ADR_CFG_PADS_1_ADD2 | ADR_CFG_CYCLES_ADD2(8),
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
	.seq_opc = {
		(SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_WREN) | SEQ_OPC_CSDEASSERT),
		(SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_PP)),
	},
	.seq = {
		FSM_INST_CMD1,
		FSM_INST_CMD2,
		FSM_INST_ADD1,
#if !defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
		FSM_INST_ADD2,
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
		FSM_INST_DATA_WRITE,
		FSM_INST_WAIT,
		FSM_INST_STOP,
	},
	.seq_cfg = (SEQ_CFG_PADS_1 |
		    SEQ_CFG_CSDEASSERT |
		    SEQ_CFG_STARTSEQ),
};

static struct fsm_seq seq_erase_sector = {
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
	.addr_cfg = ADR_CFG_PADS_1_ADD1 | ADR_CFG_CYCLES_ADD1(24) | ADR_CFG_CSDEASSERT_ADD1,
#else
	.addr_cfg = ADR_CFG_PADS_1_ADD1 | ADR_CFG_CYCLES_ADD1(16)
		  | ADR_CFG_PADS_1_ADD2 | ADR_CFG_CYCLES_ADD2(8)
		  | ADR_CFG_CSDEASSERT_ADD2,
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
	.seq_opc = {
		(SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_WREN) | SEQ_OPC_CSDEASSERT),
		(SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_SE)),
	},
	.seq = {
		FSM_INST_CMD1,
		FSM_INST_CMD2,
		FSM_INST_ADD1,
#if !defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
		FSM_INST_ADD2,
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
		FSM_INST_WAIT,
		FSM_INST_STOP,
	},
	.seq_cfg = (SEQ_CFG_PADS_1 |
		    SEQ_CFG_ERASE |
		    SEQ_CFG_READNOTWRITE |
		    SEQ_CFG_CSDEASSERT |
		    SEQ_CFG_STARTSEQ),
};

#if 0
static const struct fsm_seq seq_erase_chip = {
	.seq_opc = {
		(SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(OP_WREN) | SEQ_OPC_CSDEASSERT),
		(SEQ_OPC_PADS_1 | SEQ_OPC_CYCLES(8) | SEQ_OPC_OPCODE(FLASH_CMD_CHIPERASE)),
	},
	.seq = {
		FSM_INST_CMD1,
		FSM_INST_CMD2,
		FSM_INST_WAIT,
		FSM_INST_STOP,
	},
	.seq_cfg = (SEQ_CFG_PADS_1 |
		    SEQ_CFG_ERASE |
		    SEQ_CFG_READNOTWRITE |
		    SEQ_CFG_CSDEASSERT |
		    SEQ_CFG_STARTSEQ),
};
#endif


/**********************************************************************/


static inline int fsm_is_idle(void)
{
	return fsm_read_reg(SPI_FAST_SEQ_STA) & 0x10;
}

static inline int fsm_fifo_available(void)
{
	return fsm_read_reg(SPI_FAST_SEQ_STA) >> 5 & 0x7f;
}

static inline int fsm_set_mode(const uint32_t mode)
{
	while ((fsm_read_reg(SPI_STA_MODE_CHANGE) & 0x1) == 0);

	fsm_write_reg(SPI_MODESELECT, mode);

	return 0;
}

static void fsm_load_seq(const struct fsm_seq * const seq)
{
	uint32_t *a = (uint32_t *)((uint32_t)base_addr + SPI_FAST_SEQ_TRANSFER_SIZE);
	const uint32_t *s = (const uint32_t *)seq;
	int i = FSM_SEQ_SIZE/4;

		/* ensure the FSM is "idle" (not executing) */
	assert( fsm_is_idle() );

		/* program the sequence by writing to the FSM registers */
	while(i--) {
		writel(*s++, a++);
	}
}

static int fsm_wait_seq(void)
{
	int i = 0;

	while (!(fsm_is_idle()) && (i < 1000000)) {
		i++;
	}

	if (i == 1000000) {
		DEBUG("warning: in %s(), waited too long, reg=0x%08x\n",
			__func__,
			fsm_read_reg(SPI_FAST_SEQ_STA));
		mdelay(5);
	}

	return 0;
}

#if 0
static void fsm_clear_fifo(void)
{
	uint32_t avail;

	while ((avail = fsm_fifo_available()) > 0) {
		while (avail) {
			fsm_read_reg(SPI_FAST_SEQ_DATA_REG);
			avail--;
		}
	}
}
#endif

static int fsm_read_fifo(uint32_t *buf, const uint32_t size)
{
	uint32_t avail;
	uint32_t remaining = size/4;
	uint32_t words;
	uint32_t word;

		/* ensure parameters are 4-byte aligned */
	assert( (size & 0x3u) == 0u );
	assert( ((uint32_t)buf & 0x3u) == 0u );

	do {
		while (!(avail = fsm_fifo_available()))
			;	/* do nothing */

		words = min(avail, remaining);
		remaining -= words;
		avail -= words;
		while (words--)
		{
			word = fsm_read_reg(SPI_FAST_SEQ_DATA_REG);
			*buf++ = word;
		}
	} while (remaining);

	return (size - (remaining*4));
}

static int fsm_write_fifo(const uint32_t *buf, const uint32_t size)
{
	uint32_t remaining = size >> 2;

		/* ensure parameters are 4-byte aligned */
	assert( (size & 0x3u) == 0u );
	assert( ((uint32_t)buf & 0x3u) == 0u );

	while (remaining--) {
		fsm_write_reg(SPI_FAST_SEQ_DATA_REG, *buf);
		buf++;
	}

	return (size - (remaining*4));
}

extern uint8_t fsm_read_status(void)
{
	const struct fsm_seq * const seq = &seq_read_status;
	uint8_t status[4];

	fsm_load_seq(seq);
	fsm_read_fifo((uint32_t *)status, sizeof(status));

	/* Wait for FSM sequence to finish */
	fsm_wait_seq();

		/* return the LAST byte retrieved */
	return status[sizeof(status)-1u];
}


/**********************************************************************/


#if defined(CONFIG_SPI_FLASH_ST) && defined(CONFIG_STM_SPI_READ_FLAG_FUNCTION)
extern uint8_t fsm_read_flag_status(void)
{
	const struct fsm_seq * const seq = &seq_read_flag_status;
	uint8_t status[4];

	fsm_load_seq(seq);
	fsm_read_fifo((uint32_t *)status, sizeof(status));

	/* Wait for FSM sequence to finish */
	fsm_wait_seq();

		/* return the LAST byte retrieved */
	return status[sizeof(status)-1u];
}
#endif	/* CONFIG_SPI_FLASH_ST && CONFIG_STM_SPI_READ_FLAG_FUNCTION */


/**********************************************************************/


extern int fsm_read_jedec(const size_t bytes, uint8_t *const jedec)
{
	const struct fsm_seq * const seq = &seq_read_jedec;
	uint8_t tmp[8];		/* multiple of 4 */

	assert( bytes <= sizeof(tmp) );

	fsm_load_seq(seq);
	fsm_read_fifo((uint32_t *)tmp, sizeof(tmp));

	memcpy(jedec, tmp, bytes);

	return 0;
}


/**********************************************************************/


#if defined(CONFIG_SPI_FLASH_ST)
/*
 * ENTER "4-BYTE ADDRESS" mode (and leave the pervasive 3-byte address mode),
 * or (if enter == 0),
 * EXIT "4-BYTE ADDRESS" mode (and enter the pervasive 3-byte address mode).
 *
 * For Micron, we (sometimes) need to issue a WREN first!
 */
extern void fsm_enter_4byte_mode(const int enter)
{
	struct fsm_seq * const seq = &seq_enter_4byte_mode;
	const uint32_t cmd = enter ? OP_ENTER_4BYTE : OP_EXIT_4BYTE;

	DEBUG("debug: in %s( enter=%u )\n",
		__FUNCTION__, enter);

	/* over-write the default enter/exit op-code - this is dirty! */
	*(volatile uint8_t*)&seq->seq_opc[0] = cmd;

	/* Now load + execute the FSM sequence */
	fsm_load_seq(seq);

	/* Wait for FSM sequence to finish */
	fsm_wait_seq();
}
#endif	/* CONFIG_SPI_FLASH_ST */


/**********************************************************************/


extern int fsm_erase_sector(const uint32_t offset)
{
	struct fsm_seq * const seq = &seq_erase_sector;

	DEBUG("debug: in %s( offset=0x%x )\n",
		__FUNCTION__, offset);

	/* First, we customise the FSM sequence */
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
	seq->addr1 = offset;
#else
	seq->addr1 = (offset >> 16) & 0xffffu;
	seq->addr2 = (offset >>  0) & 0xffffu;
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */

	/* Now load + execute the FSM sequence */
	fsm_load_seq(seq);

	/* Wait for FSM sequence to finish */
	fsm_wait_seq();

	/* Wait for SPI device to complete */
	spi_wait_till_ready(NULL);

	return 0;
}


/**********************************************************************/


#if 0
static int fsm_erase_chip(void)
{
	const struct fsm_seq * const seq = &seq_erase_chip;

	fsm_load_seq(seq);

	/* Wait for FSM sequence to finish */
	fsm_wait_seq();

	/* Wait for SPI device to complete */
	spi_wait_till_ready(NULL);

	return 0;
}
#endif

extern int fsm_read(uint8_t * buf, const uint32_t bufsize, uint32_t offset)
{
	struct fsm_seq * const seq = &seq_read_data;
	uint32_t size_ub;
	uint32_t size_lb;
	uint32_t size_mop;
	uint32_t tmp;
	uint8_t *p;
	uint32_t size, remaining=bufsize;

	/* Handle non-aligned buf */
	if ((uint32_t)buf & 0x3) {
		p = page_buf;	/* use a 4-byte aligned bounce buffer */
	} else {
		p = buf;	/* use the provided buffer */
	}

	do {
		/* no more than one FIFO at a time */
		size = min(remaining,sizeof(page_buf));

		DEBUG("debug: in %s() p=%p, size=0x%x, remaining=0x%x, offset=0x%x\n",
			__FUNCTION__, p, size, remaining, offset);

		/* Handle non-aligned size */
		size_ub = (size + 0x3) & ~0x3;
		size_lb = size & ~0x3;
		size_mop = size & 0x3;

		seq->data_size = TRANSFER_SIZE(size_ub);	/* in bits! */
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
		seq->addr1 = offset;
#else
		seq->addr1 = (offset >> 16) & 0xffffu;
		seq->addr2 = (offset >>  0) & 0xffffu;
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */

		fsm_load_seq(seq);

		fsm_read_fifo((uint32_t *)p, size_lb);

		if (size_mop) {
			fsm_read_fifo(&tmp, sizeof(tmp));
			memcpy(p + size_lb, &tmp, size_mop);
		}

		/* Handle non-aligned buf */
		if (page_buf == p) {	/* used bounce buffer ? */
			memcpy(buf, p, size);
			buf+=size;
		} else {		/* used provided buffer */
			p += size;
		}

		/* prepare for next iteration */
		offset += size;
		remaining -= size;

		/* Wait for FSM sequence to finish */
		fsm_wait_seq();

	} while (remaining);

	return 0;
}

extern int fsm_write(const uint8_t * const buf, const uint32_t bufsize, uint32_t offset)
{
	struct fsm_seq * const seq = &seq_write_data;
	uint32_t size_ub;
	uint32_t size_lb;
	uint32_t size_mop;
	int i;
	const uint8_t *p=buf;
	uint32_t size, remaining=bufsize;
	uint32_t tmp;
	uint8_t *t = (uint8_t *)&tmp;

		/* ensure the FSM is "idle" (not executing) */
	assert( fsm_is_idle() );

	do {
		/* no more than one FIFO at a time */
		size = min(remaining,sizeof(page_buf));

		DEBUG("debug: in %s() p=%p, size=0x%x, remaining=0x%x, offset=0x%x\n",
			__FUNCTION__, p, size, remaining, offset);

		/* Handle non-aligned buf */
		if ((uint32_t)buf & 0x3) {
			p = page_buf;	/* use a 4-byte aligned bounce buffer */
			memcpy(page_buf, buf, size);
		} else {
					/* use the provided buffer */
		}

		/* Handle non-aligned size */
		size_ub = (size + 0x3) & ~0x3;
		size_lb = size & ~0x3;
		size_mop = size & 0x3;

		seq->data_size = TRANSFER_SIZE(size_ub);	/* in bits! */
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
		seq->addr1 = offset;
#else
		seq->addr1 = (offset >> 16) & 0xffffu;
		seq->addr2 = (offset >>  0) & 0xffffu;
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */

		/* prepare to write the data to the FIFO in the FSM */
		/* ensure at least SPI_FAST_SEQ_CFG[0,5,7] are all zero */
		fsm_write_reg(SPI_FAST_SEQ_CFG, 0x0);
		/* force the write to complete before proceeding! */
		(void)fsm_read_reg(SPI_FAST_SEQ_CFG);

		/* now, write the data to the FIFO in the FSM */
		fsm_write_fifo((uint32_t *)p, size_lb);
		p += size_lb;
		/* Handle non-aligned size */
		if (size_mop) {
			tmp = ~0ul;	/* fill with 0xFF's */
			for (i = 0; i < size_mop; i++)
				t[i] = *p++;
			fsm_write_fifo(&tmp, sizeof(tmp));
		}

		/* Start sequence */
		fsm_load_seq(seq);

		/* prepare for next iteration */
		offset += size;
		remaining -= size;

		/* Wait for FSM sequence to finish */
		fsm_wait_seq();

		/* Wait for SPI device to complete */
		spi_wait_till_ready(NULL);

	} while (remaining);

	return 0;
}


#if !defined(CONFIG_SYS_STM_SPI_CLOCKDIV)
#define CONFIG_SYS_STM_SPI_CLOCKDIV	2	/* default is SPI_CLOCKDIV = 2 */
#endif

extern int fsm_init(void)
{
	DEBUG("debug: in %s()\n", __FUNCTION__);

		/* perform a soft reset of the FSM controller */
	fsm_write_reg(SPI_FAST_SEQ_CFG, SEQ_CFG_SWRESET);
	udelay(1);	/* QQQ - is this long enough ??? */
	fsm_write_reg(SPI_FAST_SEQ_CFG, 0);

	fsm_write_reg(SPI_CLOCKDIV, CONFIG_SYS_STM_SPI_CLOCKDIV);

		/* select Fast Sequence Mode (FSM) */
	fsm_set_mode(1<<3);

	fsm_write_reg(SPI_CONFIGDATA, 0x00a00aa1);
	fsm_write_reg(SPI_PROGRAM_ERASE_TIME, 0x00000002);

	assert( fsm_fifo_available() == 0 );
/*	fsm_clear_fifo();	*/

	return 0;
}


/**********************************************************************/


#if defined(CONFIG_SPI_FLASH_ST) || defined(CONFIG_SPI_FLASH_SPANSION)
/*
 * The probed SPI device is >16MiB, so we want to use 32-bit (4-byte)
 * addresses to access everything above the first 16MiB.
 * We need now to "re-configure" the default FSM sequences to deal
 * with the consequences of larger (32-bit) address sizes.
 */
extern void fsm_init_4byte_mode(
	const unsigned char op_read,
	const unsigned char op_write,
	const unsigned char op_erase)
{
	/*
	 * Note: the use of the bit-field ADR_CFG_ADDRx_32_BIT is supported
	 * only on some SoCs (such as Lille/Liege), but not on others (such
	 * as the STx7105, and STx7108). To support 32-bit addresses on
	 * all SoCs with the FSM, one should use 2 separate address commands
	 * with 16+16 bits; or use 16+8 bits for 24-bit addresses.
	 *
	 * If CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES is defined, then
	 * we will always use a single ADDR1 FSM instruction, with 24 or
	 * 32 bits, as required.
	 *
	 * If CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES is NOT defined,
	 * then we will always use a pair of ADDR1+ADDR2 FSM instructions,
	 * with 16+8, or 16+16 bits, as required.
	 */
	const uint32_t addr_xor =
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
		(ADR_CFG_CYCLES_ADD1(24) ^ ADR_CFG_CYCLES_ADD1(32))
		| ADR_CFG_ADDR1_32_BIT;
#else
		(ADR_CFG_CYCLES_ADD2(8) ^ ADR_CFG_CYCLES_ADD2(16));
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */

	DEBUG("debug: in %s( op_read=0x%02x, op_write=0x%02x, op_erase=0x%02x )\n",
		__FUNCTION__, op_read, op_write, op_erase);

	/* over-write the default op-codes for read/write/erase - this is dirty! */
	*(volatile uint8_t*)&seq_read_data.seq_opc[0]    = op_read;
	*(volatile uint8_t*)&seq_write_data.seq_opc[1]   = op_write;
	*(volatile uint8_t*)&seq_erase_sector.seq_opc[1] = op_erase;

	/* change the number of address cycles from 24 to 32 bits */
	seq_read_data.addr_cfg    ^= addr_xor;
	seq_write_data.addr_cfg   ^= addr_xor;
	seq_erase_sector.addr_cfg ^= addr_xor;
}
#endif	/* CONFIG_SPI_FLASH_ST || CONFIG_SPI_FLASH_SPANSION */


/**********************************************************************/


#if defined(CONFIG_SPI_FLASH_SPANSION)
/*
 * read the SPI slave device's "DYB Access Register".
 *
 *	input:   offset, the sector whose DYB access register is to be accessed.
 *	returns: the value of the DYB access register.
 */
extern unsigned int fsm_read_dyb_access(
	const unsigned long offset)
{
	struct fsm_seq * const seq = &seq_read_dyb;
	uint8_t tmp[4];

#if 0
	DEBUG("debug: in %s( offset=0x%lx )\n",
		__FUNCTION__, offset);
#endif

	/* First, we customise the FSM sequence */
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
	seq->addr1 = offset;
#else
	seq->addr1 = (offset >> 16) & 0xffffu;
	seq->addr2 = (offset >>  0) & 0xffffu;
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */

	/* Now load + execute the FSM sequence */
	fsm_load_seq(seq);

	/* Read the captured data out from the FIFO */
	fsm_read_fifo((uint32_t *)tmp, sizeof(tmp));

	/* Wait for FSM sequence to finish */
	fsm_wait_seq();

	/* return the LAST byte retrieved */
	return tmp[sizeof(tmp)-1u];
}
#endif	/* CONFIG_SPI_FLASH_SPANSION */


/**********************************************************************/


#if defined(CONFIG_SPI_FLASH_SPANSION)
/*
 * write to the SPI slave device's "DYB Access Register".
 *
 *	input:   offset, the sector whose DYB access register is to be accessed.
 *		 dyb, the value that will be written to the DYB access register.
 *	returns: none.
 */
extern void fsm_write_dyb_access(
	const unsigned long offset,
	const unsigned char dyb)
{
	struct fsm_seq * const seq = &seq_write_dyb;

#if 0
	DEBUG("debug: in %s( offset=0x%lx, dyb=0x%02x )\n",
		__FUNCTION__, offset, dyb);
#endif

	/* First, we customise the FSM sequence */
#if defined(CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES)
	seq->addr1  = offset;
#else
	seq->addr1  = (offset >> 16) & 0xffffu;
	seq->addr2  = (offset >>  0) & 0xffffu;
#endif	/* CONFIG_STM_FSM_SUPPORTS_32_BIT_ADDRESSES */
	seq->status = STA_DATA_BYTE1(dyb) | STA_PADS_1 | STA_CSDEASSERT;

	/* Now load + execute the FSM sequence */
	fsm_load_seq(seq);

	/* Wait for FSM sequence to finish */
	fsm_wait_seq();

	/* Wait for SPI device to complete */
	spi_wait_till_ready(NULL);

	return;
}
#endif	/* CONFIG_SPI_FLASH_SPANSION */


/**********************************************************************/


#endif	/* defined(CONFIG_STM_FSM_SPI) */


