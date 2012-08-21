/*
 * (C) Copyright 2009
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
#include <asm/arch/emi.h>

struct cust_emi_para {
	unsigned int tap;
	unsigned int tsdp;
	unsigned int tdpw;
	unsigned int tdpr;
	unsigned int tdcs;
};

/* EMI timing setting of m28w640hc of linux kernel */
const struct cust_emi_para emi_timing_m28w640hc = {
	.tap = 0x10,
	.tsdp = 0x05,
	.tdpw = 0x0a,
	.tdpr = 0x0a,
	.tdcs = 0x05,
};

/* EMI timing setting of bootrom */
const struct cust_emi_para emi_timing_bootrom = {
	.tap = 0xf,
	.tsdp = 0x0,
	.tdpw = 0xff,
	.tdpr = 0x111,
	.tdcs = 0x02,
};

void emi_init(void)
{
	const struct cust_emi_para *p = &emi_timing_m28w640hc;
	struct emi_regs *emi_regs_p = (struct emi_regs *)CONFIG_SPEAR_EMIBASE;
	unsigned int cs;
	unsigned int val, tmp;

	val = readl(CONFIG_SPEAR_RASBASE);

	if (val & EMI_ACKMSK)
		tmp = 0x3f;
	else
		tmp = 0x0;

	writel(tmp, &emi_regs_p->ack);

	for (cs = 0; cs < CONFIG_SYS_MAX_FLASH_BANKS; cs++) {
		writel(p->tap, &emi_regs_p->bank_regs[cs].tap);
		writel(p->tsdp, &emi_regs_p->bank_regs[cs].tsdp);
		writel(p->tdpw, &emi_regs_p->bank_regs[cs].tdpw);
		writel(p->tdpr, &emi_regs_p->bank_regs[cs].tdpr);
		writel(p->tdcs, &emi_regs_p->bank_regs[cs].tdcs);
		writel(EMI_CNTL_ENBBYTERW | ((val & 0x18) >> 3),
		       &emi_regs_p->bank_regs[cs].control);
	}
}
