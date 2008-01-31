/*
 * Copyright (C) 2007-2008 STMicroelectronics Limited
 * David McKay <David.McKay@st.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __STBUS_H__
#define __STBUS_H__

/*--------------------------------------------------------------*
 * These register are described in ADCS 7801572B.
 *--------------------------------------------------------------*/

/* Defines for the USB controller register offsets. */
#define UHOST2C_BASE			0xb9100000
#define AHB2STBUS_WRAPPER_GLUE_BASE	(UHOST2C_BASE + 0x00000000)
#define AHB2STBUS_OHCI_BASE		(UHOST2C_BASE + 0x000ffc00)
#define AHB2STBUS_PROTOCOL_BASE		(UHOST2C_BASE + 0x000fff00)

/* The transaction opcode is programmed in this register. */
#define AHB2STBUS_STBUS_OPC		(AHB2STBUS_PROTOCOL_BASE + 0x00)
#define AHB2STBUS_STBUS_OPC_4BIT	0x00
#define AHB2STBUS_STBUS_OPC_8BIT	0x01
#define AHB2STBUS_STBUS_OPC_16BIT	0x02
#define AHB2STBUS_STBUS_OPC_32BIT	0x03
#define AHB2STBUS_STBUS_OPC_64BIT	0x04

/* The message length in number of packets is programmed in this register. */
#define AHB2STBUS_MSGSIZE		(AHB2STBUS_PROTOCOL_BASE + 0x04)
#define AHB2STBUS_MSGSIZE_DISABLE	0x0
#define AHB2STBUS_MSGSIZE_2		0x1
#define AHB2STBUS_MSGSIZE_4		0x2
#define AHB2STBUS_MSGSIZE_8		0x3
#define AHB2STBUS_MSGSIZE_16		0x4
#define AHB2STBUS_MSGSIZE_32		0x5
#define AHB2STBUS_MSGSIZE_64		0x6

/* The chunk size in number of packets is programmed in this register. */
#define AHB2STBUS_CHUNKSIZE		(AHB2STBUS_PROTOCOL_BASE + 0x08)
#define AHB2STBUS_CHUNKSIZE_DISABLE	0x0
#define AHB2STBUS_CHUNKSIZE_2		0x1
#define AHB2STBUS_CHUNKSIZE_4		0x2
#define AHB2STBUS_CHUNKSIZE_8		0x3
#define AHB2STBUS_CHUNKSIZE_16		0x4
#define AHB2STBUS_CHUNKSIZE_32		0x5
#define AHB2STBUS_CHUNKSIZE_64		0x6

/* AHB Strap options are programmed in this register. */
#define AHB2STBUS_STRAP			(AHB2STBUS_WRAPPER_GLUE_BASE + 0x14)
#define AHB2STBUS_STRAP_PLL		0x08	/* PLL_PWR_DWN */
#define AHB2STBUS_STRAP_16_BIT		0x04	/* SS_WORD_IF */

/* SYSCONF registers. */
#define SYSCONF_BASE			0xb9001000
#define SYS_CFG2			(SYSCONF_BASE + 0x108)
#define SYS_CFG2_PLL_POWER_DOWN_BIT	1

#endif	/* __STBUS_H__ */
