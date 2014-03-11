#
# (C) Copyright 2008,2010-2014 STMicroelectronics.
# Sean McGoogan <Sean.McGoogan@st.com>
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#


# All the STMicroelectronics-specific stuff, which may be ARCH-agnostic
ifdef CONFIG_STM
obj-y += board.o
obj-y += stm-sysconf.o
obj-$(CONFIG_STM_USB) += usb.o
obj-$(CONFIG_USB_EHCI_STM) += stm-ehci-host.o
obj-$(CONFIG_STM_SATA) += sata-init.o sata-probe.o
obj-$(CONFIG_CMD_STTESTS) += cmd_sttests.o st_mtests.o
obj-$(CONFIG_STM_FSM_SPI) += stm_spi_fsm.o
obj-$(CONFIG_SPI) += spi.o
obj-$(CONFIG_ENV_IS_IN_EEPROM) += env_eeprom.o
obj-$(CONFIG_STM_ASC_SERIAL) += stm-asc.o
obj-$(CONFIG_STM_DTF_SERIAL) += stm-dtf.o
obj-$(CONFIG_DRIVER_NETSTMAC)     += stm-stmac.o
obj-$(CONFIG_DRIVER_NET_STM_GMAC) += stm-stmac.o
obj-$(CONFIG_STMAC_USE_RTL8367_SMI) += rtl8367_smi.o
obj-$(CONFIG_STM_SDHCI) += stm-sdhci.o

ifdef CONFIG_CMD_NAND
obj-y += ecc.o stm-nand.o
obj-$(CONFIG_SYS_STM_NAND_USE_BCH) += stm-nand-bch.o
obj-$(CONFIG_SYS_STM_NAND_USE_HAMMING) += stm-nand-flex.o
obj-$(CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING) += cmd_copybbt.o
endif # CONFIG_CMD_NAND

# finally, all the SoCs ...
obj-$(CONFIG_STM_FLI7510) += soc-fli7510.o
obj-$(CONFIG_STM_FLI7540) += soc-fli7540.o
obj-$(CONFIG_STM_STX5197) += soc-stx5197.o
obj-$(CONFIG_STM_STX5206) += soc-stx5206.o
obj-$(CONFIG_STM_STX7105) += soc-stx7105.o
obj-$(CONFIG_STM_STX7108) += soc-stx7108.o
obj-$(CONFIG_STM_STX7111) += soc-stx7111.o
obj-$(CONFIG_STM_STX7141) += soc-stx7141.o
obj-$(CONFIG_STM_STX7200) += soc-stx7200.o
obj-$(CONFIG_STM_STXH205) += soc-stxh205.o
obj-$(CONFIG_STM_STXH407) += soc-stxh407.o
obj-$(CONFIG_STM_STXH415) += soc-stxh415.o
obj-$(CONFIG_STM_STXH416) += soc-stxh416.o
endif # CONFIG_STM

ifdef CONFIG_ST40
$(obj)stm-nand-flex.o: stm-nand-flex.c
	$(CC) $(CFLAGS) -m4 -c -o $@ $<
endif


