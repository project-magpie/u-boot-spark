#n
#
# (C) Copyright 2013 STMicroelectronics.
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#


#------------------------------------------------------------------


#
# Pre-cursor:
#
#	Instrument linux to generate additional output for the following
#	two functions (in drivers/net/phy/rtl8366_smi.c):
#		__rtl8366_smi_write_reg()
#		rtl8366_smi_read_reg()
#
#	Then boot linux, and save the console output as (say) "minicom.cap",
#	then cat the console output, and pipe it through sed, as follows:
#
#
# Usage:
#
#	host% cat minicom.cap | sed -f rtl8367_smi_fw.sed > rtl8367_smi_fw.c
#


#------------------------------------------------------------------


#
#	The following was used to instrument a linux kernel to generate an accurate
#	record of all SMI register read/write accesses from/to a Reltek RTL8367RB
#	ethernet switch. This can then be used to "replay" the same SMI sequence
#	in the context of a smaller program (e.g. U-Boot), without having to
#	use such a large and extensive linux driver in the smaller program.
#
#	NOTE: This patch is purely just ephemeral "development" code, and it
#	is envisaged that this patch is of nugatory value, hence it should
#	probably not be upstreamed, nor committed to any significant repository.
#

#diff --git a/drivers/net/phy/rtl8366_smi.c b/drivers/net/phy/rtl8366_smi.c
#index 6dcfb32..fce938a 100644
#--- a/drivers/net/phy/rtl8366_smi.c
#+++ b/drivers/net/phy/rtl8366_smi.c
#@@ -236,6 +236,10 @@ int rtl8366_smi_read_reg(struct rtl8366_smi *smi, u32 addr, u32 *data)
# 	rtl8366_smi_stop(smi);
# 	spin_unlock_irqrestore(&smi->lock, flags);
# 
#+#if 1	/* QQQ - DELETE */
#+	printk("%s(): ret=%d, addr=%04x, data=%04x\n", __FUNCTION__, ret, addr, *data);	/* QQQ */
#+#endif	/* QQQ - DELETE */
#+
# 	return ret;
# }
# EXPORT_SYMBOL_GPL(rtl8366_smi_read_reg);
#@@ -284,6 +288,10 @@ static int __rtl8366_smi_write_reg(struct rtl8366_smi *smi,
# 	rtl8366_smi_stop(smi);
# 	spin_unlock_irqrestore(&smi->lock, flags);
# 
#+#if 1	/* QQQ - DELETE */
#+	printk("%s(): ret=%d, addr=%04x, data=%04x, ack=%d\n", __FUNCTION__, ret, addr, data, ack);	/* QQQ */
#+#endif	/* QQQ - DELETE */
#+
# 	return ret;
# }
# 
#-- 
#1.7.7.6


#------------------------------------------------------------------


#
#	Emit code to WRITE to a SMI register.
#	e.g. convert lines such as:
#[    3.340000] __rtl8366_smi_write_reg(): ret=0, addr=1f00, data=0001, ack=1
#
/ __rtl8366_smi_write_reg/{
	s/.* __rtl8366_smi_write_reg(): ret=0, addr=\(....\), data=\(....\), ack=1/SMI_WRITE(smi, 0x\1, 0x\2);/
	p
}


#
#	Emit code to READ (and optionally verify) from a SMI register.
#	e.g. convert lines such as:
#[    3.340000] rtl8366_smi_read_reg(): ret=0, addr=1f01, data=0000
#
/ rtl8366_smi_read_reg/{
	s/.* rtl8366_smi_read_reg(): ret=0, addr=\(....\), data=\(....\)/SMI_READ(smi, 0x\1, 0x\2);/
	p
}


