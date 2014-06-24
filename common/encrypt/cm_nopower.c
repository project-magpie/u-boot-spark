// Functions control the logical power on/off for the chip

#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

// Power On Chip
//
// Returns 0 (SUCCESS) if no error
//
void cm_PowerOn(void)
{
	// Reset chip data
	cm_ResetCrypto();
    ucCM_UserZone = ucCM_AntiTearing = 0;

   /*LZ comment it out*/
    // Sequence for powering on secure memory according to ATMEL spec
    //CM_DATA_OUT;                              // SDA and SCL start as outputs
	//CM_CLK_OUT;
    //CM_CLK_LO;                                // Clock should start LOW
    //CM_DATA_HI;                               // Data high during reset
 //   cm_ClockCycles(CM_PWRON_CLKS);            // Give chip some clocks cycles to get started
/*commented out end*/

    // Chip should now be in sync mode and ready to operate
}

// Shut down secure memory
//
void cm_PowerOff(void)
{
	cm_Delay(1);
	CM_CLK_LO;
	cm_Delay(6);
}
