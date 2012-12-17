#
# (C) Copyright 2000-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

LIB	= $(obj)lib$(SOC).o

COBJS-y += cache.o
COBJS-y += cpu.o
COBJS-y += reset.o
COBJS-y += timer.o
COBJS-$(CONFIG_ARCH_SPEAR300) += spear300.o
COBJS-$(CONFIG_ARCH_SPEAR310) += spear310.o
COBJS-$(CONFIG_ARCH_SPEAR320) += spear320.o
SOBJS	:=

COBJS-$(CONFIG_ARCH_SPEAR3XX) += spear3xx.o
COBJS-$(CONFIG_SOC_SPEAR300) += spear300.o
COBJS-$(CONFIG_SOC_SPEAR310) += spear310.o
COBJS-$(CONFIG_SOC_SPEAR320) += spear320.o
COBJS-$(CONFIG_ARCH_SPEAR6XX) += spear6xx.o
COBJS-$(CONFIG_ST_EMI) += emi.o

ifdef CONFIG_SPL_BUILD
COBJS-y	+= spl.o spl_boot.o spl_nand.o
COBJS-$(CONFIG_ARCH_SPEAR3XX) += spl-spear3xx.o
COBJS-$(CONFIG_SOC_SPEAR600) += spl-spear600.o
endif

SRCS	:= $(COBJS-y:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS-y))

all:	$(obj).depend $(LIB)

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
