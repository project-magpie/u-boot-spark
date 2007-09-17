/*
 * include/asm-sh4/sh4_202.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004, STMicroelectronics
 */

#ifndef __ASM_SH4_202_H
#define __ASM_SH4_202_H

#define DMA_BASE_ADDRESS    0xFFA00004
#define TMU_BASE_ADDRESS    0xffd80000
#define RTC_BASE_ADDRESS    0xffc80000

#define CPG_BASE_ADDRESS    0xffc00000
#define INTC_BASE_ADDRESS   0xffd00000
#define INTC2_BASE_ADDRESS  0xfe080000
#define SCIF_BASE_ADDRESS   0xffe80000
#define CPG2_BASE_ADDRESS   0xfe0a0000

/*  SH42xx CPG */
/* Clock Pulse Generator control registers */

#define CPG_FRQCR SH4_REG((volatile unsigned short *),(CPG_BASE_ADDRESS+0x00))
#define CPG_STBCR SH4_REG((volatile unsigned char *),(CPG_BASE_ADDRESS+0x04))
#define CPG_WTCNT SH4_REG((volatile unsigned short *),(CPG_BASE_ADDRESS+0x08))
#define CPG_WTCSR SH4_REG((volatile unsigned short *),(CPG_BASE_ADDRESS+0x0c))
#define CPG_STBCR2 SH4_REG((volatile unsigned char *),(CPG_BASE_ADDRESS+0x10))
#define CPG_FRQCR2 SH4_REG((volatile unsigned short *),(CPG_BASE_ADDRESS+0xc0))
#define CPG_FRQCR3 SH4_REG((volatile unsigned int *),(0xfe0a0018))

/****/

/* Clock Pulse Generator control registers */

#define CPG2_CLKSTP00 SH4_REG((volatile unsigned int *),(CPG2_BASE_ADDRESS+0x00))
#define CPG2_CLKSTPCLR00 SH4_REG((volatile unsigned int *),(CPG2_BASE_ADDRESS+0x08))
#define CPG2_CLKSTPACK00 SH4_REG((volatile unsigned int *),(CPG2_BASE_ADDRESS+0x10))
#define CPG2_FRQCR3 SH4_REG((volatile unsigned int *),(CPG2_BASE_ADDRESS+0x18))

/****/

/*  SH42xx INTC */
/* Interrupt controller registers */

#define INTC_ICR SH4_REG((volatile unsigned short *),(INTC_BASE_ADDRESS+0x00))
#define INTC_IPRA SH4_REG((volatile unsigned short *),(INTC_BASE_ADDRESS+0x04))
#define INTC_IPRB SH4_REG((volatile unsigned short *),(INTC_BASE_ADDRESS+0x08))
#define INTC_IPRC SH4_REG((volatile unsigned short *),(INTC_BASE_ADDRESS+0x0c))
#define INTC_IPRD SH4_REG((volatile unsigned short *),(INTC_BASE_ADDRESS+0x10))

/****/

/*  SH42xx INTC2 */
/* ST Interrupt Controller control registers */

#define INTC2_INTPRI00 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x00))
#define INTC2_INTPRI04 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x04))
#define INTC2_INTPRI08 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x08))
#define INTC2_INTREQ00 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x20))
#define INTC2_INTREQ04 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x24))
#define INTC2_INTREQ08 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x28))
#define INTC2_INTMSK00 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x40))
#define INTC2_INTMSK04 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x44))
#define INTC2_INTMSK08 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x48))
#define INTC2_INTMSKCLR00 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x60))
#define INTC2_INTMSKCLR04 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x64))
#define INTC2_INTMSKCLR08 SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x68))
#define INTC2_INTC2MODE SH4_REG((volatile unsigned int *),(INTC2_BASE_ADDRESS+0x80))

/****/

/*  SH42xx SCIF */
/* Serial Communication Interfaces control registers */

#define SCIF_SCSMR SH4_REG((volatile unsigned short *),(SCI_BASE_ADDRESS+0x00))
#define SCIF_SCBRR SH4_REG((volatile unsigned char *),(SCI_BASE_ADDRESS+0x04))
#define SCIF_SCSCR SH4_REG((volatile unsigned short *),(SCI_BASE_ADDRESS+0x08))
#define SCIF_SCFTDR SH4_REG((volatile unsigned char *),(SCI_BASE_ADDRESS+0x0c))
#define SCIF_SCFSR SH4_REG((volatile unsigned short *),(SCI_BASE_ADDRESS+0x10))
#define SCIF_SCFRDR SH4_REG((volatile unsigned char *),(SCI_BASE_ADDRESS+0x14))
#define SCIF_SCFCR SH4_REG((volatile unsigned short *),(SCI_BASE_ADDRESS+0x18))
#define SCIF_SCFDR SH4_REG((volatile unsigned short *),(SCI_BASE_ADDRESS+0x1c))
#define SCIF_SCSPTR SH4_REG((volatile unsigned short *),(SCI_BASE_ADDRESS+0x20))
#define SCIF_SCLSR SH4_REG((volatile unsigned short *),(SCI_BASE_ADDRESS+0x24))


/*  SH42xx EMI */
/* External Memory Interface control registers */


#define EMI_VCR_0 SH4_REG((volatile unsigned int *),0xfec00000)
#define EMI_VCR_1 SH4_REG((volatile unsigned int *),0xfec00004)
#define EMI_MIM SH4_REG((volatile unsigned int *),0xfec0000c)
#define EMI_SCR SH4_REG((volatile unsigned int *),0xfec00014)
#define EMI_STR SH4_REG((volatile unsigned int *),0xfec0001c)
#define EMI_COC SH4_REG((volatile unsigned int *),0xfec0002c)
#define EMI_SDRA0 SH4_REG((volatile unsigned int *),0xfec00034)
#define EMI_SDRA1 SH4_REG((volatile unsigned int *),0xfec0003c)
#define EMI_SDMR0 SH4_REG((volatile unsigned int *),0xfed00000)
#define EMI_SDMR1 SH4_REG((volatile unsigned int *),0xfee00000)


/*  SH42xx FEMI */
/* Flash External Memory Interface control registers */

#define FEMI_VCR_0 SH4_REG((volatile unsigned int *),0xff800000)
#define FEMI_VCR_1 SH4_REG((volatile unsigned int *),0xff800004)
#define FEMI_MDCR SH4_REG((volatile unsigned int *),0xff80000c)
#define FEMI_A0MCR SH4_REG((volatile unsigned int *),0xff800014)
#define FEMI_A1MCR SH4_REG((volatile unsigned int *),0xff80001c)
#define FEMI_A2MCR SH4_REG((volatile unsigned int *),0xff800024)
#define FEMI_A3MCR SH4_REG((volatile unsigned int *),0xff80002C)
#define FEMI_A4MCR SH4_REG((volatile unsigned int *),0xff800034)

#define DMA_VCR_L SH4_REG((volatile unsigned long *),0xFFA00000)
#define DMA_VCR_H SH4_REG((volatile unsigned long *),0xFFA00004)
#define DMA_COMMON SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x08 ))
#define DMA_COUNT0 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000020 + 0 * 0x28 ))
#define DMA_COUNT1 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000020 + 1 * 0x28 ))
#define DMA_COUNT2 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000020 + 2 * 0x28 ))
#define DMA_COUNT3 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000020 + 3 * 0x28 ))
#define DMA_CTRL0 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000028 + 0 * 0x28 ))
#define DMA_CTRL1 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000028 + 1 * 0x28 ))
#define DMA_CTRL2 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000028 + 2 * 0x28 ))
#define DMA_CTRL3 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000028 + 3 * 0x28 ))
#define DMA_DAR0 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000018 + 0 * 0x28 ))
#define DMA_DAR1 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000018 + 1 * 0x28 ))
#define DMA_DAR2 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000018 + 2 * 0x28 ))
#define DMA_DAR3 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000018 + 3 * 0x28 ))
#define DMA_SAR0 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000010 + 0 * 0x28 ))
#define DMA_SAR1 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000010 + 1 * 0x28 ))
#define DMA_SAR2 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000010 + 2 * 0x28 ))
#define DMA_SAR3 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000010 + 3 * 0x28 ))
#define DMA_STATUS0 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000030 + 0 * 0x28 ))
#define DMA_STATUS1 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000030 + 1 * 0x28 ))
#define DMA_STATUS2 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000030 + 2 * 0x28 ))
#define DMA_STATUS3 SH4_REG((volatile unsigned long *),(DMA_BASE_ADDRESS + 0x000030 + 3 * 0x28 ))

/*  Values for frequency control */

#define SH4_FRQCR_DIV124 0x00A
#define SH4_FRQCR_DIV222 0x048
#define SH4_FRQCR_DIV333 0x091
#define SH4_FRQCR_DIV444 0x0DA
#define SH4_FRQCR_DIV144 0x01A
#define SH4_FRQCR_DIV244 0x05A
#define SH4_FRQCR_DIV133 0x011
#define SH4_FRQCR_PLL1EN 0x400

#define SH4_FRQCR3_DIV1  0x00
#define SH4_FRQCR3_DIV2  0x01
#define SH4_FRQCR3_DIV3  0x02
#define SH4_FRQCR3_DIV4  0x03
#define SH4_FRQCR3_DIV6  0x04
#define SH4_FRQCR3_DIV8  0x05
#define SH4_FRQCR3_DIV16 0x06

#endif
