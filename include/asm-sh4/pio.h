/*
 * (C) Copyright 2005
 * Andy Stugres, STMicroelectronics, <andy.sturges@st.com>
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

#ifndef _PIO_H_
#define _PIO_H_	1

#define STPIO_NONPIO            0       /* Non-PIO function (ST40 defn) */
#define STPIO_BIDIR_Z1          0       /* Input weak pull-up (arch defn) */
#define STPIO_BIDIR             1       /* Bidirectonal open-drain */
#define STPIO_OUT               2       /* Output push-pull */
/*efine STPIO_BIDIR             3        * Bidirectional open drain */
#define STPIO_IN                4       /* Input Hi-Z */
/*efine STPIO_IN                5        * Input Hi-Z */
#define STPIO_ALT_OUT           6       /* Alt output push-pull (arch defn) */
#define STPIO_ALT_BIDIR         7       /* Alt bidir open drain (arch defn) */

#define STPIO_POUT_OFFSET	0x00
#define STPIO_PIN_OFFSET	0x10
#define STPIO_PC0_OFFSET	0x20
#define STPIO_PC1_OFFSET	0x30
#define STPIO_PC2_OFFSET	0x40
#define STPIO_PCOMP_OFFSET	0x50
#define STPIO_PMASK_OFFSET	0x60

#define STPIO_SET_OFFSET	0x4
#define STPIO_CLEAR_OFFSET	0x8

#define PIO_PORT_SIZE 0x1000

#define PIO_PORT(n) (((n)*PIO_PORT_SIZE) + PIO_BASE)

#define PIN_C0(PIN, DIR) (((DIR & 0x1)!=0) << PIN)
#define PIN_C1(PIN, DIR) (((DIR & 0x2)!=0) << PIN)
#define PIN_C2(PIN, DIR) (((DIR & 0x4)!=0) << PIN)

#define SET_PIO_PIN(PIO_ADDR, PIN, DIR) do {writel(PIN_C0(PIN,DIR),PIO_ADDR+0x24);\
				    writel(PIN_C1(PIN,DIR),PIO_ADDR+0x34);\
				    writel(PIN_C2(PIN,DIR),PIO_ADDR+0x44);} while (0)

#define STPIO_SET_PIN(PIO_ADDR, PIN, V) writel(1<<PIN, PIO_ADDR + STPIO_POUT_OFFSET + ((V)? STPIO_SET_OFFSET : STPIO_CLEAR_OFFSET))

#define SET_PIO_ASC(PIO_ADDR, TX, RX, CTS, RTS) \
	writel(PIN_C0(TX, STPIO_ALT_OUT) | PIN_C0(RX, STPIO_IN) | \
	       PIN_C0(CTS, STPIO_IN) | PIN_C0(RTS, STPIO_ALT_OUT),\
	       PIO_ADDR+0x24) ; \
	writel(PIN_C1(TX, STPIO_ALT_OUT) | PIN_C1(RX, STPIO_IN) | \
	       PIN_C1(CTS, STPIO_IN) | PIN_C1(RTS, STPIO_ALT_OUT),\
	       PIO_ADDR+0x34) ; \
	writel(PIN_C2(TX, STPIO_ALT_OUT) | PIN_C2(RX, STPIO_IN) | \
	       PIN_C2(CTS, STPIO_IN) | PIN_C2(RTS, STPIO_ALT_OUT),\
	       PIO_ADDR+0x44) ;

#endif
