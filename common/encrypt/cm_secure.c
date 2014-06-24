// Activate Security
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

// Local function prototypes
static uchar cm_AuthenEncrypt(uchar ucCmd1, uchar ucAddrCi, puchar pucCi, puchar pucG_Sk, puchar pucRandom);

// Global Data
uchar ucCM_Ci[8], ucCM_G_Sk[8];
uchar ucCM_Q_Ch[16], ucCM_Ci2[8];

// Activate Security
//
// When called the function:
// ?reads the current cryptogram (Ci) of the key set,
// ?computes the next cryptogram (Ci+1) based on the secret key pucKey (GCi) and the random number selected,
// ?sends the (Ci+1) and the random number to the CryptoMemory?device,
// ?computes (Ci+2) and compares its computed value the new cryptogram of the key set.
// ?If (Ci+2) matches the new cryptogram of the key set, authentication was successful.
// In addition, if ucEncrypt is TRUE the function:
// ?computes the new session key (Ci+3) and a challenge,
// ?sends the new session key and the challenge to the CryptoMemory?device,
// ?If the new session key and the challenge are correctly related, encryption is activated.
//
uchar cm_ActiveSecurity(uchar ucKeySet, puchar pucKey, puchar pucRandom, uchar ucEncrypt)
{
    uchar i;
    uchar ucAddrCi;
	uchar ucReturn;

    // Read Ci for selected key set
    ucAddrCi = CM_Ci + (ucKeySet<<4);              // Ci blocks on 16 byte boundries
    if ((ucReturn = cm_ReadConfigZone(ucAddrCi, ucCM_Ci, 8)) != SUCCESS) return ucReturn;

    // Try to activate authentication
    for (i = 0; i < 8; ++i) ucCM_G_Sk[i] = pucKey[i];
    if ((ucReturn = cm_AuthenEncrypt(ucKeySet, ucAddrCi, ucCM_Ci, ucCM_G_Sk, pucRandom)) != SUCCESS) return ucReturn;
    ucCM_Authenticate = TRUE;

    // If Encryption required, try to activate that too
    if (ucEncrypt) {
        if (pucRandom) pucRandom += 8;
        if ((ucReturn = cm_AuthenEncrypt(ucKeySet+0x10, ucAddrCi, ucCM_Ci, ucCM_G_Sk, pucRandom)) != SUCCESS) return ucReturn;
        ucCM_Encrypt = TRUE;
	/*LZ modify: a bug...*/
    	ucCM_Authenticate = TRUE;
	/*end*/
    }

    // Done
    return SUCCESS;
}

// Common code for both activating authentication and encryption
static uchar cm_AuthenEncrypt(uchar ucCmd1, uchar ucAddrCi, puchar pucCi, puchar pucG_Sk, puchar pucRandom)
{
    uchar i;
	uchar ucReturn;

    // Generate chalange data
    if (pucRandom) for (i = 0; i < 8; ++i) ucCM_Q_Ch[i] = pucRandom[i];
    else           CM_LOW_LEVEL.RandomGen(ucCM_Q_Ch);

//HCX_080611 modify: fix bug
//    for (i = 0; i < 8; ++i) ucCM_Q_Ch[i] = pucRandom[i];
    if (pucRandom) for (i = 0; i < 8; ++i) ucCM_Q_Ch[i+8] = pucRandom[i];
    else           CM_LOW_LEVEL.RandomGen(&ucCM_Q_Ch[8]);
//HCX_080611 modify: end

    cm_AuthenEncryptCal(pucCi, pucG_Sk, ucCM_Q_Ch, &ucCM_Q_Ch[8]);

    // Send chalange
    ucCM_InsBuff[0] = 0xb8;
    ucCM_InsBuff[1] = ucCmd1;
    ucCM_InsBuff[2] = 0x00;
    ucCM_InsBuff[3] = 0x10;
    if ((ucReturn = cm_WriteCommand(ucCM_InsBuff, ucCM_Q_Ch, 16)) != SUCCESS) return ucReturn;

    // Give chips some clocks to do calculations
    /*LZ: extern the wait time //waitclokc(1)=0.6ms(600us)*/
    //CM_LOW_LEVEL.WaitClock(3);
    CM_LOW_LEVEL.WaitClock(20);

    // Verify result
    if ((ucReturn = cm_ReadConfigZone(ucAddrCi, ucCM_Ci2, 8)) != SUCCESS) return ucReturn;
    for(i=0; i<8; i++) if (pucCi[i]!=ucCM_Ci2[i]) return FAILED;

    // Done
    return SUCCESS;
}
