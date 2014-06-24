/*
 * (C) Copyright 2004-2008 STMicroelectronics.
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
//YWDRIVER_MODI lwj add begin
unsigned int stmac_phy_get_addr (void);
//YWDRIVER_MODI lwj add end

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
#define	PMB_ADDR(i)	((volatile unsigned long*)(P4SEG_PMB_ADDR+((i)<<8)))
#else
#define CURRENT_SE_MODE 29	/* 29-bit (Traditional) Mode */
#endif	/* CONFIG_SH_SE_MODE */

void YW_auto_phy_addr(char *commandline)
{
    char tempbuf[512];
    unsigned int phyaddr;
    char	*pstr1;

    memset(tempbuf,0,sizeof(tempbuf));

    phyaddr = stmac_phy_get_addr();
    if (phyaddr >=0 && phyaddr < 32)
    {
		pstr1 = commandline;
    	for(pstr1 = commandline; pstr1; pstr1++)
    	{
    		if(!strncmp(pstr1, "phyaddr", strlen("phyaddr")))
    		{
    		    sprintf(tempbuf, "%d", phyaddr);

                if ((*(pstr1+9) >= '0') && (*(pstr1+9) <= '9'))
                {
					if(phyaddr > 10)
					{
						strcpy(tempbuf+2, 10+pstr1);
					}
					else
					{
						strcpy(tempbuf+1, 10+pstr1);
					}
					strcpy (8+pstr1, tempbuf);
					break;
                }
                else
                {
					if(phyaddr > 10)
					{
						strcpy(tempbuf+2, 9+pstr1);
					}
					else
					{
						strcpy(tempbuf+1, 9+pstr1);
					}
					strcpy (8+pstr1, tempbuf);
					break;
                }
    		}
    	}
    }
}

void YW_stb_id(char *commandline, char *extra)
{
	#define	NWHWCONF_LEN	46
	uint	LICENSE_GetSTBMacAndSnString( unsigned char * stb_id, unsigned char * stb_mac, char * stb_sn_string );
	uchar * IDENT_GetStbID( void );

	char	std_sn_string[100];
	char	mac_str[18];
	uchar	mac[6];
	uchar	*stb_id;
	char	*pstr = commandline;

	stb_id = IDENT_GetStbID();
    printf("stb_id = %x%x%x%x%x%x%x\n",stb_id[0],stb_id[1],stb_id[2],stb_id[3],stb_id[4],stb_id[5],stb_id[6]);//don't remove
	if(memcmp(stb_id, "\xff\xff\xff\xff\xff\xff\xff", 7))
	{
		// remove parameter "nwhwconf" from bootargs
		for(pstr = commandline; pstr; pstr++)
		{
			if(!strncmp(pstr, "nwhwconf", strlen("nwhwconf")))
			{
				strcpy(pstr, pstr+NWHWCONF_LEN);
				break;
			}
		}
		LICENSE_GetSTBMacAndSnString(stb_id, (uchar *)mac, std_sn_string);
		sprintf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X",
			mac[0],
			mac[1],
			mac[2],
			mac[3],
			mac[4],
			mac[5]
			);
		mac_str[17] = 0;
//		printf("mac: %s\n", mac_str);

		sprintf(extra,
			" nwhwconf=device:eth0,hwaddr:%s Manufacture=%s STB_ID=%02X:%02X:%02X:%02X:%02X:%02X:%02X",
			mac_str,
			std_sn_string,
			stb_id[0],
			stb_id[1],
			stb_id[2],
			stb_id[3],
			stb_id[4],
			stb_id[5],
			stb_id[6]
			);
	}
	else
	{
		printf("GetStbID Failed\n");
	}
}

/*****     2012-12-05     *****/
//YWDRIVER_MODI add by lf for set ethaddr to bootargs start

void YW_turn_string_into_mac_value(uchar *v_env_mac, char *s)
{
	int reg;
	char *e;

	for (reg = 0; reg < 6; ++reg) {	/* turn string into mac value */
		v_env_mac[reg] = s ? simple_strtoul (s, &e, 16) : 0;
		if (s)
			s = (*e) ? e + 1 : e;
	}
}

int YW_check_mac_value_is_all_ff(uchar *v_env_mac)
{
	int reg;
	int bIsAllFF = 1;

	for (reg = 0; reg < 6; ++reg) {
		if (v_env_mac[reg] != 0xff)
		{
		    bIsAllFF = 0;
			break;
		}
	}

	return bIsAllFF;
}

void YW_change_bootargs_hwaddr_by_env_ethaddr_for_ipguard(char * command_line)
{
	int env_size, env_present = 0;
	char *s = NULL, es[] = "11:22:33:44:55:66";
	char s_env_mac[64];
	uchar v_env_mac[6];
	char *hwaddr = NULL;
	int bIsAllFF = 1;

	env_size = getenv_r ("ethaddr", s_env_mac, sizeof (s_env_mac));
	if ((env_size > 0) && (env_size < sizeof (es))) {	/* exit if env is bad */
		printf ("\n*** ERROR: ethaddr is not set properly!!\n");
		return;
	}

	if (env_size > 0) {
		env_present = 1;
		s = s_env_mac;
		//printf("ethaddr = %s\n", s);
	}

	YW_turn_string_into_mac_value(v_env_mac, s);

	bIsAllFF = YW_check_mac_value_is_all_ff(v_env_mac);
	if (bIsAllFF) {
	    return;
	}

	hwaddr = strstr(command_line, "hwaddr:");
	if (!hwaddr) {
		return;
	}

	//printf("hwaddr = %s\n", hwaddr);
	if (env_present) {
		//printf("s_env_mac = %s\n", s_env_mac);
		memcpy(&hwaddr[7], s_env_mac, 2 * 6 + 5);
	}
}

//YWDRIVER_MODI add by lf end

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
#if 1
    //YWDRIVER_MODI lwj add begin for auto phy addr
    YW_auto_phy_addr(commandline);
	//YWDRIVER_MODI lwj add end
	// YWDRIVER_MODI cc changed for STB_ID begin
	YW_stb_id(commandline, extra);
	// YWDRIVER_MODI cc changed for STB_ID end
#endif

#ifdef CONFIG_SH_SE_MODE
	size_t i;
#endif	/* CONFIG_SH_SE_MODE */

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
		return;
	}

#ifdef DEBUG
	printf ("## Transferring control to Linux (at address %08lx) initrd =  %08lx ...\n",
		(ulong) theKernel, *INITRD_START);
#endif

	strcpy (COMMAND_LINE, commandline);
	if (*extra)
		strcpy (COMMAND_LINE + strlen (commandline), extra);

	/*****     2012-12-05     *****/
	//YWDRIVER_MODI add by lf for set ethaddr to bootargs start
	YW_change_bootargs_hwaddr_by_env_ethaddr_for_ipguard(COMMAND_LINE);
	//YWDRIVER_MODI add by lf end

	/* linux_params_init (gd->bd->bi_boot_params, commandline); */

	printf ("\nStarting kernel %s - 0x%08x - %d ...\n\n", COMMAND_LINE,
		*INITRD_START, *INITRD_SIZE);

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

#ifdef CONFIG_SH_SE_MODE
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
	 * Note: if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES is true, then
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
#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	/* set PMB[n].V = 0, for n == 4..15 */
	for(i=4; i<16; i++)
	{
		*PMB_ADDR(i) = 0;	/* PMB[i].V = 0 */
	}
#else	/* CFG_SH_LMI_NEEDS_2_PMB_ENTRIES */
	/* set PMB[n].V = 0, for n == 1, 3..15 */
	*PMB_ADDR(1) = 0;		/* PMB[1].V = 0 */
	for(i=3; i<16; i++)
	{
		*PMB_ADDR(i) = 0;	/* PMB[i].V = 0 */
	}
#endif	/* CFG_SH_LMI_NEEDS_2_PMB_ENTRIES */

	/*
	 * Now run out of the UN-cached PMB array #0 (and #1).
	 * For 32-bit mode, our contract with the kernel requires
	 * that the kernel starts running out of an uncached PMB mapping.
	 */
	sh_toggle_pmb_cacheability();

#if CFG_SH_LMI_NEEDS_2_PMB_ENTRIES
	/* now invalidate PMB entry #2, #3, leaving just PMB #0, #1 valid */
	*PMB_ADDR(2) = 0;	/* PMB[2].V = 0 */
	*PMB_ADDR(3) = 0;	/* PMB[3].V = 0 */
#else	/* CFG_SH_LMI_NEEDS_2_PMB_ENTRIES */
	/* now invalidate PMB entry #2, leaving just PMB #0 valid */
	*PMB_ADDR(2) = 0;	/* PMB[2].V = 0 */
#endif	/* CFG_SH_LMI_NEEDS_2_PMB_ENTRIES */

	/*
	 * we need to ensure that the ITLB is flushed, and not
	 * harbouring any mappings from the recently invalidated
	 * PMB entries.
	 */
	 *(volatile unsigned long*)SH4_CCN_MMUCR |= SH4_MMUCR_TI;
#endif	/* CONFIG_SH_SE_MODE */

	/* now, finally, we pass control to the kernel itself ... */
	theKernel ();
}
