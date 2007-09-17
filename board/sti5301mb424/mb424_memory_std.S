/*# file  def.txt*/
/*# ---- Begin variable declaration  -----*/
#define SYS_SERVICES_BASE_ADDRESS 0x20f00000
#define PLL_A 0
#define PLL_B 1

#define PLL_A_FREQUENCY 533250
#define PLL_B_FREQUENCY 600000

#define CKG_MODE_CONTROL (SYS_SERVICES_BASE_ADDRESS + 0x110)
#define CKG_CLOCK_SELECT_CFG (SYS_SERVICES_BASE_ADDRESS + 0x180)
#define CKG_REGISTER_LOCK_CFG (SYS_SERVICES_BASE_ADDRESS + 0x300)
#define HS_INTERCONNECT_SOURCE PLL_A
#define LS_INTERCONNECT_SOURCE PLL_A
#define VIDEO_SOURCE PLL_A
#define FDMA_SOURCE PLL_A
#define LMI_SOURCE PLL_A
#define BLITTER_SOURCE PLL_A
#define CPU_SOURCE PLL_B
#define CKG_PLLA_CONFIG0 (SYS_SERVICES_BASE_ADDRESS + 0x000)
#define CKG_PLLA_CONFIG1 (SYS_SERVICES_BASE_ADDRESS + 0x004)
#define CKG_PLLB_CONFIG0 (SYS_SERVICES_BASE_ADDRESS + 0x008)
#define CKG_PLLB_CONFIG1 (SYS_SERVICES_BASE_ADDRESS + 0x00C)
/*# ---- End variable declaration  -----*/


/*#   gdi file jtag.txt*/
/*# ---- Begin tapmux configuration -----*/
/*#- Put tapmux in bypass mode on channel 1  --*/
/*#proc bypass_tapmux_ch1 {*/
/*????   jtag -on */
/*????   jtag -set 0x10 */
/*????   jtag -set 0x12 */
/*????   jtag -set 0x212 */
/*????   jtag -set 0x12 */
/*????   jtag -set 0x1A */
/*????   jtag -set 0x21A */
/*????   jtag -set 0x1A */
/*????   jtag -set 0x18 */
/*????   jtag -off */
/*# ---- End tapmux configuration -----*/


/*###########################*/
/*##Setup Interconnect*/
/*###########################*/
#define INTERCONNECT_BASEADDRESS 0x20D00000
#define STI5301_CONFIG_CONTROL_A (INTERCONNECT_BASEADDRESS+0x8000)
#define STI5301_CONFIG_CONTROL_B (INTERCONNECT_BASEADDRESS+0x8004)
#define STI5301_CONFIG_CONTROL_C (INTERCONNECT_BASEADDRESS+0x0)
#define STI5301_CONFIG_CONTROL_D (INTERCONNECT_BASEADDRESS+0x4)
#define STI5301_CONFIG_CONTROL_E (INTERCONNECT_BASEADDRESS+0x8)
#define STI5301_CONFIG_CONTROL_F (INTERCONNECT_BASEADDRESS+0xC)
#define STI5301_CONFIG_CONTROL_G (INTERCONNECT_BASEADDRESS+0x10)
#define STI5301_CONFIG_CONTROL_H (INTERCONNECT_BASEADDRESS+0x14)
#define STI5301_NHS2_INIT_0_PRIORITY (INTERCONNECT_BASEADDRESS+0x9000)
#define STI5301_NHS2_INIT_1_PRIORITY (INTERCONNECT_BASEADDRESS+0x9004)
#define STI5301_NHS3_INIT_0_PRIORITY (INTERCONNECT_BASEADDRESS+0xFE000)
#define STI5301_NHS3_INIT_1_PRIORITY (INTERCONNECT_BASEADDRESS+0xFE004)
#define STI5301_NHS3_INIT_2_PRIORITY (INTERCONNECT_BASEADDRESS+0xFE008)
#define STI5301_NHS3_INIT_3_PRIORITY (INTERCONNECT_BASEADDRESS+0xFE00C)
#define STI5301_NHS3_INIT_4_PRIORITY (INTERCONNECT_BASEADDRESS+0xFE010)
#define STI5301_NHS3_INIT_0_LIMIT (INTERCONNECT_BASEADDRESS+0xFE03C)
#define STI5301_NHS3_INIT_1_LIMIT (INTERCONNECT_BASEADDRESS+0xFE040)
#define STI5301_NHS3_INIT_2_LIMIT (INTERCONNECT_BASEADDRESS+0xFE044)
#define STI5301_NHS3_INIT_3_LIMIT (INTERCONNECT_BASEADDRESS+0xFE048)
#define STI5301_NHS3_INIT_4_LIMIT (INTERCONNECT_BASEADDRESS+0xFE04C)
#define STI5301_NHS4_INIT_0_PRIORITY (INTERCONNECT_BASEADDRESS+0xFF000)
#define STI5301_NHS4_INIT_1_PRIORITY (INTERCONNECT_BASEADDRESS+0xFF004)
#define STI5301_NHS4_INIT_2_PRIORITY (INTERCONNECT_BASEADDRESS+0xFF008)
#define STI5301_NHS4_INIT_3_PRIORITY (INTERCONNECT_BASEADDRESS+0xFF00C)
#define STI5301_NHS4_INIT_0_LIMIT (INTERCONNECT_BASEADDRESS+0xFF030)
#define STI5301_NHS4_INIT_1_LIMIT (INTERCONNECT_BASEADDRESS+0xFF034)
#define STI5301_NHS4_INIT_2_LIMIT (INTERCONNECT_BASEADDRESS+0xFF038)
#define STI5301_NHS4_INIT_3_LIMIT (INTERCONNECT_BASEADDRESS+0xFF03C)
#define STI5301_NHD1_INIT_0_PRIORITY (INTERCONNECT_BASEADDRESS+0x2000)
#define STI5301_NHD1_INIT_1_PRIORITY (INTERCONNECT_BASEADDRESS+0x2004)
#define STI5301_NHD1_INIT_2_PRIORITY (INTERCONNECT_BASEADDRESS+0x2008)
#define STI5301_NHD1_INIT_3_PRIORITY (INTERCONNECT_BASEADDRESS+0x200C)
#define STI5301_NHD1_INIT_4_PRIORITY (INTERCONNECT_BASEADDRESS+0x2010)
#define STI5301_NHD1_INIT_5_PRIORITY (INTERCONNECT_BASEADDRESS+0x2014)
#define STI5301_NHD1_INIT_6_PRIORITY (INTERCONNECT_BASEADDRESS+0x2018)
#define STI5301_NHD1_INIT_7_PRIORITY (INTERCONNECT_BASEADDRESS+0x201C)
#define STI5301_NHD1_INIT_0_LIMIT (INTERCONNECT_BASEADDRESS+0x2060)
#define STI5301_NHD1_INIT_1_LIMIT (INTERCONNECT_BASEADDRESS+0x2064)
#define STI5301_NHD1_INIT_2_LIMIT (INTERCONNECT_BASEADDRESS+0x2068)
#define STI5301_NHD1_INIT_3_LIMIT (INTERCONNECT_BASEADDRESS+0x206C)
#define STI5301_NHD1_INIT_4_LIMIT (INTERCONNECT_BASEADDRESS+0x2070)
#define STI5301_NHD1_INIT_5_LIMIT (INTERCONNECT_BASEADDRESS+0x2074)
#define STI5301_NHD1_INIT_6_LIMIT (INTERCONNECT_BASEADDRESS+0x2078)
#define STI5301_NHD1_INIT_7_LIMIT (INTERCONNECT_BASEADDRESS+0x207C)
#define STI5301_NHD2_INIT_0_PRIORITY (INTERCONNECT_BASEADDRESS+0x1000)
#define STI5301_NHD2_INIT_1_PRIORITY (INTERCONNECT_BASEADDRESS+0x1004)
#define STI5301_NHD2_INIT_2_PRIORITY (INTERCONNECT_BASEADDRESS+0x1008)
#define STI5301_NHD2_INIT_3_PRIORITY (INTERCONNECT_BASEADDRESS+0x100C)
#define STI5301_NHD2_INIT_4_PRIORITY (INTERCONNECT_BASEADDRESS+0x1010)
#define STI5301_NHD2_INIT_0_LIMIT (INTERCONNECT_BASEADDRESS+0x103C)
#define STI5301_NHD2_INIT_1_LIMIT (INTERCONNECT_BASEADDRESS+0x1040)
#define STI5301_NHD2_INIT_2_LIMIT (INTERCONNECT_BASEADDRESS+0x1044)
#define STI5301_NHD2_INIT_3_LIMIT (INTERCONNECT_BASEADDRESS+0x1048)
#define STI5301_NHD2_INIT_4_LIMIT (INTERCONNECT_BASEADDRESS+0x104C)
#define STI5301_CPU_FRAME (INTERCONNECT_BASEADDRESS+0x6000)
#define STI5301_CPU_LIMIT (INTERCONNECT_BASEADDRESS+0x6004)

/*################################################################################*/
/*## Name    : STI5301_InterconnectPriority*/
/*## Purpose : Modify the interconnect priorities*/
/*## In      : -*/
/*## Note    : -*/
/*################################################################################*/
/*##proc STI5301_InterconnectPriority  {*/
/*##    write*/
/*##    write Updating interconnect priority*/

/*###    poke  (STI5301_NHS2_INIT_0_PRIORITY) 0x0000000F*/
/*###    poke  (STI5301_NHS2_INIT_1_PRIORITY) 0x0000000F*/

POKE_LONG ( (STI5301_NHS3_INIT_0_PRIORITY) , 0x00000006)
POKE_LONG ( (STI5301_NHS3_INIT_1_PRIORITY) , 0x00000006)
POKE_LONG ( (STI5301_NHS3_INIT_2_PRIORITY) , 0x00000006)
POKE_LONG ( (STI5301_NHS3_INIT_3_PRIORITY) , 0x00000006)
POKE_LONG ( (STI5301_NHS3_INIT_4_PRIORITY) , 0x00000007)

POKE_LONG ( (STI5301_NHS3_INIT_0_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHS3_INIT_1_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHS3_INIT_2_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHS3_INIT_3_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHS3_INIT_4_LIMIT) , 0x00000001)

POKE_LONG ( (STI5301_NHS4_INIT_0_PRIORITY) , 0x0000000F)
POKE_LONG ( (STI5301_NHS4_INIT_1_PRIORITY) , 0x00000006)
POKE_LONG ( (STI5301_NHS4_INIT_2_PRIORITY) , 0x00000005)
POKE_LONG ( (STI5301_NHS4_INIT_3_PRIORITY) , 0x00000007)

POKE_LONG ( (STI5301_NHS4_INIT_0_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHS4_INIT_1_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHS4_INIT_2_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHS4_INIT_3_LIMIT) , 0x00000001)

POKE_LONG ( (STI5301_NHD1_INIT_0_PRIORITY) , 0x00000008)
POKE_LONG ( (STI5301_NHD1_INIT_1_PRIORITY) , 0x00000002)
POKE_LONG ( (STI5301_NHD1_INIT_2_PRIORITY) , 0x00000005)
POKE_LONG ( (STI5301_NHD1_INIT_3_PRIORITY) , 0x00000003)
POKE_LONG ( (STI5301_NHD1_INIT_4_PRIORITY) , 0x00000001)
POKE_LONG ( (STI5301_NHD1_INIT_5_PRIORITY) , 0x00000004)
POKE_LONG ( (STI5301_NHD1_INIT_6_PRIORITY) , 0x00000007)
POKE_LONG ( (STI5301_NHD1_INIT_7_PRIORITY) , 0x00000008)

POKE_LONG ( (STI5301_NHD1_INIT_0_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHD1_INIT_1_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHD1_INIT_2_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHD1_INIT_3_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHD1_INIT_4_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHD1_INIT_5_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHD1_INIT_6_LIMIT) , 0x00000001)
/*##    poke  (STI5301_NHD1_INIT_7_LIMIT) 0x00000001*/

POKE_LONG ( (STI5301_NHD2_INIT_0_PRIORITY) , 0x00000007)
POKE_LONG ( (STI5301_NHD2_INIT_1_PRIORITY) , 0x00000006)
POKE_LONG ( (STI5301_NHD2_INIT_2_PRIORITY) , 0x00000005)
POKE_LONG ( (STI5301_NHD2_INIT_3_PRIORITY) , 0x00000004)
POKE_LONG ( (STI5301_NHD2_INIT_4_PRIORITY) , 0x00000002)

POKE_LONG ( (STI5301_NHD2_INIT_0_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHD2_INIT_1_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHD2_INIT_2_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHD2_INIT_3_LIMIT) , 0x00000001)
POKE_LONG ( (STI5301_NHD2_INIT_4_LIMIT) , 0x00000001)

POKE_LONG ( (STI5301_CPU_LIMIT) , 10)
POKE_LONG ( (STI5301_CPU_FRAME) , 100)

/*###    */
/*##    write Interconnect priority update complete*/
/*##}*/




/*#  gdi file  initclocks.txt*/
/*# ---- Begin clock  configuration -----*/



/*##--------------------------------------------------------------------------*/
/*## Name    : STb5301_InitClocks*/
/*## Purpose : Initialisation of STb5301 Clock Generator*/
/*## In      : -*/
/*## Note    : -*/
/*##--------------------------------------------------------------------------*/

/*????   write "setup Clock Generator for STb5301" */

  /*##  Unlock Clock Generator registers*/
POKE_LONG ( CKG_REGISTER_LOCK_CFG , 0x00FF)
POKE_LONG ( CKG_REGISTER_LOCK_CFG , 0x0000)

  /*## Setup PLL sources for clock*/
#define Source ((HS_INTERCONNECT_SOURCE) << 8) | ((VIDEO_SOURCE) << 7) | (FDMA_SOURCE << 6) | (LMI_SOURCE << 5) \
	       | ((LS_INTERCONNECT_SOURCE) << 4) | ((BLITTER_SOURCE) << 3) | ((LMI_SOURCE) << 2) \
	       | ((CPU_SOURCE) << 1)
POKE_LONG ( CKG_CLOCK_SELECT_CFG , Source)

  /*## Setup PLL_A and PLL_B*/
  /*# STi51xx_InitPLLFrequency (PLL_A_FREQUENCY) (PLL_A)*/
#define Nfactor 237
#define Mfactor 24
#define Pfactor 0
POKE_LONG ( CKG_PLLA_CONFIG0, (((Nfactor-1)<<8) | (Mfactor-1)))
POKE_LONG ( CKG_PLLA_CONFIG1, (0x4838 | Pfactor))
  /*# STi51xx_InitPLLFrequency (PLL_B_FREQUENCY) (PLL_B)*/
#undef Nfactor
#define Nfactor 200
#undef Mfactor
#define Mfactor 18
#undef Pfactor
#define Pfactor 0
POKE_LONG ( CKG_PLLB_CONFIG0,    (((Nfactor-1)<<8) | (Mfactor-1)))
POKE_LONG ( CKG_PLLB_CONFIG1,    (0x4838 | Pfactor))
  /*#fg file simudelay.txt*/


  /*## wait for PLL to settle and lock*/
  /*#fg  file simudelay.txt*/

  /*# Begin  STi51xx_SetupDividers*/
  /*## Purpose : Configure divider for all Pll Clocks*/

  /*# Begin of addresses declaration*/
#define CPU 1
#define LMI 2
#define BLITTER 3
#define LS_INTERCONNECT 4
#define FDMA 5
#define VIDEO 6
#define HS_INTERCONNECT 7
#define FLASH 8
  /*# End of addresses declaration*/

  /*# STi51xx_SetPLLClockDivider (CPU)   (CPU_DIV = 2)  (WHOLE_VALUE)*/
  /*#(CPU_SOURCE: PLL_B)*/
  /*# CPU = 300 MHz Phase = 0*/
#define offset CPU * 0x10
#define config_0 0x00000aaa
#define config_1 0x00000000
#define config_2 0x00000071
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x80 + (offset)) , (config_0))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x84 + (offset)) , (config_1))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x88 + (offset)) , (config_2))

  /*# STi51xx_SetPLLClockDivider (LMI)    (LMI_DIV=4)  (WHOLE_VALUE)*/
  /*# (LMI_SOURCE: PLL_A)*/
  /*# LMI  = 133 MHz Phase = 0*/
#undef offset
#define offset LMI * 0x10
#undef config_0
#define config_0 0x0000cccc
#undef config_1
#define config_1 0x00000000
#undef config_2
#define config_2 0x00000075
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x80 + (offset)) , (config_0))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x84 + (offset)) , (config_1))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x88 + (offset)) , (config_2))

  /*# STi51xx_SetPLLClockDivider (BLITTER)  (BLITTER_DIV: 3.0)  (WHOLE_VALUE)*/
  /*#(BLITTER_SORCE: PLL_A)*/
  /*# BLITTER         = 177 MHz Phase = 0*/
#undef offset
#define offset BLITTER * 0x10
#undef config_0
#define config_0 0x00000db6
#undef config_1
#define config_1 0x00000000
#undef config_2
#define config_2 0x00000011
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x80 + (offset)) , (config_0))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x84 + (offset)) , (config_1))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x88 + (offset)) , (config_2))

  /*# STi51xx_SetPLLClockDivider (LS_INTERCONNECT)  (LS_INTERCONNECT_DIV: 5.0)*/
  /*# (WHOLE_VALE)  (LS_INTERCONNECT_SOURCE: PLL_A)*/
  /*# LS_INTERCONNECT = 106 MHz Phase = 0*/
#undef offset
#define offset LS_INTERCONNECT  * 0x10
#undef config_0
#define config_0 0x0000739c
#undef config_1
#define config_1 0x00000000
#undef config_2
#define config_2 0x00000014
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x80 + (offset)) , (config_0))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x84 + (offset)) , (config_1))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x88 + (offset)) , (config_2))

  /*# STi51xx_SetPLLClockDivider (FDMA)   (FDMA_DIV: 2.0)  (WHOLE_VALUE)*/
  /*# (FDMA_SOURCE: PLL_A)*/
  /*# FDMA            = 266 MHz Phase = 0*/
#undef offset
#define offset FDMA * 0x10
#undef config_0
#define config_0 0x00000aaa
#undef config_1
#define config_1 0x00000000
#undef config_2
#define config_2 0x00000071
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x80 + (offset)) , (config_0))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x84 + (offset)) , (config_1))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x88 + (offset)) , (config_2))

  /*# STi51xx_SetPLLClockDivider (VIDEO)   (VIDEO_DIV: 5.0)  (WHOLE_VALUE)*/
  /*# (VIDEO_SOURCE: PLL_A)*/
  /*# VIDEO           = 88 MHz Phase = 0*/
#undef offset
#define offset VIDEO * 0x10
#undef config_0
#define config_0 0x00000e38
#undef config_1
#define config_1 0x00000000
#undef config_2
#define config_2 0x00000071
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x80 + (offset)) , (config_0))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x84 + (offset)) , (config_1))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x88 + (offset)) , (config_2))

  /*# STi51xx_SetPLLClockDivider (HS_INTERCONNECT)  (HS_INTERCONNECT_DIV:4.0)*/
  /*# (HS_INTERCONNECT_SOURCE: PLL_A)*/
  /*# HS_INTERCONNECT = 133 MHz Phase = 0*/
#undef offset
#define offset HS_INTERCONNECT  * 0x10
#undef config_0
#define config_0 0x0000cccc
#undef config_1
#define config_1 0x00000000
#undef config_2
#define config_2 0x00000075
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x80 + (offset)) , (config_0))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x84 + (offset)) , (config_1))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x88 + (offset)) , (config_2))

  /*# STi51xx_SetPLLClockDivider (FLASH)   (FLASH_DIV: 5.0)  (WHOLE_VALUE)*/
  /*# (LS_INTERCONNCT_SOURCE: PLL_A)*/
  /*# FLASH           = 106 MHz Phase = 0*/
#undef offset
#define offset FLASH * 0x10
#undef config_0
#define config_0 0x0000739c
#undef config_1
#define config_1 0x00000000
#undef config_2
#define config_2 0x00000014
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x80 + (offset)) , (config_0))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x84 + (offset)) , (config_1))
POKE_LONG ( (SYS_SERVICES_BASE_ADDRESS + 0x88 + (offset)) , (config_2))

  /*# End of file*/

/*#FG  End   STi51xx_SetupDividers*/

/*#  Begin  STi51xx_SetupFrequencySynthesizer*/
/*## Purpose : Configure the Frequency Synthesizers for Pixel, PCM, SPDIF,*/
/*##	     		 SmartCard, DAA, USB, Auxiliary, and Audio Clock*/

  /*# Beg of addresses declaration*/

#define CKG_FSA_SETUP (SYS_SERVICES_BASE_ADDRESS + 0x010)
#define CKG_FSB_SETUP (SYS_SERVICES_BASE_ADDRESS + 0x050)

#define CKG_PIX_CLK_SETUP0 (SYS_SERVICES_BASE_ADDRESS + 0x014)
#define CKG_PIX_CLK_SETUP1 (SYS_SERVICES_BASE_ADDRESS + 0x018)
#define CKG_PCM_CLK_SETUP0 (SYS_SERVICES_BASE_ADDRESS + 0x020)
#define CKG_PCM_CLK_SETUP1 (SYS_SERVICES_BASE_ADDRESS + 0x024)
#define CKG_SPDIF_CLK_SETUP0 (SYS_SERVICES_BASE_ADDRESS + 0x030)
#define CKG_SPDIF_CLK_SETUP1 (SYS_SERVICES_BASE_ADDRESS + 0x034)
#define CKG_SC_CLK_SETUP0 (SYS_SERVICES_BASE_ADDRESS + 0x040)
#define CKG_SC_CLK_SETUP1 (SYS_SERVICES_BASE_ADDRESS + 0x044)
#define CKG_DAA_CLK_SETUP0 (SYS_SERVICES_BASE_ADDRESS + 0x054)
#define CKG_DAA_CLK_SETUP1 (SYS_SERVICES_BASE_ADDRESS + 0x058)

#define CKG_OBSERVATION_CFG_AUX (SYS_SERVICES_BASE_ADDRESS + 0x188)
#define CKG_USB_CLK_SETUP0 (SYS_SERVICES_BASE_ADDRESS + 0x060)
#define CKG_USB_CLK_SETUP1 (SYS_SERVICES_BASE_ADDRESS + 0x064)
#define CKG_AUX_CLK_SETUP0 (SYS_SERVICES_BASE_ADDRESS + 0x070)
#define CKG_AUX_CLK_SETUP1 (SYS_SERVICES_BASE_ADDRESS + 0x074)
#define CKG_AUDIO_CLK_SETUP0 (SYS_SERVICES_BASE_ADDRESS + 0x080)
#define CKG_AUDIO_CLK_SETUP1 (SYS_SERVICES_BASE_ADDRESS + 0x084)

  /*# End of addresses declaration*/

POKE_LONG ( (CKG_FSA_SETUP) , 0x4)
POKE_LONG ( (CKG_FSB_SETUP) , 0x4)

  /*## Pixel clock = 27 MHz*/
#define MD 0x1f
#define SDIV 0x2
#define PE 0x0000
POKE_LONG ( (CKG_PIX_CLK_SETUP0),  (0xE00 | (SDIV)<<6 | (MD) ))
POKE_LONG ( (CKG_PIX_CLK_SETUP1) , (PE))

  /*## PCM clock = 24.576 MHz*/
POKE_LONG ( (CKG_PCM_CLK_SETUP0) , 0x0ef1)
POKE_LONG ( (CKG_PCM_CLK_SETUP1) , 0x3600)

  /*## SPDIF clock = 27 MHz*/
POKE_LONG ( (CKG_SPDIF_CLK_SETUP0) , 0x0EBF)
POKE_LONG ( (CKG_SPDIF_CLK_SETUP1) , 0x0000)

  /*## Smart Card clock = 27 MHz*/
POKE_LONG ( (CKG_SC_CLK_SETUP0) , 0x0EBF)
POKE_LONG ( (CKG_SC_CLK_SETUP1) , 0x0000)

  /*## DAA clock = 32.4 MHz*/
POKE_LONG ( (CKG_DAA_CLK_SETUP0) , 0x0EBA)
POKE_LONG ( (CKG_DAA_CLK_SETUP1) , 0x2AAA)

  /*## USB clock = 48 MHz*/
POKE_LONG ( (CKG_USB_CLK_SETUP0) , 0x0EB1)
POKE_LONG ( (CKG_USB_CLK_SETUP1) , 0x0000)

  /*## AUX clock = 27 MHz*/
POKE_LONG ( (CKG_AUX_CLK_SETUP0) , 0x0EBF)
POKE_LONG ( (CKG_AUX_CLK_SETUP1) , 0x0000)

  /*## AUDIO clock = 160 MHz*/
POKE_LONG ( (CKG_AUDIO_CLK_SETUP0) , 0x0E35)
POKE_LONG ( (CKG_AUDIO_CLK_SETUP1) , 0x3333)
  /*# }*/

POKE_LONG ( (CKG_OBSERVATION_CFG_AUX) , 0x21)

/*# End  STi51xx_SetupFrequencySynthesizer*/


/*##  Check PLL_A and PLL_B has locked*/
#define LockA 0
#define LockB 0
#define count 0

/*#fg if $count == 0*/
  /*#while ($count < 20 && (($LockA == 0) || ($LockB == 0)))  {*/
/* #define x peek CKG_PLLA_CONFIG1 */
/* #define y peek CKG_PLLB_CONFIG1 */

WHILE_NE(CKG_PLLA_CONFIG1, 0x4000, 0x4000)
WHILE_NE(CKG_PLLB_CONFIG1, 0x4000, 0x4000)

#if 0
/*????     if ((x & 0x4000) == 0x4000) */
      /*## Check for PLL_A Lock*/
#undef LockA
#define LockA 1
/*????     endif */
/*????     if ((y & 0x4000) == 0x4000) */
     /*## Check for PLL_A Lock*/
#undef LockB
#define LockB 1
/*????     endif */
#undef count
#define count count + 1
  /*#end while*/

/*????   if (LockA == 0) */
/*????     write "PLL_A failed to LOCK!" */
    /*# quit*/
/*????   endif */
/*????   if LockB == 0 */
/*????     write  " PLL_B failed to LOCK!" */
    /*# quit*/
/*????   endif */
/*# end 	of "if $count == 0"*/
/*#fg end*/

#endif

/*#fg file simudelay.txt*/
/*## Now Transistion to programmed mode*/
POKE_LONG ( CKG_MODE_CONTROL , 0x2)
/*#fg file simudelay.txt*/

/*##  Lock Clock Generator registers*/
POKE_LONG ( CKG_REGISTER_LOCK_CFG , 0x00FF)

/*???? write  "STb5301 Clock Generator Successfully setup" */

/*# }*/


/*# Begin  STi51xx_InitFmi*/
/*# write "Setup FMI for STb5301 chip"*/

/*# file fmi.txt*/
/*# --- Begin inlining of fmi.txt ---------*/
/*#######################*/
/*## FMI CONFIGURATION ##*/
/*#######################*/


/*##  Register Declaration*/

#define FMICONFIG_BASE_ADDRESS 0x20200000
#define FMICONFIG_BUFFER_BASE_ADDRESS 0x202FF800

#define FMICONFIGDATA0_BANK0 (FMICONFIG_BASE_ADDRESS + 0x100)
#define FMICONFIGDATA1_BANK0 (FMICONFIG_BASE_ADDRESS + 0x108)
#define FMICONFIGDATA2_BANK0 (FMICONFIG_BASE_ADDRESS + 0x110)
#define FMICONFIGDATA3_BANK0 (FMICONFIG_BASE_ADDRESS + 0x118)

#define FMICONFIGDATA0_BANK1 (FMICONFIG_BASE_ADDRESS + 0x140)
#define FMICONFIGDATA1_BANK1 (FMICONFIG_BASE_ADDRESS + 0x148)
#define FMICONFIGDATA2_BANK1 (FMICONFIG_BASE_ADDRESS + 0x150)
#define FMICONFIGDATA3_BANK1 (FMICONFIG_BASE_ADDRESS + 0x158)

#define FMICONFIGDATA0_BANK2 (FMICONFIG_BASE_ADDRESS + 0x180)
#define FMICONFIGDATA1_BANK2 (FMICONFIG_BASE_ADDRESS + 0x188)
#define FMICONFIGDATA2_BANK2 (FMICONFIG_BASE_ADDRESS + 0x190)
#define FMICONFIGDATA3_BANK2 (FMICONFIG_BASE_ADDRESS + 0x198)

#define FMICONFIGDATA0_BANK3 (FMICONFIG_BASE_ADDRESS + 0x1C0)
#define FMICONFIGDATA1_BANK3 (FMICONFIG_BASE_ADDRESS + 0x1C8)
#define FMICONFIGDATA2_BANK3 (FMICONFIG_BASE_ADDRESS + 0x1D0)
#define FMICONFIGDATA3_BANK3 (FMICONFIG_BASE_ADDRESS + 0x1D8)

#define FMI_STATUSCFG (FMICONFIG_BASE_ADDRESS + 0x10)
#define FMI_STATUSLOCK (FMICONFIG_BASE_ADDRESS + 0x18)
#define FMI_LOCK (FMICONFIG_BASE_ADDRESS + 0x20)
#define FMI_GENCFG (FMICONFIG_BASE_ADDRESS + 0x28)
#define FMI_FLASHCLKSEL (FMICONFIG_BASE_ADDRESS + 0x50)
#define FMI_CLKENABLE (FMICONFIG_BASE_ADDRESS + 0x68)

#define BANK_0_TOP_ADDRESS (FMICONFIG_BUFFER_BASE_ADDRESS + 0x20)
#define BANK_1_TOP_ADDRESS (FMICONFIG_BUFFER_BASE_ADDRESS + 0x30)
#define BANK_2_TOP_ADDRESS (FMICONFIG_BUFFER_BASE_ADDRESS + 0x40)
#define BANK_3_TOP_ADDRESS (FMICONFIG_BUFFER_BASE_ADDRESS + 0x50)
#define BANKS_ENABLED (FMICONFIG_BUFFER_BASE_ADDRESS + 0x60)

#define FLASH_EPLD_CONTROL 0x41400000
#define IO_EPLD_CONTROL 0x41A00000
/*# End of register declaration*/


/*##proc STi51xx_InitFmi {*/

  /*## Ensure all FMI control registers are unlocked*/
  /*## at reset the state of these regs is 'undefined'*/

POKE_LONG ( (FMI_LOCK) , 0x00000000)
POKE_LONG ( (FMI_STATUSLOCK) , 0x00000000)

  /*## Number of FMI Banks : Enable all banks*/
POKE_LONG ( (BANKS_ENABLED) , 0x00000004)

  /*## NOTE: bits [0,7] define bottom address bits [22,29] of bank*/
  /*## Bank 0 -  Ethernet controller -   LAN9118 16 bit*/
  /*## bits [22:29] = 00000011*/
POKE_LONG ( (BANK_0_TOP_ADDRESS) , 0x00000003)
  /*## 0x40000000 - 0x40FFFFFF*/

  /*## Bank 1 - 32MBytes Stem0/DVBCI/EPLD Configured as 16-bit peripheral*/
  /*## bits [22:29] = 00001011*/
POKE_LONG ( (BANK_1_TOP_ADDRESS) , 0x0000000B)
  /*## 0x41000000 - 0x42FFFFFF*/

  /*## Bank 2 - 32MBytes Stem1 Configured as 16-bit peripheral*/
  /*## bits [22:29] = 00010011*/
POKE_LONG ( (BANK_2_TOP_ADDRESS) , 0x00000013)
  /*## 0x43000000 - 0x44FFFFFF*/

  /*## Bank 3 - 8MBytes ST M58LW064D Flash*/
  /*## Note only the top 8Mbytes is populated from 0x7F800000*/
  /*## bits [22:29] = 11111111*/
POKE_LONG ( (BANK_3_TOP_ADDRESS) , 0x000000FF)
  /*## 0x45000000 - 0x7FFFFFFF*/

  /*##----------------------------------------------------------------------------*/
  /*## Program bank functions*/
  /*##----------------------------------------------------------------------------*/
  /*##----------------------------------------------------------------------------*/
  /*## Bank 0 - Ethernet controller - LAN9118 16 bit*/
  /*##----------------------------------------------------------------------------*/

POKE_LONG ( (FMICONFIGDATA0_BANK0) , 0x001016D1)
POKE_LONG ( (FMICONFIGDATA1_BANK0) , 0x9d200000)
POKE_LONG ( (FMICONFIGDATA2_BANK0) , 0x9d220000)
POKE_LONG ( (FMICONFIGDATA3_BANK0) , 0x00000000)


  /*##----------------------------------------------------------------------------*/
  /*## Bank 1 -  Not used*/
  /*##----------------------------------------------------------------------------*/


  /*##----------------------------------------------------------------------------*/
  /*## Bank 2 -  Not Used*/
  /*##----------------------------------------------------------------------------*/


  /*## FMI Bank 3 8MBytes ST M58LW064D Flash*/
  /*##----------------------------------------------------------------------------*/
  /*## Bank 3 - 8MBytes ST M58LW064D Flash*/
  /*##----------------------------------------------------------------------------*/

POKE_LONG ( (FMICONFIGDATA0_BANK3) , 0x001016D1)
POKE_LONG ( (FMICONFIGDATA1_BANK3) , 0x9d200000)
POKE_LONG ( (FMICONFIGDATA2_BANK3) , 0x9d220000)
POKE_LONG ( (FMICONFIGDATA3_BANK3) , 0x00000000)

  /*##  ------- Program Other FMI Registers --------  ##*/
  /*## Synchronous Flash runs @ 1/3 bus clk*/
POKE_LONG ( (FMI_GENCFG) , 0x00000000)
  /*## Flash clock 1/3 STbus CLK*/
POKE_LONG ( (FMI_FLASHCLKSEL) , 0x00000002)
/* POKE_LONG ( (FMI_FLASHCLKSEL) , 0x00000002) */
  /*## Enable Flash*/
POKE_LONG ( (FMI_CLKENABLE) , 0x00000001)


/*##}*/

/*# --- End  inlining of fmi.txt ---------*/



/*# ---- End clock  configuration -----*/



/*#  gdi file board.txt*/
/*# ---- Begin Memory  configuration -----*/

/*## ---------------------------------------------------------------------*/
/*## Add below Hardware initialization using "poke" command*/
/*## ---------------------------------------------------------------------*/
/*##*/

/*#######################*/
/*## LMI CONFIGURATION ##*/
/*#######################*/


#define LMI_BASE_ADDRESS 0xC0000000
#define LMICONFIG_BASE_ADDRESS 0xE0000000

#define LMI_VCR0 (LMICONFIG_BASE_ADDRESS + 0x00)
#define LMI_VCR1 (LMICONFIG_BASE_ADDRESS + 0x04)
#define LMI_MIM0 (LMICONFIG_BASE_ADDRESS + 0x08)
#define LMI_MIM1 (LMICONFIG_BASE_ADDRESS + 0x0C)
#define LMI_SCR (LMICONFIG_BASE_ADDRESS + 0x10)
#define LMI_STR (LMICONFIG_BASE_ADDRESS + 0x18)

#define LMI_PBS (LMICONFIG_BASE_ADDRESS + 0x20)
#define LMI_COC_LOWER (LMICONFIG_BASE_ADDRESS + 0x28)
#define LMI_COC_UPPER (LMICONFIG_BASE_ADDRESS + 0x2C)
#define LMI_SDRA0 (LMICONFIG_BASE_ADDRESS + 0x30)
#define LMI_SDRA1 (LMICONFIG_BASE_ADDRESS + 0x38)
#define LMI_CIC (LMICONFIG_BASE_ADDRESS + 0x40)
#define LMI_SDMR0 (LMICONFIG_BASE_ADDRESS + 0x48)
#define LMI_SDMR1 (LMICONFIG_BASE_ADDRESS + 0x50)



/*##----------------------------------------------------------------------------*/
/*## Procedure Name : STi51xx_IniLMI_Cas20_MB390*/
/*## Purpose        : Initialisation of LMI for MB390 board (CAS = 2.0 ##ns)*/
/*## Note	          : Set up the LMI for 256Mbit DDR-SDRAM using ##1 off*/
/*##                  4Mbit x 16 x 4 bank ##devices (Samsung K4H561638D-TCB3000)*/
/*##                  DDR Clock = 133 MHz @ CAS ##2.0 (LMI Spec: ADCS 7483542)*/
/*##----------------------------------------------------------------------------*/
/*## proc STi51xx_InitLMI_Cas20_MB390 {*/

  /*## write "\n Configuring LMI for 16-bit data CAS 2.0 @ 133MHz "*/

  /*## Set LMI_COC_UPPER Register, bits [63:32]   (LMI Pad logic)  +0x2C*/
POKE_LONG ( (LMI_COC_UPPER) , 0x000C6750)

  /*## Set LMI_COC_LOWER Register, bits [31:0]  (LMI Pad logic)*/
  /*## Bits [19:18] Compensation mode DDR*/
  /*## Bits [17:16] Pad strenght 		 (0x0:5pF, 0x1:15pF, 0x2:25pF, Ox3:35pF)*/
  /*## Bits [15:14] output Impedance (0x0:25Ohm, 0x1:40Ohm, 0x2:55Ohm, Ox3:70Ohm)*/
  /*## Bit  [13]    DLL preset reset value enable*/

#if 0
POKE_LONG ( (LMI_COC_LOWER) , 0x00002000)
#define data peek (LMI_COC_LOWER)
#undef data
#define data (data | 0x0<<16 | 0x0<<14)
#endif

POKE_LONG ( (LMI_COC_LOWER) , (0x00002000))

  /*## SDRAM Mode Register*/
  /*## Set Refresh Interval, Enable Refresh, 16-bit bus,*/
  /*## Grouping Disabled, DDR-SDRAM, Enable.*/
  /*## Bits[27:16]: Refresh Interval = 7.8 microseconds (8K/64ms)*/
  /*## @  50MHz =  390 clk cycles -> 0x186*/
  /*## @  75MHz =  585 clk cycles -> 0x249*/
  /*## @ 100MHz =  780 clk cycles -> 0x30C*/
  /*## @ 125MHz =  975 clk cycles -> 0x3CF*/
  /*## @ 133MHz = 1040 clk cycles -> 0x410  <--*/
  /*## @ 166MHz = 1300 clk cycles -> 0x514*/
  /*## poke -d (LMI_MIM)*/
POKE_LONG ( (LMI_MIM0) , 0x04100203)

  /*## SDRAM Timing Register*/
  /*## For 133MHz (7.5ns) operation:*/
  /*##  3 clks RAS_precharge, Trp;*/
  /*##  3 clks RAS_to_CAS_delay, Trcd-r;*/
  /*##  8 clks RAS cycle time, Trc;*/
  /*##  6 clks RAS Active time, Tras;*/
  /*##  2 clks RAS_to_RAS_Active_delay, Trrd;*/
  /*##  2 clks Last write to PRE/PALL period SDRAM, Twr;*/
  /*##  2 clks CAS Latency;*/
  /*## 10 clks Auto Refresh RAS cycle time, Trfc;*/
  /*##  Enable Write to Read interruption;*/
  /*##  1 clk  Write to Read interruption, Twtr;*/
  /*##  3 clks RAS_to_CAS_delay, Trcd-w;*/
  /*## (200/16)=3 clks Exit self-refresh to next command, Tsxsr;*/
POKE_LONG ( (LMI_STR) , 0x35085225)

  /*## SDRAM Row Attribute 0 & 1 Registers*/
  /*## UBA = 32MB + Base Adr, Quad-bank, Shape 13x9,*/
  /*## Bank Remapping Disabled*/
  /*##*/
  /*##  LMI base address 	0xC0000000*/
  /*##  Memory size 32MB 	0x02000000*/
  /*##  Row UBA value    	0xC200*/
  /*##*/
  /*## write set memory size*/
  /*## if (SDRAM_CONFIG == "BUILT_FOR_256MBIT") {*/
POKE_LONG ( (LMI_SDRA0) , 0xC2001900)
  /*##      }*/
  /*##  We just have one Row connected to cs0, so we must program UBA0 = UBA1,*/
  /*##  following LMI specification*/
  /*## if (SDRAM_CONFIG == "BUILT_FOR_256MBIT") {*/
POKE_LONG ( (LMI_SDRA1) , 0xC9001900)
  /*##       }*/

  /*##---------------------------------------------------------------------------*/
  /*## Initialisation Sequence for LMI & DDR-SDRAM Device*/
  /*##---------------------------------------------------------------------------*/

  /*## 200 microseconds to settle clocks*/
  /*# Simudelay (100)*/
  /*#fg  file simudelay.txt*/

  /*## SDRAM Control Register*/
  /*## Clock enable*/
POKE_LONG ( (LMI_SCR) , 0x3)

  /*## NOP enable*/
POKE_LONG ( (LMI_SCR) , 0x1)

  /*## Precharge all banks*/
POKE_LONG ( (LMI_SCR) , 0x2)

  /*## LMI_SDRAM_ROW_MODE0 & LMI_SDRAM_ROW_MODE1 Registers*/
  /*## EMRS Row 0 & 1: Weak Drive : Enable DLL*/
POKE_LONG ( (LMI_SDMR0) , 0x0402)
POKE_LONG ( (LMI_SDMR1) , 0x0402)
  /*# Simudelay (100)*/
  /*#fg  file simudelay.txt*/

  /*## MRS Row 0 & 1 : Reset DLL - /CAS = 2.0, Mode Sequential, Burst Length 8*/
POKE_LONG ( (LMI_SDMR0) , 0x0123)
POKE_LONG ( (LMI_SDMR1) , 0x0123)

  /*## 200 clock cycles required to lock DLL*/
  /*#fg  # Simudelay (100)*/
  /*#fg  file simudelay.txt*/

  /*## Precharge all banks*/
POKE_LONG ( (LMI_SCR) , 0x2)

  /*## CBR enable (auto-refresh)*/
POKE_LONG ( (LMI_SCR) , 0x4)
POKE_LONG ( (LMI_SCR) , 0x4)
POKE_LONG ( (LMI_SCR) , 0x4)

  /*## LMI_SDRAM_ROW_MODE0 & 1 Registers*/
  /*## MRS Row 0 & 1 : Normal - /CAS = 2.0, Mode Sequential, Burst Length 8*/
POKE_LONG ( (LMI_SDMR0) , 0x0023)
POKE_LONG ( (LMI_SDMR1) , 0x0023)

  /*## Normal SDRAM operation, No burst Refresh after standby*/
POKE_LONG ( (LMI_SCR) , 0x0)

/*????   set_timeout 90 */


/*# ---- End Memory  configuration -----*/
