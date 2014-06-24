
/**********************************文件头部注释************************************/
//
//
//  					Copyright (C), 2005-2010, AV Frontier Tech. Co., Ltd.
//
//
// 文件名：		cryptoe2.c
//
// 创建者：		lz
//
// 创建时间：	2005.07.08
//
// 文件描述：
//
// 修改记录：   日       期      作      者       版本      修定
//				       ---------         ---------        -----        -----
//
/*****************************************************************************************/

/************************************文件包含****************************************/
// FOR ST INTEGRATED ENVIORNMENT
//FOR CURRENT MODULE
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

#include "cryptoe2.h"
/************************************宏定义*******************************************/

#define IDENT_DEBUG(x)	printf x
#define IDENT_ERROR(x)	printf x
#define IDENT_OK(x)		printf x



/************************************常量定义****************************************/
#define IDENT_DES_KEY_LENGTH     				14
#define IDENT_MSGSIZE							1024


#define CRYPTOE2_MAX_RETRY_TIMES				3
#define F_EURO_S2       						0
#define F_TRIPLE_DES    						1
/************************************变量定义****************************************/

extern sint			cpe2_enable_set_lnb;
extern sint			cpe2_enable_play_vdec;
sint				cpe2_enable_castack = 0;

/************************************变量引用****************************************/
#define WriteDebug(x)


static uchar 	CryptoE2_IdentID[7] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff};

static sint  iCryptoE2ErrorCount = CRYPTOE2_MAX_RETRY_TIMES;


void  IDENT_Init( void )
{
	sint  	IdentError = 0;
	sint 	iRet = 0;
	//uchar  	BlockData[32];


	iCryptoE2ErrorCount = CRYPTOE2_MAX_RETRY_TIMES;


	IdentError = IDENT_E2_Init();
	if ( IdentError == 0 )
	{
		if ( !IDENT_E2_IsCryptoE2Exist() )
		{
			IDENT_DEBUG(("Ident ic do not exist\n\r"));
			return;
		}

		IDENT_DEBUG(("Ident ic exist\n\r"));

		iRet = IDENT_E2_ReadSTBID(CryptoE2_IdentID);
		if ( iRet == TRUE )
		{
			iCryptoE2ErrorCount = 0;
			IDENT_DEBUG(("Ident ic get stb id\n\r"));
			IDENT_DEBUG(("IdentID : %02x %02x %02x %02x %02x %02x %02x \n\r",
				CryptoE2_IdentID[0],
				CryptoE2_IdentID[1],
				CryptoE2_IdentID[2],
				CryptoE2_IdentID[3],
				CryptoE2_IdentID[4],
				CryptoE2_IdentID[5],
				CryptoE2_IdentID[6]
				));
		}
        else
        {
            printf("ident ic does not get stb_id =\n");
        }

	}


	IDENT_E2_Term();

	return;

}



unsigned char *	IDENT_GetStbID( void )
{
	return CryptoE2_IdentID;
}



sint	IDENT_Valid( void )
{
	sint  valid = 0;

	if( iCryptoE2ErrorCount )
		valid = 0;
	else
		valid = 1;

	return valid;
}




