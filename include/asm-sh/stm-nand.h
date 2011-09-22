/*
 * (C) Copyright 2008-2011 STMicroelectronics, Sean McGoogan <Sean.McGoogan@st.com>
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

#include <nand.h>


extern int stm_nand_default_bbt(
	struct mtd_info * const mtd);


#ifdef CFG_NAND_ECC_HW3_128	/* for STM "boot-mode" ECC (3 bytes/128 byte record) */

extern void stm_nand_init(
	struct mtd_info * const mtd);

extern void stm_nand_select_chip(
	struct mtd_info * const mtd,
	const int chipnr);

#endif	/* CFG_NAND_ECC_HW3_128 */


#ifdef CFG_NAND_FLEX_MODE	/* for STM "FLEX-mode" */

extern int stm_flex_device_ready(
	struct mtd_info * const mtd);

extern void stm_flex_select_chip(
	struct mtd_info * const mtd,
	const int chipnr);

extern void stm_flex_hwcontrol(
	struct mtd_info * const mtd,
	int control);

extern u_char stm_flex_read_byte(
	struct mtd_info * const mtd);

extern void stm_flex_write_byte(
	struct mtd_info * const mtd,
	u_char byte);

extern void stm_flex_read_buf(
	struct mtd_info * const mtd,
	u_char * const buf,
	const int len);

extern void stm_flex_write_buf(
	struct mtd_info * const mtd,
	const u_char *buf,
	const int len);

#endif /* CFG_NAND_FLEX_MODE */


