/*
 * (C) Copyright 2004,2005 STMicroelectronics.
 *
 * Original port by Andy Sturges <andy.sturges@st.com>
 * Kernel parameter tag lists stolen shamelessly from ARM
 * by Carl Shaw <carl.shaw@st.com>
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
#include <asm/byteorder.h>

#define DEBUG 1

#ifdef CONFIG_SHOW_BOOT_PROGRESS
# include <status_led.h>
# define SHOW_BOOT_PROGRESS(arg)	show_boot_progress(arg)
#else
# define SHOW_BOOT_PROGRESS(arg)
#endif

int gunzip (void *, int, unsigned char *, int *);

extern image_header_t header;	/* from cmd_bootm.c */

extern int do_reset (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
extern void mmu_off (void);

/*  Forward definitions for parameter passing to kernel via tags  */
static void setup_start_tag (bd_t *bd);
static void setup_commandline_tag (bd_t *bd, char *commandline);
static void setup_initrd_tag (bd_t *bd, ulong initrd_start,
			      ulong initrd_end);
static void setup_end_tag (bd_t *bd);

static struct tag *params;

static void *get_sp() {int dummy ; return &dummy; }

void do_bootm_linux (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[],
		     ulong addr, ulong * len_ptr, int verify)
{
	DECLARE_GLOBAL_DATA_PTR;

	ulong len = 0, checksum;
	ulong initrd_start, initrd_end;
	ulong data;
	void (*theKernel) (void);
	image_header_t *hdr = &header;
	bd_t *bd = gd->bd;
	char *commandline = getenv ("bootargs");

	theKernel = (void (*)(void)) ntohl (hdr->ih_ep);

	/*
	 * Check if there is an initrd image
	 */
	if (argc >= 3) {
		SHOW_BOOT_PROGRESS (9);

		addr = simple_strtoul (argv[2], NULL, 16);

		printf ("## Loading Ramdisk Image at %08lx ...\n", addr);

		/* Copy header so we can blank CRC field for re-calculation */
		memcpy (&header, (char *) addr, sizeof (image_header_t));

		if (ntohl (hdr->ih_magic) != IH_MAGIC) {
			printf ("Bad Magic Number\n");
			SHOW_BOOT_PROGRESS (-10);
			do_reset (cmdtp, flag, argc, argv);
		}

		data = (ulong) & header;
		len = sizeof (image_header_t);

		checksum = ntohl (hdr->ih_hcrc);
		hdr->ih_hcrc = 0;

		if (crc32 (0, (char *) data, len) != checksum) {
			printf ("Bad Header Checksum\n");
			SHOW_BOOT_PROGRESS (-11);
			do_reset (cmdtp, flag, argc, argv);
		}

		SHOW_BOOT_PROGRESS (10);

		print_image_hdr (hdr);

		data = addr + sizeof (image_header_t);
		len = ntohl (hdr->ih_size);

		if (verify) {
			ulong csum = 0;

			printf ("   Verifying Checksum ... ");
			csum = crc32 (0, (char *) data, len);
			if (csum != ntohl (hdr->ih_dcrc)) {
				printf ("Bad Data CRC\n");
				SHOW_BOOT_PROGRESS (-12);
				do_reset (cmdtp, flag, argc, argv);
			}
			printf ("OK\n");
		}

		SHOW_BOOT_PROGRESS (11);

		if ((hdr->ih_os != IH_OS_LINUX) ||
		    (hdr->ih_arch != IH_CPU_ST200) ||
		    (hdr->ih_type != IH_TYPE_RAMDISK)) {
			printf ("No Linux ST200 Ramdisk Image\n");
			SHOW_BOOT_PROGRESS (-13);
			do_reset (cmdtp, flag, argc, argv);
		}


		/*
		 * Now check if we have a multifile image
		 */
	} else if ((hdr->ih_type == IH_TYPE_MULTI) && (len_ptr[1])) {
		ulong tail = ntohl (len_ptr[0]) % 4;
		int i;

		SHOW_BOOT_PROGRESS (13);

		/* skip kernel length and terminator */
		data = (ulong) (&len_ptr[2]);
		/* skip any additional image length fields */
		for (i = 1; len_ptr[i]; ++i)
			data += 4;
		/* add kernel length, and align */
		data += ntohl (len_ptr[0]);
		if (tail) {
			data += 4 - tail;
		}

		len = ntohl (len_ptr[1]);

	} else {
		/*
		 * no initrd image
		 */
		SHOW_BOOT_PROGRESS (14);

		data = 0;
	}

#ifdef	DEBUG
	if (!data) {
		printf ("No initrd\n");
	}
#endif


	if (data) {
		/*
		 * Copy ramdisk image into place
		 * data points to start of image
		 * len gives length of image
		 * we will copy image onto end of kernel aligned on a page
		 * boundary
		 *
		 */
		ulong sp = get_sp();

		debug ("## Current stack ends at 0x%08lX ", sp);

		sp -= 2048;	/* just to be sure */
		if (sp > (CFG_SDRAM_BASE + CFG_BOOTMAPSZ))
			sp = (CFG_SDRAM_BASE + CFG_BOOTMAPSZ);
		sp &= ~0xF;
		initrd_start = (sp - len) & ~(8192 - 1);

		SHOW_BOOT_PROGRESS (12);

		debug ("## initrd at 0x%08lX ... 0x%08lX (len=%ld=0x%lX)\n",
		       data, data + len - 1, len, len);

		printf ("   Loading Ramdisk to %08lx, length %08lx ... ",
			initrd_start, len);
#if defined(CONFIG_HW_WATCHDOG) || defined(CONFIG_WATCHDOG)
		{
			size_t l = len;
			void *to = (void *) initrd_start;
			void *from = (void *) data;

			while (l > 0) {
				size_t tail = (l > CHUNKSZ) ? CHUNKSZ : l;
				WATCHDOG_RESET ();
				memmove (to, from, tail);
				to += tail;
				from += tail;
				l -= tail;
			}
		}
#else /* !(CONFIG_HW_WATCHDOG || CONFIG_WATCHDOG) */
		memmove ((void *) initrd_start, (void *) data, len);
#endif /* CONFIG_HW_WATCHDOG || CONFIG_WATCHDOG */
		puts ("OK\n");

		initrd_end = initrd_start + len;
	} else {
		initrd_start = 0;
		initrd_end = 0;
	}

	SHOW_BOOT_PROGRESS (15);

#ifdef DEBUG
	printf ("## Transferring control to Linux (at address %08lx) ...\n",
		(ulong) theKernel);
#endif

	setup_start_tag (bd);
	setup_commandline_tag (bd, commandline);

	if (initrd_start && initrd_end)
		setup_initrd_tag (bd, initrd_start, initrd_end);

	setup_end_tag (bd);

	printf ("\nStarting kernel ...\n\n");

	mmu_off ();

	printf ("\nGo ...\n");
	theKernel ();
}

static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *) bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.magic = ATAG_CORE_MAGIC;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}


static void setup_commandline_tag (bd_t *bd, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

	params = tag_next (params);
}

static void setup_initrd_tag (bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next (params);
}


static void setup_end_tag (bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

