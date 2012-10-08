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
#include <asm/arch/hardware.h>
#include <asm/arch/pinmux.h>

void board_pre_ddrinit(void)
{
	/*
	 * Several provisions are present on board for Power Management wrt DDR
	 *
	 * There are 4 controls with the board that it can manipulate
	 *  - DDR_SHOFF_1V2 (PLGPIO 21): Shuts off DDR VDD 1V2
	 *  - DDR_SHOFF_1V5 (PLGPIO 22): Shuts off DDR VDD 1V5
	 *  - EN_DDR_CKE (PLGPIO 88): Enable DDR clock
	 *  - EN_DDR_nRST (PLGPIO 89): Provide reset to the DDR
	 */

	/*
	 * The selected configuration is as follows
	 *  - DDR_SHOFF_1V2/1V5 : Enable both supply voltages (Enable pull down)
	 *  - EN_DDR_CKE, EN_DDR_nRST : Force 0 in GPIO output mode
	 *    (Additionally, activate pull down on both signals)
	 */
	spear1340_configure_pin(21, PMX_PULLDOWN);
	spear1340_configure_pin(22, PMX_PULLDOWN);

	spear1340_configure_pin(88, PMX_GPIO);
	spear1340_configure_pin(89, PMX_GPIO);
	spear1340_plgpio_set(88, 0);
	spear1340_plgpio_set(89, 0);

	/* Activate PULL DOWN on both EN_DDR_CKE and EN_DDR_nRST */
	spear1340_configure_pin(88, PMX_PULLDOWN);
	spear1340_configure_pin(89, PMX_PULLDOWN);
}
