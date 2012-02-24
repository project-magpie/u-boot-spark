/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>

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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>

#ifdef CONFIG_CMD_NAND
#include <nand.h>
#endif

#ifdef CONFIG_CMD_FLASH
#include <flash.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

env_t *env_ptr = NULL;

extern uchar default_environment[];


void env_relocate_spec (void)
{
}

uchar env_get_char_spec (int index)
{
	return ( *((uchar *)(gd->env_addr + index)) );
}

/************************************************************************
 * Initialize Environment use
 *
 * We are still running from ROM, so data use is limited
 */
int  env_init(void)
{
	gd->env_addr  = (ulong)&default_environment[0];
	gd->env_valid = 0;

	return (0);
}

static int writeenv(size_t offset, u_char *buf)
{
	size_t end = offset + CONFIG_ENV_RANGE;
	size_t amount_saved = 0;
	size_t blocksize, len;

	u_char *char_ptr;

	blocksize = nand_info[0].erasesize;
	len = min(blocksize, CONFIG_ENV_SIZE);

	while (amount_saved < CONFIG_ENV_SIZE && offset < end) {
		if (nand_block_isbad(&nand_info[0], offset)) {
			offset += blocksize;
		} else {
			char_ptr = &buf[amount_saved];
			if (nand_write(&nand_info[0], offset, &len, char_ptr))
				return 1;
			offset += blocksize;
			amount_saved += len;
		}
	}
	if (amount_saved != CONFIG_ENV_SIZE)
		return 1;

	return 0;
}

static int saveenv_nand(int address)
{
	int ret = 0;
	nand_erase_options_t nand_erase_options;
	nand_erase_options.length = CONFIG_ENV_RANGE;
	nand_erase_options.quiet = 0;
	nand_erase_options.jffs2 = 0;
	nand_erase_options.scrub = 0;
	nand_erase_options.offset = address;

	if (CONFIG_ENV_RANGE < CONFIG_ENV_SIZE)
		return 1;
	puts("Erasing Nand...\n");
	if (nand_erase_opts(&nand_info[0], &nand_erase_options))
		return 1;
	puts("Writing to Nand... ");
	if (writeenv(address, (u_char *) env_ptr)) {
		puts("FAILED!\n");
		return 1;
	}
	puts("done\n");
	return ret;
}

static env_t *flash_addr;

static int saveenv_flash(int address)
{
	int	len, rc;
	ulong	end_addr;
	ulong	flash_sect_addr;
#if defined(CONFIG_ENV_SECT_SIZE) && (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE)
	flash_addr = (env_t *) address;
	ulong	flash_offset;
	uchar	env_buffer[CONFIG_ENV_SECT_SIZE];
#else
	uchar *env_buffer = (uchar *)env_ptr;
#endif	/* CONFIG_ENV_SECT_SIZE */
	int rcode = 0;

#if defined(CONFIG_ENV_SECT_SIZE) && (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE)

	flash_offset    = ((ulong)flash_addr) & (CONFIG_ENV_SECT_SIZE-1);
	flash_sect_addr = ((ulong)flash_addr) & ~(CONFIG_ENV_SECT_SIZE-1);

	debug("copy old content: "
			"sect_addr: %08lX  env_addr: %08lX  offset: %08lX\n",
			flash_sect_addr, (ulong)flash_addr, flash_offset);

	/* copy old contents to temporary buffer */
	memcpy(env_buffer, (void *)flash_sect_addr, CONFIG_ENV_SECT_SIZE);

	/* copy current environment to temporary buffer */
	memcpy((uchar *)((unsigned long)env_buffer + flash_offset),
			env_ptr,
			CONFIG_ENV_SIZE);

	len	 = CONFIG_ENV_SECT_SIZE;
#else
	flash_sect_addr = (ulong)flash_addr;
	len	 = CONFIG_ENV_SIZE;
#endif	/* CONFIG_ENV_SECT_SIZE */

	end_addr = flash_sect_addr + len - 1;

	debug("Protect off %08lX ... %08lX\n",
			(ulong)flash_sect_addr, end_addr);

	if (flash_sect_protect(0, flash_sect_addr, end_addr))
		return 1;

	puts("Erasing Flash...");
	if (flash_sect_erase(flash_sect_addr, end_addr))
		return 1;

	puts("Writing to Flash... ");
	rc = flash_write((char *)env_buffer, flash_sect_addr, len);
	if (rc != 0) {
		flash_perror(rc);
		rcode = 1;
	} else {
		puts("done\n");
	}

	/* try to re-protect */
	(void) flash_sect_protect(1, flash_sect_addr, end_addr);
	return rcode;
}

static int do_saveenv(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (argc != 3) {
		cmd_usage(cmdtp);
		return 1;
	}
	int addr;
	addr = simple_strtoul(argv[2], NULL, 16);
	if (strcmp(argv[1], "flash") == 0) {
#ifdef CONFIG_CMD_FLASH
		printf("Saving Environment to flash...\n");
		return saveenv_flash(addr) ? 1 : 0;
#else
		printf("Flash memory not defined\n");
		return 1;
#endif
	} else if (strcmp(argv[1], "nand") == 0) {
#ifdef CONFIG_CMD_NAND
		printf("Saving Environment to nand...\n");
		return saveenv_nand(addr) ? 1 : 0;
#else
		printf("NAND memory not defined\n");
		return 1;
#endif
	} else {
		printf("Memory not specified\n");
		cmd_usage(cmdtp);
		return 1;
	}
}

U_BOOT_CMD(
		saveenv, 3, 0,  do_saveenv,
		"save environment variables to persistent storage",
		"flash <address>\n"
		"saveenv nand <offset>"
	  );
