/*
 * (C) Copyright 2004,2005 STMicroelectronics.
 *
 * Original port by Andy Sturges <andy.sturges@st.com>
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <asm/io.h>

void insw(unsigned long addr, void *dst, unsigned long count)
{
	int i;
	volatile unsigned short *p=(volatile unsigned short *)dst;

	for(i=0;i<count;i++) {
		p[i]=inw(addr);
	}

}
void outsw(unsigned long addr, const void *src, unsigned long count)
{
	int i;
	volatile unsigned short *p=(volatile unsigned short*)src;

	for(i=0;i<count;i++) {
		outw(p[i],addr);
	}
}
