// Code for Testing the CryptoMemory Library
//

// Microcontroller specific include
// #include <io8515v.h>
#include <iom8v.h>                 // Mega8

// CryptoMemory Library Include Files
#include "cm_lib.h"
#include "cm_i2c.h"

// LED Port Defination
#define LEDS_IN  PINC
#define LEDS_DIR DDRC
#define LEDS_OUT PORTC

// Global Variables
uchar ucStep;

// Function Prototypes
void Step(uchar ucStp);
void Done(uchar ucReturn, uchar ucStatus);
void Delay(uchar ucDelay);

// Test Function
void main(void)
{
    uchar ucReturn, ucStatus;
	uchar ucData[16], ucCi[8], ucSk[8], ucG[8];
	uchar i, j ,ucTemp;

    // Use port C for LEDs (only 6 LEDS usable on Mega8)
	LEDS_OUT = 0xFE;                    // first LED on
	LEDS_DIR = 0xFF;                    // all bits outputs
	for (i = 0x01; i < 0x40; i = i<<1) {
	    LEDS_OUT = ~i;                  // walk a bit thru the LEDs
	    Delay(255);                     // wait a while
	}

	// Power on
	Step(1);
	cm_PowerOn();

	// Test write to MTZ
	Step(2);
	ucData[0] = 0x5A;
	ucData[1] = 0xA5;
	ucReturn = cm_WriteConfigZone(0x0A, ucData, 2, FALSE);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Read back data
	Step(3);
	ucData[0] = 0x00;
	ucData[1] = 0x00;
	ucReturn = cm_ReadConfigZone(0x0A, ucData, 2);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);
	if (ucData[0]!= 0x5A) Done(FAILED, 0x02);
	if (ucData[1]!= 0xA5) Done(FAILED, 0x03);

	// Unlock Config Zone
	Step(4);
	ucData[0] = 0x20;
	ucData[1] = 0x0C;
	ucData[2] = 0xE0;
	ucReturn = cm_VerifySecureCode(ucData);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Write Control for user zone 3 and set user zone
	Step(5);
	ucData[0] = 0xFF;     // Free Access
	ucData[1] = 0x63;     // Authentication set 1, POM set 2, Key 3
	ucReturn = cm_WriteConfigZone(0x26, ucData, 2, FALSE);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);
	ucReturn = cm_SetUserZone(3, FALSE);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Write data to user zone
	Step(6);
	for (i = 0; i < 16; ++i) ucData[i] = 'Z'-i;
	ucReturn = cm_WriteSmallZone(0, ucData, 16);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Read back data
	Step(7);
	for (i = 0; i < 16; ++i) ucData[i] = 0x00;
	ucReturn = cm_ReadSmallZone(0, ucData, 16);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x22);
	for (i = 0; i < 16; ++i) if (ucData[i]!= ('Z'-i)) Done(FAILED, ucData[i]);

	// Write PSW1 (both Write and Read)
	Step(8);
	ucData[0] = 0xFF;
	ucData[1] = 0x12;
	ucData[2] = 0x34;
	ucData[3] = 0x56;
	ucData[4] = 0xFF;
	ucData[5] = 0x78;
	ucData[6] = 0x9a;
	ucData[7] = 0xbc;
	ucReturn = cm_WriteConfigZone(0xB8, ucData, 8, FALSE);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Write Ci1
	Step(9);
	ucCi[0] = 0xFF;
	ucCi[1] = 0x01;
	ucCi[2] = 0x02;
	ucCi[3] = 0x03;
	ucCi[4] = 0x04;
	ucCi[5] = 0x05;
	ucCi[6] = 0x06;
	ucCi[7] = 0x07;
	ucReturn = cm_WriteConfigZone(0x60, ucCi, 8, FALSE);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Write Sk1
	Step(10);
	ucSk[0] = 0x11;
	ucSk[1] = 0x22;
	ucSk[2] = 0x33;
	ucSk[3] = 0x44;
	ucSk[4] = 0x55;
	ucSk[5] = 0x66;
	ucSk[6] = 0x77;
	ucSk[7] = 0x88;
	ucReturn = cm_WriteConfigZone(0x68, ucSk, 8, FALSE);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Write G1
	Step(11);
	ucG[0] = 0x12;
	ucG[1] = 0x23;
	ucG[2] = 0x34;
	ucG[3] = 0x45;
	ucG[4] = 0x56;
	ucG[5] = 0x67;
	ucG[6] = 0x78;
	ucG[7] = 0x89;
	ucReturn = cm_WriteConfigZone(0x98, ucG, 8, FALSE);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Set DCR to allow unlimited checksum reads
	Step(12);
	ucData[0] = 0xbf;
	ucReturn = cm_WriteConfigZone(0x18, ucData, 1, FALSE);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Set authentication required for user zone 3
	Step(13);
	ucData[0] = 0xDF;     // Normal Authentication mode
	ucData[1] = 0x63;     // Authentication set 1, POM set 2, Key 3
	ucReturn = cm_WriteConfigZone(0x26, ucData, 2, FALSE);

	// Try to activate authentication only
	Step(14);
	ucReturn = cm_ActiveSecurity(1, ucG, NULL, FALSE);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Read back data
	Step(15);
	for (i = 0; i < 16; ++i) ucData[i] = 0x00;
	ucReturn = cm_ReadSmallZone(0, ucData, 16);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x22);
	for (i = 0; i < 16; ++i) if (ucData[i]!= ('Z'-i)) Done(FAILED, ucData[i]);

	// Write data to user zone
	Step(16);
	for (i = 0; i < 16; ++i) ucData[i] = 'A'+i;
	ucReturn = cm_WriteSmallZone(0, ucData, 16);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Send checksum
	Step(17);
	ucReturn = cm_SendChecksum(NULL);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Read back data
	Step(18);
	for (i = 0; i < 16; ++i) ucData[i] = 0x00;
	ucReturn = cm_ReadSmallZone(0, ucData, 16);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x22);
	for (i = 0; i < 16; ++i) if (ucData[i]!= ('A'+i)) Done(FAILED, ucData[i]);

	// Try to deactivate security
	Step(19);
	ucReturn = cm_DeactiveSecurity();
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Set authentication and encryption required for user zone 3
	Step(20);
	ucData[0] = 0xD7;     // Normal Authentication mode, encryption required
	ucData[1] = 0x63;     // Authentication set 1, POM set 2, Key 3
	ucReturn = cm_WriteConfigZone(0x26, ucData, 2, FALSE);

	// Try to activate both authentication and encryption
	Step(21);
	ucReturn = cm_ActiveSecurity(1, ucG, NULL, TRUE);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Read back data
	Step(22);
	for (i = 0; i < 16; ++i) ucData[i] = 0x00;
	ucReturn = cm_ReadSmallZone(0, ucData, 16);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x22);
	for (i = 0; i < 16; ++i) if (ucData[i]!= ('A'+i)) Done(FAILED, ucData[i]);

	// Write data to user zone
	Step(23);
	for (i = 0; i < 16; ++i) ucData[i] = 'K'+i;
	ucReturn = cm_WriteSmallZone(0, ucData, 16);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Send checksum
	Step(24);
	ucReturn = cm_SendChecksum(NULL);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

	// Read back data
	Step(25);
	for (i = 0; i < 16; ++i) ucData[i] = 0x00;
	ucReturn = cm_ReadSmallZone(0, ucData, 16);
	if (ucReturn != SUCCESS) Done(ucReturn, 0x22);
	for (i = 0; i < 16; ++i) if (ucData[i]!= ('K'+i)) Done(FAILED, ucData[i]);

	// Try to deactivate security
	Step(26);
	ucReturn = cm_DeactiveSecurity();
	if (ucReturn != SUCCESS) Done(ucReturn, 0x01);

    // Success
	Step(27);
	ucData[0] = 0xFF;     // Free Access
	ucData[1] = 0x63;     // Authentication set 1, POM set 2, Key 3
	cm_WriteConfigZone(0x26, ucData, 2, FALSE);
	Done(SUCCESS, 0x00);
}

void Step(uchar ucStp)
{
    ucStep = ucStp;
	LEDS_OUT = ~ucStep;
}

// Done, display status/return code
void Done(uchar ucReturn, uchar ucStatus)
{
	while (TRUE) {
	    if (ucReturn == SUCCESS) LEDS_OUT = 0x00;
	    else                     LEDS_OUT = 0xFF;
	    Delay(255);              // wait a while
	    Delay(255);              // wait a while
	    LEDS_OUT = ~ucStep;
	    Delay(255);              // wait a while
	    LEDS_OUT = ~ucStatus;
	    Delay(255);              // wait a while
	    LEDS_OUT = ~ucReturn;
	    Delay(255);              // wait a while
	}
}

// Delay
void Delay(uchar ucDelay)
{
	uchar ucTimer;

    while(ucDelay) {
    	ucTimer = 255;
        while(ucTimer) ucTimer--;
    	ucDelay--;
    }
}






