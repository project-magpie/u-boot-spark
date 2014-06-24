/*$Source: D:/STB_BASE_HAL/u-boot-sh4-1.3.1_stm23_0051/lib_sh/vfd.c,v $*/
/*****************************文件头部注释*************************************/
//
//			Copyright (C), 2009-2014, AV Frontier Tech. Co., Ltd.
//
//
// 文 件 名：	$RCSfile: vfd.c,v $
//
// 创 建 者：	D02SH
//
// 创建时间：	2009.01.09
//
// 最后更新：	$Date: 2011/11/25 06:22:19 $
//
//				$Author: d26lf $
//
//				$Revision: 1.6 $
//
//				$State: Exp $
//
// 文件描述：	将VFD内容独立出来
//
/******************************************************************************/

/********************************  文件包含************************************/
#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/pio.h>
#include <i2c.h>
#include <config.h>
#include "vfd.h"

#if defined(YW_CONFIG_VFD)
/********************************  常量定义************************************/

/********************************  宏 定 义************************************/
#define COMBO_I2C_ADDR (0x50 >> 1)
#define COMBO_I2C_OK 0
#define msleep(a) udelay(a*1000)
#define VFD_CS_CLR() {udelay(10);STPIO_SET_PIN(PIO_PORT(3), 5, 0);}
#define VFD_CS_SET() {udelay(10);STPIO_SET_PIN(PIO_PORT(3), 5, 1);}

#define PIO_BASE  0xb8020000
#define PIO2_BASE  0xb8024000
#define YWPANEL_FP_INFO_MAX_LENGTH         (10)             /*读指令最大长度*/
#define YWPANEL_FP_DATA_MAX_LENGTH        (38)             /*写指令最大长度*/
#define LOG_OFF     0
#define LOG_ON      1

/* data type definitions */
#define U32 unsigned long
#define S32 signed long
#define I32 long
#define U16 unsigned short
#define S16 signed short
#define I16 short
#define U8 unsigned char
#define S8 signed char
#define I8 char

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

//#define DEBUG
#ifdef DEBUG
#define YWVFD_Debug(x...) printf(x)
#else
#define YWVFD_Debug(x...)
#endif

BOOL YWVFD_STANDBY_TYPE_VFD = FALSE;  //有两种面板，一种是真待机面板，一种是假待机面板

/***********************add for led ***************************/
#define YWPANEL_MAX_LED_LEGNTH    4
#define YWPANEL_LOWER_START		  10
#define YWPANEL_UPPER_START       36
U8 YWPANEL_LedCharArray[]=
{
	0xfc,0x60,0xda,0xf2,0x66,0xb6,0xbe,0xe0,0xfe,0xf6,/* 0~9*/
	0xee,0x3e,0x1a,0x7a,0xde,0x8e,0xf6,0x2e,0x60,0x70,/*a~j*/
	0x00,0x1c,0x00,0x2a,0x3a,0xce,0xe6,0x4e,0xb6,0x1e,/* k~t*/
	0x38,0x38,0x00,0x6e,0x76,0xda, /*u-z*/
	0xee,0xfe,0x9c,0xfc,0x9e,0x8e,0xbe,0x6e,0x60,0x70,/*A-J*/
	0x00,0x1c,0x00,0xec,0xfc,0xce,0x00,0xee,0xb6,0xe0,/*K-T*/
	0x7c,0x7c,0x00,0x6e,0x76,0xda/*U-Z*/
};	//d48zm modify

U8  YWPANEL_LedDisplayData[YWPANEL_MAX_LED_LEGNTH];
/***********************add for led  end***************************/

// 注意：因最高位一定为“1”，故略去
const U16 cnCRC_16 = 0x8005;
// CRC-16 = X16 + X15 + X2 + X0
const U16 cnCRC_CCITT = 0x1021;
// CRC-CCITT = X16 + X12 + X5 + X0，据说这个 16 位 CRC 多项式比上一个要好

U32 Table_CRC[256]; // CRC 表

/********************************  数据结构************************************/
typedef struct SegAddrVal_s{
	int Segaddr1;
	int Segaddr2;
	int CurrValue1;
	int CurrValue2;
}SegAddrVal_T;

static SegAddrVal_T VfdSegAddr[15];

typedef enum VFDMode_e{
    VFDWRITEMODE,
    VFDREADMODE
}VFDMode_T;

typedef enum SegNum_e{
	SEGNUM1 = 0,
	SEGNUM2
}SegNum_T;
int CharLib[49][2] =
{
	{0xF1, 0x38},	//A
	{0x74, 0x72},	//B
	{0x01, 0x68},	//C
	{0x54, 0x72},	//D
	{0xE1, 0x68},	//E
	{0xE1, 0x28},	//F
	{0x71, 0x68},	//G
	{0xF1, 0x18},	//H
	{0x44, 0x62},	//I
	{0x45, 0x22},	//J
	{0x46, 0x06},	//K
	{0x01, 0x48},	//L
	{0x51, 0x1D},	//M
	{0x53, 0x19},	//N
	{0x11, 0x78},	//O
	{0xE1, 0x38},	//P
	{0x13, 0x78},	//Q
	{0xE3, 0x38},	//R
	{0xF0, 0x68},	//S
	{0x44, 0x22},	//T
	{0x11, 0x58},	//U
	{0x49, 0x0C},	//V
	{0x5B, 0x18},	//W
	{0x4A, 0x05},	//X
	{0x44, 0x05},	//Y
	{0x48, 0x64},	//Z
	/* A--Z  */
	{0x01, 0x68},
	{0x42, 0x01},
	{0x10, 0x70},	//
	{0x43, 0x09},	//
	{0xE0, 0x00},	//
	{0xEE, 0x07},	//
	{0xE4, 0x02},	//
	{0x50, 0x00},	//
	{0xE0, 0x00},	//
	{0x05, 0x00},	//
	{0x48, 0x04},	//

	{0x11, 0x78},	//
	{0x44, 0x02},	//
	{0xE1, 0x70},	//
	{0xF0, 0x70},	//
	{0xF0, 0x18},	//
	{0xF0, 0x68},	//
	{0xF1, 0x68},	//
	{0x10, 0x30},	//
	{0xF1, 0x78},	//
	{0xF0, 0x78},	//
	/* 0--9  */
	{0x00, 0x00},//space
	//{0x4a, 0x40}  //升级专用{0x4e,0x40}
	{0xe4, 0x58}
};

int NumLib[11][2] =
{
	{0x77, 0x77},	//{01110111, 01110111},
	{0x24, 0x22},	//{00100100, 00010010},
	{0x6B, 0x6D},	//{01101011, 01101101},
	{0x6D, 0x6B},	//{01101101, 01101011},
	{0x3C, 0x3A},	//{00111100, 00111010},
	{0x5D, 0x5B},	//{01011101, 01011011},
	{0x5F, 0x5F},	//{01011111, 01011111},
	{0x64, 0x62},	//{01100100, 01100010},
	{0x7F, 0x7F},	//{01111111, 01111111},
	{0x7D, 0x7B}, 	//{01111101, 01111011},
	{0x00, 0x00}    //space
};
char dvfd_bitmap[95][5] =
	{
		//' ' 0x20 032
		{0x00, 0x00, 0x00, 0x00, 0x00, },
		//'!' 0x21 033
		{0x10, 0x42, 0x08, 0x01, 0x04, },
		//'"' 0x22 034
		{0x68, 0xad, 0x00, 0x00, 0x00, },
		//'#' 0x23 035
		{0x28, 0xf5, 0xd5, 0x57, 0x0a, },
		//'$' 0x24 036
		{0x10, 0x5f, 0x1c, 0x7d, 0x04, },
		//'%' 0x25 037
		{0x8c, 0x89, 0x88, 0xc8, 0x18, },
		//'&' 0x26 038
		{0x88, 0x52, 0x44, 0x4d, 0x16, },
		//''' 0x27 039
		{0x10, 0x42, 0x00, 0x00, 0x00, },
		//'(' 0x28 040
		{0x40, 0x44, 0x08, 0x41, 0x10, },
		//')' 0x29 041
		{0x04, 0x41, 0x08, 0x11, 0x01, },
		//'*' 0x2a 042
		{0x90, 0xea, 0x88, 0xab, 0x04, },
		//'+' 0x2b 043
		{0x00, 0x42, 0x3e, 0x21, 0x00, },
		//',' 0x2c 044
		{0x00, 0x00, 0x00, 0x31, 0x02, },
		//'-' 0x2d 045
		{0x00, 0xf0, 0x01, 0x00, 0x00, },
		//'.' 0x2e 046
		{0x00, 0x00, 0x00, 0x61, 0x00, },
		//'/' 0x2f 047
		{0x00, 0x88, 0x88, 0x08, 0x00, },
		//'0' 0x30 048
		{0xb8, 0x18, 0x63, 0x8c, 0x0e, },
		//'1' 0x31 049
		{0x10, 0x43, 0x08, 0x21, 0x1f, },
		//'2' 0x32 050
		{0xb8, 0x08, 0x11, 0x11, 0x1f, },
		//'3' 0x33 051
		{0x38, 0xc8, 0x20, 0x84, 0x0f, },
		//'4' 0x34 052
		{0x20, 0xa6, 0xd2, 0x47, 0x1c, },
		//'5' 0x35 053
		{0xbc, 0xf0, 0x20, 0x84, 0x0f, },
		//'6' 0x36 054
		{0x30, 0x11, 0x5e, 0x8c, 0x0e, },
		//'7' 0x37 055
		{0x7c, 0x88, 0x10, 0x22, 0x04, },
		//'8' 0x38 056
		{0xb8, 0x18, 0x5d, 0x8c, 0x0e, },
		//'9' 0x39 057
		{0xb8, 0x18, 0x3d, 0x44, 0x06, },
		//':' 0x3a 058
		{0x00, 0x46, 0x00, 0x61, 0x00, },
		//';' 0x3b 059
		{0x00, 0x46, 0x00, 0x31, 0x02, },
		//'<' 0x3c 060
		{0x40, 0x26, 0x06, 0x81, 0x00, },
		//'=' 0x3d 061
		{0x80, 0x0f, 0x3e, 0x00, 0x00, },
		//'>' 0x3e 062
		{0x04, 0x83, 0x30, 0x09, 0x00, },
		//'?' 0x3f 063
		{0xb8, 0x08, 0x19, 0x01, 0x04, },
		//'@' 0x40 064
		{0xb8, 0x98, 0x6b, 0x0e, 0x1e, },
		//'A' 0x41 065
		{0x08, 0x93, 0xe2, 0x8f, 0x11, },
		//'B' 0x42 066
		{0xbc, 0x18, 0x5f, 0x8c, 0x0f, },
		//'C' 0x43 067
		{0xb8, 0x18, 0x42, 0x88, 0x0e, },
		//'D' 0x44 068
		{0xbc, 0x18, 0x63, 0x8c, 0x0f, },
		//'E' 0x45 069
		{0xfc, 0x18, 0x4e, 0x89, 0x1f, },
		//'F' 0x46 070
		{0x78, 0x21, 0x9c, 0x12, 0x06, },
		//'G' 0x47 071
		{0xf8, 0x10, 0x42, 0x8e, 0x1e, },
		//'H' 0x48 072
		{0xc4, 0x18, 0x7f, 0x8c, 0x11, },
		//'I' 0x49 073
		{0x38, 0x42, 0x08, 0x21, 0x0e, },
		//'J' 0x4a 074
		{0x70, 0x08, 0x61, 0x8c, 0x0e, },
		//'K' 0x4b 075
		{0xec, 0x54, 0x4e, 0x4a, 0x13, },
		//'L' 0x4c 076
		{0x1c, 0x21, 0x84, 0x10, 0x1f, },
		//'M' 0x4d 077
		{0xc4, 0xbd, 0x6b, 0x8d, 0x1b, },
		//'N' 0x4e 078
		{0xe4, 0x39, 0x6b, 0xce, 0x13, },
		//'O' 0x4f 079
		{0xb8, 0x18, 0x63, 0x8c, 0x0e, },
		//'P' 0x50 080
		{0xbc, 0x18, 0x5f, 0x08, 0x03, },
		//'Q' 0x51 081
		{0xb8, 0x18, 0x63, 0x4d, 0x16, },
		//'R' 0x52 082
		{0xbc, 0x18, 0x5f, 0x49, 0x13, },
		//'S' 0x53 083
		{0xb8, 0x18, 0x1c, 0x8c, 0x0e, },
		//'T' 0x54 084
		{0xfc, 0x4a, 0x08, 0x21, 0x0e, },
		//'U' 0x55 085
		{0xec, 0x18, 0x63, 0x8c, 0x0e, },
		//'V' 0x56 086
		{0xc4, 0x18, 0xa5, 0x52, 0x04, },
		//'W' 0x57 087
		{0xc4, 0x58, 0x6b, 0x55, 0x0a, },
		//'X' 0x58 088
		{0x44, 0xa9, 0x88, 0xca, 0x11, },
		//'Y' 0x59 089
		{0x44, 0xa9, 0x08, 0x21, 0x0e, },
		//'Z' 0x5a 090
		{0xfc, 0x88, 0x88, 0x88, 0x1f, },
		//'[' 0x5b 091
		{0x30, 0x42, 0x08, 0x21, 0x0c, },
		//'\' 0x5c 092
		{0x80, 0x20, 0x08, 0x82, 0x00, },
		//']' 0x5d 093
		{0x18, 0x42, 0x08, 0x21, 0x06, },
		//'^' 0x5e 094
		{0x00, 0xa2, 0x22, 0x00, 0x00, },
		//'_' 0x5f 095
		{0x00, 0x00, 0x00, 0xf8, 0x00, },
		//'`' 0x60 096
		{0x00, 0x02, 0x00, 0x00, 0x00, },
		//'a' 0x61 097
		{0x00, 0x07, 0x7d, 0xf4, 0x00, },
		//'b' 0x62 098
		{0x08, 0xe1, 0xa4, 0x74, 0x00, },
		//'c' 0x63 099
		{0x00, 0x17, 0x43, 0xf0, 0x00, },
		//'d' 0x64 100
		{0x20, 0xe4, 0x52, 0x72, 0x00, },
		//'e' 0x65 101
		{0x00, 0x17, 0x7f, 0xf0, 0x00, },
		//'f' 0x66 102
		{0x60, 0xe2, 0x09, 0x21, 0x0e, },
		//'g' 0x67 103
		{0x00, 0x26, 0x25, 0x87, 0x0e, },
		//'h' 0x68 104
		{0x08, 0xe1, 0xa4, 0x94, 0x00, },
		//'i' 0x69 105
		{0x10, 0x60, 0x08, 0x71, 0x00, },
		//'j' 0x6a 106
		{0x10, 0xe0, 0x10, 0x52, 0x0c, },
		//'k' 0x6b 107
		{0x08, 0x6d, 0x8c, 0x92, 0x00, },
		//'l' 0x6c 108
		{0x18, 0x42, 0x08, 0x61, 0x00, },
		//'m' 0x6d 109
		{0x00, 0x57, 0x6b, 0xad, 0x00, },
		//'n' 0x6e 110
		{0x00, 0x26, 0xa5, 0x94, 0x00, },
		//'o' 0x6f 111
		{0x00, 0x26, 0xa5, 0x64, 0x00, },
		//'p' 0x70 112
		{0x00, 0x27, 0xa5, 0x13, 0x02, },
		//'q' 0x71 113
		{0x00, 0x97, 0x92, 0x43, 0x08, },
		//'r' 0x72 114
		{0x80, 0x26, 0x84, 0x10, 0x00, },
		//'s' 0x73 115
		{0x00, 0x17, 0x0c, 0x3a, 0x00, },
		//'t' 0x74 116
		{0x88, 0x27, 0x84, 0x62, 0x00, },
		//'u' 0x75 117
		{0x80, 0x1c, 0x63, 0x74, 0x00, },
		//'v' 0x76 118
		{0x80, 0x18, 0x95, 0x22, 0x00, },
		//'w' 0x77 119
		{0x80, 0x58, 0xeb, 0x56, 0x00, },
		//'x' 0x78 120
		{0x80, 0xa9, 0x88, 0x9a, 0x00, },
		//'y' 0x79 121
		{0x80, 0x29, 0x15, 0x23, 0x04, },
		//'z' 0x7a 122
		{0x80, 0x8f, 0x88, 0xf8, 0x00, },
		//'{' 0x7b 123
		{0x60, 0x42, 0x04, 0x21, 0x18, },
		//'|' 0x7c 124
		{0x10, 0x42, 0x08, 0x21, 0x04, },
		//'}' 0x7d 125
		{0x0c, 0x42, 0x10, 0x21, 0x03, },
		//'~' 0x7e 126
		{0x00, 0x20, 0x2a, 0x02, 0x00, },
	};


typedef enum YWPANEL_DataType_e
{
    YWPANEL_DATATYPE_LBD = 0x01,
    YWPANEL_DATATYPE_LCD,
    YWPANEL_DATATYPE_LED,
    YWPANEL_DATATYPE_VFD,
    YWPANEL_DATATYPE_DVFD,
    YWPANEL_DATATYPE_SCANKEY,
    YWPANEL_DATATYPE_IRKEY,

    YWPANEL_DATATYPE_GETVERSION,

    YWPANEL_DATATYPE_GETCPUSTATE,
    YWPANEL_DATATYPE_SETCPUSTATE,
// D02SH 2009/05/21 add begin
//Description:add for vfd state
    YWPANEL_DATATYPE_GETVFDSTATE,
    YWPANEL_DATATYPE_SETVFDSTATE,

    YWPANEL_DATATYPE_GETPOWERONSTATE,
    YWPANEL_DATATYPE_SETPOWERONSTATE,

    YWPANEL_DATATYPE_SETSTARTUPSTATE,

// D02SH 2009/05/21 add end


    YWPANEL_DATATYPE_GETSTBYKEY1,
    YWPANEL_DATATYPE_GETSTBYKEY2,
    YWPANEL_DATATYPE_GETSTBYKEY3,
    YWPANEL_DATATYPE_GETSTBYKEY4,
    YWPANEL_DATATYPE_GETSTBYKEY5,
    YWPANEL_DATATYPE_SETSTBYKEY1,
    YWPANEL_DATATYPE_SETSTBYKEY2,
    YWPANEL_DATATYPE_SETSTBYKEY3,
    YWPANEL_DATATYPE_SETSTBYKEY4,
    YWPANEL_DATATYPE_SETSTBYKEY5,

// D02SH 2009/05/20 add begin
//Description:for blue key
    YWPANEL_DATATYPE_GETBLUEKEY1,
    YWPANEL_DATATYPE_GETBLUEKEY2,
    YWPANEL_DATATYPE_GETBLUEKEY3,
    YWPANEL_DATATYPE_GETBLUEKEY4,
    YWPANEL_DATATYPE_GETBLUEKEY5,
    YWPANEL_DATATYPE_SETBLUEKEY1,
    YWPANEL_DATATYPE_SETBLUEKEY2,
    YWPANEL_DATATYPE_SETBLUEKEY3,
    YWPANEL_DATATYPE_SETBLUEKEY4,
    YWPANEL_DATATYPE_SETBLUEKEY5,
// D02SH 2009/05/20 add end

    YWPANEL_DATATYPE_GETIRCODE,
    YWPANEL_DATATYPE_SETIRCODE,

    YWPANEL_DATATYPE_GETENCRYPTMODE,    /*读取加密模式*/
    YWPANEL_DATATYPE_SETENCRYPTMODE,    /*设置加密模式*/
    YWPANEL_DATATYPE_GETENCRYPTKEY,       /*读取加密密钥*/
    YWPANEL_DATATYPE_SETENCRYPTKEY,       /*设置加密密钥*/

    YWPANEL_DATATYPE_GETVERIFYSTATE,     /*读取校验状态*/
    YWPANEL_DATATYPE_SETVERIFYSTATE,     /*读取校验状态*/

    YWPANEL_DATATYPE_GETTIME,                    /*读取面板时间*/
    YWPANEL_DATATYPE_SETTIME,                    /*设置面板时间*/
    YWPANEL_DATATYPE_CONTROLTIMER,          /*控制计时器开始计时or 停止计时*/

    YWPANEL_DATATYPE_SETPOWERONTIME,           /*设置待机时间(设置为0xffffffff时为关闭计时)*/
    YWPANEL_DATATYPE_GETPOWERONTIME,

    YWPANEL_DATATYPE_NUM
}YWPANEL_DataType_t;

typedef struct YWPANEL_LBDData_s
{
    U8  value; /*8位分别代表各个灯*/
}YWPANEL_LBDData_t;

typedef struct YWPANEL_LEDData_s
{
    U8  led1;
    U8  led2;
    U8  led3;
    U8  led4;
}YWPANEL_LEDData_t;

typedef struct YWPANEL_LCDData_s
{
    U8  value;
}YWPANEL_LCDData_t;

typedef enum YWPANEL_VFDDataType_e
{
    YWPANEL_VFD_SETTING = 0x1,
    YWPANEL_VFD_DISPLAY,
    YWPANEL_VFD_READKEY,
    YWPANEL_VFD_DISPLAYSTRING

}YWPANEL_VFDDataType_t;

typedef struct YWPANEL_VFDData_s
{
    YWPANEL_VFDDataType_t  type; /*1- setting  2- display 3- readscankey  4-displaystring*/

    U8 setValue;   //if type == YWPANEL_VFD_SETTING

    U8 address[16];  //if type == YWPANEL_VFD_DISPLAY
    U8 DisplayValue[16];

    U8 key;  //if type == YWPANEL_VFD_READKEY

}YWPANEL_VFDData_t;

typedef enum YWPANEL_DVFDDataType_e
{
    YWPANEL_DVFD_SETTING = 0x1,
    YWPANEL_DVFD_DISPLAY,
    YWPANEL_DVFD_DISPLAYSTRING,
    YWPANEL_DVFD_DISPLAYSYNC,
    YWPANEL_DVFD_SETTIMEMODE,
    YWPANEL_DVFD_GETTIMEMODE,
}YWPANEL_DVFDDataType_t;

typedef struct YWPANEL_DVFDData_s
{
    YWPANEL_DVFDDataType_t  type;

    U8						setValue;

    U8						ulen;
    U8						address[16];
    U8						DisplayValue[16][5];
}YWPANEL_DVFDData_t;

typedef struct YWPANEL_IRKEY_s
{
    U32 customCode;
    U32 dataCode;
}YWPANEL_IRKEY_t;

typedef struct YWPANEL_SCANKEY_s
{
    U32 keyValue;
}YWPANEL_SCANKEY_t;
typedef enum YWPANEL_CPUSTATE_s
{
    YWPANEL_CPUSTATE_UNKNOW,
    YWPANEL_CPUSTATE_RUNNING = 0x01,
    YWPANEL_CPUSTATE_STANDBY,
}YWPANEL_CPUSTATE_t;

typedef struct YWPANEL_CpuState_s
{
    YWPANEL_CPUSTATE_t state;

}YWPANEL_CpuState_t;

typedef struct YWPANEL_StandByKey_s
{
    U32     key;

}YWPANEL_StandByKey_t;

typedef enum YWPANEL_IRCODE_e
{
    YWPANEL_IRCODE_NONE,
    YWPANEL_IRCODE_NEC = 0x01,
    YWPANEL_IRCODE_RC5,
    YWPANEL_IRCODE_RC6,
    YWPANEL_IRCODE_PILIPS
}YWPANEL_IRCODE_T;

typedef struct YWPANEL_IRCode_s
{
    YWPANEL_IRCODE_T    code;
}YWPANEL_IRCode_t;


typedef enum YWPANEL_ENCRYPEMODE_e
{
    YWPANEL_ENCRYPEMODE_NONE =0x00,
    YWPANEL_ENCRYPEMODE_ODDBIT,             /*奇位取反*/
    YWPANEL_ENCRYPEMODE_EVENBIT,            /*偶位取反*/
    YWPANEL_ENCRYPEMODE_RAMDONBIT ,     /*随机位取反*/
}YWPANEL_ENCRYPEMODE_T;


typedef struct YWPANEL_EncryptMode_s
{
    YWPANEL_ENCRYPEMODE_T    mode;

}YWPANEL_EncryptMode_t;

typedef struct YWPANEL_EncryptKey_s
{
    U32       key;
}YWPANEL_EncryptKey_t;

typedef enum YWPANEL_VERIFYSTATE_e
{
    YWPANEL_VERIFYSTATE_NONE =0x00,
    YWPANEL_VERIFYSTATE_CRC16 ,
    YWPANEL_VERIFYSTATE_CRC32 ,
    YWPANEL_VERIFYSTATE_CHECKSUM ,

}YWPANEL_VERIFYSTATE_T;

typedef struct YWPANEL_VerifyState_s
{
    YWPANEL_VERIFYSTATE_T       state;
}YWPANEL_VerifyState_t;

typedef struct YWPANEL_Time_s
{
    U32       second;                       /*从1970年1月1日00点00分开始的秒数*/
}YWPANEL_Time_t;

typedef struct YWPANEL_ControlTimer_s
{
    BOOL            startFlag;                          // 0 - stop  1-start
}YWPANEL_ControlTimer_t;

typedef enum YWPANEL_LBDType_e
{
    YWPANEL_LBD_TYPE_POWER        =  ( 1 << 0 ),            /*  前面板Power灯 */
    YWPANEL_LBD_TYPE_SIGNAL        =  ( 1 << 1 ) ,           /*  前面板 Signal灯 */
    YWPANEL_LBD_TYPE_MAIL        =  ( 1 << 2 ),            /*  前面板Mail灯 */
    YWPANEL_LBD_TYPE_AUDIO        =  ( 1 << 3 )         /*  前面板Audio灯 */
}YWPANEL_LBDType_T;

typedef enum YWPANEL_LBDStatus_e
{
    YWPANEL_LBD_STATUS_OFF,
    YWPANEL_LBD_STATUS_ON,
    YWPANEL_LBD_STATUS_FL
}YWPANEL_LBDStatus_T;

typedef enum YWPAN_FP_MCUTYPE_E
{
    YWPANEL_FP_MCUTYPE_UNKNOW = 0x00,
    YWPANEL_FP_MCUTYPE_AVR_ATTING48,       //AVR MCU
    YWPANEL_FP_MCUTYPE_AVR_ATTING88,

    YWPAN_FP_MCUTYPE_MAX
}YWPAN_FP_MCUTYPE_T;

typedef	enum YWPANEL_FP_DispType_e
{
	YWPANEL_FP_DISPTYPE_UNKNOWN = 0x00,
	YWPANEL_FP_DISPTYPE_VFD = (1 << 0),
	YWPANEL_FP_DISPTYPE_LCD = (1 << 1),
	YWPANEL_FP_DISPTYPE_DVFD = (3),
	YWPANEL_FP_DISPTYPE_LED = (1 << 2),
	YWPANEL_FP_DISPTYPE_LBD = (1 << 3)
}YWPANEL_FP_DispType_t;

static YWPANEL_FP_DispType_t	panel_disp_type = YWPANEL_FP_DISPTYPE_UNKNOWN;

typedef struct YWPANEL_Version_s
{
    YWPAN_FP_MCUTYPE_T  CpuType;

    U8  DisplayInfo;
    U8  scankeyNum;
    U8  swMajorVersion;
    U8  swSubVersion;

}YWPANEL_Version_t;

typedef struct YWPANEL_FPData_s
{
    YWPANEL_DataType_t          dataType;

    union
    {
        YWPANEL_Version_t			version;
        YWPANEL_LBDData_t			lbdData;
        YWPANEL_LEDData_t			ledData;
        YWPANEL_LCDData_t			lcdData;
        YWPANEL_VFDData_t			vfdData;
        YWPANEL_DVFDData_t			dvfdData;
        YWPANEL_IRKEY_t				IrkeyData;
        YWPANEL_SCANKEY_t			ScanKeyData;
        YWPANEL_CpuState_t			CpuState;
        YWPANEL_StandByKey_t		stbyKey;
        YWPANEL_IRCode_t			irCode;
        YWPANEL_EncryptMode_t		EncryptMode;
        YWPANEL_EncryptKey_t		EncryptKey;
        YWPANEL_VerifyState_t		verifyState;
        YWPANEL_Time_t				time;
        YWPANEL_ControlTimer_t		TimeState;
    }data;

    BOOL                                 ack;

}YWPANEL_FPData_t;


U8 YWPANEL_CharArray[]=
{
	0x7B,0x11,0x76,0x75,0x1D,0x6D,0x6F,0x31,0xFF,0x7D/* 0~9*/
};
enum
{
    YWPANEL_INIT_INSTR_GETVERSION= 0x50,


    YWPANEL_INIT_INSTR_GETCPUSTATE = 0x52,
    YWPANEL_INIT_INSTR_SETCPUSTATE ,

    YWPANEL_INIT_INSTR_GETSTBYKEY1 ,
    YWPANEL_INIT_INSTR_GETSTBYKEY2 ,
    YWPANEL_INIT_INSTR_GETSTBYKEY3 ,
    YWPANEL_INIT_INSTR_GETSTBYKEY4 ,
    YWPANEL_INIT_INSTR_GETSTBYKEY5 ,

    YWPANEL_INIT_INSTR_SETSTBYKEY1 ,
    YWPANEL_INIT_INSTR_SETSTBYKEY2 ,
    YWPANEL_INIT_INSTR_SETSTBYKEY3 ,
    YWPANEL_INIT_INSTR_SETSTBYKEY4 ,
    YWPANEL_INIT_INSTR_SETSTBYKEY5 ,

    YWPANEL_INIT_INSTR_GETIRCODE , /*0x5e*/
    YWPANEL_INIT_INSTR_SETIRCODE,


    YWPANEL_INIT_INSTR_GETENCRYPTMODE,       /*0x60*/
    YWPANEL_INIT_INSTR_SETENCRYPTMODE,

    YWPANEL_INIT_INSTR_GETENCRYPTKEY,       /*0x62*/
    YWPANEL_INIT_INSTR_SETENCRYPTKEY,


    YWPANEL_INIT_INSTR_GETVERIFYSTATE ,       /*0x64*/
    YWPANEL_INIT_INSTR_SETVERIFYSTATE,

    YWPANEL_INIT_INSTR_GETTIME,                 /*0x66*/
    YWPANEL_INIT_INSTR_SETTIME,                 /*0x67*/
    YWPANEL_INIT_INSTR_CONTROLTIMER,                 /*0x68*/
    YWPANEL_INIT_INSTR_POWERONTIME,         /*0x69*/
    YWPANEL_INIT_INSTR_GETPOWERONTIME,         /*0x6a*/

    YWPANEL_INIT_INSTR_GETVFDSTANDBYSTATE,      /*0x6b*/
    YWPANEL_INIT_INSTR_SETVFDSTANDBYSTATE,      /*0x6c*/

    YWPANEL_INIT_INSTR_GETBLUEKEY1 ,    /*0x6d*/
    YWPANEL_INIT_INSTR_GETBLUEKEY2 ,
    YWPANEL_INIT_INSTR_GETBLUEKEY3 ,
    YWPANEL_INIT_INSTR_GETBLUEKEY4 ,    /*0x70*/
    YWPANEL_INIT_INSTR_GETBLUEKEY5 ,

    YWPANEL_INIT_INSTR_SETBLUEKEY1 ,    /*0x72*/
    YWPANEL_INIT_INSTR_SETBLUEKEY2 ,
    YWPANEL_INIT_INSTR_SETBLUEKEY3 ,
    YWPANEL_INIT_INSTR_SETBLUEKEY4 ,
    YWPANEL_INIT_INSTR_SETBLUEKEY5 ,    /*0x76*/

    YWPANEL_INIT_INSTR_GETPOWERONSTATE,      /*0x77*/
    YWPANEL_INIT_INSTR_SETPOWERONSTATE,      /*0x78*/

    YWPANEL_INIT_INSTR_GETSTARTUPSTATE,      /*0x79*/

	YWPANEL_INIT_INSTR_SETMVFDDISPLAY = 0x80,		/*0x80*/
	YWPANEL_INIT_INSTR_STRMVFDBRIGHTNESS,
	YWPANEL_INIT_INSTR_SETMVFDTIMEMODE,
	YWPANEL_INIT_INSTR_GETMVFDTIMEMODE,

};

enum YWPANL_READ_INSTR_e
{
    YWPANEL_READ_INSTR_ACK = 0x10,
    YWPANEL_READ_INSTR_SCANKEY = 0x11,
    YWPANEL_READ_INSTR_IRKEY = 0x12,
    YWPANEL_READ_INSTR_VFDKEY = 0x13,

};

enum YWPANL_WRITE_INSTR_e
{
    YWPANEL_DISPLAY_INSTR_LBD = 0x30,
    YWPANEL_DISPLAY_INSTR_LED,
    YWPANEL_DISPLAY_INSTR_LCD,
    YWPANEL_DISPLAY_INSTR_VFD,
    YWPANEL_DISPLAY_INSTR_MVFD_SINGLE,
    YWPANEL_DISPLAY_INSTR_MVFD,

};

typedef struct YWPANEL_I2CData_s
{
    U8                                          writeBuffLen;
    U8                                          writeBuff[YWPANEL_FP_DATA_MAX_LENGTH];

    U8                                          readBuff[YWPANEL_FP_INFO_MAX_LENGTH]; /*I2C返回的缓冲*/

}YWPANEL_I2CData_t;

typedef enum LogNum_e
{
/*----------------------------------11G-------------------------------------*/
	PLAY_FASTBACKWARD = 11*15+1,
	PLAY_HEAD,
	PLAY_LOG,
	PLAY_TAIL,
	PLAY_FASTFORWARD,
	PLAY_PAUSE,
    REC1,
    MUTE,
    CYCLE,
    DUBI,
    CA,
    CI,
    USB,
    DOUBLESCREEN,
    REC2,
/*----------------------------------12G-------------------------------------*/
    HDD_A8 = 12*15+1,
    HDD_A7,
    HDD_A6,
    HDD_A5,
    HDD_A4,
    HDD_A3,
    HDD_FULL,
    HDD_A2,
    HDD_A1,
    MP3,
    AC3,
    TVMODE_LOG,
    AUDIO,
    ALERT,
    HDD_A9,
/*----------------------------------13G-------------------------------------*/
    CLOCK_PM = 13*15+1,
    CLOCK_AM,
    CLOCK,
    TIME_SECOND, /*时与分之间的点*/
    DOT2,
    STANDBY,
    TER,
    DISK_S3,
    DISK_S2,
    DISK_S1,
    DISK_S0,
    SAT,
    TIMESHIFT,
    DOT1,
    CAB,
  /*----------------------------------end-------------------------------------*/
    LogNum_Max
}LogNum_T;

//需要显示的内容
static char show_p_boot[]="!BOOT-";
static char show_p_kern[]="!K0";
static char show_p_root[]="!R0";

extern void VFD_ROOTFS_P(int pro_rate);
extern void VFD_U_BOOT(void);
extern void VFD_U_Kernel(void);
extern void VFD_Warn(char* str);
extern void VFD_U_MENU(void);
extern void VFD_BOOT_P(int pro_rate);
extern void VFD_KERNEL_P(int pro_rate);
extern void VFD_Print(char * str);

void YW_VFD_WR(int data)
{
    int i = 0;

	for(i = 0; i < 8; i++)
	{
        STPIO_SET_PIN(PIO_PORT(3), 4, 0);
        udelay(1);
		if(data & 0x01)
		{
            STPIO_SET_PIN(PIO_PORT(3), 2, 1);
		}
		else
		{
            STPIO_SET_PIN(PIO_PORT(3), 2, 0);
		}
        STPIO_SET_PIN(PIO_PORT(3), 4, 1);
        udelay(1);
		data >>= 1;
	}
}

int  YW_VFD_RD(void)
{
    int i;
    int val = 0 ;
    SET_PIO_PIN(PIO_PORT(3), 2, STPIO_IN);
    for (i = 0; i < 8; i++)
    {
        //val >>= 1;
        STPIO_SET_PIN(PIO_PORT(3), 4, 0);
        udelay(1);
        val |= (STPIO_GET_PIN(PIO_PORT(3), 2) << i);
        STPIO_SET_PIN(PIO_PORT(3), 4, 1);
        udelay(1);
    }
    udelay(1);
    SET_PIO_PIN(PIO_PORT(3), 2, STPIO_OUT);
    return val;
}

void SegAddrInit(void)
{
	int i = 0, addr = 0xC0;
	for(i = 0; i < 13; i++)
	{
		VfdSegAddr[i + 1].CurrValue1 = 0;
		VfdSegAddr[i + 1].CurrValue2 = 0;
		VfdSegAddr[i + 1].Segaddr1 = addr;
		VfdSegAddr[i + 1].Segaddr2 = addr + 1;
		addr += 3;
	}
}

void SetDigAndSeg(int dignum, SegNum_T segnum, int val)
{
    int  addr=0;
    VFD_CS_CLR();
    if(segnum == SEGNUM1)
	{
        addr = VfdSegAddr[dignum].Segaddr1;
        VfdSegAddr[dignum].CurrValue1 = val ;
	}
    else if(segnum == SEGNUM2)
    {
        addr = VfdSegAddr[dignum].Segaddr2;
        VfdSegAddr[dignum].CurrValue2 = val ;
    }
    YW_VFD_WR(addr);
    udelay(10);
    YW_VFD_WR(val);
    VFD_CS_SET();
}

void ShowContent(void)
{
    VFD_CS_CLR();
    YW_VFD_WR(0x8F);
    VFD_CS_SET();
}

void VFD_ClearAll(void)
{
 	int i = 0;
 	for(i = 0; i < 13; i++)
	{
        SetDigAndSeg(i + 1,SEGNUM1,0x00);
        ShowContent();
		VfdSegAddr[i + 1].CurrValue1 = 0x00;
        SetDigAndSeg(i + 1,SEGNUM2,0x00);
        ShowContent();
		VfdSegAddr[i + 1].CurrValue2 = 0;
    }
}
int YWPANEL_VFD_SetMode(VFDMode_T mode)
{

    int      ST_ErrCode = 0x0;
    int      data = 0;
    //模式不同对CS要求不同
    if(mode == VFDWRITEMODE)
    {
        data = 0x44;
        VFD_CS_CLR();
        YW_VFD_WR(data);
        VFD_CS_SET();
    }
    else if(mode == VFDREADMODE)
    {
        data = 0x46;
        YW_VFD_WR(data);
        udelay(5);
    }
    return ST_ErrCode;
}

void VFDDrawChar(char c, int position)
{
	if(position < 1 || position > 8)
	{
		return;
	}
	if(c >= 65 && c <= 95)
		c = c - 65;
	else if(c >= 97 && c <= 122)
		c = c - 97;
	else if(c >= 42 && c <= 57)
		c = c - 11;
	else if(c == 32)
		c = 47;
	else if(c == 33)
        c = 48;
    else
	{
		return;
	}
	SetDigAndSeg(position, SEGNUM1, CharLib[(int)c][0]);
	SetDigAndSeg(position, SEGNUM2, CharLib[(int)c][1]);

    ShowContent();
}


void VFDDrawNum(int c, int position)
{
	int dignum = 0;

	if(position < 1 || position > 4)
	{
		return;
	}
    if(c > 9)
	{
		return;
	}
	dignum =10 - position / 3;
	if(position % 2 == 1)
	{
		if(NumLib[c][1] & 0x01)
			SetDigAndSeg(dignum, SEGNUM1, VfdSegAddr[dignum].CurrValue1 | 0x80);
		else
			SetDigAndSeg(dignum, SEGNUM1, VfdSegAddr[dignum].CurrValue1 & 0x7F);
    		VfdSegAddr[dignum].CurrValue2 = VfdSegAddr[dignum].CurrValue2 & 0x40;//sz
    		SetDigAndSeg(dignum, SEGNUM2, (NumLib[c][1] >> 1) | VfdSegAddr[dignum].CurrValue2);
	}
	else if(position % 2 == 0)
	{
	   if((NumLib[c][0] & 0x01))
        {
            SetDigAndSeg(dignum, SEGNUM2, VfdSegAddr[dignum].CurrValue2 | 0x40);// SZ  08-05-30
	   	}
	   else
			SetDigAndSeg(dignum, SEGNUM2, VfdSegAddr[dignum].CurrValue2 & 0x3F);
    		VfdSegAddr[dignum].CurrValue1 = VfdSegAddr[dignum].CurrValue1 & 0x80;
    		SetDigAndSeg(dignum, SEGNUM1, (NumLib[c][0] >>1 ) | VfdSegAddr[dignum].CurrValue1 );
	}
    ShowContent();
}


int YWVFD_COMMON_ShowTime(int hh,int mm)
{

    if( (hh > 24) && (mm > 60))
    {
       return -1;
    }
    VFDDrawNum((hh/10), 1);
    VFDDrawNum((hh%10), 2);
    VFDDrawNum((mm/10), 3);
    VFDDrawNum((mm%10), 4);
    return 0;
}

int YWVFD_COMMON_ShowString(char* str)
{
	int lenth = 0;
 	int i = 0;
 	lenth = strlen(str);
    if (lenth > 8)
    {
        lenth = 8;
    }
  	for(i = 0; i < 8; i++)
	{
        if(i < lenth)
        {
    	 	VFDDrawChar(*str, i + 1);
    		str++;
        }
        else
        {
            VFDDrawChar(' ', i + 1);
        }
	}
    return 0;
 }

void YWVFD_COMMON_ShowIco(LogNum_T log_num,int log_stat)
{
    int dig_num = 0,seg_num = 0;
    SegNum_T seg_part = 0;
    int  seg_offset = 0;
    int  addr = 0,val = 0;
    //--------------------参数过滤---------------------
    if(log_num >= LogNum_Max)
    {
        return;
    }
    dig_num = log_num/15;
    seg_num = log_num%15;
    seg_part = seg_num/9;
    VFD_CS_CLR();
    if(seg_part == SEGNUM1)
	{
        seg_offset = 0x01 << ((seg_num%9) - 1);
        addr = VfdSegAddr[dig_num].Segaddr1;
        if(log_stat == LOG_ON)
        {
           VfdSegAddr[dig_num].CurrValue1 |= seg_offset;
        }
        if(log_stat == LOG_OFF)
        {
           VfdSegAddr[dig_num].CurrValue1 &= (0xFF-seg_offset);
        }
        val = VfdSegAddr[dig_num].CurrValue1 ;
	}
    else if(seg_part == SEGNUM2)
    {
        seg_offset = 0x01 << ((seg_num%8) - 1);
        addr = VfdSegAddr[dig_num].Segaddr2;
        if(log_stat == LOG_ON)
        {
           VfdSegAddr[dig_num].CurrValue2 |= seg_offset;
        }
        if(log_stat == LOG_OFF)
        {
           VfdSegAddr[dig_num].CurrValue2 &= (0xFF-seg_offset);
        }
        val = VfdSegAddr[dig_num].CurrValue2 ;
    }
    YW_VFD_WR(addr);
    udelay(10);
    YW_VFD_WR(val);
    VFD_CS_SET();
    ShowContent();
}
int YWVFD_COMMON_SetBrightness(int level)
{
    int ret = 0 ;
    if(level <=0)
        level = 0;
    else if(level >= 7)
        level = 7;

    VFD_CS_CLR();
    YW_VFD_WR(0x88+level);
    VFD_CS_SET();
    return ret ;
}

int YWVFD_COMMON_GetScanKey(int *key)
{
    int key_val[6] ;
    int i = 0;

    VFD_CS_CLR();
    YWPANEL_VFD_SetMode(VFDREADMODE);

    for (i = 0; i < 6; i++)
    {
    	key_val[i] = YW_VFD_RD();
        //printf("Key_val[%d] = [0x%2x] \n",i,key_val[i]);
    }
    VFD_CS_SET();
    //---------转回写显示区模式
    YWPANEL_VFD_SetMode(VFDWRITEMODE);
    *key = key_val[5] ;

	//d48zm add
	if(*key == 0xff)	//invalid value
	{
		return -1;
	}
	else
	{
		 return 0;
	}
	//add end
}

// 构造 16 位 CRC 表
void YWPANEL_BuildTable16( U16 aPoly )
{
    U16 i, j;
    U16 nData;
    U16 nAccum;
    for ( i = 0; i < 256; i++ )
    {
        nData = ( U16 )( i << 8 );
        nAccum = 0;
        for ( j = 0; j < 8; j++ )
        {
            if ( ( nData ^ nAccum ) & 0x8000 )
                nAccum = ( nAccum << 1 ) ^ aPoly;
            else
                nAccum <<= 1;
            nData <<= 1;
        }
        Table_CRC[i] = ( U32 )nAccum;
     }
}
// 计算 16 位 CRC 值，CRC-16 或 CRC-CCITT
U16 YWPANEL_GenerateCRC16( U8 * buffer, U32 bufLength )
{
    U32 i;
    U16 nAccum = 0;
    YWPANEL_BuildTable16( cnCRC_CCITT ); // or cnCRC_CCITT
    for ( i = 0; i < bufLength; i++ )
        nAccum = ( nAccum << 8 ) ^ ( U16 )Table_CRC[( nAccum >> 8 ) ^ *buffer++];
    return nAccum;
}


BOOL YWPANEL_FP_DvfdFillCmd(YWPANEL_FPData_t *data,
										YWPANEL_I2CData_t *I2CData)
{
	switch(data->data.dvfdData.type)
	{
	    case YWPANEL_DVFD_DISPLAYSTRING:
	    {
			I2CData->writeBuff[0] = YWPANEL_DISPLAY_INSTR_MVFD;
	    }
	    break;
	    case YWPANEL_DVFD_DISPLAYSYNC:
	    {
			I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETMVFDDISPLAY;
	    }
	    break;
	    case YWPANEL_DVFD_SETTIMEMODE:
	    {
			I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETMVFDTIMEMODE;
	    }
	    break;
	    case YWPANEL_DVFD_GETTIMEMODE:
	    {
			I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETMVFDTIMEMODE;
	    }
	    break;
		default:
			break;
	}

	return TRUE;
}

BOOL YWPANEL_FP_DvfdFillLen(YWPANEL_FPData_t *data,
										YWPANEL_I2CData_t *I2CData)
{
    if(data->data.dvfdData.type == YWPANEL_DVFD_DISPLAYSTRING)
    {
        I2CData->writeBuff[1] = 1 + (5 + 1) * 4;
    }
    else
    {
        I2CData->writeBuff[1] = 0x4;
    }

	return TRUE;
}

void YWPANEL_FP_DvfdFillString(YWPANEL_FPData_t *data,
										YWPANEL_I2CData_t *I2CData,
										U8 	uMax)
{
	U8	i;

	for (i = 0; i < uMax; i++)
	{
		I2CData->writeBuff[6 * i + 3] = data->data.dvfdData.address[i];
		I2CData->writeBuff[6 * i + 4] = data->data.dvfdData.DisplayValue[i][0];
		I2CData->writeBuff[6 * i + 5] = data->data.dvfdData.DisplayValue[i][1];
		I2CData->writeBuff[6 * i + 6] = data->data.dvfdData.DisplayValue[i][2];
		I2CData->writeBuff[6 * i + 7] = data->data.dvfdData.DisplayValue[i][3];
		I2CData->writeBuff[6 * i + 8] = data->data.dvfdData.DisplayValue[i][4];
	}
}

BOOL YWPANEL_FP_DvfdFillData(YWPANEL_FPData_t *data,
										YWPANEL_I2CData_t *I2CData)
{
    switch(data->data.dvfdData.type)
    {
        case YWPANEL_DVFD_DISPLAYSTRING:
        {
			U8 	uMax = data->data.dvfdData.ulen;
			if (uMax > 4)
			{
			    uMax = 4;
			}
			I2CData->writeBuff[2] = uMax;
			YWPANEL_FP_DvfdFillString(data, I2CData, uMax);
        }
        break;
        case YWPANEL_DVFD_SETTIMEMODE:
        {
			I2CData->writeBuff[2] = data->data.dvfdData.setValue;
        }
        break;
		default:
			break;
    }

	return TRUE;
}

BOOL YWPANEL_FP_DvfdFillCrc(YWPANEL_FPData_t *data,
										YWPANEL_I2CData_t *I2CData)
{
    U16             usCRC16 = 0;

    if(data->data.dvfdData.type == YWPANEL_DVFD_DISPLAYSTRING)
    {
        usCRC16 = YWPANEL_GenerateCRC16(I2CData->writeBuff, 27);
        I2CData->writeBuff[27] =  (usCRC16 &0xff);
        I2CData->writeBuff[28] =  ((usCRC16>>8) &0xff);
        I2CData->writeBuffLen = 29;
    }
    else
    {
        usCRC16 = YWPANEL_GenerateCRC16(I2CData->writeBuff, 6);
        I2CData->writeBuff[6] =  (usCRC16 &0xff);
        I2CData->writeBuff[7] =  ((usCRC16>>8) &0xff);
        I2CData->writeBuffLen = 8;
    }

	return TRUE;
}

BOOL YWPANEL_FP_SetI2cData(YWPANEL_FPData_t  *data,YWPANEL_I2CData_t   *I2CData)
{
    U16             usCRC16 = 0;

    if((data == NULL)
        ||(I2CData == NULL)
        ||(data->dataType < YWPANEL_DATATYPE_LBD)
        ||(data->dataType > YWPANEL_DATATYPE_NUM))
    {
       //printf("bad parameter\n");
        return FALSE;
    }
    //printf("%s::date->dateType=[0x%x]\n",__FUNCTION__,data->dataType);
	YWVFD_Debug("%s:%d\n", __FUNCTION__, __LINE__);
    /*设置指令类型*/
    switch(data->dataType)
    {
        case YWPANEL_DATATYPE_LBD:
        {
            I2CData->writeBuff[0] = YWPANEL_DISPLAY_INSTR_LBD;
        }
        break;

        case YWPANEL_DATATYPE_LCD:
        {
            //I2CData->writeBuff[0] = YWPANEL_DISPLAY_INSTR_LED;
            I2CData->writeBuff[0] = YWPANEL_DISPLAY_INSTR_LCD;	//d48zm modify
        }
        break;

        case YWPANEL_DATATYPE_LED:
        {
            //I2CData->writeBuff[0] = YWPANEL_DISPLAY_INSTR_LCD;
            I2CData->writeBuff[0] = YWPANEL_DISPLAY_INSTR_LED;	//d48zm modify
        }
        break;

        case YWPANEL_DATATYPE_VFD:
        {
            I2CData->writeBuff[0] = YWPANEL_DISPLAY_INSTR_VFD;
        }
        break;

        case YWPANEL_DATATYPE_DVFD:
        {
			YWPANEL_FP_DvfdFillCmd(data, I2CData);
        }

        break;

        case YWPANEL_DATATYPE_SCANKEY:
        {
            I2CData->writeBuff[0] = YWPANEL_READ_INSTR_SCANKEY;
        }
        break;

        case YWPANEL_DATATYPE_IRKEY:
        {
            I2CData->writeBuff[0] = YWPANEL_READ_INSTR_IRKEY;
        }
        break;

        case  YWPANEL_DATATYPE_GETVERSION:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETVERSION;
        }
        break;

        case  YWPANEL_DATATYPE_GETCPUSTATE:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETCPUSTATE;
        }
        break;

        case  YWPANEL_DATATYPE_SETCPUSTATE:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETCPUSTATE;
        }
        break;

        case  YWPANEL_DATATYPE_SETSTARTUPSTATE:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETSTARTUPSTATE;
        }
        break;
        case  YWPANEL_DATATYPE_GETVFDSTATE:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETVFDSTANDBYSTATE;
        }
        break;

        case  YWPANEL_DATATYPE_SETVFDSTATE:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETVFDSTANDBYSTATE;
        }
        break;

        case  YWPANEL_DATATYPE_GETPOWERONSTATE:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETPOWERONSTATE;
        }
        break;

        case  YWPANEL_DATATYPE_SETPOWERONSTATE:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETPOWERONSTATE;
        }
        break;

        case YWPANEL_DATATYPE_GETSTBYKEY1:
        case YWPANEL_DATATYPE_GETSTBYKEY2:
        case YWPANEL_DATATYPE_GETSTBYKEY3:
        case YWPANEL_DATATYPE_GETSTBYKEY4:
        case YWPANEL_DATATYPE_GETSTBYKEY5:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETSTBYKEY1 + (data->dataType - YWPANEL_DATATYPE_GETSTBYKEY1);
        }
        break;

        case    YWPANEL_DATATYPE_SETSTBYKEY1:
        case    YWPANEL_DATATYPE_SETSTBYKEY2:
        case    YWPANEL_DATATYPE_SETSTBYKEY3:
        case    YWPANEL_DATATYPE_SETSTBYKEY4:
        case    YWPANEL_DATATYPE_SETSTBYKEY5:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETSTBYKEY1+data->dataType - YWPANEL_DATATYPE_SETSTBYKEY1;
        }
        break;
 //add for blue key
        case YWPANEL_DATATYPE_GETBLUEKEY1:
        case YWPANEL_DATATYPE_GETBLUEKEY2:
        case YWPANEL_DATATYPE_GETBLUEKEY3:
        case YWPANEL_DATATYPE_GETBLUEKEY4:
        case YWPANEL_DATATYPE_GETBLUEKEY5:
        {
          I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETBLUEKEY1 + (data->dataType - YWPANEL_DATATYPE_GETBLUEKEY1);
        }
        break;

        case    YWPANEL_DATATYPE_SETBLUEKEY1:
        case    YWPANEL_DATATYPE_SETBLUEKEY2:
        case    YWPANEL_DATATYPE_SETBLUEKEY3:
        case    YWPANEL_DATATYPE_SETBLUEKEY4:
        case    YWPANEL_DATATYPE_SETBLUEKEY5:
        {
          I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETBLUEKEY1+data->dataType - YWPANEL_DATATYPE_SETBLUEKEY1;
        }
        break;
//add end
        case    YWPANEL_DATATYPE_GETIRCODE:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETIRCODE;
        }
        break;


        case    YWPANEL_DATATYPE_SETIRCODE:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETIRCODE;
        }
        break;

        case    YWPANEL_DATATYPE_GETENCRYPTMODE:    /*读取加密模式*/
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETENCRYPTMODE;
        }
        break;

        case    YWPANEL_DATATYPE_SETENCRYPTMODE:  /*设置加密模式*/
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETENCRYPTMODE;
        }
        break;

        case    YWPANEL_DATATYPE_GETENCRYPTKEY:      /*读取加密密钥*/
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETENCRYPTKEY;
        }
        break;

        case    YWPANEL_DATATYPE_SETENCRYPTKEY:       /*设置加密密钥*/
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETENCRYPTKEY;
        }
        break;

        case   YWPANEL_DATATYPE_GETVERIFYSTATE:     /*读取校验状态*/
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETVERIFYSTATE;
        }
        break;

        case    YWPANEL_DATATYPE_SETVERIFYSTATE:    /*读取校验状态*/
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETVERIFYSTATE;
        }
        break;

        case    YWPANEL_DATATYPE_GETTIME:                    /*读取面板时间*/
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETTIME;
        }
        break;

        case    YWPANEL_DATATYPE_SETTIME:                  /*设置面板时间*/
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_SETTIME;
        }
        break;

        case    YWPANEL_DATATYPE_CONTROLTIMER:         /*控制计时器开始计时or 停止计时*/
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_CONTROLTIMER;
        }
        break;

        case    YWPANEL_DATATYPE_SETPOWERONTIME:         /*设置定时关机的时间*/
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_POWERONTIME;
        }
        break;
        case    YWPANEL_DATATYPE_GETPOWERONTIME:
        {
            I2CData->writeBuff[0] = YWPANEL_INIT_INSTR_GETPOWERONTIME;
        }
        break;
        default:
        break;

    }

	YWVFD_Debug("%s:%d\n", __FUNCTION__, __LINE__);
    /*设置指令的长度*/
    switch(data->dataType)
    {
        case YWPANEL_DATATYPE_VFD:
        {
            if(data->data.vfdData.type == YWPANEL_VFD_DISPLAYSTRING)
            {
                I2CData->writeBuff[1] = 0x22;     /*指令的长度 = 数据长度+指令类型长度*/
            }
            else
            {
                I2CData->writeBuff[1] = 0x4;
            }
        }
        break;
        case YWPANEL_DATATYPE_DVFD:
        {
			YWPANEL_FP_DvfdFillLen(data, I2CData);
        }
        break;
		default:
        {
            I2CData->writeBuff[1] = 0x4;
        }
        break;
    }
	YWVFD_Debug("%s:%d\n", __FUNCTION__, __LINE__);
    /*设置指令数据*/
    switch(data->dataType)
    {
        case YWPANEL_DATATYPE_LBD:
        {
            I2CData->writeBuff[2] = data->data.lbdData.value;
        }
        break;

        case YWPANEL_DATATYPE_LCD:
        {
            I2CData->writeBuff[2] = data->data.lcdData.value;
        }
        break;

        case YWPANEL_DATATYPE_LED:
        {
            I2CData->writeBuff[2] = data->data.ledData.led1;
            I2CData->writeBuff[3] = data->data.ledData.led2;
            I2CData->writeBuff[4] = data->data.ledData.led3;
            I2CData->writeBuff[5] = data->data.ledData.led4;
        }
        break;

        case YWPANEL_DATATYPE_VFD:
        {
            I2CData->writeBuff[2] = data->data.vfdData.type;
            switch(data->data.vfdData.type)
            {
                case YWPANEL_VFD_SETTING:
                {
                    I2CData->writeBuff[3] = data->data.vfdData.setValue;
                }
                break;
                case YWPANEL_VFD_DISPLAY:
                {
                    I2CData->writeBuff[3] = data->data.vfdData.address[0];
                    I2CData->writeBuff[4] = data->data.vfdData.DisplayValue[0];
                }
                break;
                case YWPANEL_VFD_READKEY:
                {
                    I2CData->writeBuff[3] = data->data.vfdData.key;
                }
                break;
                case YWPANEL_VFD_DISPLAYSTRING:
                {
                    int i;
                    for(i=0; i<16;i++)
                    {
                        I2CData->writeBuff[4+2*i] = data->data.vfdData.address[i];
                        I2CData->writeBuff[4+2*i+1] = data->data.vfdData.DisplayValue[i];
                    }
                }
                break;
            }
        }
        break;

        case YWPANEL_DATATYPE_DVFD:
        {
			YWPANEL_FP_DvfdFillData(data, I2CData);
        }
        break;

        case YWPANEL_DATATYPE_SETCPUSTATE:
        {
            I2CData->writeBuff[2] = data->data.CpuState.state;
        }
        break;

        case YWPANEL_DATATYPE_SETSTARTUPSTATE:
        {
            I2CData->writeBuff[2] = data->data.CpuState.state;
        }
        break;

        case YWPANEL_DATATYPE_SETVFDSTATE:
        {
           I2CData->writeBuff[2] = data->data.CpuState.state;
        }
        break;

        case YWPANEL_DATATYPE_SETPOWERONSTATE:
        {
            I2CData->writeBuff[2] = data->data.CpuState.state;
        }
        break;

        case YWPANEL_DATATYPE_SETSTBYKEY1:
        case YWPANEL_DATATYPE_SETSTBYKEY2:
        case YWPANEL_DATATYPE_SETSTBYKEY3:
        case YWPANEL_DATATYPE_SETSTBYKEY4:
        case YWPANEL_DATATYPE_SETSTBYKEY5:
        {
            I2CData->writeBuff[2] = (U8) ((data->data.stbyKey.key>>24)&0xff);
            I2CData->writeBuff[3] = (U8) ((data->data.stbyKey.key >>16)&0xff);
            I2CData->writeBuff[4] = (U8) ((data->data.stbyKey.key >>8)&0xff);
            I2CData->writeBuff[5] = (U8) (data->data.stbyKey.key&0xff);
        }
        break;
//add for blue key
        case YWPANEL_DATATYPE_SETBLUEKEY1:
        case YWPANEL_DATATYPE_SETBLUEKEY2:
        case YWPANEL_DATATYPE_SETBLUEKEY3:
        case YWPANEL_DATATYPE_SETBLUEKEY4:
        case YWPANEL_DATATYPE_SETBLUEKEY5:
        {
            I2CData->writeBuff[2] = (U8) ((data->data.stbyKey.key>>24)&0xff);
            I2CData->writeBuff[3] = (U8) ((data->data.stbyKey.key >>16)&0xff);
            I2CData->writeBuff[4] = (U8) ((data->data.stbyKey.key >>8)&0xff);
            I2CData->writeBuff[5] = (U8) (data->data.stbyKey.key&0xff);
        }
        break;
//add end
        case YWPANEL_DATATYPE_SETIRCODE:
        {
            I2CData->writeBuff[2] =  data->data.irCode.code;
        }
        break;

        case YWPANEL_DATATYPE_SETENCRYPTMODE:
        {
            I2CData->writeBuff[2] =  data->data.EncryptMode.mode;
        }
        break;

        case YWPANEL_DATATYPE_SETENCRYPTKEY:
        {
            I2CData->writeBuff[2] = (U8) ((data->data.EncryptKey.key>>24)&0xff);
            I2CData->writeBuff[3] = (U8) ((data->data.EncryptKey.key >>16)&0xff);
            I2CData->writeBuff[4] = (U8) ((data->data.EncryptKey.key >>8)&0xff);
            I2CData->writeBuff[5] = (U8) (data->data.EncryptKey.key&0xff);
        }
        break;

        case YWPANEL_DATATYPE_SETVERIFYSTATE:
        {
            I2CData->writeBuff[2] = data->data.verifyState.state;
        }
        break;

        case YWPANEL_DATATYPE_SETTIME:
        {
            I2CData->writeBuff[2] = (U8) ((data->data.time.second>>24)&0xff);
            I2CData->writeBuff[3] = (U8) ((data->data.time.second >>16)&0xff);
            I2CData->writeBuff[4] = (U8) ((data->data.time.second >>8)&0xff);
            I2CData->writeBuff[5] = (U8) (data->data.time.second&0xff);
        }
        break;

        case YWPANEL_DATATYPE_CONTROLTIMER:
        {
            I2CData->writeBuff[2] = data->data.TimeState.startFlag;
        }
        break;

        case YWPANEL_DATATYPE_SETPOWERONTIME:
        {
            I2CData->writeBuff[2] = (U8) ((data->data.time.second>>24)&0xff);
            I2CData->writeBuff[3] = (U8) ((data->data.time.second >>16)&0xff);
            I2CData->writeBuff[4] = (U8) ((data->data.time.second >>8)&0xff);
            I2CData->writeBuff[5] = (U8) (data->data.time.second&0xff);
        }
        break;

        default:
        break;
    }

	YWVFD_Debug("%s:%d\n", __FUNCTION__, __LINE__);
    /*设置CRC16 连指令ID一起校验*/
    switch(data->dataType)
    {
        case YWPANEL_DATATYPE_VFD:
        {
            if(data->data.vfdData.type == YWPANEL_VFD_DISPLAYSTRING)
            {
                usCRC16 = YWPANEL_GenerateCRC16(I2CData->writeBuff, 36);
                I2CData->writeBuff[36] =  (usCRC16 &0xff);
                I2CData->writeBuff[37] =  ((usCRC16>>8) &0xff);
                I2CData->writeBuffLen = 38;
            }
            else
            {
                usCRC16 = YWPANEL_GenerateCRC16(I2CData->writeBuff, 6);
                I2CData->writeBuff[6] =  (usCRC16 &0xff);
                I2CData->writeBuff[7] =  ((usCRC16>>8) &0xff);
                I2CData->writeBuffLen = 8;
            }
        }
        break;
        case YWPANEL_DATATYPE_DVFD:
        {
			YWPANEL_FP_DvfdFillCrc(data, I2CData);
        }
        break;

        default:
        {
            usCRC16 = YWPANEL_GenerateCRC16(I2CData->writeBuff, 6);
            I2CData->writeBuff[6] =  (usCRC16 &0xff);
            I2CData->writeBuff[7] =  ((usCRC16>>8) &0xff);
            I2CData->writeBuffLen = 8;
        }
        break;
    }

    return TRUE;
}

BOOL YWPANEL_FP_ParseI2cData(YWPANEL_FPData_t  *data,YWPANEL_I2CData_t   *I2CData)
{
    U16     crc16Code = 0;
    U16     receiveCode = 0;

    U8       dataType;
    U8       datalength;

    if((data == NULL) ||(I2CData == NULL))
    {
//        printf("%s::error @%d\n",__FUNCTION__,__LINE__);
        return FALSE;
    }
    receiveCode =((U16)(I2CData->readBuff[7]<<8)&0xff00) |((U16)(I2CData->readBuff[6])&0xff);
    crc16Code = YWPANEL_GenerateCRC16(I2CData->readBuff, 6);

    if(receiveCode != crc16Code )
    {
//        printf("check crc16 is wrong! at %d \n",__LINE__);
        //return FALSE;	 //YWDRIVER_MODI d48zm 2009/12/16 REMARK
    }

//    //printf(TRACE_INFO,"%s::date->dateType=[0x%x]\n",__FUNCTION__,data->dataType);
    dataType = I2CData->readBuff[0];
    datalength = I2CData->readBuff[1];

	//zy 2008-10-07
	switch(data->dataType)
    {
        case YWPANEL_DATATYPE_LBD:
        case YWPANEL_DATATYPE_LCD:
        case YWPANEL_DATATYPE_LED:
        //case YWPANEL_DATATYPE_SCANKEY:
        case YWPANEL_DATATYPE_SETCPUSTATE:
        case YWPANEL_DATATYPE_SETVFDSTATE:
        case YWPANEL_DATATYPE_SETPOWERONSTATE:
        case YWPANEL_DATATYPE_SETSTBYKEY1:
        case YWPANEL_DATATYPE_SETSTBYKEY2:
        case YWPANEL_DATATYPE_SETSTBYKEY3:
        case YWPANEL_DATATYPE_SETSTBYKEY4:
        case YWPANEL_DATATYPE_SETSTBYKEY5:
//add for blue key
        case YWPANEL_DATATYPE_SETBLUEKEY1:
        case YWPANEL_DATATYPE_SETBLUEKEY2:
        case YWPANEL_DATATYPE_SETBLUEKEY3:
        case YWPANEL_DATATYPE_SETBLUEKEY4:
        case YWPANEL_DATATYPE_SETBLUEKEY5:
//add end
        case YWPANEL_DATATYPE_SETIRCODE:
        case YWPANEL_DATATYPE_SETENCRYPTMODE:    /*设置加密模式*/
        case YWPANEL_DATATYPE_SETENCRYPTKEY:       /*设置加密密钥*/
        case YWPANEL_DATATYPE_SETVERIFYSTATE:     /*读取校验状态*/
        case YWPANEL_DATATYPE_SETTIME:                   /*设置面板时间*/
        case YWPANEL_DATATYPE_CONTROLTIMER:          /*控制计时器开始计时or 停止计时*/
        case YWPANEL_DATATYPE_SETPOWERONTIME:           /*设置待机时间(设置为0xffffffff时为关闭计时)*/
        {
            if(dataType != YWPANEL_READ_INSTR_ACK)
            {
//                printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                return FALSE;
            }
        }
        break;

        case YWPANEL_DATATYPE_SCANKEY:
        {
            if(dataType != YWPANEL_READ_INSTR_SCANKEY)//modify by d48zm 2009/12/21
            {
				//printf("%s::error @%d dataType = %d\n",__FUNCTION__,__LINE__, dataType);
                return FALSE;
            }
        }
        break;

        case YWPANEL_DATATYPE_VFD:
        {
            if(data->data.vfdData.type == YWPANEL_VFD_READKEY)
            {
                if(dataType != YWPANEL_READ_INSTR_VFDKEY)
                {
//                    printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                    return FALSE;
                }
            }
            else
            {
                if(dataType != YWPANEL_READ_INSTR_ACK)
                {
//                    printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                    return FALSE;
                }
            }
        }
        break;

        case YWPANEL_DATATYPE_DVFD:
        {
            if (data->data.dvfdData.type == YWPANEL_DVFD_DISPLAYSTRING)
            {
                if(dataType != YWPANEL_READ_INSTR_ACK)
                {
//                    printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                    return FALSE;
                }
            }
			else if (data->data.dvfdData.type == YWPANEL_DVFD_SETTIMEMODE)
			{
				if(dataType != YWPANEL_READ_INSTR_ACK)
                {
//                    printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                    return FALSE;
                }
			}
			else if (data->data.dvfdData.type == YWPANEL_DVFD_GETTIMEMODE)
			{
				if(dataType != YWPANEL_INIT_INSTR_GETMVFDTIMEMODE)
				{
//                    printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                    return FALSE;
                }
			}
            else
            {
                if(dataType != YWPANEL_READ_INSTR_ACK)
                {
//                    printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                    return FALSE;
                }
            }
        }
        break;
        case YWPANEL_DATATYPE_GETSTBYKEY1:
        case YWPANEL_DATATYPE_GETSTBYKEY2:
        case YWPANEL_DATATYPE_GETSTBYKEY3:
        case YWPANEL_DATATYPE_GETSTBYKEY4:
        case YWPANEL_DATATYPE_GETSTBYKEY5:
        {
            if(dataType != (YWPANEL_INIT_INSTR_GETSTBYKEY1+ data->dataType -YWPANEL_DATATYPE_GETSTBYKEY1))
            {
//                printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                return FALSE;
            }
        }
        break;

		case YWPANEL_DATATYPE_GETBLUEKEY1:
        case YWPANEL_DATATYPE_GETBLUEKEY2:
        case YWPANEL_DATATYPE_GETBLUEKEY3:
        case YWPANEL_DATATYPE_GETBLUEKEY4:
        case YWPANEL_DATATYPE_GETBLUEKEY5:
        {
            if(dataType != (YWPANEL_INIT_INSTR_GETBLUEKEY1+ data->dataType -YWPANEL_DATATYPE_GETBLUEKEY1))
            {
//                printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                return FALSE;
            }
        }
        break;
        case YWPANEL_DATATYPE_GETTIME:
        {
            if(dataType != YWPANEL_INIT_INSTR_GETTIME)
            {
//                printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                return FALSE;
            }
        }
        break;
        case YWPANEL_DATATYPE_GETPOWERONTIME:
        {
            if(dataType != YWPANEL_INIT_INSTR_GETPOWERONTIME)
            {
//                printf("dateType=[0x%x]###\n",dataType);
//                printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                return FALSE;
            }
        }
        break;

        default:
        break;
    }


    switch(dataType)
    {
        case YWPANEL_READ_INSTR_ACK:  //ACK
        {
            data->ack = TRUE;
        }
        break;

        case YWPANEL_READ_INSTR_SCANKEY:  //scan key
        {
            data->data.ScanKeyData.keyValue= I2CData->readBuff[2];
            data->ack = TRUE;
        }
        break;

        case YWPANEL_READ_INSTR_IRKEY:
        {
            data->data.IrkeyData.customCode = ((I2CData->readBuff[3] <<8)&0xff00)|(I2CData->readBuff[2]);
            data->data.IrkeyData.dataCode = ((I2CData->readBuff[5] <<8)&0xff00)|(I2CData->readBuff[4]);
            data->ack = TRUE;
        }
        break;

        case YWPANEL_READ_INSTR_VFDKEY:
        {
            data->data.vfdData.key = I2CData->readBuff[2];
            data->ack = TRUE;
        }
        break;

        case YWPANEL_INIT_INSTR_GETVERSION: /*get version */
        {
            if(data->dataType != YWPANEL_DATATYPE_GETVERSION)
            {
//                printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                return FALSE;
            }
            data->data.version.CpuType = I2CData->readBuff[2];
            data->data.version.DisplayInfo = (I2CData->readBuff[3]>>4)&0x0f;
            data->data.version.scankeyNum = I2CData->readBuff[3]&0x0f;
            data->data.version.swMajorVersion = I2CData->readBuff[4];
            data->data.version.swSubVersion = I2CData->readBuff[5];

            data->ack = TRUE;
        }
        break;

        case YWPANEL_INIT_INSTR_GETCPUSTATE: /*get cpu state*/
        {
            if(data->dataType == YWPANEL_DATATYPE_GETCPUSTATE)
            {
                data->data.CpuState.state= I2CData->readBuff[2];
                data->ack = TRUE;
            }
            else
            {
//                printf("%s::error @%d\n",__FUNCTION__,__LINE__);
                return FALSE;
            }
        }
        break;

        case YWPANEL_INIT_INSTR_GETVFDSTANDBYSTATE: /*get vfd state*/
        {
           if(data->dataType == YWPANEL_DATATYPE_GETVFDSTATE)
           {
               data->data.CpuState.state= I2CData->readBuff[2];
               data->ack = TRUE;
           }
           else
           {
//               printf("%s::error @%d\n",__FUNCTION__,__LINE__);
               return FALSE;
           }
        }
        break;
        case YWPANEL_INIT_INSTR_GETMVFDTIMEMODE:
        {
           if((data->dataType == YWPANEL_DATATYPE_DVFD) &&
				(data->data.dvfdData.type == YWPANEL_DVFD_GETTIMEMODE))
           {
               data->data.dvfdData.setValue = I2CData->readBuff[2];
               data->ack = TRUE;
           }
           else
           {
//               printf("%s::error @%d\n",__FUNCTION__,__LINE__);
               return FALSE;
           }
        }
        break;
        case YWPANEL_INIT_INSTR_GETPOWERONSTATE: /*get power on  state*/
        {
           if(data->dataType == YWPANEL_DATATYPE_GETPOWERONSTATE)
           {
               data->data.CpuState.state= I2CData->readBuff[2];
               data->ack = TRUE;
           }
           else
           {
//               printf("%s::error @%d\n",__FUNCTION__,__LINE__);
               return FALSE;
           }
        }
        break;


        case YWPANEL_INIT_INSTR_GETSTBYKEY1: /*get standby key*/
        case YWPANEL_INIT_INSTR_GETSTBYKEY2: /*get standby key*/
        case YWPANEL_INIT_INSTR_GETSTBYKEY3: /*get standby key*/
        case YWPANEL_INIT_INSTR_GETSTBYKEY4: /*get standby key*/
        case YWPANEL_INIT_INSTR_GETSTBYKEY5: /*get standby key*/
        {
            data->data.stbyKey.key = ((I2CData->readBuff[2]<<24)&0xff000000)
                                                |((I2CData->readBuff[3]<<16)&0xff0000)
                                                |((I2CData->readBuff[4]<<8)&0xff00)
                                                |((I2CData->readBuff[5])&0xff);
            data->ack = TRUE;
        }
        break;
        case YWPANEL_INIT_INSTR_GETBLUEKEY1: /*get blue key*/
        case YWPANEL_INIT_INSTR_GETBLUEKEY2: /*get blue key*/
        case YWPANEL_INIT_INSTR_GETBLUEKEY3: /*get blue key*/
        case YWPANEL_INIT_INSTR_GETBLUEKEY4: /*get blue key*/
        case YWPANEL_INIT_INSTR_GETBLUEKEY5: /*get blue key*/
        {
            data->data.stbyKey.key = ((I2CData->readBuff[2]<<24)&0xff000000)
                                                |((I2CData->readBuff[3]<<16)&0xff0000)
                                                |((I2CData->readBuff[4]<<8)&0xff00)
                                                |((I2CData->readBuff[5])&0xff);
            data->ack = TRUE;
        }
        break;
        case YWPANEL_INIT_INSTR_GETIRCODE: /*get ir protocoll*/
        {
            data->data.irCode.code = I2CData->readBuff[2];
        }
        break;

        case YWPANEL_INIT_INSTR_GETENCRYPTMODE: /*get encrypt mode */
        {
            data->data.EncryptMode.mode = I2CData->readBuff[2];
        }
        break;

        case YWPANEL_INIT_INSTR_GETENCRYPTKEY: /*get encrypt key */
        {
            data->data.EncryptKey.key = (U32)I2CData->readBuff[2] |(U32) I2CData->readBuff[3]<<8
                                                 |(U32)I2CData->readBuff[4]<<16 |(U32)I2CData->readBuff[5]<<24;
        }
        break;

        case YWPANEL_INIT_INSTR_GETVERIFYSTATE: /*get verify state */
        {
            data->data.verifyState.state = I2CData->readBuff[2];
        }
        break;
        case YWPANEL_INIT_INSTR_GETPOWERONTIME:
        case YWPANEL_INIT_INSTR_GETTIME:
        {
            data->data.time.second = ((I2CData->readBuff[2]<<24)&0xff000000)
                                    |((I2CData->readBuff[3]<<16)&0xff0000)
                                    |((I2CData->readBuff[4]<<8)&0xff00)
                                    |((I2CData->readBuff[5])&0xff);
            data->ack = TRUE;
        }
        break;

        default:
        {
//            printf("%s::error @%d\n",__FUNCTION__,__LINE__);
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL YWPANEL_FPWriteDataToI2c(	U8 * writeBufer,U32 writeBufLen,
										U8 *readBuffer,	U32 readBufLen)
{
    BOOL           	ret = 0;

    #if 0
	int				i;
    printf("writebuf:");
    for(i=0;i<writeBufLen;i++)
    {
        printf("%x,",writeBufer[i]);
    }
    printf("\n");
    #endif  /* 0 */

    ret = i2c_write(COMBO_I2C_ADDR,0,0, writeBufer, writeBufLen);
    if (ret != COMBO_I2C_OK)
    {
//        printf("I2C write error for %d at %d\n",ret,__LINE__);
        return FALSE;
    }

	msleep(1);

    ret = i2c_read(COMBO_I2C_ADDR,0,0 ,readBuffer,readBufLen);
    if (ret != COMBO_I2C_OK)
    {
//        printf("I2C read error for %d at %d\n",ret,__LINE__);
        return FALSE;
    }
    #if 0
    printf("readbuf:");
    for(i=0;i<readBufLen;i++)
    {
        printf("%x,",readBuffer[i]);
    }
    printf("\n");
    #endif  /* 0 */
    return TRUE;
}

BOOL YWPANEL_FP_SendData(YWPANEL_FPData_t  *data)
{
    BOOL                ret = FALSE;
    YWPANEL_I2CData_t   I2CData;

    if(data == NULL)
    {
//        printf("bad parameter @ %d\n",__LINE__);
        return FALSE;
    }

    memset(&I2CData, 0, sizeof(I2CData));
    if(YWPANEL_FP_SetI2cData(data,&I2CData) != TRUE)
    {
//        printf("SetI2cData @ %d\n",__LINE__);
        return FALSE;
    }

	YWVFD_Debug("%s:%d\n", __FUNCTION__, __LINE__);
    ret = YWPANEL_FPWriteDataToI2c( I2CData.writeBuff, I2CData.writeBuffLen,
                                    I2CData.readBuff, YWPANEL_FP_INFO_MAX_LENGTH);
    if(ret != TRUE)
    {
//        printf("YWPANEL_FPWriteDataToI2c @ %d\n",__LINE__);
        return FALSE;
    }

	YWVFD_Debug("%s:%d\n", __FUNCTION__, __LINE__);

    ret = YWPANEL_FP_ParseI2cData(data, &I2CData);
    if(ret != TRUE)
    {
//        printf("YWPANEL_FP_ParseI2cData @ %d\n",__LINE__);
        return FALSE;
    }
    return ret;
}

BOOL  YWPANEL_FP_GetVersion(YWPANEL_Version_t *version)
{
    YWPANEL_FPData_t   data;

    memset(&data, 0, sizeof(YWPANEL_FPData_t));
    data.dataType = YWPANEL_DATATYPE_GETVERSION;

    if(YWPANEL_FP_SendData(&data) != TRUE)
    {
        //printf("YWPANEL_FP_SendData not seccussfully!![%d]\n",__LINE__);
        return FALSE;
    }
    memcpy(version, &(data.data.version), sizeof(YWPANEL_Version_t));

	return TRUE;
}

int YWVFD_STANDBY_ShowTime(U8 hh,U8 mm)
{
    int 		ret = 0;
    YWPANEL_FPData_t    data;
    U8                  digitNum1,digitNum2,temp;
//show hour
    {
        memset(&data, 0, sizeof(YWPANEL_FPData_t));

        data.dataType = YWPANEL_DATATYPE_VFD;

        digitNum2 = YWPANEL_CharArray[hh/10 ];
        digitNum1 = YWPANEL_CharArray[hh%10];

        temp = digitNum2;
        digitNum2 = (digitNum2&0xbf)|(digitNum1&0x40);
        digitNum1 = (digitNum1&0x3c)|((temp&0x40)<<1)|((digitNum1&0x01)<<1)|((digitNum1&0x02)>>1);


        data.data.vfdData.type = YWPANEL_VFD_DISPLAY;
        data.data.vfdData.address[0] = VfdSegAddr[10].Segaddr2;

        data.data.vfdData.DisplayValue[0] = digitNum2;
        VfdSegAddr[10].CurrValue2 = data.data.vfdData.DisplayValue[0];

        if(YWPANEL_FP_SendData(&data) != TRUE)
        {
//            printf("YWPANEL_FP_SendData not seccussfully!![%d]\n",__LINE__);
            ret = -1;
        }

        data.data.vfdData.address[0] = VfdSegAddr[10].Segaddr1;

        data.data.vfdData.DisplayValue[0] = digitNum1;
        VfdSegAddr[10].CurrValue1= data.data.vfdData.DisplayValue[0];

        if(YWPANEL_FP_SendData(&data) != TRUE)
        {
//            printf("YWPANEL_FP_SendData not seccussfully!![%d]\n",__LINE__);
            ret = -2;
        }
    }
//show minite
    {

        memset(&data, 0, sizeof(YWPANEL_FPData_t));
        data.dataType = YWPANEL_DATATYPE_VFD;
        digitNum2 = YWPANEL_CharArray[mm/10 ];
        digitNum1 = YWPANEL_CharArray[mm%10];

        temp = digitNum2;
        digitNum2 = (digitNum2&0xbf)|(digitNum1&0x40);
        digitNum1 = (digitNum1&0x3c)|((temp&0x40)<<1)|((digitNum1&0x01)<<1)|((digitNum1&0x02)>>1);
        data.data.vfdData.type = YWPANEL_VFD_DISPLAY;
        data.data.vfdData.address[0] = VfdSegAddr[9].Segaddr2;

        data.data.vfdData.DisplayValue[0] = digitNum2;
        VfdSegAddr[9].CurrValue2 = data.data.vfdData.DisplayValue[0];

        if(YWPANEL_FP_SendData(&data) != TRUE)
        {
//            printf("YWPANEL_FP_SendData not seccussfully!![%d]\n",__LINE__);
            ret = -3;
        }

        data.data.vfdData.address[0] = VfdSegAddr[9].Segaddr1;

        data.data.vfdData.DisplayValue[0] = digitNum1;
        VfdSegAddr[9].CurrValue1= data.data.vfdData.DisplayValue[0];

        if(YWPANEL_FP_SendData(&data) != TRUE)
        {
//            printf("YWPANEL_FP_SendData not seccussfully!![%d]\n",__LINE__);
            ret = -4;
        }
    }
    return ret;
}
int YWVFD_STANDBY_SetBrightness(int level)
{
    int 		ret = 0;
    YWPANEL_FPData_t    data;

    if(level <=0)
        level = 0;
    else if(level >= 7)
        level = 7;
    data.dataType = YWPANEL_DATATYPE_VFD;
    data.data.vfdData.type = YWPANEL_VFD_SETTING;
    data.data.vfdData.setValue = level+0x88;
    if(YWPANEL_FP_SendData(&data) != TRUE)
    {
//        printf("SetBrightness wrong!!\n");
        ret = -1;
    }
    return ret ;
}

BOOL  YWPANEL_FP_SetCpuStatus(YWPANEL_CPUSTATE_t state)
{
    int 		ret = 0;
    YWPANEL_FPData_t   Data;

    memset(&Data, 0, sizeof(YWPANEL_FPData_t));
    Data.dataType = YWPANEL_DATATYPE_SETCPUSTATE;
    Data.data.CpuState.state = state;

    if(YWPANEL_FP_SendData(&Data) != TRUE)
    {
//        printf("SetBrightness wrong!!\n");
        ret = -1;
    }

    return ret ;
}

static BOOL	bTimeMode = TRUE;
static char strDvfd[16][5];

void YWVFDi_DVFDCleanChar(U8 i)
{
	U8	j;

	if (i >= 16)
	{
	    return;
	}
	if (bTimeMode)
	{
		if (i >= 10)
		{
		    return;
		}
	}

	for (j = 0; j < 5; j++)
	{
		if (bTimeMode)
		{
			strDvfd[i + 6][j] = 0;
		}
		else
		{
			strDvfd[i][j] = 0;
		}
	}
}

void YWVFDi_DVFDFillAsciiChar(U8 i, int iChar)
{
	U8	j;

	if (i >= 16)
	{
	    return;
	}

	if (bTimeMode)
	{
		if (i >= 10)
		{
		    return;
		}
	}

	for (j = 0; j < 5; j++)
	{
		if (bTimeMode)
		{
			strDvfd[i + 6][j] = dvfd_bitmap[iChar][j];
		}
		else
		{
			strDvfd[i][j] = dvfd_bitmap[iChar][j];
		}
	}
}

void YWVFDi_DVFDFillChar(U8 i, U8 c)
{
	if ((c >= 32) && (c <= 126))
	{
		YWVFDi_DVFDFillAsciiChar(i, c - 32);
	}
	else
	{
		YWVFDi_DVFDCleanChar(i);
	}
}

int YWVFDi_DVFDFillString(char* str, U8 lenth)
{
	U8 i;

	for(i = 0; i < 16; i++)
	{
	    if(i < lenth)
	    {
			YWVFDi_DVFDFillChar(i, str[i]);
	    }
	    else
	    {
			YWVFDi_DVFDCleanChar(i);
		}
	}

	return 0;
}

int YWVFDi_DVFDDisplaySync(void)

{
	int	ret = 0 ;
	YWPANEL_FPData_t    data;

    memset(&data, 0, sizeof(YWPANEL_FPData_t));

	data.dataType = YWPANEL_DATATYPE_DVFD;
	data.data.dvfdData.type = YWPANEL_DVFD_DISPLAYSYNC;

    if(YWPANEL_FP_SendData(&data) != TRUE)
    {
        //printf("VFD show stings is wrong!!\n");
        ret = -2;
    }

	return ret;
}

int YWVFDi_DVFDSendString(void)
{
	int	ret = 0 ;
	U8	i, j;
	YWPANEL_FPData_t    data;

    memset(&data, 0, sizeof(YWPANEL_FPData_t));

	data.dataType = YWPANEL_DATATYPE_DVFD;
	data.data.dvfdData.type = YWPANEL_DVFD_DISPLAYSTRING;
	for(i = 0; i < 4; i++)
	{
		data.data.dvfdData.ulen = 4;
		for (j = 0; j < 4; j++)
		{
			U8 address = i * 4 + j;
			data.data.dvfdData.address[j] = address;
			data.data.dvfdData.DisplayValue[j][0] = strDvfd[address][0];
			data.data.dvfdData.DisplayValue[j][1] = strDvfd[address][1];
			data.data.dvfdData.DisplayValue[j][2] = strDvfd[address][2];
			data.data.dvfdData.DisplayValue[j][3] = strDvfd[address][3];
			data.data.dvfdData.DisplayValue[j][4] = strDvfd[address][4];
		}
		YWVFD_Debug("%s:%d\n", __FUNCTION__, __LINE__);
	    if(YWPANEL_FP_SendData(&data) != TRUE)
	    {
	        printf("VFD show stings is wrong!!\n");
	        ret = -2;
	    }
	}

	return ret;
}

int YWVFDi_DVFDDisplayString(void)
{
	int	ret = 0 ;
	ret = YWVFDi_DVFDSendString();

	YWVFD_Debug("%s:%d\n", __FUNCTION__, __LINE__);

	ret = YWVFDi_DVFDDisplaySync();

	return ret;
}

int YWVFD_STANDBY_DvfdShowString(char* str)
{
	int ret = 0 ;
	U8 lenth;

	lenth = strlen(str);
	if(lenth > 16)
	{
	    ret = -1 ;
		printf("Bad params\n");
	    return ret;
	}

	YWVFD_Debug("%s:%d\n", __FUNCTION__, __LINE__);

	YWVFDi_DVFDFillString(str, lenth);

	YWVFD_Debug("%s:%d\n", __FUNCTION__, __LINE__);

	ret = YWVFDi_DVFDDisplayString();

	return ret;
 }

BOOL YWPANEL_FP_DvfdSetTimeMode(BOOL on)
{
    YWPANEL_FPData_t   Data;

    memset(&Data, 0, sizeof(YWPANEL_FPData_t));
    Data.dataType = YWPANEL_DATATYPE_DVFD;
    Data.data.dvfdData.type = YWPANEL_DVFD_SETTIMEMODE;
    Data.data.dvfdData.setValue = on;

    if(YWPANEL_FP_SendData(&Data) != TRUE)
    {
        return FALSE;
    }

	bTimeMode = on;
    return TRUE;
}

BOOL YWPANEL_FP_DvfdGetTimeMode(BOOL *pOn)
{
    YWPANEL_FPData_t   Data;

    memset(&Data, 0, sizeof(YWPANEL_FPData_t));
    Data.dataType = YWPANEL_DATATYPE_DVFD;
    Data.data.dvfdData.type = YWPANEL_DVFD_GETTIMEMODE;

    if(YWPANEL_FP_SendData(&Data) != TRUE)
    {
        return FALSE;
    }

	(*pOn) = Data.data.dvfdData.setValue;
	bTimeMode = Data.data.dvfdData.setValue;

    return TRUE;
}

int YWVFD_STANDBY_ShowString(char* str)
{
    int ret = 0 ;
	U8 lenth;
 	U8 i,c;

    YWPANEL_FPData_t    data;

 	lenth = strlen(str);
	if(lenth > 8)
	{
        ret = -1 ;
		//printf("Bad params\n");
        return ret;
    }
    data.dataType = YWPANEL_DATATYPE_VFD;
 	for(i = 0; i < 8; i++)
	{
        data.data.vfdData.type = YWPANEL_VFD_DISPLAYSTRING;
        if(i <lenth)
        {
            c = str[i] ;

            if(c >= 65 && c <= 95)
                c = c - 65;
            else if(c >= 97 && c <= 122)
                c = c - 97;
            else if(c >= 42 && c <= 57)
                c = c - 11;
            else
            {
                c = 47;         /*遇到不能显示的显示空*/
            }
            VfdSegAddr[i+1].CurrValue1 = CharLib[c][0] ;
            VfdSegAddr[i+1].CurrValue2 = CharLib[c][1] ;
        }
        else
        {
            VfdSegAddr[i+1].CurrValue1 = 0;
            VfdSegAddr[i+1].CurrValue2 = 0;
        }
        data.data.vfdData.address[2*i] = VfdSegAddr[i+1].Segaddr1;
        data.data.vfdData.DisplayValue[2*i] = VfdSegAddr[i+1].CurrValue1;
        data.data.vfdData.address[2*i+1] = VfdSegAddr[i+1].Segaddr2;
        data.data.vfdData.DisplayValue[2*i+1] = VfdSegAddr[i+1].CurrValue2;
	}

    if(YWPANEL_FP_SendData(&data) != TRUE)
    {
        //printf("VFD show stings is wrong!!\n");
        ret = -2;
    }
    return ret;
 }

//d48zm add
void YWPANEL_LEDSetString(const char *LEDStrBuf)
{
	U32             i;
    U8              tempData;
	char 			c;

	U16 StrLen = strlen(LEDStrBuf);
	for(i=0; i<YWPANEL_MAX_LED_LEGNTH; i++)
    {
    	c = LEDStrBuf[i];

		if(c>='0' && c<='9')
		{
			tempData = YWPANEL_LedCharArray[(LEDStrBuf[i]-'0')];
		}
		else if(c>='A' && c<='Z')
		{
			tempData = YWPANEL_LedCharArray[(c-'A'+YWPANEL_UPPER_START)];
		}
		else if(c>='a' && c<='z')
        {
			tempData = YWPANEL_LedCharArray[(c-'a'+YWPANEL_LOWER_START)];
		}
		else
        {
            tempData = 0; /*无法显示的字符用空位代替*/
        }

        if(i<StrLen)
        {
            YWPANEL_LedDisplayData[i] = tempData;
        }
        else
        {
            YWPANEL_LedDisplayData[i] = 0;
        }
    }
}

int YWPANEL_LEDDisplayString(void)
{
	int ret = 0;
	YWPANEL_FPData_t    data;

	data.dataType = YWPANEL_DATATYPE_LED;

    data.data.ledData.led1 = YWPANEL_LedDisplayData[0];
    data.data.ledData.led2 = YWPANEL_LedDisplayData[1];
    data.data.ledData.led3 = YWPANEL_LedDisplayData[2];
    data.data.ledData.led4 = YWPANEL_LedDisplayData[3];

    if(YWPANEL_FP_SendData(&data)!= TRUE)
    {
		ret = -1;
        //printf("[ERROR][YWPANEL_LEDDisplayString] TIME OUT\n");
    }

	return ret;
}

BOOL YWPANEL_FP_ControlTimer(BOOL on)
{
    YWPANEL_FPData_t   Data;

    memset(&Data, 0, sizeof(YWPANEL_FPData_t));
    Data.dataType = YWPANEL_DATATYPE_CONTROLTIMER;
    Data.data.TimeState.startFlag = on;

    if(YWPANEL_FP_SendData(&Data)!= TRUE)
    {
        return FALSE;
    }

    return TRUE;

}

int YWVFD_LED_ShowString(const char *str)
{
	YWPANEL_FP_ControlTimer(FALSE);	//防止时间刷掉显示的字符
	YWPANEL_LEDSetString(str);
	return YWPANEL_LEDDisplayString();
}
//add end
int YWPANEL_LBD_SetStatus(YWPANEL_LBDStatus_T  LBDStatus )
{
    U8 	lbdValue = 0;
    YWPANEL_FPData_t   data;
    int ret = 0;

    memset(&data, 0, sizeof(YWPANEL_FPData_t));
    data.dataType = YWPANEL_DATATYPE_LBD;
    if(LBDStatus == YWPANEL_LBD_STATUS_ON)
    {
        lbdValue |= YWPANEL_LBD_TYPE_POWER;
    }
    else
    {
        lbdValue &= ~(YWPANEL_LBD_TYPE_POWER);
    }

    data.data.lbdData.value = lbdValue;

    if(YWPANEL_FP_SendData(&data) != TRUE)
    {
		ret = -1;
    }

    return ret;
}

//d48zm 2010/1/21 add
int YWVFD_LBD_PowerOff(void)
{
	return YWPANEL_LBD_SetStatus(YWPANEL_LBD_STATUS_OFF);
}
//add end

int YWVFD_STANDBY_ShowIco(LogNum_T log_num,int log_stat)
{
    int ret = 0 ;
    int dig_num = 0,seg_num = 0;
    SegNum_T seg_part = 0;
    U8  seg_offset = 0;
    YWPANEL_FPData_t    	data;

    data.dataType = YWPANEL_DATATYPE_VFD;

    //--------------------参数过滤---------------------
    if((log_num >= LogNum_Max)||(log_num < PLAY_FASTBACKWARD))
    {
        ret = -1 ;
//        printf("Bad params\n");
        return ret;
    }
    dig_num = log_num/15;
    seg_num = log_num%15;
    seg_part = seg_num/9;

    data.data.vfdData.type = YWPANEL_VFD_DISPLAY;

    if(seg_part == SEGNUM1)
	{
        seg_offset = 0x01 << ((seg_num%9) - 1);
        data.data.vfdData.address[0] = VfdSegAddr[dig_num].Segaddr1;
        if(log_stat == LOG_ON)
        {
           VfdSegAddr[dig_num].CurrValue1 |= seg_offset;
        }
        if(log_stat == LOG_OFF)
        {
           VfdSegAddr[dig_num].CurrValue1 &= (0xFF-seg_offset);
        }
        data.data.vfdData.DisplayValue[0] = VfdSegAddr[dig_num].CurrValue1 ;
	}
    else if(seg_part == SEGNUM2)
    {
        seg_offset = 0x01 << ((seg_num%8) - 1);
        data.data.vfdData.address[0] = VfdSegAddr[dig_num].Segaddr2;
        if(log_stat == LOG_ON)
        {
           VfdSegAddr[dig_num].CurrValue2 |= seg_offset;
        }
        if(log_stat == LOG_OFF)
        {
           VfdSegAddr[dig_num].CurrValue2 &= (0xFF-seg_offset);
        }
        data.data.vfdData.DisplayValue[0] = VfdSegAddr[dig_num].CurrValue2 ;
    }
   if(YWPANEL_FP_SendData(&data) != TRUE)
   {
//        printf("Show a Ico wrong!!\n");
        ret = -2;
   }
    return ret ;
}

int YWVFD_STANDBY_GetScanKey(int *key)
{
    YWPANEL_FPData_t    data;
    int ret = 0;

	memset(&data, 0, sizeof(data));
	*key = 0;

	//2009/12/22 d48zm add
    switch(panel_disp_type)
	{
	case YWPANEL_FP_DISPTYPE_VFD:
		data.dataType = YWPANEL_DATATYPE_VFD;
		data.data.vfdData.type = YWPANEL_VFD_READKEY;
		break;
	case YWPANEL_FP_DISPTYPE_LCD:
		break;
	case YWPANEL_FP_DISPTYPE_DVFD:
		data.dataType = YWPANEL_DATATYPE_SCANKEY;
		break;
	case YWPANEL_FP_DISPTYPE_LED:
		data.dataType = YWPANEL_DATATYPE_SCANKEY;
 		break;
	case YWPANEL_FP_DISPTYPE_LBD:
		break;
	default:
		break;
	}

    if(YWPANEL_FP_SendData(&data) != TRUE)
    {
        ret = -1;
    }

	switch(panel_disp_type)
	{
	case YWPANEL_FP_DISPTYPE_VFD:
		*key = data.data.vfdData.key;
		break;
	case YWPANEL_FP_DISPTYPE_LCD:
		break;
	case YWPANEL_FP_DISPTYPE_DVFD:
	    *key = data.data.ScanKeyData.keyValue;
	case YWPANEL_FP_DISPTYPE_LED:
	    *key = data.data.ScanKeyData.keyValue;
 		break;
	case YWPANEL_FP_DISPTYPE_LBD:
		break;
	default:
		break;
	}

	if(*key == 0xff)	//防止面板坏掉时*key值为0xff
	{
		ret = -1;
	}
	//add end

    return ret;
}
/*****************************Common Functions*****************************************/
/*****************************start with YWVFD*****************************************/
void YWVFD_ShowIco(LogNum_T log_num,int log_stat)
{
    if(YWVFD_STANDBY_TYPE_VFD){
         YWVFD_STANDBY_ShowIco(log_num,log_stat);
     }else{
         YWVFD_COMMON_ShowIco(log_num,log_stat);
     }
}
int YWVFD_ShowTime(U8 hh,U8 mm)
{
    if(YWVFD_STANDBY_TYPE_VFD){
         return YWVFD_STANDBY_ShowTime(hh,mm);
     }else{
         return YWVFD_COMMON_ShowTime(hh,mm);
     }
}
int YWVFD_SetBrightness(int level)
{
    if(YWVFD_STANDBY_TYPE_VFD){
        return YWVFD_STANDBY_SetBrightness(level);
     }else{
        return YWVFD_COMMON_SetBrightness(level);
     }
}

int YWVFD_ShowString(char* str)		//d48zm modify
{
	int ret = 0;

    if(YWVFD_STANDBY_TYPE_VFD)
	{
		switch(panel_disp_type)
		{
			case YWPANEL_FP_DISPTYPE_VFD:
				ret = YWVFD_STANDBY_ShowString(str);
				break;
			case YWPANEL_FP_DISPTYPE_DVFD:
				ret = YWVFD_STANDBY_DvfdShowString(str);
				break;
			case YWPANEL_FP_DISPTYPE_LED:
				ret = YWVFD_LED_ShowString(str);
				break;

			default:
				ret = -1;
				break;
		}
    }
	else
    {
        ret = YWVFD_COMMON_ShowString(str);
    }

	return ret;
}

int YWVFD_GetScanKey(int *key)
{
    if(YWVFD_STANDBY_TYPE_VFD){
        return YWVFD_STANDBY_GetScanKey(key);
    }else{
        return YWVFD_COMMON_GetScanKey(key);
    }
}

void YWVFD_Warn(char* str)
{
    YWVFD_ShowIco(ALERT,LOG_ON);
    YWVFD_ShowString(str);
    udelay(5000);
    YWVFD_ShowIco(ALERT,LOG_OFF);
    udelay(5000);
    YWVFD_ShowIco(ALERT,LOG_ON);
}

void YWVFD_Print(char* str)
{
    YWVFD_ShowString(str);
}

int do_YWVFD_Print(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *cmd;

	if (argc < 2)
		goto usage;

	cmd = argv[1];

    YWVFD_ShowString(cmd);

	return 0;

usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

U_BOOT_CMD(
	vfdprint,	2,	1,	do_YWVFD_Print,
	"vfdprint- printf string to vfd\n",
	"[string]\n"
);

int do_YWVFD_Version(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	YWPANEL_Version_t version;

    YWPANEL_FP_GetVersion(&version);

    printf("version Cpu type =%x\n",version.CpuType);
    printf("version DisplayInfo =%x\n",version.DisplayInfo);
    printf("version scankeyNum =%x\n",version.scankeyNum);
    printf("version swMajorVersion =%x\n",version.swMajorVersion);
    printf("version swSubVersion =%x\n",version.swSubVersion);

	return 0;
}

U_BOOT_CMD(
	vfdversion,	2,	1,	do_YWVFD_Version,
	"vfdversion- printf vfd version\n",
	"\n"
);


int do_YWVFD_Standby(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    YWPANEL_FP_SetCpuStatus(YWPANEL_CPUSTATE_STANDBY);

	return 0;
}

U_BOOT_CMD(
	vfdstandby,	2,	1,	do_YWVFD_Standby,
	"vfdstandby- vfd standby\n",
	"\n"
);

int do_YWVFD(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *cmd, *s;

	/* at least two arguments please */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strcmp(cmd, "version") == 0)
	{
		do_YWVFD_Version(cmdtp, flag, argc, argv);
	}
	else if (strcmp(cmd, "print") == 0)
	{
		s = argv[2];

	    YWVFD_ShowString(s);
	}
	else if (strcmp(cmd, "lbd") == 0)
	{
		s = argv[2];

		if (strcmp(s, "on") == 0)
		{
			YWPANEL_LBD_SetStatus(YWPANEL_LBD_STATUS_ON);
		}
		else
		{
			YWPANEL_LBD_SetStatus(YWPANEL_LBD_STATUS_OFF);
		}
	}
	else if (strcmp(cmd, "getkey") == 0)
	{
		int Key;

	    YWVFD_WaitNewKeyPress(&Key);
		printf("Key = 0x%x\n", Key);
	}
	else if (strcmp(cmd, "settimemode") == 0)
	{
		s = argv[2];

		if (strcmp(s, "on") == 0)
		{
			YWPANEL_FP_DvfdSetTimeMode(TRUE);
		}
		else
		{
			YWPANEL_FP_DvfdSetTimeMode(FALSE);
		}
	}
	else if (strcmp(cmd, "gettimemode") == 0)
	{
		BOOL bOn;
		YWPANEL_FP_DvfdGetTimeMode(&bOn);
		printf("bOn = %d\n", bOn);
	}
	else
	{
		goto usage;
	}
	return 0;

usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}


U_BOOT_CMD(ywvfd, 5, 1, do_YWVFD,
	"ywvfd    - ywvfd sub-system\n",
	"ywvfd version			- show version\n"
	"ywvfd print string		- show string\n"
	"ywvfd getkey			- get key\n"
	"ywvfd lbd on			- set lbd\n");

/*************************************************************
    函数名称:	VFD_BOOT_P
    函数说明:	显示升级ＵＢＯＯＴ的相应进度
    算法说明:	无
    调用说明:   Calls                         	Called By
               ------                         ------------
                AAAAA()                        BBBBB()
    输入参数:
    输出参数:
    返 回 值:
    修改记录:	日       期      作      者     修定
                ---------        ---------      -----
                2008.12.03        D02SH         新建
*************************************************************/
void YWVFD_BOOT_P(int pro_rate)
{
    char str[16];
    sprintf(str, "%s%d",show_p_boot,pro_rate);
    YWVFD_ShowString(str);
}

void YWVFD_KERNEL_P(int pro_rate)
{
    char str[16];
    sprintf(str, "%s%d",show_p_kern,pro_rate);
    YWVFD_ShowString(str);
}

void YWVFD_ROOTFS_P(int pro_rate)
{
    char str[16];
    sprintf(str, "%s%d",show_p_root,pro_rate);
    YWVFD_ShowString(str);
}

void YWVFD_U_MENU(void)
{
   YWVFD_ShowString("UP-MENU");
}

void YWVFD_U_BOOT(void)
{
    YWVFD_ShowString("UP-BOOT");
}
void YWVFD_U_Kernel(void)
{
    YWVFD_ShowString("UP-KERNEL");
}
/**************************************Check IT AUTO*******************/

char *YWVFD_DispTypeString(YWPANEL_FP_DispType_t panel_disp_type)
{
	switch (panel_disp_type)
	{
	    case YWPANEL_FP_DISPTYPE_VFD:
	        return "VFD";
	        break;
	    case YWPANEL_FP_DISPTYPE_LCD:
	        return "LCD";
	        break;
	    case YWPANEL_FP_DISPTYPE_DVFD:
	        return "DVFD";
	        break;
	    case YWPANEL_FP_DISPTYPE_LED:
	        return "LED";
	        break;
	    case YWPANEL_FP_DISPTYPE_LBD:
	        return "LBD";
	        break;
	    default:
	        return "UNKNOWN";
	        break;
	}
}

void YWVFD_Init(void)
{
    YWPANEL_Version_t panel_version;
	int try_num = 0;
	int try_ret = -1;
	const int TRY_MAX_NUM = 50;

    //初始化地址值
    SegAddrInit();

    //for i2c as soft
    #if 0
    if (I2C_DA_PORT == 3)
    {
        SET_PIO_PIN(PIO_PORT(3), I2C_CL_PIN, STPIO_OUT);
        SET_PIO_PIN(PIO_PORT(3), I2C_DA_PIN, STPIO_OUT);
    }else if(I2C_DA_PORT == 4)
    {
        SET_PIO_PIN(PIO_PORT(4), I2C_CL_PIN, STPIO_OUT);
        SET_PIO_PIN(PIO_PORT(4), I2C_DA_PIN, STPIO_OUT);
    }else if(I2C_DA_PORT == 2)
    {
        SET_PIO_PIN(PIO_PORT(2), I2C_CL_PIN, STPIO_OUT);
        SET_PIO_PIN(PIO_PORT(2), I2C_DA_PIN, STPIO_OUT);
    }
    else
    {
//      #error "Need tell me I2C_DA_PORT"
        printf("Need tell me I2C_DA_PORT \n");
    }
    #endif  /* 0 */
    i2c_init(1000000,0x50); //100K

{
	BOOL				bVersion = FALSE;
	int					iVersionNum = 0;
	int					iTryNum = 100;

	while ((!bVersion) && iVersionNum < iTryNum) //i2c工作可能不正常,尝试多次
	{
		bVersion = YWPANEL_FP_GetVersion(&panel_version);
		iVersionNum++;
	}
	if(bVersion)
	{
		panel_disp_type = panel_version.DisplayInfo;

		printf("Panel: %s\n", YWVFD_DispTypeString(panel_disp_type));

		//注意:YW-H011-CS+T板子panel_disp_type值会为0x0f
		if(panel_disp_type < YWPANEL_FP_DISPTYPE_UNKNOWN ||
			panel_disp_type > YWPANEL_FP_DISPTYPE_LBD)
		{
			panel_disp_type = YWPANEL_FP_DISPTYPE_VFD;
		}
		if (YWPANEL_FP_DISPTYPE_DVFD == panel_disp_type)
		{
			BOOL bOn;
			YWVFD_STANDBY_TYPE_VFD = TRUE;
			YWPANEL_FP_DvfdGetTimeMode(&bOn);
		    YWVFD_ShowString("INIT");
			YWVFD_Debug("INIT\n");
			YWVFD_Print("boot");
			YWVFD_Debug("boot\n");
			return;
		}
	}
}

	//d48zm 2010/04/20 add begin
	while (try_ret && try_num<TRY_MAX_NUM) //i2c工作可能不正常,尝试多次
	{
		try_ret = YWVFD_STANDBY_ShowString((char*)"INIT");
		++try_num;
	}
	//d48zm 2010/04/20 add end
    if(!try_ret)//i2c seccuss is standby d48zm 2010/04/20 modify
    {
       	YWVFD_STANDBY_TYPE_VFD = TRUE;

		//2009/12/22 d48zm add
		if(YWPANEL_FP_GetVersion(&panel_version))
		{
			panel_disp_type = panel_version.DisplayInfo;

			//注意:YW-H011-CS+T板子panel_disp_type值会为0x0f
			if(panel_disp_type<YWPANEL_FP_DISPTYPE_UNKNOWN || panel_disp_type>YWPANEL_FP_DISPTYPE_LBD)
			{
				panel_disp_type = YWPANEL_FP_DISPTYPE_VFD;
			}
		}
	    //add end
    }
	else
    {
		YWVFD_STANDBY_TYPE_VFD = FALSE;
		#if !defined(CONFIG_CMD_NAND )
        SET_PIO_PIN(PIO_PORT(3), 2, STPIO_OUT);
        SET_PIO_PIN(PIO_PORT(3), 4, STPIO_OUT);//
        SET_PIO_PIN(PIO_PORT(3), 5, STPIO_OUT);
		#endif
         //设置为13dig ----15seg
        VFD_CS_CLR();
        YW_VFD_WR(0x0C);
        VFD_CS_SET();
        //初始化为写模式
        VFD_CS_CLR();
        YW_VFD_WR(0x44);
        VFD_CS_SET();
         //清空显示缓存
        VFD_ClearAll();
        ShowContent();
    }

    YWVFD_Print("boot");
}

BOOL YWVFD_IsStandbyTypeVfd(void)
{
	return YWVFD_STANDBY_TYPE_VFD;
}

BOOL YWVFDi_HasKeyPress(int *pKey)
{
    if (0 == YWVFD_GetScanKey(pKey))
    {
        if ((*pKey) != 0x00)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL YWVFD_HasKeyPress(int *pKey)
{
    int key = 0x00;
    if (YWVFDi_HasKeyPress(&key))
    {
        if (pKey)
        {
            *pKey = key;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL YWVFD_HasKeyPressJudge(void)
{
    int key = 0x00;
    return YWVFDi_HasKeyPress(&key);
}

void YWVFD_WaitKeyRelease(void)
{
    while (YWVFD_HasKeyPressJudge());
}

void YWVFD_WaitKeyPress(int *pKey)
{
    do
    {
        if (YWVFD_HasKeyPress(pKey))
        {
            break;
        }
    } while (1);
}

BOOL YWVFD_WaitNewKeyPress(int *pKey)
{
    YWVFD_WaitKeyRelease();
    YWVFD_WaitKeyPress(pKey);
    return TRUE;
}

BOOL YWVFD_GetScanKeyUntilRelease(int *pKey)
{
    BOOL    bKerPressed = FALSE;
    int     key = 0x00;
    do
    {
        if (YWVFD_HasKeyPress(&key))
        {
            bKerPressed = TRUE;
            if (pKey)
            {
                *pKey = key;
            }
        }
        else
        {
            return bKerPressed;
        }
    } while (1);
}
#endif

/* EOF------------------------------------------------------------------------*/

/* BOL-------------------------------------------------------------------*/
//$Log: vfd.c,v $
//Revision 1.6  2011/11/25 06:22:19  d26lf
//change to spi boot
//
//Revision 1.5  2011/10/31 06:02:25  d26lf
//fix nand markbad bug
//fix usb recognize bug
//
//Revision 1.4  2011/09/29 07:53:21  d26lf
//1.0.006修正soft i2c的BUG
//
//Revision 1.3  2011/07/06 01:28:24  d27lwj
//no message
//
//Revision 1.2  2011/06/20 09:54:41  d26lf
//删除行后多余空格
//
//Revision 1.1  2011/06/17 08:07:46  d27lwj
//no message
//
//Revision 1.6  2010/09/25 03:24:03  d22cj
//增加双启动功能
//
//Revision 1.5  2010/06/02 01:47:38  d48zm
//使用YWVFD_Print打印
//
//Revision 1.4  2010/06/02 01:27:06  d48zm
//扩充面板led字符显示
//
//Revision 1.3  2010/04/20 08:52:45  d48zm
//修改109版面板待机启机无法显示boot的bug
//
//Revision 1.2  2010/01/21 03:43:31  d48zm
//加入power lbd灯熄灭功能
//
//Revision 1.1.1.1  2010/01/20 05:03:00  d22cj
//1、U-Boot首次导入
//2、修改启动参数
//3、修改进入菜单的方法：只能用ESC或者键盘进入
//
/* EOL-------------------------------------------------------------------*/

