/*
 * (C) Copyright 2012
 * Amit Virdi, ST Micoelectronics, amit.virdi@st.com.
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

#include <common.h>
#include <linux/mtd/st_smi.h>
#include <linux/mtd/fsmc_nand.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <asm/arch/spr_generic.h>
#include <asm/arch/spr_misc.h>

DECLARE_GLOBAL_DATA_PTR;

struct pad_dir {
	u32 dirsel0;
	u32 dirsel1;
	u32 dirsel2;
};

struct sh_ip_sel {
	u32 funcenb0;
	u32 funcenb1;
	u32 funcenb2;
};

static void enable_pads(const struct sh_ip_sel *sh_ip_sel,
		const struct pad_dir *pad_dir)
{
	u32 *func_enb_0 = (u32 *)CONFIG_SPEAR_FUNCENB0;
	u32 *func_enb_1 = (u32 *)CONFIG_SPEAR_FUNCENB1;
	u32 *func_enb_2 = (u32 *)CONFIG_SPEAR_FUNCENB2;

	u32 *dir_sel_0 = (u32 *)CONFIG_SPEAR_DIRSEL0;
	u32 *dir_sel_1 = (u32 *)CONFIG_SPEAR_DIRSEL1;
	u32 *dir_sel_2 = (u32 *)CONFIG_SPEAR_DIRSEL2;

	if (sh_ip_sel) {
		writel(sh_ip_sel->funcenb0, func_enb_0);
		writel(sh_ip_sel->funcenb1, func_enb_1);
		writel(sh_ip_sel->funcenb2, func_enb_2);
	}

	if (pad_dir->dirsel0)
		writel(readl(dir_sel_0) | pad_dir->dirsel0, dir_sel_0);
	if (pad_dir->dirsel1)
		writel(readl(dir_sel_1) | pad_dir->dirsel1, dir_sel_1);
	if (pad_dir->dirsel2)
		writel(readl(dir_sel_2) | pad_dir->dirsel2, dir_sel_2);
}

enum ips {
	SMI = 1,
	NAND8,
	NAND16,
	PNOR8,
	PNOR16,
	PNOR32,
	USBD,
	UART,
	I2C,
	GMAC,
	SDMMC,
};

static void enable_pads_for_ip(int ip)
{
	switch (ip) {
	case SMI:
		{
			const struct pad_dir pad_dir_smi = {
				.dirsel0 = 0x00010000,
				.dirsel1 = 0x00000000,
				.dirsel2 = 0x00000000,
			};

			enable_pads(NULL, &pad_dir_smi);
			break;
		}
	case NAND8:
		{
			const struct pad_dir pad_dir_nand8 = {
				.dirsel0 = 0x00020000,
				.dirsel1 = 0x00000000,
				.dirsel2 = 0x00000000,
			};

			enable_pads(NULL, &pad_dir_nand8);
			break;
		}
	case NAND16:
		{
			const struct pad_dir pad_dir_nand16 = {
				.dirsel0 = 0x00020000,
				.dirsel1 = 0x00020000,
				.dirsel2 = 0x00000000,
			};

			enable_pads(NULL, &pad_dir_nand16);
			break;
		}
	case PNOR8:
		{
			const struct pad_dir pad_dir_pnor8 = {
				.dirsel0 = 0x27020000,
				.dirsel1 = 0x00000000,
				.dirsel2 = 0x00000000,
			};

			enable_pads(NULL, &pad_dir_pnor8);
			break;
		}
	case PNOR16:
		{
			const struct pad_dir pad_dir_pnor16 = {
				.dirsel0 = 0x27020000,
				.dirsel1 = 0x00020000,
				.dirsel2 = 0x00000000,
			};

			enable_pads(NULL, &pad_dir_pnor16);
			break;
		}
	case PNOR32:
		{
			const struct pad_dir pad_dir_pnor32 = {
				.dirsel0 = 0x27020000,
				.dirsel1 = 0x00020000,
				.dirsel2 = 0x0f59ff00,
			};

			enable_pads(NULL, &pad_dir_pnor32);
			break;
		}
	case UART:
		{
			const struct pad_dir pad_dir_uart = {
				.dirsel0 = 0x00000002,
				.dirsel1 = 0x00000000,
				.dirsel2 = 0x00000000,
			};

			enable_pads(NULL, &pad_dir_uart);
			break;
		}
	case I2C:
		{
			const struct pad_dir pad_dir_i2c = {
				.dirsel0 = 0x00000004,
				.dirsel1 = 0x00000000,
				.dirsel2 = 0x00000000,
			};

			enable_pads(NULL, &pad_dir_i2c);
			break;
		}
	case GMAC:
		{
			const struct pad_dir pad_dir_gmii = {
				.dirsel0 = 0x007C0000,
				.dirsel1 = 0x00000000,
				.dirsel2 = 0x00000000,
			};

			enable_pads(NULL, &pad_dir_gmii);
			break;
		}
	case SDMMC:
		{
			const struct sh_ip_sel sh_ip_sel_sdmmc = {
				.funcenb0 = 0xffffffff,
				.funcenb1 = 0xfaffffff,
				.funcenb2 = 0xffffffff,
			};
			const struct pad_dir pad_dir_sdmmc = {
				.dirsel0 = 0x00000000,
				.dirsel1 = 0x88000000,
				.dirsel2 = 0x0c0388bf,
			};

			enable_pads(&sh_ip_sel_sdmmc, &pad_dir_sdmmc);
			break;
		}
	case USBD:
		{
			break;
		}
	default:
		{
			const struct sh_ip_sel sh_ip_sel_def = {
				.funcenb0 = 0xffffffff,
				.funcenb1 = 0xffffffff,
				.funcenb2 = 0xffffffff,
			};
			const struct pad_dir pad_dir_def = {
				.dirsel0 = 0x00000000,
				.dirsel1 = 0x00000000,
				.dirsel2 = 0x00000000,
			};

			enable_pads(&sh_ip_sel_def, &pad_dir_def);
			break;
		}
	}
}

/* Enable all pad multiplex device */
void enable_pad_mux(void)
{
#if defined(CONFIG_PL011_SERIAL)
	enable_pads_for_ip(UART);
#endif

#if defined(CONFIG_SPEAR_MMC)
	enable_pads_for_ip(SDMMC);
#endif

#if defined(CONFIG_ST_SMI)
	enable_pads_for_ip(SMI);
#endif

#if defined(CONFIG_NAND_FSMC)
#if defined(CONFIG_SYS_FSMC_NAND_16BIT)
	enable_pads_for_ip(NAND16);
#elif defined(CONFIG_SYS_FSMC_NAND_8BIT)
	enable_pads_for_ip(NAND8);
#endif
#endif

#if defined(CONFIG_USB_EHCI_SPEAR)
	enable_pads_for_ip(USBD);
#endif

#if defined(CONFIG_DESIGNWARE_ETH)
	enable_pads_for_ip(GMAC);
#endif

#if defined(CONFIG_DW_I2C)
	enable_pads_for_ip(I2C);
#endif
}
