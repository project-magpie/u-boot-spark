/*
 * (C) Copyright 2013-2014 STMicroelectronics
 * Youssef TRIKI <youssef.triki@st.com>
 * Sean McGoogan <Sean.McGoogan@st.com>
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
#include <stm/soc.h>


	/*
	 * Pointer to SoC-specific mmc_getcd() function.
	 */
typedef int (*const stm_getcd_fn)(const int port);
static stm_getcd_fn getcd =
#if defined(CONFIG_STM_STXH407)
	stxh407_mmc_getcd;
#else
#	error Missing xxx_mmc_getcd() for this SoC!
#endif	/* CONFIG_STM_STxxxxx */


static char * const sdhci_name[] =
{
	"stm-sdhci0",	/* MMC #0 */
	"stm-sdhci1",	/* MMC #1 */
	"stm-sdhci2",	/* MMC #2 */
	"stm-sdhci3",	/* MMC #3 */
};

extern int board_mmc_getcd(struct mmc * const mmc)
{
	if (!strcmp(mmc->name,sdhci_name[0]))
	{
		return !(*getcd)(0);	/* MMC #0 */
	}
	else if (!strcmp(mmc->name,sdhci_name[1]))
	{
		return !(*getcd)(1);	/* MMC #1 */
	}
	else if (!strcmp(mmc->name,sdhci_name[2]))
	{
		return !(*getcd)(2);	/* MMC #2 */
	}
	else if (!strcmp(mmc->name,sdhci_name[3]))
	{
		return !(*getcd)(3);	/* MMC #3 */
	}
	else
	{
		BUG();		/* should never get here! */
	}
}

extern int stm_sdhci_init(const int port, const u32 regbase)
{
	struct sdhci_host *host;

	/* paranoia */
	BUG_ON(port >= ARRAY_SIZE(sdhci_name));

	host = (struct sdhci_host *)malloc(sizeof(struct sdhci_host));
	if (!host)
	{
		printf("stm_sdhci_init() malloc fail for host structure!\n");
		return -1;	/* return an error */
	}
		/* fill in the newly allocated host structure */
	memset(host, 0, sizeof(struct sdhci_host));
	host->name      = sdhci_name[port];
	host->ioaddr    = (void *)regbase;
	host->quirks    = SDHCI_QUIRK_BROKEN_VOLTAGE | SDHCI_QUIRK_BROKEN_R1B | SDHCI_QUIRK_WAIT_SEND_CMD | SDHCI_QUIRK_32BIT_DMA_ADDR;
	host->version   = sdhci_readw(host, SDHCI_HOST_VERSION);
	host->host_caps = MMC_MODE_HC /* | MMC_MODE_8BIT*/;

#if defined(CONFIG_STM_STXH407)
	if (port == 0)
	{
		host->voltages = MMC_VDD_165_195;
		host->quirks |= SDHCI_QUIRK_NO_HISPD_BIT;
		return add_sdhci(host, 50*1000*1000, 400*1000);
	}
	else if (port == 1)
#endif	/* CONFIG_STM_STXH407 */
	{
		host->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
		return add_sdhci(host, 50*1000*1000, 400*1000);
	}
#if defined(CONFIG_STM_STXH407)
	else
	{
		printf("stm_sdhci_init() no such port! (port=%u)\n", port);
		return -1;	/* return an error */
	}
#endif	/* CONFIG_STM_STXH407 */
}
