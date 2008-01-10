#define SYS_SERVICES_BASE_ADDRESS       0x20F00000
#define CKG_REGISTER_LOCK_CFG           (SYS_SERVICES_BASE_ADDRESS + 0x300)
#define CKG_REGISTER_PLLA_0             (SYS_SERVICES_BASE_ADDRESS + 0x0)
#define CKG_REGISTER_PLLA_1             (SYS_SERVICES_BASE_ADDRESS + 0x4)
#define CKG_REGISTER_PLLB_0             (SYS_SERVICES_BASE_ADDRESS + 0x8)
#define CKG_REGISTER_PLLB_1             (SYS_SERVICES_BASE_ADDRESS + 0xC)
#define CKG_REGISTER_CLOCK_SELECT_CFG   (SYS_SERVICES_BASE_ADDRESS + 0x180)
#define CKG_REGISTER_FS_PIXCLK          (SYS_SERVICES_BASE_ADDRESS + 0x14)
#define CKG_REGISTER_MODE               (SYS_SERVICES_BASE_ADDRESS + 0x110)

        /*
         * CLOCKGEN setup step 1
         */
        POKE_LONG(CKG_REGISTER_LOCK_CFG, 0x000000ff)
        POKE_LONG(CKG_REGISTER_LOCK_CFG, 0x00000000)
        POKE_LONG(CKG_REGISTER_PLLA_0,  0x0000EC17)
        POKE_LONG(CKG_REGISTER_PLLA_1,   0x00004838)
        POKE_LONG(CKG_REGISTER_PLLB_0,   0x0000C711)
        POKE_LONG(CKG_REGISTER_PLLB_1,   0x00004838)
        POKE_LONG(CKG_REGISTER_CLOCK_SELECT_CFG, 0x00000002)

        /*
         * SYSCONF pokes
         */
        /* CPU */
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x80 + 0x10), 0x00000aaa)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x84 + 0x10), 0x00000000)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x88 + 0x10), 0x00000071)

        /* LMI */
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x80 + 0x20), 0x0000cccc)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x84 + 0x20), 0x00000000)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x88 + 0x20), 0x00000075)

        /* Blitter */
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x80 + 0x30), 0x0000cccc)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x84 + 0x30), 0x00000000)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x88 + 0x30), 0x00000075)

        /* LS Interconnect */
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x80 + 0x40), 0x0000739c)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x84 + 0x40), 0x00000000)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x88 + 0x40), 0x00000014)

        /* FDMA */
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x80 + 0x50), 0x00000aaa)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x84 + 0x50), 0x00000000)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x88 + 0x50), 0x00000071)

        /* Video */
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x80 + 0x60), 0x00000e38)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x84 + 0x60), 0x00000000)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x88 + 0x60), 0x00000071)

        /* HS_INTERCONNECT = 133 MHz Phase = 0 */
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x80 + 0x70), 0x0000cccc)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x84 + 0x70), 0x00000000)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x88 + 0x70), 0x00000075)

        /* Flash */
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x80 + 0x80), 0x0000739c)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x84 + 0x80), 0x00000000)
        POKE_LONG((SYS_SERVICES_BASE_ADDRESS + 0x88 + 0x80), 0x00000014)


/* Begin  STi51xx_SetupFrequencySynthesizer */
/* Purpose : Configure the Frequency Synthesizers for Pixel, PCM, SPDIF, */
/*           SmartCard, DAA, USB, Auxiliary, and Audio Clock */

  /* Beg of addresses declaration */

#define CKG_FSA_SETUP           (SYS_SERVICES_BASE_ADDRESS + 0x010)
#define CKG_FSB_SETUP           (SYS_SERVICES_BASE_ADDRESS + 0x050)

#define CKG_PIX_CLK_SETUP0      (SYS_SERVICES_BASE_ADDRESS + 0x014)
#define CKG_PIX_CLK_SETUP1      (SYS_SERVICES_BASE_ADDRESS + 0x018)
#define CKG_PCM_CLK_SETUP0      (SYS_SERVICES_BASE_ADDRESS + 0x020)
#define CKG_PCM_CLK_SETUP1      (SYS_SERVICES_BASE_ADDRESS + 0x024)
#define CKG_SPDIF_CLK_SETUP0    (SYS_SERVICES_BASE_ADDRESS + 0x030)
#define CKG_SPDIF_CLK_SETUP1    (SYS_SERVICES_BASE_ADDRESS + 0x034)
#define CKG_SC_CLK_SETUP0       (SYS_SERVICES_BASE_ADDRESS + 0x040)
#define CKG_SC_CLK_SETUP1       (SYS_SERVICES_BASE_ADDRESS + 0x044)
#define CKG_DAA_CLK_SETUP0      (SYS_SERVICES_BASE_ADDRESS + 0x054)
#define CKG_DAA_CLK_SETUP1      (SYS_SERVICES_BASE_ADDRESS + 0x058)

#define CKG_OBSERVATION_CFG_AUX (SYS_SERVICES_BASE_ADDRESS + 0x188)
#define CKG_USB_CLK_SETUP0      (SYS_SERVICES_BASE_ADDRESS + 0x060)
#define CKG_USB_CLK_SETUP1      (SYS_SERVICES_BASE_ADDRESS + 0x064)
#define CKG_AUX_CLK_SETUP0      (SYS_SERVICES_BASE_ADDRESS + 0x070)
#define CKG_AUX_CLK_SETUP1      (SYS_SERVICES_BASE_ADDRESS + 0x074)
#define CKG_AUDIO_CLK_SETUP0    (SYS_SERVICES_BASE_ADDRESS + 0x080)
#define CKG_AUDIO_CLK_SETUP1    (SYS_SERVICES_BASE_ADDRESS + 0x084)

        /* End of addresses declaration */

        POKE_LONG (CKG_FSA_SETUP, 0x4 )
        POKE_LONG (CKG_FSB_SETUP, 0x4 )

        /* Pixel clock = 27 MHz */
#define  MD     0x1f
#define  SDIV   0x2
#define  PE     0x0000
        POKE_LONG (CKG_PIX_CLK_SETUP0, (0xE00 | (SDIV)<<6 | (MD)) )
        POKE_LONG (CKG_PIX_CLK_SETUP1, (PE) )

        /* PCM clock = 24.576 MHz */
        POKE_LONG (CKG_PCM_CLK_SETUP0, 0x0ef1 )
        POKE_LONG (CKG_PCM_CLK_SETUP1, 0x3600 )

        /* SPDIF clock = 27 MHz */
        POKE_LONG (CKG_SPDIF_CLK_SETUP0, 0x0EBF )
        POKE_LONG (CKG_SPDIF_CLK_SETUP1, 0x0000 )

        /* Smart Card clock = 27 MHz */
        POKE_LONG (CKG_SC_CLK_SETUP0, 0x0EBF )
        POKE_LONG (CKG_SC_CLK_SETUP1, 0x0000 )

        /* DAA clock = 32.4 MHz */
        POKE_LONG (CKG_DAA_CLK_SETUP0, 0x0EBA )
        POKE_LONG (CKG_DAA_CLK_SETUP1, 0x2AAA )

        /* USB clock = 48 MHz */
        POKE_LONG (CKG_USB_CLK_SETUP0, 0x0EB1 )
        POKE_LONG (CKG_USB_CLK_SETUP1, 0x0000 )

        /* AUX clock = 27 MHz */
        POKE_LONG (CKG_AUX_CLK_SETUP0, 0x0EBF )
        POKE_LONG (CKG_AUX_CLK_SETUP1, 0x0000 )

        /* AUDIO clock = 160 MHz */
        POKE_LONG (CKG_AUDIO_CLK_SETUP0, 0x0E35 )
        POKE_LONG (CKG_AUDIO_CLK_SETUP1, 0x3333 )

        POKE_LONG (CKG_OBSERVATION_CFG_AUX, 0x21 )

        /* End  STi51xx_SetupFrequencySynthesizer */

        /* Now Transistion to programmed mode */
        POKE_LONG( CKG_REGISTER_MODE, 0x2)

        /*  Lock Clock Generator registers */
        POKE_LONG (CKG_REGISTER_LOCK_CFG,   0x01FF)

        /* Final transition */
        POKE_LONG(CKG_REGISTER_MODE, 0x00000002)

/*
 * --- Begin inlining of fmi.txt ---------
 *
 * FMI CONFIGURATION
 */


/*  Register Declaration */

#define FMICONFIG_BASE_ADDRESS        0x20200000
#define FMICONFIG_BUFFER_BASE_ADDRESS 0x202FF800

#define FMICONFIGDATA0_BANK0  (FMICONFIG_BASE_ADDRESS + 0x100)
#define FMICONFIGDATA1_BANK0  (FMICONFIG_BASE_ADDRESS + 0x108)
#define FMICONFIGDATA2_BANK0  (FMICONFIG_BASE_ADDRESS + 0x110)
#define FMICONFIGDATA3_BANK0  (FMICONFIG_BASE_ADDRESS + 0x118)

#define FMICONFIGDATA0_BANK1  (FMICONFIG_BASE_ADDRESS + 0x140)
#define FMICONFIGDATA1_BANK1  (FMICONFIG_BASE_ADDRESS + 0x148)
#define FMICONFIGDATA2_BANK1  (FMICONFIG_BASE_ADDRESS + 0x150)
#define FMICONFIGDATA3_BANK1  (FMICONFIG_BASE_ADDRESS + 0x158)

#define FMICONFIGDATA0_BANK2  (FMICONFIG_BASE_ADDRESS + 0x180)
#define FMICONFIGDATA1_BANK2  (FMICONFIG_BASE_ADDRESS + 0x188)
#define FMICONFIGDATA2_BANK2  (FMICONFIG_BASE_ADDRESS + 0x190)
#define FMICONFIGDATA3_BANK2  (FMICONFIG_BASE_ADDRESS + 0x198)

#define FMICONFIGDATA0_BANK3  (FMICONFIG_BASE_ADDRESS + 0x1C0)
#define FMICONFIGDATA1_BANK3  (FMICONFIG_BASE_ADDRESS + 0x1C8)
#define FMICONFIGDATA2_BANK3  (FMICONFIG_BASE_ADDRESS + 0x1D0)
#define FMICONFIGDATA3_BANK3  (FMICONFIG_BASE_ADDRESS + 0x1D8)

#define FMI_STATUSCFG         (FMICONFIG_BASE_ADDRESS + 0x10)
#define FMI_STATUSLOCK        (FMICONFIG_BASE_ADDRESS + 0x18)
#define FMI_LOCK              (FMICONFIG_BASE_ADDRESS + 0x20)
#define FMI_GENCFG            (FMICONFIG_BASE_ADDRESS + 0x28)
#define FMI_FLASHCLKSEL       (FMICONFIG_BASE_ADDRESS + 0x50)
#define FMI_CLKENABLE         (FMICONFIG_BASE_ADDRESS + 0x68)

#define BANK_0_TOP_ADDRESS    (FMICONFIG_BUFFER_BASE_ADDRESS + 0x20)
#define BANK_1_TOP_ADDRESS    (FMICONFIG_BUFFER_BASE_ADDRESS + 0x30)
#define BANK_2_TOP_ADDRESS    (FMICONFIG_BUFFER_BASE_ADDRESS + 0x40)
#define BANK_3_TOP_ADDRESS    (FMICONFIG_BUFFER_BASE_ADDRESS + 0x50)
#define BANKS_ENABLED         (FMICONFIG_BUFFER_BASE_ADDRESS + 0x60)

#define FLASH_EPLD_CONTROL    0x41400000
#define IO_EPLD_CONTROL       0x41A00000
/* End of register declaration */

  /* Ensure all FMI control registers are unlocked */
  /* at reset the state of these regs is 'undefined' */

  POKE_LONG (FMI_LOCK, 0x00000000)
  POKE_LONG (FMI_STATUSLOCK, 0x00000000)

  /* Number of FMI Banks : Enable all banks */
  POKE_LONG (BANKS_ENABLED, 0x00000004)

  /* NOTE: bits [0,7] define bottom address bits [22,29] of bank */
  /* Bank 0 - 16MBytes Atapi Configured as 16-bit peripheral */
  /* bits [22:29] = 00000011 */
  POKE_LONG (BANK_0_TOP_ADDRESS, 0x00000003)
  /* 0x40000000 - 0x40FFFFFF */

  /* Bank 1 - 32MBytes Stem0/DVBCI/EPLD Configured as 16-bit peripheral */
  /* bits [22:29] = 00001011 */
  POKE_LONG (BANK_1_TOP_ADDRESS, 0x0000000B)
  /* 0x41000000 - 0x42FFFFFF */

  /* Bank 2 - 32MBytes Stem1 Configured as 16-bit peripheral */
  /* bits [22:29] = 00010011 */
  POKE_LONG (BANK_2_TOP_ADDRESS, 0x00000013)
  /* 0x43000000 - 0x44FFFFFF */

  /* Bank 3 - 8MBytes ST M58LW064D Flash */
  /* Note only the top 8Mbytes is populated from 0x7F800000 */
  /* bits [22:29] = 11111111 */
  POKE_LONG (BANK_3_TOP_ADDRESS, 0x000000FF)
  /* 0x45000000 - 0x7FFFFFFF */

  /* ------------------------------------------------------------------- */
  /* Program bank functions */
  /* ------------------------------------------------------------------- */

  /* ------------------------------------------------------------------- */
  /* Bank 0 - 16MBytes Atapi Configured as 16-bit peripheral */
  /* ------------------------------------------------------------------- */

  POKE_LONG (FMICONFIGDATA0_BANK0, 0x00878791)
  POKE_LONG (FMICONFIGDATA1_BANK0, 0x9c028821)
  POKE_LONG (FMICONFIGDATA2_BANK0, 0x9c028821)
  POKE_LONG (FMICONFIGDATA3_BANK0, 0x0000000a)

  /* ------------------------------------------------------------------- */
  /* Bank 1 - 32MBytes Stem0/DVBCI/EPLD Configured as 16-bit peripheral */
  /* ------------------------------------------------------------------- */

  POKE_LONG (FMICONFIGDATA0_BANK1,  0x001016D1)
  POKE_LONG (FMICONFIGDATA1_BANK1,  0x9d200000)
  POKE_LONG (FMICONFIGDATA2_BANK1,  0x9d220000)
  POKE_LONG (FMICONFIGDATA3_BANK1,  0x00000000)

  /* ------------------------------------------------------------------- */
  /* Bank 2 - 32MBytes Stem1 Configured as 16-bit peripheral */
  /* ------------------------------------------------------------------- */

  /*BE not active during rd */
  POKE_LONG (FMICONFIGDATA0_BANK2, 0x001016D1)
  POKE_LONG (FMICONFIGDATA1_BANK2, 0x9d200000)
  POKE_LONG (FMICONFIGDATA2_BANK2, 0x9d220000)
  POKE_LONG (FMICONFIGDATA3_BANK2, 0x00000000)

  /* FMI Bank 3 8MBytes ST M58LW064D Flash - ONLY 8MBytes FLASH ON BOARD THIS MAY BE WRONG! */
  /* ------------------------------------------------------------------- */
  /* Bank 3 - 8MBytes ST M58LW064D Flash */
  /* ------------------------------------------------------------------- */

  POKE_LONG  (FMICONFIGDATA0_BANK3, 0x001016D1)
  POKE_LONG  (FMICONFIGDATA1_BANK3, 0x9d200000)
  POKE_LONG  (FMICONFIGDATA2_BANK3, 0x9d220000)
  POKE_LONG  (FMICONFIGDATA3_BANK3, 0x00000000)

  /*  ------- Program Other FMI Registers --------   */
  /* Synchronous Flash runs @ 1/3 bus clk */
  POKE_LONG (FMI_GENCFG,      0xC0000000)
  /* Flash clock 1/3 STbus CLK */
  POKE_LONG (FMI_FLASHCLKSEL, 0x00000002)
  POKE_LONG (FMI_FLASHCLKSEL, 0x00000002)
  /* Enable Flash */
  POKE_LONG (FMI_CLKENABLE,   0x00000001)

  /*  Reset flash Banks */

#if 0
  POKE_LONG (FLASH_EPLD_CONTROL,   0x0)
  POKE_LONG (FLASH_EPLD_CONTROL,   0x33333333)
  POKE_LONG (IO_EPLD_CONTROL,      0x01010101)
#endif

/*
 * LMI CONFIGURATION ##
 */


#define LMI_BASE_ADDRESS        0xC0000000
#define LMICONFIG_BASE_ADDRESS  0xE0000000

#define LMI_VCR0        (LMICONFIG_BASE_ADDRESS + 0x00)
#define LMI_VCR1        (LMICONFIG_BASE_ADDRESS + 0x04)
#define LMI_MIM0        (LMICONFIG_BASE_ADDRESS + 0x08)
#define LMI_MIM1        (LMICONFIG_BASE_ADDRESS + 0x0C)
#define LMI_SCR         (LMICONFIG_BASE_ADDRESS + 0x10)
#define LMI_STR         (LMICONFIG_BASE_ADDRESS + 0x18)

#define LMI_PBS         (LMICONFIG_BASE_ADDRESS + 0x20)
#define LMI_COC_LOWER   (LMICONFIG_BASE_ADDRESS + 0x28)
#define LMI_COC_UPPER   (LMICONFIG_BASE_ADDRESS + 0x2C)
#define LMI_SDRA0       (LMICONFIG_BASE_ADDRESS + 0x30)
#define LMI_SDRA1       (LMICONFIG_BASE_ADDRESS + 0x38)
#define LMI_CIC         (LMICONFIG_BASE_ADDRESS + 0x40)
#define LMI_SDMR0       (LMICONFIG_BASE_ADDRESS + 0x48)
#define LMI_SDMR1       (LMICONFIG_BASE_ADDRESS + 0x50)

/* ---------------------------------------------------------------------- */
/* Procedure Name : STi51xx_IniLMI_Cas20_MB390 */
/* Purpose      : Initialisation of LMI for MB390 board (CAS = 2.0 ##ns) */
/* Note         : Set up the LMI for 256Mbit DDR-SDRAM using ##1 off */
/*            4Mbit x 16 x 4 bank ##devices (Samsung K4H561638D-TCB3000) */
/*            DDR Clock = 133 MHz @ CAS ##2.0 (LMI Spec: ADCS 7483542)   */
/* --------------------------------------------------------------------- */
/* proc STi51xx_InitLMI_Cas20_MB390 { */


        /* Set LMI_COC_UPPER Register, bits [63:32]   (LMI Pad logic)  +0x2C */
        POKE_LONG (LMI_COC_UPPER, 0x000C6750)

        /* Set LMI_COC_LOWER Register, bits [31:0]  (LMI Pad logic) */
        /* Bits [19:18] Compensation mode DDR */
        /* Bits [17:16] Pad strenght     (0x0:5pF, 0x1:15pF, 0x2:25pF, Ox3:35pF) */
        /* Bits [15:14] output Impedance (0x0:25Ohm, 0x1:40Ohm, 0x2:55Ohm, Ox3:70Ohm) */
        /* Bit  [13]    DLL preset reset value enable */

        POKE_LONG (LMI_COC_LOWER, 0x00002000)
        /*   data = peek (LMI_COC_LOWER) */
        /*   data = (data | 0x0<<16 | 0x0<<14) */
        POKE_LONG (LMI_COC_LOWER,  (0x00002000))

        /* SDRAM Mode Register */
        /* Set Refresh Interval, Enable Refresh, 16-bit bus, */
        /* Grouping Disabled, DDR-SDRAM, Enable. */
        /* Bits[27:16]: Refresh Interval = 7.8 microseconds (8K/64ms) */
        /* @  50MHz =  390 clk cycles -> 0x186 */
        /* @  75MHz =  585 clk cycles -> 0x249 */
        /* @ 100MHz =  780 clk cycles -> 0x30C */
        /* @ 125MHz =  975 clk cycles -> 0x3CF */
        /* @ 133MHz = 1040 clk cycles -> 0x410  <-- */
        /* @ 166MHz = 1300 clk cycles -> 0x514 */
        /* POKE_LONG -d (LMI_MIM) */
        POKE_LONG (LMI_MIM0,     0x04100203)

        /* SDRAM Timing Register */
        /* For 133MHz (7.5ns) operation: */
        /*  3 clks RAS_precharge, Trp; */
        /*  3 clks RAS_to_CAS_delay, Trcd-r; */
        /*  8 clks RAS cycle time, Trc; */
        /*  6 clks RAS Active time, Tras; */
        /*  2 clks RAS_to_RAS_Active_delay, Trrd; */
        /*  2 clks Last write to PRE/PALL period SDRAM, Twr; */
        /*  2 clks CAS Latency; */
        /* 10 clks Auto Refresh RAS cycle time, Trfc; */
        /*  Enable Write to Read interruption; */
        /*  1 clk  Write to Read interruption, Twtr; */
        /*  3 clks RAS_to_CAS_delay, Trcd-w; */
        /* (200/16)=3 clks Exit self-refresh to next command, Tsxsr; */
        POKE_LONG (LMI_STR, 0x35085225)

        /* SDRAM Row Attribute 0 & 1 Registers */
        /* UBA = 32MB + Base Adr, Quad-bank, Shape 13x9, */
        /* Bank Remapping Disabled */

        /*  LMI base address  0xC0000000 */
        /*  Memory size 32MB  0x02000000 */
        /*  Row UBA value     0xC200 */

        /* set memory size */
        /* if (SDRAM_CONFIG == "BUILT_FOR_256MBIT")  */
                POKE_LONG (LMI_SDRA0,    0xC2001900)

        /*  We just have one Row connected to cs0, so we must program UBA0 = UBA1, */
        /*  following LMI specification */
        /* if (SDRAM_CONFIG == "BUILT_FOR_256MBIT")  */
                POKE_LONG (LMI_SDRA1,    0xC9001900)

        /* ------------------------------------------------------------------- */
        /* Initialisation Sequence for LMI & DDR-SDRAM Device */
        /* ------------------------------------------------------------------- */

        /* 200 microseconds to settle clocks */

        /* SDRAM Control Register */
        /* Clock enable */
        POKE_LONG (LMI_SCR,    0x3)

        /* NOP enable */
        POKE_LONG (LMI_SCR,    0x1)

        /* Precharge all banks */
        POKE_LONG (LMI_SCR,    0x2)

        /* LMI_SDRAM_ROW_MODE0 & LMI_SDRAM_ROW_MODE1 Registers */
        /* EMRS Row 0 & 1: Weak Drive : Enable DLL */
        POKE_LONG (LMI_SDMR0,    0x0402)
        POKE_LONG (LMI_SDMR1,    0x0402)

        /* MRS Row 0 & 1 : Reset DLL - /CAS = 2.0, Mode Sequential, Burst Length 8 */
        POKE_LONG (LMI_SDMR0,    0x0123)
        POKE_LONG (LMI_SDMR1,    0x0123)

        /* 200 clock cycles required to lock DLL */

        /* Precharge all banks */
        POKE_LONG (LMI_SCR,    0x2)

        /* CBR enable (auto-refresh) */
        POKE_LONG (LMI_SCR,    0x4)
        POKE_LONG (LMI_SCR,    0x4)
        POKE_LONG (LMI_SCR,    0x4)

        /* LMI_SDRAM_ROW_MODE0 & 1 Registers */
        /* MRS Row 0 & 1 : Normal - /CAS = 2.0, Mode Sequential, Burst Length 8 */
        POKE_LONG (LMI_SDMR0,    0x0023)
        POKE_LONG (LMI_SDMR1,    0x0023)

        /* Normal SDRAM operation, No burst Refresh after standby */
        POKE_LONG (LMI_SCR,    0x0)
