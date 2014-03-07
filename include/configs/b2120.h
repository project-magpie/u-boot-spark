/*
 * (C) Copyright 2008-2014 STMicroelectronics.
 *
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_STM	1		/* The SoC is from STMicroelectronics	*/
#if defined(CONFIG_ST40)		/* It must not be an ST40		*/
#  error CONFIG_ST40 must not be defined for this board!
#endif


/*-----------------------------------------------------------------------
 *	Jumper settings to select between the SoC's main boot-modes:
 *		*) boot from 8-bit NAND (with the Hamming or BCH controller)
 *		*) boot from SPI serial flash (NOR)
 *		*) boot from MMC/eMMC (v4.4, or v4.5)
 *
 *	In the following, NAND is always assumed to be 8-bits wide.
 *	The following table only covers *some* of the more likely
 *	configurations. Please consult the full boot-device selection
 *	encoding table, for the other options for MODE[6-2].
 *
 *	There are 5 jumpers for the boot device (SW1-3 to SW1-7)
 *
 *					MODE	1-7 1-6 1-5 1-4 1-3
 *	Flash Boot Configuration	[6-2]	[6] [5] [4] [3] [2]
 *	------------------------	------	--- --- --- --- ---
 *	NAND  2k, 5 cycles, 18-bit BCH	00101	 ON  ON off  ON off
 *	NAND  2k, 5 cycles, 30-bit BCH	00110	 ON  ON off off  ON
 *	NAND  4k, 5 cycles, 18-bit BCH	01001	 ON off  ON  ON off
 *	NAND  4k, 5 cycles, 30-bit BCH	01010	 ON off  ON off  ON
 *	NAND 512, 4 cycles, Hamming	10010	off  ON  ON off  ON
 *	NAND  2k, 5 cycles, Hamming	10100	off  ON off  ON  ON
 *	SPI (NOR) (divide by 4)		11010	off off  ON off  ON
 *	MMC (v4.4, or v4.5)		11110	off off off off  ON
 *
 *	Note: There is no NAND present on the B2120 board.
 */


/*-----------------------------------------------------------------------
 * For ARM CPUs, to boot linux, we also need a "Machine ID",
 * which uniquely identifies this specific class of board.
 * See http://www.arm.linux.org.uk/developer/machines
 */
#define CONFIG_MACH_STM_B2120	4747		/* MACH_TYPE_STM_B2120 */
#define CONFIG_MACH_TYPE	CONFIG_MACH_STM_B2120


/*-----------------------------------------------------------------------
 * Are we booting directly from a SPI Serial Flash device ?
 * If so, then define the "CONFIG_SYS_BOOT_FROM_SPI" macro,
 * otherwise (e.g. for eMMC Flash booting), do not define it.
 * Note: this will be implicitly defined if "make b2120<xx>_spi_config" is run.
 */
//#define CONFIG_SYS_BOOT_FROM_SPI		/* define to build a SPI-bootable image */


/*-----------------------------------------------------------------------
 * Are we booting directly from an eMMC Flash device ?
 * If so, then define the "CONFIG_SYS_BOOT_FROM_EMMC" macro,
 * otherwise (e.g. for SPI Flash booting), do not define it.
 * Note: this will be implicitly defined if "make b2120<xx>_emmc_config" is run.
 */
//#define CONFIG_SYS_BOOT_FROM_EMMC		/* define to build a eMMC-bootable image */


/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * Assume we run out of uncached memory for the moment
 */

#if defined(CONFIG_SYS_BOOT_FROM_SPI)			/* we are booting from SPI */
#elif defined(CONFIG_SYS_BOOT_FROM_EMMC)		/* we are booting from eMMC */
#else
#	error Either CONFIG_SYS_BOOT_FROM_SPI or CONFIG_SYS_BOOT_FROM_EMMC must be defined!
#endif /* CONFIG_SYS_BOOT_FROM_SPI || CONFIG_SYS_BOOT_FROM_EMMC */
#if defined(CONFIG_SYS_BOOT_FROM_SPI) && defined(CONFIG_SYS_BOOT_FROM_EMMC)
#	error Only one of CONFIG_SYS_BOOT_FROM_SPI or CONFIG_SYS_BOOT_FROM_EMMC must be defined!
#endif /* CONFIG_SYS_BOOT_FROM_SPI && CONFIG_SYS_BOOT_FROM_EMMC */

#define CONFIG_SYS_SDRAM_SIZE		0x80000000	/* 2 GiB of LMI SDRAM */

#if defined(CONFIG_SYS_BOOT_FROM_SPI)			/* we are booting from SPI */
#	define CONFIG_SYS_EMI_SPI_BASE	0x00000000	/* CSA: SPI Flash,  Physical 0x00000000 (4MiB) */
#endif	/* CONFIG_SYS_BOOT_FROM_SPI */
#define CONFIG_L2_OFF					/* Disable the L2 caches */
#define CONFIG_NR_DRAM_BANKS		1		/* Number of memory banks */
#define CONFIG_SYS_SDRAM_BASE		0x40000000	/* Start of LMI RAM (identity mapped) */
/* Cortex-A9 CPU clock, ideally it should be read dynamically, but for now set to fixed */
#define CONFIG_SYS_CPU_CLK		1000000000	/* 1 GHz */

#define CONFIG_SYS_MONITOR_LEN		0x00080000	/* Reserve 512 KiB for Monitor */
#define CONFIG_SYS_MALLOC_LEN		(4 << 20)	/* Reserve 4 MiB for malloc */
#define CONFIG_SYS_GBL_DATA_SIZE	(1024-8)	/* Global data structures */

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_TEXT_BASE - CONFIG_SYS_MALLOC_LEN - (1 << 20))

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define XSTR(s) STR(s)
#define STR(s) #s

#define BOARD b2120

#define CONFIG_EXTRA_ENV_SETTINGS \
		"board=" XSTR(BOARD) "\0" \
		"load_addr=" XSTR(CONFIG_SYS_LOAD_ADDR) "\0"

/*--------------------------------------------------------------
 * Command line configuration.
 */

#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_NFS
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII

/*--------------------------------------------------------------
 * Serial console info
 */

/*
 * Use the (normal hardware) internal ST ASC UART as the console.
 */
#define CONFIG_STM_ASC_SERIAL	/* use a ST ASC UART */

/* choose which ST ASC UART to use */
#if 1
#	define CONFIG_SYS_STM_ASC_BASE	STXH407_SBC_ASC0_BASE	/* SBC_UART0 on J12 (off-board) */
#else
#	define CONFIG_SYS_STM_ASC_BASE	STXH407_SBC_ASC1_BASE	/* SBC_UART1 on J13 (off-board) */
#endif

/*---------------------------------------------------------------
 * Ethernet driver config
 */

/*
 * On the STxH407 SoC there is only a single GMAC device (GMAC1) present.
 * On the B2120 board, it is connected to an on-board RealTek RTL8367RB
 * switch (UE1), which we use in RGMII mode, as a (dumb) "fixed PHY".
 */

/* are we using the internal ST GMAC device ? */
#define CONFIG_DRIVER_NET_STM_GMAC

/*
 * Select the appropriate base address for the GMAC.
 * Also, choose which PHY to use.
 */
#ifdef CONFIG_DRIVER_NET_STM_GMAC
#	define CONFIG_SYS_STM_STMAC_BASE	0x09630000ul	/* GMAC #1 */
#	define CONFIG_STMAC_RTL8367RB		/* RealTek RTL8367RB (UE1) */
#	define CONFIG_STMAC_USE_RTL8367_SMI	/* Use (I2C-type) SMI with RTL8367RB */
#	define CONFIG_STMAC_RTL8367_SMI_SDA	1, 0	/* use PIO1[0] */
#	define CONFIG_STMAC_RTL8367_SMI_SCK	1, 1	/* use PIO1[1] */
#endif	/* CONFIG_DRIVER_NET_STM_GMAC */

/*  If this board does not have eeprom for ethernet address so allow the user
 *  to set it in the environment
 */
#define CONFIG_ENV_OVERWRITE

/*---------------------------------------------------------------
 * USB driver config
 */

/*
 * QQQ - to do!
 */

/* Choose if we want USB Mass-Storage Support */
#undef CONFIG_STM_USB

/*---------------------------------------------------------------
 * SATA driver config
 */

/*
 * QQQ - to do!
 */

/* Choose if we want to use a SATA HDD */
#undef CONFIG_STM_SATA

/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */

#define CONFIG_SYS_HUSH_PARSER		1
#define CONFIG_AUTO_COMPLETE		1
#define CONFIG_SYS_LONGHELP		1			/* undef to save memory		*/
#define CONFIG_SYS_PROMPT		"B2120> "		/* Monitor Command Prompt	*/
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_HZ			1000			/* HZ for timer ticks	*/
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_SDRAM_BASE	/* default load address		*/
#define CONFIG_BOOTDELAY		10			/* default delay before executing bootcmd */
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_CMDLINE_EDITING

	/*
	 * Do we want to decrease Vcore for the A9 sub-system?
	 * To boot linux reliably, we may need to decrease the voltage of the A9
	 * sub-system (i.e. Vcore) to a nominal value of 1.10V.
	 * This is due to an issue in the JTAG-A9 connection whereby it appears
	 * some samples need to be powered at 1.30V, but this elevated voltage
	 * can create problems for some IP blocks (e.g. PL310 L2$ controller).
	 * This is just a temporary software work-around for such samples.
	 * WARNING: After the Vcore voltage is decreased, JTAG connections can be lost.
	 * Enable this option if you have a sample affected by such an issue.
	 */
#define CONFIG_MACH_STM_STXH407_A9SS_VCORE_HACK

/*-----------------------------------------------------------------------
 * FLASH organization
 */

/*
 * Choose if we want FLASH Support (eMMC/MMC/SD and/or SPI devices).
 * Note: by default CONFIG_CMD_FLASH is defined in config_cmd_default.h
 */
#undef CONFIG_CMD_FLASH		/* undefine it, define only if needed */
#undef CONFIG_CMD_NAND		/* No NAND present on the B2120 board */
#define CONFIG_SPI_FLASH	/* define for SPI serial flash */
#define CONFIG_STM_SDHCI_0	/* define for MMC #0 (eMMC) */
#define CONFIG_STM_SDHCI_1	/* define for MMC #1 (MMC/SD) */

/*-----------------------------------------------------------------------
 * NOR FLASH organization
 */

#ifdef CONFIG_CMD_FLASH				/* NOR flash present ? */
	/* There is no support for parallel NOR flashes on this board. */
#else
#	undef CONFIG_CMD_IMLS			/* NOR-flash specific */
#	define CONFIG_SYS_NO_FLASH		/* NOR-flash specific */
#endif	/* CONFIG_CMD_FLASH */

/*-----------------------------------------------------------------------
 * SPI SERIAL FLASH organization
 */

/*
 *	Name	Manuf		Device
 *	-----	-----		------
 *	U25	ST		N25Q256
 */
#if defined(CONFIG_SPI_FLASH)				/* SPI serial flash present ? */
#	define CONFIG_SPI_FLASH_ST			/* ST N25Q256 */
#	define CONFIG_SPI				/* enable the SPI driver */
#	define CONFIG_CMD_EEPROM			/* enable the "eeprom" command set */
#	define CONFIG_SYS_I2C_FRAM			/* to minimize performance degradation */
#	undef  CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	/* to minimize performance degradation */

#if 1
	/* Use H/W FSM SPI Controller (not H/W SSC, nor S/W "bit-banging") */
#	define CONFIG_STM_FSM_SPI			/* Use the H/W FSM for SPI */
#	define CONFIG_SYS_STM_SPI_CLOCKDIV	4	/* set SPI_CLOCKDIV = 4 */
#	undef CONFIG_CMD_SPI				/* SPI serial bus command support - NOT with FSM! */
#else
	/* Use S/W "bit-banging", (not H/W FSM, nor H/W SSC) */
#	define CONFIG_SOFT_SPI			/* Use "bit-banging" PIO */
#	define SPI_SCL(val)	do { stxh407_spi_scl((val)); } while (0)
#	define SPI_SDA(val)	do { stxh407_spi_sda((val)); } while (0)
#	define SPI_READ		stxh407_spi_read()
#	define SPI_DELAY	do { udelay(1); } while (0)	/* Note: only 500 kHz! */
#endif
#endif	/* CONFIG_SPI_FLASH */

/*-----------------------------------------------------------------------
 * eMMC/MMC organization
 */

#if defined(CONFIG_STM_SDHCI_0) || defined(CONFIG_STM_SDHCI_1)
#	define CONFIG_STM_SDHCI
#	define CONFIG_SDHCI
#	define CONFIG_MMC
#	define CONFIG_GENERIC_MMC
#	define CONFIG_CMD_MMC
#	if defined(CONFIG_STM_SDHCI_0)
#		define CONFIG_SUPPORT_EMMC_BOOT
#		define CONFIG_MMC_BOOT_MODE_1_BIT		/* use 1-bit boot-mode */
#	endif	/* CONFIG_STM_SDHCI_0 */
#endif	/* CONFIG_STM_SDHCI_0 || CONFIG_STM_SDHCI_1 */

/*-----------------------------------------------------------------------
 * Address, size, & location of U-boot's Environment Sector
 */

#define CONFIG_ENV_SIZE_REQUIRED	0x4000	/* 16 KiB of environment data required */

#if 1 && defined(CONFIG_SPI_FLASH)		/* SPI serial flash present ? */
#	define CONFIG_ENV_IS_IN_EEPROM		/* ENV is stored in SPI Serial Flash */
#	define CONFIG_ENV_OFFSET	CONFIG_SYS_MONITOR_LEN	/* immediately after u-boot.bin */
#	define CONFIG_ENV_SIZE		CONFIG_ENV_SIZE_REQUIRED
#elif 1 && defined(CONFIG_MMC)			/* eMMC/SD flash present ? */
#	define CONFIG_ENV_IS_IN_MMC		/* ENV is stored in eMMC/SD flash */
#	define CONFIG_SYS_MMC_ENV_DEV	0	/* use MMC(0), i.e. the first MMC device */
#	define CONFIG_ENV_OFFSET	CONFIG_SYS_MONITOR_LEN	/* immediately after u-boot.bin */
#	define CONFIG_ENV_SIZE		CONFIG_ENV_SIZE_REQUIRED
#else
#	define CONFIG_ENV_IS_NOWHERE		/* ENV is stored in volatile RAM */
#	undef  CONFIG_CMD_SAVEENV		/* no need for "saveenv" */
#endif	/* CONFIG_SPI_FLASH */

/*---------------------------------------------------------------
 * Disk-based Partition and Filesystem configuration
 */

#if defined(CONFIG_STM_SATA) ||	\
    defined(CONFIG_STM_USB)  ||	\
    defined(CONFIG_MMC)
#	define CONFIG_SYS_64BIT_LBA
#	define CONFIG_LBA48
#	define CONFIG_DOS_PARTITION
#	define CONFIG_CMD_FAT
#	define CONFIG_CMD_EXT2
#endif

/*----------------------------------------------------------------------
 * I2C configuration
 */

#define CONFIG_CMD_I2C				/* do we want I2C support ? */

#if defined(CONFIG_CMD_I2C)
	/* Note: I2C Bus #7 also "probes" devices when a suitable HDMI device is plugged in to CN6. */
#	define CONFIG_I2C_BUS		5	/* Use I2C Bus associated with SSC #5 */
#	define CONFIG_I2C_CMD_TREE		/* use a "i2c" root command */
#	define CONFIG_SYS_I2C_SLAVE	0x7F	/* I2C slave address - Bogus: master-only in U-Boot */
#	define CONFIG_SOFT_I2C			/* I2C with S/W bit-banging	*/
#	undef  CONFIG_HARD_I2C			/* I2C withOUT hardware support	*/
#	define I2C_ACTIVE			/* open-drain, nothing to do */
#	define I2C_TRISTATE			/* open-drain, nothing to do */
#	define I2C_SCL(val)		do { stxh407_i2c_scl((val)); } while (0)
#	define I2C_SDA(val)		do { stxh407_i2c_sda((val)); } while (0)
#	define I2C_READ			stxh407_i2c_read()

	/*
	 * The "BOGOS" for NDELAY() may be calibrated using the
	 * following code fragment, and measuring (using an oscilloscope)
	 * the frequency of the I2C SCL pin, and adjusting
	 * NDELAY_BOGOS, until the SCL is approximately 100 kHz.
	 * (100kHz has a period of 5us + 5us).
	 *
	 *	printf("just toggling I2C SCL (100kHz frequency) ...\n");
	 *	while (1)
	 *	{
	 *		I2C_SCL(1); NDELAY(5000);
	 *		I2C_SCL(0); NDELAY(5000);
	 *	}
	 */
#	define NDELAY_BOGOS		20	/* Empirical measurement for 1ns on B2020 */
#	define NDELAY(ns)						\
		do {							\
			const unsigned n_bogo = NDELAY_BOGOS;		\
			const unsigned n_ticks =			\
				((ns)<n_bogo) ? 1u : (ns)/n_bogo;	\
			volatile unsigned n_count;			\
			for(n_count=0; n_count<n_ticks; n_count++)	\
				;	/* do nothing */		\
		} while(0)

	/*
	 * Note there are 4 * I2C_DELAY per I2C clock cycle
	 * So, 100kHz requires an I2C delay of 2,500 ns,
	 * whereas, 400 kHz requires an I2C delay of 625 ns.
	 * However, this calculation only works if the S/W
	 * overhead in I2C bit-banging is negligible - which it is not!
	 * So, in practice, either I2C_DELAY or CONFIG_SYS_I2C_SPEED will be lower.
	 * The higher the clock frequency, the greater the difference.
	 * Empirical measurement/adjustment is recommended.
	 */
#	if 0	/* run the I2C bus clock at 400 kHz ? */
#		define CONFIG_SYS_I2C_SPEED	400000			/* I2C clock speed (400 kHz) */
#		define I2C_DELAY	do { NDELAY(625); } while (0)	/* 625 ns */
#	else	/* else, run the I2C bus clock at 100 kHz */
#		define CONFIG_SYS_I2C_SPEED	100000			/* I2C clock speed (100 kHz) */
#		define I2C_DELAY	do { NDELAY(625*4); } while (0)	/* 2,500 ns */
#	endif	/* 100kHz or 400kHz */

#endif	/* CONFIG_CMD_I2C */


/*----------------------------------------------------------------------
 * Support for Linux Hibernation-on-Memory (HoM)
 */

	/*
	 * Define the following only if we want to use the
	 * Linux Hibernation-on-Memory (HoM) capability.
	 */
#undef CONFIG_HIBERNATION_ON_MEMORY

	/*
	 * If we are using HoM, we need to tell U-Boot where the
	 * special "tag" is located in memory. This must be the
	 * same address as configured for the linux kernel.
	 */
#undef CONFIG_HOM_TAG_VIRTUAL_ADDRESS


#endif	/* __CONFIG_H */
