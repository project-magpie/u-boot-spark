/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Microelectronics, vipin.kumar@st.com
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/mpmc.h>

static struct mpmc_regs *mpmc_p = (struct mpmc_regs *)CONFIG_SYS_MPMC_BASE;

void writel_field(u32 value, u32 mask, u32 *address)
{
	u32 reg;
	reg = readl(address);
	reg &= ~mask;
	reg |= value;
	writel(reg, address);
}

u32 readl_field(u32 shift, u32 mask, u32 *address)
{
	u32 reg = readl(address);
	return (reg & mask) >> shift;
}

void set_swlvl_mode(u32 lvlmode)
{
	switch (lvlmode) {
	case WRITE_LVL:
		writel_field(WRITE_LVL, SWLVL_MODE_MSK, &mpmc_p->reg20);
		break;
	case GATE_LVL:
		writel_field(GATE_LVL, SWLVL_MODE_MSK, &mpmc_p->reg20);
		break;
	case READ_LVL:
		writel_field(READ_LVL, SWLVL_MODE_MSK, &mpmc_p->reg20);
		break;
	}
}

void swlvl_start(void)
{
	writel(readl(&mpmc_p->reg13) | SWLVL_START, &mpmc_p->reg13);
}

void swlvl_load(void)
{
	writel(readl(&mpmc_p->reg12) | SWLVL_LOAD, &mpmc_p->reg12);
}

void swlvl_exit(void)
{
	writel(readl(&mpmc_p->reg12) | SWLVL_EXIT, &mpmc_p->reg12);
}

void wait_op_done(void)
{
	while (!(readl(&mpmc_p->reg13) & SWLVL_OP_DONE))
		;
}

u32 read_resp(u32 slice)
{
	u32 *resp_reg = &mpmc_p->reg57;

	if (slice < 2)
		return (readl(resp_reg) >> (slice * 8 + 16)) & 0x1;

	resp_reg = &mpmc_p->reg58;

	/* return (readl(resp_reg) >> ((slice - 2) * 8)) & 0xFF; */
	return (readl(resp_reg) >> ((slice - 2) * 8)) & 0x1;
}
