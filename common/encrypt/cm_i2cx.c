// Functions that directly control the hardware that are not needed in all cases

#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"


// Send a command byte
//
uchar cm_SendCmdByte(uchar ucCommand)
{
    uchar i, ucCmd;

    i = CM_START_TRIES;

    ucCmd = (ucCommand&0x0F)|CM_PORT_CFG.ucChipSelect;
    while (i) {
      cm_Start();
	    if (cm_Write(ucCmd) == 0) break;
	    if (--i == 0) return FAIL_CMDSTART;
    }

    return SUCCESS;
}
