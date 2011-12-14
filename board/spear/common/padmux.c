/*
 * SPEAr platform specific gpio pads muxing source file
 *
 * Copyright (C) 2009 ST Microelectronics
 * Viresh Kumar<viresh.kumar@st.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <common.h>
#include <malloc.h>
#include <linux/err.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/padmux.h>

#define printk	printf

/*
 * struct pmx: pmx definition structure
 *
 * mode_reg: mode configurations
 * active_mode: pointer to current active mode
 */
struct pmx {
	struct pmx_reg mode_reg;
	struct pmx_mode *active_mode;
};

static struct pmx _pmx;
static struct pmx *pmx = &_pmx;

/**
 * pmx_mode_set - Enables an multiplexing mode
 * @mode - pointer to pmx mode
 *
 * It will set mode of operation in hardware.
 * Returns -ve on Err otherwise 0
 */
static int pmx_mode_set(struct pmx_mode *mode)
{
	u32 val, *address;

	if (!mode->name)
		return -EFAULT;

	pmx->active_mode = mode;

	address = (u32 *)pmx->mode_reg.address;
	if (address) {
		val = readl(address);
		val &= ~pmx->mode_reg.mask;
		val |= mode->value & pmx->mode_reg.mask;
		writel(val, address);
	}

	return 0;
}

void enable_dev_for_mode(struct pmx_dev_mode *mode)
{
	int k;
	struct pmx_mux_reg *mux_reg;
	u32 *address, val;

	/* enable peripheral */
	for (k = 0; k < mode->mux_reg_cnt; k++) {
		mux_reg = &mode->mux_regs[k];

		address = (u32 *)mux_reg->address;
		if (address) {
			val = readl(address);
			val &= ~mux_reg->mask;
			val |= mux_reg->value & mux_reg->mask;
			writel(val, address);
		}
	}
}

/**
 * pmx_devs_enable - Enables list of devices
 * @devs - pointer to pmx device array
 * @count - number of devices to enable
 *
 * It will enable pads for all required peripherals once and only once.
 * If peripheral is not supported by current mode then request is rejected.
 * Conflicts between peripherals are not handled and peripherals will be
 * enabled in the order they are present in pmx_dev array.
 * In case of conflicts last peripheral enabled will remain present.
 * Returns -ve on Err otherwise 0
 */
int pmx_devs_enable(struct pmx_dev **devs, u8 count)
{
	u32 i, j;
	bool found;

	if (!count)
		return -EINVAL;

	for (i = 0, j = 0; i < count; i++) {
		if (!devs[i]->name || !devs[i]->modes) {
			printk(KERN_ERR "padmux: dev name or modes is null\n");
			continue;
		}

		found = false;

		/* Enable dev for all struct modes which support current mode */
		for (j = 0; j < devs[i]->mode_count; j++) {
			if (!pmx->active_mode || (devs[i]->modes[j].ids &
						pmx->active_mode->id)) {
				found = true;
				enable_dev_for_mode(&devs[i]->modes[j]);
			}
		}

		if (found) {
			devs[i]->is_active = true;
			continue;
		}

		printk(KERN_ERR "%s device not available in %s mode\n",
				devs[i]->name, pmx->active_mode->name);
	}

	return 0;
}

/**
 * pmx_register - registers a platform requesting pad mux feature
 * @driver - pointer to driver structure containing driver specific parameters
 *
 * Also this must be called only once. This will allocate memory for pmx
 * structure, will call pmx_mode_set, will call pmx_devs_enable.
 * Returns -ve on Err otherwise 0
 */
int pmx_register(struct pmx_driver *driver)
{
	int ret = 0;

	if (!driver || !driver->devs)
		return -EFAULT;

	pmx->mode_reg.address = driver->mode_reg.address;
	pmx->mode_reg.mask = driver->mode_reg.mask;

	/* choose mode to enable */
	if (driver->mode) {
		ret = pmx_mode_set(driver->mode);
		if (ret)
			goto pmx_fail;
	}
	ret = pmx_devs_enable(driver->devs, driver->devs_count);
	if (ret)
		goto pmx_fail;

	return 0;

pmx_fail:
	return ret;
}
