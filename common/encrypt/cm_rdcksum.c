// Read Checksum
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

uchar	ucCmdRdChk[4] = {0xb6, 0x02, 0x00, 0x02};

// Read Checksum
uchar cm_ReadChecksum(puchar pucChkSum)
{
    uchar ucDCR[1];
	uchar ucReturn;

    // 20 0x00s (10 0x00s, ignore first byte, 5 0x00s, ignore second byte, 5 0x00s
    cm_GPAGenN(20);

    // Read the checksum
    if((ucReturn = cm_ReadCommand(ucCmdRdChk, pucChkSum, 2))!= SUCCESS) return ucReturn;

    // Check if unlimited reads allowed
    if ((ucReturn = cm_ReadConfigZone(DCR_ADDR, ucDCR, 1)) != SUCCESS) return ucReturn;
    if ((ucDCR[0]&DCR_UCR)) cm_ResetCrypto();

    return SUCCESS;
}
