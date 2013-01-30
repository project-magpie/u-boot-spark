/*
 * (C) Copyright 2007,2009-2013 STMicroelectronics.
 *
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
#include <malloc.h>
#include "stm_spi_fsm.h"


/**********************************************************************/


#if defined(CONFIG_SPI)


/**********************************************************************/


/*
 * Ensure that of the following 3 macros *exactly* one is defined:
 *	CONFIG_SOFT_SPI		- S/W Bit-Banging
 *	CONFIG_STM_SSC_SPI	- H/W using STM's SSC
 *	CONFIG_STM_FSM_SPI	- H/W using STM's FSM SPI Controller
 */
#if !defined(CONFIG_SOFT_SPI) && !defined(CONFIG_STM_SSC_SPI) && !defined(CONFIG_STM_FSM_SPI)
#error One of CONFIG_SOFT_SPI, CONFIG_STM_SSC_SPI or CONFIG_STM_FSM_SPI must be defined!
#endif

#if (	(defined(CONFIG_SOFT_SPI) && defined(CONFIG_STM_SSC_SPI))		||	\
	(defined(CONFIG_SOFT_SPI) && defined(CONFIG_STM_FSM_SPI))		||	\
	(defined(CONFIG_STM_FSM_SPI) && defined(CONFIG_STM_SSC_SPI))	)
#error No more than one of CONFIG_SOFT_SPI, CONFIG_STM_SSC_SPI or CONFIG_STM_FSM_SPI may be defined!
#endif


/**********************************************************************/


/*
 * Define the following macro to define the helper function
 * spi_read_flag_status(), to read the FLAG register.
 * This is usually only for DEBUG purposes.
 */
//#define CONFIG_STM_SPI_READ_FLAG_FUNCTION


/**********************************************************************/


static struct spi_slave * MyDefaultSlave = NULL;	/* QQQ - TO DO SOMETHING HERE ??? */

#define DEFAULT_SPI_XFER_FLAGS	(SPI_XFER_BEGIN | SPI_XFER_END)


/**********************************************************************/


#if defined(CONFIG_STM_SSC_SPI)		/* Use the H/W SSC for SPI */
/* SSC Baud Rate Generator Register */
#define SSC_BRG			0x0000

/* SSC Transmit Buffer Register */
#define SSC_TBUF		0x0004

/* SSC Receive Buffer Register */
#define SSC_RBUF		0x0008

/* SSC Control Register */
#define SSC_CON			0x000C
#define SSC_CON_HB		(1ul<<4)	/* endianness */
#define SSC_CON_PH		(1ul<<5)	/* clock phase */
#define SSC_CON_PO		(1ul<<6)	/* clock polarity */
#define SSC_CON_SR		(1ul<<7)	/* SSC software reset */
#define SSC_CON_MS		(1ul<<8)	/* master/slave select */
#define SSC_CON_EN		(1ul<<9)	/* SSC enable */
#define SSC_CON_LPB		(1ul<<10)	/* SSC loopback mode */
#define SSC_CON_TXFIFO		(1ul<<11)	/* transmit-side FIFO */
#define SSC_CON_RXFIFO		(1ul<<12)	/* receive-side FIFO */
#define SSC_CON_CLSTRX		(1ul<<13)	/* clock stretch mechanism */

/* SSC Interrupt Enable Register */
#define SSC_IEN			0x0010

/* SSC Status Register */
#define SSC_STAT		0x0014
#define SSC_STAT_RIR		(1ul<<0)	/* receive buffer full flag */
#define SSC_STAT_TIR		(1ul<<1)	/* transmit buffer empty flag */
#define SSC_STAT_TE		(1ul<<2)	/* transmit Error flag */
#define SSC_STAT_RE		(1ul<<3)	/* receive Error flag */
#define SSC_STAT_PE		(1ul<<4)	/* phase Error flag */

/* SSC I2C Control Register */
#define SSC_I2C			0x0018

/* SPI phase/polarity modes */
#define SPI_CPHA		0x01		/* clock phase */
#define SPI_CPOL		0x02		/* clock polarity */
#define SPI_LSB_FIRST		0x08		/* endianness */
#define SPI_LOOP		0x20		/* loop-back test mode */

#define SPI_MODE_0		(0|0)
#define SPI_MODE_1		(0|SPI_CPHA)
#define SPI_MODE_2		(SPI_CPOL|0)
#define SPI_MODE_3		(SPI_CPOL|SPI_CPHA)

/* SPI Controller's Base Address */
#if !defined(CONFIG_SYS_STM_SPI_SSC_BASE)
#error Please define CONFIG_SYS_STM_SPI_SSC_BASE (e.g. ST40_SSC0_REGS_BASE)
#endif
static const unsigned long ssc = CONFIG_SYS_STM_SPI_SSC_BASE;	/* SSC base */

/* SPI Controller Register's Accessors */
#define ssc_write(offset, value)	writel((value), (ssc)+(offset))
#define ssc_read(offset)		readl((ssc)+(offset))

#endif	/* CONFIG_STM_SSC_SPI */


/**********************************************************************/


static unsigned pageSize;	/* 256, 512 or 528 bytes per page ? */
static unsigned eraseSize;	/* smallest supported erase size */
static unsigned deviceSize;	/* Size of the device in Bytes */
static const char * deviceName;	/* Name of the device */

#if defined(CONFIG_SPI_FLASH_ST) || defined(CONFIG_SPI_FLASH_MXIC) || defined(CONFIG_SPI_FLASH_WINBOND) || defined(CONFIG_SPI_FLASH_SPANSION)
#if defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI) || defined(CONFIG_SPI_FLASH_ST) || defined(CONFIG_SPI_FLASH_SPANSION)
static unsigned char op_write = OP_PP;		/* default write command opcode to use */
static unsigned char op_erase = OP_SE;		/* default erase command opcode to use */
#endif	/* CONFIG_SOFT_SPI || CONFIG_STM_SSC_SPI || CONFIG_SPI_FLASH_ST || CONFIG_SPI_FLASH_SPANSION */
#endif
static unsigned char op_read  = OP_READ_ARRAY;	/* default read command opcode to use */

/*
 * Helper to return the number of address bytes we need to use.
 * i.e. 3 bytes for <= 16MiB, and 4 bytes for > 16MiB.
 */
#define NUM_ADDRESS_BYTES()	( (op_read == OP_READ_ARRAY) ? 3 : 4 )


/**********************************************************************/


/*
 * "DYB Access Register" values for the Spansion S25FLxxxS family.
 */
#define DYB_LOCKED	0x00u
#define DYB_UNLOCK	0xffu


/**********************************************************************/


#if 0
#define isprint(x)    ( ((x)>=0x20u) && ((x)<0x7fu) )
static void hexdump(
	const unsigned char * const data,
	const unsigned int data_size)
{
	const unsigned int wrap = 16;
	const unsigned int ceiling = ((data_size-1) / wrap + 1) * wrap;
	unsigned int i, j;

	if (data_size==0)	/* no data ? */
	{
		return;		/* do nothing */
	}

	for(i=0; i<ceiling; i++)
	{
			/* print the hexadecimal represenation */
		printf(i<data_size ? "%02x " : ".. ",
			data[i]);

			/* now the ASCII representation */
		if (i%wrap==(wrap-1))
		{
			printf("   ");
			for(j=i+1-wrap; (j<=i)&&(j<data_size);j++)
			{
				printf("%c",
					isprint(data[j]) ? data[j] : '.');
			}
			printf("\n");
		}
	}
}
#endif


/**********************************************************************/


#if defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI)

/*-----------------------------------------------------------------------
 * Determine if a SPI chipselect is valid.
 * This function is provided by the board if the low-level SPI driver
 * needs it to determine if a given chipselect is actually valid.
 *
 * Returns: 1 if bus:cs identifies a valid chip on this board, 0
 * otherwise.
 *
 * Note: Initially, we will only accept CS #0, on BUS #0 (i.e. 0:0).
 */
extern int spi_cs_is_valid(
	const unsigned int bus,
	const unsigned int cs)
{
	if ( (bus==0) && (cs==0) )	/* is it 0:0 ? */
	{
		return 1;		/* PASS - bus:cs is "OK" */
	}
	else
	{
		return 0;		/* FAIL - bus:cs is NOT ok */
	}
}

/*-----------------------------------------------------------------------
 * Set up communications parameters for a SPI slave.
 *
 * This must be called once for each slave. Note that this function
 * usually doesn't touch any actual hardware, it only initializes the
 * contents of spi_slave so that the hardware can be easily
 * initialized later.
 *
 *   bus:     Bus ID of the slave chip.
 *   cs:      Chip select ID of the slave chip on the specified bus.
 *   max_hz:  Maximum SCK rate in Hz.
 *   mode:    Clock polarity, clock phase and other parameters.
 *
 * Returns: A spi_slave reference that can be used in subsequent SPI
 * calls, or NULL if one or more of the parameters are not supported.
 */
extern struct spi_slave * spi_setup_slave(
	const unsigned int bus,
	const unsigned int cs,
	const unsigned int max_hz,
	const unsigned int mode)
{
	struct spi_slave * slave;

	if (!spi_cs_is_valid(bus, cs))	/* bad bus/slave specification? */
	{
		return NULL;		/* failed! */
	}

	/* allocate memory for new slave structure */
	slave = malloc(sizeof(struct spi_slave));

	if (!slave)			/* unable to allocate memory? */
	{
		return NULL;		/* failed! */
	}

	/* initialize the newly allocated structure */
	slave->bus = bus;
	slave->cs  = cs;

	return slave;			/* sucess - return it */
}

/*-----------------------------------------------------------------------
 * Free any memory associated with a SPI slave.
 *
 *   slave:	The SPI slave
 */
extern void spi_free_slave(struct spi_slave * const slave)
{
	if (slave == MyDefaultSlave)	/* the "default" one ? */
	{
		MyDefaultSlave = NULL;
	}

	free(slave);			/* safely free it */
}

/*-----------------------------------------------------------------------
 * Claim the bus and prepare it for communication with a given slave.
 *
 * This must be called before doing any transfers with a SPI slave. It
 * will enable and initialize any SPI hardware as necessary, and make
 * sure that the SCK line is in the correct idle state. It is not
 * allowed to claim the same bus for several slaves without releasing
 * the bus in between.
 *
 *   slave:	The SPI slave
 *
 * Returns: 0 if the bus was claimed successfully, or a negative value
 * if it wasn't.
 */
extern int spi_claim_bus(struct spi_slave * const slave)
{
	return 0;		/* do nothing! */
}

/*-----------------------------------------------------------------------
 * Release the SPI bus
 *
 * This must be called once for every call to spi_claim_bus() after
 * all transfers have finished. It may disable any SPI hardware as
 * appropriate.
 *
 *   slave:	The SPI slave
 */
extern void spi_release_bus(struct spi_slave * const slave)
{
	return;			/* do nothing! */
}

#endif /* defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI) */


/**********************************************************************/


/*
 * Transfer (i.e. exchange) one "word" with selected SPI device.
 * Typically one word is 8-bits (an octet), but it does not need to be,
 * this function is word-width agnostic - when using the SSC.
 * However, for PIO bit-banging, then one word is explicitly
 * unconditionally assumed to be exactly 8-bits in length.
 *
 *	input:   "out" is word to be send to slave SPI device
 *	returns: one word read from the slave SPI device
 *
 * It is the caller's responsibility to ensure that the
 * chip select (SPI_NOTCS) is correctly asserted.
 */
#if defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI)
static unsigned int spi_xfer_one_word(const unsigned int out)
{
	unsigned int in = 0;

#if defined(CONFIG_SOFT_SPI)		/* Use PIO Bit-Banging for SPI */
	signed int i;
	for(i=7; i>=0; i--)		/* for each bit in turn ... */
	{				/* do 8 bits, msb first */
		SPI_SCL(0);		/* SPI_CLK = low */
		SPI_SDA(out & (1u<<i));	/* output next bit on SPI_DOUT */
		SPI_DELAY;		/* clock low cycle */

		SPI_SCL(1);		/* SPI_CLK = high */
		SPI_DELAY;		/* sample on RISING clock edge */

		in <<= 1;		/* shift */
		in |= SPI_READ;		/* get next bit from SPI_DIN */
	}
#elif defined(CONFIG_STM_SSC_SPI)	/* Use the H/W SSC for SPI */
	/* write out data 'out' */
	ssc_write(SSC_TBUF, out);

	/* wait for Receive Buffer Full flag to be asserted */
	while ((ssc_read(SSC_STAT) & SSC_STAT_RIR) == 0)
	{
		;	/* busy poll - do nothing */
	}

	/* read in data */
	in = ssc_read(SSC_RBUF);
#endif	/* CONFIG_SOFT_SPI */

	/* return exchanged data */
	return in;
}
#endif	/* defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI) */


/**********************************************************************/


/*
 * transfer (i.e. exchange) a series of "words" with SPI device.
 * Typically one word is 8-bits (an octet).
 * This function is expects a word to be exactly 8-bits.
 *
 *	input:   slave, the SPI slave which will be sending/receiving the data.
 *		 bitlen number of *bits* (not bytes) to be exchanged
 *		 dout pointer to array of words to be sent to SPI
 *		 din pointer to array of words that were read from SPI
 *		 flags, additional transfer flags
 *	returns: zero on success, else non-zero.
 *
 *	Note: 'din' may be NULL if caller does not need to see it.
 */
#if defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI)
extern int spi_xfer(
	struct spi_slave * const slave,
	const unsigned int bitlen,
	const void * const _dout,
	      void * const _din,
	const unsigned long flags)
{
	size_t i;
	const int bytelen = bitlen / 8;	/* number of 8-bit bytes */
	const uchar * const dout = _dout;
	      uchar * const din  = _din;

	/*
	 * This code assumes that as we are using the SSC, that we will always
	 * be swapping multiples of *whole* 8-bit bytes between the master
	 * and the slave device.
	 * This assumption is strictly unnecessary, as we could re-program
	 * the SSC to allow any bit-size we want. However, this initial version
	 * of the code sets the pervading transfers to 8-bits in the SSC.
	 * So, right now, we just give up if the number of bits is not a
	 * whole number of (8-bit) bytes - after printing a suitable diagnostic.
	 */
	if (bitlen & 0x7)
	{
		printf("ERROR: %s() called with non-multiple of octets (%u bits)\n",
			__FUNCTION__,
			bitlen);
		return 1;	/* error status */
	}

#if 0	/* QQQ - DELETE */
	hexdump(dout, bytelen);
#endif	/* QQQ - DELETE */

	if (flags & SPI_XFER_BEGIN)
	{	/* assert SPI CS */
		spi_cs_activate(slave);
	}

	/* transfer: write bytes in 'dout', and read into 'din' */
	for(i=0; i<bytelen; i++)
	{
		uchar data;
		data = spi_xfer_one_word(dout[i]);
		if (din != NULL) din[i] = data;
	}

	if (flags & SPI_XFER_END)
	{	/* de-assert SPI CS */
		spi_cs_deactivate(slave);
	}

#if 0	/* QQQ - DELETE */
	if (din != NULL)
	{
		hexdump(din, bytelen);
		printf("\n");
	}
#endif	/* QQQ - DELETE */

	return 0;	/* success */
}
#endif	/* defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI) */


/**********************************************************************/


/*
 * read the SPI slave device's "Status Register".
 *
 * input:   slave, the SPI slave which will be sending/receiving the data.
 * returns: the value of the status register.
 */
static unsigned int spi_read_status(
	struct spi_slave * const slave)
{
#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
	/* return the status byte read */
	return fsm_read_status();
#else	/* CONFIG_STM_FSM_SPI */
	unsigned char data[2] = { OP_READ_STATUS, 0x00 };

	/* issue the Status Register Read command */
	spi_xfer(slave, sizeof(data)*8, data, data, DEFAULT_SPI_XFER_FLAGS);

	/* return the status byte read */
	return data[1];
#endif	/* CONFIG_STM_FSM_SPI */
}


/**********************************************************************/


#if defined(CONFIG_SPI_FLASH_ST) && defined(CONFIG_STM_SPI_READ_FLAG_FUNCTION)
/*
 * read the SPI slave device's "Flag Status Register".
 *
 * input:   slave, the SPI slave which will be sending/receiving the data.
 * returns: the value of the flag status register.
 */
static unsigned int spi_read_flag_status(
	struct spi_slave * const slave)
{
#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
	/* return the flag status byte read */
	return fsm_read_flag_status();
#else	/* CONFIG_STM_FSM_SPI */
	unsigned char data[2] = { OP_READ_FLAG_STATUS, 0x00 };

	/* issue the Flag Status Register Read command */
	spi_xfer(slave, sizeof(data)*8, data, data, DEFAULT_SPI_XFER_FLAGS);

	/* return the flag status byte read */
	return data[1];
#endif	/* CONFIG_STM_FSM_SPI */
}
#endif	/* CONFIG_SPI_FLASH_ST && CONFIG_STM_SPI_READ_FLAG_FUNCTION */


/**********************************************************************/


/*
 * poll the "Status Register" waiting till it is not busy.
 *
 * input:   slave, the SPI slave which will be sending/receiving the data.
 * returns: none
 */
extern void spi_wait_till_ready(
	struct spi_slave * const slave)
{
#if defined(CONFIG_SPI_FLASH_ATMEL)
	while (!(spi_read_status(slave) & SR_READY))
		;	/* do nothing */
#elif defined(CONFIG_SPI_FLASH_ST) || defined(CONFIG_SPI_FLASH_MXIC) || defined(CONFIG_SPI_FLASH_WINBOND) || defined(CONFIG_SPI_FLASH_SPANSION)
	while (spi_read_status(slave) & SR_WIP)
		;	/* do nothing */
#else
#error Please specify which SPI Serial Flash is being used
#endif	/* defined(CONFIG_STM_SPI_xxxxxx) */
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
static void spi_enter_4byte_mode(
	struct spi_slave * const slave,
	const int enter)
{
#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
	/* issue "WREN" +  "Enter/Exit 4-Byte Address Mode" commands */
	fsm_enter_4byte_mode(enter);
#else	/* CONFIG_STM_FSM_SPI */
	const unsigned char enable[1] = { OP_WREN };
	const unsigned char cmd[1] = { (enter) ? OP_ENTER_4BYTE : OP_EXIT_4BYTE };

	/* issue a WRITE ENABLE (WREN) command */
	spi_xfer(slave, sizeof(enable)*8, enable, NULL, DEFAULT_SPI_XFER_FLAGS);

	/* issue an "Enter/Exit 4-Byte Address Mode" command */
	spi_xfer(slave, sizeof(cmd)*8, cmd, NULL, DEFAULT_SPI_XFER_FLAGS);
#endif	/* CONFIG_STM_FSM_SPI */
}
#endif	/* CONFIG_SPI_FLASH_ST */


/**********************************************************************/


#if defined(CONFIG_SPI_FLASH_SPANSION)
/*
 * read the SPI slave device's "DYB Access Register".
 *
 *	input:   slave, the SPI slave which will be sending/receiving the data.
 *		 sector, the sector whose DYB access register is to be accessed.
 *	returns: the value of the DYB access register.
 */
static unsigned int spi_read_dyb_access(
	struct spi_slave * const slave,
	const unsigned long sector)
{
#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
	/* return the DYB Access Register byte read */
	return fsm_read_dyb_access(sector);
#else	/* CONFIG_STM_FSM_SPI */
	unsigned char data[6] = {
		OP_DYB_READ,
		(sector>>24) & 0xffu,
		(sector>>16) & 0xffu,
		(sector>>8)  & 0xffu,
		(sector>>0)  & 0xffu,
	};

	/* issue the Read DYB Access Register command */
	spi_xfer(slave, sizeof(data)*8, data, data, DEFAULT_SPI_XFER_FLAGS);

	/* return the DYB Access Register byte read */
	return data[5];
#endif	/* CONFIG_STM_FSM_SPI */
}
#endif	/* CONFIG_SPI_FLASH_SPANSION */


/**********************************************************************/


#if defined(CONFIG_SPI_FLASH_SPANSION)
/*
 * write to the SPI slave device's "DYB Access Register".
 *
 *	input:   slave, the SPI slave which will be sending/receiving the data.
 *		 sector, the sector whose DYB access register is to be accessed.
 *		 dyb, the value that will be written to the DYB access register.
 *	returns: none.
 */
static void spi_write_dyb_access(
	struct spi_slave * const slave,
	const unsigned long sector,
	const unsigned char dyb)
{
#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
	/* issue "WREN" +  "Write DYB" commands */
	fsm_write_dyb_access(sector, dyb);
#else	/* CONFIG_STM_FSM_SPI */
	const unsigned char enable[1] = { OP_WREN };
	const unsigned char data[6] = {
		OP_DYB_WRITE,
		(sector>>24) & 0xffu,
		(sector>>16) & 0xffu,
		(sector>>8)  & 0xffu,
		(sector>>0)  & 0xffu,
		dyb,
	};

	/* issue a WRITE ENABLE (WREN) command */
	spi_xfer(slave, sizeof(enable)*8, enable, NULL, DEFAULT_SPI_XFER_FLAGS);

	/* issue the Write DYB Access Register command */
	spi_xfer(slave, sizeof(data)*8, data, NULL, DEFAULT_SPI_XFER_FLAGS);

	/* now wait until the programming has completed ... */
	spi_wait_till_ready(slave);
#endif	/* CONFIG_STM_FSM_SPI */
}
#endif	/* CONFIG_SPI_FLASH_SPANSION */


/**********************************************************************/


#if defined(CONFIG_SPI_FLASH_SPANSION)
static void spi_dby_unlock_all(
	struct spi_slave * const slave)
{
	const unsigned long sizeOfNonUniform = 4 << 10;	/* 4KiB sectors, and ... */
	const unsigned long numNonUniform = 32;		/* ... there are 32 of them. */
	const unsigned long sizeOfUniform = 64 << 10;	/* 64KiB sectors */
	const unsigned long startOfTop = deviceSize - sizeOfNonUniform * numNonUniform;
	unsigned long address;
	unsigned int dyb;
	unsigned int unlocked = 0;		/* total number of sectors unlocked */

	/*
	 * For the Spansion S25FLxxxS family of parts, there are three
	 * main sector topologies: "UNIFORM", "BOTTOM", or "TOP".
	 *
	 *	UNIFORM: all sectors are of the same size, being 256KiB.
	 *
	 *	BOTTOM:	the first 32 sectors are all 4KiB, and the
	 *		remainder are all 64KiB.
	 *
	 *	TOP:	the last 32 sectors are all 4KiB, and the
	 *		remainder are all 64KiB.
	 *
	 * In order to lock/unlock all the sectors, then it is essential
	 * that each sector is individually addresses. However, we play
	 * it safe here, and assume the device is a "union" of "TOP" and
	 * "BOTTOM", by unlocking every block assuming a 64KiB stride,
	 * and also unlocking the top 32 and bottom 32 with a 4KiB stride.
	 * This results in doing more that than strictly necessary, but
	 * it means we do not need to know the exact topology, but we
	 * can be confident of unlocking *all* of the sectors.
	 *
	 * Note: to be more efficient, we only try and unlock sectors
	 * that we know to be locked, by reading the DYB access register
	 * for each sector first. Reading the DYB should be much faster
	 * (only 48 SPI clock cycles) than writing the DYB access
	 * registers (WIP delay). Hence, the intrinsic inefficiencies
	 * are expected to be small enough, not to be significant.
	 */

	/* BOTTOM: first 32 * 4KiB sectors */
	for (	address = 0x0;
		address < sizeOfNonUniform * numNonUniform;
		address += sizeOfNonUniform)
	{
		dyb = spi_read_dyb_access(slave, address);
		if (dyb == DYB_LOCKED)
		{
			// printf("info: UN-LOCKING address 0x%08lx\n", address);
			spi_write_dyb_access(slave, address, DYB_UNLOCK);
			unlocked++;
		}
	}

	/* UNIFORM: (middle) assume a uniform 64KiB sector size */
	for (	address = sizeOfNonUniform * numNonUniform;
		address < startOfTop;
		address += sizeOfUniform)
	{
		dyb = spi_read_dyb_access(slave, address);
		if (dyb == DYB_LOCKED)
		{
			// printf("info: UN-LOCKING address 0x%08lx\n", address);
			spi_write_dyb_access(slave, address, DYB_UNLOCK);
			unlocked++;
		}
	}

	/* TOP: last 32 * 4KiB sectors */
	for (	address = startOfTop;
		address < deviceSize;
		address += sizeOfNonUniform)
	{
		dyb = spi_read_dyb_access(slave, address);
		if (dyb == DYB_LOCKED)
		{
			// printf("info: UN-LOCKING address 0x%08lx\n", address);
			spi_write_dyb_access(slave, address, DYB_UNLOCK);
			unlocked++;
		}
	}

	/*
	 * if we unlocked any, then tell the user how many we unlocked.
	 */
	if (unlocked)
	{
		printf("info: Unlocked %u sectors with DYB on %s\n",
			unlocked, deviceName);
	}
}
#endif	/* CONFIG_SPI_FLASH_SPANSION */


/**********************************************************************/


/*
 * probe the serial flash on the SPI bus, to ensure
 * it is a known type, and initialize its properties.
 */
static int spi_probe_serial_flash(
	struct spi_slave * const slave)
{
	unsigned int status;
	unsigned char devid[8] = {
		OP_READ_DEVID,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

	/* read & detect the SPI device type */
	status = spi_read_status(slave);
	if (
		(status == 0xffu)	/* nothing talking to us ? */	||
		( (status & CONFIG_SYS_STM_SPI_DEVICE_MASK) != CONFIG_SYS_STM_SPI_DEVICE_VAL )
	   )
	{
		printf("ERROR: Unknown SPI Device detected, status = 0x%02x\n",
			status);
		return -1;
	}

	/*
	 * if we get here, then we think we may have a SPI
	 * device, so now check it is the correct one!
	 */
#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
	/*
	 * Note: devid[0] is assumed to the be OP_READ_DEVID.
	 * Hence, devid[1] needs to be the FIRST JEDEC byte retrieved.
	 */
	fsm_read_jedec(sizeof(devid)-1u, &devid[1]);
#else	/* CONFIG_STM_FSM_SPI */
	spi_xfer(slave, sizeof(devid)*8, devid, devid, DEFAULT_SPI_XFER_FLAGS);
#endif	/* CONFIG_STM_FSM_SPI */

#if defined(CONFIG_SPI_FLASH_ATMEL)

	/* extract the page size */
	if (
		(devid[1] != 0x1fu)	||	/* Manufacturer ID */
		(devid[2] != 0x27u)	||	/* Memory Type */
		(devid[3] != 0x01u)		/* Code + Version */
	   )
	{
		printf("ERROR: Unknown SPI Device detected, devid = 0x%02x, 0x%02x, 0x%02x\n",
			devid[1], devid[2], devid[3]);
		return -1;
	}
	pageSize = (status & 1u) ? 512u : 528u;
	eraseSize = pageSize;
	deviceSize = 8192u * pageSize;		/* 32 Mbit == 4 MiB */
	deviceName = "Atmel AT45DB321D";

#elif defined(CONFIG_SPI_FLASH_ST)

	if (
		(devid[1] == 0x20u)	&&	/* Manufacturer ID */
		(devid[2] == 0x20u)	&&	/* Memory Type */
		(				/* Memory Capacity */
			(devid[3] == 0x14u) ||	/* M25P80 */
			(devid[3] == 0x15u) ||	/* M25P16 */
			(devid[3] == 0x16u) ||	/* M25P32 */
			(devid[3] == 0x17u) ||	/* M25P64 */
			(devid[3] == 0x18u)	/* M25P128 */
		)
	   )
	{
		pageSize   = 256u;
		eraseSize  = 64u<<10;			/* 64 KiB, 256 pages/sector */
		deviceSize = 1u<<devid[3];		/* Memory Capacity */
		if (devid[3] == 0x14u)
		{
			deviceName = "ST M25P80";	/* 8 Mbit == 1 MiB */
		}
		else if (devid[3] == 0x15u)
		{
			deviceName = "ST M25P16";	/* 16 Mbit == 2 MiB */
		}
		else if (devid[3] == 0x16u)
		{
			deviceName = "ST M25P32";	/* 32 Mbit == 4 MiB */
		}
		else if (devid[3] == 0x17u)
		{
			deviceName = "ST M25P64";	/* 64 Mbit == 8 MiB */
		}
		else if (devid[3] == 0x18u)
		{
			deviceName = "ST M25P128";	/* 128 Mbit == 16 MiB */
			eraseSize = 256u<<10;		/* 256 KiB, 1024 pages/sector */
		}
	}
	else if (
		(devid[1] == 0x20u)	&&	/* Manufacturer ID */
		(devid[2] == 0x71u)	&&	/* Memory Type */
		(				/* Memory Capacity */
			(devid[3] == 0x14u) ||	/* M25PX80 */
			(devid[3] == 0x15u) ||	/* M25PX16 */
			(devid[3] == 0x16u) ||	/* M25PX32 */
			(devid[3] == 0x17u)	/* M25PX64 */
		)
	   )
	{
		pageSize   = 256u;
		eraseSize  = 4u<<10;			/* 4 KiB, 16 pages/sub-sector */
		op_erase   = OP_SSE;			/* use SSE (4KiB) for erase */
		deviceSize = 1u<<devid[3];		/* Memory Capacity */
		if (devid[3] == 0x14u)
		{
			deviceName = "ST M25PX80";	/* 8 Mbit == 1 MiB */
		}
		else if (devid[3] == 0x15u)
		{
			deviceName = "ST M25PX16";	/* 16 Mbit == 2 MiB */
		}
		else if (devid[3] == 0x16u)
		{
			deviceName = "ST M25PX32";	/* 32 Mbit == 4 MiB */
		}
		else if (devid[3] == 0x17u)
		{
			deviceName = "ST M25PX64";	/* 64 Mbit == 8 MiB */
		}
	}
	else if (
		(devid[1] == 0x20u)	&&	/* Manufacturer ID */
		(devid[2] == 0xBAu)	&&	/* Memory Type */
		(				/* Memory Capacity */
			(devid[3] == 0x16u) ||	/* N25Q032 */
			(devid[3] == 0x17u) ||	/* N25Q064 */
			(devid[3] == 0x18u) ||	/* N25Q128 */
			(devid[3] == 0x19u)	/* N25Q256 */
		)
	   )
	{
		pageSize   = 256u;
		eraseSize  = 64u<<10;			/* 64 KiB, 256 pages/sector */
		deviceSize = 1u<<devid[3];		/* Memory Capacity */
		if (devid[3] == 0x16u)
		{
			deviceName = "ST N25Q032";	/* 32 Mbit == 4 MiB */
		}
		else if (devid[3] == 0x17u)
		{
			deviceName = "ST N25Q064";	/* 64 Mbit == 8 MiB */
		}
		else if (devid[3] == 0x18u)
		{
			deviceName = "ST N25Q128";	/* 128 Mbit == 16 MiB */
		}
		else if (devid[3] == 0x19u)
		{
			deviceName = "ST N25Q256";	/* 256 Mbit == 32 MiB */
			op_read = OP_READ_4BYTE;	/* use 4-byte addressing for READ */
		}
	}
	else
	{
		printf("ERROR: Unknown SPI Device detected, devid = 0x%02x, 0x%02x, 0x%02x\n",
			devid[1], devid[2], devid[3]);
		return -1;
	}

#elif defined(CONFIG_SPI_FLASH_MXIC)

	if (
		(devid[1] == 0xc2u)	&&	/* Manufacturer ID */
		(devid[2] == 0x26u)	&&	/* Memory Type */
		(				/* Memory Capacity */
			(devid[3] == 0x15u) ||	/* MX25L1655D */
			(devid[3] == 0x17u) ||	/* MX25L6455E */
			(devid[3] == 0x18u)	/* MX25L12855E */
		)
	   )
	{
		pageSize   = 256u;
		eraseSize  = 4u<<10;			/* 4 KiB, 16 pages/sector */
		deviceSize = 1u<<devid[3];		/* Memory Capacity */
		if (devid[3] == 0x15u)
		{
			deviceName = "MXIC MX25L1655D";	/* 16 Mbit == 2 MiB */
		}
		else if (devid[3] == 0x17u)
		{
			deviceName = "MXIC MX25L6455E";	/* 64 Mbit == 8 MiB */
		}
		else if (devid[3] == 0x18u)
		{
			deviceName = "MXIC MX25L12855E";/* 128 Mbit == 16 MiB */
		}
	}
	else if (
		(devid[1] == 0xc2u)	&&	/* Manufacturer ID */
		(devid[2] == 0x20u)	&&	/* Memory Type */
		(devid[3] == 0x16u)		/* MX25L3206E */
	   )
	{
		pageSize   = 256u;
		eraseSize  = 4u<<10;			/* 4 KiB, 16 pages/sector */
		deviceSize = 1u<<devid[3];		/* Memory Capacity */
		deviceName = "MXIC MX25L3206E";		/* 32 Mbit == 4 MiB */
	}
	else
	{
		printf("ERROR: Unknown SPI Device detected, devid = 0x%02x, 0x%02x, 0x%02x\n",
			devid[1], devid[2], devid[3]);
		return -1;
	}

#elif defined(CONFIG_SPI_FLASH_WINBOND)

	if (
		(devid[1] != 0xefu)	||	/* Manufacturer ID */
		(devid[2] != 0x40u)	||	/* Memory Type */
		(				/* Memory Capacity */
			(devid[3] != 0x14u) &&	/* W25Q80V */
			(devid[3] != 0x15u) &&	/* W25Q16V */
			(devid[3] != 0x16u) &&	/* W25Q32V */
			(devid[3] != 0x17u) &&	/* W25Q64V */
			(devid[3] != 0x18u)	/* W25Q128V */
		)
	   )
	{
		printf("ERROR: Unknown SPI Device detected, devid = 0x%02x, 0x%02x, 0x%02x\n",
			devid[1], devid[2], devid[3]);
		return -1;
	}
	pageSize   = 256u;
	eraseSize  = 4u<<10;			/* 4 KiB, 16 pages/sector */
	deviceSize = 1u<<devid[3];		/* Memory Capacity */
	if (devid[3] == 0x14u)
	{
		deviceName = "Winbond W25Q80V";	/*  8 Mbit == 1 MiB */
	}
	else if (devid[3] == 0x15u)
	{
		deviceName = "Winbond W25Q16V";	/* 16 Mbit == 2 MiB */
	}
	else if (devid[3] == 0x16u)
	{
		deviceName = "Winbond W25Q32V";	/* 32 Mbit == 4 MiB */
	}
	else if (devid[3] == 0x17u)
	{
		deviceName = "Winbond W25Q64V";	/* 64 Mbit == 8 MiB */
	}
	else if (devid[3] == 0x18u)
	{
		deviceName = "Winbond W25Q128V";/* 128 Mbit == 16 MiB */
	}

#elif defined(CONFIG_SPI_FLASH_SPANSION)

	if (
		(devid[1] != 0x01u)				||	/* Manufacturer ID */
		(
			( (devid[2] != 0x20u) || (devid[3] != 0x18) )	&&	/* S25FL128S */
			( (devid[2] != 0x02u) || (devid[3] != 0x19) )		/* S25FL256S */
		)
	   )
	{
		printf("ERROR: Unknown SPI Device detected, devid = 0x%02x, 0x%02x, 0x%02x\n",
			devid[1], devid[2], devid[3]);
		return -1;
	}
	pageSize   = 256u;
	if (devid[5] == 0)				/* Uniform Sector Architecture ? */
	{
		eraseSize = 256u<<10;			/* Uniform 256 KiB sector */
	}
	else
	{
		eraseSize = 64u<<10;			/* Hybrid: 64 KiB + 4KiB parameter sectors  */
	}
	deviceSize = 1u<<devid[3];			/* Memory Capacity */
	if (devid[3] == 0x18u)
	{
		deviceName = "Spansion S25FL128S";	/* 128 Mbit == 16 MiB */
	}
	else if (devid[3] == 0x19u)
	{
		deviceName = "Spansion S25FL256S";	/* 256 Mbit == 32 MiB */
		op_read  = OP_READ_4BYTE;		/* use 4-byte addressing for READ */
		op_write = OP_PP_4BYTE;			/* use 4-byte addressing for WRITE */
		op_erase = OP_SE_4BYTE;			/* use 4-byte addressing for ERASE */
	}

#else
#error Please specify which SPI Serial Flash is being used
#endif	/* defined(CONFIG_STM_SPI_xxxxxx) */

#if 1
	/* tell them what we found */
	printf("info: found %s (%uMiB) device (page=%u,erase=%u)\n",
		deviceName,
		deviceSize >> 20,	/* in MiB */
		pageSize,		/* in bytes */
		eraseSize);		/* in bytes */
#endif

	/*
	 * With only 24-bit addresses, we can only access the first 16MiB!
	 */
	if ( (deviceSize > (1u << 24))	&&	/* need more than 24-bit addresses ? */
	     (NUM_ADDRESS_BYTES() == 3) )	/* but, still using 24-bit addressing ? */
	{
		printf("warning: SPI device (%s) effectively capped at first 16MiB only!\n", deviceName);
		deviceSize = 16u << 20;	/* cap to the first 16 MiB */
	}

#if defined(CONFIG_SPI_FLASH_ST) || defined(CONFIG_SPI_FLASH_MXIC) || defined(CONFIG_SPI_FLASH_WINBOND)
	/* is the device in a write protected mode ? */
	if (status & SR_BP_MASK)	/* BPx != 0 ? */
	{
		printf( "warning: "
			"SPI device may be write-protected (status=0x%02x)\n",
			status);
#if 0	/* do we want to un-lock it, if we can ? */
{
	const unsigned char enable[1] = { OP_WREN };
	const unsigned char unlock[2] = { OP_WRITE_STATUS, 0x00 };

	/* let the user know we are trying to un-lock it */
	printf("info: unlocking SPI ...\n");

	/* issue a WRITE ENABLE (WREN) command */
	spi_xfer(slave, sizeof(enable)*8, enable, NULL, DEFAULT_SPI_XFER_FLAGS);

	/* issue a WRITE Status Register (WRSR) command */
	spi_xfer(slave, sizeof(unlock)*8, unlock, NULL, DEFAULT_SPI_XFER_FLAGS);

	/* give it some time to clear the non-volatile flags */
	udelay(2 * 1000);	/* 2 ms */

	/* re-read (and display) the updated status register */
	status = spi_read_status(slave);
	if (status & SR_BP_MASK)	/* BPx != 0 ? */
	{	/* we MAY have succeeded, but we needed a longer delay! */
		printf("warning:            ... FAILED! (status=0x%02x)\n",
			status);
	}
	else
	{	/* the delay was long enough, and we succeeded. */
		printf("info:               ... succeeded.\n");
	}
}
#endif	/* unlock it */
	}
#endif	/* CONFIG_SPI_FLASH_ST || CONFIG_SPI_FLASH_MXIC || defined(CONFIG_SPI_FLASH_WINBOND) */

#if defined(CONFIG_SPI_FLASH_SPANSION)
	if (1)	/* if from the S25FLxxxS family ? */
	{
		/* Unlock all the sectors protected with DBY */
		spi_dby_unlock_all(slave);
	}
#endif	/* CONFIG_SPI_FLASH_SPANSION */

	return 0;
}


/**********************************************************************/


/*
 * initialise the H/W to talk to the slave SPI device.
 */
extern void spi_init(void)
{
#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
	/* initialize the H/W FSM SPI Controller. */
	fsm_init();
#else	/* CONFIG_STM_FSM_SPI */

#if defined(CONFIG_STM_SSC_SPI)		/* Use the H/W SSC for SPI */
	/* initialize the H/W SSC for SPI. */
	unsigned long reg;
	const unsigned long bits_per_word = 8;	/* one word == 8-bits */
	const unsigned long mode = CONFIG_SYS_STM_SPI_MODE /* | SPI_LOOP */;
	const unsigned long fcomms = stm_get_ssc_clk_rate();
	const unsigned long hz = CONFIG_SYS_STM_SPI_FREQUENCY;
	      unsigned long sscbrg = fcomms/(2*hz);

#if defined(CONFIG_ST40_STX5197)
	/* configure SSC0 to use the SPI pads (not PIO1[7:6]) */
	reg = *STX5197_HD_CONF_MON_CONFIG_CONTROL_M;
	reg |= 1ul<<14;	/* CONFIG_SYS_CTRL_M.SPI_BOOTNOTCOMMS = 1 [14] */
	*STX5197_HD_CONF_MON_CONFIG_CONTROL_M = reg;
#endif	/* CONFIG_ST40_STX5197 */

#endif	/* CONFIG_STM_SSC_SPI */

	/* de-assert SPI CS */
	spi_cs_deactivate(MyDefaultSlave);

#if defined(CONFIG_STM_SSC_SPI)		/* Use the H/W SSC for SPI */
	/* program the SSC's Baud-Rate Generator */
	if ((sscbrg < 0x07u) || (sscbrg > (0x1u << 16)))
	{
		printf("ERROR: Unable to set SSC buad-rate generator to 0x%04lx\n",
			sscbrg);
		return;
	}
	/* TODO: program pre-scaler for slower baud rates */
	if (sscbrg == (0x1 << 16)) /* 16-bit counter wraps */
	{
		sscbrg = 0x0;	/* slowest possible clock frequency */
	}
	ssc_write(SSC_BRG,sscbrg);
#if 0	/* QQQ */
	printf("info: fcomms=%uMHz, SPI=%uHz, brg=0x%04x\n",
		fcomms/1000/1000, hz, sscbrg);
#endif

	/* Disable I2C sub-system */
	ssc_write( SSC_I2C, 0x0);

	/* Perform a S/W reset the SSC */
	reg = ssc_read( SSC_CON);
	reg |= SSC_CON_SR;		/* enable software reset */
	ssc_write( SSC_CON, reg);
	udelay(1);			/* let reset propagate */
	reg = ssc_read( SSC_CON);
	reg &= ~SSC_CON_SR;		/* disable software reset */
	ssc_write( SSC_CON, reg);

	/* Configure & enable the SSC's control register */
	reg = ssc_read(SSC_CON);
	reg |= SSC_CON_EN;		/* Enable the SSC */
	reg |= SSC_CON_MS;		/* set SSC as the SPI master */
	if (mode & SPI_CPOL)
		reg |= SSC_CON_PO;	/* Clock idles at logic 1 */
	else
		reg &= ~SSC_CON_PO;	/* Clock idles at logic 0 */
	if (mode & SPI_CPHA)
		reg |= SSC_CON_PH;	/* Pulse in first half-cycle */
	else
		reg &= ~SSC_CON_PH;	/* Pulse in second half-cycle */
	if (mode & SPI_LSB_FIRST)
		reg &= ~SSC_CON_HB;	/* LSB first */
	else
		reg |= SSC_CON_HB;	/* MSB first */
	if (mode & SPI_LOOP)
		reg |= SSC_CON_LPB;	/* put SSC in loop-back mode */
	else
		reg &= ~SSC_CON_LPB;	/* remove SSC from loop-back mode */
	reg &= ~0x0ful;			/* set bit width */
	reg |= (bits_per_word - 1ul);	/* set bit width */
	ssc_write(SSC_CON,reg);

	/* clear the status register */
	(void)ssc_read(SSC_RBUF);
#endif	/* CONFIG_STM_SSC_SPI */
#endif	/* CONFIG_STM_FSM_SPI */

	/* now probe the serial flash, to ensure it is the correct one */
	spi_probe_serial_flash(MyDefaultSlave);

#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
#if defined(CONFIG_SPI_FLASH_ST) || defined(CONFIG_SPI_FLASH_SPANSION)
	/* Enable support for 4-Byte Address mode, if we need to */
	if (NUM_ADDRESS_BYTES() == 4)	/* using 4-byte addressing ? */
	{
		/* configure FSM driver to use opcodes with 32-bit addresses */
		fsm_init_4byte_mode(op_read, op_write, op_erase);
	}
#endif	/* CONFIG_SPI_FLASH_ST || CONFIG_SPI_FLASH_SPANSION */
#endif	/* CONFIG_STM_FSM_SPI */
}


/**********************************************************************/


extern void spi_init_f (void) { }
extern void spi_init_r (void) { }


/**********************************************************************/


static unsigned long get_binary_offset(const uchar * addr, int alen)
{
	unsigned long offset = 0;

	for(;alen>0;--alen)
	{
		offset <<= 8;
		offset |= *addr++;
	}

	return offset;
}


/**********************************************************************/


static unsigned long get_dataflash_offset(const unsigned long addr)
{
	unsigned long offset;

	/* optionally map a 'binary' address to a 'dataflash' address */
	if (pageSize == 528u)
	{	/* one page is 528 (0x210) bytes */
		const unsigned long page = addr / pageSize;
		const unsigned long byte = addr % pageSize;
		offset = (page << 10) | (byte);
	}
	else
	{	/* no special mapping needed */
		offset = addr;
	}

	return offset;
}


/**********************************************************************/


extern ssize_t spi_read (
	uchar * const addr,
	const int alen,
	uchar * const buffer,
	const int len)
{
	const unsigned long start = get_binary_offset(addr,alen);
	const unsigned long offset = get_dataflash_offset(start);
	const unsigned long last   = start + len - 1ul;
#if defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI)
	int i;
#endif	/* defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI) */

	if (len < 1) return len;
	if (deviceSize == 0) return 0;	/* no valid device found ? */
	if (last >= deviceSize)	/* Out of range ? */
	{
		printf("ERROR: Offset out of range (max=0x%lx)\n",
			deviceSize-1ul);
		return 0;
	}

#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
	fsm_read(buffer, len, offset);
#else	/* CONFIG_STM_FSM_SPI */

	/* assert SPI CS */
	spi_cs_activate(MyDefaultSlave);

	/* issue appropriate READ array command */
	spi_xfer_one_word(op_read);

	/* (optionally) write the MSB of the 32-bit start address */
	if (NUM_ADDRESS_BYTES() == 4)	/* using 4-byte addressing ? */
	{
		spi_xfer_one_word( (offset>>24) & 0xffu );
	}
	/* write the (bottom) 24-bit address to start reading from */
	spi_xfer_one_word( (offset>>16) & 0xffu );
	spi_xfer_one_word( (offset>>8)  & 0xffu );
	spi_xfer_one_word( (offset>>0)  & 0xffu );

#if (OP_READ_ARRAY==0xe8u)	/* Legacy command needs 4 dummy bytes */
	spi_xfer_one_word(0x00);
	spi_xfer_one_word(0x00);
	spi_xfer_one_word(0x00);
	spi_xfer_one_word(0x00);
#elif (OP_READ_ARRAY==0x0bu)	/* High-Speed command needs 1 dummy byte */
	spi_xfer_one_word(0x00);
#endif

	/* now read in each byte in turn, and put it in "buffer" */
	for(i=0; i<len; i++)
	{
		buffer[i] = spi_xfer_one_word(0x00);
	}

	/* de-assert SPI CS */
	spi_cs_deactivate(MyDefaultSlave);

#endif	/* CONFIG_STM_FSM_SPI */

	return len;
}


/**********************************************************************/


static void my_spi_write(
	struct spi_slave * const slave,
	const unsigned long address,
	const uchar * const buffer,
	unsigned long len)
#if defined(CONFIG_SPI_FLASH_ATMEL)
{
#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
#error ATMEL Serial Flash not yet supported with FSM SPI Controller!
#else	/* CONFIG_STM_FSM_SPI */
	const unsigned long offset = get_dataflash_offset(address);
	size_t i;

#if 0	/* QQQ - DELETE */
	printf("%s():\t buffer=0x%08x, len=%-3u  0x%06x..0x%06x\n",
		__FUNCTION__, buffer, len, address, address+len-1u);
#endif	/* QQQ - DELETE */

	if (len != pageSize)	/* partial page update ? */
	{
		/*
		 * Need to read, merge, erase & write one page.
		 * That is, we copy the whole page into one
		 * of the serial-flash buffers, and then just over-write
		 * with the new data to be updated (in 'buffer'). Then
		 * we erase & write back the whole (merged) page.
		 */

		const unsigned char transfer[4] = {
			OP_PAGE_TO_BUFFER1,
			(offset>>16) & 0xffu,
			(offset>>8)  & 0xffu,
			(offset>>0)  & 0xffu,
		};

		/* copy page (to be updated) in serial flash into buffer #1 */
		spi_xfer(slave, sizeof(transfer)*8, transfer, NULL, DEFAULT_SPI_XFER_FLAGS);

		/* now wait until the transfer has completed ... */
		spi_wait_till_ready(slave);
	}

	/* assert SPI CS */
	spi_cs_activate(slave);

	/* issue appropriate WRITE command */
	spi_xfer_one_word(OP_WRITE_VIA_BUFFER1);

	/* write the 24-bit address to start writing to */
	spi_xfer_one_word( (offset>>16) & 0xffu );
	spi_xfer_one_word( (offset>>8)  & 0xffu );
	spi_xfer_one_word( (offset>>0)  & 0xffu );

	/* now write in each byte in turn */
	for(i=0; i<len; i++)
	{
		spi_xfer_one_word(buffer[i]);
	}

	/* de-assert SPI CS */
	spi_cs_deactivate(slave);

	/* now wait until the programming has completed ... */
	spi_wait_till_ready(slave);
#endif	/* CONFIG_STM_FSM_SPI */
}
#elif defined(CONFIG_SPI_FLASH_ST) || defined(CONFIG_SPI_FLASH_MXIC) || defined(CONFIG_SPI_FLASH_WINBOND) || defined(CONFIG_SPI_FLASH_SPANSION)
{
	const unsigned pages       = eraseSize / pageSize;
	const unsigned long sector = (address / eraseSize) * eraseSize;
	unsigned long page_base;
	size_t i;
	unsigned page;
	const uchar * ptr;
#if defined(CONFIG_SPI_FLASH_ST) || defined(CONFIG_SPI_FLASH_SPANSION)
	unsigned char buff[256<<10];	/* maximum of 256 KiB erase size */
#elif defined(CONFIG_SPI_FLASH_MXIC) || defined(CONFIG_SPI_FLASH_WINBOND)
	unsigned char buff[4<<10];	/* maximum of 4 KiB erase size */
#endif
#if defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI)
	const unsigned char enable[1] = { OP_WREN };
	      unsigned char erase[5]  = { op_erase };
#endif	/* defined(CONFIG_SOFT_SPI) || defined(CONFIG_STM_SSC_SPI) */

#if 0	/* QQQ - DELETE */
	printf("%s():\t buffer=0x%08x, len=%-5lu  0x%06lx..0x%06lx\n",
		__FUNCTION__, (unsigned)buffer, len, address, address+len-1u);
#endif	/* QQQ - DELETE */

	if (len != eraseSize)	/* partial sector update ? */
	{
		/*
		 * Need to read, merge, erase & write one sector.
		 * That is, we copy the whole sector from
		 * the serial-flash into RAM, and then just over-write
		 * with the new data to be updated (in 'buffer'). Then
		 * we erase & write back the whole (merged) sector.
		 */
		unsigned char addr[4] = {
			(sector>>24) & 0xffu,
			(sector>>16) & 0xffu,
			(sector>>8)  & 0xffu,
			(sector>>0)  & 0xffu,
		};
		const unsigned long offset = address - sector;

		/* read the entire extant sector's content into RAM */
		spi_read ( addr, sizeof(addr), buff, eraseSize );

		/* now merge the old with the new data */
		for( i=0; i<len; i++)
		{
			buff[offset+i] = buffer[i];
		}
		ptr = buff;	/* use "merged" buffer */
	}
	else
	{
		ptr = buffer;	/* use original buffer */
	}

#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
	/* erase ONE sector (using command op_erase) */
	fsm_erase_sector(sector);
#else	/* CONFIG_STM_FSM_SPI */
	/* issue a WRITE ENABLE (WREN) command */
	spi_xfer(slave, sizeof(enable)*8, enable, NULL, DEFAULT_SPI_XFER_FLAGS);

	/* Transform the sector number, ready for serialization */
	if (NUM_ADDRESS_BYTES() == 3)	/* 3-byte addresses ? */
	{
		erase[1] = (sector>>16) & 0xffu;
		erase[2] = (sector>>8)  & 0xffu;
		erase[3] = (sector>>0)  & 0xffu;
	}
	else				/* else, 4-byte addresses */
	{
		erase[1] = (sector>>24) & 0xffu;
		erase[2] = (sector>>16) & 0xffu;
		erase[3] = (sector>>8)  & 0xffu;
		erase[4] = (sector>>0)  & 0xffu;
	}

	/* issue a Sector Erase command */
	spi_xfer(slave, (NUM_ADDRESS_BYTES()+1)*8, erase, NULL, DEFAULT_SPI_XFER_FLAGS);
#endif	/* CONFIG_STM_FSM_SPI */

	/* now wait until the erase has completed ... */
	spi_wait_till_ready(slave);

	/* now program each page in turn ... */
	for (page_base=sector,page=0u; page<pages; page++)
	{
		/* program one page... */
#if defined(CONFIG_STM_FSM_SPI)		/* Use the H/W FSM for SPI */
		fsm_write(ptr, pageSize, page_base);
		ptr += pageSize;
#else	/* CONFIG_STM_FSM_SPI */
		/* issue a WRITE ENABLE (WREN) command */
		spi_xfer(slave, sizeof(enable)*8, enable, NULL, DEFAULT_SPI_XFER_FLAGS);

		/* assert SPI CS */
		spi_cs_activate(slave);

		/* issue a Page Program command */
		spi_xfer_one_word(op_write);

		/* (optionally) write the MSB of the 32-bit start address */
		if (NUM_ADDRESS_BYTES() == 4)	/* using 4-byte addressing ? */
		{
			spi_xfer_one_word( (page_base>>24) & 0xffu );
		}
		/* write the (bottom) 24-bit address to start writing to */
		spi_xfer_one_word( (page_base>>16) & 0xffu );
		spi_xfer_one_word( (page_base>>8)  & 0xffu );
		spi_xfer_one_word( (page_base>>0)  & 0xffu );

		/* now write in each byte in turn */
		for(i=0; i<pageSize; i++)
		{
			spi_xfer_one_word(*ptr++);
		}

		/* de-assert SPI CS */
		spi_cs_deactivate(slave);
#endif	/* CONFIG_STM_FSM_SPI */

		/* now wait until the programming has completed ... */
		spi_wait_till_ready(slave);

		/* advance to next page */
		page_base += pageSize;
	}
}
#else
#error Please specify which SPI Serial Flash is being used
#endif	/* defined(CONFIG_STM_SPI_xxxxxx) */


/**********************************************************************/


extern ssize_t spi_write (
	uchar * const addr,
	const int alen,
	uchar * buffer,
	const int len)
{
	const unsigned long first  = get_binary_offset(addr,alen);
	const unsigned long last   = first + len - 1ul;
	const unsigned long lsector= (first + len) / eraseSize;
	      unsigned long sector = first / eraseSize;
	const unsigned long byte   = first % eraseSize;
	      unsigned long ptr    = first;
	unsigned written = 0;		/* amount written between two dots */
	struct spi_slave * const slave = MyDefaultSlave;

	if (len < 1) return len;
	if (deviceSize == 0) return 0;	/* no valid device found ? */
	if (last >= deviceSize)	/* Out of range ? */
	{
		printf("ERROR: Offset out of range (max=0x%lx)\n",
			deviceSize-1ul);
		return 0;
	}

#if defined(CONFIG_SPI_FLASH_ST)
	/* Enter 4-Byte Address mode, if we need to */
	if (NUM_ADDRESS_BYTES() == 4)	/* using 4-byte addressing ? */
	{
		spi_enter_4byte_mode(slave, 1);
	}
#endif	/* CONFIG_SPI_FLASH_ST */

	/* process up to end of first erase block */
	if (byte != 0)
	{
		/* till end of first erase block, or entirety, which is less */
		const unsigned long size = MIN(len,eraseSize-byte);
		my_spi_write(slave, first, buffer, size);

		sector++;
		ptr += size;
		buffer += size;
		written += size;
	}

	/* process each whole erase block in turn */
	while(sector<lsector)
	{
		/* print a series of dots (every 16KiB) */
		if ( (written+=eraseSize,written) >= (16u<<10) )
		{
			printf(".");	/* print a dot */
			written = 0;	/* reset counter */
		}

		/* a whole erase block */
		my_spi_write(slave, ptr, buffer, eraseSize);

		sector++;
		ptr += eraseSize;
		buffer += eraseSize;

	}
	printf("\n");	/* terminate any row of printed dots */

	/* finally, process any data at the tail */
	if (ptr <= last)
	{
		my_spi_write(slave, ptr, buffer, last-ptr+1u);
	}

#if defined(CONFIG_SPI_FLASH_ST)
	/* Exit from 4-Byte Address mode, if we needed to use it */
	if (NUM_ADDRESS_BYTES() == 4)	/* using 4-byte addressing ? */
	{
		spi_enter_4byte_mode(slave, 0);
	}
#endif	/* CONFIG_SPI_FLASH_ST */

	return len;
}


/**********************************************************************/


#endif	/* defined(CONFIG_SPI) */


