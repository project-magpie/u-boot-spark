// Functions that directly control the hardware

#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"
#include "cm_gpa.h"

// Power On Chip
//
// Returns 0 (SUCCESS) if no error
//
void cm_FullPowerOn(void)
{
	// Reset chip data
	cm_ResetCrypto();
    ucCM_UserZone = ucCM_AntiTearing = 0;

	// Power control
	CM_POWER_OUT;                           // Set power pin to be an output
	if (CM_POWER_POL) CM_POWER_LO; else CM_POWER_HI;   // Turn OFF power
    CM_DIR_INIT;                              // SDA, SCL both start as outputs
    CM_CLK_LO;                                // Clock should start LOW
    CM_DATA_HI;                               // Data high during reset
	if (CM_POWER_POL) CM_POWER_HI; else CM_POWER_LO;   // Turn ON power
    cm_Delay(100);                           // Give chip a chance stabilize after power is applied

    // Sequence for powering on secure memory according to ATMEL spec
    cm_ClockCycles(CM_PWRON_CLKS);           // Give chip some clocks cycles to get started

    // Chip should now be in sync mode and ready to operate
}

// Shut down secure memory
//
void cm_FullPowerOff(void)
{
	cm_Delay(1);
	CM_CLK_LO;
	cm_Delay(6);
	if (CM_POWER_POL) CM_POWER_LO; else CM_POWER_HI;   // Turn OFF power
}
