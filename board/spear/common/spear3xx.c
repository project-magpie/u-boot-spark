/*
 * SPEAr3xx machine source file
 *
 * Copyright (C) 2009 ST Microelectronics
 * Viresh Kumar<viresh.kumar@st.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <common.h>
#include <linux/types.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/spr_misc.h>
#include <asm/arch/spr_generic.h>
#include <asm/arch/spear320.h>
#include <asm/arch/padmux.h>

/* pad multiplexing support */
/* Pad multiplexing for i2c device */
static struct pmx_mux_reg pmx_i2c_mux[] = {
	{
		.mask = PMX_I2C_MASK,
		.value = PMX_I2C_MASK,
	},
};

static struct pmx_dev_mode pmx_i2c_modes[] = {
	{
		.ids = 0xffffffff,
		.mux_regs = pmx_i2c_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_i2c_mux),
	},
};

struct pmx_dev spear3xx_pmx_i2c = {
	.name = "i2c",
	.modes = pmx_i2c_modes,
	.mode_count = ARRAY_SIZE(pmx_i2c_modes),
};

/* Pad multiplexing for ssp device */
static struct pmx_mux_reg pmx_ssp_cs_mux[] = {
	{
		.mask = PMX_SSP_CS_MASK,
		.value = PMX_SSP_CS_MASK,
	},
};

static struct pmx_dev_mode pmx_ssp_cs_modes[] = {
	{
		.ids = 0xffffffff,
		.mux_regs = pmx_ssp_cs_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_ssp_cs_mux),
	},
};

struct pmx_dev spear3xx_pmx_ssp_cs = {
	.name = "ssp_chip_selects",
	.modes = pmx_ssp_cs_modes,
	.mode_count = ARRAY_SIZE(pmx_ssp_cs_modes),
};

/* Pad multiplexing for ssp device */
static struct pmx_mux_reg pmx_ssp_mux[] = {
	{
		.mask = PMX_SSP_MASK,
		.value = PMX_SSP_MASK,
	},
};

static struct pmx_dev_mode pmx_ssp_modes[] = {
	{
		.ids = 0xffffffff,
		.mux_regs = pmx_ssp_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_ssp_mux),
	},
};

struct pmx_dev spear3xx_pmx_ssp = {
	.name = "ssp",
	.modes = pmx_ssp_modes,
	.mode_count = ARRAY_SIZE(pmx_ssp_modes),
};

/* Pad multiplexing for mii device */
static struct pmx_mux_reg pmx_mii_mux[] = {
	{
		.mask = PMX_MII_MASK,
		.value = PMX_MII_MASK,
	},
};

static struct pmx_dev_mode pmx_mii_modes[] = {
	{
		.ids = 0xffffffff,
		.mux_regs = pmx_mii_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_mii_mux),
	},
};

struct pmx_dev spear3xx_pmx_mii = {
	.name = "mii",
	.modes = pmx_mii_modes,
	.mode_count = ARRAY_SIZE(pmx_mii_modes),
};

/* Pad multiplexing for uart0 device */
static struct pmx_mux_reg pmx_uart0_mux[] = {
	{
		.mask = PMX_UART0_MASK,
		.value = PMX_UART0_MASK,
	},
};

static struct pmx_dev_mode pmx_uart0_modes[] = {
	{
		.ids = 0xffffffff,
		.mux_regs = pmx_uart0_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_uart0_mux),
	},
};

struct pmx_dev spear3xx_pmx_uart0 = {
	.name = "uart0",
	.modes = pmx_uart0_modes,
	.mode_count = ARRAY_SIZE(pmx_uart0_modes),
};

/*
 * This fixes addresses of all pmx devices for different machines, if address is
 * passed as zero. If a valid address is passed, then it will not be
 * overwritten.
 */
void spear3xx_pmx_init_addr(struct pmx_driver *driver, unsigned int addr)
{
	int i;
	for (i = 0; i < driver->devs_count; i++) {
		int j;
		struct pmx_dev *pdev = driver->devs[i];

		for (j = 0; j < pdev->mode_count; j++) {
			int k;
			struct pmx_dev_mode *mode = &pdev->modes[j];

			for (k = 0; k < mode->mux_reg_cnt; k++) {
				if (!mode->mux_regs[k].address)
					mode->mux_regs[k].address = addr;
			}
		}
	}
}
