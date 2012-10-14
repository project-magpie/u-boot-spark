/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
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

#ifndef ASM_ARCH_SPEAR_SPL_NAND_H
#define ASM_ARCH_SPEAR_SPL_NAND_H

extern void spl_nand_init(void);
extern int nand_read_skip_bad(u32 blk, size_t off, size_t *len, u_char *buff);

/*
 * The following definitions come from BootROM library.
 * Please do not change the routine names as the refrences are picked from
 * BootROM library all the versions of which are also committed along with
 * SPL sources
 */

#define BOOT_OK			0

enum {
	BOTH_8_16		= 0,
	EIGHT_BITS_ONLY		= 1,
	SIXTEEN_BITS_ONLY	= 2
};

typedef enum {
	NAND			= 0,
	NOR			= 1,
	ONE_NAND		= 2
} t_flash_device;

typedef enum {
	/* SMALL PAGES */
	/* 64M */
	NAND_64MS_1V_8		= 0x39,
	/* 128M */
	NAND_128MS_1V_8		= 0x33,
	NAND_128MS_1V_16	= 0x43,
	NAND_128MS_3V_8		= 0x73,
	NAND_128MS_3V_16	= 0x53,
	/* 256M */
	NAND_256MS_1V_8		= 0x35,
	NAND_256MS_1V_16	= 0x45,
	NAND_256MS_3V_8		= 0x75,
	NAND_256MS_3V_16	= 0x55,
	/* 512M */
	NAND_512MS_1V_8		= 0x36,
	NAND_512MS_1V_16	= 0x46,
	NAND_512MS_3V_8		= 0x76,
	NAND_512MS_3V_16	= 0x56,
	/* 1G */
	NAND_1GS_1V_8		= 0x40,
	NAND_1GS_1V_16		= 0x49,
	NAND_1GS_3V_8		= 0x79,
	NAND_1GS_3V_16		= 0x59,
	NAND_1GS_3V_DDP_16	= 0x74,
	NAND_1GS_1V_DDP_8	= 0x78,
	NAND_1GS_1V_DDP_16	= 0x72,

	/* LARGE PAGES */
	/* 512M */
	NAND_512ML_1V_8		= 0xA2,
	NAND_512ML_1V_16	= 0xB2,
	NAND_512ML_3V_8		= 0xF2,
	NAND_512ML_3V_16	= 0xC2,
	/* 1G */
	NAND_1GL_1V_8		= 0xA1,
	NAND_1GL_1V_16		= 0xB1,
	NAND_1GL_3V_8		= 0xF1,
	NAND_1GL_3V_16		= 0xC1,
	/* 2G */
	NAND_2GL_1V_8		= 0xAA,
	NAND_2GL_3V_8		= 0xDA,
	NAND_2GL_1V_16		= 0xBA,
	NAND_2GL_3V_16		= 0xCA,
	/* 4G */
	NAND_4GL_1V_8		= 0xAC,
	NAND_4GL_3V_8		= 0xDC,
	NAND_4GL_1V_16		= 0xBC,
	NAND_4GL_3V_16		= 0xCC,
	/* 8G */
	NAND_8GL_1V_8		= 0xA3,
	NAND_8GL_3V_8		= 0xD3,
	NAND_8GL_1V_16		= 0xB3,
	NAND_8GL_3V_16		= 0xC3,
	/* 8G */
	NAND_16GL_1V_8		= 0xA5,
	NAND_16GL_3V_8		= 0xD5,
	NAND_16GL_1V_16		= 0xB5,
	NAND_16GL_3V_16		= 0xC5,
	/* NOR */
	NOR_8			= -1,
	NOR_16			= -2,
	/* NAND */
	ONE_NAND_16		= -4,
	NO_VALID_ID		= 0,
	UNKNOWN_ID		= -3
} t_flash_command_id;

typedef enum {
	NOR_BANK_0 = 0,
	NAND_BANK_0 = 1,
	NAND_BANK_1 = 2,
	NAND_BANK_2 = 3,
	LAST_BANK
} t_flash_FSMC_bank;

struct command_set {
	u32 pagexblock;
	u32 page_size;
	u32 block_size;
	u32 spare_size;
	u32 memory_size;
	u32 ecc_size;
	u32 data_width;
	u32 pmem_command;
	u32 pmem_address[2];
	u32 patt_command;
	u32 pmem_read;
	u32 mem_command;
	u32 att_command;
	u32 mem_command_write;
	u32 att_command_write;
	u32 spare_command;
	u8 /* t_flash_FSMC_bank */ memory_bank;
	u8  padding[3];
};

/*
 * There is a hack while defining this structure. The bootROM interface uses the
 * enum types for a few elements of the structure which it optimizes using the
 * compiler flag '-fshort-enums'. Since the bootrom is fixed it can be hacked
 * here and not let the flag apply to all other parts of code */
struct flashtype {
	u8 /* t_flash_device */		device;
	u8				padding;
	s16 /* t_flash_command_id */	read_id;
	struct command_set		comm_set;
};

typedef enum {
	PAGE_OK,
	PAGE_KO
} t_valid_flag;

struct boot_nand_page {
	u32  page_start_address;
	u8    page[512];
	t_valid_flag page_valid;
	u8    padding[3];
};

typedef u32 (*boot_flashdetectandinit_ptr_type) (struct flashtype *,
		u32, u32, struct boot_nand_page *);
typedef u32 (*boot_flashread_ptr_type) (struct flashtype *, u32,
		u8 *, u32, struct boot_nand_page *);
typedef u32 (*boot_nandsanitycheck_ptr_type) (struct flashtype *,
		u32);
typedef u32 (*boot_nandreadpage_ptr_type) (u32, struct flashtype *,
		struct boot_nand_page *);

#endif
