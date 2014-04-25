#
# (C) Copyright 2009-2010 STMicroelectronics.
# Sean McGoogan <Sean.McGoogan@st.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

#
# Freeman-Ultra (FLI7540) Development Board:
#
#	Valid values for CONFIG_SYS_TEXT_BASE are:
#
#	0x87F00000	32-bit mode (Space-Enhancement Mode)
#

ifndef CONFIG_SYS_TEXT_BASE
# Installs at LMI0 BASE + 127MB in P1 (cachable)
CONFIG_SYS_TEXT_BASE = 0x87F00000
endif

PLATFORM_LDFLAGS +=
