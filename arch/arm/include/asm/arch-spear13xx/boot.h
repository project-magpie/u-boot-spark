/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Microelectronics, vipin.kumar@st.com
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

#ifndef __ASM_ARCH_BOOT_H
#define __ASM_ARCH_BOOT_H

#include <linux/compiler.h>

/* This file defines the BootROM <-> Xloader interface for spear13xx devices */

/*****************************************************************************
 * SPEAr1310 as well as SPEAr1340 device support the same table version ie 2.1
 *****************************************************************************/

/*
 * BootROM provides a table as an interface for its routines to be used in
 * xloader. This file needs to be compatible with all the supported spear13xx
 * bootrom table interfaces.
 *
 * All the following definitions are for using already existing BootROM routines
 */

struct soc_type_s {
	u8	soc;
	u8	revision;
};

struct table_2_1 {
	struct soc_type_s *	(*get_soc_type)(void);
	u8			(*get_boot_type)(void);

	void *			nand_info;
	int			(*nand_read)(void *nand, size_t offset,
					size_t *length, u_char *buffer);
	u8 *			(*get_version)(void);

	int			(*get_otpbits)(u32 bit_off, u32 bit_cnt,
					u32 *buffer);

	u32			(*hamming_encode)(u32 parity, void *data,
					unsigned int d, unsigned int nr);
	void			(*hamming_fix)(void *data, unsigned int d,
					unsigned int nr, unsigned int fix);
};

union table_u {
	struct table_2_1	table_2_1;
};

struct table_s {
	u32			table_version;
#define TABLE_VERSION_2_1  3

	union table_u		table_u;
} __packed;

/* These routines are encapsulated based on the table version */
/*
 * getboottype routine returns one of these independent of the spear platform
 */
#define BOOT_TYPE_BYPASS	1
#define BOOT_TYPE_SMI		2
#define BOOT_TYPE_NAND		3
#define BOOT_TYPE_RESERVED	4
#define BOOT_TYPE_I2C		5
#define BOOT_TYPE_SPI		6
#define BOOT_TYPE_USBD		7
#define BOOT_TYPE_TFTP		8
#define BOOT_TYPE_PCIE		9
#define BOOT_TYPE_UART		10
#define BOOT_TYPE_MMC		11
#define BOOT_TYPE_PNOR8		12
#define BOOT_TYPE_PNOR16	13
#define BOOT_TYPE_PNOR32	14
#define BOOT_TYPE_UNSUPPORTED	0xff

static inline u8 get_boot_type(void)
{
	struct table_s *table = (struct table_s *)0xFFFF7F00;

	if (table->table_version == TABLE_VERSION_2_1)
		return (*table->table_u.table_2_1.get_boot_type)();

	return BOOT_TYPE_UNSUPPORTED;
}
#endif
