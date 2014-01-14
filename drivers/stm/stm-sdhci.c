/*
 * (C) Copyright 2013 STMicroelectronics
 * Youssef TRIKI <youssef.triki@st.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <common.h>
#include <malloc.h>
#include <sdhci.h>

static char *STM_SDHCI_NAME[] = {"stm-sdhci0","stm-sdhci1","stm-sdhci2","stm-sdhci3"};

int stm_sdhci_init(u32 port,u32 regbase)
{
	struct sdhci_host *host = NULL;
	host = (struct sdhci_host *)malloc(sizeof(struct sdhci_host));
	if (!host) {
		printf("stm_sdhci_init() malloc fail for host structure !\n");
		return 1;
	}
	memset(host,0,sizeof(struct sdhci_host));
	host->name      = STM_SDHCI_NAME[port];
	host->ioaddr    = (void *)regbase;
	host->quirks    = SDHCI_QUIRK_BROKEN_VOLTAGE | SDHCI_QUIRK_BROKEN_R1B | SDHCI_QUIRK_WAIT_SEND_CMD | SDHCI_QUIRK_32BIT_DMA_ADDR;
	host->version   = sdhci_readw(host, SDHCI_HOST_VERSION);
	host->host_caps = MMC_MODE_HC/* | MMC_MODE_8BIT*/;
#ifdef CONFIG_STM_STXH407
	if (port==1) 
	{
		host->voltages= (MMC_VDD_32_33 | MMC_VDD_33_34);
		return add_sdhci(host,  50000000, 400000);
	}
	if (port==0)
	{
		host->voltages= MMC_VDD_165_195;
		host->quirks |= SDHCI_QUIRK_NO_HISPD_BIT;
		return add_sdhci(host, 50000000, 400000);
	}
	return 1;
#else
	host->voltages  = MMC_VDD_32_33 | MMC_VDD_33_34;
	return add_sdhci(host, 50000000, 400000);
#endif
}
