#
# (C) Copyright 2008,2010-2012 STMicroelectronics.
# Sean McGoogan <Sean.McGoogan@st.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

#
# STiH207-HDK (B2057) board:
# STiH238-HDK (B2067) board:
# STiH239-HDK (B2064) board:
# STiH205-HDK (B2075) board:
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
