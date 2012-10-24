/*
 * (C) Copyright 2007-2009,2011-2012 STMicroelectronics.
 *
 * Stuart Menefy <stuart.menefy@st.com>
 * Sean McGoogan <Sean.McGoogan@st.com>
 *
 * (C) Copyright 2004 STMicroelectronics.
 *
 * Andy Sturges <andy.sturges@st.com>
 *
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

OUTPUT_ARCH(sh4)
ENTRY(_start)


	/*
	 * Both the "u-boot" and "u-boot.bin" images contain the following
	 * allocated and loadable sections:
	 *	".text", ".rodata", ".data", and ".u_boot_cmd".
	 * Whereas, the STACK, HEAP, and the ".bss" sections are all *not*
	 * allocated/loadable, hence they are not physically present in these images.
	 *
	 * When booting from GDB, by default, GDB loads all the loadable
	 * sections into RAM, using their LMA (Load Memory Address). Normally,
	 * the LMA is the same as the VMA (Virtual Memory Address).
	 *
	 * When booting from FLASH, the "u-boot.bin" binary is burned into
	 * FLASH in its entirety, and it copies itself as a homogeneous binary
	 * blob from FLASH to RAM, and once relocated, runs directly out of RAM.
	 *
	 * When booting from RAM, U-Boot is essentially a Secondary Boot Loader (SBL),
	 * and some other "entity" has performed the duty of a primary boot loader
	 * (PBL, or another booting mechanism). In this case, U-Boot does not need to
	 * perform the pokes in the "poke-table", or even relocate itself.
	 *
	 * However, boot-from-RAM, has one further (optional) complexity. It is
	 * possible that when U-Boot has been coped to RAM, that the region of RAM
	 * containing the u-boot.bin image is then marked as READ-ONLY by the underlying
	 * hardware system, and U-Boot should not relocate itself, but run
	 * in-place, directly out the RAM. This is not a problem for the ".text", and
	 * the ".rodata" sections, as (in general) these should be read-only. It is not
	 * a problem for the STACK, HEAP, and the ".bss" sections, as these should never
	 * be positioned in a read-only region of RAM. However, this presents a significant
	 * problem for the ".data", and the ".u_boot_cmd" sections, which must be
	 * stored in a READ-WRITE region of RAM, but are also physically stored in
	 * the u-boot.bin binary image which is now in a READ-ONLY region of RAM!
	 *
	 * To solve this last problem, we need to "re-relocate" the READ-WRITE sections
	 * that are allocated/loadable from RAM to RAM when booting from RAM. To do
	 * this, we need to do a few things:
	 *
	 *	1) place ".data" at a VMA where the CPU will normally access it
	 *
	 *	2) place ".data" at a LMA where it physically abuts after ".rodata".
	 *
	 *	3) when U-Boot starts, copy ".data" from the LMA to the VMA.
	 *
	 *	Note: for ".data", read all sections that need to be read-write.
	 *
	 *	Note: this file takes care of points #1, and #2 (but not #3).
	 *
	 * To control if the "normal" (VMA==LMA) placement scheme is used,
	 * or if a "special" (VMA!=LMA) placement scheme is to be used, the
	 * linker symbol "__USE_VMA_DATA" will be used, with the following semantics:
	 *
	 *	if (__USE_VMA_DATA == 0)
	 *	{
	 *		use the "normal" (or traditional) placement scheme.
	 *		place ".data" VIRTUALLY+PHYSICALLY at end of ".rodata".
	 *		i.e. VMA == LMA
	 *	}
	 *	else
	 *	{	
	 *		use a "special" (non-normal) placement scheme.
	 *		place ".data" PHYSICALLY at end of ".rodata" (LMA).
	 *		place ".data" VIRTUALLY  at __USE_VMA_DATA (VMA).
	 *		i.e. VMA != LMA
	 *		"start.S" also needs to perform an additional relocation!
	 *	}
	 *
	 * Note - later definitions will dominate earlier definitions.
	 */
__USE_VMA_DATA = _start - (15<<20);	/* Use a "SPECIAL-case" as the placement scheme. */
__USE_VMA_DATA = 0;			/* Use the NORMAL, and default placement scheme. */


SECTIONS
{
	/* First, the READ-ONLY sections, that are not modifiable */

	.text ALIGN(0x4):
	{
		__text_start = .;
		cpu/st40/start.o (.text.init)
		*(.text.init)		/* pre-initialization code */
		. = ALIGN(32);		/* align to a cache-line */
		*(.data.init)		/* pre-initialization data */
		*(.text*)		/* post-initialization code */
		__text_end = .;
	}

	.rodata ALIGN(0x4):
	{
		__rodata_start = .;
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
		__rodata_end = .;
	}

	/* Now, the READ-WRITE sections that we ALWAYS need to run from (writable) RAM */

	.dummy ALIGN(0x4): { }				/* A syntactic necessity! */
	__data_lma_start = .;				/* PHYSICAL address (LMA) in file */
	. = (__USE_VMA_DATA) ? (__USE_VMA_DATA) : (.);	/* VIRTUAL  address (VMA) in CPU */
	__data_vma_start = .;

	.data ALIGN(0x4): AT(__data_lma_start)
	{
		__data_start = .;
		*(.data)
		__data_end = .;
	}

	.u_boot_cmd ALIGN(0x4):
	{
		__u_boot_cmd_start = .;
		*(.u_boot_cmd)
		__u_boot_cmd_end = .;
	}

	__data_vma_end = .;

	.bss ALIGN(0x4) (NOLOAD):
	{
		__bss_start = .;
		*(.bss)
		*(COMMON)		/* ensure common data is initialized */
		. = ALIGN(0x4);		/* ensure .bss ends on 4-byte alignment */
		__bss_end = .;
	}

	/*
	 * Ideally, the .data section should abut with the .text section.
	 * However, we need to make sure that the end of the ".data" section
	 * does not actually overlap with the ".text" section.
	 */
	ASSERT((__USE_VMA_DATA) ? (. <= __text_start) : (1), "ERROR: .data located too close to TEXT_BASE! (move it!)")
}


	/*
	 * Finally, create a symbol which should be the size of "u-boot.bin".
	 * That is, the size of the image which is to written to FLASH.
	 */
__uboot_bin_size = LOADADDR(.bss) - LOADADDR(.text);
