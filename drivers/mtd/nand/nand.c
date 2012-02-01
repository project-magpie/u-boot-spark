/*
 * (C) Copyright 2005
 * 2N Telekomunikace, a.s. <www.2n.cz>
 * Ladislav Michl <michl@2n.cz>
 * Copyright (C) 2009-2012 STMicroelectronics, Sean McGoogan <Sean.McGoogan@st.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
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
#include <nand.h>

#if defined(CONFIG_ST40)
#include <asm/stm-nand.h>
#endif

#ifndef CFG_NAND_BASE_LIST
#define CFG_NAND_BASE_LIST { CFG_NAND_BASE }
#endif

int nand_curr_device = -1;
nand_info_t nand_info[CFG_MAX_NAND_DEVICE] = {{0}};

static struct nand_chip nand_chip[CFG_MAX_NAND_DEVICE];
static ulong base_address[CFG_MAX_NAND_DEVICE] = CFG_NAND_BASE_LIST;

static const char default_nand_name[] = "nand";

extern int board_nand_init(struct nand_chip *nand);



#if defined(CONFIG_ST40)
/**
 * nand_scan - [NAND Interface] Scan for the NAND device
 * @mtd:	MTD device structure
 * @maxchips:	Number of chips to scan for
 *
 * This fills out all the uninitialized function pointers
 * with the defaults.
 * The flash ID is read and the mtd/chip structures are
 * filled with the appropriate values.
 * The mtd->owner field must be set to the module of the caller
 *
 */
static int stm_nand_scan(struct mtd_info *mtd, int maxchips)
{
	int ret;

	/*
	 * Perform the first phase of the normal nand_scan() function.
	 * It reads the flash ID and sets up MTD fields accordingly.
	 */
	ret = nand_scan_ident(mtd, maxchips);
	if (ret)
		return ret;

	/*
	 * Now that we have probed the physical NAND device, and we now know
	 * the *actual* device ID, we can complete any other ST40-specific
	 * structure fields properly (e.g. nand->ecc.layout).
	 */
	stm_nand_chip_init(mtd);

	/*
	 * Perform the second phase of the normal nand_scan() function.
	 * If fills out the remaining uninitialized function pointers.
	 */
	ret = nand_scan_tail(mtd);
	if (ret)
		return ret;

	/*
	 * Finally, perform any post-scanning initialization we want to do.
	 */
	stm_nand_chip_init_end(mtd);

	return ret;
}
#define nand_scan stm_nand_scan		/* kludge: map nand_scan() to stm_nand_scan() */
#endif	/* CONFIG_ST40 */


static void nand_init_chip(struct mtd_info *mtd, struct nand_chip *nand,
			   ulong base_addr)
{
	mtd->priv = nand;
	nand->priv = mtd;	/* so board_nand_init() can use 'mtd' */

	nand->IO_ADDR_R = nand->IO_ADDR_W = (void  __iomem *)base_addr;
	if (board_nand_init(nand) == 0) {
		if (nand_scan(mtd, 1) == 0) {
			if (!mtd->name)
				mtd->name = (char *)default_nand_name;
		} else
			mtd->name = NULL;
	} else {
		mtd->name = NULL;
		mtd->size = 0;
	}

}

void nand_init(void)
{
	int i;
	unsigned int size = 0;
	for (i = 0; i < CFG_MAX_NAND_DEVICE; i++) {
		nand_init_chip(&nand_info[i], &nand_chip[i], base_address[i]);
		size += nand_info[i].size / 1024;
		if (nand_curr_device == -1)
			nand_curr_device = i;
	}
	printf("%u MiB\n", size / 1024);

#ifdef CFG_NAND_SELECT_DEVICE
	/*
	 * Select the chip in the board/cpu specific driver
	 */
	board_nand_select_device(nand_info[nand_curr_device].priv, nand_curr_device);
#endif
}
