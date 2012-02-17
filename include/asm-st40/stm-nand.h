/*
 * (C) Copyright 2008-2012 STMicroelectronics, Sean McGoogan <Sean.McGoogan@st.com>
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


#ifndef __ASM_ST40_STM_NAND_H
#define __ASM_ST40_STM_NAND_H


struct nand_chip;	/* defined elsewhere */
struct mtd_info;	/* defined elsewhere */


extern void stm_default_board_nand_init(
	struct nand_chip * const nand,
	void (*cmd_ctrl)(struct mtd_info *mtdinfo, int dat, unsigned int ctrl),
	int (*dev_ready)(struct mtd_info *mtd));

extern void stm_flex_init_nand(
	struct mtd_info * const mtd,
	struct nand_chip * const nand);

extern void stm_bch_init_nand(
	struct mtd_info * const mtd,
	struct nand_chip * const nand);

extern void stm_nand_scan_post_ident(
	struct mtd_info * const mtd);

extern void stm_nand_scan_post_tail(
	struct mtd_info * const mtd);

extern int stm_bch_scan_read_raw(
	struct mtd_info *mtd,
	uint8_t *buf,
	loff_t offs,
	size_t len);

#endif /* __ASM_ST40_STM_NAND_H */
