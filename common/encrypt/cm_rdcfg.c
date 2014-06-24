// Read Configuration Zone
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

// Read Configuration Zone
uchar cm_ReadConfigZone(uchar ucCryptoAddr, puchar pucBuffer, uchar ucCount)
{
    uchar ucReturn, ucEncrypt;

    ucCM_InsBuff[0] = 0xb6;
    ucCM_InsBuff[1] = 0x00;
    ucCM_InsBuff[2] = ucCryptoAddr;
    ucCM_InsBuff[3] = ucCount;

    // Three bytes of the command must be included in the polynominals
    cm_GPAcmd2(ucCM_InsBuff);

    // Do the read
    if ((ucReturn = cm_ReadCommand(ucCM_InsBuff, pucBuffer, ucCount)) != SUCCESS) return ucReturn;
    // Only password zone is ever encrypted
    ucEncrypt = ((ucCryptoAddr>= CM_PSW) && ucCM_Encrypt);

    // Include the data in the polynominals and decrypt if required
    cm_GPAdecrypt(ucEncrypt, pucBuffer, ucCount);

    // Done
    return SUCCESS;
}
