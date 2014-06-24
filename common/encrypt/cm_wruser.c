// Write User Zone
//
// The Write Large User Zone function is used to write data to CryptoMemory devices that have
// greater than 256 bytes in each user zone (AT88SC6416C, and larger).

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

// Write User Zone
char cm_WriteLargeZone(uint uiCryptoAddr, puchar pucBuffer, uchar ucCount)
{
    uchar ucReturn;

    ucCM_InsBuff[0] = 0xb0;
    ucCM_InsBuff[1] = (uchar)(uiCryptoAddr>>8);
    ucCM_InsBuff[2] = (uchar)uiCryptoAddr;
    ucCM_InsBuff[3] = ucCount;

    // Three bytes of the command must be included in the polynominals
    cm_GPAcmd3(ucCM_InsBuff);

    // Include the data in the polynominals and encrypt it required
    cm_GPAencrypt(ucCM_Encrypt, pucBuffer, ucCount);

    ucReturn = cm_WriteCommand(ucCM_InsBuff, pucBuffer, ucCount);

	// when anti-tearing, the host should send ACK should >= 20ms after write command
    /*LZ: extern the wait time //waitclokc(1)=0.6ms(600us)*/
	//if (ucCM_AntiTearing) CM_LOW_LEVEL.WaitClock(10);
	if (ucCM_AntiTearing) CM_LOW_LEVEL.WaitClock(40);

	// Done
	return ucReturn;
}
