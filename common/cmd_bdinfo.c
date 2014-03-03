/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2009-2013 STMicroelectronics.
 * Sean McGoogan <Sean.McGoogan@st.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Boot support
 */
#include <common.h>
#include <command.h>
#include <linux/compiler.h>

DECLARE_GLOBAL_DATA_PTR;

__maybe_unused
static void print_num(const char *name, ulong value)
{
	printf("%-12s= 0x%08lX\n", name, value);
}

__maybe_unused
static void print_eth(int idx)
{
	char name[10], *val;
	if (idx)
		sprintf(name, "eth%iaddr", idx);
	else
		strcpy(name, "ethaddr");
	val = getenv(name);
	if (!val)
		val = "(not set)";
	printf("%-12s= %s\n", name, val);
}

__maybe_unused
static void print_eths(void)
{
	struct eth_device *dev;
	int i = 0;

	do {
		dev = eth_get_dev_by_index(i);
		if (dev) {
			printf("eth%dname    = %s\n", i, dev->name);
			print_eth(i);
			i++;
		}
	} while (dev);

	printf("current eth = %s\n", eth_get_name());
	printf("ip_addr     = %s\n", getenv("ipaddr"));
}

__maybe_unused
static void print_lnum(const char *name, unsigned long long value)
{
	printf("%-12s= 0x%.8llX\t(", name, value);
	/* Also, print it in a 'human-friendly' format as well. */
	print_size(value, ")\n");
}

__maybe_unused
static void print_mhz(const char *name, unsigned long hz)
{
	char buf[32];

	printf("%-12s= %6s MHz\n", name, strmhz(buf, hz));
}

#if defined(CONFIG_PPC)
void __weak board_detail(void)
{
	/* Please define boot_detail() for your platform */
}

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

#ifdef DEBUG
	print_num("bd address",		(ulong)bd);
#endif
	print_num("memstart",		bd->bi_memstart);
	print_lnum("memsize",		bd->bi_memsize);
	print_num("flashstart",		bd->bi_flashstart);
	print_num("flashsize",		bd->bi_flashsize);
	print_num("flashoffset",	bd->bi_flashoffset);
	print_num("sramstart",		bd->bi_sramstart);
	print_num("sramsize",		bd->bi_sramsize);
#if	defined(CONFIG_5xx)  || defined(CONFIG_8xx) || \
	defined(CONFIG_8260) || defined(CONFIG_E500)
	print_num("immr_base",		bd->bi_immr_base);
#endif
	print_num("bootflags",		bd->bi_bootflags);
#if	defined(CONFIG_405EP) || \
	defined(CONFIG_405GP) || \
	defined(CONFIG_440EP) || defined(CONFIG_440EPX) || \
	defined(CONFIG_440GR) || defined(CONFIG_440GRX) || \
	defined(CONFIG_440SP) || defined(CONFIG_440SPE) || \
	defined(CONFIG_XILINX_405)
	print_mhz("procfreq",		bd->bi_procfreq);
	print_mhz("plb_busfreq",	bd->bi_plb_busfreq);
#if	defined(CONFIG_405EP) || defined(CONFIG_405GP) || \
	defined(CONFIG_440EP) || defined(CONFIG_440EPX) || \
	defined(CONFIG_440GR) || defined(CONFIG_440GRX) || \
	defined(CONFIG_440SPE) || defined(CONFIG_XILINX_405)
	print_mhz("pci_busfreq",	bd->bi_pci_busfreq);
#endif
#else	/* ! CONFIG_405GP, CONFIG_405EP, CONFIG_XILINX_405, CONFIG_440EP CONFIG_440GR */
#if defined(CONFIG_CPM2)
	print_mhz("vco",		bd->bi_vco);
	print_mhz("sccfreq",		bd->bi_sccfreq);
	print_mhz("brgfreq",		bd->bi_brgfreq);
#endif
	print_mhz("intfreq",		bd->bi_intfreq);
#if defined(CONFIG_CPM2)
	print_mhz("cpmfreq",		bd->bi_cpmfreq);
#endif
	print_mhz("busfreq",		bd->bi_busfreq);
#endif /* CONFIG_405GP, CONFIG_405EP, CONFIG_XILINX_405, CONFIG_440EP CONFIG_440GR */

#ifdef CONFIG_ENABLE_36BIT_PHYS
#ifdef CONFIG_PHYS_64BIT
	puts("addressing  = 36-bit\n");
#else
	puts("addressing  = 32-bit\n");
#endif
#endif

	print_eth(0);
#if defined(CONFIG_HAS_ETH1)
	print_eth(1);
#endif
#if defined(CONFIG_HAS_ETH2)
	print_eth(2);
#endif
#if defined(CONFIG_HAS_ETH3)
	print_eth(3);
#endif
#if defined(CONFIG_HAS_ETH4)
	print_eth(4);
#endif
#if defined(CONFIG_HAS_ETH5)
	print_eth(5);
#endif

#ifdef CONFIG_HERMES
	print_mhz("ethspeed",		bd->bi_ethspeed);
#endif
	printf("IP addr     = %s\n", getenv("ipaddr"));
	printf("baudrate    = %6u bps\n", bd->bi_baudrate);
	print_num("relocaddr", gd->relocaddr);
	board_detail();
	return 0;
}

#elif defined(CONFIG_NIOS2)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	print_num("mem start",		(ulong)bd->bi_memstart);
	print_lnum("mem size",		(u64)bd->bi_memsize);
	print_num("flash start",	(ulong)bd->bi_flashstart);
	print_num("flash size",		(ulong)bd->bi_flashsize);
	print_num("flash offset",	(ulong)bd->bi_flashoffset);

#if defined(CONFIG_SYS_SRAM_BASE)
	print_num ("sram start",	(ulong)bd->bi_sramstart);
	print_num ("sram size",		(ulong)bd->bi_sramsize);
#endif

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif

	printf("baudrate    = %u bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_MICROBLAZE)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;
	print_num("mem start      ",	(ulong)bd->bi_memstart);
	print_lnum("mem size       ",	(u64)bd->bi_memsize);
	print_num("flash start    ",	(ulong)bd->bi_flashstart);
	print_num("flash size     ",	(ulong)bd->bi_flashsize);
	print_num("flash offset   ",	(ulong)bd->bi_flashoffset);
#if defined(CONFIG_SYS_SRAM_BASE)
	print_num("sram start     ",	(ulong)bd->bi_sramstart);
	print_num("sram size      ",	(ulong)bd->bi_sramsize);
#endif
#if defined(CONFIG_CMD_NET)
	print_eths();
#endif
	printf("baudrate    = %u bps\n", bd->bi_baudrate);
	return 0;
}

#elif defined(CONFIG_SPARC)

int do_bdinfo(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

#ifdef DEBUG
	print_num("bd address             ", (ulong) bd);
#endif
	print_num("memstart               ", bd->bi_memstart);
	print_lnum("memsize                ", bd->bi_memsize);
	print_num("flashstart             ", bd->bi_flashstart);
	print_num("CONFIG_SYS_MONITOR_BASE       ", CONFIG_SYS_MONITOR_BASE);
	print_num("CONFIG_ENV_ADDR           ", CONFIG_ENV_ADDR);
	printf("CONFIG_SYS_RELOC_MONITOR_BASE = 0x%x (%d)\n", CONFIG_SYS_RELOC_MONITOR_BASE,
	       CONFIG_SYS_MONITOR_LEN);
	printf("CONFIG_SYS_MALLOC_BASE        = 0x%x (%d)\n", CONFIG_SYS_MALLOC_BASE,
	       CONFIG_SYS_MALLOC_LEN);
	printf("CONFIG_SYS_INIT_SP_OFFSET     = 0x%x (%d)\n", CONFIG_SYS_INIT_SP_OFFSET,
	       CONFIG_SYS_STACK_SIZE);
	printf("CONFIG_SYS_PROM_OFFSET        = 0x%x (%d)\n", CONFIG_SYS_PROM_OFFSET,
	       CONFIG_SYS_PROM_SIZE);
	printf("CONFIG_SYS_GBL_DATA_OFFSET    = 0x%x (%d)\n", CONFIG_SYS_GBL_DATA_OFFSET,
	       GENERATED_GBL_DATA_SIZE);

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate               = %6u bps\n", bd->bi_baudrate);
	return 0;
}

#elif defined(CONFIG_M68K)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	print_num("memstart",		(ulong)bd->bi_memstart);
	print_lnum("memsize",		(u64)bd->bi_memsize);
	print_num("flashstart",		(ulong)bd->bi_flashstart);
	print_num("flashsize",		(ulong)bd->bi_flashsize);
	print_num("flashoffset",	(ulong)bd->bi_flashoffset);
#if defined(CONFIG_SYS_INIT_RAM_ADDR)
	print_num("sramstart",		(ulong)bd->bi_sramstart);
	print_num("sramsize",		(ulong)bd->bi_sramsize);
#endif
#if defined(CONFIG_SYS_MBAR)
	print_num("mbar",		bd->bi_mbar_base);
#endif
	print_mhz("cpufreq",		bd->bi_intfreq);
	print_mhz("busfreq",		bd->bi_busfreq);
#ifdef CONFIG_PCI
	print_mhz("pcifreq",		bd->bi_pcifreq);
#endif
#ifdef CONFIG_EXTRA_CLOCK
	print_mhz("flbfreq",		bd->bi_flbfreq);
	print_mhz("inpfreq",		bd->bi_inpfreq);
	print_mhz("vcofreq",		bd->bi_vcofreq);
#endif
#if defined(CONFIG_CMD_NET)
	print_eth(0);
#if defined(CONFIG_HAS_ETH1)
	print_eth(1);
#endif
#if defined(CONFIG_HAS_ETH2)
	print_eth(2);
#endif
#if defined(CONFIG_HAS_ETH3)
	print_eth(3);
#endif

	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate    = %u bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_BLACKFIN)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	printf("U-Boot      = %s\n", bd->bi_r_version);
	printf("CPU         = %s\n", bd->bi_cpu);
	printf("Board       = %s\n", bd->bi_board_name);
	print_mhz("VCO",	bd->bi_vco);
	print_mhz("CCLK",	bd->bi_cclk);
	print_mhz("SCLK",	bd->bi_sclk);

	print_num("boot_params",	(ulong)bd->bi_boot_params);
	print_num("memstart",		(ulong)bd->bi_memstart);
	print_lnum("memsize",		(u64)bd->bi_memsize);
	print_num("flashstart",		(ulong)bd->bi_flashstart);
	print_num("flashsize",		(ulong)bd->bi_flashsize);
	print_num("flashoffset",	(ulong)bd->bi_flashoffset);

	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
	printf("baudrate    = %u bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_STM)

#include <stm/socregs.h>
#include <stm/clk.h>

#if !defined(CONFIG_CMD_BDI_DUMP_EMI_BANKS)
#define CONFIG_CMD_BDI_DUMP_EMI_BANKS 1
#endif

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#if CONFIG_CMD_BDI_DUMP_EMI_BANKS
	#if !defined(STM_EMI_SIZE)
	#define STM_EMI_SIZE	(128 << 20)	/* EMI is usually 128 MiB */
	#endif	/* STM_EMI_SIZE */
	#define MAX_EMI_BANKS	6	/* Maximum of 6 EMI Banks */
	const u32 emi_base = 0xa0000000u;
	u32 base[MAX_EMI_BANKS+1];	/* Base address for each bank */
	u32 enabled;			/* number of enabled EMI banks */
#endif	/* CONFIG_CMD_BDI_DUMP_EMI_BANKS */
#if CONFIG_CMD_BDI_DUMP_EMI_BANKS
	unsigned int i;
#endif
	bd_t * const bd = gd->bd;

	print_num ("memstart",		(ulong)bd->bi_memstart);
	print_lnum ("memsize",		(u64)bd->bi_memsize);
#if defined(CONFIG_ARM)
	print_num ("boot_params",	bd->bi_boot_params);
	print_num ("arch_number",	bd->bi_arch_number);
#endif	/* CONFIG_ST40 */
#ifndef CONFIG_SYS_NO_FLASH
	print_num ("flashstart",	(ulong)bd->bi_flashstart);
	print_lnum ("flashsize",	(u64)bd->bi_flashsize);
	print_num ("flashoffset",	(ulong)bd->bi_flashoffset);
#endif /* CONFIG_SYS_NO_FLASH */

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate    = %d bps\n", bd->bi_baudrate);

#if defined(CONFIG_STM_STX5197)
	if (STX5197_DEVICEID_5197(gd->stm_devid))
		printf ("\nSTx5197 version %ld.x", STX5197_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_STX5206)
	if (STX5206_DEVICEID_5206(gd->stm_devid))
		printf ("\nSTx5206/STx5289 version %ld.x", STX5206_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_STX7105)
	if (STX7105_DEVICEID_7105(gd->stm_devid))
		printf ("\nSTx7105 version %ld.x", STX7105_DEVICEID_CUT(gd->stm_devid));
	else if (STX7105_DEVICEID_7106(gd->stm_devid))
		printf ("\nSTx7106 version %ld.x", STX7105_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_STX7108)
	if (STX7108_DEVICEID_7108(gd->stm_devid))
		printf ("\nSTx7108 version %ld.x", STX7108_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_STX7111)
	if (STX7111_DEVICEID_7111(gd->stm_devid))
		printf ("\nSTx7111 version %ld.x", STX7111_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_STX7141)
	if (STX7141_DEVICEID_7141(gd->stm_devid))
		printf ("\nSTx7141 version %ld.x", STX7141_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_STX7200)
	if (STX7200_DEVICEID_7200(gd->stm_devid))
		printf ("\nSTx7200 version %ld.x", STX7200_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_FLI7510)
	if (FLI7510_DEVICEID_7510(gd->stm_devid))
		printf ("\nFLI7510 version %ld.x", FLI7510_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_FLI7540)
	if (FLI7540_DEVICEID_7540(gd->stm_devid))
		printf ("\nFLI7540 version %ld.x", FLI7540_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_STXH407)
	if (STXH407_DEVICEID_407(gd->stm_devid))
		printf ("\nSTxH407 version %ld.x", STXH407_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_STXH415)
	if (STXH415_DEVICEID_415(gd->stm_devid))
		printf ("\nSTxH415 version %ld.x", STXH415_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_STXH416)
	if (STXH416_DEVICEID_416(gd->stm_devid))
		printf ("\nSTxH416 version %ld.x", STXH416_DEVICEID_CUT(gd->stm_devid));
#elif defined(CONFIG_STM_STXH205)
	if (STXH205_DEVICEID_H205(gd->stm_devid))
		printf ("\nSTxH205/7 version %ld.x", STXH205_DEVICEID_CUT(gd->stm_devid));
#else
#error Missing Device Definitions!
#endif
	else
		printf ("\nUnknown device! (id=0x%08lx)", gd->stm_devid);

#if defined(CONFIG_ST40) && defined(CONFIG_ST40_SE_MODE)
	printf ("  [32-bit mode]\n");
#elif defined(CONFIG_ST40)
	printf ("  [29-bit mode]\n");
#elif defined(CONFIG_ARM)
	printf ("  [ARM]\n");
#else
#error Unknown ARCH for STMicroelectronics.
#endif

	print_mhz("UART", stm_get_uart_clk_rate());
#if defined(CONFIG_ST40)
	print_mhz("TMU",  stm_get_tmu_clk_rate());
#endif
	print_mhz("SSC",  stm_get_ssc_clk_rate());

#if CONFIG_CMD_BDI_DUMP_EMI_BANKS
	enabled = *STM_EMI_BANK_ENABLE;
	printf("#EMI Banks  = %u\n", enabled);
	if (enabled > MAX_EMI_BANKS)
	{
		printf("Error: Maximum Number of Enabled Banks should be %u\n", MAX_EMI_BANKS);
		enabled = MAX_EMI_BANKS;
	}

	/*
	 * EmiBaseAddress[5:0] == Address[27:22] (Multiple of 4MiB)
	 *
	 * Retreive all the configured EMI bank bases into base[].
	 */
	for(i=0; i<enabled; i++)
	{
		const u32 start = *STM_EMI_BASEADDRESS(i) & 0x3fu;
		base[i] = emi_base + (start << (22));
	}
	/* last valid bank occupies all remaining space */
	base[i] = emi_base + STM_EMI_SIZE;	/* total size of EMI is usually 128MiB */

	/*
	 * Print out the ranges of each bank.
	 */
	for(i=0; i<enabled; i++)
	{
		const u32 lower = base[i];
		const u32 upper = base[i+1];
		printf ("EMI #%u CS%c  = 0x%08X ... 0x%08X (",
			i,
			'A' + i,
			lower,
			upper-1u);
		print_size (upper-lower, ")\n");
	}
#endif	/* CONFIG_CMD_BDI_DUMP_EMI_BANKS */

	return 0;
}

#elif defined(CONFIG_MIPS)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	print_num("boot_params",	(ulong)bd->bi_boot_params);
	print_num("memstart",		(ulong)bd->bi_memstart);
	print_lnum("memsize",		(u64)bd->bi_memsize);
	print_num("flashstart",		(ulong)bd->bi_flashstart);
	print_num("flashsize",		(ulong)bd->bi_flashsize);
	print_num("flashoffset",	(ulong)bd->bi_flashoffset);

	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
	printf("baudrate    = %u bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_AVR32)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	print_num("boot_params",	(ulong)bd->bi_boot_params);
	print_num("memstart",		(ulong)bd->bi_memstart);
	print_lnum("memsize",		(u64)bd->bi_memsize);
	print_num("flashstart",		(ulong)bd->bi_flashstart);
	print_num("flashsize",		(ulong)bd->bi_flashsize);
	print_num("flashoffset",	(ulong)bd->bi_flashoffset);

	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
	printf("baudrate    = %u bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_ARM)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	bd_t *bd = gd->bd;

	print_num("arch_number",	bd->bi_arch_number);
	print_num("boot_params",	(ulong)bd->bi_boot_params);

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; ++i) {
		print_num("DRAM bank",	i);
		print_num("-> start",	bd->bi_dram[i].start);
		print_num("-> size",	bd->bi_dram[i].size);
	}

#if defined(CONFIG_CMD_NET)
	print_eths();
#endif
	printf("baudrate    = %u bps\n", bd->bi_baudrate);
#if !(defined(CONFIG_SYS_ICACHE_OFF) && defined(CONFIG_SYS_DCACHE_OFF))
	print_num("TLB addr", gd->arch.tlb_addr);
#endif
	print_num("relocaddr", gd->relocaddr);
	print_num("reloc off", gd->reloc_off);
	print_num("irq_sp", gd->irq_sp);	/* irq stack pointer */
	print_num("sp start ", gd->start_addr_sp);
#if defined(CONFIG_LCD) || defined(CONFIG_VIDEO)
	print_num("FB base  ", gd->fb_base);
#endif
	/*
	 * TODO: Currently only support for davinci SOC's is added.
	 * Remove this check once all the board implement this.
	 */
#ifdef CONFIG_CLOCKS
	printf("ARM frequency = %ld MHz\n", gd->bd->bi_arm_freq);
	printf("DSP frequency = %ld MHz\n", gd->bd->bi_dsp_freq);
	printf("DDR frequency = %ld MHz\n", gd->bd->bi_ddr_freq);
#endif
	return 0;
}

#elif defined(CONFIG_SH)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;
	print_num("mem start      ",	(ulong)bd->bi_memstart);
	print_lnum("mem size       ",	(u64)bd->bi_memsize);
	print_num("flash start    ",	(ulong)bd->bi_flashstart);
	print_num("flash size     ",	(ulong)bd->bi_flashsize);
	print_num("flash offset   ",	(ulong)bd->bi_flashoffset);

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate    = %u bps\n", bd->bi_baudrate);
	return 0;
}

#elif defined(CONFIG_X86)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	bd_t *bd = gd->bd;

	print_num("boot_params",	(ulong)bd->bi_boot_params);
	print_num("bi_memstart",	bd->bi_memstart);
	print_num("bi_memsize",		bd->bi_memsize);
	print_num("bi_flashstart",	bd->bi_flashstart);
	print_num("bi_flashsize",	bd->bi_flashsize);
	print_num("bi_flashoffset",	bd->bi_flashoffset);
	print_num("bi_sramstart",	bd->bi_sramstart);
	print_num("bi_sramsize",	bd->bi_sramsize);
	print_num("bi_bootflags",	bd->bi_bootflags);
	print_mhz("cpufreq",		bd->bi_intfreq);
	print_mhz("busfreq",		bd->bi_busfreq);

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; ++i) {
		print_num("DRAM bank",	i);
		print_num("-> start",	bd->bi_dram[i].start);
		print_num("-> size",	bd->bi_dram[i].size);
	}

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
	print_mhz("ethspeed",	    bd->bi_ethspeed);
#endif
	printf("baudrate    = %u bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_SANDBOX)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	bd_t *bd = gd->bd;

	print_num("boot_params", (ulong)bd->bi_boot_params);

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; ++i) {
		print_num("DRAM bank", i);
		print_num("-> start", bd->bi_dram[i].start);
		print_num("-> size", bd->bi_dram[i].size);
	}

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
#if defined(CONFIG_LCD) || defined(CONFIG_VIDEO)
	print_num("FB base  ", gd->fb_base);
#endif
	return 0;
}

#elif defined(CONFIG_NDS32)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	bd_t *bd = gd->bd;

	print_num("arch_number",	bd->bi_arch_number);
	print_num("boot_params",	(ulong)bd->bi_boot_params);

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; ++i) {
		print_num("DRAM bank",	i);
		print_num("-> start",	bd->bi_dram[i].start);
		print_num("-> size",	bd->bi_dram[i].size);
	}

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif
	printf("baudrate    = %u bps\n", bd->bi_baudrate);

	return 0;
}

#elif defined(CONFIG_OPENRISC)

int do_bdinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	bd_t *bd = gd->bd;

	print_num("mem start",		(ulong)bd->bi_memstart);
	print_lnum("mem size",		(u64)bd->bi_memsize);
	print_num("flash start",	(ulong)bd->bi_flashstart);
	print_num("flash size",		(ulong)bd->bi_flashsize);
	print_num("flash offset",	(ulong)bd->bi_flashoffset);

#if defined(CONFIG_CMD_NET)
	print_eth(0);
	printf("ip_addr     = %s\n", getenv("ipaddr"));
#endif

	printf("baudrate    = %u bps\n", bd->bi_baudrate);

	return 0;
}

#else
 #error "a case for this architecture does not exist!"
#endif

/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	bdinfo,	1,	1,	do_bdinfo,
	"print Board Info structure",
	""
);
