/*
 * Based on linux/drivers/mmc/host/sdhci.c - Secure Digital Host
 * Controller Interface driver
 * Copyright (C) 2005-2008 Pierre Ossman, All Rights Reserved.
 *
 * (C) Copyright 2010
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
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
#include <mmc.h>
#include <asm/io.h>
#include "spear_mmc.h"

struct spear_mmc {
	void			*ioaddr;	/* Mapped address */
	unsigned int		max_clk;	/* Max possible freq (MHz) */

	unsigned int		clock;		/* Current clock (MHz) */
	u8			pwr;		/* Current voltage */
};

static void sdhci_dumpregs(struct mmc *mmc)
{
	struct spear_mmc *host = (struct spear_mmc *)mmc->priv;
	const ulong base_addr = (ulong)host->ioaddr;

	printf(": ============== REGISTER DUMP ==============\n");

	printf(": Sys addr: 0x%08x | Version:  0x%08x\n",
		readl(base_addr + SDHCI_DMA_ADDRESS),
		readw(base_addr + SDHCI_HOST_VERSION));
	printf(": Blk size: 0x%08x | Blk cnt:  0x%08x\n",
		readw(base_addr + SDHCI_BLOCK_SIZE),
		readw(base_addr + SDHCI_BLOCK_COUNT));
	printf(": Argument: 0x%08x | Trn mode: 0x%08x\n",
		readl(base_addr + SDHCI_ARGUMENT),
		readw(base_addr + SDHCI_TRANSFER_MODE));
	printf(": Present:  0x%08x | Host ctl: 0x%08x\n",
		readl(base_addr + SDHCI_PRESENT_STATE),
		readb(base_addr + SDHCI_HOST_CONTROL));
	printf(": Power:    0x%08x | Blk gap:  0x%08x\n",
		readb(base_addr + SDHCI_POWER_CONTROL),
		readb(base_addr + SDHCI_BLOCK_GAP_CONTROL));
	printf(": Wake-up:  0x%08x | Clock:    0x%08x\n",
		readb(base_addr + SDHCI_WAKE_UP_CONTROL),
		readw(base_addr + SDHCI_CLOCK_CONTROL));
	printf(": Timeout:  0x%08x | Int stat: 0x%08x\n",
		readb(base_addr + SDHCI_TIMEOUT_CONTROL),
		readl(base_addr + SDHCI_INT_STATUS));
	printf(": Int enab: 0x%08x | Sig enab: 0x%08x\n",
		readl(base_addr + SDHCI_INT_ENABLE),
		readl(base_addr + SDHCI_SIGNAL_ENABLE));
	printf(": AC12 err: 0x%08x | Slot int: 0x%08x\n",
		readw(base_addr + SDHCI_ACMD12_ERR),
		readw(base_addr + SDHCI_SLOT_INT_STATUS));
	printf(": Caps:     0x%08x | Max curr: 0x%08x\n",
		readl(base_addr + SDHCI_CAPABILITIES),
		readl(base_addr + SDHCI_MAX_CURRENT));

	printf(": ADMA Err: 0x%08x | ADMA Ptr: 0x%08x\n",
	       readl(base_addr + SDHCI_ADMA_ERROR),
	       readl(base_addr + SDHCI_ADMA_ADDRESS));

	printf(": ===========================================\n");
}

static void sdhci_clear_set_irqs(struct mmc *mmc, u32 clear, u32 set)
{
	struct spear_mmc *host = (struct spear_mmc *)mmc->priv;
	const ulong base_addr = (ulong)host->ioaddr;
	u32 ier;

	ier = readl(base_addr + SDHCI_INT_ENABLE);
	ier &= ~clear;
	ier |= set;
	writel(ier, base_addr + SDHCI_INT_ENABLE);
	writel(ier, base_addr + SDHCI_SIGNAL_ENABLE);
}

static void sdhci_unmask_irqs(struct mmc *mmc, u32 irqs)
{
	sdhci_clear_set_irqs(mmc, 0, irqs);
}

static void sdhci_mask_irqs(struct mmc *mmc, u32 irqs)
{
	sdhci_clear_set_irqs(mmc, irqs, 0);
}

static void sdhci_set_card_detection(struct mmc *mmc, u8 enable)
{
	u32 irqs = SDHCI_INT_CARD_REMOVE | SDHCI_INT_CARD_INSERT;

	if (enable)
		sdhci_unmask_irqs(mmc, irqs);
	else
		sdhci_mask_irqs(mmc, irqs);
}

void sdhci_reset(struct mmc *mmc, u8 mask)
{
	struct spear_mmc *host = (struct spear_mmc *)mmc->priv;
	const ulong base_addr = (ulong)host->ioaddr;
	unsigned long timeout;

	if (!(readl(base_addr + SDHCI_PRESENT_STATE) & SDHCI_CARD_PRESENT))
		return;

	writeb(mask, base_addr + SDHCI_SOFTWARE_RESET);

	/* Wait max 100 ms */
	timeout = 100;

	/* hw clears the bit when it's done */
	while (readb(base_addr + SDHCI_SOFTWARE_RESET) & mask) {
		if (timeout == 0) {
			printf("Reset 0x%x never completed.\n", (int)mask);
			sdhci_dumpregs(mmc);
			return;
		}
		timeout--;
		udelay(1000);
	}
}

static void sdhci_set_power(struct mmc *mmc, unsigned short power)
{
	struct spear_mmc *host = (struct spear_mmc *)mmc->priv;
	ulong base_addr = (ulong)host->ioaddr;
	u8 pwr = 0;

	if (power == (unsigned short)-1)
		pwr = 0;
	else {
		switch (1 << power) {
		case MMC_VDD_165_195:
			pwr = SDHCI_POWER_180;
			break;
		case MMC_VDD_29_30:
		case MMC_VDD_30_31:
			pwr = SDHCI_POWER_300;
			break;
		case MMC_VDD_32_33:
		case MMC_VDD_33_34:
			pwr = SDHCI_POWER_330;
			break;
		default:
			BUG();
		}
	}

	if (host->pwr == pwr)
		return;

	host->pwr = pwr;

	/*
	 * Spec says that we should clear the power reg before setting
	 * a new value. Some controllers don't seem to like this though.
	 */
	writeb(0, base_addr + SDHCI_POWER_CONTROL);
	if (pwr == 0)
		return;

	writeb(pwr, base_addr + SDHCI_POWER_CONTROL);
	pwr |= SDHCI_POWER_ON;
	writeb(pwr, base_addr + SDHCI_POWER_CONTROL);
}

static int sdhci_init(struct mmc *mmc)
{
	sdhci_reset(mmc, SDHCI_RESET_ALL);

	sdhci_clear_set_irqs(mmc, SDHCI_INT_ALL_MASK,
		SDHCI_INT_BUS_POWER | SDHCI_INT_DATA_MASK |
		SDHCI_INT_CMD_MASK);

	sdhci_set_power(mmc, fls(mmc->voltages) - 1);
	return 0;
}

static void sdhci_set_clock(struct mmc *mmc, unsigned int clock)
{
	struct spear_mmc *host = (struct spear_mmc *)mmc->priv;
	const ulong base_addr = (ulong)host->ioaddr;
	int div;
	unsigned long timeout;
	u16 clk;

	if (clock == host->clock)
		return;

	writew(0, base_addr + SDHCI_CLOCK_CONTROL);

	if (clock == 0)
		goto out;

	for (div = 1; div < 256; div *= 2) {
		if ((host->max_clk / div) <= clock)
			break;
	}
	div >>= 1;

	clk = div << SDHCI_DIVIDER_SHIFT;
	clk |= SDHCI_CLOCK_INT_EN;
	writew(clk, base_addr + SDHCI_CLOCK_CONTROL);

	/* Wait max 20 ms */
	timeout = 20;
	while (!((clk = readw(base_addr + SDHCI_CLOCK_CONTROL))
		& SDHCI_CLOCK_INT_STABLE)) {
		if (timeout == 0) {
			printf("Internal clock never stabilised.\n");
			sdhci_dumpregs(mmc);
			return;
		}
		timeout--;
		udelay(1000);
	}

	clk |= SDHCI_CLOCK_CARD_EN;
	writew(clk, base_addr + SDHCI_CLOCK_CONTROL);

out:
	host->clock = clock;
}

static void sdhci_set_ios(struct mmc *mmc)
{
	struct spear_mmc *host = (struct spear_mmc *)mmc->priv;
	const ulong base_addr = (ulong)host->ioaddr;
	u32 power;
	u8 ctrl;

	sdhci_set_clock(mmc, mmc->clock);

	power = mmc->ocr & mmc->voltages;
	sdhci_set_power(mmc, ffs(power) - 1);

	ctrl = readb(base_addr + SDHCI_HOST_CONTROL);
	ctrl &= ~SDHCI_CTRL_HISPD;

	if (mmc->bus_width == 4)
		ctrl |= SDHCI_CTRL_4BITBUS;
	else
		ctrl &= ~SDHCI_CTRL_4BITBUS;
	writeb(ctrl, base_addr + SDHCI_HOST_CONTROL);
}

#if defined(CONFIG_SPEAR1300) || \
	defined(CONFIG_SPEAR1310_REVA) || \
	defined(CONFIG_SPEAR900)
static void swap_2_3(char *buffer, size_t size)
{
	u8 swap, swap_tmp;

	while (size) {
		swap = *buffer;

		swap_tmp = swap;
		swap &= ~0x0C;
		swap &= ~0xC0;

		if (swap_tmp & (1 << 2))
			swap |= 1 << 3;
		if (swap_tmp & (1 << 3))
			swap |= 1 << 2;
		if (swap_tmp & (1 << 6))
			swap |= 1 << 7;
		if (swap_tmp & (1 << 7))
			swap |= 1 << 6;

		*buffer = swap;
		buffer++;
		size--;
	}
}
#endif

static void sdhci_read_block_pio(struct mmc *mmc, struct mmc_data *data,
		char *buffer)
{
	struct spear_mmc *host = (struct spear_mmc *)mmc->priv;
	const ulong base_addr = (ulong)host->ioaddr;
	size_t blksize, chunk = 0;
	u32 uninitialized_var(scratch);
	char *buff = buffer;

	blksize = data->blocksize;

	while (blksize) {
		if (chunk == 0) {
			scratch = readl(base_addr + SDHCI_BUFFER);
			chunk = 4;
		}

		*buff = scratch & 0xFF;

		buff++;
		scratch >>= 8;
		chunk--;
		blksize--;
	}

#if defined(CONFIG_SPEAR1300) || \
	defined(CONFIG_SPEAR1310_REVA) || \
	defined(CONFIG_SPEAR900)
	if (mmc->bus_width == 4)
		swap_2_3(buffer, data->blocksize);
#endif
}

static void sdhci_write_block_pio(struct mmc *mmc, struct mmc_data *data,
		char *buffer)
{
	struct spear_mmc *host = (struct spear_mmc *)mmc->priv;
	const ulong base_addr = (ulong)host->ioaddr;
	size_t blksize, chunk = 0;
	u32 scratch = 0;

#if defined(CONFIG_SPEAR1300) || \
	defined(CONFIG_SPEAR1310_REVA) || \
	defined(CONFIG_SPEAR900)
	if (mmc->bus_width == 4)
		swap_2_3(buffer, data->blocksize);
#endif

	blksize = data->blocksize;

	while (blksize) {
		scratch |= (u32)*buffer << (chunk * 8);

		buffer++;
		chunk++;
		blksize--;

		if ((chunk == 4) || (blksize == 0)) {
			writel(scratch, base_addr + SDHCI_BUFFER);
			chunk = 0;
			scratch = 0;
		}
	}
}

static int sdhci_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			struct mmc_data *data)
{
	struct spear_mmc *host = (struct spear_mmc *)mmc->priv;
	const ulong base_addr = (ulong)host->ioaddr;

	int flags;
	u16 mode;
	u32 i, mask, intstatus;
	unsigned long timeout;

	/* Wait max 10 ms */
	timeout = 10;

	mask = SDHCI_CMD_INHIBIT;
	if ((data != NULL) || (cmd->resp_type & MMC_RSP_BUSY))
		mask |= SDHCI_DATA_INHIBIT;

	while (readl(base_addr + SDHCI_PRESENT_STATE) & mask) {
		if (timeout == 0) {
			printf("Controller never released inhibit bit(s).\n");
			sdhci_dumpregs(mmc);
			return TIMEOUT;
		}
		timeout--;
		udelay(1000);
	}

	writel(cmd->cmdarg, base_addr + SDHCI_ARGUMENT);

	if (data) {
		/* Relaxed timings */
		writeb(0xE, base_addr + SDHCI_TIMEOUT_CONTROL); /* TODO */

		writew(data->blocksize, base_addr + SDHCI_BLOCK_SIZE);
		writew(data->blocks, base_addr + SDHCI_BLOCK_COUNT);

		if (data->blocks > 1)
			mode = SDHCI_TRNS_MULTI | SDHCI_TRNS_BLK_CNT_EN;
		if (data->flags & MMC_DATA_READ)
			mode = SDHCI_TRNS_READ;

		writew(mode, base_addr + SDHCI_TRANSFER_MODE);
	}

	if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY)) {
		printf("Unsupported response type!\n");
		return TIMEOUT; /* TODO */
	}

	if (!(cmd->resp_type & MMC_RSP_PRESENT))
		flags = SDHCI_CMD_RESP_NONE;
	else if (cmd->resp_type & MMC_RSP_136)
		flags = SDHCI_CMD_RESP_LONG;
	else if (cmd->resp_type & MMC_RSP_BUSY)
		flags = SDHCI_CMD_RESP_SHORT_BUSY;
	else
		flags = SDHCI_CMD_RESP_SHORT;

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= SDHCI_CMD_CRC;
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= SDHCI_CMD_INDEX;
	if (data)
		flags |= SDHCI_CMD_DATA;

	writew(SDHCI_MAKE_CMD(cmd->cmdidx, flags), base_addr + SDHCI_COMMAND);

	/* Wait for command completion */
	intstatus = readl(base_addr + SDHCI_INT_STATUS);
	timeout = 100;

	while (!(intstatus & SDHCI_INT_CMD_MASK)) {
		if (timeout == 0)
			return TIMEOUT;

		timeout--;
		udelay(100);
		intstatus = readl(base_addr + SDHCI_INT_STATUS);
	}

	writel(intstatus & SDHCI_INT_CMD_MASK, base_addr + SDHCI_INT_STATUS);

	if (intstatus & SDHCI_INT_CMD_ERR) {
		sdhci_reset(mmc, SDHCI_RESET_CMD);
		writel(intstatus, base_addr + SDHCI_INT_STATUS);
		return TIMEOUT;
	}

	if (!(intstatus & SDHCI_INT_RESPONSE)) {
		writel(intstatus, base_addr + SDHCI_INT_STATUS);
		return TIMEOUT;
	}

	intstatus &= ~SDHCI_INT_CMD_MASK;

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136) {
			/* CRC is stripped so we need to do some shifting. */
			for (i = 0; i < 4; i++) {
				cmd->response[i] = readl(base_addr +
						SDHCI_RESPONSE + (3-i)*4) << 8;
				if (i != 3)
					cmd->response[i] |= readb(base_addr +
						SDHCI_RESPONSE + (3-i)*4-1);
			}
		} else {
			cmd->response[0] = readl(base_addr + SDHCI_RESPONSE);
		}
	}

	if (data) {
		char *buffer;

		i = 0;
		if (data->flags & MMC_DATA_READ)
			buffer = data->dest;
		else
			buffer = (char *)data->src;

		while (i < data->blocks) {

			intstatus = readl(base_addr + SDHCI_INT_STATUS);

			if (intstatus & SDHCI_INT_DATA_MASK) {
				writel(intstatus & SDHCI_INT_DATA_MASK,
						base_addr + SDHCI_INT_STATUS);

				if (intstatus & SDHCI_INT_DATA_ERR) {
					sdhci_reset(mmc, SDHCI_RESET_DATA);
					writel(intstatus,
						base_addr + SDHCI_INT_STATUS);
					return TIMEOUT;
				}

				intstatus &= ~SDHCI_INT_DATA_MASK;
				if (data->flags & MMC_DATA_READ)
					sdhci_read_block_pio(mmc, data, buffer);
				else
					sdhci_write_block_pio(mmc, data, buffer);

				buffer += data->blocksize;
				i++;
			}
		}
	}

	if (data || (cmd->resp_type & MMC_RSP_BUSY)) {
		timeout = 100;
		intstatus = readl(base_addr + SDHCI_INT_STATUS);

		while (!(intstatus & SDHCI_INT_DATA_END)) {
			if (timeout == 0)
				return TIMEOUT;

			timeout--;
			udelay(100);
			intstatus = readl(base_addr + SDHCI_INT_STATUS);
		}

		writel(intstatus & SDHCI_INT_DATA_END,
				base_addr + SDHCI_INT_STATUS);
		intstatus &= ~SDHCI_INT_DATA_END;
	}

	if (intstatus) {
		printf("Unexpected interrupt!\n");
		writel(intstatus, base_addr + SDHCI_INT_STATUS);
	}

	return 0;
}

int sdhci_initialize(u32 id, ulong base_addr)
{
	struct mmc *mmc;
	struct spear_mmc *host;
	u32 caps;

	mmc = malloc(sizeof(struct mmc));
	if (!mmc)
		return -1;

	host = malloc(sizeof(struct spear_mmc));
	if (!host) {
		free(mmc);
		return -1;
	}

	memset(host, 0, sizeof(*host));
	memset(mmc, 0, sizeof(*mmc));

	sprintf(mmc->name, "mmc");

	mmc->priv = (void *)host;
	host->ioaddr = (void *)base_addr;

	mmc->init = sdhci_init;
	mmc->send_cmd = sdhci_send_cmd;
	mmc->set_ios = sdhci_set_ios;

	caps = readl(base_addr + SDHCI_CAPABILITIES);

	mmc->voltages = 0;
	if (caps & SDHCI_CAN_VDD_330)
		mmc->voltages |= MMC_VDD_32_33 | MMC_VDD_33_34;
	if (caps & SDHCI_CAN_VDD_300)
		mmc->voltages |= MMC_VDD_29_30 | MMC_VDD_30_31;
	if (caps & SDHCI_CAN_VDD_180)
		mmc->voltages |= MMC_VDD_165_195;

	mmc->host_caps = MMC_MODE_4BIT | MMC_MODE_8BIT;
	if (caps & SDHCI_CAN_DO_HISPD)
		mmc->host_caps |= MMC_MODE_HS;

	host->max_clk = (caps & SDHCI_CLOCK_BASE_MASK)
				>> SDHCI_CLOCK_BASE_SHIFT;
	host->max_clk *= 1000000;
	mmc->f_max = 12000000;
	mmc->f_min = 400000;

	/* First reset the SDHCI controller */
	sdhci_reset(mmc, SDHCI_RESET_ALL);
	sdhci_set_card_detection(mmc, 1);

	mmc_register(mmc);

	return 0;
}
