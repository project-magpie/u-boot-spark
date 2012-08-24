/*
 * SPEAr310 generic declarations
 *
 * Copyright (C) 2009 ST Microelectronics
 * Shiraz Hashim <shiraz.hashim@st.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __MACH_SPEAR310_H
#define __MACH_SPEAR310_H

#define PL_GPIO_EN0 0xB4000010
#define PL_GPIO_EN1 0xB4000014
#define PL_GPIO_EN2 0xB4000018
#define PL_GPIO_EN3 0xB400001C
#define FUNC_ENABLE_REG 0xB4000008

extern void enable_i2c_pads(void);
extern void enable_ssp_pads(void);
extern void enable_uart0_pads(void);
extern void enable_fsmc_pads(void);
extern void enable_mii_pads(void);

#endif /* _SPR_GENERIC_ */
