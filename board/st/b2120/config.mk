#
# (C) Copyright 2008-2013 STMicroelectronics.
# Sean McGoogan <Sean.McGoogan@st.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

#
# B2120 board:
#
#	Valid values for CONFIG_SYS_TEXT_BASE are:
#
#	0xBFF00000
#

ifndef CONFIG_SYS_TEXT_BASE
CONFIG_SYS_TEXT_BASE = 0xBFF00000
endif

PLATFORM_LDFLAGS +=
