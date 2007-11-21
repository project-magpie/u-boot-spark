/*
 * (C) Copyright 2004 STMicroelectronics.
 *
 * Andy Sturges <andy.sturges@st.com>
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
#include <command.h>
#include <asm/stb7100reg.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <ata.h>

static int st40c_div[] = {1, 2, 3, 4, 6, 8, 1, 1};
static int st40b_div[] = {1, 2, 3, 4, 6, 8, 2, 2};
static int st40p_div[] = {4, 2, 4, 4, 6, 8, 4, 4};

void stb7100_clocks(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;
	unsigned long data, mdiv, ndiv, pdiv, pll0frq, pll1frq, mainfrq ;

	data = *STB7100_CLOCKGENA_PLL0_CFG;
	mdiv = data & 0xff;
	ndiv = (data >> 8) & 0xff;
	pdiv = (data >> 16) & 0x7;

	pll0frq = ((2 * INPUT_CLOCK_RATE * ndiv)/ mdiv) / (1 << pdiv);
	mainfrq = pll0frq / 2;

	data = *STB7100_CLOCKGENA_PLL1_CFG;
	mdiv = data & 0xff;
	ndiv = (data >> 8) & 0xff;
	pdiv = (data >> 16) & 0x7;
	pll1frq = ((2 * INPUT_CLOCK_RATE * ndiv)/ mdiv) / (1 << pdiv);

	bd->bi_pll0frq = pll0frq;
	bd->bi_pll1frq = pll1frq;
	bd->bi_st40cpufrq = mainfrq / st40c_div[*STB7100_CLOCKGENA_PLL0_CLK1_CTRL & 0x7];
	bd->bi_st40busfrq = mainfrq / st40b_div[*STB7100_CLOCKGENA_PLL0_CLK2_CTRL & 0x7];
	bd->bi_st40perfrq = mainfrq / st40p_div[*STB7100_CLOCKGENA_PLL0_CLK3_CTRL & 0x7];
	bd->bi_st231frq = pll1frq;
	bd->bi_stbusfrq = pll1frq/2;
	bd->bi_emifrq = pll1frq/4;
	bd->bi_lmifrq = pll1frq/2;
}

#ifdef CONFIG_DRIVER_NETSTMAC

#define MAC_SPEED_SEL       0x00100000 /* MAC is running at 100 Mbps speed */
#define PHY_CLK_EXT         0x00080000 /* PHY clock is external (RMII mode)*/
#define MII_MODE            0x00040000 /* RMII interface activated */
#define ETH_IF_ON           0x00010000 /* ETH interface on */
#define DVO_ETH_PAD_DISABLE 0x00020000 /* DVO eth pad disable */

#define PIO_BASE  0xb8020000

int stmac_default_pbl(void)
{
  DECLARE_GLOBAL_DATA_PTR;
  bd_t *bd = gd->bd;
  if (STB7100_DEVICEID_CUT(bd->bi_devid) == 1)
    return 1;
  return 32; /*  may be modified externally */
}

#ifdef CONFIG_STMAC_STE101P_RMII
void stb7109_mac_speed(int speed)
{

	unsigned long sysconf = *STB7100_SYSCONF_SYS_CFG07;

	if (speed == 100)
		sysconf |= MAC_SPEED_SEL;
	else if (speed == 10)
		sysconf &= ~MAC_SPEED_SEL;

	*STB7100_SYSCONF_SYS_CFG07 = sysconf;
}
#endif

/* ETH MAC pad configuration */
static void stmac_eth_hw_setup(void)
{
	unsigned long sysconf;

	sysconf = *STB7100_SYSCONF_SYS_CFG07;
	sysconf |= (DVO_ETH_PAD_DISABLE | ETH_IF_ON /*| MAC_SPEED_SEL*/);

#ifdef CONFIG_STMAC_STE101P_RMII
	sysconf |= MII_MODE; /* RMII selected*/
#else
	sysconf &= ~MII_MODE; /* MII selected */
#endif
	*STB7100_SYSCONF_SYS_CFG07 = sysconf;

	/* STe101P: enable the external interrupts */
	sysconf = *STB7100_SYSCONF_SYS_CFG10;
	sysconf |= 0x0000000f;
	*STB7100_SYSCONF_SYS_CFG10;

       /* Configure e/net PHY clock */
	SET_PIO_PIN(PIO_PORT(3), 7, STPIO_ALT_OUT);

	return;
}
#endif

int soc_init(void)
{
  DECLARE_GLOBAL_DATA_PTR;
  bd_t *bd = gd->bd;

  stb7100_clocks();

  #ifdef CONFIG_DRIVER_NETSTMAC
  if (STB7100_DEVICEID_7109(*STB7100_SYSCONF_DEVICEID_0))
	stmac_eth_hw_setup();
  else
	printf("warning STMAC configured for a non STb7109 device\n");
  #endif

  bd->bi_devid = *STB7100_SYSCONF_DEVICEID_0;

  /*  Make sure reset period is shorter than WDT timeout */

 *STB7100_SYSCONF_SYS_CFG09 = (*STB7100_SYSCONF_SYS_CFG09 & 0xFF000000) | 0x000A8C;

  return 0;
}

void stb7100_reset(void)
{
	ulong sr;
	asm ("stc sr, %0":"=r" (sr));
	sr |= (1 << 28);	/* set block bit */
	asm ("ldc %0, sr": :"r" (sr));
	asm volatile ("trapa #0");
}

#ifdef CONFIG_SH_STB7100_SATA

#define SATA_AHB2STBUS_BASE			0xB9209000
#define SATA_AHBHOST_BASE			0xB9209800

/* AHB_STBus protocol converter */
#define SATA_AHB2STBUS_STBUS_OPC		(SATA_AHB2STBUS_BASE + 0x0000)
#define SATA_AHB2STBUS_MESSAGE_SIZE_CONFIG	(SATA_AHB2STBUS_BASE + 0x0004)
#define SATA_AHB2STBUS_CHUNK_SIZE_CONFIG	(SATA_AHB2STBUS_BASE + 0x0008)
#define SATA_AHB2STBUS_SW_RESET			(SATA_AHB2STBUS_BASE + 0x000c)
#define SATA_AHB2STBUS_PC_STATUS		(SATA_AHB2STBUS_BASE + 0x0010)
#define SATA_PC_GLUE_LOGIC			(SATA_AHB2STBUS_BASE + 0x0014)
#define SATA_PC_GLUE_LOGICH			(SATA_AHB2STBUS_BASE + 0x0018)

#define SATA_CDR0                               (SATA_AHBHOST_BASE + 0x00000000)
#define SATA_CDR1                               (SATA_AHBHOST_BASE + 0x00000004)
#define SATA_CDR2                               (SATA_AHBHOST_BASE + 0x00000008)
#define SATA_CDR3                               (SATA_AHBHOST_BASE + 0x0000000c)
#define SATA_CDR4                               (SATA_AHBHOST_BASE + 0x00000010)
#define SATA_CDR5                               (SATA_AHBHOST_BASE + 0x00000014)
#define SATA_CDR6                               (SATA_AHBHOST_BASE + 0x00000018)
#define SATA_CDR7                               (SATA_AHBHOST_BASE + 0x0000001c)
#define SATA_CLR0                               (SATA_AHBHOST_BASE + 0x00000020)
#define SATA_SCR0                               (SATA_AHBHOST_BASE + 0x00000024)
#define SATA_SCR1                               (SATA_AHBHOST_BASE + 0x00000028)
#define SATA_SCR2                               (SATA_AHBHOST_BASE + 0x0000002c)
#define SATA_SCR3                               (SATA_AHBHOST_BASE + 0x00000030)
#define SATA_SCR4                               (SATA_AHBHOST_BASE + 0x00000034)
#define SATA_DMACR                              (SATA_AHBHOST_BASE + 0x00000070)
#define SATA_DBTSR                              (SATA_AHBHOST_BASE + 0x00000074)

#define SATA_PHYCR				(SATA_AHBHOST_BASE + 0x88)

static void stm_phy_reset(void)
{
  DECLARE_GLOBAL_DATA_PTR;
  bd_t *bd = gd->bd;

  if (STB7100_DEVICEID_7100(bd->bi_devid))
  {
    if (STB7100_DEVICEID_CUT(bd->bi_devid) == 1)
	writel(0x0013704A, SATA_PHYCR);
    else if (STB7100_DEVICEID_CUT(bd->bi_devid) == 3)
	writel(0x388FC, SATA_PHYCR);
    else
	writel(0x3889C, SATA_PHYCR);
  }

  udelay(100000);
}

void stb7100_sata_init(void)
{
	int t;

	/* AHB bus wrapper setup */

	/*
	// SATA_AHB2STBUS_STBUS_OPC
	// 2:0  -- 100 = Store64/Load64
	// 4    -- 1   = Enable write posting
	// DMA Read, write posting always = 0
	*/

	/* opcode = Load4 |Store 4*/
	writel(0, SATA_AHB2STBUS_STBUS_OPC);

	/*
	// SATA_AHB2STBUS_MESSAGE_SIZE_CONFIG
	// 3:0  -- 0111 = 128 Packets
	// 3:0  -- 0110 =  64 Packets
	*/
	/* WAS: Message size = 64 packet when 6 now 3*/
	writel(3, SATA_AHB2STBUS_MESSAGE_SIZE_CONFIG);

	/*
	// SATA_AHB2STBUS_CHUNK_SIZE_CONFIG
	// 3:0  -- 0110 = 64 Packets
	// 3:0  -- 0001 =  2 Packets
	*/

	/* WAS Chunk size = 2 packet when 1, now 0 */
	writel(0, SATA_AHB2STBUS_CHUNK_SIZE_CONFIG);

	/*
	// PC_GLUE_LOGIC
	// 7:0  -- 0xFF = Set as reset value, 256 STBus Clock Cycles
	// 8    -- 1  = Time out enabled
	*/
	/* time out count = 0xa0(160 dec)
	 * time out enable = 1
	 */

	writel(0x1ff, SATA_PC_GLUE_LOGIC);

	stm_phy_reset();

	writel(0x301, SATA_SCR2);  /* issue phy wake/reset */
	readl(SATA_SCR0);          /* dummy read; flush */
	udelay(400);               /* FIXME: a guess */

	writel(0x300, SATA_SCR2);  /* issue phy wake/reset */

	{
	  int timeout = 25;
	  do {
	    udelay(200000);
	    t = readl(SATA_SCR0);
	    if ((t & 0xf) != 1)
	      break;
	  } while (--timeout);
	}

	return;
}
#endif
