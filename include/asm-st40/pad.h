/*
 * (C) Copyright 2008-2012 STMicroelectronics.
 *
 * Sean McGoogan <Sean.McGoogan@st.com>
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



#ifndef __ASM_ST40_PAD_H
#define __ASM_ST40_PAD_H


enum stm_pad_gpio_direction {
	stm_pad_direction_unknown,		/* oe=?, pu=?, od=? */
	stm_pad_direction_ignored,		/* oe=?, pu=?, od=? */
	stm_pad_direction_input,		/* oe=0, pu=0, od=0 */
	stm_pad_direction_input_with_pullup,	/* oe=0, pu=1, od=0 */
	stm_pad_direction_output,		/* oe=1, pu=0, od=0 */
	stm_pad_direction_bidir_no_pullup,	/* oe=1, pu=0, od=1 */
	stm_pad_direction_bidir_with_pullup	/* oe=1, pu=1, od=1 */
};


struct stm_gmac_pin {
	struct {
		const unsigned char port, pin;
		      unsigned char alt;
	} pio;
	const char phy_clock:1;
	const char tx_clock:1;
	const char txer:1;
	enum stm_pad_gpio_direction direction;
	const struct stm_pio_control_retime_config * const retime;
};


#define stm_pad_set_pio_ignored(_pin)				\
	do {							\
		(_pin)->direction = stm_pad_direction_ignored;	\
	} while(0)

#define stm_pad_set_pio_out(_pin, _func)			\
	do {							\
		(_pin)->direction = stm_pad_direction_output;	\
		(_pin)->pio.alt = (_func);			\
	} while(0)

#define stm_pad_set_pio_in(_pin, _func)				\
	do {							\
		(_pin)->direction = stm_pad_direction_input;	\
		(_pin)->pio.alt = (_func);			\
	} while(0)


/*
 * Find first pin which is tagged as being a "PHY CLOCK", and return it.
 * Otherwise return NULL, if none found!
 */
static inline struct stm_gmac_pin * stm_gmac_find_phy_clock(
	struct stm_gmac_pin * const array,
	const size_t count)
{
	size_t i;

	for(i=0; i<count; i++)
	{
		if (array[i].phy_clock)
			return &array[i];	/* found it */
	}

	BUG();
	return NULL;				/* not found! */
}


/*
 * Find first pin which is tagged as being a "TXER", and return it.
 * Otherwise return NULL, if none found!
 */
static inline struct stm_gmac_pin * stm_gmac_find_txer(
	struct stm_gmac_pin * const array,
	const size_t count)
{
	size_t i;

	for(i=0; i<count; i++)
	{
		if (array[i].txer)
			return &array[i];	/* found it */
	}

	BUG();
	return NULL;				/* not found! */
}

/*
 * Find first pin which is tagged as being a "TX_CLOCK", and return it.
 * Otherwise return NULL, if none found!
 */
static inline struct stm_gmac_pin * stm_gmac_find_tx_clock(
	struct stm_gmac_pin * const array,
	const size_t count)
{
	size_t i;

	for(i=0; i<count; i++)
	{
		if (array[i].tx_clock)
			return &array[i];	/* found it */
	}

	BUG();
	return NULL;				/* not found! */
}

#endif	/* __ASM_ST40_PIO_PAD_H */


