/*
 * Realtek RTL8367 SMI interface driver defines
 *
 * Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2013 STMicroelectronics.
 *	Sean McGoogan <Sean.McGoogan@st.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RTL8367_SMI_H
#define _RTL8367_SMI_H


struct rtl8367_mib_counter {
	unsigned char	base;
	unsigned char	offset;
	unsigned char	length;
	const char	*name;
};

struct rtl8367_smi {
	unsigned short		gpio_sda;
	unsigned short		gpio_sck;
	unsigned int		clk_delay;	/* nsecs */
	u8			cmd_read;
	u8			cmd_write;
	u8			fw_loaded : 1;	/* boolean */
};


extern int rtl8367_smi_init(struct rtl8367_smi *smi);

extern int rtl8367_smi_init_fw(struct rtl8367_smi * const smi);

extern int rtl8367_smi_write_reg(struct rtl8367_smi *smi, u32 addr, u32 data);
extern int rtl8367_smi_write_reg_noack(struct rtl8367_smi *smi, u32 addr, u32 data);
extern int rtl8367_smi_read_reg(struct rtl8367_smi *smi, u32 addr, u32 * data);
extern int rtl8367_smi_rmwr(struct rtl8367_smi *smi, u32 addr, u32 mask, u32 data);

extern int rtl8367_smi_phy_read_reg(
	struct rtl8367_smi * const smi,
	const unsigned int phy_addr,
	const unsigned int phy_reg,
	unsigned int * const data);

extern int rtl8367_reset_mib_counters(
	struct rtl8367_smi * const smi);
extern int rtl8367_get_mib_counter(
	struct rtl8367_smi * const smi,
	const int counter,
	const int port,
	unsigned long long * const val);
extern void rtl8367_dump_mib_counters(
	struct rtl8367_smi * const smi);


#endif /*  _RTL8367_SMI_H */
