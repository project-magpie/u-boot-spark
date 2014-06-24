// Read Fuze Byte
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

uchar	ucCmdRdFuze[4] = {0xb6, 0x01, 0x00, 0x01};

// Read Fuse Byte
uchar cm_ReadFuse(puchar pucFuze)
{
	uchar ucReturn;

  // 5 0x00, A2 (0x00), 5 0x00, N (0x01)
	cm_GPAGenNF(11, 0x01);

	if((ucReturn = cm_ReadCommand(ucCmdRdFuze,pucFuze,1)) != SUCCESS) return ucReturn;

	cm_GPAGen(*pucFuze);         // fuze byte
  cm_GPAGenN(5);               // 5 0x00s

	return SUCCESS;

}
