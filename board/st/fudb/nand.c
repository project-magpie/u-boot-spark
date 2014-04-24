/*
 * (C) Copyright 2006 DENX Software Engineering
 * (C) Copyright 2008-2014 STMicroelectronics, Sean McGoogan <Sean.McGoogan@st.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <stm/pio.h>
#include <stm/socregs.h>
#include <stm/stm-nand.h>


/*
 * Board-specific NAND initialization.
 * We use a "generic" STM function stm_default_board_nand_init() to do it.
 * However, we can easily override anything locally, if required.
 */
#if defined(CONFIG_SYS_STM_NAND_USE_BIT_BANGING)	/* use the "EMI bit-banging" driver */
#error It is not possible to use "EMI Bit-Banging" with NAND on this system!
#else						/* else, use a H/W driver */
extern int board_nand_init(struct nand_chip * const nand)
{
	stm_default_board_nand_init(nand, NULL, NULL);

	/*
	 * Only enable the following to use a (volatile) RAM-based
	 * (not NAND-resident) Bad Block Table (BBT).
	 * This is *not* recommended! A NAND-resident BBT is recommended.
	 */
#if 0
	nand->options &= ~NAND_USE_FLASH_BBT;
#endif

	return 0;
}
#endif /* CONFIG_SYS_STM_NAND_USE_BIT_BANGING */
