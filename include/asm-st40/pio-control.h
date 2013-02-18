/*
 * (c) 2010-2013 STMicroelectronics Limited
 *
 * Author: Pawel Moll <pawel.moll@st.com>
 *         Sean McGoogan <Sean.McGoogan@st.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */



#ifndef __ASM_ST40_PIO_CONTROL_H
#define __ASM_ST40_PIO_CONTROL_H

struct stm_pio_control_retime_config {
	int retime:2;
	int clk:3;		/* was previously "clk1notclk0:2" */
	int clknotdata:2;
	int double_edge:2;
	int invertclk:2;
	int delay:5;		/* was previously "delay_input:3" */
};


/*
 * list of valid values for the "clk" bitfield above
 */
#define STM_RETIME_VALUE_CLK_0			(0)
#define STM_RETIME_VALUE_CLK_1			(1)
	/* if the H/W bitfield is 2-bits wide ? */
#if defined(CONFIG_ST40_STXH416)
//#define STM_RETIME_VALUE_CLK_2		(2)
//#define STM_RETIME_VALUE_CLK_3		(3)
#endif	/* H/W bitfield is 2-bits wide */

/*
 * list of valid values for the "delay" bitfield above
 */
#define STM_RETIME_VALUE_DELAY_0		(0)
#define STM_RETIME_VALUE_DELAY_1		(1)
#define STM_RETIME_VALUE_DELAY_2		(2)
#define STM_RETIME_VALUE_DELAY_3		(3)
	/* if the H/W bitfield is 4-bits wide ? */
#if defined(CONFIG_ST40_STXH416)
//#define STM_RETIME_VALUE_DELAY_4		(4)
//#define STM_RETIME_VALUE_DELAY_5		(5)
//#define STM_RETIME_VALUE_DELAY_6		(6)
//#define STM_RETIME_VALUE_DELAY_7		(7)
//#define STM_RETIME_VALUE_DELAY_8		(8)
//#define STM_RETIME_VALUE_DELAY_9		(9)
//#define STM_RETIME_VALUE_DELAY_10		(10)
//#define STM_RETIME_VALUE_DELAY_11		(11)
//#define STM_RETIME_VALUE_DELAY_12		(12)
//#define STM_RETIME_VALUE_DELAY_13		(13)
//#define STM_RETIME_VALUE_DELAY_14		(14)
//#define STM_RETIME_VALUE_DELAY_15		(15)
#endif	/* H/W bitfield is 4-bits wide */


/* 	Generic Retime Padlogic possible modes
 * Refer to GRP Functional specs (ADCS 8198257) for more details */

/* B Mode
 * Bypass retime with optional delay */
#define	RET_BYPASS(_delay) (&(struct stm_pio_control_retime_config){ \
		.retime = 0, \
		.clk = STM_RETIME_VALUE_CLK_0, \
		.clknotdata = 0, \
		.double_edge = 0, \
		.invertclk = 0, \
		.delay = STM_RETIME_VALUE_DELAY_ ## _delay, \
		})
/*
 * (R0, R1, R0D, R1D modes )
 * single-edge data non inverted clock, retime data with clk */
#define	RET_SE_NICLK_IO(_delay, _clk) (&(struct stm_pio_control_retime_config){ \
		.retime = 1, \
		.clk = STM_RETIME_VALUE_CLK_ ## _clk, \
		.clknotdata = 0, \
		.double_edge = 0, \
		.invertclk = 0, \
		.delay = STM_RETIME_VALUE_DELAY_ ## _delay, \
		})

/* RIV0, RIV1, RIV0D, RIV1D modes
 * single-edge data inverted clock, retime data with clk */
#define	RET_SE_ICLK_IO(_delay, _clk) (&(struct stm_pio_control_retime_config){ \
		.retime = 1, \
		.clk = STM_RETIME_VALUE_CLK_ ## _clk, \
		.clknotdata = 0, \
		.double_edge = 0, \
		.invertclk = 1, \
		.delay = STM_RETIME_VALUE_DELAY_ ## _delay, \
		})

/* R0E, R1E, R0ED, R1ED modes
 * double-edge data, retime data with clk */
#define	RET_DE_IO(_delay, _clk) (&(struct stm_pio_control_retime_config){ \
		.retime = 1, \
		.clk = STM_RETIME_VALUE_CLK_ ## _clk, \
		.clknotdata = 0, \
		.double_edge = 1, \
		.invertclk = 0, \
		.delay = STM_RETIME_VALUE_DELAY_ ## _delay, \
		})

/* CIV0, CIV1 modes with inverted clock
 * Retiming the clk pins will park clock & reduce the noise within the core. */
#define RET_ICLK(_clk) (&(struct stm_pio_control_retime_config){ \
		.retime = 1, \
		.clk = STM_RETIME_VALUE_CLK_ ## _clk, \
		.clknotdata = 1, \
		.double_edge = 0, \
		.invertclk = 1, \
		.delay = STM_RETIME_VALUE_DELAY_0, \
		})

/* CLK0, CLK1 modes with non-inverted clock
 * Retiming the clk pins will park clock & reduce the noise within the core. */
#define RET_NICLK(_clk) (&(struct stm_pio_control_retime_config){ \
		.retime = 1, \
		.clk = STM_RETIME_VALUE_CLK_ ## _clk, \
		.clknotdata = 1, \
		.double_edge = 0, \
		.invertclk = 0, \
		.delay = STM_RETIME_VALUE_DELAY_0, \
		})

#endif	/* __ASM_ST40_PIO_CONTROL_H */

