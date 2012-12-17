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

#define MIN_1_8TH_CYCLE_PREAMBLE	6
#define MIN_NO_OF_1_8TH_CYCLE		2
#define MAX_NO_OF_1_8TH_CYCLE		6
static struct mpmc_regs *mpmc_p = (struct mpmc_regs *)CONFIG_SYS_MPMC_BASE;

static void prog_rdlvl_gate_delay(u32 slice, u32 delay)
{
	u32 *rdlvl_gate_delay_reg = &mpmc_p->reg86;
	u32 shift = (slice % 2) ? 0 : 16;

	rdlvl_gate_delay_reg += (slice + 1) >> 1;
	writel_field(delay << shift, 0xFFFF << shift, rdlvl_gate_delay_reg);
}

#if defined(CONFIG_DDR2)
void ddr2_lvl_gatetrn(void)
{
	u32 *phy_ctrl_reg0 = &mpmc_p->reg124;
	u32 *phy_ctrl_reg6 = &mpmc_p->reg155;
	u32 final_gate_counter[DATA_SLICE_MAX];
	u32 slice;

	u32 phy_ctrl_reg6_dqs_assertval[] = {
		(0x0 << 0) | (0x0 << 20),
		(0x0 << 0) | (0x0 << 20),
		(0x8 << 0) | (0x8 << 20),
		(0x9 << 0) | (0x9 << 20),
		(0xa << 0) | (0xa << 20),
		(0xb << 0) | (0xb << 20)
	};
	u32 phy_ctrl_reg6_dqs_deassertval[] = {
		(0x00 << 5) | (0x00 << 25),
		(0x10 << 5) | (0x10 << 25),
		(0x11 << 5) | (0x11 << 25),
		(0x12 << 5) | (0x12 << 25),
		(0x13 << 5) | (0x13 << 25),
		(0x14 << 5) | (0x14 << 25)
	};

	int RDLVL_GATE_DELAY_VALUE[5] = {32, 32, 32, 32, 32};

	RDLVL_GATE_DELAY_VALUE[0] = RDLVL_GATE_DELAY_VALUE_0;
	RDLVL_GATE_DELAY_VALUE[1] = RDLVL_GATE_DELAY_VALUE_1;
	RDLVL_GATE_DELAY_VALUE[2] = RDLVL_GATE_DELAY_VALUE_2;
	RDLVL_GATE_DELAY_VALUE[3] = RDLVL_GATE_DELAY_VALUE_3;
	RDLVL_GATE_DELAY_VALUE[4] = RDLVL_GATE_DELAY_VALUE_4;

	for (slice = 0; slice < DATA_SLICE_MAX; slice++) {
		final_gate_counter[slice] = RDLVL_GATE_DELAY_VALUE[slice];
		writel_field((final_gate_counter[slice] / 8) << 20, 7 << 20,
				phy_ctrl_reg0 + slice);
		prog_rdlvl_gate_delay(slice, final_gate_counter[slice] % 8);
	}

	for (slice = 0; slice < DATA_SLICE_MAX; slice++) {
		writel_field(phy_ctrl_reg6_dqs_assertval[final_gate_counter[slice] / 8],
				(0x1F << 0) | (0x1F << 20), phy_ctrl_reg6 + slice);
		writel_field(phy_ctrl_reg6_dqs_deassertval[final_gate_counter[slice] / 8],
				(0x1F << 5) | (0x1F << 25), phy_ctrl_reg6 + slice);
	}
}
#elif defined(CONFIG_DDR3)
static void set_gate_parms(u32 gate_cnt, u32 slice)
{
	u32 *phy_ctrl_reg0 = &mpmc_p->reg124;

	writel_field((gate_cnt / 8) << 20, 7 << 20, phy_ctrl_reg0 + slice);
	prog_rdlvl_gate_delay(slice, gate_cnt % 8);

	swlvl_load();
	wait_op_done();
}

static void set_gate_parms_resp(u32 *gate_cnt_p, u32 slice, u32 loopon)
{
	do {
		(*gate_cnt_p)++;
		set_gate_parms(*gate_cnt_p, slice);
	} while (read_resp(slice) == loopon);
}

void ddr3_lvl_gatetrn(void)
{
	u32 *phy_ctrl_reg0 = &mpmc_p->reg124;
	u32 *phy_ctrl_reg6 = &mpmc_p->reg155;
	u32 *phy_ctrl_reg7 = &mpmc_p->reg160;
	u32 final_gate_counter[DATA_SLICE_MAX];
	u32 slice, gate_counter, resp;
	u32 prelim_preamble_start = 8, prelim_rise_edge = 8, prelim_fall_edge;
	u32 prelim_dqs_low_start, prelim_dqs_low_end;

	u32 phy_ctrl_reg6_dqs_assertval[] = {
		(0x0 << 0) | (0x0 << 20),
		(0x0 << 0) | (0x0 << 20),
		(0x8 << 0) | (0x8 << 20),
		(0x9 << 0) | (0x9 << 20),
		(0xa << 0) | (0xa << 20),
		(0xb << 0) | (0xb << 20)
	};
	u32 phy_ctrl_reg6_dqs_deassertval[] = {
		(0x00 << 5) | (0x00 << 25),
		(0x10 << 5) | (0x10 << 25),
		(0x11 << 5) | (0x11 << 25),
		(0x12 << 5) | (0x12 << 25),
		(0x13 << 5) | (0x13 << 25),
		(0x14 << 5) | (0x14 << 25)
	};

	writel_field(TDFI_RDLVL_RR, TDFI_RDLVL_RR_MSK, &mpmc_p->reg64);

	for (slice = 0; slice < DATA_SLICE_MAX; slice++) {
		writel_field(0 << 0, 7 << 0, phy_ctrl_reg6 + slice);
		writel_field(0 << 3, 3 << 3, phy_ctrl_reg6 + slice);
		writel_field(3 << 5, 7 << 5, phy_ctrl_reg6 + slice);
		writel_field(2 << 8, 3 << 8, phy_ctrl_reg6 + slice);

		writel_field(1 << 0, 7 << 0, phy_ctrl_reg7 + slice);
	}

	set_swlvl_mode(GATE_LVL);
	swlvl_start();
	wait_op_done();

	for (slice = 0; slice < DATA_SLICE_MAX; slice++) {
		gate_counter = 7;
		set_gate_parms(++gate_counter, slice);

		resp = read_resp(slice);
		if (resp == 0) {
			prelim_preamble_start = gate_counter;
			set_gate_parms_resp(&gate_counter, slice, 0);
			prelim_rise_edge = gate_counter;
		} else if (resp == 1) {
			set_gate_parms_resp(&gate_counter, slice, 1);
			prelim_preamble_start = gate_counter;

			set_gate_parms_resp(&gate_counter, slice, 0);
			prelim_rise_edge = gate_counter;
		}

step_14:
		while (prelim_rise_edge - prelim_preamble_start < MIN_1_8TH_CYCLE_PREAMBLE) {
			set_gate_parms_resp(&gate_counter, slice, 1);
			prelim_preamble_start = gate_counter;

			set_gate_parms_resp(&gate_counter, slice, 0);
			prelim_rise_edge = gate_counter;
		}

		if (prelim_rise_edge - prelim_preamble_start >= MIN_1_8TH_CYCLE_PREAMBLE) {
			set_gate_parms_resp(&gate_counter, slice, 1);
			prelim_fall_edge = gate_counter;
		}

		if ((prelim_fall_edge - prelim_rise_edge < MIN_NO_OF_1_8TH_CYCLE) ||
			(prelim_fall_edge - prelim_rise_edge > MAX_NO_OF_1_8TH_CYCLE)) {

			prelim_preamble_start = gate_counter;

			set_gate_parms_resp(&gate_counter, slice, 0);
			prelim_rise_edge = gate_counter;

			goto step_14;
		}

		if ((prelim_fall_edge - prelim_rise_edge >= MIN_NO_OF_1_8TH_CYCLE) &&
			(prelim_fall_edge - prelim_rise_edge <= MAX_NO_OF_1_8TH_CYCLE)) {

			prelim_dqs_low_start = gate_counter;

			set_gate_parms_resp(&gate_counter, slice, 0);
			prelim_dqs_low_end = gate_counter;
		}

		if ((prelim_dqs_low_end - prelim_dqs_low_start < MIN_NO_OF_1_8TH_CYCLE) ||
			(prelim_dqs_low_end - prelim_dqs_low_start > MAX_NO_OF_1_8TH_CYCLE)) {

			set_gate_parms_resp(&gate_counter, slice, 1);
			prelim_preamble_start = gate_counter;

			set_gate_parms_resp(&gate_counter, slice, 0);
			prelim_rise_edge = gate_counter;

			goto step_14;
		}

		if ((prelim_dqs_low_end - prelim_dqs_low_start >= MIN_NO_OF_1_8TH_CYCLE) &&
			(prelim_dqs_low_end - prelim_dqs_low_start <= MAX_NO_OF_1_8TH_CYCLE)) {

			final_gate_counter[slice] = prelim_rise_edge;
		}

		final_gate_counter[slice] -= 7;
		writel_field((final_gate_counter[slice] / 8) << 20, 7 << 20, phy_ctrl_reg0 + slice);
		prog_rdlvl_gate_delay(slice, final_gate_counter[slice] % 8);

		swlvl_load();
		wait_op_done();
	}

	swlvl_exit();
	wait_op_done();

	for (slice = 0; slice < DATA_SLICE_MAX; slice++) {
		writel_field(phy_ctrl_reg6_dqs_assertval[final_gate_counter[slice] / 8],
				(0x1F << 0) | (0x1F << 20), phy_ctrl_reg6 + slice);
		writel_field(phy_ctrl_reg6_dqs_deassertval[final_gate_counter[slice] / 8],
				(0x1F << 5) | (0x1F << 25), phy_ctrl_reg6 + slice);
	}
}
#endif
