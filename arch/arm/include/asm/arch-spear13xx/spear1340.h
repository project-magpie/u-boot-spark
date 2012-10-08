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

#ifndef _ASM_ARCH_SPEAR1340_PINMUX_H
#define _ASM_ARCH_SPEAR1340_PINMUX_H

#include <asm/arch/spear13xx.h>

/* Macros to select an interface within a peripheral pins */
#define SPEAR1340_PERIP_CFG			(CONFIG_SYS_MISC_BASE + 0x42C)
	/* PERIP_CFG register masks */
	#define MCIF_SEL_SD				1
	#define MCIF_SEL_CF				2
	#define MCIF_SEL_XD				3
	#define MCIF_SEL_MASK				3
	#define MCIF_SEL_SHIFT				0

#define SPEAR1340_GMAC_CLK_CFG			(CONFIG_SYS_MISC_BASE + 0x248)
	#define GMAC_PHY_IF_GMII_VAL			(0 << 3)
	#define GMAC_PHY_IF_RGMII_VAL			(1 << 3)
	#define GMAC_PHY_IF_SGMII_VAL			(2 << 3)
	#define GMAC_PHY_IF_RMII_VAL			(4 << 3)
	#define GMAC_PHY_IF_SEL_MASK			(7 << 3)

/*
 * Macro's for first level of pmx - pads as gpio OR peripherals. There are 8
 * registers with 32 bits each for handling gpio pads, register 8 has only 26
 * relevant bits.
 */
#define SPEAR1340_PAD_FUNCTION_EN_1		(CONFIG_SYS_MISC_BASE + 0x668)
#define SPEAR1340_PAD_FUNCTION_EN_2		(CONFIG_SYS_MISC_BASE + 0x66C)
#define SPEAR1340_PAD_FUNCTION_EN_3		(CONFIG_SYS_MISC_BASE + 0x670)
#define SPEAR1340_PAD_FUNCTION_EN_4		(CONFIG_SYS_MISC_BASE + 0x674)
#define SPEAR1340_PAD_FUNCTION_EN_5		(CONFIG_SYS_MISC_BASE + 0x690)
#define SPEAR1340_PAD_FUNCTION_EN_6		(CONFIG_SYS_MISC_BASE + 0x694)
#define SPEAR1340_PAD_FUNCTION_EN_7		(CONFIG_SYS_MISC_BASE + 0x698)
#define SPEAR1340_PAD_FUNCTION_EN_8		(CONFIG_SYS_MISC_BASE + 0x69C)
	/* macro's for making pads as gpio's */
	#define PADS_AS_GPIO_REG0_MASK			0xFFFFFFFE
	#define PADS_AS_GPIO_REGS_MASK			0xFFFFFFFF
	#define PADS_AS_GPIO_REG7_MASK			0x07FFFFFF
	/* macro's for making pads as peripherals */
	#define FSMC_16_BIT_AND_KBD_ROW_COL_REG0_MASK	0x00000FFE
	#define UART0_ENH_AND_GPT_REG0_MASK		0x0003F000
	#define PWM1_AND_KBD_COL5_REG0_MASK		0x00040000
	#define I2C1_REG0_MASK				0x01080000
	#define SPDIF_IN_REG0_MASK			0x00100000
	#define PWM2_AND_GPT0_TMR0_CPT_REG0_MASK	0x00400000
	#define PWM3_AND_GPT0_TMR1_CLK_REG0_MASK	0x00800000
	#define PWM0_AND_SSP0_CS1_REG0_MASK		0x02000000
	#define VIP_AND_CAM3_REG0_MASK			0xFC200000
	#define VIP_AND_CAM3_REG1_MASK			0x0000000F
	#define VIP_REG1_MASK				0x00001EF0
	#define VIP_AND_CAM2_REG1_MASK			0x007FE100
	#define VIP_AND_CAM1_REG1_MASK			0xFF800000
	#define VIP_AND_CAM1_REG2_MASK			0x00000003
	#define VIP_AND_CAM0_REG2_MASK			0x00001FFC
	#define SMI_REG2_MASK				0x0021E000
	#define SSP0_REG2_MASK				0x001E0000
	#define TS_AND_SSP0_CS2_REG2_MASK		0x00400000
	#define UART0_REG2_MASK				0x01800000
	#define UART1_REG2_MASK				0x06000000
	#define I2S_IN_REG2_MASK			0xF8000000
	#define DEVS_GRP_AND_MIPHY_DBG_REG3_MASK	0x000001FE
	#define I2S_OUT_REG3_MASK			0x000001EF
	#define I2S_IN_REG3_MASK			0x00000010
	#define GMAC_REG3_MASK				0xFFFFFE00
	#define GMAC_REG4_MASK				0x0000001F
	#define DEVS_GRP_AND_MIPHY_DBG_REG4_MASK	0x7FFFFF20
	#define SSP0_CS3_REG4_MASK			0x00000020
	#define I2C0_REG4_MASK				0x000000C0
	#define CEC0_REG4_MASK				0x00000100
	#define CEC1_REG4_MASK				0x00000200
	#define SPDIF_OUT_REG4_MASK			0x00000400
	#define CLCD_REG4_MASK				0x7FFFF800
	#define CLCD_AND_ARM_TRACE_REG4_MASK		0x80000000
	#define CLCD_AND_ARM_TRACE_REG5_MASK		0xFFFFFFFF
	#define CLCD_AND_ARM_TRACE_REG6_MASK		0x00000001
	#define FSMC_PNOR_AND_MCIF_REG6_MASK		0x073FFFFE
	#define MCIF_REG6_MASK				0xF8C00000
	#define MCIF_REG7_MASK				0x000043FF
	#define FSMC_8BIT_REG7_MASK			0x07FFBC00



/* - If peripherals, then primary OR alternate peripheral */
#define SPEAR1340_PAD_SHARED_IP_EN_1		(CONFIG_SYS_MISC_BASE + 0x6A0)
#define SPEAR1340_PAD_SHARED_IP_EN_2		(CONFIG_SYS_MISC_BASE + 0x6A4)
	/*
	 * Macro's for second level of pmx
	 *  - pads as primary OR alternate peripheral
	 */
	/* Write 0 to enable FSMC_16_BIT */
	#define KBD_ROW_COL_MASK			(1 << 0)
	/* Write 0 to enable UART0_ENH */
	#define GPT_MASK				(1 << 1)
	/* Write 0 to enable PWM1 */
	#define KBD_COL5_MASK				(1 << 2)
	/* Write 0 to enable PWM2 */
	#define GPT0_TMR0_CPT_MASK			(1 << 3)
	/* Write 0 to enable PWM3 */
	#define GPT0_TMR1_CLK_MASK			(1 << 4)
	/* Write 0 to enable PWM0 */
	#define SSP0_CS1_MASK				(1 << 5)
	/* Write 0 to enable VIP */
	#define CAM3_MASK				(1 << 6)
	/* Write 0 to enable VIP */
	#define CAM2_MASK				(1 << 7)
	/* Write 0 to enable VIP */
	#define CAM1_MASK				(1 << 8)
	/* Write 0 to enable VIP */
	#define CAM0_MASK				(1 << 9)
	/* Write 0 to enable TS */
	#define SSP0_CS2_MASK				(1 << 10)
	/* Write 0 to enable FSMC PNOR */
	#define MCIF_MASK				(1 << 11)
	/* Write 0 to enable CLCD */
	#define ARM_TRACE_MASK				(1 << 12)
	/* Write 0 to enable I2S, SSP0_CS2, CEC0, 1, SPDIF out, CLCD */
	#define MIPHY_DBG_MASK				(1 << 13)

/* Pull DOWN and Pull UP */
#define SPEAR1340_PAD_PU_CFG_1		(CONFIG_SYS_MISC_BASE + 0x600)
#define SPEAR1340_PAD_PD_CFG_1		(CONFIG_SYS_MISC_BASE + 0x620)

/* Macro's to configure plgpios as Pull UP, Pull Down */
#define CONFIG_SYS_PLGPIO_BASE		0xE2800000
	#define SPEAR1340_PLGPIO_EN0		(CONFIG_SYS_PLGPIO_BASE + 0x00)
	#define SPEAR1340_PLGPIO_IN0		(CONFIG_SYS_PLGPIO_BASE + 0x20)
	#define SPEAR1340_PLGPIO_OUT0		(CONFIG_SYS_PLGPIO_BASE + 0x40)
	#define SPEAR1340_MAX_PLGPIOS		249

extern void spear1340_pins_default(void);
extern void spear1340_enable_pins(u32 ip, u32 mode);
extern void spear1340_configure_pin(u32 plgpio, u32 mode);
extern void spear1340_plgpio_set(u32 plgpio, u32 val);
extern int  spear1340_plgpio_get(u32 plgpio);

#endif
