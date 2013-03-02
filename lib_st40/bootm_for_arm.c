/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 * Copyright (C) 2013  STMicroelectronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <u-boot/zlib.h>
#include <asm/byteorder.h>
#include <asm-arm/setup.h>
#include <asm-st40/sh4reg.h>
#include <asm-st40/socregs.h>
#include <asm-st40/pio-control.h>
#include <asm-st40/cache.h>
#include <asm-st40/addrspace.h>
#include <asm-st40/soc.h>
#include "bootm_for_arm.h"



/* /!\ Be careful: these offsets (from the base of the memory) could change depending of the version of the kernel /!\ */
/* /!\ The secondary entry point base is (CONFIG_PAGE_OFFSET + 0x200) -- see kernel file platsmp.c                 /!\ */
/* /!\ However, We do have the kernel entry point, so the secondary entry point is                                 /!\ */
/* /!\ KernelEntryPoint - KERNEL_BASE_OFFSET (0x8000) + SECONDARY_CPU_MEMORY_OFFSET (0x200)                        /!\ */
#define KERNEL_BASE_OFFSET          (0x00008000)
#define ATAGS_MEMORY_OFFSET         (0x00000100)
#define SECONDARY_CPU_MEMORY_OFFSET (0x00000200)

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG


#if defined(CONFIG_STM_DUMP_ATAGS)
	/*
	 * Note, this just simply dumps the ATAGS as raw 32-bit words.
	 * We may want to decode this properly in the future!
	 */
static void DumpTags(const uint32_t start, const uint32_t bytes)
{
	const uint32_t * const array = (uint32_t*)start;
	const uint32_t words = (bytes >> 2) + 1;
	uint32_t i;

	printf("\nATAGS:\n");
	for(i=0; i<words; i+=4)
	{
		printf("%08x:  %8x %8x %8x %8x\n",
			(unsigned int)&array[i],
			array[i+0], array[i+1], array[i+2], array[i+3]);
	}
	printf("\n");
}
#endif /* CONFIG_STM_DUMP_ATAGS */


/****************************************************************************************************************/
/************************************** virt_to_phys() api implementation ***************************************/
/****************************************************************************************************************/

/* Definitions */
/* =========== */
#define MB                          (1024*1024)
#define SH4_PMB_DATA_ARRAY          0xf7100000
#define SH4_PMB_ADDR_ARRAY          0xf6100000
#define SH4_PMB_DATA_ARRAY_ENTRY(n) SH4_DWORD_REG(SH4_PMB_DATA_ARRAY+(n << 8))
#define SH4_PMB_ADDR_ARRAY_ENTRY(n) SH4_DWORD_REG(SH4_PMB_ADDR_ARRAY+(n << 8))
#define ADDR_IN_PMB_RANGE(x)        (((unsigned int)x)>=0x80000000 && ((unsigned int)x)<0xc0000000)
#define ADDR_IN_P4(x)               (((uint32_t)x) > 0xf0000000)
#define PMB_VALID(x)                (x & (1 << 8))
#define PMB_CACHEABLE(x)            (x & (1 << 3))
#define PMB_BASE(x)                 (x & (0xFF << 24))
#define PMB_SIZE(x)                 ((((((x & (1 << 7)) >> 7) * 112) + 16) * (1 + (3 * ((x & (1 << 4)) >> 4)))) * MB)

/* Get the physical address for a given virtual address */
/* ==================================================== */
static const void * virt_to_phys(const void * const vAddr)
{
    uint32_t seEnabled = 0;
    uint32_t pvr = ((*SH4_CCN_PVR) >> 8) & 0xffff;
    if (pvr >= 0x9090 && pvr <= 0x9097)
    {
        /* ST40-300 core */
        if ((*SH4_CCN_PASCR) & (1 << 31))
            seEnabled = 1;
    }
    else if ((*SH4_CCN_MMUCR) & (1 << 4))
    {
        seEnabled = 1;
    }

    if (ADDR_IN_P4(vAddr))
        return vAddr;
    else if (ADDR_IN_PMB_RANGE(vAddr) && seEnabled)
    {
        /* Search PMB registers... */
        int i;
        for (i = 0; i <= 15; i++)
        {
            uint32_t pmbAddr = *SH4_PMB_ADDR_ARRAY_ENTRY(i);
            uint32_t pmbData = *SH4_PMB_DATA_ARRAY_ENTRY(i);
            if (!PMB_VALID(pmbAddr))
                continue;
            if ((uint32_t)vAddr >= PMB_BASE(pmbAddr) && (uint32_t)vAddr < PMB_BASE(pmbAddr) + PMB_SIZE(pmbData))
                return (void*)(PMB_BASE(pmbData) + ((uint32_t)vAddr - PMB_BASE(pmbAddr)));
        }
        /* It's not mapped! */
        return (void*)0xFFFFFFFF;
    }
    else if (seEnabled) /* But vaddr in P0 or P3 => identity mapping */
    {
#ifdef ELF_PHYS_REMAP_ADDR
        /* If the address is in the BART-aliased address range, we
           return the real physical memory address rather than the
           physical BART address.
           Note: We don't do this for the case when
               ADDR_IN_PMB_RANGE(vAddr) && seEnabled
             Instead we assume the BART address range will not be
             mapped in the PMBs.
         */
        return (void*)ELF_PHYS_REMAP_ADDR(vAddr);
#else
        return vAddr;
#endif
    }
    else /* 29-bit mode */
    {
        return (void*)((uint32_t)vAddr & 0x1fffffff);
    }
}


/****************************************************************************************************************/
/*************************** The following is heavily based on the file "lib_arm/bootm.c".  *********************/
/****************************************************************************************************************/


DECLARE_GLOBAL_DATA_PTR;

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_VFD) || \
    defined (CONFIG_LCD)
static void setup_start_tag (bd_t *bd);

# ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd);
# endif
static void setup_commandline_tag (bd_t *bd, char *commandline);

# ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start,
                  ulong initrd_end);
# endif
static void setup_end_tag (bd_t *bd);

# if defined (CONFIG_VFD) || defined (CONFIG_LCD)
static void setup_videolfb_tag (gd_t *gd);
# endif

static struct tag *params;
#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */


extern int do_bootm_armlinux(int flag, int argc, char *argv[], bootm_headers_t *images)
{
    bd_t    *bd = gd->bd;
    char    *s;
    int machid = bd->bi_arch_number;
    void    (*theKernel)(int zero, int arch, uint params);

#ifdef CONFIG_CMDLINE_TAG
    char *commandline = getenv ("bootargs");
#endif

#if 0    /* QQQ: should this be enabled ? */
    if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
        return 1;
#endif

    theKernel = (void (*)(int, int, uint))images->ep;

    s = getenv ("machid");
    if (s) {
        machid = simple_strtoul (s, NULL, 16);
        printf ("Using machid 0x%x from environment\n", machid);
    }

    show_boot_progress (15);

    debug ("## Transferring control to Linux (at address %08lx) ...\n",
           (ulong) theKernel);

    #if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_LCD) || \
    defined (CONFIG_VFD)
    params = (struct tag *)(theKernel-KERNEL_BASE_OFFSET+ATAGS_MEMORY_OFFSET);
    setup_start_tag (bd);
#ifdef CONFIG_SERIAL_TAG
    setup_serial_tag (&params);
#endif
#ifdef CONFIG_REVISION_TAG
    setup_revision_tag (&params);
#endif
#ifdef CONFIG_SETUP_MEMORY_TAGS
    setup_memory_tags (bd);
#endif
#ifdef CONFIG_CMDLINE_TAG
    setup_commandline_tag (bd, commandline);
#endif
#ifdef CONFIG_INITRD_TAG
    if (images->rd_start && images->rd_end)
        setup_initrd_tag (bd, images->rd_start, images->rd_end);
#endif
#if defined (CONFIG_VFD) || defined (CONFIG_LCD)
    setup_videolfb_tag ((gd_t *) gd);
#endif
    setup_end_tag (bd);
#endif

    /* we assume that the kernel is in place */
    printf("\nStarting kernel ...\n\n");

#ifdef CONFIG_USB_DEVICE
    {
        extern void udc_disconnect (void);
        udc_disconnect ();
    }
#endif

    /* Boot arm cpu procedure */
    {
     /* theKernel variable is the physical address of the kernel entry point */
     armKernelEntryAddrPtr          = (uint32_t)(theKernel);
     const uint32_t armSecondaryStartPtr
                                    = (uint32_t)(theKernel - KERNEL_BASE_OFFSET + SECONDARY_CPU_MEMORY_OFFSET);
     armKernelSecondaryEntryAddrPtr = (uint32_t)virt_to_phys((void*)(&armSecondaryStartPtr));
     atagsAddrPtr                   = (uint32_t)(theKernel - KERNEL_BASE_OFFSET + ATAGS_MEMORY_OFFSET);

#if defined(CONFIG_STM_DUMP_ATAGS)
     /* Dump out the newly constructed ATAGS, if required for debug. */
     DumpTags(atagsAddrPtr, (uint32_t)params-atagsAddrPtr);
#endif /* CONFIG_STM_DUMP_ATAGS */

     /* Purge cache */
     sh_flush_cache_all();

     /* Invalidate both instruction and data caches */
     sh_cache_set_op(SH4_CCR_OCI|SH4_CCR_ICI);

#if defined(CONFIG_ST40_STXH415)
     /* Wait for LPM STxP70 to be out of reset */
     int timeout = 0x100000;
     while ((*STXH415_SYSSTS(36) & (1 << 6)) != (1 << 6) && timeout-- > 0) { }

     /* Put ARM boot address in STxP70 DMEM base */
     *STXH415_LPM_STXP70_DMEM_BASE = (uint32_t)virt_to_phys(&armLinuxPrep);
     /* Clear marker location used to check whether STxP70 did its job */
     *STXH415_LPM_STXP70_DMEM_04 = 0;

     /* Load STxP70 code */
     memcpy((void*)STXH415_LPM_STXP70_IMEM_BASE, &stxp70ARMBootHelperCode_bin, stxp70ARMBootHelperCode_bin_len);

     /* Start STxP70 */
     *STXH415_LPM_STXP70_CONFIG_REGS_BASE |= 1;
     *STXH415_SYSCFG(15) |= (1 << 1);

     /* Wait and see if STxP70 completes its task */
     timeout = 0x100000;
     while (*STXH415_LPM_STXP70_DMEM_04 != 0xb007b007 && timeout-- > 0) { }
     if (timeout <= 0)
     {
        printf("Warning: LPM STxP70 did not complete ABAP initialisation\n");
     }

     /* Clean-up hardware */
#if defined(CONFIG_DRIVER_NET_STM_GMAC)
#if defined(CONFIG_ST40_B2000)
#if (CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE)
     /* /!\ Without this little hack, resetting PIO15.5 (MDC signal), the phy is not detected anymore on B2000 platform after kernel boot /!\ */
     {
      stxh415_pioalt_select(15,5,0);
      stxh415_pioalt_pad(15,5,stm_pad_direction_input);
      stxh415_pioalt_retime(15,5,RET_BYPASS(0));
      stmac_phy_reset();
     }
#endif
#endif
#endif

     /* Boot A9 from ABAP CONFIG647 */
     *STXH415_SYSCFG(647)|=1;
     /* Hold A9 reset */
     *STXH415_SYSCFG(644)=0xE;
     /* Wait 1ms */
     udelay(1000);
     /* Release reset A9 */
     *STXH415_SYSCFG(644)=0xF;
     /* Wait 1ms */
     udelay(1000);
     /* Reset ST40 as not used anymore */
     *STXH415_SYSCFG(644)=0xB;
#elif defined(CONFIG_ST40_STXH416)
     /* Put ARM boot address in the (mirrored) reset vector  */
     *STXH416_A9_RESET_VECTOR_MIRROR_BOOTADDR = (uint32_t)virt_to_phys(&armLinuxPrep);
     /* Clean-up hardware */
#if defined(CONFIG_DRIVER_NET_STM_GMAC)
#if defined(CONFIG_ST40_B2000)
#if (CONFIG_SYS_STM_STMAC_BASE == CONFIG_SYS_STM_STMAC0_BASE)
     /* /!\ Without this little hack, resetting PIO15.5 (MDC signal), the phy is not detected anymore on B2000 platform after kernel boot /!\ */
     {
      stxh416_pioalt_select(15,5,0);
      stxh416_pioalt_pad(15,5,stm_pad_direction_input);
      stxh416_pioalt_retime(15,5,RET_BYPASS(0),stm_pad_direction_input);
      stmac_phy_reset();
     }
#endif
#endif
#endif

     /* Boot A9 from ABAP CONFIG7548 */
     *STXH416_SYSCFG(7548)|=1;
     /* Hold A9 reset */
     *STXH416_SYSCFG(7545)=0xE;
     /* Wait 1ms */
     udelay(1000);
     /* Release reset A9 */
     *STXH416_SYSCFG(7545)=0xF;
     /* Wait 1ms */
     udelay(1000);
     /* Reset ST40 as not used anymore */
     *STXH416_SYSCFG(7545)=0xB;
#else
#error ARM booting is only currently available on the STxH415 or STxH416!
#endif	/* CONFIG_ST40_STXH415/CONFIG_ST40_STXH416 */

     /* Does not return, Cpu should die as we just reset it */
     while(1);
    }

    return 1;
}


#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_LCD) || \
    defined (CONFIG_VFD)
static void setup_start_tag (bd_t *bd)
{
    params->hdr.tag = ATAG_CORE;
    params->hdr.size = tag_size (tag_core);

    params->u.core.flags = 0;
    params->u.core.pagesize = 0;
    params->u.core.rootdev = 0;

    params = tag_next (params);
}


#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd)
{
    params->hdr.tag = ATAG_MEM;
    params->hdr.size = tag_size (tag_mem32);

    params->u.mem.start = (u32)virt_to_phys((void *)CONFIG_SYS_SDRAM_BASE);
    params->u.mem.size  = CONFIG_SYS_SDRAM_SIZE;

    params = tag_next (params);
}
#endif /* CONFIG_SETUP_MEMORY_TAGS */


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


#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start, ulong initrd_end)
{
    /* an ATAG_INITRD node tells the kernel where the compressed
     * ramdisk can be found. ATAG_RDIMG is a better name, actually.
     */
    params->hdr.tag = ATAG_INITRD2;
    params->hdr.size = tag_size (tag_initrd);

    params->u.initrd.start = initrd_start;
    params->u.initrd.size = initrd_end - initrd_start;

    params = tag_next (params);
}
#endif /* CONFIG_INITRD_TAG */


#if defined (CONFIG_VFD) || defined (CONFIG_LCD)
extern ulong calc_fbsize (void);
static void setup_videolfb_tag (gd_t *gd)
{
    /* An ATAG_VIDEOLFB node tells the kernel where and how large
     * the framebuffer for video was allocated (among other things).
     * Note that a _physical_ address is passed !
     *
     * We only use it to pass the address and size, the other entries
     * in the tag_videolfb are not of interest.
     */
    params->hdr.tag = ATAG_VIDEOLFB;
    params->hdr.size = tag_size (tag_videolfb);

    params->u.videolfb.lfb_base = (u32) gd->fb_base;
    /* Fb size is calculated according to parameters for our panel
     */
    params->u.videolfb.lfb_size = calc_fbsize();

    params = tag_next (params);
}
#endif /* CONFIG_VFD || CONFIG_LCD */

#ifdef CONFIG_SERIAL_TAG
void setup_serial_tag (struct tag **tmp)
{
    struct tag *params = *tmp;
    struct tag_serialnr serialnr;
    void get_board_serial(struct tag_serialnr *serialnr);

    get_board_serial(&serialnr);
    params->hdr.tag = ATAG_SERIAL;
    params->hdr.size = tag_size (tag_serialnr);
    params->u.serialnr.low = serialnr.low;
    params->u.serialnr.high= serialnr.high;
    params = tag_next (params);
    *tmp = params;
}
#endif

#ifdef CONFIG_REVISION_TAG
void setup_revision_tag(struct tag **in_params)
{
    u32 rev = 0;
    u32 get_board_rev(void);

    rev = get_board_rev();
    params->hdr.tag = ATAG_REVISION;
    params->hdr.size = tag_size (tag_revision);
    params->u.revision.rev = rev;
    params = tag_next (params);
}
#endif  /* CONFIG_REVISION_TAG */


static void setup_end_tag (bd_t *bd)
{
    params->hdr.tag = ATAG_NONE;
    params->hdr.size = 0;
}

#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */
