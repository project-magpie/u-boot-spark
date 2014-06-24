// Write Configuration Zone
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

// Write Configuration Zone
uchar cm_WriteConfigZone(uchar ucCryptoAddr, puchar pucBuffer, uchar ucCount, uchar ucAntiTearing)
{
    uchar ucReturn, ucEncrypt;

    ucCM_InsBuff[0] = 0xb4;
    if(ucAntiTearing) ucCM_InsBuff[1] = 0x08;
    else              ucCM_InsBuff[1] = 0x00;
    ucCM_InsBuff[2] = ucCryptoAddr;
    ucCM_InsBuff[3] = ucCount;

    // Three bytes of the command must be included in the polynominals
    cm_GPAcmd2(ucCM_InsBuff);

    // Only password zone is ever encrypted
    ucEncrypt = ((ucCryptoAddr>= CM_PSW) && ucCM_Encrypt);

    // Include the data in the polynominals and encrypt if required
    cm_GPAencrypt(ucEncrypt, pucBuffer, ucCount);

    // Do the write
    ucReturn = cm_WriteCommand(ucCM_InsBuff, pucBuffer,ucCount);

    // when anti-tearing, the host should send ACK should >= 20ms after write command

    /*LZ: extern the wait time //waitclokc(1)=0.6ms(600us)*/
   // if (ucAntiTearing) CM_LOW_LEVEL.WaitClock(10);
    if (ucAntiTearing) CM_LOW_LEVEL.WaitClock(40);

    return ucReturn;
}
