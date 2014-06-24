// Write Checksum
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

uchar	ucCmdWrChk[4] = {0xb4, 0x02, 0x00, 0x02};

// Send Checksum
uchar cm_SendChecksum(puchar pucChkSum)
{
	uchar ucReturn;
	uchar ucChkSum[2];

    // Get Checksum if required
	if(pucChkSum == NULL) cm_CalChecksum(ucChkSum);
	else {
	   ucChkSum[0] = *pucChkSum++;
	   ucChkSum[1] = *pucChkSum;
    }

	// Send the command
	ucReturn = cm_WriteCommand(ucCmdWrChk, ucChkSum, 2);

    // Give the CyrptoMemory some processing time
    /*LZ: extern the wait time //waitclokc(1)=0.6ms(600us)*/
    //CM_LOW_LEVEL.WaitClock(5);
    CM_LOW_LEVEL.WaitClock(20);

	// Done
    return ucReturn;
}
