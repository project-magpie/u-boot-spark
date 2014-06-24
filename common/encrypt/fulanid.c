
/**********************************文件头部注释************************************/
//
//
//  					Copyright (C), 2005-2010, AV Frontier Tech. Co., Ltd.
//
//
// 文件名：		fulanid.c
//
// 创建者：		dhy
//
// 创建时间：	2008.06.16
//
// 文件描述：
//
// 修改记录：   日       期      作      者       版本      修定
//				       ---------         ---------        -----        -----
//
/*****************************************************************************************/

/************************************文件包含****************************************/

#include <common.h>

#include <linux/types.h>
#include <linux/string.h>

/************************************宏定义*******************************************/

//#define LICENSE_DEBUG(x)					printf x
//#define LICENSE_ERROR(x)         			printf x
#define LICENSE_DEBUG(x)
#define LICENSE_ERROR(x)



/************************************常量定义****************************************/

#define  STB_SN_STRING_LEN						100
#define  STB_MAC_LEN							6

#define  STB_CHIP_ID_OFFSET						0
#define  STB_HW_VER_OFFSET    					1
#define  STB_CUSTOMER_ID_OFFSET    				2
#define  STB_SN_OFFSET    						4

enum
{
	CHIP_STI5119 = 0x00,
	CHIP_STI5105 = 0x01,
	CHIP_STI5100 = 0x02,
	CHIP_STI7101 = 0x03,
	CHIP_ALI3329E = 0x04,
	CHIP_ALI3329C = 0x05,
	CHIP_ALI3602 = 0x06
};


enum
{
	MAC_OUI_STI5119 = 0x00000B,
	MAC_OUI_STI5105 = 0x00000C,
	MAC_OUI_STI5100 = 0x00000E,
	MAC_OUI_STI7101 = 0x00000F,
	MAC_OUI_ALI3329E = 0x00010,
	MAC_OUI_ALI3329C = 0x00020,
	MAC_OUI_ALI3602 = 0x00040
};


enum
{
	CUSTOMER_TEST = 0x00,
	CUSTOMER_YUMATU = 0x01,
	CUSTOMER_GLOBO = 0x02,
	CUSTOMER_TRUMAN = 0x03,
	CUSTOMER_SPIDER = 0x04,
	CUSTOMER_STARTRACK = 0x05,
	CUSTOMER_NEOSAT = 0x06,
	CUSTOMER_GOLDEN = 0x07,
	CUSTOMER_EDISION = 0x08,
	CUSTOMER_ONLIEN = 0x09
};



/***********************************结构体定义***************************************/

typedef struct
{
	unsigned char			chip_id;
	char					*chip_name;
	unsigned int			mac_oui;

} MODE_CHIP_ID;


typedef struct
{
	unsigned char			hw_ver;
	char 					*hw_name;
	char					*hw_detail;

} MODE_HW_VER;


typedef struct
{
	unsigned char  		customer_id;
	char 					*customer_name;

} MODE_CUSTOMER_ID;


typedef struct
{
	unsigned int			mode_id;
	unsigned int			mac_offset;
	unsigned int			stb_num;

} MAC_OFFSET_TABLE;



/************************************变量定义****************************************/
int     			g_StbChipIdNum;
MODE_CHIP_ID		g_StbChipIdTable[] =
{
	{CHIP_STI5119, "STi5119", MAC_OUI_STI5119},
	{CHIP_STI5105, "STi5105", MAC_OUI_STI5105},
	{CHIP_STI5100, "STi5100", MAC_OUI_STI5100},
	{CHIP_STI7101, "STi7101", MAC_OUI_STI7101},
	{CHIP_ALI3329E, "ALi3329E", MAC_OUI_ALI3329E},
	{CHIP_ALI3329C, "ALi3329C", MAC_OUI_ALI3329C},
	{CHIP_ALI3602, "ALi3602", MAC_OUI_ALI3602}
};


int     		g_StbSTi5119HwNum = 0;
MODE_HW_VER		g_StbSTi5119HwTable[] =
{
	{0x00, "VA", "1 DVB-S, 1 CARD, 1 NET"}
};


int     		g_StbSTi5105HwNum = 0;
MODE_HW_VER		g_StbSTi5105HwTable[] =
{
	{0x00, "VA", "1 DVB-S, 1 CARD 1 NET"}
};


int     		g_StbSTi5100HwNum = 0;
MODE_HW_VER		g_StbSTi5100HwTable[] =
{
	{0x00, "YW-DVR-200-1.4", "2 DVB-S, 2 CARD, 1 NET, 1 USB, 1 SATA"},
	{0x01, "ST5100-4-PVR-VER1.0", "1 DVB-S, 1 DVB-T, 2 CARD, 1 NET, 1 USB"}
};


int     		g_StbSTi7101HwNum = 0;
MODE_HW_VER		g_StbSTi7101HwTable[] =
{
	{0x00, "YW-HD-STB-1.2", "1 DVB-S2, 2 CARD, 2 CI, 1 NET, 1 USB, 1 SATA, 1 HDMI"},
	{0x01, "YW-MOON-S+CI-VER1.0", "1 DVB-S2, 1 DVB-T, 2 CARD, 1 CI, 1 NET, 1 USB, 1 SATA, 1 HDMI"},
	{0x02, "YW-MOON-T+S+2CI-VER1.1", "1 DVB-S2, 1 DVB-T, 2 CARD, 1 CI, 1 NET, 1 USB, 1 SATA, 1 HDMI"},
	{0x03, "YW-MOON-T+S(N1)+2CI-VER1.0", "1 DVB-S2(ALi3501), 1 DVB-T, 2 CARD, 1 CI, 1 NET, 1 USB, 1 SATA, 1 HDMI"},
	{0x04, "YW-MOON-2S-VER1.0", "2 DVB-S2, 2 CARD, 1 NET, 1 USB, 1 SATA, 1 HDMI"},
	{0x05, "YW-MOON-IBOX-VER1.0", "TWIN TUNER, 2 CARD, 1 NET, 1 USB, 1 SATA, 1 HDMI"}
};


int     		g_StbALi3329EHwNum = 0;
MODE_HW_VER		g_StbALi3329EHwTable[] =
{
	{0x00, "ALI-29E-VER2.0", "1 DVB-S, 2 CARD, 1 CI,  1 USB, 1 HDMI"},
	{0x01, "ALI-PVR-VER1.1", "1 DVB-S, 2 CARD, 1 USB"},
	{0x02, "VFD-6826-VER1.2", "1 DVB-S, 2 CARD, 1 CI, 1 USB, 1 HDMI, PANEL=VFD"}
};


int     		g_StbALi3329CHwNum = 0;
MODE_HW_VER		g_StbALi3329CHwTable[] =
{
	{0x00, "ALI3329C", "1 DVB-S, 1 CARD"},
	{0x01, "ALI3329C", "1 DVB-S, 1 CARD : Tuner on board"},
	{0x02, "ALI3329C", "1 DVB-S, 1 CARD, 1 HDMI"}
};


int     		g_StbALi3602HwNum = 0;
MODE_HW_VER		g_StbALi3602HwTable[] =
{
	{0x00, "ALI-HD1-LV-VER1.0", "1 DVB-S2, 1 DVB-T, 2 CARD, 2 CI, 1 USB, 1 HDMI"},
	{0x01, "ALI-HD1-LV-VER1.1", "1 DVB-S2, 1 DVB-T, 2 CARD, 2 CI, 1 USB, 1 HDMI"},
	{0x02, "ALI-HD1-LV-VER1.2", "1 DVB-S2, 1 DVB-T, 2 CARD, 2 CI, 1 USB, 1 HDMI"},
	{0x03, "ALI-HD2-LV-VER1.0", "1 DVB-S2, 1 DVB-T, 2 CARD, 1 NET, 1 USB, 1 HDMI"}
};


int					g_StbCustomerIdNum = 0;
MODE_CUSTOMER_ID  	g_StbCustomerIdTable[] =
{
	{CUSTOMER_TEST, "TEST"},
	{CUSTOMER_YUMATU, "YUMATU"},
	{CUSTOMER_GLOBO, "GLOBO"},
	{CUSTOMER_TRUMAN, "TRUMAN"},
	{CUSTOMER_SPIDER, "SPIDER"},
	{CUSTOMER_STARTRACK, "STARTRACK"},
	{CUSTOMER_NEOSAT, "NEOSAT"},
	{CUSTOMER_GOLDEN, "GOLDENINTERSTAR"},
	{CUSTOMER_EDISION, "EDISION"},
	{CUSTOMER_ONLIEN, "ONLIEN"}
};


unsigned char    		g_StbMac[STB_MAC_LEN];
char			   		g_StbSnString[STB_SN_STRING_LEN];



MAC_OFFSET_TABLE	g_StbALI3329EMacTable[]=
{
		{0x04000000, 0x00000000, 0x00000100}, 		/* ALI3329E_ALI-29E-VER2.0_TEST				00000000 - 000000FF	(256) */
		{0x04000100, 0x00000100, 0x0001FF00}, 		/* ALI3329E_ALI-29E-VER2.0_YUMATU			00000100 - 0001FFFF	(130816) */
		{0x04000200, 0x00020000, 0x00020000}, 		/* ALI3329E_ALI-29E-VER2.0_GLOBO			00020000 - 0003FFFF	(131072) */
		{0x04000300, 0x00040000, 0x00020000}, 		/* ALI3329E_ALI-29E-VER2.0_TRUMAN			00040000 - 0005FFFF	(131072) */
		{0x04000400, 0x00060000, 0x00020000}, 		/* ALI3329E_ALI-29E-VER2.0_SPIDER			00060000 - 0007FFFF	(131072) */
		{0x04000500, 0x00080000, 0x00020000}, 		/* ALI3329E_ALI-29E-VER2.0_STARTRACK		00080000 - 0009FFFF	(131072) */

		{0x04010000, 0x000A0000, 0x00000100}, 		/* ALI3329E_ALI-PVR-VER1.1_TEST				000A0000 - 000A00FF	(256) */
		{0x04010100, 0x000A0100, 0x0001FF00}, 		/* ALI3329E_ALI-PVR-VER1.1_YUMATU			000A0100 - 000BFFFF	(130816) */
		{0x04010200, 0x000C0000, 0x00020000}, 		/* ALI3329E_ALI-PVR-VER1.1_GLOBO			000C0000 - 000DFFFF	(131072) */
		{0x04010300, 0x000E0000, 0x00020000}, 		/* ALI3329E_ALI-PVR-VER1.1_TRUMAN			000E0000 - 000FFFFF	(131072) */
		{0x04010400, 0x00100000, 0x00020000}, 		/* ALI3329E_ALI-PVR-VER1.1_SPIDER			00100000 - 0011FFFF	(131072) */
		{0x04010500, 0x00120000, 0x00020000}, 		/* ALI3329E_ALI-PVR-VER1.1_STARTRACK		00120000 - 0013FFFF	(131072) */

		{0x04000600, 0x00140000, 0x00020000}, 		/* ALI3329E_ALI-29E-VER2.0_NEOSAT			00140000 - 0015FFFF	(131072) */
		{0x04000700, 0x00160000, 0x00020000}, 		/* ALI3329E_ALI-29E-VER2.0_GOLDEN			00160000 - 0017FFFF	(131072) */

		{0x04020000, 0x00180000, 0x00000100}, 		/* VFD-6826-VER1.2_TEST						00180000 - 001800FF	(256) */
		{0x04020200, 0x00180100, 0x0001FF00}, 		/* VFD-6826-VER1.2_GLOBO					00180100 - 0019FFFF	(130816) */

		{0x04000900, 0x001A0000, 0x00020000}, 		/* ALI3329E_ALI-29E-VER2.0_ONLIEN			001A0000 - 001BFFFF	(131072) */
};


MAC_OFFSET_TABLE	g_StbALI3329CMacTable[]=
{
		{0x05000000, 0x00000000, 0x00000100}, 		/* ALI3329C_TEST							00000000 - 000000FF	(256) */
		{0x05000100, 0x00000100, 0x0001FF00}, 		/* ALI3329C_YUMATU							00000100 - 0001FFFF	(130816) */
		{0x05000200, 0x00020000, 0x00020000}, 		/* ALI3329C_GLOBO							00020000 - 0003FFFF	(131072) */
		{0x05000300, 0x00040000, 0x00020000}, 		/* ALI3329C_TRUMAN							00040000 - 0005FFFF	(131072) */
		{0x05000400, 0x00060000, 0x00020000}, 		/* ALI3329C_SPIDER							00060000 - 0007FFFF	(131072) */
		{0x05000500, 0x00080000, 0x00020000}, 		/* ALI3329C_STARTRACK						00080000 - 0009FFFF	(131072) */
		{0x05000600, 0x000A0000, 0x00020000}, 		/* ALI3329C_NEOSAT							000A0000 - 000BFFFF	(131072) */
		{0x05000700, 0x000C0000, 0x00020000}, 		/* ALI3329C_GOLDENINTERSTAR					000C0000 - 000DFFFF	(131072) */
		{0x05000800, 0x000E0000, 0x00020000}, 		/* ALI3329C_EDISION							000E0000 - 000FFFFF	(131072) */

		{0x05010000, 0x00100000, 0x00000100}, 		/* ALI3329C_TEST : Tuner on board			00100000 - 001000FF	(256) */
		{0x05010100, 0x00100100, 0x0001FF00}, 		/* ALI3329C_YUMATU : Tuner on board 		00100100 - 0011FFFF	(130816) */
		{0x05010200, 0x00120000, 0x00020000}, 		/* ALI3329C_GLOBO : Tuner on board			00120000 - 0013FFFF	(131072) */
		{0x05010300, 0x00140000, 0x00020000}, 		/* ALI3329C_TRUMAN : Tuner on board			00140000 - 0015FFFF	(131072) */
		{0x05010400, 0x00160000, 0x00020000}, 		/* ALI3329C_SPIDER : Tuner on board			00160000 - 0017FFFF	(131072) */
		{0x05010500, 0x00180000, 0x00020000}, 		/* ALI3329C_STARTRACK : Tuner on board		00180000 - 0019FFFF	(131072) */

		{0x05020200, 0x00200000, 0x00020000}, 		/* ALI3329C_GLOBO : HDMI					00200000 - 0021FFFF	(131072) */
};


MAC_OFFSET_TABLE	g_StbALI3602MacTable[]=
{
		{0x06000000, 0x00000000, 0x00000100}, 		/* ALI3602_TEST								00000000 - 000000FF	(256) */
		{0x06000200, 0x00000100, 0x0001FF00}, 		/* ALI3602_GLOBO							00000100 - 0001FFFF	(65280) */
		{0x06000100, 0x00020000, 0x00010000}, 		/* ALI3602_YUMATU							00020000 - 0002FFFF	(65536) */
		{0x06000300, 0x00030000, 0x00010000}, 		/* ALI3602_TRUMAN							00030000 - 0003FFFF	(65536) */
		{0x06000500, 0x00040000, 0x00010000}, 		/* ALI3602_STARTRACK						00040000 - 0004FFFF	(65536) */
		{0x06000600, 0x00050000, 0x00010000}, 		/* ALI3602_NEOSAT							00050000 - 0005FFFF	(65536) */
		{0x06000700, 0x00060000, 0x00010000}, 		/* ALI3602_GOLDENINTERSTAR					00060000 - 0006FFFF	(65536) */
		{0x06000800, 0x00070000, 0x00010000}, 		/* ALI3602_EDISION							00070000 - 0007FFFF	(65536) */
		{0x06000900, 0x00080000, 0x00010000}, 		/* ALI3602_ONLIEN							00080000 - 0008FFFF	(65536) */

		{0x06030000, 0x00090000, 0x00000100}, 		/* ALI3602_HD2_TEST							00090000 - 000900FF	(256) */
		{0x06030100, 0x00090100, 0x0001FF00}, 		/* ALI3602_HD2_YUMATU						00090100 - 0009FFFF	(65280) */
		{0x06030200, 0x000A0000, 0x00010000}, 		/* ALI3602_HD2_GLOBO						000A0000 - 000AFFFF	(65536) */
		{0x06030300, 0x000B0000, 0x00010000}, 		/* ALI3602_HD2_TRUMAN						000B0000 - 000BFFFF	(65536) */
		{0x06030500, 0x000C0000, 0x00010000}, 		/* ALI3602_HD2_STARTRACK					000C0000 - 000CFFFF	(65536) */
		{0x06030600, 0x000D0000, 0x00010000}, 		/* ALI3602_HD2_NEOSAT						000D0000 - 000DFFFF	(65536) */
		{0x06030700, 0x000E0000, 0x00010000}, 		/* ALI3602_HD2_GOLDENINTERSTAR				000E0000 - 000EFFFF	(65536) */
		{0x06030800, 0x000F0000, 0x00010000}, 		/* ALI3602_HD2_EDISION						000F0000 - 000FFFFF	(65536) */
		{0x06030900, 0x00100000, 0x00010000}, 		/* ALI3602_HD2_ONLIEN						00100000 - 0010FFFF	(65536) */
};


MAC_OFFSET_TABLE	g_StbSTi5100MacTable[]=
{
		{0x02000000, 0x00000000, 0x00000100}, 		/* STi5100_YW-DVR-200-1.4_TEST				00000000 - 000000FF	(256) */
		{0x02000100, 0x00000100, 0x0001FF00}, 		/* STi5100_YW-DVR-200-1.4_YUMATU			00000100 - 0001FFFF	(130816) */
		{0x02000200, 0x00020000, 0x00020000}, 		/* STi5100_YW-DVR-200-1.4_GLOBO				00020000 - 0003FFFF	(131072) */
		{0x02000300, 0x00040000, 0x00020000}, 		/* STi5100_YW-DVR-200-1.4_TRUMAN			00040000 - 0005FFFF	(131072) */
		{0x02000400, 0x00060000, 0x00020000}, 		/* STi5100_YW-DVR-200-1.4_SPIDER			00060000 - 0007FFFF	(131072) */
		{0x02000500, 0x00080000, 0x00020000}, 		/* STi5100_YW-DVR-200-1.4_STARTRACK			00080000 - 0009FFFF	(131072) */
		{0x02000600, 0x000A0000, 0x00020000}, 		/* STi5100_YW-DVR-200-1.4_NEOSAT			000A0000 - 000BFFFF	(131072) */
		{0x02000700, 0x000C0000, 0x00020000}, 		/* STi5100_YW-DVR-200-1.4_GOLDEN			000C0000 - 000DFFFF	(131072) */

		{0x02010000, 0x000E0000, 0x00000100}, 		/* ST5100-4-PVR-VER1.0_TEST					000E0000 - 000E00FF	(256) */
		{0x02010400, 0x000E0100, 0x0001FF00}, 		/* ST5100-4-PVR-VER1.0_SPIDER				000E0100 - 00FFFFFF	(130816) */
};


MAC_OFFSET_TABLE	g_StbSTi7101EMacTable[]=
{
		{0x03000000, 0x00000000, 0x00000100}, 		/* STi7101_YW-HD-STB-1.2_TEST				00000000 - 000000FF	(256) */
		{0x03000100, 0x00000100, 0x00002F00}, 		/* STi7101_YW-HD-STB-1.2_YUMATU				00000100 - 00002FFF	(12032) */
		{0x03000200, 0x00003000, 0x00003000}, 		/* STi7101_YW-HD-STB-1.2_GLOBO				00003000 - 00005FFF	(12288) */
		{0x03000300, 0x00006000, 0x00003000}, 		/* STi7101_YW-HD-STB-1.2_TRUMAN				00006000 - 00008FFF	(12288) */
		{0x03000400, 0x00009000, 0x00003000}, 		/* STi7101_YW-HD-STB-1.2_SPIDER				00009000 - 0000BFFF	(12288) */
		{0x03000500, 0x0000C000, 0x00003000}, 		/* STi7101_YW-HD-STB-1.2_STARTRACK			0000C000 - 0000EFFF	(12288) */
		{0x03000700, 0x0000F000, 0x00003000}, 		/* STi7101_YW-HD-STB-1.2_GOLDEN				0000F000 - 00011FFF	(12288) */

		{0x03020000, 0x00012000, 0x00000100}, 		/* YW-MOON-T+S+2CI-VER1.0_TEST				00012000 - 000120FF	(256) */
		{0x03020200, 0x00012100, 0x00002F00}, 		/* YW-MOON-T+S+2CI-VER1.0_GLOBO				00012100 - 00014FFF	(12032) */

		{0x03030000, 0x00015000, 0x00000100}, 		/* YW-MOON-T+S(N1)+2CI-VER1.0_TEST			00015000 - 000150FF	(256) */
		{0x03030200, 0x00015100, 0x00002F00}, 		/* YW-MOON-T+S(N1)+2CI-VER1.0_GLOBO			00015100 - 00017FFF	(12032) */

		{0x03040000, 0x00018000, 0x00000100}, 		/* YW-MOON-2S-VER1.0_TEST					00018000 - 000180FF	(256) */
		{0x03040400, 0x00018100, 0x00002F00}, 		/* YW-MOON-2S-VER1.0_SPIDER					00018100 - 0001AFFF	(12032) */

		{0x03000800, 0x0001B000, 0x00003000}, 		/* STi7101_YW-HD-STB-1.4_EDISION			0001B000 - 0001DFFF	(12288) */

		{0x03020000, 0x0001E000, 0x00000100}, 		/* YW-MOON-T+S+2CI-VER1.1_TEST				0001E000 - 0001E0FF	(256) */
		{0x03020900, 0x0001E100, 0x00000F00}, 		/* YW-MOON-T+S+2CI-VER1.1_ONLIEN			0001E100 - 0001EFFF	(3840) */

		{0x03000200, 0x0001F000, 0x00010000}, 		/* STi7101_YW-HD-STB-1.2_GLOBO				0001F000 - 0002EFFF	(65536) */

		{0x03050000, 0x0002F000, 0x00000100}, 		/* YW-MOON-IBOX-VER1.0_TEST				0002F000 - 0002F0FF	(256) */

		{0x03020700, 0x0002F100, 0x00002F00}, 		/* YW-MOON-T+S+2CI-VER1.1_Golden			0002F100 - 00031FFF	(12032) */

		{0x03050800, 0x00032000, 0x00003000}, 		/* YW-MOON-IBOX-VER1.0_EDISION				00032000 - 00034FFF	(12288) */
};


/************************************变量引用****************************************/



/************************************内部函数****************************************/

char *  GetChipName( unsigned char  chip_id )
{
	int i;


	for(i = 0 ; i< g_StbChipIdNum ; i++)
	{
		if( g_StbChipIdTable[i].chip_id == chip_id )
		{
			break;
		}
	}

	if( i < g_StbChipIdNum )
	{
		return g_StbChipIdTable[i].chip_name;
	}
	else
	{
		LICENSE_ERROR(("GetChipName => [%02x] not valid chip id \n", chip_id));
		return NULL;
	}
}


char *  GetHwName( unsigned char  chip_id , unsigned char hw_ver)
{
	int 				i;
	int				error = 0;
	int				temp_hw_num = 0 ;
	MODE_HW_VER	*temp_hw_point;


	switch ( chip_id  )
	{
		case CHIP_STI5119 :

			temp_hw_num = g_StbSTi5119HwNum ;
			temp_hw_point = g_StbSTi5119HwTable;
			break;

		case CHIP_STI5105 :

			temp_hw_num = g_StbSTi5105HwNum ;
			temp_hw_point = g_StbSTi5105HwTable;
			break;

		case CHIP_STI5100 :

			temp_hw_num = g_StbSTi5100HwNum ;
			temp_hw_point = g_StbSTi5100HwTable;
			break;

		case CHIP_STI7101 :

			temp_hw_num = g_StbSTi7101HwNum ;
			temp_hw_point = g_StbSTi7101HwTable;
			break;

		case CHIP_ALI3329E :

			temp_hw_num = g_StbALi3329EHwNum ;
			temp_hw_point = g_StbALi3329EHwTable;
			break;

		case CHIP_ALI3329C :

			temp_hw_num = g_StbALi3329CHwNum ;
			temp_hw_point = g_StbALi3329CHwTable;
			break;

		case CHIP_ALI3602 :

			temp_hw_num = g_StbALi3602HwNum ;
			temp_hw_point = g_StbALi3602HwTable;
			break;

		default :

			LICENSE_ERROR(("Get HwName => no hw table for  chip id  [%02x]  \n" , hw_ver  ));
			error = 1;
			break;
	}

	if( error == 0 )
	{
		for( i = 0 ;  i < temp_hw_num ; i ++ )
		{
			if( temp_hw_point[i].hw_ver == hw_ver )
				break;
		}

		if( i < temp_hw_num )
		{
			return temp_hw_point[i].hw_name;
		}
		else
		{
			LICENSE_ERROR(("Get HwName => not hw ver[%02x]  for chip id  [%02x]  \n"  , hw_ver , chip_id ));
		}
	}

	return NULL;

}



int  GetHwNum( unsigned char  chip_id )
{
	int   temp_hw_num = 0 ;


	g_StbSTi5119HwNum = sizeof (g_StbSTi5119HwTable) / sizeof( MODE_HW_VER );
	g_StbSTi5105HwNum = sizeof (g_StbSTi5105HwTable) /sizeof (MODE_HW_VER );
	g_StbSTi5100HwNum= sizeof (g_StbSTi5100HwTable) /sizeof (MODE_HW_VER );
	g_StbSTi7101HwNum= sizeof (g_StbSTi7101HwTable) /sizeof (MODE_HW_VER );
	g_StbALi3329EHwNum= sizeof (g_StbALi3329EHwTable) /sizeof(MODE_HW_VER );
	g_StbALi3329CHwNum= sizeof (g_StbALi3329CHwTable) /sizeof(MODE_HW_VER );
	g_StbALi3602HwNum= sizeof (g_StbALi3602HwTable) /sizeof(MODE_HW_VER );

	switch ( chip_id  )
	{
		case CHIP_STI5119 :

			temp_hw_num = g_StbSTi5119HwNum ;
			break;

		case CHIP_STI5105 :

			temp_hw_num = g_StbSTi5105HwNum ;
			break;

		case CHIP_STI5100 :

			temp_hw_num = g_StbSTi5100HwNum ;
			break;

		case CHIP_STI7101 :

			temp_hw_num = g_StbSTi7101HwNum ;
			break;

		case CHIP_ALI3329E :

			temp_hw_num = g_StbALi3329EHwNum ;
			break;

		case CHIP_ALI3329C :

			temp_hw_num = g_StbALi3329CHwNum ;
			break;

		case CHIP_ALI3602 :

			temp_hw_num = g_StbALi3602HwNum ;
			break;

		default :

			LICENSE_ERROR(("GetHwNum => no hw table for  chip id  [%02x]  \n" , chip_id  ));
			break;
	}

	return temp_hw_num;

}


char *  GetCustomerName( unsigned char  customer_id )
{
	int i;


	for(i = 0 ; i< g_StbCustomerIdNum ; i++)
	{
		if( g_StbCustomerIdTable[i].customer_id == customer_id )
		{
			break;
		}
	}

	if( i < g_StbCustomerIdNum )
	{
		return g_StbCustomerIdTable[i].customer_name;
	}
	else
	{
		LICENSE_ERROR(("GetCustomerName => no customer id[%02x].\n"  , customer_id));
		return NULL;
	}
}


unsigned int GetMacOui( unsigned char  chip_id )
{
	int i;


	for(i = 0 ; i< g_StbChipIdNum ; i++)
	{
		if( g_StbChipIdTable[i].chip_id == chip_id )
			break;
	}

	if( i < g_StbChipIdNum )
	{
		return g_StbChipIdTable[i].mac_oui;
	}
	else
	{
		LICENSE_ERROR(("GetMacOui => no chip id  [%02x]  \n"  , chip_id));
		return 0;
	}
}


int  GetMacTable( unsigned char  chip_id , int *table_num, MAC_OFFSET_TABLE  **table_point )
{
	int  error = 0;


	switch (chip_id )
	{
		case CHIP_STI5119 :

			error = 1;
			break;

		case CHIP_STI5105 :

			error = 1;
			break;

		case CHIP_STI5100 :

			*table_num = sizeof(g_StbSTi5100MacTable) / sizeof(MAC_OFFSET_TABLE);
			*table_point = g_StbSTi5100MacTable;
			break;

		case CHIP_STI7101 :

			*table_num = sizeof(g_StbSTi7101EMacTable) / sizeof(MAC_OFFSET_TABLE);
			*table_point = g_StbSTi7101EMacTable;
			break;

		case CHIP_ALI3329E :

			*table_num = sizeof(g_StbALI3329EMacTable) / sizeof(MAC_OFFSET_TABLE);
			*table_point = g_StbALI3329EMacTable;
			break;

		case CHIP_ALI3329C :

			*table_num = sizeof(g_StbALI3329CMacTable) / sizeof(MAC_OFFSET_TABLE);
			*table_point = g_StbALI3329CMacTable;
			break;

		case CHIP_ALI3602 :

			*table_num = sizeof(g_StbALI3602MacTable) / sizeof(MAC_OFFSET_TABLE);
			*table_point = g_StbALI3602MacTable;
			break;

		default :

			error = 1;
			break;
	}

	if( error )
	{
		LICENSE_ERROR(("GetMacTable => no mac table for  chip id  [%02x]  \n"  , chip_id));
	}

	return error;

}


int  GetMacAddr( unsigned char* stb_id,  int table_num, MAC_OFFSET_TABLE  *table_point , unsigned char* mac_addr)
{
	int  error = 0;
	int   index;
	int   temp_stb_num;
	int   temp_sn;
	unsigned int   temp_mode_id;


	if( error == 0 )
	{
		temp_stb_num = 0;

		temp_sn = (stb_id[STB_SN_OFFSET ]  << 16) | (stb_id[STB_SN_OFFSET+1]  <<8) | stb_id[STB_SN_OFFSET+2] ;
		temp_mode_id = (stb_id[STB_CHIP_ID_OFFSET ]  << 24) | (stb_id[STB_CHIP_ID_OFFSET+1 ]  << 16) | (stb_id[STB_CHIP_ID_OFFSET+2]  <<8) | stb_id[STB_CHIP_ID_OFFSET+3] ;

		for( index = 0 ; index < table_num ; index++ )
		{
			if( temp_mode_id == table_point[index].mode_id )
			{
				temp_stb_num = temp_stb_num + table_point[index].stb_num ;

				if( temp_sn < temp_stb_num )
					break;
			}
		}

		if( index < table_num )
		{
			mac_addr[0] = ( GetMacOui( stb_id[STB_CHIP_ID_OFFSET ]  ) >> 16 ) & 0xff;
			mac_addr[1] = ( GetMacOui( stb_id[STB_CHIP_ID_OFFSET ]  ) >> 8 ) & 0xff;
			mac_addr[2] = ( GetMacOui( stb_id[STB_CHIP_ID_OFFSET ]  )  ) & 0xff;
			mac_addr[3] = ( ( table_point[index].mac_offset + temp_sn - (temp_stb_num -table_point[index].stb_num ) ) >> 16 ) & 0xff;
			mac_addr[4] = ( ( table_point[index].mac_offset + temp_sn  - (temp_stb_num -table_point[index].stb_num )) >> 8 ) & 0xff;
			mac_addr[5] = ( ( table_point[index].mac_offset + temp_sn  - (temp_stb_num -table_point[index].stb_num )) ) & 0xff;
		}
		else
		{
			LICENSE_ERROR(("GetMacAddr => stb id is out of allocated mac address [%06x] \n" , table_point[table_num-1].mac_offset +  table_point[table_num-1].stb_num  ));
			error = 1;
		}
	}

	return error;
}


int  CheckModeIdValid(unsigned int mode_id)
{
	int    error = 0;

	unsigned char  chip_id;
	unsigned char  hw_ver;
	unsigned char  customer_id;


	chip_id = (mode_id>>24) &0xff ;
	hw_ver = (mode_id>>16) &0xff ;
	customer_id = (mode_id>>8) &0xff ;

	g_StbChipIdNum = sizeof (g_StbChipIdTable) / sizeof ( MODE_CHIP_ID ) ;
	g_StbCustomerIdNum= sizeof (g_StbCustomerIdTable) /sizeof(MODE_CUSTOMER_ID );

	if(  chip_id >= g_StbChipIdNum )
	{
		LICENSE_ERROR(("CheckModeIdValid => not support chip id  [%02x]  \n"  , chip_id ));
		error = 1;
	}

	if( customer_id >= g_StbCustomerIdNum )
	{
		LICENSE_ERROR(("CheckModeIdValid => not support customer id  [%02x]  \n"  , customer_id));
		error = 1;
	}

	if( hw_ver  >= GetHwNum(chip_id ) )
	{
		LICENSE_ERROR(("CheckModeIdValid => chip id  [%02x]  not support hw ver[%02x] \n"  , chip_id ,  hw_ver));
		error = 1;
	}

	return error;

}


int  CheckMacTableValid( unsigned char  chip_id  , int table_num, MAC_OFFSET_TABLE  *table_point )
{
	int  error = 0;
	unsigned int  mac_offset;
	int   index;


	mac_offset = table_point[0].mac_offset ;

	for( index = 0 ; index <( table_num -1 ); index++ )
	{
		mac_offset = mac_offset + table_point[index].stb_num ;

		if ( ( (table_point[index].mode_id >> 24) & 0xff )  != chip_id)
		{
			LICENSE_ERROR(("CheckMacTableValid => mode [%08x] not for chip id [%02x] \n"  , table_point[index].mode_id , chip_id ));
			break;
		}

		if ( ( (table_point[index+1].mode_id >> 24) & 0xff ) !=  chip_id )
		{
			LICENSE_ERROR(("CheckMacTableValid => mode [%08x] not for chip id [%02x] \n"  , table_point[index+1].mode_id , chip_id));
			break;
		}

		if( (mac_offset != table_point[index+1].mac_offset ) ||  ( mac_offset > 0xffffff ))
		{
			LICENSE_ERROR(("CheckMacTableValid => chip id [%02x]  mac table[%d]  offset crash  [%08x | %08x] \n"  ,  chip_id , index , mac_offset ,  table_point[index+1].mac_offset ));
			break;
		}
	}

	if( index < (table_num-1) )
	{
		error = 1;
	}

	if( error )
	{
		LICENSE_ERROR(("CheckMacTableValid =>  mac table for  chip id  [%02x]  not valid\n"  , chip_id));
	}

	return error;
}



int  DumpMacTable( unsigned char  chip_id,  int  table_num  , MAC_OFFSET_TABLE  *table_point )
{
	int   error = 0;
	int   index;

	unsigned int   temp_mode_id;
	unsigned char   temp_hw_ver;
	unsigned char   temp_customer_id;


	memset( g_StbSnString, 0 , STB_SN_STRING_LEN);
	memset( g_StbMac, 0, STB_MAC_LEN);

	g_StbChipIdNum = sizeof (g_StbChipIdTable) / sizeof ( MODE_CHIP_ID ) ;
	g_StbCustomerIdNum= sizeof (g_StbCustomerIdTable) /sizeof(MODE_CUSTOMER_ID );

	LICENSE_DEBUG(("=================CHIP[%s] MAC table=================\n" , GetChipName(chip_id)  ));
	LICENSE_DEBUG(("Mode id	\toffset	\tnumber   \tstring	\t\tmac start	\t\tmac end \n\n" ));

	if( CheckMacTableValid(chip_id , table_num , table_point  ) == 0  )
	{
		for( index = 0 ; index <  table_num  ; index++ )
		{
			temp_mode_id = table_point[index].mode_id ;
			temp_customer_id = (temp_mode_id>>8) &0xff;
			temp_hw_ver = (temp_mode_id>>16) &0xff;

			LICENSE_DEBUG(("[%08x]  \t", temp_mode_id ));
			LICENSE_DEBUG(("[%06x]  \t", table_point[index].mac_offset  ));
			LICENSE_DEBUG(("[%08x]  \t", table_point[index].stb_num ));

			if( CheckModeIdValid(temp_mode_id) == 0 )
			{
				sprintf(g_StbSnString, "%s_%s_%s",
									GetChipName( chip_id  ) ,
									GetHwName(chip_id, temp_hw_ver),
									GetCustomerName( temp_customer_id ));

				LICENSE_DEBUG(("[%s] \t", g_StbSnString ));

				g_StbMac[0] = ( GetMacOui( chip_id  ) >> 16 ) & 0xff;
				g_StbMac[1] = ( GetMacOui( chip_id  ) >> 8 ) & 0xff;
				g_StbMac[2] = ( GetMacOui( chip_id )  ) & 0xff;
				g_StbMac[3] = ( ( table_point[index].mac_offset  ) >> 16 ) & 0xff;
				g_StbMac[4] = ( ( table_point[index].mac_offset  ) >> 8 ) & 0xff;
				g_StbMac[5] = ( ( table_point[index].mac_offset  ) ) & 0xff;

				LICENSE_DEBUG(("[%02x %02x %02x %02x %02x %02x]  \t", g_StbMac[0] , g_StbMac[1], g_StbMac[2], g_StbMac[3], g_StbMac[4], g_StbMac[5] ));

				g_StbMac[3] = ( ( table_point[index].mac_offset + table_point[index].stb_num  ) >> 16 ) & 0xff;
				g_StbMac[4] = ( ( table_point[index].mac_offset + table_point[index].stb_num  ) >> 8 ) & 0xff;
				g_StbMac[5] = ( ( table_point[index].mac_offset + table_point[index].stb_num  ) ) & 0xff;

				LICENSE_DEBUG(("   [%02x %02x %02x %02x %02x %02x] \n", g_StbMac[0] , g_StbMac[1], g_StbMac[2], g_StbMac[3], g_StbMac[4], g_StbMac[5] ));
			}
		}
	}

	return  error;
}


char*		LICENSE_GetHwName(unsigned char * stb_id)
{
	int 				i;
	int				error = 0;
	int				temp_hw_num = 0 ;
	MODE_HW_VER	*temp_hw_point;
	unsigned char  	chip_id;
	unsigned char 		hw_ver;


	chip_id = stb_id[STB_CHIP_ID_OFFSET];
	hw_ver = stb_id[STB_HW_VER_OFFSET];

	GetHwNum(chip_id);

	switch ( chip_id  )
	{
		case CHIP_STI5119 :

			temp_hw_num = g_StbSTi5119HwNum ;
			temp_hw_point = g_StbSTi5119HwTable;
			break;

		case CHIP_STI5105 :

			temp_hw_num = g_StbSTi5105HwNum ;
			temp_hw_point = g_StbSTi5105HwTable;
			break;

		case CHIP_STI5100 :

			temp_hw_num = g_StbSTi5100HwNum ;
			temp_hw_point = g_StbSTi5100HwTable;
			break;

		case CHIP_STI7101 :

			temp_hw_num = g_StbSTi7101HwNum ;
			temp_hw_point = g_StbSTi7101HwTable;
			break;

		case CHIP_ALI3329E :

			temp_hw_num = g_StbALi3329EHwNum ;
			temp_hw_point = g_StbALi3329EHwTable;
			break;

		case CHIP_ALI3329C :

			temp_hw_num = g_StbALi3329CHwNum ;
			temp_hw_point = g_StbALi3329CHwTable;
			break;

		case CHIP_ALI3602 :

			temp_hw_num = g_StbALi3602HwNum ;
			temp_hw_point = g_StbALi3602HwTable;
			break;

		default :

			LICENSE_ERROR(("Get Hw Name => no hw table for  chip id  [%02x]  \n" , hw_ver  ));
			error = 1;
			break;
	}

	if( error == 0 )
	{
		for( i = 0 ;  i < temp_hw_num ; i ++ )
		{
			if( temp_hw_point[i].hw_ver == hw_ver )
				break;
		}

		if( i < temp_hw_num )
		{
			return temp_hw_point[i].hw_name;
		}
		else
		{
			LICENSE_ERROR(("Get Hw Name => not hw ver[%02x]  for chip id  [%02x]  \n"  , hw_ver , chip_id ));
		}
	}

	return NULL;

}


/***********************************************************************
	函数名称:	GetSTBMacAndSn

	函数说明:	根据机顶盒ID 获取MAC地址和序列号字符

       修改记录:	日       期      作      者       修定
 				       ---------         ---------         -----

************************************************************************/
unsigned int	LICENSE_GetSTBMacAndSnString( unsigned char * stb_id, unsigned char * stb_mac, char * stb_sn_string )
{
	unsigned int   error = 0;

	//unsigned int   mode_id;
	//int   mac_table_num = 0;
	//MAC_OFFSET_TABLE  *mac_table_point;


	//////////////////////////////////check oui valid //////////////////////////////////////
	LICENSE_DEBUG((	"\n stb id : %02x %02x %02x %02x : %02x %02x %02x \n",
							stb_id[0],
							stb_id[1],
							stb_id[2],
							stb_id[3],
							stb_id[4],
							stb_id[5],
							stb_id[6] ));

	memset( g_StbSnString, 0 , STB_SN_STRING_LEN);
	memset( g_StbMac, 0 , STB_MAC_LEN);

#if 0
	mode_id = (stb_id[STB_CHIP_ID_OFFSET]<<24)  |( stb_id[STB_HW_VER_OFFSET] <<16)  | (stb_id[STB_CUSTOMER_ID_OFFSET]<<8)  |stb_id[STB_CUSTOMER_ID_OFFSET+1] ;

	if( CheckModeIdValid(mode_id) == 0 )
	{
		sprintf(g_StbSnString, "%s_%s_%s",
						GetChipName(stb_id[STB_CHIP_ID_OFFSET]),
						GetHwName(stb_id[STB_CHIP_ID_OFFSET], stb_id[STB_HW_VER_OFFSET]),
						GetCustomerName(stb_id[STB_CUSTOMER_ID_OFFSET]));

		LICENSE_DEBUG(("stb sn  : %s \n", g_StbSnString ));

		error = 1;
		if( GetMacTable(stb_id[STB_CHIP_ID_OFFSET]  , &mac_table_num ,  &mac_table_point ) == 0 )
		{
			if( CheckMacTableValid( stb_id[STB_CHIP_ID_OFFSET]  , mac_table_num , mac_table_point   ) == 0 )
			{
				if( GetMacAddr(stb_id , mac_table_num ,  mac_table_point ,  g_StbMac ) == 0 )
				{
					error = 0;
					LICENSE_DEBUG((	"stb mac : %02x  %02x  %02x  %02x  %02x  %02x  \n",
											g_StbMac[0],
											g_StbMac[1],
											g_StbMac[2],
											g_StbMac[3],
											g_StbMac[4],
											g_StbMac[5]));
				}
			}
		}
	}
#else
	g_StbMac[0] = stb_id[STB_CHIP_ID_OFFSET] <<2;
	g_StbMac[1] = stb_id[STB_HW_VER_OFFSET];
	g_StbMac[2] = stb_id[STB_CUSTOMER_ID_OFFSET];

	g_StbMac[3] = stb_id[STB_SN_OFFSET];
	g_StbMac[4] = stb_id[STB_SN_OFFSET+1];
	g_StbMac[5] = stb_id[STB_SN_OFFSET+2];

	strcpy(g_StbSnString, "Spark-Project");
#endif

	if( stb_mac != NULL )
	{
		//stb_mac= g_StbMac;
		memcpy(stb_mac, g_StbMac, STB_MAC_LEN);
	}

	if( stb_sn_string != NULL)
	{
		//stb_sn_string = g_StbSnString ;
		memcpy(stb_sn_string, g_StbSnString, STB_SN_STRING_LEN);
	}

	return  error;
}


void  LICENSE_TestID(void)
{
	//unsigned char   stb_id[7];
	int mac_table_num;
	MAC_OFFSET_TABLE  *mac_table_point ;
	unsigned char  chip_id;


	mac_table_num = sizeof(g_StbALI3329EMacTable) / sizeof(MAC_OFFSET_TABLE);
	mac_table_point = g_StbALI3329EMacTable;
	chip_id = CHIP_ALI3329E;
	DumpMacTable(chip_id , mac_table_num , mac_table_point );

	mac_table_num = sizeof(g_StbSTi5100MacTable) / sizeof(MAC_OFFSET_TABLE);
	mac_table_point = g_StbSTi5100MacTable;
	chip_id = CHIP_STI5100;
	DumpMacTable(chip_id , mac_table_num , mac_table_point );

	mac_table_num = sizeof(g_StbSTi7101EMacTable) / sizeof(MAC_OFFSET_TABLE);
	mac_table_point = g_StbSTi7101EMacTable;
	chip_id = CHIP_STI7101;
	DumpMacTable(chip_id , mac_table_num , mac_table_point );

#if 0
	stb_id[0] =0x04;
	stb_id[1] =0x00;
	stb_id[2] =0x00;
	stb_id[3] =0x00;

	stb_id[4] =0x00;
	stb_id[5] =0x02;
	stb_id[6] =0x00;

	LICENSE_GetSTBMacAndSnString(stb_id, NULL, NULL);

	stb_id[0] =0x04;
	stb_id[1] =0x00;
	stb_id[2] =0x01;
	stb_id[3] =0x00;

	stb_id[4] =0x00;
	stb_id[5] =0x00;
	stb_id[6] =0x00;

	LICENSE_GetSTBMacAndSnString(stb_id, NULL, NULL);

	stb_id[0] =0x04;
	stb_id[1] =0x00;
	stb_id[2] =0x02;
	stb_id[3] =0x00;

	stb_id[4] =0x00;
	stb_id[5] =0x02;
	stb_id[6] =0x00;

	LICENSE_GetSTBMacAndSnString(stb_id, NULL, NULL);

	stb_id[0] =0x04;
	stb_id[1] =0x00;
	stb_id[2] =0x00;
	stb_id[3] =0x00;

	stb_id[4] =0x08;
	stb_id[5] =0x09;
	stb_id[6] =0x00;

	LICENSE_GetSTBMacAndSnString(stb_id, NULL, NULL);

	stb_id[0] =0x01;
	stb_id[1] =0x00;
	stb_id[2] =0x00;
	stb_id[3] =0x00;

	stb_id[4] =0x08;
	stb_id[5] =0x09;
	stb_id[6] =0x00;

	LICENSE_GetSTBMacAndSnString(stb_id, NULL, NULL);

	stb_id[0] =0x01;
	stb_id[1] =0x02;
	stb_id[2] =0x00;
	stb_id[3] =0x00;

	stb_id[4] =0x08;
	stb_id[5] =0x09;
	stb_id[6] =0x00;

	LICENSE_GetSTBMacAndSnString(stb_id, NULL, NULL);

	stb_id[0] =0x00;
	stb_id[1] =0x02;
	stb_id[2] =0x00;
	stb_id[3] =0x00;

	stb_id[4] =0x08;
	stb_id[5] =0x09;
	stb_id[6] =0x00;

	LICENSE_GetSTBMacAndSnString(stb_id, NULL, NULL);

	stb_id[0] =0x00;
	stb_id[1] =0x00;
	stb_id[2] =0x07;
	stb_id[3] =0x00;

	stb_id[4] =0x08;
	stb_id[5] =0x09;
	stb_id[6] =0x00;

	LICENSE_GetSTBMacAndSnString(stb_id, NULL, NULL);
#endif

	return;
}




