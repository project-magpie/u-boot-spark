/*
 *  Copyright (c) 2006  STMicroelectronics Limited
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
 *
 * author(s): Andy Sturges (andy.sturges@st.com)
 */

#include <common.h>

#ifdef CONFIG_DRIVER_NETSTMAC

#include <command.h>
#include <asm/addrspace.h>
#include <asm/io.h>
#include <net.h>
#include <malloc.h>
#include <miiphy.h>
#include "netstmac_eth.h"

#if (CONFIG_COMMANDS & CFG_CMD_NET)

/* #define DEBUG */

#ifdef DEBUG
#define PRINTK(args...) printf(args)
#else
#define PRINTK(args...)
#endif

/* This structure is common for both receive and transmit DMA descriptors.
 * A descriptor should not be used for storing more than one frame. */
struct stmac_dma_des_t
{
	unsigned int des0;	/* Status */
	unsigned int des1;	/* Ctrl bits, Buffer 2 length, Buffer 1 length */
	void *des2;		/* Buffer 1 Address Pointer */
	void *des3;		/* Buffer 2 Address Pointer or the next Descriptor */
};

typedef struct stmac_dma_des_t stmac_dma_des;

/* Use single dma descriptors */

#define CONFIG_DMA_RX_SIZE 8
#define CONFIG_DMA_TX_SIZE 1	/* Only ever use 1 tx buffer */

static volatile stmac_dma_des *dma_tx;
static volatile stmac_dma_des *dma_rx;
static int cur_rx;
static int eth_phy_addr;
static char miidevice[] = "stmacphy";

#define MAX_ETH_FRAME_SIZE      1536
#define MAX_PAUSE_TIME (MAC_FLOW_CONTROL_PT_MASK>>MAC_FLOW_CONTROL_PT_SHIFT)

static void stmac_mii_write (int phy_addr, int reg, int value);
static unsigned int stmac_mii_read (int phy_addr, int reg);
static void stmac_set_mac_mii_cap (int full_duplex, unsigned int speed);

/* DMA structure */

struct dma_t
{
	uchar _dummy1[L1_CACHE_BYTES];
	stmac_dma_des desc_rx[CONFIG_DMA_RX_SIZE];
	stmac_dma_des desc_tx[CONFIG_DMA_TX_SIZE];
	uchar rx_buff[CONFIG_DMA_RX_SIZE * (PKTSIZE_ALIGN)];
	uchar _dummy2[L1_CACHE_BYTES];
} dma;

static uchar *rx_packets[CONFIG_DMA_RX_SIZE];

extern int stmac_default_pbl(void);

/* ----------------------------------------------------------------------------
				 Phy interface
   ---------------------------------------------------------------------------*/

#if defined(CONFIG_STMAC_STE10XP)	/* ST STE10xP */

/* STE101P phy identifier values */
#define STE100P_PHY_HI_ID       0x1c04
#define STE100P_PHY_LO_ID       0x0011

#define STE101P_PHY_HI_ID       0x0006
#define STE101P_PHY_LO_ID_REVA  0x1c51
#define STE101P_PHY_LO_ID_REVB  0x1c52

/******************************************************************************
 * IEEE Standard 802.3-2002 vendor specific registers (0x10-0x1e) STe10xP
 *****************************************************************************/
#define MII_XCIIS                0x11	/* Config info & int status register */
#define MII_XIE                  0x12	/* Interrupt enable register */
#define MII_100CTR               0x13	/* 100BaseX control register */
#define MII_XMC                  0x14	/* Mode control register */

/******************************************************************************
 * 100BaseX Auxiliary Status register defines
 *****************************************************************************/
#define XCIIS_FIFO_OVR           0x0800	/* FIFO Overrun */
#define XCIIS_SPEED              0x0200	/* Speed */
#define XCIIS_DUPLEX             0x0100	/* Duplex */
#define XCIIS_PAUSE              0x0080	/* Pause */
#define XCIIS_ANEG_INT           0x0040	/* Auto Negotiation Interrupt */
#define XCIIS_RFAULT             0x0020	/* Remote Fault Interrupt */
#define XCIIS_LDOWN              0x0010	/* Link Down Interrupt */
#define XCIIS_LCWR               0x0008	/* Link Code Word Received Interrupt */
#define XCIIS_PFAULT             0x0004	/* Parallel Detection Fault */
#define XCIIS_ANEG_PAGE          0x0002	/* Auto Negotiation Page Rec Intr */
#define XCIIS_REF_INTR           0x0001	/* Ref Interrupt */

/******************************************************************************
 * XCVR Mode Control register defines
 *****************************************************************************/
#define XMC_LDETECT              0x0800	/* Link Detect */
#define XMC_PHY_ADDR_MSK         0x00f8	/* PHY Address Mask */
#define XMC_PHY_ADDR_SHIFT       3	/* PHY Address Mask */
#define XMC_PRE_SUP              0x0002	/* Preamble Suppression */

/* MII mode */
#define MII_TSTAT_SMII  0x1000
#define MII_TSTAT_RMII  0x0800
#define MII_TSTAT_MII   0x0400

#elif defined(CONFIG_STMAC_LAN8700)	/* SMSC LAN8700 */

/* SMSC LAN8700 phy identifier values */
#define LAN8700_PHY_HI_ID       0x0007
#define LAN8700_PHY_LO_ID       0xc0c3

#define	SPECIAL_MODE_REG	0x12		/* Special Modes Register */
#define	PHY_ADDR_MSK		0x001f		/* PHY Address Mask */
#define	PHY_ADDR_SHIFT		0		/* PHY Address Mask */

#else
#error Need to define PHY
#endif


/* MII mode */
#define MII_ADVERTISE_PAUSE 0x0400	/* supports the pause command */


static int stmac_phy_negotiate (int phy_addr)
{
	uint now, tmp, status;

	status = 0;

	tmp = stmac_mii_read (phy_addr, MII_BMCR);
	tmp |= (BMCR_ANENABLE | BMCR_ANRESTART);
	stmac_mii_write (phy_addr, MII_BMCR, tmp);

	now = get_timer (0);
	while (get_timer (now) < CONFIG_STMAC_AUTONEG_TIMEOUT) {
		status = stmac_mii_read (phy_addr, MII_BMSR);
		if (status & BMSR_ANEGCOMPLETE) {
			break;
		}

		/* Restart auto-negotiation if remote fault */
		if (status & BMSR_RFAULT) {
			printf ("PHY remote fault detected\n");
			/* Restart auto-negotiation */
			printf ("PHY restarting auto-negotiation\n");
			stmac_mii_write (phy_addr, MII_BMCR,
					 BMCR_ANENABLE | BMCR_ANRESTART);
		}
	}

	if (!(status & BMSR_ANEGCOMPLETE)) {
		printf ("PHY auto-negotiate timed out\n");
	}

	if (status & BMSR_RFAULT) {
		printf ("PHY remote fault detected\n");
	}

	return (1);
}

static unsigned int stmac_phy_check_speed (int phy_addr)
{
	unsigned int status;
	int full_duplex = 0;
	int speed = 0;

	/* Read Status register */
	status = stmac_mii_read (phy_addr, MII_BMSR);

	printf ("STMAC: ");

	/* Check link status.  If 0, default to 100 Mbps. */
	if ((status & BMSR_LSTATUS) == 0) {
		printf ("*Warning* no link detected\n");
		return 1;
	} else {
		int negotiated = stmac_mii_read (phy_addr, MII_LPA);

		if (negotiated & LPA_100FULL) {
			printf ("100Mbs full duplex link detected\n");
			full_duplex = 1;
			speed = 100;
		} else if (negotiated & LPA_100HALF) {
			printf ("100Mbs half duplex link detected\n");
			full_duplex = 0;
			speed = 100;
		} else if (negotiated & LPA_10FULL) {
			printf ("10Mbs full duplex link detected\n");
			full_duplex = 1;
			speed = 10;
		} else {
			printf ("10Mbs half duplex link detected\n");
			full_duplex = 0;
			speed = 10;
		}
	}
	stmac_set_mac_mii_cap (full_duplex, speed);
	return 0;
}

/* Automatically gets and returns the PHY device */
static unsigned int stmac_phy_get_addr (void)
{
	int i, phyaddr;

	uint stmac_phy_id = 0;

	for (i = 0; i < 32; i++) {
		unsigned int id1, id2;
		phyaddr = (i + 1) % 32;

		id1 = stmac_mii_read (phyaddr, MII_PHYSID1);
		id2 = stmac_mii_read (phyaddr, MII_PHYSID2);

		/* Make sure it is a valid identifier */
#if defined(CONFIG_STMAC_STE10XP)
		if ((id1 == STE101P_PHY_HI_ID) &&
		    ((id2 == STE101P_PHY_LO_ID_REVB) ||
		     (id2 == STE101P_PHY_LO_ID_REVA))) {
			stmac_phy_id = id1;
			printf ("STMAC: STE101P found\n");
		} else if ((id1 == STE100P_PHY_HI_ID) &&
			   (id2 == STE100P_PHY_LO_ID)) {
			stmac_phy_id = id1;
			printf ("STMAC: STE100P found\n");
		}
#elif defined(CONFIG_STMAC_LAN8700)
		if ((id1 == LAN8700_PHY_HI_ID) &&
		    (id2 == LAN8700_PHY_LO_ID)) {
			stmac_phy_id = id1;
			printf ("STMAC: SMSC LAN8700 found\n");
		}
#endif	/* CONFIG_STMAC_LAN8700 */

		if (stmac_phy_id)
			return phyaddr;
	}
	return (-1);
}

static int stmac_phy_init (void)
{
	uint advertised_caps, value;

	/* Obtain the PHY's address/id */
	eth_phy_addr = stmac_phy_get_addr ();
	if (eth_phy_addr < 0)
		return -1;

	/* Now reset the PHY we just found */
	if (miiphy_reset (miidevice, eth_phy_addr)< 0) {
		PRINTK ("PHY reset failed!");
		return -1;
	}

	/* test for H/W address disagreement with the assigned address */
#if defined(CONFIG_STMAC_STE10XP)
	value = stmac_mii_read (eth_phy_addr, MII_XMC);
	value = (value & XMC_PHY_ADDR_MSK) >> XMC_PHY_ADDR_SHIFT;
#elif defined(CONFIG_STMAC_LAN8700)
	value = stmac_mii_read (eth_phy_addr, SPECIAL_MODE_REG);
	value = (value & PHY_ADDR_MSK) >> PHY_ADDR_SHIFT;
#else
#error Need to define PHY
#endif
	if (value != eth_phy_addr) {
		printf ("PHY address mismatch with hardware (hw %d != %d)\n",
			value,
			eth_phy_addr);
	}

	/* Read the ANE Advertisement register */
	advertised_caps = stmac_mii_read (eth_phy_addr, MII_ADVERTISE);

	/* Copy our capabilities from MII_BMSR to MII_ADVERTISE */
	value = stmac_mii_read (eth_phy_addr, MII_BMSR);

	/* Set the advertised capabilities */
	if (value & BMSR_100BASE4)
		advertised_caps |= ADVERTISE_100BASE4;
	if (value & BMSR_100FULL)
		advertised_caps |= ADVERTISE_100FULL;
	if (value & BMSR_100HALF)
		advertised_caps |= ADVERTISE_100HALF;
	if (value & BMSR_10FULL)
		advertised_caps |= ADVERTISE_10FULL;
	if (value & BMSR_10HALF)
		advertised_caps |= ADVERTISE_10HALF;

#ifdef CONFIG_STMAC_FLOWCTRL
	advertised_caps |= MII_ADVERTISE_PAUSE;
#else
	advertised_caps &= ~MII_ADVERTISE_PAUSE;
#endif

	/* Update our Auto-Neg Advertisement Register */
	stmac_mii_write (eth_phy_addr, MII_ADVERTISE, advertised_caps);

	stmac_phy_negotiate (eth_phy_addr);
	stmac_phy_check_speed (eth_phy_addr);

	return 0;
}


/* ----------------------------------------------------------------------------
				 MII Interface
   ---------------------------------------------------------------------------*/

static int stmac_mii_poll_busy (void)
{
	/* arm simple, non interrupt dependent timer */
	ulong now = get_timer (0);
	while (get_timer (now) < CONFIG_STMAC_MII_POLL_BUSY_DELAY) {
		if (!(STMAC_READ (MAC_MII_ADDR) & MAC_MII_ADDR_BUSY)) {
			return 1;
		}
	}
	printf ("stmac_mii_busy timeout\n");
	return (0);
}

static void stmac_mii_write (int phy_addr, int reg, int value)
{
	int mii_addr;

	/* Select register */
	mii_addr =
		((phy_addr & MAC_MII_ADDR_PHY_MASK) << MAC_MII_ADDR_PHY_SHIFT)
		| ((reg & MAC_MII_ADDR_REG_MASK) << MAC_MII_ADDR_REG_SHIFT) |
		MAC_MII_ADDR_WRITE;

	stmac_mii_poll_busy ();

	/* Set the MII address register to write */
	STMAC_WRITE (mii_addr, MAC_MII_DATA);
	STMAC_WRITE (value, MAC_MII_ADDR);

	stmac_mii_poll_busy ();

	(void) stmac_mii_read (phy_addr, reg);
}

static unsigned int stmac_mii_read (int phy_addr, int reg)
{
	int mii_addr, val;

	mii_addr =
		((phy_addr & MAC_MII_ADDR_PHY_MASK) << MAC_MII_ADDR_PHY_SHIFT)
		| ((reg & MAC_MII_ADDR_REG_MASK) << MAC_MII_ADDR_REG_SHIFT);

	/* Select register */
	stmac_mii_poll_busy ();

	STMAC_WRITE (mii_addr, MAC_MII_ADDR);

	stmac_mii_poll_busy ();

	/* Return read value */
	val = STMAC_READ (MAC_MII_DATA);
	return val;
}

/* define external interface to mii, through miiphy_register() */
static int stmac_miiphy_read (char *devname, unsigned char addr, unsigned char reg, unsigned short *value)
{
	*value = stmac_mii_read (addr, reg);
	return 0;
}

static int stmac_miiphy_write (char *devname, unsigned char addr, unsigned char reg, unsigned short value)
{
	stmac_mii_write (addr, reg, value);
	return 0;
}

/* ----------------------------------------------------------------------------
				 MAC CORE Interface
   ---------------------------------------------------------------------------*/

static void stmac_set_mac_addr (unsigned char *Addr)
{
	unsigned long data;

	data = (Addr[5] << 8) | Addr[4];
	STMAC_WRITE (data, MAC_ADDR_HIGH);
	data = (Addr[3] << 24) | (Addr[2] << 16) | (Addr[1] << 8) | Addr[0];
	STMAC_WRITE (data, MAC_ADDR_LOW);

	return;
}

static int stmac_get_mac_addr (unsigned char *addr)
{
	unsigned int hi_addr, lo_addr;

	/* Read the MAC address from the hardware */
	hi_addr = (unsigned int) STMAC_READ (MAC_ADDR_HIGH);
	lo_addr = (unsigned int) STMAC_READ (MAC_ADDR_LOW);

	if ((hi_addr == 0x0000FFFFUL) && (lo_addr == 0xFFFFFFFF))
		return 0;

	/* Extract the MAC address from the high and low words */
	addr[0] = lo_addr & 0xffu;
	addr[1] = (lo_addr >> 8) & 0xffu;
	addr[2] = (lo_addr >> 16) & 0xffu;
	addr[3] = (lo_addr >> 24) & 0xffu;
	addr[4] = hi_addr & 0xffu;
	addr[5] = (hi_addr >> 8) & 0xffu;

	return 1;
}

static void stmac_mac_enable (void)
{
	unsigned int value = (unsigned int) STMAC_READ (MAC_CONTROL);

	PRINTK ("MAC RX/TX enabled\n");

	/* set: TE (transmitter enable, bit 3), RE (receive enable, bit 2)
	   and RA (receive all mode, bit 31) */
	value |= (MAC_CONTROL_RA | MAC_CONTROL_TE | MAC_CONTROL_RE);
	STMAC_WRITE (value, MAC_CONTROL);
	return;
}

static void stmac_mac_disable (void)
{
	unsigned int value = (unsigned int) STMAC_READ (MAC_CONTROL);

	PRINTK ("%s: MAC RX/TX disabled\n", __FUNCTION__);

	value &= ~(MAC_CONTROL_RA | MAC_CONTROL_TE | MAC_CONTROL_RE);
	STMAC_WRITE (value, MAC_CONTROL);
	return;
}

static void stmac_set_rx_mode (void)
{
	unsigned int value = (unsigned int) STMAC_READ (MAC_CONTROL);

	PRINTK ("STMAC: perfect filtering mode.\n");
	value &= ~(MAC_CONTROL_PM | MAC_CONTROL_PR | MAC_CONTROL_IF |
		   MAC_CONTROL_HO | MAC_CONTROL_HP);
	STMAC_WRITE (0x0, MAC_HASH_HIGH);
	STMAC_WRITE (0x0, MAC_HASH_LOW);

	STMAC_WRITE (value, MAC_CONTROL);

	return;
}

static void stmac_set_mac_mii_cap (int full_duplex, unsigned int speed)
{
	unsigned int flow = (unsigned int) STMAC_READ (MAC_FLOW_CONTROL),
		ctrl = (unsigned int) STMAC_READ (MAC_CONTROL);

	PRINTK ("%s\n", __FUNCTION__);

	if (!(full_duplex)) {	/* Half Duplex */
		flow &= ~(MAC_FLOW_CONTROL_FCE | MAC_FLOW_CONTROL_PT_MASK |
			  MAC_FLOW_CONTROL_PCF);
		ctrl &= ~MAC_CONTROL_F;
		ctrl |= MAC_CONTROL_DRO;
	} else {		/* Full Duplex */
		flow |= MAC_FLOW_CONTROL_FCE | MAC_FLOW_CONTROL_PCF |
			(MAX_PAUSE_TIME << MAC_FLOW_CONTROL_PT_SHIFT);
		ctrl |= MAC_CONTROL_F;
		ctrl &= ~MAC_CONTROL_DRO;
	}

	STMAC_WRITE (flow, MAC_FLOW_CONTROL);
	STMAC_WRITE (ctrl, MAC_CONTROL);
#ifdef CONFIG_STMAC_STE101P_RMII
	stb7109_mac_speed (speed);
#endif
	return;
}

/* This function provides the initial setup of the MAC controller */
static void stmac_mac_core_init (void)
{
	unsigned int value = 0;

	/* Set the MAC control register with our default value */
	value = (unsigned int) STMAC_READ (MAC_CONTROL);
	value |= MAC_CONTROL_HBD | MAC_CONTROL_PM;
	STMAC_WRITE (value, MAC_CONTROL);

	/* Change the MAX_FRAME bits in the MMC control register. */
	STMAC_WRITE (((MAX_ETH_FRAME_SIZE << MMC_CONTROL_MAX_FRM_SHIFT) &
		      MMC_CONTROL_MAX_FRM_MASK), MMC_CONTROL);

	return;
}

/* ----------------------------------------------------------------------------
 *  			DESCRIPTORS functions
 * ---------------------------------------------------------------------------*/
static void display_dma_desc_ring (volatile stmac_dma_des * p, int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf ("\t%d [0x%x]: "
			"desc0=0x%x desc1=0x%x buffer1=0x%x\n", i,
			(unsigned int) &p[i].des0, p[i].des0,
			p[i].des1, (unsigned int) p[i].des2);
}

static void init_desc_owner (volatile stmac_dma_des * head, unsigned int size,
			     unsigned int owner)
{
	int i;
	volatile stmac_dma_des *p = head;

	for (i = 0; i < size; i++) {
		p->des0 = owner;
		p++;
	}
	return;
}

static void init_dma_ring (volatile stmac_dma_des * p, uchar ** phy,
			   unsigned int ring_size, unsigned int own_bit)
{
	int i;
	for (i = 0; i < ring_size; i++) {
		p->des0 = own_bit;
		p->des1 = (!(own_bit) ? 0 :
			   ((PKTSIZE_ALIGN) << DES1_RBS1_SIZE_SHIFT));
		if (i == ring_size - 1) {
			p->des1 |= DES1_CONTROL_TER;
		}
		p->des2 = (!(own_bit) ? 0 : ((void *) (PHYSADDR (phy[i]))));
		p->des3 = NULL;
		p++;
	}
	return;
}

/* Allocate and init the TX and RX descriptors rings.
 * The driver uses the 'implicit' scheme for implementing the TX/RX DMA
 * linked lists. */

static void init_dma_desc_rings (void)
{
	int i;

	PRINTK ("allocate and init the DMA RX/TX lists\n");

	/* Clean out uncached buffers */

	flush_cache ((unsigned long)&dma, sizeof (struct dma_t));

	/* Allocate memory for the DMA RX/TX buffer descriptors */
	dma_rx = (volatile stmac_dma_des *) P2SEGADDR (&dma.desc_rx[0]);
	dma_tx = (volatile stmac_dma_des *) P2SEGADDR (&dma.desc_tx[0]);

	cur_rx = 0;

	if ((dma_rx == NULL) || (dma_tx == NULL)) {
		printf ("%s:ERROR allocating the DMA Tx/Rx desc\n",
			__FUNCTION__);
		return;
	}

	for (i = 0; i < CONFIG_DMA_RX_SIZE; i++)
		rx_packets[i] = (uchar*)P2SEGADDR (dma.rx_buff + (PKTSIZE_ALIGN * i));

	init_dma_ring (dma_rx, rx_packets, CONFIG_DMA_RX_SIZE, OWN_BIT);
	init_dma_ring (dma_tx, 0, CONFIG_DMA_TX_SIZE, 0);

#ifdef DEBUG
	printf (" - RX descriptor ring:\n");
	display_dma_desc_ring (dma_rx, CONFIG_DMA_RX_SIZE);
	printf (" - TX descriptor ring:\n");
	display_dma_desc_ring (dma_tx, CONFIG_DMA_TX_SIZE);
#endif

	return;
}

/* Release and free the descriptor resources. */
static void free_dma_desc_resources (void)
{
	dma_tx = NULL;
	dma_rx = NULL;
	return;
}

/* ----------------------------------------------------------------------------
				DMA FUNCTIONS
 * ---------------------------------------------------------------------------*/

/* DMA SW reset.
 *  NOTE1: the MII_TxClk and the MII_RxClk must be active before this
 *	   SW reset otherwise the MAC core won't exit the reset state.
 *  NOTE2: after a SW reset all interrupts are disabled */

static void stmac_dma_reset (void)
{
	unsigned int value;

	value = (unsigned int) STMAC_READ (DMA_BUS_MODE);
	value |= DMA_BUS_MODE_SFT_RESET;

	STMAC_WRITE (value, DMA_BUS_MODE);

	while ((STMAC_READ (DMA_BUS_MODE) & DMA_BUS_MODE_SFT_RESET)) {
	}

	return;
}

/* START/STOP the DMA TX/RX processes */
static void stmac_dma_start_tx (void)
{
	unsigned int value;

	value = (unsigned int) STMAC_READ (DMA_CONTROL);
	value |= DMA_CONTROL_ST;
	STMAC_WRITE (value, DMA_CONTROL);

	return;
}

static void stmac_dma_stop_tx (void)
{
	unsigned int value;

	value = (unsigned int) STMAC_READ (DMA_CONTROL);
	value |= ~DMA_CONTROL_ST;
	STMAC_WRITE (value, DMA_CONTROL);

	return;
}
static void stmac_dma_start_rx (void)
{
	unsigned int value;

	value = (unsigned int) STMAC_READ (DMA_CONTROL);
	value |= DMA_CONTROL_SR;
	STMAC_WRITE (value, DMA_CONTROL);

	return;
}

static void stmac_dma_stop_rx (void)
{
	unsigned int value;

	value = (unsigned int) STMAC_READ (DMA_CONTROL);
	value |= ~DMA_CONTROL_SR;
	STMAC_WRITE (value, DMA_CONTROL);

	return;
}

static void stmac_eth_stop_tx (void)
{

	stmac_dma_stop_tx ();

	init_desc_owner (dma_tx, CONFIG_DMA_TX_SIZE, 0);

	return;
}

/* The DMA init function performs:
 * - the DMA RX/TX SW descriptors initialization
 * - the DMA HW controller initialization
 * NOTE: the DMA TX/RX processes will be started in the 'open' method. */

static int stmac_dma_init (void)
{

	PRINTK ("STb7109eth: DMA Core setup\n");

	/* DMA SW reset */
	stmac_dma_reset ();

	/* Enable Application Access by writing to DMA CSR0 */
	STMAC_WRITE (DMA_BUS_MODE_DEFAULT |
		     (stmac_default_pbl () << DMA_BUS_MODE_PBL_SHIFT),
		     DMA_BUS_MODE);

	/* Disable interrupts */
	STMAC_WRITE (0, DMA_INTR_ENA);

	/* The base address of the RX/TX descriptor */

	STMAC_WRITE (PHYSADDR (dma_tx), DMA_TX_BASE_ADDR);
	STMAC_WRITE (PHYSADDR (dma_rx), DMA_RCV_BASE_ADDR);

	return (0);
}

static int check_tx_error_summary (uint status)
{

	PRINTK ("TDES0: 0x%x\n", status);

	if (status & TDES0_STATUS_ES) {
		printf ("STMAC: DMA tx ERROR: ");

		if (status & TDES0_STATUS_UF) {
			printf ("Underflow Error\n");
			goto out_error;
		}
		if (status & TDES0_STATUS_EX_DEF) {
			printf ("Ex Deferrals\n");
			goto set_collision;
		}
		if (status & TDES0_STATUS_EX_COL) {
			printf ("Ex Collisions\n");
			goto set_collision;
		}
		if (status & TDES0_STATUS_LATE_COL) {
			printf ("Late Collision\n");
			goto set_collision;
		}
		if (status & TDES0_STATUS_NO_CARRIER) {
			printf ("No Carrier\n");
			goto out_error;
		}
		if (status & TDES0_STATUS_LOSS_CARRIER) {
			printf ("Loss of Carrier\n");
			goto out_error;
		}
	}

	return (0);

      set_collision:
      out_error:

	return (-1);
}

/* When a frame is received the status is written into TDESC0 of the descriptor
 * having the LS bit set. */
static int check_rx_error_summary (unsigned int status)
{
	if (status & RDES0_STATUS_ES) {
		printf ("STMAC: DMA rx ERROR: ");
		if (status & RDES0_STATUS_DE)
			printf ("descriptor error\n");
		if (status & RDES0_STATUS_PFE)
			printf ("partial frame error\n");
		if (status & RDES0_STATUS_RUNT_FRM)
			printf ("runt Frame\n");
		if (status & RDES0_STATUS_TL)
			printf ("frame too long\n");
		if (status & RDES0_STATUS_COL_SEEN) {
			printf ("collision seen\n");
		}
		if (status & RDES0_STATUS_CE)
			printf ("CRC Error\n");

		return (-1);
	}
	return (0);
}

static int stmac_eth_tx (volatile uchar * data, int len)
{
	volatile stmac_dma_des *p = dma_tx;
	uint now = get_timer (0);
	uint status = 0;

	while ((p->des0 & OWN_BIT)
	       && (get_timer (now) < CONFIG_STMAC_TX_TIMEOUT)) {
		;
	}

	if (p->des0 & OWN_BIT) {
		printf ("STMAC: tx timeout - no desc available\n");
		return -1;
	}

	flush_cache ((ulong) data, len);	/* Make sure data in memory */
	p->des2 = (stmac_dma_des *) PHYSADDR (data);

	/* Clean and set the descriptor 1 */

	p->des1 = ((TDES1_CONTROL_IC | TDES1_CONTROL_FS | TDES1_CONTROL_LS) |
		   ((p->des1 & DES1_CONTROL_TER) ? DES1_CONTROL_TER : 0) |
		   ((len << DES1_RBS1_SIZE_SHIFT) & DES1_RBS1_SIZE_MASK));
	p->des0 = OWN_BIT;

#ifdef DEBUG
	PRINTK ("\nSTMAC: TX (cur_tx = %d)\n", cur_tx);
	display_dma_desc_ring (dma_tx, CONFIG_DMA_TX_SIZE);
#endif

	/* CSR1 enables the transmit DMA to check for new descriptor */
	STMAC_WRITE (DMA_STATUS_TI, DMA_STATUS);
	STMAC_WRITE (1, DMA_XMT_POLL_DEMAND);

	now = get_timer (0);
	while (get_timer (now) < CONFIG_STMAC_TX_TIMEOUT) {
		status = STMAC_READ (DMA_STATUS);
		if (status & DMA_STATUS_TI)
			break;
	}
	if (!(status & DMA_STATUS_TI)) {
		printf ("STMAC: tx timeout\n");
	}
	check_tx_error_summary (status);

	return (0);
}

/* Receive function */
static void stmac_eth_rx (void)
{
	int frame_len = 0, pos;
	volatile stmac_dma_des *drx;

	pos = cur_rx;
	drx = dma_rx + pos;

	if ((pos < 0) || (pos >= CONFIG_DMA_RX_SIZE)) {
		printf ("STMAC %s: [dma drx = 0x%x, pos=%d]\n", __FUNCTION__,
			(unsigned int) drx, pos);
		display_dma_desc_ring (dma_rx, CONFIG_DMA_RX_SIZE);
	}

	if (!(drx->des0 & OWN_BIT) && (drx->des0 & RDES0_STATUS_LS)) {
		unsigned int status = drx->des0;
#ifdef  DEBUG
		PRINTK ("RX descriptor ring:\n");
		display_dma_desc_ring (dma_rx, CONFIG_DMA_RX_SIZE);
#endif

		/* Check if the frame was not successfully received */
		if (check_rx_error_summary (status) < 0) {
			drx->des0 = OWN_BIT;
		} else if ((status & RDES0_STATUS_FS)
			   && (status & RDES0_STATUS_LS)) {

			/* FL (frame length) indicates the length in byte including
			 * the CRC */
			frame_len =
				(status & RDES0_STATUS_FL_MASK) >>
				RDES0_STATUS_FL_SHIFT;

			if ((frame_len >= 0) && (frame_len <= PKTSIZE_ALIGN)) {
				memcpy ((void*)NetRxPackets[0], rx_packets[pos],
					frame_len);
				NetReceive (NetRxPackets[0], frame_len);
			} else {
				printf ("%s: Framelen %d too long\n",
					__FUNCTION__, frame_len);
			}

			drx->des0 = OWN_BIT;

#ifdef DEBUG
			PRINTK ("%s: frame received \n", __FUNCTION__);
#endif
		} else {
			printf ("%s: very long frame received\n",
				__FUNCTION__);
		}

		if (drx->des1 & DES1_CONTROL_TER)
			pos = 0;
		else
			pos++;
		/* drx = dma_rx + pos; */
		cur_rx = pos;

	} else {
		STMAC_WRITE (1, DMA_RCV_POLL_DEMAND);	/* request input */
	}
	return;
}

static int stmac_get_ethaddr (bd_t * bd)
{
	int env_size, rom_valid, env_present = 0, reg;
	char *s = NULL, *e, es[] = "11:22:33:44:55:66";
	char s_env_mac[64];
	uchar v_env_mac[6], v_rom_mac[6], *v_mac;

	env_size = getenv_r ("ethaddr", s_env_mac, sizeof (s_env_mac));
	if ((env_size > 0) && (env_size < sizeof (es))) {	/* exit if env is bad */
		printf ("\n*** ERROR: ethaddr is not set properly!!\n");
		return (-1);
	}

	if (env_size > 0) {
		env_present = 1;
		s = s_env_mac;
	}

	for (reg = 0; reg < 6; ++reg) {	/* turn string into mac value */
		v_env_mac[reg] = s ? simple_strtoul (s, &e, 16) : 0;
		if (s)
			s = (*e) ? e + 1 : e;
	}

	rom_valid = stmac_get_mac_addr (v_rom_mac);	/* get ROM mac value if any */

	if (!env_present) {	/* if NO env */
		if (rom_valid) {	/* but ROM is valid */
			v_mac = v_rom_mac;
			sprintf (s_env_mac, "%02X:%02X:%02X:%02X:%02X:%02X",
				 v_mac[0], v_mac[1], v_mac[2], v_mac[3],
				 v_mac[4], v_mac[5]);
			setenv ("ethaddr", s_env_mac);
		} else {	/* no env, bad ROM */
			printf ("\n*** ERROR: ethaddr is NOT set !!\n");
			return (-1);
		}
	} else {		/* good env, don't care ROM */
		v_mac = v_env_mac;	/* always use a good env over a ROM */
	}

	if (env_present && rom_valid) {	/* if both env and ROM are good */
		if (memcmp (v_env_mac, v_rom_mac, 6) != 0) {
			printf ("\nWarning: MAC addresses don't match:\n");
			printf ("\tHW MAC address:  "
				"%02X:%02X:%02X:%02X:%02X:%02X\n",
				v_rom_mac[0], v_rom_mac[1],
				v_rom_mac[2], v_rom_mac[3],
				v_rom_mac[4], v_rom_mac[5]);
			printf ("\t\"ethaddr\" value: "
				"%02X:%02X:%02X:%02X:%02X:%02X\n",
				v_env_mac[0], v_env_mac[1],
				v_env_mac[2], v_env_mac[3],
				v_env_mac[4], v_env_mac[5]);
		}
	}
	memcpy (bd->bi_enetaddr, v_mac, 6);	/* update global address to match env (allows env changing) */
	stmac_set_mac_addr (v_mac);	/* use old function to update default */
	printf ("Using MAC Address %02X:%02X:%02X:%02X:%02X:%02X\n", v_mac[0],
		v_mac[1], v_mac[2], v_mac[3], v_mac[4], v_mac[5]);
	return (0);
}

static int stmac_reset_eth (bd_t * bd)
{
	int err;

	err = stmac_get_ethaddr (bd);	/* set smc_mac_addr, and sync it with u-boot globals */

	if (err < 0) {
		memset (bd->bi_enetaddr, 0, 6);	/* hack to make error stick! upper code will abort if not set */
		return (-1);	/* upper code ignores this, but NOT bi_enetaddr */
	}

	if (stmac_phy_init () < 0) {
		printf ("Phy not detected\n");
		return -1;
	}

	init_dma_desc_rings ();

	stmac_mac_core_init ();
	stmac_dma_init ();

	stmac_set_rx_mode ();

	stmac_mac_enable ();

	stmac_dma_start_rx ();
	stmac_dma_start_tx ();

	STMAC_WRITE (1, DMA_RCV_POLL_DEMAND);	/* request input */

	return (0);
}

extern int eth_init (bd_t * bd)
{
	stmac_reset_eth (bd);
	return 0;
}

extern void eth_halt (void)
{
	PRINTK ("%s\n", __FUNCTION__);

	/* Reset the TX/RX processes */
	stmac_dma_stop_rx ();
	stmac_eth_stop_tx ();

	/* Disable the MAC core */
	stmac_mac_disable ();

	/* Free buffers */

	free_dma_desc_resources ();
}

/* Get a data block via Ethernet */
extern int eth_rx (void)
{
	PRINTK ("%s: status 0x%x\n", __FUNCTION__, status);
	stmac_eth_rx ();
	return 1;
}

/* Send a data block via Ethernet. */
extern int eth_send (volatile void *packet, int length)
{
	stmac_eth_tx (packet, length);
	return 1;
}

#endif /* COMMANDS & CFG_NET */

extern int stmac_miiphy_initialize(bd_t *bis)
{
#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)
	miiphy_register(miidevice, stmac_miiphy_read, stmac_miiphy_write);
#endif
	return 0;
}

#endif /* CONFIG_DRIVER_NETSTMAC */
