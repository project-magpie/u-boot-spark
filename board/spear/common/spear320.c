/*
 * SPEAr320 machine source file
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

/* modes */
#define AUTO_NET_SMII_MODE	(1 << 0)
#define AUTO_NET_MII_MODE	(1 << 1)
#define AUTO_EXP_MODE		(1 << 2)
#define SMALL_PRINTERS_MODE	(1 << 3)
#define ALL_LEGACY_MODES	0xF
#define SPEAR320S_EXTENDED_MODE	(1 << 4)

struct pmx_mode spear320_auto_net_smii_mode = {
	.id = AUTO_NET_SMII_MODE,
	.name = "Automation Networking SMII Mode",
	.value = AUTO_NET_SMII_MODE_VAL,
};

struct pmx_mode spear320_auto_net_mii_mode = {
	.id = AUTO_NET_MII_MODE,
	.name = "Automation Networking MII Mode",
	.value = AUTO_NET_MII_MODE_VAL,
};

struct pmx_mode spear320_auto_exp_mode = {
	.id = AUTO_EXP_MODE,
	.name = "Automation Expanded Mode",
	.value = AUTO_EXP_MODE_VAL,
};

struct pmx_mode spear320_small_printers_mode = {
	.id = SMALL_PRINTERS_MODE,
	.name = "Small Printers Mode",
	.value = SMALL_PRINTERS_MODE_VAL,
};

struct pmx_mode spear320s_extended_mode = {
	.id = SPEAR320S_EXTENDED_MODE,
	.name = "spear320s extended mode",
	.value = SPEAR320S_EXTENDED_MODE_VAL,
};

/* devices */
/* Pad multiplexing for EMI (Parallel NOR flash) device */
static struct pmx_mux_reg pmx_emi_mux1[] = {
	{
		.mask = PMX_TIMER_1_2_MASK | PMX_TIMER_3_4_MASK,
		.value = 0,
	},
};

static struct pmx_mux_reg pmx_emi_ext_mux[] = {
	{
		.address = SPEAR320S_IP_SEL_PAD_40_49_REG,
		.mask = SPEAR320S_PMX_PL_46_47_MASK |
			SPEAR320S_PMX_PL_48_49_MASK,
		.value = SPEAR320S_PMX_FSMC_EMI_PL_46_47_VAL |
			SPEAR320S_PMX_FSMC_EMI_PL_48_49_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_50_59_REG,
		.mask = SPEAR320S_PMX_PL_50_51_MASK |
			SPEAR320S_PMX_PL_52_53_MASK |
			SPEAR320S_PMX_PL_54_55_56_MASK |
			SPEAR320S_PMX_PL_58_59_MASK,
		.value = SPEAR320S_PMX_EMI_PL_50_51_VAL |
			SPEAR320S_PMX_EMI_PL_52_53_VAL |
			SPEAR320S_PMX_FSMC_EMI_PL_54_55_56_VAL |
			SPEAR320S_PMX_FSMC_EMI_PL_58_59_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_60_69_REG,
		.mask = SPEAR320S_PMX_PL_69_MASK,
		.value = SPEAR320S_PMX_EMI_PL_69_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_70_79_REG,
		.mask = SPEAR320S_PMX_PL_70_MASK | SPEAR320S_PMX_PL_71_72_MASK |
			SPEAR320S_PMX_PL_73_MASK | SPEAR320S_PMX_PL_74_MASK |
			SPEAR320S_PMX_PL_75_76_MASK |
			SPEAR320S_PMX_PL_77_78_79_MASK,
		.value = SPEAR320S_PMX_FSMC_EMI_PL_70_VAL |
			SPEAR320S_PMX_FSMC_EMI_PL_71_72_VAL |
			SPEAR320S_PMX_FSMC_EMI_PL_73_VAL |
			SPEAR320S_PMX_EMI_PL_74_VAL |
			SPEAR320S_PMX_EMI_PL_75_76_VAL |
			SPEAR320S_PMX_EMI_PL_77_78_79_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_80_89_REG,
		.mask = SPEAR320S_PMX_PL_80_TO_85_MASK |
			SPEAR320S_PMX_PL_86_87_MASK |
			SPEAR320S_PMX_PL_88_89_MASK,
		.value = SPEAR320S_PMX_EMI_PL_80_TO_85_VAL |
			SPEAR320S_PMX_EMI_PL_86_87_VAL |
			SPEAR320S_PMX_EMI_PL_88_89_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_90_99_REG,
		.mask = SPEAR320S_PMX_PL_90_91_MASK |
			SPEAR320S_PMX_PL_92_93_MASK |
			SPEAR320S_PMX_PL_94_95_MASK |
			SPEAR320S_PMX_PL_96_97_MASK,
		.value = SPEAR320S_PMX_EMI1_PL_90_91_VAL |
			SPEAR320S_PMX_EMI1_PL_92_93_VAL |
			SPEAR320S_PMX_EMI1_PL_94_95_VAL |
			SPEAR320S_PMX_EMI1_PL_96_97_VAL,
	}, {
		.address = SPEAR320S_EXT_CTRL_REG,
		.mask = SPEAR320S_EMI_FSMC_DYNAMIC_MUX_MASK,
		.value = SPEAR320S_EMI_FSMC_DYNAMIC_MUX_MASK,
	},
};

static struct pmx_dev_mode pmx_emi_modes[] = {
	{
		.ids = AUTO_EXP_MODE | SPEAR320S_EXTENDED_MODE,
		.mux_regs = pmx_emi_mux1,
		.mux_reg_cnt = ARRAY_SIZE(pmx_emi_mux1),
	}, {
		.ids = SPEAR320S_EXTENDED_MODE,
		.mux_regs = pmx_emi_ext_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_emi_ext_mux),
	},
};

struct pmx_dev spear320_pmx_emi = {
	.name = "emi",
	.modes = pmx_emi_modes,
	.mode_count = ARRAY_SIZE(pmx_emi_modes),
};

/* Pad multiplexing for FSMC (NAND flash) device */
static struct pmx_mux_reg pmx_fsmc_8bit_mux[] = {
	{
		.address = SPEAR320S_IP_SEL_PAD_50_59_REG,
		.mask = SPEAR320S_PMX_PL_52_53_MASK |
			SPEAR320S_PMX_PL_54_55_56_MASK |
			SPEAR320S_PMX_PL_57_MASK | SPEAR320S_PMX_PL_58_59_MASK,
		.value = SPEAR320S_PMX_FSMC_PL_52_53_VAL |
			SPEAR320S_PMX_FSMC_EMI_PL_54_55_56_VAL |
			SPEAR320S_PMX_FSMC_PL_57_VAL |
			SPEAR320S_PMX_FSMC_EMI_PL_58_59_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_60_69_REG,
		.mask = SPEAR320S_PMX_PL_60_MASK |
			SPEAR320S_PMX_PL_61_TO_64_MASK |
			SPEAR320S_PMX_PL_65_TO_68_MASK,
		.value = SPEAR320S_PMX_FSMC_PL_60_VAL |
			SPEAR320S_PMX_FSMC_PL_61_TO_64_VAL |
			SPEAR320S_PMX_FSMC_PL_65_TO_68_VAL,
	}, {
		.address = SPEAR320S_EXT_CTRL_REG,
		.mask = SPEAR320S_EMI_FSMC_DYNAMIC_MUX_MASK,
		.value = SPEAR320S_EMI_FSMC_DYNAMIC_MUX_MASK,
	},
};
static struct pmx_mux_reg pmx_fsmc_16bit_mux[] = {
	{
		.address = SPEAR320S_IP_SEL_PAD_40_49_REG,
		.mask = SPEAR320S_PMX_PL_46_47_MASK |
			SPEAR320S_PMX_PL_48_49_MASK,
		.value = SPEAR320S_PMX_FSMC_EMI_PL_46_47_VAL |
			SPEAR320S_PMX_FSMC_EMI_PL_48_49_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_70_79_REG,
		.mask = SPEAR320S_PMX_PL_70_MASK | SPEAR320S_PMX_PL_71_72_MASK |
			SPEAR320S_PMX_PL_73_MASK,
		.value = SPEAR320S_PMX_FSMC_EMI_PL_70_VAL |
			SPEAR320S_PMX_FSMC_EMI_PL_71_72_VAL |
			SPEAR320S_PMX_FSMC_EMI_PL_73_VAL,
	}
};

static struct pmx_dev_mode pmx_fsmc_8bit_modes[] = {
	{
		.ids = SPEAR320S_EXTENDED_MODE,
		.mux_regs = pmx_fsmc_8bit_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_fsmc_8bit_mux),
	},
};

static struct pmx_dev_mode pmx_fsmc_16bit_modes[] = {
	{
		.ids = SPEAR320S_EXTENDED_MODE,
		.mux_regs = pmx_fsmc_8bit_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_fsmc_8bit_mux),
	}, {
		.ids = SPEAR320S_EXTENDED_MODE,
		.mux_regs = pmx_fsmc_16bit_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_fsmc_16bit_mux),
	},
};

struct pmx_dev spear320s_pmx_fsmc[] = {
	{
		.name = "fsmc - 8bit",
		.modes = pmx_fsmc_8bit_modes,
		.mode_count = ARRAY_SIZE(pmx_fsmc_8bit_modes),
	}, {
		.name = "fsmc - 16bit",
		.modes = pmx_fsmc_16bit_modes,
		.mode_count = ARRAY_SIZE(pmx_fsmc_16bit_modes),
	},
};

/* Pad multiplexing for SDHCI device */
static struct pmx_mux_reg pmx_sdhci_mux[] = {
	{
		.mask = PMX_TIMER_1_2_MASK | PMX_TIMER_3_4_MASK,
		.value = 0,
	},
};

static struct pmx_mux_reg pmx_sdhci_ext_mux[] = {
	{
		.address = SPEAR320S_IP_SEL_PAD_40_49_REG,
		.mask = SPEAR320S_PMX_PL_43_MASK | SPEAR320S_PMX_PL_44_45_MASK |
			SPEAR320S_PMX_PL_46_47_MASK |
			SPEAR320S_PMX_PL_48_49_MASK,
		.value = SPEAR320S_PMX_SDHCI_PL_43_VAL |
			SPEAR320S_PMX_SDHCI_PL_44_45_VAL |
			SPEAR320S_PMX_SDHCI_PL_46_47_VAL |
			SPEAR320S_PMX_SDHCI_PL_48_49_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_50_59_REG,
		.mask = SPEAR320S_PMX_PL_50_MASK,
		.value = SPEAR320S_PMX_SDHCI_PL_50_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_90_99_REG,
		.mask = SPEAR320S_PMX_PL_99_MASK,
		.value = SPEAR320S_PMX_SDHCI_PL_99_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_MIX_PAD_REG,
		.mask = SPEAR320S_PMX_PL_100_101_MASK,
		.value = SPEAR320S_PMX_SDHCI_PL_100_101_VAL,
	},
};

static struct pmx_mux_reg pmx_sdhci_cd_12_mux[] = {
	{
		.address = SPEAR320S_IP_SEL_PAD_10_19_REG,
		.mask = SPEAR320S_PMX_PL_12_MASK,
		.value = SPEAR320S_PMX_SDHCI_CD_PL_12_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_MIX_PAD_REG,
		.mask = SPEAR320S_PMX_SDHCI_CD_PORT_SEL_MASK,
		.value = SPEAR320S_PMX_SDHCI_CD_PORT_12_VAL,
	}, {
		.mask = PMX_MII_MASK,
		.value = 0,
	},
};

static struct pmx_mux_reg pmx_sdhci_cd_51_mux[] = {
	{
		.address = SPEAR320S_IP_SEL_PAD_50_59_REG,
		.mask = SPEAR320S_PMX_PL_51_MASK,
		.value = SPEAR320S_PMX_SDHCI_CD_PL_51_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_MIX_PAD_REG,
		.mask = SPEAR320S_PMX_SDHCI_CD_PORT_SEL_MASK,
		.value = SPEAR320S_PMX_SDHCI_CD_PORT_51_VAL,
	},
};

#define pmx_sdhci_common_modes						\
	{								\
		.ids = AUTO_NET_SMII_MODE | AUTO_NET_MII_MODE |		\
			SMALL_PRINTERS_MODE | SPEAR320S_EXTENDED_MODE,	\
		.mux_regs = pmx_sdhci_mux,				\
		.mux_reg_cnt = ARRAY_SIZE(pmx_sdhci_mux),		\
	}, {								\
		.ids = SPEAR320S_EXTENDED_MODE,				\
		.mux_regs = pmx_sdhci_ext_mux,				\
		.mux_reg_cnt = ARRAY_SIZE(pmx_sdhci_ext_mux),		\
	}

static struct pmx_dev_mode pmx_sdhci_modes[][3] = {
	{
		/* select pin 12 for cd */
		pmx_sdhci_common_modes,
		{
			.ids = SPEAR320S_EXTENDED_MODE,
			.mux_regs = pmx_sdhci_cd_12_mux,
			.mux_reg_cnt = ARRAY_SIZE(pmx_sdhci_cd_12_mux),
		},
	}, {
		/* select pin 51 for cd */
		pmx_sdhci_common_modes,
		{
			.ids = SPEAR320S_EXTENDED_MODE,
			.mux_regs = pmx_sdhci_cd_51_mux,
			.mux_reg_cnt = ARRAY_SIZE(pmx_sdhci_cd_51_mux),
		},
	}
};

struct pmx_dev spear320_pmx_sdhci[] = {
	{
		.name = "sdhci, cd-12",
		.modes = pmx_sdhci_modes[0],
		.mode_count = ARRAY_SIZE(pmx_sdhci_modes[0]),
	}, {
		.name = "sdhci, cd-51",
		.modes = pmx_sdhci_modes[1],
		.mode_count = ARRAY_SIZE(pmx_sdhci_modes[1]),
	},
};

/* Pad multiplexing for UART1 device */
static struct pmx_mux_reg pmx_uart1_mux[] = {
	{
		.mask = PMX_GPIO_PIN0_MASK | PMX_GPIO_PIN1_MASK,
		.value = 0,
	},
};

static struct pmx_mux_reg pmx_uart1_ext_mux[] = {
	{
		.address = SPEAR320S_IP_SEL_PAD_20_29_REG,
		.mask = SPEAR320S_PMX_PL_28_29_MASK,
		.value = SPEAR320S_PMX_UART1_PL_28_29_VAL,
	},
};

static struct pmx_dev_mode pmx_uart1_modes[] = {
	{
		.ids = ALL_LEGACY_MODES | SPEAR320S_EXTENDED_MODE,
		.mux_regs = pmx_uart1_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_uart1_mux),
	}, {
		.ids = SPEAR320S_EXTENDED_MODE,
		.mux_regs = pmx_uart1_ext_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_uart1_ext_mux),
	},
};

struct pmx_dev spear320_pmx_uart1 = {
	.name = "uart1",
	.modes = pmx_uart1_modes,
	.mode_count = ARRAY_SIZE(pmx_uart1_modes),
};

/* Pad multiplexing for cadence mii2 as mii device */
static struct pmx_mux_reg pmx_mii2_mux[] = {
	{
		.address = SPEAR320S_IP_SEL_PAD_80_89_REG,
		.mask = SPEAR320S_PMX_PL_80_TO_85_MASK |
			SPEAR320S_PMX_PL_86_87_MASK |
			SPEAR320S_PMX_PL_88_89_MASK,
		.value = SPEAR320S_PMX_MII2_PL_80_TO_85_VAL |
			SPEAR320S_PMX_MII2_PL_86_87_VAL |
			SPEAR320S_PMX_MII2_PL_88_89_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_90_99_REG,
		.mask = SPEAR320S_PMX_PL_90_91_MASK |
			SPEAR320S_PMX_PL_92_93_MASK |
			SPEAR320S_PMX_PL_94_95_MASK |
			SPEAR320S_PMX_PL_96_97_MASK,
		.value = SPEAR320S_PMX_MII2_PL_90_91_VAL |
			SPEAR320S_PMX_MII2_PL_92_93_VAL |
			SPEAR320S_PMX_MII2_PL_94_95_VAL |
			SPEAR320S_PMX_MII2_PL_96_97_VAL,
	}, {
		.address = SPEAR320S_EXT_CTRL_REG,
		.mask = (SPEAR320S_MAC_MODE_MASK << SPEAR320S_MAC2_MODE_SHIFT) |
			(SPEAR320S_MAC_MODE_MASK << SPEAR320S_MAC1_MODE_SHIFT) |
			SPEAR320S_MII_MDIO_MASK,
		.value = (SPEAR320S_MAC_MODE_MII << SPEAR320S_MAC2_MODE_SHIFT) |
			(SPEAR320S_MAC_MODE_MII << SPEAR320S_MAC1_MODE_SHIFT) |
			SPEAR320S_MII_MDIO_81_VAL,
	},
};

static struct pmx_dev_mode pmx_mii2_modes[] = {
	{
		.ids = SPEAR320S_EXTENDED_MODE,
		.mux_regs = pmx_mii2_mux,
		.mux_reg_cnt = ARRAY_SIZE(pmx_mii2_mux),
	},
};

struct pmx_dev spear320s_pmx_mii2 = {
	.name = "mii2",
	.modes = pmx_mii2_modes,
	.mode_count = ARRAY_SIZE(pmx_mii2_modes),
};

/* Pad multiplexing for cadence mii 1_2 as smii or rmii device */
static struct pmx_mux_reg pmx_mii1_2_mux[] = {
	{
		.mask = PMX_MII_MASK,
		.value = 0,
	},
};

static struct pmx_mux_reg pmx_smii1_2_ext_mux[] = {
	{
		.address = SPEAR320S_IP_SEL_PAD_10_19_REG,
		.mask = SPEAR320S_PMX_PL_10_11_MASK,
		.value = SPEAR320S_PMX_SMII_PL_10_11_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_20_29_REG,
		.mask = SPEAR320S_PMX_PL_21_TO_27_MASK,
		.value = SPEAR320S_PMX_SMII_PL_21_TO_27_VAL,
	}, {
		.address = SPEAR320S_EXT_CTRL_REG,
		.mask = (SPEAR320S_MAC_MODE_MASK << SPEAR320S_MAC2_MODE_SHIFT) |
			(SPEAR320S_MAC_MODE_MASK << SPEAR320S_MAC1_MODE_SHIFT) |
			SPEAR320S_MII_MDIO_MASK,
		.value = (SPEAR320S_MAC_MODE_SMII << SPEAR320S_MAC2_MODE_SHIFT)|
			(SPEAR320S_MAC_MODE_SMII << SPEAR320S_MAC1_MODE_SHIFT) |
			SPEAR320S_MII_MDIO_10_11_VAL,
	},
};

static struct pmx_mux_reg pmx_rmii1_2_ext_mux[] = {
	{
		.address = SPEAR320S_IP_SEL_PAD_10_19_REG,
		.mask = SPEAR320S_PMX_PL_10_11_MASK |
			SPEAR320S_PMX_PL_13_14_MASK |
			SPEAR320S_PMX_PL_15_16_MASK |
			SPEAR320S_PMX_PL_17_18_MASK | SPEAR320S_PMX_PL_19_MASK,
		.value = SPEAR320S_PMX_RMII_PL_10_11_VAL |
			SPEAR320S_PMX_RMII_PL_13_14_VAL |
			SPEAR320S_PMX_RMII_PL_15_16_VAL |
			SPEAR320S_PMX_RMII_PL_17_18_VAL |
			SPEAR320S_PMX_RMII_PL_19_VAL,
	}, {
		.address = SPEAR320S_IP_SEL_PAD_20_29_REG,
		.mask = SPEAR320S_PMX_PL_20_MASK |
			SPEAR320S_PMX_PL_21_TO_27_MASK,
		.value = SPEAR320S_PMX_RMII_PL_20_VAL |
			SPEAR320S_PMX_RMII_PL_21_TO_27_VAL,
	}, {
		.address = SPEAR320S_EXT_CTRL_REG,
		.mask = (SPEAR320S_MAC_MODE_MASK << SPEAR320S_MAC2_MODE_SHIFT) |
			(SPEAR320S_MAC_MODE_MASK << SPEAR320S_MAC1_MODE_SHIFT) |
			SPEAR320S_MII_MDIO_MASK,
		.value = (SPEAR320S_MAC_MODE_RMII << SPEAR320S_MAC2_MODE_SHIFT)|
			(SPEAR320S_MAC_MODE_RMII << SPEAR320S_MAC1_MODE_SHIFT) |
			SPEAR320S_MII_MDIO_10_11_VAL,
	},
};

#define pmx_mii1_2_common_modes						\
	{								\
		.ids = AUTO_NET_SMII_MODE | AUTO_EXP_MODE |		\
			SMALL_PRINTERS_MODE | SPEAR320S_EXTENDED_MODE,	\
		.mux_regs = pmx_mii1_2_mux,				\
		.mux_reg_cnt = ARRAY_SIZE(pmx_mii1_2_mux),		\
	}

static struct pmx_dev_mode pmx_mii1_2_modes[][2] = {
	{
		/* configure as smii */
		pmx_mii1_2_common_modes,
		{
			.ids = SPEAR320S_EXTENDED_MODE,
			.mux_regs = pmx_smii1_2_ext_mux,
			.mux_reg_cnt = ARRAY_SIZE(pmx_smii1_2_ext_mux),
		},
	}, {
		/* configure as rmii */
		pmx_mii1_2_common_modes,
		{
			.ids = SPEAR320S_EXTENDED_MODE,
			.mux_regs = pmx_rmii1_2_ext_mux,
			.mux_reg_cnt = ARRAY_SIZE(pmx_rmii1_2_ext_mux),
		},
	},
};

struct pmx_dev spear320_pmx_mii1_2[] = {
	{
		.name = "smii1_2",
		.modes = pmx_mii1_2_modes[0],
		.mode_count = ARRAY_SIZE(pmx_mii1_2_modes[0]),
	}, {
		.name = "rmii1_2",
		.modes = pmx_mii1_2_modes[1],
		.mode_count = ARRAY_SIZE(pmx_mii1_2_modes[1]),
	},
};

/* pmx driver structure */
static struct pmx_driver pmx_driver = {
	.mode_reg = {.address = SPEAR320_CONTROL_REG, .mask = 0x00000007},
};

/*
 * Function to dnamically set control of shared mdio lines to concerned
 * mac controller
 */
#ifdef CONFIG_ETH_MDIO_HOOK
void arch_get_mdio_control(struct eth_device *netdev)
{
	u32 val;

	val = readl(SPEAR320_CONTROL_REG);

#ifdef CONFIG_SPEAR320
	/*
	 * There is just one Ethernet interface on SPEAr320 PLC board
	 * which is RAS Eth#2
	 */
	if (!strcmp(netdev->name, "macb0"))
		val |= (0x1 << MII_ENB);
	else
		printf ("no such device:%s\n", netdev->name);
#elif defined(CONFIG_SPEAR320_HMI)
	/*
	 * There are two Ethernet (RMII) interfaces on SPEAr320 new HMI
	 * board
	 */
	if (!strcmp(netdev->name, "macb0"))
		val &= ~(0x1 << MII_ENB);
	else if (!strcmp(netdev->name, "macb1"))
		val |= (0x1 << MII_ENB);
	else
		printf ("no such device:%s\n", netdev->name);
#else
#error "unknown SPEAr320 based board"
#endif

	writel(val, SPEAR320_CONTROL_REG);
}
#endif

/*
 * retreive the SoC-id for differentiating between SPEAr320
 * and future variants of the same (for e.g. SPEAr320s)
 */
#define SOC_SPEAR320_ID		0x1
#define SOC_SPEAR320S_ID	0x2
static int get_soc_id(void)
{
	struct misc_regs *const misc_regs_p =
		(struct misc_regs *)CONFIG_SPEAR_MISCBASE;

	int soc_id = readl(&misc_regs_p->soc_core_id);

	return soc_id ? SOC_SPEAR320S_ID : SOC_SPEAR320_ID;
}

/*
 * There are two Soc Versions of SPEAr320. One is called SPEAr320 and
 * other is called SPEAr320s.
 */
static void spear320_init(void)
{
	/* Currently empty */
}

static void spear320s_init(struct pmx_mode *pmx_mode)
{
	if (pmx_mode == &spear320s_extended_mode) {
		/* Fix SPEAr320s specific pmx stuff */
		pmx_driver.mode_reg.address = SPEAR320S_EXT_CTRL_REG;
		pmx_driver.mode_reg.mask = SPEAR320S_EXTENDED_MODE_VAL;
	} else {
		/* Clear ext ctrl reg to select legacy modes */
		writel(0, SPEAR320S_EXT_CTRL_REG);
	}
}

void spear320_common_init(struct pmx_mode *pmx_mode, struct pmx_dev
		**pmx_devs, u8 pmx_dev_count)
{
	int ret = 0;
	int soc_id;

	/* retreive soc-id */
	soc_id = get_soc_id();
	if (soc_id == SOC_SPEAR320_ID)
		spear320_init();
	else
		spear320s_init(pmx_mode);

	/* pmx initialization */
	pmx_driver.mode = pmx_mode;
	pmx_driver.devs = pmx_devs;
	pmx_driver.devs_count = pmx_dev_count;

	/* This fixes addresses of all pmx devices for spear320 */
	spear3xx_pmx_init_addr(&pmx_driver, SPEAR320_PAD_MUX_CONFIG_REG);
	ret = pmx_register(&pmx_driver);
	if (ret)
		printf("padmux: registeration failed. err no: %d\n", ret);
}
