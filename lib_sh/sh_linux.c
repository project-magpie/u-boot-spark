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
#include <asm/sh4-cache.h>
#include <asm/io.h>
#include <asm/sh4reg.h>

#ifdef CONFIG_SHOW_BOOT_PROGRESS
# include <status_led.h>
# define SHOW_BOOT_PROGRESS(arg)	show_boot_progress(arg)
#else
# define SHOW_BOOT_PROGRESS(arg)
#endif

int gunzip (void *, int, unsigned char *, int *);

extern image_header_t header;	/* from cmd_bootm.c */

extern int do_reset (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);

#define PAGE_OFFSET 0x1000

#define MOUNT_ROOT_RDONLY	((unsigned long *) (param+0x000))
#define RAMDISK_FLAGS		((unsigned long *) (param+0x004))
#define ORIG_ROOT_DEV		((unsigned long *) (param+0x008))
#define LOADER_TYPE		((unsigned long *) (param+0x00c))
#define INITRD_START		((unsigned long *) (param+0x010))
#define INITRD_SIZE		((unsigned long *) (param+0x014))
#define SE_MODE			((const unsigned long *) (param+0x018))
/* ... */
#define COMMAND_LINE		((char *) (param+0x100))


extern void sh_cache_set_op(ulong);
extern void flashWriteDisable(void);
#ifdef CONFIG_SH_SE_MODE
extern void sh_toggle_pmb_cacheability(void);
#endif	/* CONFIG_SH_SE_MODE */

#ifdef CONFIG_SH_SE_MODE
#define CURRENT_SE_MODE 32	/* 32-bit (Space Enhanced) Mode */
#else
#define CURRENT_SE_MODE 29	/* 29-bit (Traditional) Mode */
#endif	/* CONFIG_SH_SE_MODE */

void sh4_flush_cache_all(void)
{
	unsigned long addr, end_addr, entry_offset;

	/* Flush dcache */

	end_addr = CACHE_OC_ADDRESS_ARRAY +
		(DCACHE_SETS << DCACHE_ENTRY_SHIFT) * DCACHE_WAYS;

	entry_offset = 1 << DCACHE_ENTRY_SHIFT;
	for (addr = CACHE_OC_ADDRESS_ARRAY;
	     addr < end_addr;
	     addr += entry_offset) {
		ctrl_outl(0, addr);
	}

	/* Invalidate caches */

	sh_cache_set_op(SH4_CCR_OCI|SH4_CCR_ICI);
}

void do_bootm_linux (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[],
		     ulong addr, ulong * len_ptr, int verify)
{
	DECLARE_GLOBAL_DATA_PTR;

	ulong len = 0, checksum;
	ulong initrd_start;
	ulong data, param;
	void (*theKernel) (void);
	image_header_t *hdr = &header;
	char *commandline = getenv ("bootargs");
	char extra[128];	/* Extra command line args */
	extra[0] = 0;

	theKernel = (void (*)(void)) ntohl (hdr->ih_ep);
	param = ntohl (hdr->ih_load);

	/*
	 * Check if there is an initrd image
	 */
	if (argc >= 3) {
		SHOW_BOOT_PROGRESS (9);

		addr = simple_strtoul (argv[2], NULL, 16);

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

		if (crc32 (0, (unsigned char *) data, len) != checksum) {
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
			csum = crc32 (0, (unsigned char *) data, len);
			if (csum != ntohl (hdr->ih_dcrc)) {
				printf ("Bad Data CRC\n");
				SHOW_BOOT_PROGRESS (-12);
				do_reset (cmdtp, flag, argc, argv);
			}
			printf ("OK\n");
		}

		SHOW_BOOT_PROGRESS (11);

		if ((hdr->ih_os != IH_OS_LINUX) ||
		    (hdr->ih_arch != IH_CPU_SH) ||
		    (hdr->ih_type != IH_TYPE_RAMDISK)) {
			printf ("No Linux SH Ramdisk Image\n");
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
		ulong sp;
	      asm ("mov r15, %0": "=r" (sp):);
				/* read stack pointer */
		debug ("## Current stack ends at 0x%08lX ", sp);

		sp -= 2048;	/* just to be sure */
		if (sp > (CFG_SDRAM_BASE + CFG_BOOTMAPSZ))
			sp = (CFG_SDRAM_BASE + CFG_BOOTMAPSZ);
		sp &= ~0xF;
		initrd_start = (sp - len) & ~(4096 - 1);

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


		*LOADER_TYPE = 1;
		*INITRD_START = (initrd_start - (param - PAGE_OFFSET)) ;	/* passed of offset from memory base */
		*INITRD_SIZE = len;
	} else {
		*LOADER_TYPE = 0;
		*INITRD_START = 0;
		*INITRD_SIZE = 0;
	}

	SHOW_BOOT_PROGRESS (15);

	/* try and detect if the kernel is incomptable with U-boot */
	if ((*SE_MODE & 0xFFFFFF00) != 0x53453F00)	/* 'SE?.' */
	{
		printf("\nWarning: Unable to determine if kernel is built for 29- or 32-bit mode!\n");
	}
	else if ((*SE_MODE & 0xFF) != CURRENT_SE_MODE)
	{
		printf("\n"
			"Error: A %2u-bit Kernel is incomptable with this %2u-bit U-Boot!\n"
			"Please re-configure and re-build vmlinux or u-boot.\n"
			"Aborting the Boot process - Boot FAILED.  (SE_MODE=0x%08x)\n",
			CURRENT_SE_MODE,
			CURRENT_SE_MODE ^ 0x3d,
			*SE_MODE);
		return;
	}

#ifdef DEBUG
	printf ("## Transferring control to Linux (at address %08lx) initrd =  %08lx ...\n",
		(ulong) theKernel, *INITRD_START);
#endif

	strcpy (COMMAND_LINE, commandline);
	if (*extra)
		strcpy (COMMAND_LINE + strlen (commandline), extra);

	/* linux_params_init (gd->bd->bi_boot_params, commandline); */

	printf ("\nStarting kernel %s - 0x%08x - %d ...\n\n", COMMAND_LINE,
		*INITRD_START, *INITRD_SIZE);

	sh4_flush_cache_all();
	flashWriteDisable();

#ifdef CONFIG_SH_SE_MODE
	/*
	 * Now run out of the UN-cached PMB array #0.
	 * For 32-bit mode, our contract with the kernel requires
	 * that the kernel starts running out of an uncached PMB mapping.
	 */
	sh_toggle_pmb_cacheability();
#endif	/* CONFIG_SH_SE_MODE */

	theKernel ();
}
