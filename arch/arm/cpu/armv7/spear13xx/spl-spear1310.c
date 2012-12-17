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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/generic.h>
#include <asm/arch/misc.h>
#include <asm/arch/mpmc.h>

static void pll_init(void)
{
	struct spear1310_misc_regs *const misc_p =
		(struct spear1310_misc_regs *)CONFIG_SYS_MISC_BASE;
	u32 usbphycfg;

	/* Initialize PLLs */
	writel(SPEAR1310_FREQ_1000, &misc_p->pll1_frq);
	writel(readl(&misc_p->pll1_ctr) | SPEAR1310_PLLENABLE,
			&misc_p->pll1_ctr);

	writel(SPEAR1310_FREQ_125, &misc_p->pll2_frq);
	writel(readl(&misc_p->pll2_ctr) | SPEAR1310_PLLENABLE,
			&misc_p->pll2_ctr);

	writel(SPEAR1310_FREQ_332, &misc_p->pll3_frq);
	writel(readl(&misc_p->pll3_ctr) | SPEAR1310_PLLENABLE,
			&misc_p->pll3_ctr);
#if defined(CONFIG_SPEAR_DDRFREQ400)
	writel(SPEAR1310_FREQ_400, &misc_p->pll4_frq);
#elif defined(CONFIG_SPEAR_DDRFREQ533)
	writel(SPEAR1310_FREQ_533, &misc_p->pll4_frq);
#else
#error Define one of the valid DDR frequency(CONFIG_SPEAR_DDRFREQ[533/400])
#endif
	/* strobing required for pll4 */
	writel(0x60A, &misc_p->pll4_ctr);
	writel(0x60E, &misc_p->pll4_ctr);
	writel(0x606, &misc_p->pll4_ctr);
	writel(0x60E, &misc_p->pll4_ctr);

	usbphycfg = readl(&misc_p->usbphy_gen_cfg);
	usbphycfg &= ~(SPEAR1310_COMMON_PWDN | SPEAR1310_USBPHY_POR);
	usbphycfg |= SPEAR1310_USBPHY_RST;
	writel(usbphycfg, &misc_p->usbphy_gen_cfg);

	while (!(readl(&misc_p->usbphy_gen_cfg) & SPEAR1310_USB_PLL_LOCK))
		;
	usbphycfg = readl(&misc_p->usbphy_gen_cfg);

	usbphycfg |= SPEAR1310_AUTOPPD_ON_OVRCURR | \
		     SPEAR1310_UTMI_XFER_RST0 | \
		     SPEAR1310_UTMI_XFER_RST1 | \
		     SPEAR1310_UTMI_XFER_RST2;

	writel(usbphycfg, &misc_p->usbphy_gen_cfg);

	/* wait for pll locks */
	while (!(readl(&misc_p->pll1_ctr) & SPEAR1310_PLLLOCK))
		;
	while (!(readl(&misc_p->pll2_ctr) & SPEAR1310_PLLLOCK))
		;
	while (!(readl(&misc_p->pll3_ctr) & SPEAR1310_PLLLOCK))
		;
	while (!(readl(&misc_p->pll4_ctr) & SPEAR1310_PLLLOCK))
		;
}

static void sys_init(void)
{
	struct spear1310_misc_regs *const misc_p =
		(struct spear1310_misc_regs *)CONFIG_SYS_MISC_BASE;
	u32 sysclkctrl;

	/* Set system state to SLOW */
	sysclkctrl = readl(&misc_p->sys_clk_ctrl);
	sysclkctrl &= ~SPEAR1310_SYS_MODE_MASK;
	sysclkctrl |= SPEAR1310_XTAL_TIMEOUT_ENB | \
		      SPEAR1310_PLL_TIMEOUT_ENB | \
		      SPEAR1310_SYS_MODE_REQ_SLOW;
	writel(sysclkctrl, &misc_p->sys_clk_ctrl);

	writel(SPEAR1310_PLL_TIM, &misc_p->sys_clk_plltimer);
	writel(SPEAR1310_OSCI_TIM, &misc_p->sys_clk_oscitimer);

	/* Initialize PLLs */
	pll_init();

	/* Set system state to NORMAL */
	sysclkctrl = readl(&misc_p->sys_clk_ctrl);
	sysclkctrl &= ~SPEAR1310_SYS_MODE_MASK;
	sysclkctrl |= SPEAR1310_XTAL_TIMEOUT_ENB | \
		      SPEAR1310_PLL_TIMEOUT_ENB | \
		      SPEAR1310_SYS_MODE_REQ_NORMAL;
	writel(sysclkctrl, &misc_p->sys_clk_ctrl);

	/* Wait for system to switch to normal mode */
	while ((readl(&misc_p->sys_clk_ctrl) & SPEAR1310_SYS_STATE_MASK) !=
			SPEAR1310_SYS_STATE_NORMAL)
		;

#if defined(CONFIG_C3)
	writel(readl(&misc_p->perip1_clk_enb) | SPEAR1310_C3_CLKEN,
			&misc_p->perip1_clk_enb);
#endif
}

static void ddr_clock_init(void)
{
	struct spear1310_misc_regs *const misc_p =
		(struct spear1310_misc_regs *)CONFIG_SYS_MISC_BASE;
	u32 perip_clkcfg, perip2_clkenb, perip2_swrst;

	perip_clkcfg = readl(&misc_p->perip_clk_cfg);
	perip_clkcfg |= SPEAR1310_MPMCCLKPLL4;
	writel(perip_clkcfg, &misc_p->perip_clk_cfg);

	perip2_clkenb = readl(&misc_p->perip2_clk_enb);
	perip2_clkenb |= SPEAR1310_DDRCTRL_CLKEN | SPEAR1310_DDRCORE_CLKEN;
	writel(perip2_clkenb, &misc_p->perip2_clk_enb);

	perip2_swrst = readl(&misc_p->perip2_sw_rst);
	perip2_swrst |= SPEAR1310_DDRCTRL_RST | SPEAR1310_DDRCORE_RST;
	writel(perip2_swrst, &misc_p->perip2_sw_rst);

	perip2_swrst = readl(&misc_p->perip2_sw_rst);
	perip2_swrst &= ~(SPEAR1310_DDRCTRL_RST | SPEAR1310_DDRCORE_RST);
	writel(perip2_swrst, &misc_p->perip2_sw_rst);
}

static void ddr_pad_compensation(u8 enable)
{
	struct spear1310_misc_regs *const misc_p =
		(struct spear1310_misc_regs *)CONFIG_SYS_MISC_BASE;

	if (enable) {
		writel(0x00000400, &misc_p->compensation_ddr_cfg);
		writel(0x00000400, &misc_p->compensation_ddr_cfg);
		writel(0x00000400, &misc_p->compensation_ddr_cfg);

		/* wait for comzcrdy done */
		while (!(readl(&misc_p->compensation_ddr_cfg) & 0x1))
			;
	} else {
		writel(0x00000000, &misc_p->compensation_ddr_cfg);
		writel(0x00000000, &misc_p->compensation_ddr_cfg);
		writel(0x00000000, &misc_p->compensation_ddr_cfg);
	}
}

void ddr_init(void)
{
	struct spear1310_misc_regs *const misc_p =
		(struct spear1310_misc_regs *)CONFIG_SYS_MISC_BASE;
#if defined(CONFIG_DDR_ECC_ENABLE)
	u32 mpmc_cfg;
#endif

	/* Clock related settings for DDR */
	ddr_clock_init();

#if defined(CONFIG_DDR_ECC_ENABLE)
	/*
	 * Following modifies write access as non-bufferable
	 * and read to happen word by word. Without this
	 * dependent write-read are happening out of order
	 * resulting in Linux crash.
	 */
	mpmc_cfg = readl(&misc_p->mpmc_cfg);
	mpmc_cfg |= SPEAR1310_AXI0_AWCOBUF | SPEAR1310_AXI2_AWCOBUF;
	mpmc_cfg &= ~(SPEAR1310_AXI0_CMDTHRESH_MSK | \
			SPEAR1310_AXI2_CMDTHRESH_MSK);
	mpmc_cfg |= (SPEAR1310_AXI0_CMDTHRESH_1 | SPEAR1310_AXI2_CMDTHRESH_1);
	writel(mpmc_cfg, &misc_p->mpmc_cfg);

	/* enable MPMC ECC gasket for all AXI ports */
	writel(0x0, &misc_p->mpmc_ctr_sts);

	/* wait for turn-on */
	while ((readl(&misc_p->mpmc_ctr_sts) & 0xFFF))
		;
#endif

	/* Initialize mpmc register values */
	spear13xx_mpmc_init((u32 *)CONFIG_SYS_MPMC_BASE, CONFIG_SPEAR_MPMCREGS);

	writel(SPEAR1310_PAD_VREF | SPEAR1310_DATA_PROGB | \
		SPEAR1310_DATA_PROGA | SPEAR1310_CLK_PROGB | \
		SPEAR1310_CLK_PROGA | SPEAR1310_CTRL_PROGB | \
		SPEAR1310_CTRL_PROGA,
		&misc_p->ddr_pad_cfg);

	ddr_pad_compensation(0);
	spear13xx_ddr_level();
	ddr_pad_compensation(1);
}

void soc_init(void)
{
	struct spear1310_misc_regs *const misc_p =
		(struct spear1310_misc_regs *)CONFIG_SYS_MISC_BASE;

	/* Initialize PLLs */
	sys_init();

	/* Enable IPs (release reset) */
	writel(SPEAR1310_PERIPH1_RST_ALL, &misc_p->perip1_sw_rst);
	writel(SPEAR1310_PERIPH2_RST_ALL, &misc_p->perip2_sw_rst);
	writel(SPEAR1310_RAS_RST_ALL, &misc_p->ras_sw_rst);
}
