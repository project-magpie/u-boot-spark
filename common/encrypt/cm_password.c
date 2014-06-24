// Verify Password
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

uchar ucCmdPassword[4] = {0xba, 0x00, 0x00, 0x03};
uchar ucPSW[3];

// Verify Password
uchar cm_VerifyPassword(puchar pucPassword, uchar ucSet, uchar ucRW)
{
	uchar i, j;
    uchar ucReturn;
	uchar ucAddr;


	// Build command and PAC address
    ucAddr = CM_PSW + (ucSet<<3);
	ucCmdPassword[1] = ucSet;
	if (ucRW != CM_PWWRITE) {
	  	ucCmdPassword[1] |= 0x10;
	  	ucAddr += 4;
	}

	// Deal with encryption if in authenticate mode
	for (j = 0; j<3; j++) {
	    // Encrypt the password
	    if(ucCM_Authenticate) {
    	    for(i = 0; i<5; i++) cm_GPAGen(pucPassword[j]);
    		ucPSW[j] = Gpa_byte;
	  	}
		/*LZ modify: a bug.....*/
	    // Else just copy it
		//ucPSW[j] = pucPassword[j];
		else
		{
		    // Else just copy it
			ucPSW[j] = pucPassword[j];
		}
		/*end modify*/
    }

	// Send the command
	ucReturn = cm_WriteCommand(ucCmdPassword, ucPSW, 3);

    // Wait for chip to process password

    /*LZ: extern the wait time //waitclokc(1)=0.6ms(600us)*/
    //CM_LOW_LEVEL.WaitClock(3);
    CM_LOW_LEVEL.WaitClock(12);

    // Read Password attempts counter to determine if the password was accepted
    if (ucReturn == SUCCESS) {
        ucReturn = cm_ReadConfigZone(ucAddr, ucPSW, 1);
	    if (ucPSW[0]!= 0xFF) ucReturn = FAILED;
	}
	if (ucCM_Authenticate && (ucReturn != SUCCESS)) cm_ResetCrypto();

    // Done
    return ucReturn;
}
