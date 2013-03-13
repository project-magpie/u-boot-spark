/*
 * SPEAr platform specific gpio pads muxing file
 *
 * Copyright (C) 2009 ST Microelectronics
 * Viresh Kumar<viresh.kumar@st.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __PLAT_PADMUX_H
#define __PLAT_PADMUX_H

#include <linux/types.h>

typedef enum {
	false,
	true,
} bool;

/*
 * struct pmx_reg: configuration structure for mode reg and mux reg
 *
 * address: physical address of mode reg
 * mask: mask of mode reg
 */
struct pmx_reg {
	u32 address;
	u32 mask;
};

/*
 * struct pmx_mux_reg: configuration structure every group of modes of a device
 *
 * address: physical address of multiplexing register
 * mask: mask for supported mode
 * value: value to be written
 */
struct pmx_mux_reg {
	u32 address;
	u32 mask;
	u32 value;
};

/*
 * struct pmx_dev_mode: configuration structure every group of modes of a device
 *
 * ids: all modes for this configuration
 * mux_regs: array of mux registers, masks and values to enable the device in
 *		this group of modes
 * mux_reg_cnt: count of mux_regs elements
 */
struct pmx_dev_mode {
	u32 ids;
	struct pmx_mux_reg *mux_regs;
	u8 mux_reg_cnt;
};

/*
 * struct pmx_mode: mode definition structure
 *
 * name: mode name
 * value: mode value
 */
struct pmx_mode {
	char *name;
	u32 id;
	u32 value;
};

/*
 * struct pmx_dev: device definition structure
 *
 * name: device name
 * modes: device configuration array for different modes supported
 * mode_count: size of modes array
 * is_active: is peripheral active/enabled
 */
struct pmx_dev {
	char *name;
	struct pmx_dev_mode *modes;
	u8 mode_count;
	bool is_active;
};

/*
 * struct pmx_driver: driver definition structure
 *
 * mode: mode to be set
 * devs: array of pointer to pmx devices
 * devs_count: ARRAY_SIZE of devs
 * mode_reg: structure of mode config register
 */
struct pmx_driver {
	struct pmx_mode *mode;
	struct pmx_dev **devs;
	u8 devs_count;
	struct pmx_reg mode_reg;
};

/* pmx functions */
int pmx_register(struct pmx_driver *driver);
int pmx_devs_enable(struct pmx_dev **devs, u8 count);

#endif /* __PLAT_PADMUX_H */
