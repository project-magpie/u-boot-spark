/*
 * File     : sample.c
 * Synopsys : Simple LED flashing platform for various platforms
 *
 * Copyright 2003 STMicroelectronics Limited.  All right reserved.
 */

volatile unsigned char *ledregister = (volatile unsigned char *) (0x41100000);

#define LED_BASE        0x41100000


#define ON_VALUE  0x0
#define OFF_VALUE 0x1

static void led_on (int value)
{
	*ledregister = ON_VALUE;
}

static void led_off (int value)
{
	*ledregister = OFF_VALUE;
}
