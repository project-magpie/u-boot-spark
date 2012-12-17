#
# (C) Copyright 2012
# Vipin Kumar, ST Microelectronics, vipin.kumar@st.com
#
# (C) Copyright 2000-2004
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

LIB	= $(obj)lib$(BOARD)ddr.o

SOBJS-y	:=
COBJS-y	:=

ifdef CONFIG_SPL_BUILD
COBJS-$(CONFIG_DDR_H5TQ2G63BFRPBC) += h5tq2g63bfr_pbc_400_cl6.o
COBJS-$(CONFIG_DDR_H5TQ2G63BFRPBC) += h5tq2g63bfr_pbc_533_cl8.o
COBJS-$(CONFIG_DDR_K4B2G1646CK0) += k4b2g1646c_k0_400_cl6.o
COBJS-$(CONFIG_DDR_K4B2G1646CK0) += k4b2g1646c_k0_533_cl8.o
COBJS-$(CONFIG_DDR_MT41J128M16) += mt41j128m16_15e_400_cl6_dqs_dq_term_en.o
COBJS-$(CONFIG_DDR_MT41J256M8) += mt41j256m8187e_400_cl6_dqs_dq_term_en.o
COBJS-$(CONFIG_DDR_MT41J256M8) += mt41j256m8187e_533_cl7_dqs_dq_term_en.o
COBJS-$(CONFIG_DDR_MT41J64M16) += mt41j64m16_15e_400_cl6_dqs_dq_term_en.o
COBJS-$(CONFIG_DDR_MT41J64M16) += mt41j64m16_15e_533_cl7_dqs_dq_term_en.o
COBJS-$(CONFIG_DDR_MT47H128M16) += mt47h128m16rt187e_400_cl6_ddr2.o
COBJS-$(CONFIG_DDR_MT47H128M16) += mt47h128m16rt187e_533_cl7_ddr2.o
endif

COBJS	:= $(sort $(COBJS-y))
SOBJS	:= $(sort $(SOBJS-y))
SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))
SOBJS	:= $(addprefix $(obj),$(SOBJS))

$(LIB):	$(obj).depend $(OBJS) $(SOBJS)
	$(call cmd_link_o_target, $(OBJS) $(SOBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
