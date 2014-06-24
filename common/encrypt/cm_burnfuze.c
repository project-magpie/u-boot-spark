// Burn Fuze
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"

uchar ucCmdWrFuze[4] = {0xb4, 0x01, 0x00, 0x00};

// Burn Fuse
uchar cm_BurnFuse(uchar ucFuze)
{
	uchar ucReturn;

  // Burn Fuze
	ucCmdWrFuze[2] = ucFuze;
	if((ucReturn = CM_LOW_LEVEL.SendCommand(ucCmdWrFuze))!= SUCCESS) return ucReturn;

  // done
       /*LZ: modify*/
	//return  CM_LOW_LEVEL.ReceiveRet(NULL,0);
	return  CM_LOW_LEVEL.ReceiveRet(&ucReturn,0);
}


