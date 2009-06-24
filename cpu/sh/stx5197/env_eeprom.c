/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * (C) Copyright 2009 STMicroelectronics Ltd.
 * Sean McGoogan <Sean.McGoogan@st.com>

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

/*
 * NOTE: For the ST40 series of SoCs, we use the "eeprom" set of
 * commands to access SPI serial flash memory devices.
 *
 * Hence we need to define CFG_ENV_IS_IN_EEPROM is we want the
 * environment stored in SPI serial flash.
 *
 * This file allows us to store U-boot's environment in SPI
 * serial flash, and boot from SPI using the SPIBOOT mode
 * controller in the STMicroelectronics' EMI.
 *
 * Sometimes (during initialization) we access the SPI via the
 * SPIBOOT mode controller, other times (post-initialization)
 * we access the SPI serial flash via the SSC.
 * This file tries to take care of dealing with the
 * duality aspects of all this.
 */
#if defined(CFG_ENV_IS_IN_EEPROM) && defined(CFG_BOOT_FROM_SPI)

#include <command.h>
#include <environment.h>
#include <linux/stddef.h>

DECLARE_GLOBAL_DATA_PTR;

env_t *env_ptr = NULL;

char * env_name_spec = "SPI Serial Flash";

extern uchar (*env_get_char)(int);
extern uchar env_get_char_memory (int index);


	/*
	 * The following function will read a 32-bit value
	 * from the serial flash, via the SPIBOOT controller,
	 * whilst in SPI Boot mode.
	 */
static inline u32 spiboot_get_u32(const int index)
{
	return *(u32*)(CFG_EMI_SPI_BASE + index);
}


	/*
	 * The following function will read a byte
	 * from the serial flash, via the SPIBOOT
	 * controller, whilst in SPI boot mode.
	 * NOTE: we *need* to perform a 4-byte read, and only
	 * extract the one byte we are really interested in.
	 */
static inline uchar spiboot_get_byte(const int index)
{
	const u32 word = spiboot_get_u32(index & ~0x3u);
	return ( ( word >> (index%4)*8 ) & 0xffu );
}


extern uchar env_get_char_spec (int index)
{
	uchar c;

	if ( (env_ptr==NULL) ||
	     (gd->env_addr != (ulong)&(env_ptr->data)) )
	{
		/*
		 * Not relocated the environment yet.
		 * So the SPI is not yet initialized, so use the SPI-BOOT
		 * controller to read the environment directly via the EMI.
		 */
		c = spiboot_get_byte(CFG_ENV_OFFSET + index);
	}
	else
	{
		/*
		 * else, serial flash accessed via SSC, so
		 * we can need to use the normal SPI interface
		 * to talk to the serial flash device.
		 */
		eeprom_read (CFG_DEF_EEPROM_ADDR,
			     CFG_ENV_OFFSET+index+offsetof(env_t,data),
			     &c, 1);
	}

	return (c);
}


extern void env_relocate_spec (void)
{
	const uchar * const from = (uchar*)(CFG_EMI_SPI_BASE + CFG_ENV_OFFSET);

#if 0
	printf("\nQQQ: relocating %u bytes from %p to %p ...\n\n",
		CFG_ENV_SIZE, from, env_ptr);
#endif
	memcpy( (uchar*)env_ptr, from, CFG_ENV_SIZE);
}


extern int saveenv(void)
{
	return eeprom_write (CFG_DEF_EEPROM_ADDR,
			     CFG_ENV_OFFSET,
			     (uchar *)env_ptr,
			     CFG_ENV_SIZE);
}


/************************************************************************
 * Initialize Environment use
 *
 * We are still running from ROM, so data use is limited
 * Use a (moderately small) buffer on the stack
 *
 * Note: as we are using SPIBOOT mode to read the serial flash,
 * then we need to perform 32-bit accesses only. Hence, we loop
 * whilst reading 4-bytes into 'buf' and then update the CRC.
 */
extern int env_init(void)
{
	ulong crc, len, new;
	unsigned off;
	uchar buf[4];		/* only do 32-bits per iteration */

	/* read old CRC (from flash) */
	crc = spiboot_get_u32(CFG_ENV_OFFSET + offsetof(env_t,crc));

	/* compute current CRC */
	new = 0;
	len = ENV_SIZE;
	off = CFG_ENV_OFFSET + offsetof(env_t,data);
	while (len > 0)
	{
		int n = (len > sizeof(buf)) ? sizeof(buf) : len;
		*(ulong*)buf = spiboot_get_u32(off);
		new = crc32 (new, buf, n);
		len -= n;
		off += n;
	}

	/* is the flash environment good ? */
	if (crc == new)
	{
		gd->env_addr  = CFG_ENV_OFFSET + offsetof(env_t,data);
		gd->env_valid = 1;
	}
	else
	{
		gd->env_addr  = 0;
		gd->env_valid = 0;
	}

	return (0);
}


#endif /* CFG_ENV_IS_IN_EEPROM && CFG_BOOT_FROM_SPI */


