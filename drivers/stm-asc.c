/*
 * drivers/stm-asc.c
 *
 * Support for Serial I/O using STMicroelectronics' on-chip ASC.
 *
 *  Copyright (c) 2004  STMicroelectronics Limited
 *  Copyright (C) 1999  Takeshi Yaegachi & Niibe Yutaka
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License.  See the file "COPYING.LIB" in the main
 * directory of this archive for more details.
 *
 */

#include "common.h"

#ifdef CONFIG_STM_ASC_SERIAL

#include "asm/termbits.h"
#include "asm/io.h"
#include "asm/pio.h"

#define CREAD	0000200
#define HUPCL	0002000
#define CLOCAL	0004000
#define	CSTOPB	0000100
#define CSIZE	0000060
#define CS7	0000040
#define CS8	0000060
#define PARENB	0000400
#define PARODD  0001000

#define BAUDMODE	0x00001000
#define CTSENABLE	0x00000800
#define RXENABLE	0x00000100
#define RUN		0x00000080
#define LOOPBACK	0x00000000
#define STOPBIT		0x00000008
#define MODE		0x00000001
#define MODE_7BIT_PAR	0x0003
#define MODE_8BIT_PAR	0x0007
#define MODE_8BIT	0x0001
#define STOP_1BIT	0x0008
#define PARITYODD	0x0020

#define STA_NKD		0x0400
#define STA_TF		0x0200
#define STA_RHF		0x0100
#define STA_TOI		0x0080
#define STA_TNE		0x0040
#define STA_OE		0x0020
#define STA_FE		0x0010
#define STA_PE		0x0008
#define	STA_THE		0x0004
#define STA_TE		0x0002
#define STA_RBF		0x0001


#define UART_BAUDRATE_OFFSET    0x00
#define UART_TXBUFFER_OFFSET    0x04
#define UART_RXBUFFER_OFFSET    0x08
#define UART_CONTROL_OFFSET     0x0C
#define UART_INTENABLE_OFFSET   0x10
#define UART_STATUS_OFFSET      0x14
#define UART_GUARDTIME_OFFSET   0x18
#define UART_TIMEOUT_OFFSET     0x1C
#define UART_TXRESET_OFFSET     0x20
#define UART_RXRESET_OFFSET     0x24
#define UART_RETRIES_OFFSET     0x28

#define UART_BAUDRATE_REG   (CFG_STM_ASC_BASE + UART_BAUDRATE_OFFSET)
#define UART_TXBUFFER_REG   (CFG_STM_ASC_BASE + UART_TXBUFFER_OFFSET)
#define UART_RXBUFFER_REG   (CFG_STM_ASC_BASE + UART_RXBUFFER_OFFSET)
#define UART_CONTROL_REG    (CFG_STM_ASC_BASE + UART_CONTROL_OFFSET)
#define UART_INTENABLE_REG  (CFG_STM_ASC_BASE + UART_INTENABLE_OFFSET)
#define UART_STATUS_REG     (CFG_STM_ASC_BASE + UART_STATUS_OFFSET)
#define UART_GUARDTIME_REG  (CFG_STM_ASC_BASE + UART_GUARDTIME_OFFSET)
#define UART_TIMEOUT_REG    (CFG_STM_ASC_BASE + UART_TIMEOUT_OFFSET)
#define UART_TXRESET_REG    (CFG_STM_ASC_BASE + UART_TXRESET_OFFSET)
#define UART_RXRESET_REG    (CFG_STM_ASC_BASE + UART_RXRESET_OFFSET)
#define UART_RETRIES_REG    (CFG_STM_ASC_BASE + UART_RETRIES_OFFSET)


/*---- Values for the BAUDRATE Register -----------------------*/


#if defined(CONFIG_SH_STB7100) || defined(CONFIG_SH_STX7200)
#define PCLK			(gd->bd->bi_emifrq*1000000)
#define BAUDRATE_VAL_M0(bps)	(PCLK / (16 * (bps)))
#define BAUDRATE_VAL_M1(bps)    ( (((bps * (1 << 14))+ (1<<13)) / (PCLK/(1 << 6))))
#else
#define PCLK			B_CLOCK_RATE
#define BAUDRATE_VAL_M0(bps)	(PCLK / (16 * (bps)))
#define BAUDRATE_VAL_M1(bps)	(int)((((double)bps * (1 << 20))/ PCLK)+0.5)
#endif

/*
 * MODE 0
 *                       ICCLK
 * ASCBaudRate =   ----------------
 *                   baudrate * 16
 *
 * MODE 1
 *                   baudrate * 16 * 2^16
 * ASCBaudRate =   ------------------------
 *                          ICCLK
 */

#define BPS_2400	BAUDRATE_VAL_M0(2400)
#define BPS_4800	BAUDRATE_VAL_M0(4800)
#define BPS_9600	BAUDRATE_VAL_M0(9600)
#define BPS_19200	BAUDRATE_VAL_M1(19200)
#define BPS_38400	BAUDRATE_VAL_M1(38400)
#define BPS_57600	BAUDRATE_VAL_M1(57600)
#define BPS_96000	BAUDRATE_VAL_M1(96000)
#define BPS_115200	BAUDRATE_VAL_M1(115200)


#if defined(CONFIG_STM_ST231)

#define p2_outl(addr,b) writel(b,addr)
#define p2_inl(addr)    readl(addr)

#endif


/* This function should be implemented for asc driver */
void handleError (void)
{
	return;
}

static int hwflow = 0;		/* turned off by default */

int serial_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	int cflag = CREAD | HUPCL | CLOCAL | CSTOPB;
	unsigned int status;
	int baud = gd->baudrate;
	int t;
	int p2_val;
	switch (baud) {
	case 0:
		t = -1;
		break;
	case 2400:
		t = BPS_2400;
		break;
	case 4800:
		t = BPS_4800;
		break;
	case 9600:
		t = BPS_9600;
		break;
	case 19200:
		t = BPS_19200;
		break;
	case 38400:
		t = BPS_38400;
		break;
	case 57600:
		t = BPS_57600;
		break;
	default:
		printf ("sci: unsupported baud rate: %d, using 115200 instead.\n", baud);
	case 115200:
		t = BPS_115200;
		break;
	}

	cflag |= CS8;
	cflag |= PARODD;

	/* wait for end of current transmission */
	do {
		status = p2_inl (UART_STATUS_REG);
	} while (status & STA_TF);

	p2_val = p2_inl (UART_CONTROL_REG);
	p2_outl (UART_CONTROL_REG, (p2_val & ~RUN));
	p2_val = RXENABLE;

	/* reset fifo rx e tx */
	p2_outl (UART_TXRESET_REG, 1);
	p2_outl (UART_RXRESET_REG, 1);

	/* set character length */
	if ((cflag & CSIZE) == CS7)
		p2_val |= MODE_7BIT_PAR;
	else {
		if (cflag & PARENB)
			p2_val |= MODE_8BIT_PAR;
		else
			p2_val |= MODE_8BIT;
	}

	/* set stop bit */
	/* it seems no '0 stop bits' option is available: by default
	 * we get 0.5 stop bits */
	if (cflag & CSTOPB)
		p2_val |= STOP_1BIT;

	/* odd parity */
	if (cflag & PARODD)
		p2_val |= PARITYODD;

	/*  set flow control */
	if (hwflow)
		p2_val |= CTSENABLE;

	/* set speed and baud generator mode */
	p2_outl (UART_BAUDRATE_REG, t);
	p2_val |= BAUDMODE;

	/* write final value and enable port */
	p2_outl (UART_CONTROL_REG, (p2_val | RUN));
	return 0;
}

int serial_tstc (void)
{
	unsigned short status;

	status = p2_inl (UART_STATUS_REG);
	return (status & STA_RBF);
}

/* This function doesn't correctly work */
int serial_getc (void)
{
	char ch;

	while (!serial_tstc ());

	ch = p2_inl (UART_RXBUFFER_REG);

	return ch;
}

static inline int putDebugCharReady (void)
{
	unsigned long status;

	status = p2_inl (UART_STATUS_REG);
	return !(status & STA_TF);
}

void serial_putc (char ch)
{
	if (ch == 10)
		serial_putc ('\r');
	while (!putDebugCharReady ());
	p2_outl (UART_TXBUFFER_REG, ch);
}

void serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}


void serial_setbrg (void)
{

  serial_init();
}

#ifdef CONFIG_HWFLOW
int hwflow_onoff (int on)
{
	switch (on) {
	case 0:
	default:
		break;		/* return current */
	case 1:
		hwflow = 1;	/* turn on */
		serial_init ();
		break;
	case -1:
		hwflow = 0;	/* turn off */
		serial_init ();
		break;
	}
	return hwflow;
}
#endif

#endif	/* CONFIG_STM_ASC_SERIAL */
