// Low quality random number generator

#include "cm_lib.h"
#include "cm_i2c.h"
#include "cm_i2c_l.h"

uchar	rand(void)
{
	static int rand;
	/* See "Numerical Recipes in C", second edition, p. 284 */
	rand = rand * 1664525L + 1013904223L;
	return (unsigned char) (rand >> 24);
}

void cm_RandGen(puchar pucRanddat)
{
	uchar i;

//	srand(2);                      // need to introduce a source of entrophy
	for(i = 0; i < 8; i++) pucRanddat[i] = (uchar)rand();
}
