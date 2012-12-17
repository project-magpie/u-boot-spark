/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
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

#ifndef SPR13XX_MPMC_H
#define SPR13XX_MPMC_H

struct mpmc_regs {
	u32 reg0;
	u32 reg1;
	u32 reg2;
	u32 reg3;
	u32 reg4;
	u32 reg5;
	u32 reg6;
	u32 reg7;
	u32 reg8;
	u32 reg9;
	u32 reg10;
	u32 reg11;
	u32 reg12;
	u32 reg13;
	u32 reg14;
	u32 reg15;
	u32 reg16;
	u32 reg17;
	u32 reg18;
	u32 reg19;
	u32 reg20;
	u32 reg21;
	u32 reg22;
	u32 reg23;
	u32 reg24;
	u32 reg25;
	u32 reg26;
	u32 reg27;
	u32 reg28;
	u32 reg29;
	u32 reg30;
	u32 reg31;
	u32 reg32;
	u32 reg33;
	u32 reg34;
	u32 reg35;
	u32 reg36;
	u32 reg37;
	u32 reg38;
	u32 reg39;
	u32 reg40;
	u32 reg41;
	u32 reg42;
	u32 reg43;
	u32 reg44;
	u32 reg45;
	u32 reg46;
	u32 reg47;
	u32 reg48;
	u32 reg49;
	u32 reg50;
	u32 reg51;
	u32 reg52;
	u32 reg53;
	u32 reg54;
	u32 reg55;
	u32 reg56;
	u32 reg57;
	u32 reg58;
	u32 reg59;
	u32 reg60;
	u32 reg61;
	u32 reg62;
	u32 reg63;
	u32 reg64;
	u32 reg65;
	u32 reg66;
	u32 reg67;
	u32 reg68;
	u32 reg69;
	u32 reg70;
	u32 reg71;
	u32 reg72;
	u32 reg73;
	u32 reg74;
	u32 reg75;
	u32 reg76;
	u32 reg77;
	u32 reg78;
	u32 reg79;
	u32 reg80;
	u32 reg81;
	u32 reg82;
	u32 reg83;
	u32 reg84;
	u32 reg85;
	u32 reg86;
	u32 reg87;
	u32 reg88;
	u32 reg89;
	u32 reg90;
	u32 reg91;
	u32 reg92;
	u32 reg93;
	u32 reg94;
	u32 reg95;
	u32 reg96;
	u32 reg97;
	u32 reg98;
	u32 reg99;
	u32 reg100;
	u32 reg101;
	u32 reg102;
	u32 reg103;
	u32 reg104;
	u32 reg105;
	u32 reg106;
	u32 reg107;
	u32 reg108;
	u32 reg109;
	u32 reg110;
	u32 reg111;
	u32 reg112;
	u32 reg113;
	u32 reg114;
	u32 reg115;
	u32 reg116;
	u32 reg117;
	u32 reg118;
	u32 reg119;
	u32 reg120;
	u32 reg121;
	u32 reg122;
	u32 reg123;
	u32 reg124;
	u32 reg125;
	u32 reg126;
	u32 reg127;
	u32 reg128;
	u32 reg129;
	u32 reg130;
	u32 reg131;
	u32 reg132;
	u32 reg133;
	u32 reg134;
	u32 reg135;
	u32 reg136;
	u32 reg137;
	u32 reg138;
	u32 reg139;
	u32 reg140;
	u32 reg141;
	u32 reg142;
	u32 reg143;
	u32 reg144;
	u32 reg145;
	u32 reg146;
	u32 reg147;
	u32 reg148;
	u32 reg149;
	u32 reg150;
	u32 reg151;
	u32 reg152;
	u32 reg153;
	u32 reg154;
	u32 reg155;
	u32 reg156;
	u32 reg157;
	u32 reg158;
	u32 reg159;
	u32 reg160;
	u32 reg161;
	u32 reg162;
	u32 reg163;
	u32 reg164;
	u32 reg165;
	u32 reg166;
	u32 reg167;
	u32 reg168;
	u32 reg169;
	u32 reg170;
	u32 reg171;
	u32 reg172;
	u32 reg173;
	u32 reg174;
	u32 reg175;
	u32 reg176;
	u32 reg177;
	u32 reg178;
	u32 reg179;
	u32 reg180;
	u32 reg181;
	u32 reg182;
	u32 reg183;
	u32 reg184;
	u32 reg185;
	u32 reg186;
	u32 reg187;
	u32 reg188;
	u32 reg189;
	u32 reg190;
	u32 reg191;
	u32 reg192;
	u32 reg193;
	u32 reg194;
	u32 reg195;
	u32 reg196;
	u32 reg197;
	u32 reg198;
	u32 reg199;
	u32 reg200;
	u32 reg201;
	u32 reg202;
	u32 reg203;
	u32 reg204;
	u32 reg205;
	u32 reg206;
	u32 reg207;
	u32 reg208;
	u32 reg209;
};

#define PHY_CTRL_DELAY			200

#ifdef CONFIG_DDR_ECC_ENABLE
#define DATA_SLICE_MAX			5
#else
#define DATA_SLICE_MAX			4
#endif

#define WRLVL_DELAY_MAX			8
#define RDLVL_DELAY_VALS		8
#define RDLVL_DELAY_INIT		4

#define MPMC_REG18_ECC_MASK		(3 << 8)
#define MPMC_REG18_NO_ECC		(0 << 8)
#define MPMC_REG18_ECC_REPORT_ON	(1 << 8)
#define MPMC_REG18_ECC_CORR_ON		(3 << 8)

/* DDR2 settings*/
#define WRLVL_BASE_OFFSET_REG_VALUE	0

#define WRLVL_DELAY_VALUE_0	2
#define WRLVL_DELAY_VALUE_1	2
#define WRLVL_DELAY_VALUE_2	2
#define WRLVL_DELAY_VALUE_3	2
#define WRLVL_DELAY_VALUE_4	2

#define RDLVL_GATE_DELAY_VALUE_0	31
#define RDLVL_GATE_DELAY_VALUE_1	26
#define RDLVL_GATE_DELAY_VALUE_2	26
#define RDLVL_GATE_DELAY_VALUE_3	31
#define RDLVL_GATE_DELAY_VALUE_4	31

#define RDLVL_DELAY_VALUE_0	8
#define RDLVL_DELAY_VALUE_1	8
#define RDLVL_DELAY_VALUE_2	8
#define RDLVL_DELAY_VALUE_3	8
#define RDLVL_DELAY_VALUE_4	8

/* mpmc register definitions */
#define SWLVL_MODE_MSK			(3 << 8)
#define NO_LVL				(0 << 8)
#define WRITE_LVL			(1 << 8)
#define READ_LVL			(2 << 8)
#define GATE_LVL			(3 << 8)
#define SWLVL_START			(1 << 8)
#define SWLVL_LOAD			(1 << 24)
#define SWLVL_EXIT			(1 << 16)
#define SWLVL_OP_DONE			(1 << 0)
#define WRLVL_REG_EN			(1 << 8)
#define RDLVL_GATE_REG_EN		(1 << 0)
#define RDLVL_REG_EN			(1 << 24)

#define TDFI_RDLVL_RR			(0x10 << 16)
#define TDFI_RDLVL_RR_MSK		(0x3FF << 16)
#define DQSGATE_ASSERT_MSK		(0x1F << 0)
#define DQSGATE_DEASSERT_MSK		(0x1F << 5)
#define DQSOE_ASSERT_MSK		(0x1F << 10)
#define DQSOE_DEASSERT_MSK		(0x1F << 15)
#define REG1_ASSERT_MSK			(0x1F << 20)
#define REG1_DEASSERT_MSK		(0x1F << 25)
#define REG5_ASSERT_MSK			(0x1F << 0)
#define REG5_DEASSERT_MSK		(0x1F << 5)
#define REG6_DQS_ASSERT_MSK		(0x1F << 10)
#define REG6_DQS_DEASSERT_MSK		(0x1F << 15)
#define REG7_ASSERT_MSK			(0x1F << 18)
#define REG7_DEASSERT_MSK		(0x1F << 23)

/* externs */
extern void writel_field(u32, u32, u32 *);
extern u32 readl_field(u32, u32, u32 *);
extern void swlvl_load(void);
extern void wait_op_done(void);
extern u32 read_resp(u32);
extern void set_swlvl_mode(u32);
extern void swlvl_start(void);
extern void swlvl_exit(void);

extern void spear13xx_mpmc_init(u32 *, u32);
extern void spear13xx_ddr_level(void);

extern u32 mpmc_conf_vals[];

#if defined(CONFIG_DDR3)
extern void ddr3_lvl_write(void);
extern void ddr3_lvl_gatetrn(void);
extern void ddr3_lvl_read(void);
#elif defined(CONFIG_DDR2)
extern void ddr2_lvl_write(void);
extern void ddr2_lvl_gatetrn(void);
extern void ddr2_lvl_read(void);
#endif

#endif
