/*
 * (C) Copyright 2012
 * Vipin Kumar, ST Microelectronics, vipin.kumar@st.com.
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
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/bootrom_table.h>
#include <asm/arch/generic.h>
#include <asm/arch/spl_nand.h>

static struct flashtype flashtype;
static boot_flashdetectandinit_ptr_type boot_flashdetectandinit;
static boot_flashread_ptr_type boot_flashread;
static boot_nandsanitycheck_ptr_type boot_nandsanitycheck;
static struct boot_nand_page *tmp_page_p;

void spl_nand_init(void)
{
	struct bootrom_table *romtb_p =
		(struct bootrom_table *)BOOTROM_TABLE_ADDRESS;

	/* Global function pointers */
	switch (get_socrev()) {
	case SOC_SPEAR300:
		boot_flashdetectandinit =
			(boot_flashdetectandinit_ptr_type)0xffff1774;
		boot_flashread = (boot_flashread_ptr_type)0xffff1050;
		boot_nandsanitycheck =
			(boot_nandsanitycheck_ptr_type)0xffff193C;
		tmp_page_p = (struct boot_nand_page *)0x50030CCC;
		break;
	case SOC_SPEAR600_BA:
	case SOC_SPEAR600_BB:
		/* NAND Boot does not work for Revisions SPEAr600 BA and BB */
	case SOC_SPEAR600_BC:
		boot_flashdetectandinit =
			(boot_flashdetectandinit_ptr_type)0xffff14ec;
		boot_flashread = (boot_flashread_ptr_type)0xffff0dc4;
		boot_nandsanitycheck =
			(boot_nandsanitycheck_ptr_type)0xffff1628;
		tmp_page_p = (struct boot_nand_page *)0xd2800844;
		break;
	case SOC_SPEAR310:
	case SOC_SPEAR320:
	case SOC_SPEAR600_BD:
		boot_flashdetectandinit =
			(romtb_p->table.table_1_0.boot_flashdetectandinit_ptr);
		boot_flashread = (romtb_p->table.table_1_0.boot_flashread_ptr);
		boot_nandsanitycheck =
			(romtb_p->table.table_1_0.boot_nandsanitycheck_ptr);
		tmp_page_p = (struct boot_nand_page *)0xd280084C;
		break;
	default:
		break;
	}

	if (boot_flashdetectandinit)
		(*boot_flashdetectandinit) (&flashtype, 1, BOTH_8_16,
				tmp_page_p);
}

/**
 * nand_read_skip_bad: Read image from NAND flash. Blocks that are marked bad
 * are skipped and the next block is read instead as long as the image is short
 * enough to fit even after skipping the bad blocks.
 *
 * @block: block number to start the read
 * @offset: offset in the block number
 * @length: buffer length, on return holds remaining bytes to read
 * @buffer: buffer to write to
 * @return 0 in case of success
 */
int nand_read_skip_bad(u32 block, size_t offset, size_t *length,
		       u_char *buffer)
{
	struct command_set *command = &(flashtype.comm_set);
	u32 chip_off, readlen;

	if (!boot_nandsanitycheck || !boot_flashread)
		return -EINVAL;

	while (*length) {
		if (BOOT_OK == (*boot_nandsanitycheck) (&flashtype, block)) {
			/* Block is OK */
			chip_off = command->block_size * block + offset;
			readlen = min(command->block_size - offset, *length);

			if (BOOT_OK == (*boot_flashread) (&flashtype, chip_off,
					       buffer, readlen, tmp_page_p)) {
				offset = 0;
				*length -= readlen;
				buffer += readlen;
			} else
				return -EINVAL;
		}
		/* Block is bad */
		block++;
	}

	return 0;
}
