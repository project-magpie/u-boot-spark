/**********************************文件头部注释***********************************/
//
//
//						Copyright (C), 2005-2010, AV Frontier Tech. Co., Ltd.
//
//
// 文件名：		crypto_e2.h
//
// 创建者：		LZ
//
// 创建时间：
//
// 文件描述：
//
// 修改记录：	日		 期 	 作 	 者 	  版本		修定
//					   ---------		 ---------		  ----- 	   -----
//
/****************************************************************************************/

#ifndef __CRYPTOE2_H
#define __CRYPTOE2_H

/*COMMENT: write configuration zone, the data written may be the password, secure code etc.*/
sint  E2PROM_WriteConfigZone(uint 	Addr,
							uchar		*Buffer,
							uint 	NumberToWrite,
							bool	bAntiTearing);

/*COMMENT: read configuration zone.*/
sint  E2PROM_ReadConfigZone( uchar	Addr,
							uchar	*Buffer,
							uchar	NumberToWrite );

/*COMMENT: read  fuse state(burn or not).*/


/*COMMENT: read checksum from e2 device.*/
sint  E2PROM_ReadChecksum(uchar *pucCheckSum);


/*COMMENT: check whether the password is valid.*/
sint  E2PROM_VerifyPwd( uchar ucPwdSet, uchar aucPwd[3], bool WritePwd );


/*COMMENT: check whether the user zone2 work good.*/
sint  E2PROM_TestZone2( void );


/*COMMENT: check whether the user zone1 work good.*/
sint  E2PROM_TestZone1( void );


/*COMMENT: check whether the user zone0 work good.*/
sint  E2PROM_TestZone0( void );


/*COMMENT: init crypto e2 config zone, config zone0-2. see 'E2PROM_TestZone0-2'  for more details.*/
sint  E2PROM_TestInit( void );


/*COMMENT: net functions.*/

sint	E2PROM_NetInit( void );


/*COMMENT: check whether cryptoe2 exist or not.
					(return TRUE : exist)
					(return FALSE: not exist)*/
bool	IDENT_E2_IsCryptoE2Exist( void );
bool	IDENT_E2_ReadSTBID( uchar *pucData );

/*COMMENT: just need a i2c handle for pio simulating.*/
sint  IDENT_E2_Init( void );
sint  IDENT_E2_Term( void );

#endif




