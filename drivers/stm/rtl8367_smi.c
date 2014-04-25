/*
 * Realtek RTL8367 SMI interface driver
 *
 * Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2013 STMicroelectronics.
 *	Sean McGoogan <Sean.McGoogan@st.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * It should be noted that some of the information included in this file was
 * provided from OpenWrt (https://dev.openwrt.org), and some information from
 * a Realtek document entitled "Switch Controller Register Table" (Rev. 1.0.2,
 * Feb 17th 2011), whose filename was "RTL8363SB_Register_Table(GPL).pdf".
 *			Sean McGoogan <Sean.McGoogan@st.com> 2013-12-10.
 */


#include <common.h>
#include <malloc.h>
#include <linux/ctype.h>
#include <asm/errno.h>
#include <stm/soc.h>
#include <stm/socregs.h>
#include <asm/io.h>
#include <stm/pio.h>
#include "rtl8367_smi.h"


#define BIT(n)					(1ul << (n))

#define RTL8367_PHY_REG_MAX			31		/* 0 … 31 */
#define RTL8367_PHY_ADDR_MAX			7		/* 0 … 7 */
#define RTL8367_NUM_PORTS			(RTL8367_PHY_ADDR_MAX+1)

#define RTL8367_SMI_ACK_RETRY_COUNT		5

#define RTL8367_MIB_COUNTER_REG(n)		(0x1000 + (n))
#define RTL8367_MIB_ADDRESS_REG			0x1004
#define RTL8367_MIB_CTRL_REG			0x1005
#define RTL8367_MIB_CTRL_BUSY_FLAG		BIT(0)
#define RTL8367_MIB_CTRL_RESET_FLAG		BIT(1)
#define RTL8367_MIB_CTRL_GLOBAL_RESET_MASK	BIT(11)
#define RTL8367_MIB_COUNT			31	/* 0 … 30 */

#define RTL8367_CHIP_RESET_REG			0x1322
#define RTL8367_RESET_DELAY			(1000+500)	/* msecs */

#define ndelay(x)				udelay(1)

#define dev_err(fmt, ...)

typedef enum {
	FALSE = 0,
	TRUE = 1
} bool_t;


	/*******************************************************/


#define REG_RD(_smi, _reg, _data)					\
do {									\
	int result = rtl8367_smi_read_reg( (_smi), (_reg), (_data) );	\
	if (result != 0) {						\
		printf("ERROR: result = %u, at %s:%u\n",		\
			result, __FILE__, __LINE__);			\
		return result;	/* error */				\
	}								\
} while (0)

#define REG_WR(_smi, _reg, _data)					\
do {									\
	int result = rtl8367_smi_write_reg( (_smi), (_reg), (_data) );	\
	if (result != 0) {						\
		printf("ERROR: result = %u, at %s:%u\n",		\
			result, __FILE__, __LINE__);			\
		return result;	/* error */				\
	}								\
} while (0)

#define SMI_READ(_smi, _reg, _expected)					\
do {									\
	unsigned int data;						\
	REG_RD( (_smi), (_reg), &data );				\
	if (data != (_expected)) {					\
		printf("ERROR: 0x%04x != 0x%04x, at %s:%u\n",		\
			(_expected), data, __FILE__, __LINE__);		\
		return -1;	/* error */				\
	}								\
} while(0)

#define SMI_WRITE(_smi, _reg, _data)					\
do {									\
	REG_WR( (_smi), (_reg), (_data) );				\
		/* also, add a delay, if we are performing a reset! */	\
	if ((_reg) == RTL8367_CHIP_RESET_REG)				\
		udelay(RTL8367_RESET_DELAY*1000);			\
} while(0)


	/*******************************************************/


#define stm_gpio(PAIR)		stm_gpio2(PAIR)
#define stm_gpio2(port,bit)	( ((port)<<4) + (bit) )

static void gpio_set_value(unsigned int pio, unsigned int value)
{
	if (pio == stm_gpio(CONFIG_STMAC_RTL8367_SMI_SDA))
		STPIO_SET_PIN2(CONFIG_STMAC_RTL8367_SMI_SDA,value);
	else
		STPIO_SET_PIN2(CONFIG_STMAC_RTL8367_SMI_SCK,value);
}

static unsigned int gpio_get_value(unsigned int pio)
{
	if (pio == stm_gpio(CONFIG_STMAC_RTL8367_SMI_SDA))
		return STPIO_GET_PIN2(CONFIG_STMAC_RTL8367_SMI_SDA);
	else
		return STPIO_GET_PIN2(CONFIG_STMAC_RTL8367_SMI_SCK);
}

static void gpio_direction_input(unsigned int pio)
{
	if (pio == stm_gpio(CONFIG_STMAC_RTL8367_SMI_SDA))
		SET_PIO_PIN2(CONFIG_STMAC_RTL8367_SMI_SDA, STPIO_BIDIR_Z1);
	else
		SET_PIO_PIN2(CONFIG_STMAC_RTL8367_SMI_SCK, STPIO_BIDIR_Z1);
}

static void gpio_direction_output(unsigned int pio, unsigned int value)
{
	if (pio == stm_gpio(CONFIG_STMAC_RTL8367_SMI_SDA))
		SET_PIO_PIN2(CONFIG_STMAC_RTL8367_SMI_SDA, STPIO_OUT);
	else
		SET_PIO_PIN2(CONFIG_STMAC_RTL8367_SMI_SCK, STPIO_OUT);
	/* make sure we also set the required driven value! */
	gpio_set_value(pio, value);
}


	/*******************************************************/


static inline void rtl8367_smi_clk_delay(struct rtl8367_smi *smi)
{
	ndelay(smi->clk_delay);
}

static void rtl8367_smi_start(struct rtl8367_smi *smi)
{
	unsigned int sda = smi->gpio_sda;
	unsigned int sck = smi->gpio_sck;

	/*
	 * Set GPIO pins to output mode, with initial state:
	 * SCK = 0, SDA = 1
	 */
	gpio_direction_output(sck, 0);
	gpio_direction_output(sda, 1);
	rtl8367_smi_clk_delay(smi);

	/* CLK 1: 0 -> 1, 1 -> 0 */
	gpio_set_value(sck, 1);
	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8367_smi_clk_delay(smi);

	/* CLK 2: */
	gpio_set_value(sck, 1);
	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sda, 0);
	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sda, 1);
}

static void rtl8367_smi_stop(struct rtl8367_smi *smi)
{
	unsigned int sda = smi->gpio_sda;
	unsigned int sck = smi->gpio_sck;

	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sda, 0);
	gpio_set_value(sck, 1);
	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sda, 1);
	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sck, 1);
	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sck, 1);

	/* add a click */
	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sck, 0);
	rtl8367_smi_clk_delay(smi);
	gpio_set_value(sck, 1);

	/* set GPIO pins to input mode */
	gpio_direction_input(sda);
	gpio_direction_input(sck);
}

static void rtl8367_smi_write_bits(struct rtl8367_smi *smi, u32 data, u32 len)
{
	unsigned int sda = smi->gpio_sda;
	unsigned int sck = smi->gpio_sck;

	for (; len > 0; len--) {
		rtl8367_smi_clk_delay(smi);

		/* prepare data */
		gpio_set_value(sda, !!(data & ( 1 << (len - 1))));
		rtl8367_smi_clk_delay(smi);

		/* clocking */
		gpio_set_value(sck, 1);
		rtl8367_smi_clk_delay(smi);
		gpio_set_value(sck, 0);
	}
}

static void rtl8367_smi_read_bits(struct rtl8367_smi *smi, u32 len, u32 *data)
{
	unsigned int sda = smi->gpio_sda;
	unsigned int sck = smi->gpio_sck;

	gpio_direction_input(sda);

	for (*data = 0; len > 0; len--) {
		u32 u;

		rtl8367_smi_clk_delay(smi);

		/* clocking */
		gpio_set_value(sck, 1);
		rtl8367_smi_clk_delay(smi);
		u = !!gpio_get_value(sda);
		gpio_set_value(sck, 0);

		*data |= (u << (len - 1));
	}

	gpio_direction_output(sda, 0);
}

static int rtl8367_smi_wait_for_ack(struct rtl8367_smi *smi)
{
	int retry_cnt;

	retry_cnt = 0;
	do {
		u32 ack;

		rtl8367_smi_read_bits(smi, 1, &ack);
		if (ack == 0)
			break;

		if (++retry_cnt > RTL8367_SMI_ACK_RETRY_COUNT) {
			dev_err(smi->parent, "ACK timeout\n");
			return -ETIMEDOUT;
		}
	} while (1);

	return 0;
}

static int rtl8367_smi_write_byte(struct rtl8367_smi *smi, u8 data)
{
	rtl8367_smi_write_bits(smi, data, 8);
	return rtl8367_smi_wait_for_ack(smi);
}

static inline int rtl8367_smi_write_byte_noack(struct rtl8367_smi *smi, u8 data)
{
	rtl8367_smi_write_bits(smi, data, 8);
	return 0;
}

static inline int rtl8367_smi_read_byte0(struct rtl8367_smi *smi, u8 *data)
{
	u32 t;

	/* read data */
	rtl8367_smi_read_bits(smi, 8, &t);
	*data = (t & 0xff);

	/* send an ACK */
	rtl8367_smi_write_bits(smi, 0x00, 1);

	return 0;
}

static inline int rtl8367_smi_read_byte1(struct rtl8367_smi *smi, u8 *data)
{
	u32 t;

	/* read data */
	rtl8367_smi_read_bits(smi, 8, &t);
	*data = (t & 0xff);

	/* send an ACK */
	rtl8367_smi_write_bits(smi, 0x01, 1);

	return 0;
}


	/*******************************************************/


extern int rtl8367_smi_read_reg(struct rtl8367_smi *smi, u32 addr, u32 * data)
{
	u8 lo = 0;
	u8 hi = 0;
	int ret;

	rtl8367_smi_start(smi);

	/* send READ command */
	ret = rtl8367_smi_write_byte(smi, smi->cmd_read);
	if (ret)
		goto out;

	/* set ADDR[7:0] */
	ret = rtl8367_smi_write_byte(smi, addr & 0xff);
	if (ret)
		goto out;

	/* set ADDR[15:8] */
	ret = rtl8367_smi_write_byte(smi, addr >> 8);
	if (ret)
		goto out;

	/* read DATA[7:0] */
	rtl8367_smi_read_byte0(smi, &lo);
	/* read DATA[15:8] */
	rtl8367_smi_read_byte1(smi, &hi);

	*data = ((u32) lo) | (((u32) hi) << 8);

	ret = 0;

 out:
	rtl8367_smi_stop(smi);

	return ret;
}

static inline int __rtl8367_smi_write_reg(struct rtl8367_smi *smi,
				   u32 addr, u32 data, bool_t ack)
{
	int ret;

	rtl8367_smi_start(smi);

	/* send WRITE command */
	ret = rtl8367_smi_write_byte(smi, smi->cmd_write);
	if (ret)
		goto out;

	/* set ADDR[7:0] */
	ret = rtl8367_smi_write_byte(smi, addr & 0xff);
	if (ret)
		goto out;

	/* set ADDR[15:8] */
	ret = rtl8367_smi_write_byte(smi, addr >> 8);
	if (ret)
		goto out;

	/* write DATA[7:0] */
	ret = rtl8367_smi_write_byte(smi, data & 0xff);
	if (ret)
		goto out;

	/* write DATA[15:8] */
	if (ack)
		ret = rtl8367_smi_write_byte(smi, data >> 8);
	else
		ret = rtl8367_smi_write_byte_noack(smi, data >> 8);
	if (ret)
		goto out;

	ret = 0;

 out:
	rtl8367_smi_stop(smi);

	return ret;
}

extern int rtl8367_smi_write_reg(struct rtl8367_smi *smi, u32 addr, u32 data)
{
	return __rtl8367_smi_write_reg(smi, addr, data, TRUE);
}

#if 0
extern int rtl8367_smi_write_reg_noack(struct rtl8367_smi *smi, u32 addr, u32 data)
{
	return __rtl8367_smi_write_reg(smi, addr, data, FALSE);
}
#endif

#if 0
extern int rtl8367_smi_rmwr(struct rtl8367_smi *smi, u32 addr, u32 mask, u32 data)
{
	u32 t;
	int err;

	err = rtl8367_smi_read_reg(smi, addr, &t);
	if (err)
		return err;

	err = rtl8367_smi_write_reg(smi, addr, (t & ~mask) | data);
	return err;
}
#endif


	/*******************************************************/


extern int rtl8367_smi_phy_read_reg(
	struct rtl8367_smi * const smi,
	const unsigned int phy_addr,
	const unsigned int phy_reg,
	unsigned int * const data)
{
	/* just do some bounds checking … */
	if (phy_addr > RTL8367_PHY_ADDR_MAX)
		return -1;	/* out of range */
	if (phy_reg > RTL8367_PHY_REG_MAX)
		return -1;	/* out of range */

	/* ensure the switch is ready - or return */
	REG_RD(smi, 0x1f01, data);

	/* send the address */
	REG_WR(smi, 0x1f02, 0x2000 + (phy_addr<<5) + phy_reg);

	/* send the read command */
	REG_WR(smi, 0x1f00, 0x0001);

	/* ensure the switch is ready - or return */
	REG_RD(smi, 0x1f01, data);

	/* finally, read + return the PHY register */
	REG_RD(smi, 0x1f04, data);

	return 0;	/* success */
}


	/*******************************************************/


#if defined(CONFIG_STM_RTL8367_MIB_SUPPORT)
static const struct rtl8367_mib_counter rtl8367_mib_counters[] = {
	{ 0,  0, 4, "IfInOctets"				},
	{ 0,  4, 2, "Dot3StatsFCSErrors"			},
	{ 0,  6, 2, "Dot3StatsSymbolErrors"			},
	{ 0,  8, 2, "Dot3InPauseFrames"				},
	{ 0, 10, 2, "Dot3ControlInUnknownOpcodes"		},
	{ 0, 12, 2, "EtherStatsFragments"			},
	{ 0, 14, 2, "EtherStatsJabbers"				},
	{ 0, 16, 2, "IfInUcastPkts"				},
	{ 0, 18, 2, "EtherStatsDropEvents"			},
	{ 0, 20, 2, "IfInMulticastPkts"				},
	{ 0, 22, 2, "IfInBroadcastPkts"				},

	{ 0, 40, 4, "EtherStatsOctets"				},
	{ 0, 44, 2, "EtherStatsUnderSizePkts"			},
	{ 0, 46, 2, "EtherOversizeStats"			},
	{ 0, 48, 2, "EtherStatsPkts64Octets"			},
	{ 0, 50, 2, "EtherStatsPkts65to127Octets"		},
	{ 0, 52, 2, "EtherStatsPkts128to255Octets"		},
	{ 0, 54, 2, "EtherStatsPkts256to511Octets"		},
	{ 0, 56, 2, "EtherStatsPkts512to1023Octets"		},
	{ 0, 58, 2, "EtherStatsPkts1024to1518Octets"		},
	{ 0, 60, 4, "IfOutOctets"				},
	{ 0, 64, 2, "Dot3StatsSingleCollisionFrames"		},
	{ 0, 66, 2, "Dot3StatMultipleCollisionFrames"		},
	{ 0, 68, 2, "Dot3sDeferredTransmissions"		},
	{ 0, 70, 2, "Dot3StatsLateCollisions"			},
	{ 0, 72, 2, "EtherStatsCollisions"			},
	{ 0, 74, 2, "Dot3StatsExcessiveCollisions"		},
	{ 0, 76, 2, "Dot3OutPauseFrames"			},
	{ 0, 78, 2, "IfOutDiscards"				},
	{ 0, 80, 2, "Dot1dTpPortInDiscards"			},
	{ 0, 82, 2, "IfOutUcastPkts"				},
	{ 0, 84, 2, "IfOutMulticastPkts"			},
	{ 0, 86, 2, "IfOutBroadcastPkts"			},
	{ 0, 88, 2, "OutOampduPkts"				},
	{ 0, 90, 2, "InOampduPkts"				},
	{ 0, 0,  0, NULL	/* must be last */		}
};

extern int rtl8367_reset_mib_counters(
	struct rtl8367_smi * const smi)
{
	u32 data;

	REG_RD(smi, RTL8367_MIB_CTRL_REG, &data);
	data |= RTL8367_MIB_CTRL_GLOBAL_RESET_MASK;
	REG_WR(smi, RTL8367_MIB_CTRL_REG, data);

	return 0;	/* success */
}

extern int rtl8367_get_mib_counter(
	struct rtl8367_smi * const smi,
	const int counter,
	const int port,
	unsigned long long * const val)
{
	unsigned long long mibvalue;
	u32 addr, data;
	int i;

	/* sanity check on incoming parameters */
	if (port >= RTL8367_NUM_PORTS)
		return -1;	/* error */
	if (counter >= RTL8367_MIB_COUNT)
		return -1;	/* error */

	/* Compute the address of the "watched" register */
	addr = (port * RTL8367_MIB_COUNT) + counter;

	/* Write the MIB Address Register (with register to be "watched") */
	REG_WR(smi, RTL8367_MIB_ADDRESS_REG, addr);

	/* Read the MIB Control Register */
	REG_RD(smi, RTL8367_MIB_CTRL_REG, &data);

	/* Ensure it is not "busy", nor "resetting" */
	if (data & (RTL8367_MIB_CTRL_BUSY_FLAG|RTL8367_MIB_CTRL_RESET_FLAG))
	{
		printf("ERROR in %s(), data=0x%x\n", __FUNCTION__, data);
		return -1;	/* error */
	}

	/* Read the 64-bit data from the MIB register */
	mibvalue = 0;
	for (i = 0; i < 4; i++)
	{
		REG_RD(smi, RTL8367_MIB_COUNTER_REG(3 - i), &data);
		mibvalue = (mibvalue << 16) | (data & 0xffffu);
	}

	/* Update the caller's variable */
	*val = mibvalue;

	return 0;	/* success */
}

extern void rtl8367_dump_mib_counters(
	struct rtl8367_smi * const smi)
{
	int counter, port, result;
	const struct rtl8367_mib_counter * mib;
	union {
		unsigned long long	ll;
		unsigned long		a[2];
	} data;
	#define fmt2 "MIB-%02u: [%2u,%u]  %-31s  %ld\n"
	#define fmt4 "MIB-%02u: [%2u,%u]  %-31s  %lld\n"

	for(port=0; port < RTL8367_NUM_PORTS; port++)
	{
		if ( (port >= 1) && (port <= 4) )
			continue;	/* Skip ports #1, #2, #3, and #4 */

		printf("\n\n--------  Port #%u  --------\n", port);

		for(mib=rtl8367_mib_counters; mib->length; mib++)
		{
			counter = mib->offset >> 2;
			result = rtl8367_get_mib_counter(smi, counter, port, &data.ll);

			if (result)
			{
				printf("Error: rtl8367_get_mib_counter() returned %d\n", result);
				continue;
			}

			if (data.ll)	/* counter is not ZERO ? */
			{
				if (mib->length == 4)			/* a 64-bit counter ? */
				{
					printf(fmt4,
						counter, mib->offset, mib->length, mib->name, data.ll);
				}
				else					/* else, only a 32-bit counter */
				{
					if (mib->offset%4==0)		/* bottom-half ? */
					{
						if (data.a[0])
							printf(fmt2,
								counter,
								mib->offset,
								mib->length,
								mib->name,
								data.a[0]);
							/* already have next value ? */
						if (mib[0].offset + 2 == mib[1].offset)
							mib++;		/* neat optimization! */
					}
					if (mib->offset%4==2)		/* top-half ? */
					{
						if (data.a[1])
							printf(fmt2,
								counter,
								mib->offset,
								mib->length,
								mib->name,
								data.a[1]);
					}
				}
			}
		}
	}
}
#endif	/* CONFIG_STM_RTL8367_MIB_SUPPORT */


	/*******************************************************/


	/*
	 * Initialize the rtl8367_smi data structure
	 */
extern int rtl8367_smi_init(struct rtl8367_smi * const smi)
{
	/* specify the 2 GPIO pins for the (I2C-type) SMI */
	smi->gpio_sda = stm_gpio(CONFIG_STMAC_RTL8367_SMI_SDA);
	smi->gpio_sck = stm_gpio(CONFIG_STMAC_RTL8367_SMI_SCK);

	smi->clk_delay = 1500;		/* nsecs */
	smi->cmd_read  = 0xb9;
	smi->cmd_write = 0xb8;

	smi->fw_loaded = 0;		/* firmware NOT loaded yet */

	return 0;			/* success */
}


	/*******************************************************/


	/*
	 * Initialize the "firmware" in the RTL8367 switch.
	 * This is an expensive (time-wise) operation,
	 * so we only want to do this ONCE (per boot).
	 *
	 * NOTE: Please see "rtl8367_smi_fw.sed" for details
	 * on how to re-generate the "rtl8367_smi_fw.c" file.
	 *
	 * NOTE: we will exit with a non-zero status if *any*
	 * of the SMI accesses return any error condition!
	 */
extern int rtl8367_smi_init_fw(struct rtl8367_smi * const smi)
{
	if (smi->fw_loaded)	/* already done it? */
		return 0;	/* success */

#include "rtl8367_smi_fw.c"	/* perform all the programming! */

	smi->fw_loaded = 1;	/* do not do this again! */

	return 0;		/* all done - success! */
}


