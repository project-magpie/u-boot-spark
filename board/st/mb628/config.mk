#
# (C) Copyright 2008-2012 STMicroelectronics.
# Sean McGoogan <Sean.McGoogan@st.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

#
# MB628 board:
#
#	Following are *Physical* Addresses:
#
#	Region		29-bit		32-bit		Size
#	------		------		------		----
#	LMI_BASE	0x0c000000	0x40000000	128MiB
#	VIDEO_BASE	0x0c000000	0x40000000	  4MiB
#	AUDIO_BASE	0x0c400000	0x40400000	  4MiB
#	eSTB_BASE	0x0c800000	0x40800000	 50MiB
#	eSTB_END	0x0f9fffff	0x439fffff	   ---
#
#	Note: to ensure that CONFIG_SYS_SDRAM_SIZE is strictly a
#	multiple of 16 MiB (for PMB alignment purposes), then
#	we will cap the size of the eSTB allocation to 48 MiB.
#
#
#	Following are *Virtual* Addresses:
#
#	Valid values for CONFIG_SYS_TEXT_BASE are:
#
#	0x8F700000	29-bit mode (Traditional Mode)
#	0x83700000	32-bit mode (Space-Enhancement Mode)
#

ifndef CONFIG_SYS_TEXT_BASE
# Installs at eSTB BASE + 47MB in P1 (cachable)
CONFIG_SYS_TEXT_BASE = 0x8F700000
endif

PLATFORM_LDFLAGS +=
