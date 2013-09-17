#
# (C) Copyright 2004-2013
# Andy Sturges, STMicroelectronics, andy.sturges@st.com
# Sean McGoogan STMicroelectronics, <Sean.McGoogan@st.com>
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

include $(TOPDIR)/stm.mk

CROSS_COMPILE ?= sh4-linux-

sinclude $(OBJTREE)/include/autoconf.mk
CONFIG_STANDALONE_LOAD_ADDR ?= $(CONFIG_SYS_SDRAM_BASE)

PLATFORM_CPPFLAGS += -DCONFIG_ST40 -D__SH4__ -DCONFIG_IDENT_STRING=$(STM_IDENT_STRING)
PLATFORM_LDFLAGS  += -n
