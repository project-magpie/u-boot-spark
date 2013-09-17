/*
 * (C) Copyright 2004-2013 STMicroelectronics.
 *
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

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <stdio_dev.h>
#include <version.h>
#include <timestamp.h>
#include <net.h>
#include <environment.h>
#if defined(CONFIG_CMD_NAND)
#include <nand.h>
#endif
#if defined(CONFIG_SPI)
#include <spi.h>
#endif
#include <stm/socregs.h>
#include <stm/stxxxxxreg.h>
#include <asm/addrspace.h>
#include <stm/soc.h>


DECLARE_GLOBAL_DATA_PTR;


extern ulong __uboot_bin_size;	/* linker symbol defined in arch/st40/cpu/u-boot.lds */
ulong monitor_flash_len = (ulong)&__uboot_bin_size;


#ifndef CONFIG_IDENT_STRING
#define CONFIG_IDENT_STRING ""
#endif

const char version_string[] =
	U_BOOT_VERSION" (" U_BOOT_DATE " - " U_BOOT_TIME ") - " CONFIG_IDENT_STRING ;


extern int board_init (void);   /* Set up board             */
extern int timer_init (void);
extern int checkboard (void);   /* Give info about board    */


static int init_func_ram (void)
{
#ifdef	CONFIG_BOARD_TYPES
	int board_type = gd->board_type;
#endif

	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;
	puts ("DRAM:  ");
	print_size (gd->ram_size, "\n");

	return (0);
}

static int display_banner (void)
{

	printf ("\n\n%s\n\n", version_string);
	return (0);
}

#ifndef CONFIG_SYS_NO_FLASH
static void display_flash_config (ulong size)
{
	puts ("NOR:   ");
	print_size (size, "\n");
}
#endif /* CONFIG_SYS_NO_FLASH */


static int init_baudrate (void)
{
	char tmp[64];		/* long enough for environment variables */
	int i = getenv_f("baudrate", tmp, sizeof(tmp));

	gd->baudrate = (i > 0)
		? (int) simple_strtoul (tmp, NULL, 10)
		: CONFIG_BAUDRATE;

	return (0);
}

void flashWriteEnable(void);

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependend #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t) (void);

init_fnc_t *init_sequence[] = {
	arch_cpu_init,		/* basic arch cpu dependent setup */
	timer_init,
	board_init,
	env_init,		/* initialize environment */
	init_baudrate,		/* initialze baudrate settings */
	serial_init,		/* serial communications setup */
	console_init_f,		/* Initial console             */
	checkboard,
	display_banner,		/* say that we are here */
	init_func_ram,
	NULL,
};


void start_st40_boot (void)
{
	bd_t *bd;
	ulong addr;
	init_fnc_t **init_fnc_ptr;
#ifndef CONFIG_SYS_NO_FLASH
	ulong size;
#endif /* CONFIG_SYS_NO_FLASH */

	char *s;

	addr = CONFIG_SYS_TEXT_BASE;
	/* Reserve memory for malloc() arena. */
	addr -= CONFIG_SYS_MALLOC_LEN;
	/* (permanently) allocate a Board Info struct
	 * and a permanent copy of the "global" data
	 */
	addr -= sizeof (gd_t);
	gd = (gd_t *) addr;
	memset ((void *) gd, 0, sizeof (gd_t));
	addr -= sizeof (bd_t);
	bd = (bd_t *) addr;
	gd->bd = bd;

	/*
	 * Call all the functions in init_sequence[] in turn ...
	 */
	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr) () != 0) {
			hang ();
		}
	}

	gd->flags |= GD_FLG_RELOC;	/* tell others: relocation done */

	/* configure available FLASH banks */
	flashWriteEnable();
#ifndef CONFIG_SYS_NO_FLASH
	size = flash_init ();
	display_flash_config (size);
#endif /* CONFIG_SYS_NO_FLASH */

	bd = gd->bd;
	bd->bi_memstart = CONFIG_SYS_SDRAM_BASE;	/* start of  DRAM memory */
	bd->bi_memsize = gd->ram_size;	/* size  of  DRAM memory in bytes */
	bd->bi_baudrate = gd->baudrate;	/* Console Baudrate */
#ifndef CONFIG_SYS_NO_FLASH
	bd->bi_flashstart = CONFIG_SYS_FLASH_BASE;
	bd->bi_flashsize = size;
#if CONFIG_SYS_MONITOR_BASE == CONFIG_SYS_FLASH_BASE
	bd->bi_flashoffset = monitor_flash_len;	/* reserved area for U-Boot */
#else
	bd->bi_flashoffset = 0;
#endif
#endif /* CONFIG_SYS_NO_FLASH */

	/* initialize malloc() area */
	mem_malloc_init(CONFIG_SYS_TEXT_BASE - CONFIG_SYS_MALLOC_LEN,
		CONFIG_SYS_MALLOC_LEN);

#if defined(CONFIG_CMD_NAND)
	puts ("NAND:  ");
	nand_init ();		/* go init the NAND */
#endif

#if defined(CONFIG_SPI)
	puts ("SPI:  ");
	spi_init ();		/* go init the SPI */
#if defined(CONFIG_ENV_IS_IN_EEPROM) && !defined(CONFIG_SYS_BOOT_FROM_SPI)
	env_init_after_spi_done ();
#endif
#endif	/* defined(CONFIG_SPI) */

	/* Allocate environment function pointers etc. */
	env_relocate ();

	/* IP Address */
	bd->bi_ip_addr = getenv_IPaddr ("ipaddr");

#if defined(CONFIG_PCI)
	/*
	 * Do pci configuration
	 */
	pci_init ();
#endif

/** leave this here (after malloc(), environment and PCI are working) **/
	/* Initialize stdio devices */
	stdio_init ();

	jumptable_init ();

	/* Initialize the console (after the relocation and devices init) */
	console_init_r ();

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

	/* Initialize from environment */
	if ((s = getenv ("loadaddr")) != NULL) {
		load_addr = simple_strtoul (s, NULL, 16);
	}
#if defined(CONFIG_CMD_NET)
	if ((s = getenv ("bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof (BootFile));
	}
#endif /* CONFIG_CMD_NET */

#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r ();
#endif

#if defined(CONFIG_CMD_NET)
#if defined(CONFIG_NET_MULTI)
	puts ("Net:   ");
#endif
	eth_initialize(gd->bd);
#endif

	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;) {
		main_loop ();
	}

	/* NOTREACHED - no way out of command loop except booting */
}


void hang (void)
{
	puts ("### ERROR ### Please RESET the board ###\n");
	for (;;);
}


static void sh_reset (void) __attribute__ ((noreturn));
static void sh_reset (void)
{
#if defined(CONFIG_CPU_SUBTYPE_SH4_2XX)		/* SH4-200 series */
	/*
	 * We will use the on-chip watchdog timer to force a
	 * power-on-reset of the device.
	 * A power-on-reset is required to guarantee all SH4-200 cores
	 * will reset back into 29-bit mode, if they were in SE mode.
	 */
		/* WTCNT          = FF	counter to overflow next tick */
	*ST40_CPG_WTCNT = 0x5AFF;

		/* WTCSR2.CKS[3]  = 0	use legacy clock dividers */
	*ST40_CPG_WTCSR2 = 0xAA00;

		/* WTCSR.TME      = 1	enable up-count counter */
		/* WTCSR.WT       = 1	watchdog timer mode */
		/* WTCSR.RSTS     = 0	enable power-on reset */
		/* WTCSR.CKS[2:0] = 2	clock division ratio 1:128 */
		/* NOTE: we need CKS to be big enough to allow
		 * U-Boot to disable the watchdog, AFTER the reset,
		 * otherwise, we enter an infinite-loop of resetting! */
	*ST40_CPG_WTCSR = 0xA5C2;
#elif defined(CONFIG_CPU_SUBTYPE_SH4_3XX)	/* SH4-300 series */
	/*
	 * However, on SH4-300 series parts, issuing a TRAP instruction
	 * with SR.BL=1 should always be sufficient.
	 */
	ulong sr;
	asm ("stc sr, %0":"=r" (sr));
	sr |= (1 << 28);	/* set block bit, SR.BL=1 */
	asm ("ldc %0, sr": :"r" (sr));
	asm volatile ("trapa #0");
#endif

	/* wait for H/W reset to kick in ... */
	for (;;);
}


extern int do_reset(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	sh_reset();
	/*NOTREACHED*/ return (0);
}
