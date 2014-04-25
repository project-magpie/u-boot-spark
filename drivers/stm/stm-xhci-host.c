/*
 *  Copyright (C) 2014 STMicroelectronics Limited
 *	Sachin Verma <sachin.verma@st.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#include <common.h>
#include <malloc.h>
#include <usb.h>
#include <watchdog.h>
#include <asm-generic/errno.h>
#include <linux/compat.h>
#include <linux/usb/dwc3.h>

#include <../drivers/usb/host/xhci.h>
#include <stm/stbus.h>

/* Declare global data pointer */
//DECLARE_GLOBAL_DATA_PTR;

/**
 * Contains pointers to register base addresses
 * for the usb controller.
 */
struct stm_xhci {
    struct xhci_hccr *hcd;
    struct dwc3 *dwc3_reg;
};

static struct stm_xhci stmxhci;


void dwc3_set_mode(struct dwc3 *dwc3_reg, u32 mode)
{
	clrsetbits_le32(&dwc3_reg->g_ctl,
			DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG),
			DWC3_GCTL_PRTCAPDIR(mode));
}

static void dwc3_core_soft_reset(struct dwc3 *dwc3_reg)
{
	/* Before Resetting PHY, put Core in Reset */
	setbits_le32(&dwc3_reg->g_ctl,
			DWC3_GCTL_CORESOFTRESET);

	/* Assert USB3 PHY reset */
	setbits_le32(&dwc3_reg->g_usb3pipectl[0],
			DWC3_GUSB3PIPECTL_PHYSOFTRST);

	/* Assert USB2 PHY reset */
	setbits_le32(&dwc3_reg->g_usb2phycfg,
			DWC3_GUSB2PHYCFG_PHYSOFTRST);

	mdelay(100);

	/* Clear USB3 PHY reset */
	clrbits_le32(&dwc3_reg->g_usb3pipectl[0],
			DWC3_GUSB3PIPECTL_PHYSOFTRST);

	/* Clear USB2 PHY reset */
	clrbits_le32(&dwc3_reg->g_usb2phycfg,
			DWC3_GUSB2PHYCFG_PHYSOFTRST);

	/* After PHYs are stable we can take Core out of reset state */
	clrbits_le32(&dwc3_reg->g_ctl,
			DWC3_GCTL_CORESOFTRESET);
}

static int dwc3_core_init(struct dwc3 *dwc3_reg)
{
	u32 reg;
	u32 revision;
	unsigned int dwc3_hwparams1;

	revision = readl(&dwc3_reg->g_snpsid);
	/* This should read as U3 followed by revision number */
	if ((revision & DWC3_GSNPSID_MASK) != 0x55330000) {
		puts("this is not a DesignWare USB3 DRD Core\n");
		return -EINVAL;
	}

	dwc3_core_soft_reset(dwc3_reg);

	dwc3_hwparams1 = readl(&dwc3_reg->g_hwparams1);

	reg = readl(&dwc3_reg->g_ctl);
	reg &= ~DWC3_GCTL_SCALEDOWN_MASK;
	reg &= ~DWC3_GCTL_DISSCRAMBLE;
	switch (DWC3_GHWPARAMS1_EN_PWROPT(dwc3_hwparams1)) {
	case DWC3_GHWPARAMS1_EN_PWROPT_CLK:
		reg &= ~DWC3_GCTL_DSBLCLKGTNG;
		break;
	default:
		debug("No power optimization available\n");
	}

	/*
	 * WORKAROUND: DWC3 revisions <1.90a have a bug
	 * where the device can fail to connect at SuperSpeed
	 * and falls back to high-speed mode which causes
	 * the device to enter a Connect/Disconnect loop
	 */
	if ((revision & DWC3_REVISION_MASK) < 0x190a)
		reg |= DWC3_GCTL_U2RSTECN;

	writel(reg, &dwc3_reg->g_ctl);

	return 0;
}


static int stm_xhci_core_init(struct stm_xhci *stmxhci)
{
    int ret;
    ret = dwc3_core_init(stmxhci->dwc3_reg);
    if (ret) {
        printf("stm_xhci_core_init:: failed to initialize core\n");
        return -EINVAL;
    }
    /* We are hard-coding DWC3 core to Host Mode */
    dwc3_set_mode(stmxhci->dwc3_reg, DWC3_GCTL_PRTCAP_HOST);
    return 0;
}

int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	uint32_t hc_length;
    struct stm_xhci *ctx = &stmxhci;

	/*
	 * First, we call the board- and CPU-specific initialization functions,
	 * which will enable the controller, and configure all the USB PIOs.
	 * We will also call the function STM_start_host_control(),
	 * which configures the AMBA interface for the IP block.
	 */
	usb_cpu_init();


	/*
	 * Save address for both the HCCR and HCOR registers in the EHCI
	 * H/W controller, into the 2 indirect-pointers "hccr", and "hcor".
	 * NOTE: this limits us to only ONE host-controller (QQQ: allow 1+)
	 */
	ctx->hcd = (struct xhci_hccr *)STM_DWC3_CORE_BASE;
	ctx->dwc3_reg = (struct dwc3 *)((char *)(ctx->hcd) + DWC3_REG_OFFSET);

	stm_xhci_core_init(ctx);

    *hccr = (ctx->hcd);
    *hcor = (struct xhci_hcor *)((uint32_t) *hccr
                + HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

    return 0;
}

void xhci_hcd_stop(int index)
{
	// QQQQ - To implement Platform specific shutdown
	struct stm_xhci *ctx = &stmxhci;
	return 0;
}

