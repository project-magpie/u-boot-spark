#
# (C) Copyright 2008,2010-2013 STMicroelectronics.
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

include $(TOPDIR)/config.mk

LIB	= $(obj)libstm.a

# All the STMicroelectronics-specific stuff, which may be ARCH-agnostic
ifdef CONFIG_STM
COBJS-y += board.o
COBJS-y += stm-sysconf.o
COBJS-$(CONFIG_STM_USB) += usb.o
COBJS-$(CONFIG_STM_SATA) += sata-init.o sata-probe.o
COBJS-$(CONFIG_CMD_STTESTS) += cmd_sttests.o st_mtests.o
COBJS-$(CONFIG_STM_FSM_SPI) += stm_spi_fsm.o
COBJS-$(CONFIG_SPI) += spi.o
COBJS-$(CONFIG_ENV_IS_IN_EEPROM) += env_eeprom.o

ifdef CONFIG_CMD_NAND
COBJS-y += ecc.o stm-nand.o
COBJS-$(CONFIG_SYS_STM_NAND_USE_BCH) += stm-nand-bch.o
COBJS-$(CONFIG_SYS_STM_NAND_USE_HAMMING) += stm-nand-flex.o
COBJS-$(CONFIG_SYS_NAND_SKIP_BAD_BLOCKS_ON_RELOCATING) += cmd_copybbt.o
endif # CONFIG_CMD_NAND

# finally, all the SoCs ...
COBJS-$(CONFIG_STM_FLI7510) += soc-fli7510.o
COBJS-$(CONFIG_STM_FLI7540) += soc-fli7540.o
COBJS-$(CONFIG_STM_STX5197) += soc-stx5197.o
COBJS-$(CONFIG_STM_STX5206) += soc-stx5206.o
COBJS-$(CONFIG_STM_STX7105) += soc-stx7105.o
COBJS-$(CONFIG_STM_STX7108) += soc-stx7108.o
COBJS-$(CONFIG_STM_STX7111) += soc-stx7111.o
COBJS-$(CONFIG_STM_STX7141) += soc-stx7141.o
COBJS-$(CONFIG_STM_STX7200) += soc-stx7200.o
COBJS-$(CONFIG_STM_STXH205) += soc-stxh205.o
COBJS-$(CONFIG_STM_STXH415) += soc-stxh415.o
COBJS-$(CONFIG_STM_STXH416) += soc-stxh416.o
endif # CONFIG_STM

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

ifdef CONFIG_ST40
$(obj)stm-nand-flex.o: stm-nand-flex.c
	$(CC) $(CFLAGS) -m4 -c -o $@ $<
endif

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
