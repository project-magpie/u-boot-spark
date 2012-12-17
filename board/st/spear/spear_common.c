/*
 * (C) Copyright 2009
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

#include <common.h>
#include <command.h>
#include <i2c.h>
#include <net.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/generic.h>

#define CPU		0
#define DDR		1
#define SRAM_REL	0xD2801000

DECLARE_GLOBAL_DATA_PTR;

void lowlevel_init(void)
{
}

int checkboard(void)
{
#ifdef CONFIG_MACH_SPEAR300EVB
	printf("BOARD: SPEAr300-EVB\n");
#elif defined(CONFIG_MACH_SPEAR310EVB)
	printf("BOARD: SPEAr310-EVB\n");
#elif defined(CONFIG_MACH_SPEAR320EVB)
	printf("BOARD: SPEAr320-PLC\n");
#elif defined(CONFIG_MACH_SPEAR320HMI)
	printf("BOARD: SPEAr320-HMI\n");
#elif defined(CONFIG_MACH_SPEAR600EVB)
	printf("BOARD: SPEAr600-EVB\n");
#else
#error BOARD not supported
#endif
	return 0;
}

int dram_init(void)
{
	/* Store complete RAM size and return */
	gd->ram_size = get_ram_size(CONFIG_SYS_SDRAM_BASE, SDRAM_MAX_SIZE);

	return 0;
}

#if defined(CONFIG_SPEAR_MACID_IN_I2CMEM) && defined(CONFIG_CMD_NET) && \
	defined(CONFIG_CMD_I2C)
int i2c_read_mac(uchar *buffer)
{
	u8 buf[2];

	/*
	 * A magic ID which is present at offset 0 and reads 0x55AA represents
	 * whether MACID is present starting at offset 2
	 */
	i2c_read(CONFIG_I2C_CHIPADDRESS, 0, 1, buf, 2);

	/* Check if mac in i2c memory is valid */
	if ((buf[0] == 0x55) && (buf[1] == 0xAA)) {
		/* Valid mac address is saved in i2c eeprom */
		i2c_read(CONFIG_I2C_CHIPADDRESS, 0x2, 1, buffer, 6);
		return 0;
	}

	return -1;
}

static int i2c_write_mac(uchar *mac)
{
	u8 buf[2];
	u8 temp[MAC_LEN];
	int i;

	buf[0] = (u8)MAGIC_BYTE0;
	buf[1] = (u8)MAGIC_BYTE1;
	i2c_write(CONFIG_I2C_CHIPADDRESS, MAGIC_OFF, 1, buf, MAGIC_LEN);

	buf[0] = (u8)~MAGIC_BYTE0;
	buf[1] = (u8)~MAGIC_BYTE1;

	i2c_read(CONFIG_I2C_CHIPADDRESS, MAGIC_OFF, 1, buf, MAGIC_LEN);

	/* check if valid MAC address is saved in I2C EEPROM or not? */
	if ((buf[0] == MAGIC_BYTE0) && (buf[1] == MAGIC_BYTE1)) {
		i2c_write(CONFIG_I2C_CHIPADDRESS, MAC_OFF, 1, mac, MAC_LEN);

		for (i = 0; i < MAC_LEN; i++)
			temp[i] = ~mac[i];
		i2c_read(CONFIG_I2C_CHIPADDRESS, MAC_OFF, 1, temp, MAC_LEN);

		if (!memcmp(mac, temp, MAC_LEN)) {
			puts("I2C EEPROM written with mac address\n");
			return 0;
		}
	}

	puts("I2C EEPROM writing failed\n");
	return -1;
}
#endif

int do_chip_config(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	void (*sram_setfreq) (unsigned int, unsigned int);
	unsigned int frequency;
#if defined(CONFIG_SPEAR_MACID_IN_I2CMEM) && defined(CONFIG_CMD_NET) && \
	defined(CONFIG_CMD_I2C)
	unsigned char mac[6];
#endif

	if ((argc > 3) || (argc < 2))
		return cmd_usage(cmdtp);

	if ((!strcmp(argv[1], "cpufreq")) || (!strcmp(argv[1], "ddrfreq"))) {

		frequency = simple_strtoul(argv[2], NULL, 0);

		if (frequency > 333) {
			printf("Frequency is limited to 333MHz\n");
			return 1;
		}

		sram_setfreq = memcpy((void *)SRAM_REL, setfreq, setfreq_sz);

		if (!strcmp(argv[1], "cpufreq")) {
			sram_setfreq(CPU, frequency);
			printf("CPU frequency changed to %u\n", frequency);
		} else {
			sram_setfreq(DDR, frequency);
			printf("DDR frequency changed to %u\n", frequency);
		}

		return 0;

#if defined(CONFIG_SPEAR_MACID_IN_I2CMEM) && defined(CONFIG_CMD_NET) && \
	defined(CONFIG_CMD_I2C)
	} else if (!strcmp(argv[1], "ethaddr")) {

		u32 reg;
		char *e, *s = argv[2];
		for (reg = 0; reg < 6; ++reg) {
			mac[reg] = s ? simple_strtoul(s, &e, 16) : 0;
			if (s)
				s = (*e) ? e + 1 : e;
		}
		i2c_write_mac(mac);

		return 0;
	} else if (!strcmp(argv[1], "print")) {
		if (!i2c_read_mac(mac)) {
			printf("Ethaddr (from i2c mem) = %pM\n", mac);
		} else {
			printf("Ethaddr (from i2c mem) = Not set\n");
		}

		return 0;
#endif
	}

	return cmd_usage(cmdtp);
}

U_BOOT_CMD(chip_config, 3, 1, do_chip_config,
	   "configure chip",
	   "chip_config cpufreq/ddrfreq frequency"
#if defined(CONFIG_SPEAR_MACID_IN_I2CMEM) && defined(CONFIG_CMD_NET) && \
	defined(CONFIG_CMD_I2C)
	   "\n"
	   "chip_config ethaddr XX:XX:XX:XX:XX:XX\n"
	   "chip_config print"
#endif
	   "");
