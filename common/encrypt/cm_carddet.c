// Function that detects if the card is present

#include "cm_i2c.h"
#include "cm_i2c_l.h"

// Detect card
uchar cm_CardDetect(void)
{
     CM_DETECT_IN;                                       // Make detect pin an input
     if (CM_DETECT_RD) return CM_DETECT_POL?TRUE:FALSE;  // Adjust pin HI for polarity
     return CM_DETECT_POL?FALSE:TRUE;                    // Adjust pin LO for polarity
}
