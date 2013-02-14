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

/* 	Generic Retime Padlogic possible modes
 * Refer to GRP Functional specs (ADCS 8198257) for more details */

/* B Mode
 * Bypass retime with optional delay */
#define	RET_BYPASS(_delay) (&(struct stm_pio_control_retime_config){ \
		.retime = 0, \
		.clk = -1, \
		.clknotdata = 0, \
		.double_edge = -1, \
		.invertclk = -1, \
		.delay = _delay, \
		})
/*
 * (R0, R1, R0D, R1D modes )
 * single-edge data non inverted clock, retime data with clk */
#define	RET_SE_NICLK_IO(_delay, _clk) (&(struct stm_pio_control_retime_config){ \
		.retime = 1, \
		.clk = _clk, \
		.clknotdata = 0, \
		.double_edge = 0, \
		.invertclk = 0, \
		.delay = _delay, \
		})

/* RIV0, RIV1, RIV0D, RIV1D modes
 * single-edge data inverted clock, retime data with clk */
#define	RET_SE_ICLK_IO(_delay, _clk) (&(struct stm_pio_control_retime_config){ \
		.retime = 1, \
		.clk = _clk, \
		.clknotdata = 0, \
		.double_edge = 0, \
		.invertclk = 1, \
		.delay = _delay, \
		})

/* R0E, R1E, R0ED, R1ED modes
 * double-edge data, retime data with clk */
#define	RET_DE_IO(_delay, _clk) (&(struct stm_pio_control_retime_config){ \
		.retime = 1, \
		.clk = _clk, \
		.clknotdata = 0, \
		.double_edge = 1, \
		.invertclk = -1, \
		.delay = _delay, \
		})

/* CIV0, CIV1 modes with inverted clock
 * Retiming the clk pins will park clock & reduce the noise within the core. */
#define RET_ICLK(_clk) (&(struct stm_pio_control_retime_config){ \
		.retime = 1, \
		.clk = _clk, \
		.clknotdata = 1, \
		.double_edge = -1, \
		.invertclk = 1, \
		.delay = 0, \
		})

/* CLK0, CLK1 modes with non-inverted clock
 * Retiming the clk pins will park clock & reduce the noise within the core. */
#define RET_NICLK(_clk) (&(struct stm_pio_control_retime_config){ \
		.retime = 1, \
		.clk = _clk, \
		.clknotdata = 1, \
		.double_edge = -1, \
		.invertclk = 0, \
		.delay = 0, \
		})

#endif	/* __ASM_ST40_PIO_CONTROL_H */

