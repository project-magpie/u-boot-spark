#
# (C) Copyright 2008 STMicroelectronics.
# Sean McGoogan <Sean.McGoogan@st.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

#
# HDK-7106 board:
#
#	Valid values for CONFIG_SYS_TEXT_BASE are:
#
#	0x9BF00000	29-bit mode (Traditional Mode)
#	0x8FF00000	32-bit mode (Space-Enhancement Mode)
#

ifndef CONFIG_SYS_TEXT_BASE
# Installs at LMI0 BASE + 255MB in P1 (cachable)
CONFIG_SYS_TEXT_BASE = 0x9BF00000
endif

PLATFORM_LDFLAGS +=
