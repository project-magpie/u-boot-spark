/*
 *  (C) Copyright 2009
 *  Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
 *
 *  See file CREDITS for list of people who contributed to this
 *  project.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA 02111-1307 USA
 */

#include <common.h>
#include <linux/mtd/st_smi.h>
#include <linux/mtd/fsmc_nand.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <asm/arch/spr_generic.h>
#include <asm/arch/spr_misc.h>


DECLARE_GLOBAL_DATA_PTR;

static void enable_pad(u8 pad_num)
{
	u32 *func_enb_0 = (u32 *)CONFIG_SPEAR_FUNCENB0;
	u32 *func_enb_4 = (u32 *)CONFIG_SPEAR_FUNCENB4;
	u32 *reg;

	if (pad_num <= 127)
		reg = func_enb_0 + (pad_num / 32);
	else if ((pad_num >= 128) && (pad_num <= 255)) {
		pad_num -= 128;
		reg = func_enb_4 + (pad_num / 32);
	}
		writel(readl(reg) | (1 << (pad_num % 32)), reg);
}

static void enable_alternate_pad(u8 bit_num)
{
	u32 *shrd_pad_enb_0 = (u32 *)CONFIG_SPEAR_SHAREDIPENB0;
	u32 *reg;

	reg = shrd_pad_enb_0 + (bit_num / 32);
	writel(readl(reg) | (1 << (bit_num % 32)), reg);
}

void enable_pads(u8 *pads, u32 count)
{
	int i;

	for (i = 0; i < count; i++)
		enable_pad(pads[i] + 1);
}

void enable_alternate_pads(u8 *bits, u32 count)
{
	int i;

	for (i = 0; i < count; i++)
		enable_alternate_pad(bits[i]);
}

void enable_serial_pad(void)
{
	u8 pads[] = {86, 87};

	enable_pads(pads, sizeof(pads));
}

void enable_mmc_pad(void)
{
	u8 pads[] = {213, 214, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227,
		228, 229, 230, 231, 232, 237};

	enable_pads(pads, sizeof(pads));
}

void enable_spi_pad(void)
{
	u8 spi_pads[] = {24, 80, 81, 82, 83};
	u8 spi_alt_pads[] = {5};

	enable_pads(spi_pads, sizeof(spi_pads));
	enable_alternate_pads(spi_alt_pads, sizeof(spi_alt_pads));
}

void enable_smi_pad(void)
{
	u8 pads[] = {76, 77, 78, 79, 84};
	enable_pads(pads, sizeof(pads));
}

void enable_fsmc_pad(int width)
{
	u8 pads_8[] = {8, 9, 10, 192, 193, 194, 195, 196, 197, 198, 199, 200,
		201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 215,
		216, 217, 233, 234, 235, 236, 238, 239, 240, 241, 242, 243, 244,
		245, 246, 247, 248, 249};

	u8 pads_16[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 192, 193, 194, 195,
		196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
		209, 210, 211, 212, 215, 216, 217, 233, 234, 235, 236, 238, 239,
		240, 241, 242, 243, 244, 245, 246, 247, 248, 249};

	if (width == 16)
		enable_pads(pads_16, sizeof(pads_16));
	else if (width == 8)
		enable_pads(pads_8, sizeof(pads_8));
}

void enable_usbd_pad(void)
{
	enable_pads(NULL, 0);
}

void enable_gmi_pad(void)
{
	u8 gmi_pads[] = {104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
		115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
		128, 129, 130, 131};
	enable_pads(gmi_pads, sizeof(gmi_pads));
}

void enable_i2c_pad(void)
{
	u8 i2c_pads[] = {18, 23, 133, 134};
	enable_pads(i2c_pads, sizeof(i2c_pads));
}

/* Enable all pad multiplex device */
void enable_pad_mux(void)
{
#if defined(CONFIG_PL011_SERIAL)
	enable_serial_pad();
#endif

#if defined(CONFIG_SPEAR_MMC)
	enable_mmc_pad();
#endif

#if defined(CONFIG_ST_SMI)
	enable_smi_pad();
#endif

#if defined(CONFIG_NAND_FSMC)
#if defined(CONFIG_SYS_FSMC_NAND_16BIT)
	enable_fsmc_pad(16);
#elif defined(CONFIG_SYS_FSMC_NAND_8BIT)
	enable_fsmc_pad(8);
#endif
#endif

#if defined(CONFIG_USB_EHCI_SPEAR)
	enable_usbd_pad();
#endif

#if defined(CONFIG_DESIGNWARE_ETH)
	enable_gmi_pad();
#endif

#if defined(CONFIG_DW_I2C)
	enable_i2c_pad();
#endif

#if defined(CONFIG_PL022_SPI)
	enable_spi_pad();
#endif
}
