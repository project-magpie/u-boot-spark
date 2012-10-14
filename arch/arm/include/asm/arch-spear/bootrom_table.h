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

#ifndef ASM_ARCH_BOOTROM_TABLE_H
#define ASM_ARCH_BOOTROM_TABLE_H

/*
 * BootROM Table Structures
 */
#define BOOTROM_TABLE_ADDRESS		0xFFFF7F00

#define BOOTROM_TABLE_VERSION_1_0	1

struct bootrom_table_1_0 {
	const void *boot_flashdetectandinit_ptr;
	const void *boot_flashread_ptr;
	const void *boot_nandsanitycheck_ptr;
	const void *boot_nandreadpage_ptr;
};

/*
 * Generic bootrom table structure's union. Contains the table structure for
 * all versions
 */
union bootrom_ver_table {
	struct bootrom_table_1_0 table_1_0;
};

struct bootrom_table {
	const unsigned int table_version;
	union bootrom_ver_table table;
};

#endif
