/*
 * (C) Copyright 2002-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

#ifndef	__ARCH_ST40_INCLUDE_ASM_GLOBAL_DATA_H
#define __ARCH_ST40_INCLUDE_ASM_GLOBAL_DATA_H


/*
 * The following data structure is placed in some memory which is
 * available very early after boot (like DPRAM on MPC8xx/MPC82xx, or
 * some locked parts of the data cache) to allow for a minimum set of
 * global variables during system initialization (until we have set
 * up the memory controller so that we can use RAM).
 */

typedef struct global_data
{
	bd_t		*bd;
	unsigned long	flags;
	unsigned long	baudrate;
	unsigned long	have_console;	/* serial_init() was called */
	phys_size_t	ram_size;	/* RAM size */
	unsigned long	env_addr;	/* Address  of Environment struct */
	unsigned long	env_valid;	/* Checksum of Environment valid? */
	void		**jt;		/* jump table */
	char		env_buf[32];	/* buffer for getenv() before reloc. */
	unsigned long	stm_devid;
	unsigned long	stm_uart_frq;	/* frequency of the UART (in Hertz) */
	unsigned long	stm_tmu_frq;	/* frequency of the TMU  (in Hertz) */
	unsigned long	stm_ssc_frq;	/* frequency of the SSC  (in Hertz) */
} gd_t;

#include <asm-generic/global_data_flags.h>

#if 0
	/* use at file-scope, but only when "gd" is referenced */
#	define DECLARE_GLOBAL_DATA_PTR		register gd_t *gd asm ("r13")
#else
	/* use globally in *all* source files */
#	define DECLARE_GLOBAL_DATA_PTR	/* nothing */
	/* volatile */ register gd_t *gd asm ("r13");
#endif

#endif /* __ARCH_ST40_INCLUDE_ASM_GLOBAL_DATA_H */
