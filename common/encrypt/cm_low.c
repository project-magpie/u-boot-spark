// Data and Functions used by other low level functions
//
// Note: this module must be after all other low level functions in the library
//       to assure that any reference to functions in this library are satistified.

#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"

// Zone Data
uchar ucCM_UserZone;
uchar ucCM_AntiTearing;

// Chip state
uchar ucCM_Encrypt;
uchar ucCM_Authenticate;

// Global data
uchar ucCM_InsBuff[4];

// Delay
/*LZ: use larger unit*/
#if 0
void cm_Delay(uchar ucDelay)
{
	uchar ucTimer;

    while(ucDelay) {
    	ucTimer = CM_TIMER;
        while(ucTimer) ucTimer--;
    	ucDelay--;
    }
}
#else
void cm_Delay(unsigned int ucDelay)
{
	unsigned int ucTimer;

    while(ucDelay) {
    	ucTimer = CM_TIMER;
        while(ucTimer) ucTimer--;
    	ucDelay--;
    }
}
#endif
/*end*/

