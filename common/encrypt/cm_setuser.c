// Set User Zone
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_gpa.h"

// Set User Zone
uchar cm_SetUserZone(uchar ucZoneNumber, uchar ucAntiTearing)
{
	uchar ucReturn;

	ucCM_InsBuff[0] = 0xb4;
	if (ucAntiTearing) ucCM_InsBuff[1] = 0x0b;
	else 	           ucCM_InsBuff[1] = 0x03;
    ucCM_InsBuff[2] = ucZoneNumber;
	ucCM_InsBuff[3] = 0x00;

    // Only zone number is included in the polynomial
	cm_GPAGen(ucZoneNumber);

	if ((ucReturn = CM_LOW_LEVEL.SendCommand(ucCM_InsBuff))!= SUCCESS) return ucReturn;

	// save zone number and anti-tearing state
    ucCM_UserZone = ucZoneNumber;
    ucCM_AntiTearing = ucAntiTearing;

    // done
       /*LZ: modify*/
	//return  CM_LOW_LEVEL.ReceiveRet(NULL,0);
	return  CM_LOW_LEVEL.ReceiveRet(&ucReturn,0);

}
