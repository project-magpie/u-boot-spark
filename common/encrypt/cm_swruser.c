// Write Small User Zone
//
// The Write Small User Zone function is used to write data to CryptoMemory devices that have
// 256 bytes or less in each user zone (AT88SC3216C, and smaller)

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

// Write Small User Zone
uchar cm_WriteSmallZone(uchar  ucCryptoAddr, puchar pucBuffer, uchar ucCount)
{
	uchar ucReturn;

	ucCM_InsBuff[0] = 0xb0;
    ucCM_InsBuff[1] = 0x00;
    ucCM_InsBuff[2] = ucCryptoAddr;
    ucCM_InsBuff[3] = ucCount;

    // Two bytes of the command must be included in the polynominals
    cm_GPAcmd2(ucCM_InsBuff);

    // Include the data in the polynominals and encrypt it required
    cm_GPAencrypt(ucCM_Encrypt, pucBuffer, ucCount);

	// Write the data
	ucReturn = cm_WriteCommand(ucCM_InsBuff, pucBuffer,ucCount);

	// when anti-tearing, the host should send ACK should >= 20ms after write command

    /*LZ: extern the wait time //waitclokc(1)=0.6ms(600us)*/
   // if (ucCM_AntiTearing) CM_LOW_LEVEL.WaitClock(10);
    if (ucCM_AntiTearing) CM_LOW_LEVEL.WaitClock(40);

	return ucReturn;
}
