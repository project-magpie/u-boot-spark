/*
 * (C) Copyright 2004-2011 STMicroelectronics.
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
#include <asm/cache.h>
#include <asm/io.h>
#include <asm/sh4reg.h>
#include <asm/addrspace.h>
#include <asm/pmb.h>
#include <asm/soc.h>

#ifdef CONFIG_SHOW_BOOT_PROGRESS
# include <status_led.h>
# define SHOW_BOOT_PROGRESS(arg)	show_boot_progress(arg)
#else
# define SHOW_BOOT_PROGRESS(arg)
#endif


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
#ifdef CONFIG_ST40_SE_MODE
extern void sh_toggle_pmb_cacheability(void);
#endif	/* CONFIG_ST40_SE_MODE */

#ifdef CONFIG_ST40_SE_MODE
#define CURRENT_SE_MODE 32	/* 32-bit (Space Enhanced) Mode */
#define	PMB_ADDR(i)	((volatile unsigned long*)(P4SEG_PMB_ADDR+((i)<<8)))
#else
#define CURRENT_SE_MODE 29	/* 29-bit (Traditional) Mode */
#endif	/* CONFIG_ST40_SE_MODE */

void do_bootm_linux (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[],
		     bootm_headers_t *images)
{
	ulong initrd_start, initrd_end, initrd_len;
	ulong param, ep = 0, load = 0;
	void (*theKernel) (void);
	const char * const commandline = getenv ("bootargs");
	int ret;
#ifdef CONFIG_ST40_SE_MODE
	size_t i;
#endif	/* CONFIG_ST40_SE_MODE */

	/* find kernel entry point */
	if (images->legacy_hdr_valid) {
		ep   = image_get_ep   (&images->legacy_hdr_os_copy);
		load = image_get_load (&images->legacy_hdr_os_copy);
#if defined(CONFIG_FIT)
	} else if (images->fit_uname_os) {
		ret = fit_image_get_entry (images->fit_hdr_os,
				images->fit_noffset_os, &ep);
		if (ret) {
			puts ("Can't get entry point property!\n");
			goto error;
		}
#endif
	} else {
		puts ("Could not find kernel entry point!\n");
		goto error;
	}
	theKernel = (void (*)(void))ep;	/* The Kernel's Entry Point */
	param = load;			/* The kernel's "parameter" block */

	/*
	 * Check if there is a valid initrd image
	 */
	ret = boot_get_ramdisk (argc, argv, images, IH_ARCH_SH,
			&initrd_start, &initrd_end);
	initrd_len = initrd_end - initrd_start /* + 1 */;
	if (ret) {
		/* a RAM disk was found, but was corrupt or invalid */
		puts("RAM Disk invalid (or corrupt)!\n");
		goto error;
	}

	if (initrd_start) {
		/*
		 * Copy the ramdisk image into place
		 * initrd_start points to start of initrd image
		 * initrd_end points to end of initrd image
		 * initrd_len gives length of initrd image
		 */
		const ulong orig_start = initrd_start;
		const ulong orig_len   = initrd_len;
		struct lmb * const lmb = images->lmb;
		ulong sp;	/* holds the stack pointer (R15) */

		/* reserve all the memory from just below the SP to end of RAM */
		asm ("mov r15, %0": "=r" (sp):);
		debug ("## Current stack ends at 0x%08lX\n", sp);
		sp -= 64 * 1024;	/* just to be sure */
		lmb_reserve(lmb, sp, (CFG_SDRAM_BASE + CFG_SDRAM_SIZE - sp));

		debug ("## ORIGINAL  initrd at 0x%08lX ... 0x%08lX\n",
		       orig_start, orig_start+initrd_len);
		SHOW_BOOT_PROGRESS (12);

		/* relocate the initrd, honoring 'initrd_high' (if it exists) */
		ret = boot_ramdisk_high(lmb, orig_start, orig_len,
					&initrd_start, &initrd_end);
		if (ret) {
			puts("### Failed to relocate RAM disk\n");
			goto error;
		}
		if (orig_start != initrd_start) {
			printf ("   Relocated initrd from 0x%08lx to 0x%08lx (length 0x%08lx)\n",
				orig_start, initrd_start, initrd_len);
		}
		*LOADER_TYPE = 1;
		*INITRD_START = (initrd_start - (param - PAGE_OFFSET)) ;	/* passed of offset from memory base */
		*INITRD_SIZE = initrd_len;
	} else {
		debug ("## No initrd\n");
		*LOADER_TYPE = 0;
		*INITRD_START = 0;
		*INITRD_SIZE = 0;
	}

	SHOW_BOOT_PROGRESS (15);

	/* try and detect if the kernel is incompatible with U-boot */
	if ((*SE_MODE & 0xFFFFFF00) != 0x53453F00)	/* 'SE?.' */
	{
		printf("\nWarning: Unable to determine if kernel is built for 29- or 32-bit mode!\n");
	}
	else if ((*SE_MODE & 0xFF) != CURRENT_SE_MODE)
	{
		printf("\n"
			"Error: A %2u-bit Kernel is incompatible with this %2u-bit U-Boot!\n"
			"Please re-configure and re-build vmlinux or u-boot.\n"
			"Aborting the Boot process - Boot FAILED.  (SE_MODE=0x%08x)\n",
			CURRENT_SE_MODE ^ (32 ^ 29),
			CURRENT_SE_MODE,
			*SE_MODE);
		goto error;
	}

	/*
	 * now copy the Kernel's command line into the parameter block.
	 * i.e. copy the string in the environment variable "bootargs".
	 */
	strcpy (COMMAND_LINE, commandline);

	debug ("## Preparing to transfer control to Linux (at address %08lx) initrd = %08lx ...\n",
		(ulong) theKernel, *INITRD_START);

	/* if not "autostart", then we are done! */
	if (!images->autostart)
		return ;

	/*
	 * We are now ready to start the final stages, prior
	 * ultimately to transferring control to the kernel.
	 */
	printf ("\nStarting kernel:\n"
		"   start    = 0x%08x\n"
		"   initrd   = 0x%08x (%d bytes)\n"
		"   bootargs = %s\n\n",
		(ulong)theKernel, *INITRD_START, *INITRD_SIZE, COMMAND_LINE);

	/*
	 * remove Vpp from the FLASH, so that no further writes can occur.
	 */
	flashWriteDisable();

	/*
	 * Flush the operand caches, to ensure that there is no unwritten
	 * data residing only in the caches, before the kernel invalidates
	 * them.
	 */
	sh_flush_cache_all();

	/* Invalidate both instruction and data caches */
	sh_cache_set_op(SH4_CCR_OCI|SH4_CCR_ICI);

#if 0
	/*
	 * In order to measure boot-times reasonably accurately,
	 * it is desirable to toggle some pin/signal to indicate
	 * that the period we wish to measure has started and
	 * terminated. We can then attach some probes from a
	 * digital oscilloscope (or logic analyser) and measure
	 * the time between the two transitions.
	 *
	 * In the case of measuring the time for U-Boot from "reset"
	 * to "ready to pass control to linux", then *now* would be a
	 * good opportunity to toggle a PIO pin to indicate that
	 * we can stop measuring time. We ideally want to do this
	 * before we irretrievably "damage" the PMB configuration.
	 *
	 * In this case, we call stmac_phy_reset() before retuning
	 * to the caller. It should be noted that although we are calling
	 * stmac_phy_reset(), our intention is *not* to reset the PHY,
	 * but it is purely to effect a transition on a pin that we can
	 * easily add a probe to. The fact that the PHY has been reset
	 * is just be a benign by-product of this "signalling" mechanism.
	 */
	stmac_phy_reset();
	printf("info: Not passing control to linux "
		"- just called stmac_phy_reset() instead!\n");
	return;
#endif

#ifdef CONFIG_ST40_SE_MODE
	/*
	 * Before we can jump into the kernel, we need to invalidate all
	 * (bar one, or two) of the PMB array entries we are currently using.
	 * Failure to do this, can result in the kernel creating a
	 * new PMB entry with an overlapping virtual address, which
	 * when accessed may result in a ITLBMULTIHIT or OTLBMULTIHIT
	 * exception being raised.
	 *
	 * We also need to enter the kernel running out of an UNCACHED
	 * PMB entry. To perform this mode switch, we actually need to
	 * have 2 PMB entries (#0, #2) both valid for the duration of
	 * this mode switching. However, we invalidate all the others,
	 * prior to this mode switch. Only after the mode switch, can
	 * we then invalidate PMB[2], leaving just one (uncached) PMB
	 * still valid - the one mapping the kernel itself (PMB[0]).
	 *
	 * Note: if CFG_ST40_LMI_NEEDS_2_PMB_ENTRIES is true, then
	 * please read the previous comment as:
	 *
	 * We also need to enter the kernel running out of an UNCACHED
	 * PMB entry. To perform this mode switch, we actually need to
	 * have 4 PMB entries (#0, #1, #2 & #3) valid for the duration of
	 * this mode switching. However, we invalidate all the others,
	 * prior to this mode switch. Only after the mode switch, can
	 * we then invalidate PMB[2:3], leaving just two (uncached) PMB
	 * still valid - the two mapping the kernel itself (PMB[0:1]).
	 *
	 * Note: after this point, U-boot may lose access to
	 * peripherals, including the serial console - so we can not
	 * safely call puts(), printf(), etc. from this point onwards.
	 */
#if CFG_ST40_LMI_NEEDS_2_PMB_ENTRIES
	/* set PMB[n].V = 0, for n == 4..15 */
	for(i=4; i<16; i++)
	{
		*PMB_ADDR(i) = 0;	/* PMB[i].V = 0 */
	}
#else	/* CFG_ST40_LMI_NEEDS_2_PMB_ENTRIES */
	/* set PMB[n].V = 0, for n == 1, 3..15 */
	*PMB_ADDR(1) = 0;		/* PMB[1].V = 0 */
	for(i=3; i<16; i++)
	{
		*PMB_ADDR(i) = 0;	/* PMB[i].V = 0 */
	}
#endif	/* CFG_ST40_LMI_NEEDS_2_PMB_ENTRIES */

	/*
	 * Now run out of the UN-cached PMB array #0 (and #1).
	 * For 32-bit mode, our contract with the kernel requires
	 * that the kernel starts running out of an uncached PMB mapping.
	 */
	sh_toggle_pmb_cacheability();

#if CFG_ST40_LMI_NEEDS_2_PMB_ENTRIES
	/* now invalidate PMB entry #2, #3, leaving just PMB #0, #1 valid */
	*PMB_ADDR(2) = 0;	/* PMB[2].V = 0 */
	*PMB_ADDR(3) = 0;	/* PMB[3].V = 0 */
#else	/* CFG_ST40_LMI_NEEDS_2_PMB_ENTRIES */
	/* now invalidate PMB entry #2, leaving just PMB #0 valid */
	*PMB_ADDR(2) = 0;	/* PMB[2].V = 0 */
#endif	/* CFG_ST40_LMI_NEEDS_2_PMB_ENTRIES */

	/*
	 * we need to ensure that the ITLB is flushed, and not
	 * harbouring any mappings from the recently invalidated
	 * PMB entries.
	 */
	 *(volatile unsigned long*)SH4_CCN_MMUCR |= SH4_MMUCR_TI;
#endif	/* CONFIG_ST40_SE_MODE */

	/* now, finally, we pass control to the kernel itself ... */
	theKernel ();
	/* does not return */
	return;

error:
	if (images->autostart)
		do_reset (cmdtp, flag, argc, argv);
	return;
}
