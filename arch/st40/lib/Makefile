#
# (C) Copyright 2003-2004
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

LIB	= $(obj)lib$(ARCH).a

SOBJS-y = memchr.o memcpy.o memmove.o memset.o strlen.o cacheops.o div64.o

COBJS-y = board.o bootm.o time.o io_generic.o udivdi3.o sconsole.o

# Object files to use when CONFIG_SYS_BOOT_ARM_IMAGE is defined.
SOBJS-$(CONFIG_SYS_BOOT_ARM_IMAGE) += armlinuxprep.o
COBJS-$(CONFIG_SYS_BOOT_ARM_IMAGE) += bootm_for_arm.o stxp70armboothelper.o

SRCS 	:= $(SOBJS-y:.o=.S) $(COBJS-y:.o=.c)
OBJS	:= $(addprefix $(obj),$(SOBJS-y) $(COBJS-y))

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
