/*
 * SPEAr generic declarations
 *
 * Copyright (C) 2009 ST Microelectronics
 * Shiraz Hashim <shiraz.hashim@st.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __MACH_SPEAR300_H
#define __MACH_SPEAR300_H

#define RAS_REG1 0x99000000
#define RAS_REG2 0x99000004

extern void enable_i2c_pads(void);
extern void enable_ssp_pads(void);
extern void enable_uart0_pads(void);
extern void enable_fsmc_nand_16bit_pads(void);
extern void enable_fsmc_nand_8bit_pads(void);
extern void enable_sdhci_pads(void);
extern void enable_mii_pads(void);

#endif /* _SPR_GENERIC_ */
