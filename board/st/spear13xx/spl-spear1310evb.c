/*
 * (C) Copyright 2012
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
#include <asm/arch/pinmux.h>

void board_pre_ddrinit(void)
{
	/*
	 * Several provisions are present on board for Power Management wrt DDR
	 *
	 * There are 2 controls with the board that it can manipulate
	 *  - PLGPIO 145: Controls DDR_CKE
	 *  - PLGPIO 146: Controls DDR_nRESET
	 */

	/*
	 * The selected configuration is as follows
	 *  - DDR_nRESET, DDR_CKE : Force 0 in GPIO output mode
	 *    (Additionally, activate pull down on both signals)
	 */
	spear1310_configure_pin(145, PMX_GPIO);
	spear1310_configure_pin(146, PMX_GPIO);
	spear1310_plgpio_set(145, 0);
	spear1310_plgpio_set(146, 0);

	/* Activate PULL DOWN on both EN_DDR_CKE and EN_DDR_nRST */
	spear1310_configure_pin(145, PMX_PULLDOWN);
	spear1310_configure_pin(146, PMX_PULLDOWN);
}
