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
#include <errno.h>

#if defined(CONFIG_STM)
#include <stm/stm-nand.h>
#endif

#ifndef CONFIG_SYS_NAND_BASE_LIST
#define CONFIG_SYS_NAND_BASE_LIST { CONFIG_SYS_NAND_BASE }
#endif

DECLARE_GLOBAL_DATA_PTR;

int nand_curr_device = -1;


nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];

#ifndef CONFIG_SYS_NAND_SELF_INIT
static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];
static ulong base_address[CONFIG_SYS_MAX_NAND_DEVICE] = CONFIG_SYS_NAND_BASE_LIST;
#endif

static char dev_name[CONFIG_SYS_MAX_NAND_DEVICE][8];

static unsigned long total_nand_size; /* in kiB */

/* Register an initialized NAND mtd device with the U-Boot NAND command. */
int nand_register(int devnum)
{
	struct mtd_info *mtd;

	if (devnum >= CONFIG_SYS_MAX_NAND_DEVICE)
		return -EINVAL;

	mtd = &nand_info[devnum];

	sprintf(dev_name[devnum], "nand%d", devnum);
	mtd->name = dev_name[devnum];

#ifdef CONFIG_MTD_DEVICE
	/*
	 * Add MTD device so that we can reference it later
	 * via the mtdcore infrastructure (e.g. ubi).
	 */
	add_mtd_device(mtd);
#endif

	total_nand_size += mtd->size / 1024;

	if (nand_curr_device == -1)
		nand_curr_device = devnum;

	return 0;
}


#if defined(CONFIG_STM)
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
	 * the *actual* device ID, we can complete any other STM-specific
	 * structure fields properly (e.g. nand->ecc.layout).
	 */
	stm_nand_scan_post_ident(mtd);

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
	stm_nand_scan_post_tail(mtd);

	return ret;
}
#define nand_scan stm_nand_scan		/* kludge: map nand_scan() to stm_nand_scan() */
#endif	/* CONFIG_STM */


#ifndef CONFIG_SYS_NAND_SELF_INIT
static void nand_init_chip(int i)
{
	struct mtd_info *mtd = &nand_info[i];
	struct nand_chip *nand = &nand_chip[i];
	ulong base_addr = base_address[i];
	int maxchips = CONFIG_SYS_NAND_MAX_CHIPS;

	if (maxchips < 1)
		maxchips = 1;

	mtd->priv = nand;
	nand->priv = mtd;	/* so board_nand_init() can use 'mtd' */
	nand->IO_ADDR_R = nand->IO_ADDR_W = (void  __iomem *)base_addr;

	if (board_nand_init(nand))
		return;

	if (nand_scan(mtd, maxchips))
		return;

	nand_register(i);
}
#endif

void nand_init(void)
{
#ifdef CONFIG_SYS_NAND_SELF_INIT
	board_nand_init();
#else
	int i;

	for (i = 0; i < CONFIG_SYS_MAX_NAND_DEVICE; i++)
		nand_init_chip(i);
#endif

	printf("%lu MiB\n", total_nand_size / 1024);

#ifdef CONFIG_SYS_NAND_SELECT_DEVICE
	/*
	 * Select the chip in the board/cpu specific driver
	 */
	board_nand_select_device(nand_info[nand_curr_device].priv, nand_curr_device);
#endif
}
