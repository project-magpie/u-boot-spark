/*
 * (C) Copyright 2004 STMicroelectronics.
 *
 * Andy Sturges <andy.sturges@st.com>
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
#include <version.h>
#include <devices.h>
#include <version.h>
#include <net.h>
#include <environment.h>

gd_t *gd;			/* Global pointer */

extern ulong _uboot_end_data;
extern ulong _uboot_end;

ulong monitor_flash_len;

#ifndef CONFIG_IDENT_STRING
#define CONFIG_IDENT_STRING ""
#endif

const char version_string[] =
	U_BOOT_VERSION" (" __DATE__ " - " __TIME__ ") - " CONFIG_IDENT_STRING ;

/*
 * Begin and End of memory area for malloc(), and current "brk"
 */

#define	TOTAL_MALLOC_LEN	CFG_MALLOC_LEN


static ulong mem_malloc_start;
static ulong mem_malloc_end;
static ulong mem_malloc_brk;

extern int board_init (void);
extern int timer_init (void);
extern int checkboard (void);

static void mem_malloc_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	ulong dest_addr = TEXT_BASE + gd->reloc_off;

	mem_malloc_end = dest_addr;
	mem_malloc_start = dest_addr - TOTAL_MALLOC_LEN;
	mem_malloc_brk = mem_malloc_start;

	memset ((void *) mem_malloc_start,
		0, mem_malloc_end - mem_malloc_start);
}

void *sbrk (ptrdiff_t increment)
{
	ulong old = mem_malloc_brk;
	ulong new = old + increment;

	if ((new < mem_malloc_start) || (new > mem_malloc_end)) {
		return (NULL);
	}
	mem_malloc_brk = new;
	return ((void *) old);
}

static int init_func_ram (void)
{
	DECLARE_GLOBAL_DATA_PTR;

#ifdef	CONFIG_BOARD_TYPES
	int board_type = gd->board_type;
#else
#endif

	gd->ram_size = CFG_SDRAM_SIZE;
	puts ("DRAM:  ");
	print_size (gd->ram_size, "\n");

	return (0);
}

static int display_banner (void)
{

	printf ("\n\n%s\n\n", version_string);
	return (0);
}

static void display_flash_config (ulong size)
{
	puts ("Flash: ");
	print_size (size, "\n");
}


static int init_baudrate (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	uchar tmp[64];		/* long enough for environment variables */
	int i = getenv_r ("baudrate", tmp, sizeof (tmp));

	gd->baudrate = (i > 0)
		? (int) simple_strtoul (tmp, NULL, 10)
		: CONFIG_BAUDRATE;

	return (0);
}

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


char *__bss_start;
char *__bss_end;

extern void led_blink (int);

void start_st200boot (ulong bootflag)
{
	DECLARE_GLOBAL_DATA_PTR;


	bd_t *bd;
	ulong addr;
	init_fnc_t **init_fnc_ptr;
	ulong size;

	char *s, *e;
	int i;

	addr = TEXT_BASE;
	/* Reserve memory for malloc() arena. */
	addr -= TOTAL_MALLOC_LEN;
	/* (permanently) allocate a Board Info struct
	 * and a permanent copy of the "global" data
	 */
	addr -= sizeof (gd_t);
	gd = (gd_t *) addr;
	memset ((void *) gd, 0, sizeof (gd_t));
	addr -= sizeof (bd_t);
	bd = (bd_t *) addr;
	gd->bd = bd;

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr) () != 0) {
			hang ();
		}
	}

	gd->reloc_off = 0;
	gd->flags |= GD_FLG_RELOC;	/* tell others: relocation done */
	size = 0;

	monitor_flash_len = (ulong) & _uboot_end_data - TEXT_BASE;

	/* configure available FLASH banks */
	size = flash_init ();
	display_flash_config (size);

	bd = gd->bd;
	bd->bi_boot_params = CFG_SDRAM_BASE;	/* zero page address */
	bd->bi_memstart = CFG_SDRAM_BASE;	/* start of  DRAM memory */
	bd->bi_memsize = gd->ram_size;	/* size  of  DRAM memory in bytes */
	bd->bi_baudrate = gd->baudrate;	/* Console Baudrate */
	bd->bi_flashstart = CFG_FLASH_BASE;
	bd->bi_flashsize = size;
#if CFG_MONITOR_BASE == CFG_FLASH_BASE
	bd->bi_flashoffset = monitor_flash_len;	/* reserved area for U-Boot */
#else
	bd->bi_flashoffset = 0;
#endif

	/* initialize malloc() area */

	mem_malloc_init ();

	/* Allocate environment function pointers etc. */
	env_relocate ();

	/* board MAC address */
	s = getenv ("ethaddr");
	for (i = 0; i < 6; ++i) {
		bd->bi_enetaddr[i] = (s ? simple_strtoul (s, &e, 16) : 0)
			&& 0xff;
		if (s)
			s = (*e) ? e + 1 : e;
	}

	/* IP Address */
	bd->bi_ip_addr = getenv_IPaddr ("ipaddr");

#if defined(CONFIG_PCI)
	/*
	 * Do pci configuration
	 */
	pci_init ();
#endif


/** leave this here (after malloc(), environment and PCI are working) **/
	/* Initialize devices */
	devices_init ();

	jumptable_init ();

	/* Initialize the console (after the relocation and devices init) */
	console_init_r ();


/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

	/* Initialize from environment */
	if ((s = getenv ("loadaddr")) != NULL) {
		load_addr = simple_strtoul (s, NULL, 16);
	}
#if (CONFIG_COMMANDS & CFG_CMD_NET)
	if ((s = getenv ("bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof (BootFile));
	}
#endif /* CFG_CMD_NET */

#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r ();
#endif

#if (CONFIG_COMMANDS & CFG_CMD_NET)
	puts ("Net:   ");
	eth_init (gd->bd);
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
