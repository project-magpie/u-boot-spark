#
# (C) Copyright 2000-2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# (C) Copyright 2008
# Guennadi Liakhovetki, DENX Software Engineering, <lg@denx.de>
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

SOBJS-y	:=
COBJS-y	:=

COBJS-$(CONFIG_ARCH_SPEAR13XX) += spear13xx.o
COBJS-$(CONFIG_SOC_SPEAR1310) += spear1310.o
COBJS-$(CONFIG_SOC_SPEAR1310) += spear1310-pinmux.o
COBJS-$(CONFIG_SOC_SPEAR1340) += spear1340.o
COBJS-$(CONFIG_SOC_SPEAR1340) += spear1340-pinmux.o

ifdef CONFIG_SPL_BUILD
SOBJS-y	+= wakeup.o

COBJS-y	+= spl-boot.o
COBJS-$(CONFIG_ARCH_SPEAR13XX) += spl-spear13xx.o
COBJS-$(CONFIG_ARCH_SPEAR13XX) += spl-lvl_gatetrn.o
COBJS-$(CONFIG_ARCH_SPEAR13XX) += spl-lvl_read.o
COBJS-$(CONFIG_ARCH_SPEAR13XX) += spl-lvl_write.o
COBJS-$(CONFIG_ARCH_SPEAR13XX) += spl-swlvl.o
COBJS-$(CONFIG_SOC_SPEAR1340) += spl-spear1340.o
endif

COBJS	:= $(sort $(COBJS-y))
SOBJS	:= $(sort $(SOBJS-y))
SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))
SOBJS	:= $(addprefix $(obj),$(SOBJS))

all:	$(obj).depend $(LIB)

$(LIB):	$(OBJS) $(SOBJS)
	$(call cmd_link_o_target, $(OBJS) $(SOBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
