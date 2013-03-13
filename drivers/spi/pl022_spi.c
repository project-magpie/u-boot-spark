/*
 * (C) Copyright 2012
 * Armando Visconti, ST Microelectronics, armando.visconti@st.com.
 *
 * Driver for ARM PL022 SPI Controller. Based on atmel_spi.c
 * by Atmel Corporation.
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
#include <spi.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>

/* SSP registers mapping */
#define SSP_CR0		0x000
#define SSP_CR1		0x004
#define SSP_DR		0x008
#define SSP_SR		0x00C
#define SSP_CPSR	0x010
#define SSP_IMSC	0x014
#define SSP_RIS		0x018
#define SSP_MIS		0x01C
#define SSP_ICR		0x020
#define SSP_DMACR	0x024
#define SSP_ITCR	0x080
#define SSP_ITIP	0x084
#define SSP_ITOP	0x088
#define SSP_TDR		0x08C

#define SSP_PID0	0xFE0
#define SSP_PID1	0xFE4
#define SSP_PID2	0xFE8
#define SSP_PID3	0xFEC

#define SSP_CID0	0xFF0
#define SSP_CID1	0xFF4
#define SSP_CID2	0xFF8
#define SSP_CID3	0xFFC

/* SSP Control Register 0  - SSP_CR0 */
#define SSP_CR0_SPO		(0x1 << 6)
#define SSP_CR0_SPH		(0x1 << 7)
#define SSP_CR0_8BIT_MODE	(0x07)
#define SSP_SCR_MAX		(0xFF)
#define SSP_SCR_SHFT		8

/* SSP Control Register 0  - SSP_CR1 */
#define SSP_CR1_MASK_SSE	(0x1 << 1)

#define SSP_CPSR_MAX		(0xFE)

/* SSP Status Register - SSP_SR */
#define SSP_SR_MASK_TFE		(0x1 << 0) /* Transmit FIFO empty */
#define SSP_SR_MASK_TNF		(0x1 << 1) /* Transmit FIFO not full */
#define SSP_SR_MASK_RNE		(0x1 << 2) /* Receive FIFO not empty */
#define SSP_SR_MASK_RFF		(0x1 << 3) /* Receive FIFO full */
#define SSP_SR_MASK_BSY		(0x1 << 4) /* Busy Flag */

struct pl022_spi_slave {
	struct spi_slave slave;
	void *regs;
	unsigned int freq;
};

static inline struct pl022_spi_slave *to_pl022_spi(struct spi_slave *slave)
{
	return container_of(slave, struct pl022_spi_slave, slave);
}

/*
 * Following three functions should be provided by the
 * board support package.
 */
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
	__attribute__((weak, alias("__spi_cs_is_valid")));
void spi_cs_activate(struct spi_slave *slave)
	__attribute__((weak, alias("__spi_cs_activate")));
void spi_cs_deactivate(struct spi_slave *slave)
	__attribute__((weak, alias("__spi_cs_deactivate")));

int __spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return 1;
}

void __spi_cs_activate(struct spi_slave *slave)
{
	/* do nothing */
}

void __spi_cs_deactivate(struct spi_slave *slave)
{
	/* do nothing */
}

void spi_init()
{
	/* do nothing */
}

/*
 * ARM PL022 exists in different 'flavors'.
 * This drivers currently support the standard variant (0x00041022), that has a
 * 16bit wide and 8 locations deep TX/RX FIFO.
 */
static int pl022_is_supported(struct pl022_spi_slave *ps)
{
	/* PL022 version is 0x00041022 */
	if ((readw(ps->regs + SSP_PID0) == 0x22) &&
		(readw(ps->regs + SSP_PID1) == 0x10) &&
		((readw(ps->regs + SSP_PID2) & 0xf) == 0x04) &&
		(readw(ps->regs + SSP_PID3) == 0x00))
			return 1;

	return 0;
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
			unsigned int max_hz, unsigned int mode)
{
	struct pl022_spi_slave	*ps;
	u16 scr = 1, prescaler, cr0 = 0, cpsr = 0;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	ps = malloc(sizeof(*ps));
	if (!ps)
		return NULL;

	ps->slave.bus = bus;
	ps->slave.cs = cs;
	ps->freq = max_hz;

	switch (bus) {
	case 0:
		ps->regs = (void *)CONFIG_SYS_SPI_BASE;
		break;
#ifdef CONFIG_SYS_SPI_BASE1
	case 1:
		ps->regs = (void *)CONFIG_SYS_SPI_BASE1;
		break;
#endif
#ifdef CONFIG_SYS_SPI_BASE2
	case 2:
		ps->regs = (void *)CONFIG_SYS_SPI_BASE2;
		break;
#endif
#ifdef CONFIG_SYS_SPI_BASE3
	case 3:
		ps->regs = (void *)CONFIG_SYS_SPI_BASE3;
		break;
#endif
	default:
		free(ps);
		return NULL;
	}

	/* Check the PL022 version */
	if (!pl022_is_supported(ps)) {
		free(ps);
		return NULL;
	}

	/* Set requested polarity and 8bit mode */
	cr0 = SSP_CR0_8BIT_MODE;
	cr0 |= (mode & SPI_CPHA) ? SSP_CR0_SPH : 0;
	cr0 |= (mode & SPI_CPOL) ? SSP_CR0_SPO : 0;

	writew(cr0, ps->regs + SSP_CR0);

	/* Program the SSPClk frequency */
	prescaler = CONFIG_SYS_SPI_CLK / ps->freq;

	if (prescaler <= 0xFF)
		cpsr = prescaler;
	else {
		for (scr = 1; scr <= SSP_SCR_MAX; scr++) {
			if (!(prescaler % scr)) {
				cpsr = prescaler / scr;
				if (cpsr <= SSP_CPSR_MAX)
					break;
			}
		}

		if (scr > SSP_SCR_MAX) {
			scr = SSP_SCR_MAX;
			cpsr = prescaler / scr;
			cpsr &= SSP_CPSR_MAX;
		}
	}

	if (cpsr & 0x1)
		cpsr++;

	writew(cpsr, ps->regs + SSP_CPSR);
	cr0 = readw(ps->regs + SSP_CR0);
	writew(cr0 | (scr - 1) << SSP_SCR_SHFT, ps->regs + SSP_CR0);

	return &ps->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);

	free(ps);
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);

	/* Enable the SPI hardware */
	writew(readw(ps->regs + SSP_CR1) | SSP_CR1_MASK_SSE, ps->regs + SSP_CR1);

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);

	/* Disable the SPI hardware */
	writew(0x0, ps->regs + SSP_CR1);
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	struct pl022_spi_slave *ps = to_pl022_spi(slave);
	u32		len_tx = 0, len_rx = 0, len;
	u32		ret = 0;
	const u8	*txp = dout;
	u8		*rxp = din, value;

	if (bitlen == 0)
		/* Finish any previously submitted transfers */
		goto out;

	/*
	 * TODO: The controller can do non-multiple-of-8 bit
	 * transfers, but this driver currently doesn't support it.
	 *
	 * It's also not clear how such transfers are supposed to be
	 * represented as a stream of bytes...this is a limitation of
	 * the current SPI interface.
	 */
	if (bitlen % 8) {
		ret = -1;

		/* Errors always terminate an ongoing transfer */
		flags |= SPI_XFER_END;
		goto out;
	}

	len = bitlen / 8;

	if (flags & SPI_XFER_BEGIN)
		spi_cs_activate(slave);

	while (len_tx < len) {
		if (readw(ps->regs + SSP_SR) & SSP_SR_MASK_TNF) {
			value = (txp != NULL) ? *txp++ : 0;
			writew(value, ps->regs + SSP_DR);
			len_tx++;
		}

		if (readw(ps->regs + SSP_SR) & SSP_SR_MASK_RNE) {
			value = readw(ps->regs + SSP_DR);
			if (rxp)
				*rxp++ = value;
			len_rx++;
		}
	}

	while (len_rx < len_tx) {
		if (readw(ps->regs + SSP_SR) & SSP_SR_MASK_RNE) {
			value = readw(ps->regs + SSP_DR);
			if (rxp)
				*rxp++ = value;
			len_rx++;
		}
	}

out:
	if (flags & SPI_XFER_END)
		spi_cs_deactivate(slave);

	return ret;
}
