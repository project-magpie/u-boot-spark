#
# (C) Copyright 2007-2008 STMicroelectronics.
# Stuart Menefy <stuart.menefy@st.com>
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

#
# MB671 board:
#
#	Valid values for CONFIG_SYS_TEXT_BASE are:
#
#	0x97F00000	29-bit mode (Traditional Mode)
#	0x8FF00000	32-bit mode (Space-Enhancement Mode)
#
# Note:	Alternative definitions of CONFIG_SYS_TEXT_BASE are put into
#	'config.tmp' from the top-level 'Makefile'.
#

sinclude $(OBJTREE)/board/$(BOARDDIR)/config.tmp

ifndef CONFIG_SYS_TEXT_BASE
# Installs at LMI0 BASE + 255MB in P1 (cachable)
CONFIG_SYS_TEXT_BASE = 0x97F00000
endif

PLATFORM_LDFLAGS +=