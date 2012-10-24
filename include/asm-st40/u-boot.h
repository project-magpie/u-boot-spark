/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 * (C) Copyright 2004-2012 STMicroelectronics.
 * Andy Sturges <andy.sturges@st.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __ASM_ST40_U_BOOT_H
#define __ASM_ST40_U_BOOT_H

typedef struct bd_info
{
	int bi_baudrate;		/* serial console baud-rate */
	unsigned long bi_ip_addr;	/* IP Address */
	unsigned long bi_arch_number;	/* unique id for this board */
	unsigned long bi_memstart;	/* start of DRAM memory */
	phys_size_t   bi_memsize;	/* size  of DRAM memory in bytes */
#ifndef CONFIG_SYS_NO_FLASH
	unsigned long bi_flashstart;	/* start of FLASH memory */
	unsigned long bi_flashsize;	/* size  of FLASH memory */
	unsigned long bi_flashoffset;	/* reserved area for startup monitor */
#endif /* CONFIG_SYS_NO_FLASH */
	unsigned long bi_devid;
	unsigned long bi_uart_frq;	/* frequency of the UART (in Hertz) */
	unsigned long bi_tmu_frq;	/* frequency of the TMU  (in Hertz) */
	unsigned long bi_ssc_frq;	/* frequency of the SSC  (in Hertz) */
} bd_t;

#endif /* __ASM_ST40_U_BOOT_H */
