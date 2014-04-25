#
# (C) Copyright 2010 STMicroelectronics.
# Sean McGoogan <Sean.McGoogan@st.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

#
# HDK5289 board:
#
#	Valid values for CONFIG_SYS_TEXT_BASE are:
#
#	0x93F00000	29-bit mode (Traditional Mode)
#	0x87F00000	32-bit mode (Space-Enhancement Mode)
#

ifndef CONFIG_SYS_TEXT_BASE
# Installs at LMI0 BASE + 127MB in P1 (cachable)
CONFIG_SYS_TEXT_BASE = 0x93F00000
endif

PLATFORM_LDFLAGS +=
