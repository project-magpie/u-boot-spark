// Reset Password
//

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"

// Reset Password
uchar cm_ResetPassword(void)
{
	   return CM_LOW_LEVEL.SendCmdByte(0xba);
}
