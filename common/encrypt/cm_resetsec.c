// Deactivate Security
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

// Deactivate Security
uchar cm_DeactiveSecurity(void)
{
	uchar ucReturn;

	if ((ucReturn = CM_LOW_LEVEL.SendCmdByte(0xb8)) != SUCCESS) return ucReturn;
	cm_ResetCrypto();

	return SUCCESS;
}
