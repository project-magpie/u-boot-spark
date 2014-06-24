#include "linux/types.h"
#include "linux/string.h"
#include "des.h"

#define	DES_DBG(x)	//printf x

static byte  PC2[8][6] =
{
    { 14, 17, 11, 24,  1,  5 },

    {  3, 28, 15,  6, 21, 10 },

    { 23, 19, 12,  4, 26,  8 },

    { 16,  7, 27, 20, 13,  2 },

    { 41, 52, 31, 37, 47, 55 },

    { 30, 40, 51, 45, 33, 48 },

    { 44, 49, 39, 56, 34, 53 },

    { 46, 42, 50, 36, 29, 32 }

};


static byte  E[8][6] =
{

    { 32,  1,  2,  3,  4,  5 },

    {  4,  5,  6,  7,  8,  9 },

    {  8,  9, 10, 11, 12, 13 },

    { 12, 13, 14, 15, 16, 17 },

    { 16, 17, 18, 19, 20, 21 },

    { 20, 21, 22, 23, 24, 25 },

    { 24, 25, 26, 27, 28, 29 },

    { 28, 29, 30, 31, 32,  1 }

};


static byte  P[32] =
{

    16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,

     2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25

};



static byte  SBOXES[4][64] =
{
    {

        0x2e, 0xe0, 0xc4, 0xbf, 0x4d, 0x27, 0x11, 0xc4,

        0x72, 0x4e, 0xaf, 0x72, 0xbb, 0xdd, 0x68, 0x11,

        0x83, 0x5a, 0x5a, 0x06, 0x36, 0xfc, 0xfc, 0xab,

        0xd5, 0x39, 0x09, 0x95, 0xe0, 0x83, 0x97, 0x68,

        0x44, 0xbf, 0x21, 0x8c, 0x1e, 0xc8, 0xb8, 0x72,

        0xad, 0x14, 0xd6, 0xe9, 0x72, 0x21, 0x8b, 0xd7,

        0xff, 0x65, 0x9c, 0xfb, 0xc9, 0x03, 0x57, 0x9e,

        0x63, 0xaa, 0x3a, 0x40, 0x05, 0x56, 0xe0, 0x3d

    },



    {

        0xcf, 0xa3, 0x11, 0xfd, 0xa8, 0x44, 0xfe, 0x27,

        0x96, 0x7f, 0x2b, 0xc2, 0x63, 0x98, 0x84, 0x5e,

        0x09, 0x6c, 0xd7, 0x10, 0x32, 0xd1, 0x4d, 0xea,

        0xec, 0x06, 0x70, 0xb9, 0x55, 0x3b, 0xba, 0x85,

        0x90, 0x4d, 0xee, 0x38, 0xf7, 0x2a, 0x5b, 0xc1,

        0x2a, 0x93, 0x84, 0x5f, 0xcd, 0xf4, 0x31, 0xa2,

        0x75, 0xbb, 0x08, 0xe6, 0x4c, 0x17, 0xa6, 0x7c,

        0x19, 0x60, 0xd3, 0x05, 0xb2, 0x8e, 0x6f, 0xd9

    },



    {

        0x4a, 0xdd, 0xb0, 0x07, 0x29, 0xb0, 0xee, 0x79,

        0xf6, 0x43, 0x03, 0x94, 0x8f, 0x16, 0xd5, 0xaa,

        0x31, 0xe2, 0xcd, 0x38, 0x9c, 0x55, 0x77, 0xce,

        0x5b, 0x2c, 0xa4, 0xfb, 0x62, 0x8f, 0x18, 0x61,

        0x1d, 0x61, 0x46, 0xba, 0xb4, 0xdd, 0xd9, 0x80,

        0xc8, 0x16, 0x3f, 0x49, 0x73, 0xa8, 0xe0, 0x77,

        0xab, 0x94, 0xf1, 0x5f, 0x62, 0x0e, 0x8c, 0xf3,

        0x05, 0xeb, 0x5a, 0x25, 0x9e, 0x32, 0x27, 0xcc

    },



    {

        0xd7, 0x1d, 0x2d, 0xf8, 0x8e, 0xdb, 0x43, 0x85,

        0x60, 0xa6, 0xf6, 0x3f, 0xb9, 0x70, 0x1a, 0x43,

        0xa1, 0xc4, 0x92, 0x57, 0x38, 0x62, 0xe5, 0xbc,

        0x5b, 0x01, 0x0c, 0xea, 0xc4, 0x9e, 0x7f, 0x29,

        0x7a, 0x23, 0xb6, 0x1f, 0x49, 0xe0, 0x10, 0x76,

        0x9c, 0x4a, 0xcb, 0xa1, 0xe7, 0x8d, 0x2d, 0xd8,

        0x0f, 0xf9, 0x61, 0xc4, 0xa3, 0x95, 0xde, 0x0b,

        0xf5, 0x3c, 0x32, 0x57, 0x58, 0x62, 0x84, 0xbe

    }

};



static byte  PC1[][8] =
{

                          {57, 49, 41, 33, 25, 17,  9, 1},

                          {58, 50, 42, 34, 26, 18, 10, 2},

                          {59, 51, 43, 35, 27, 19, 11, 3},

                          {60, 52, 44, 36, 63, 55, 47,39},

                          {31, 23, 15,  7, 62, 54, 46,38},

                          {30, 22, 14,  6, 61, 53, 45,37},

                          {29, 21, 13,  5, 28, 20, 12, 4}

};



void doPC1_(byte data[])
{

byte buf[8];

byte i, j;

	memset(buf, 0, 8);

	for(j=0; j<7; j++) {

		for(i=0; i<8; i++) {

			byte lookup = PC1[j][i];

			buf[j] |= ((data[(lookup>>3)]>>(8-(lookup & 7))) & 1) << (7-i);

		}

	}

	memcpy(data, buf, 8);

}



static void doIp(byte data[])
{

unsigned char j, k;

byte val;

byte buf[8];

byte *p;

byte i = 8;



    for(i=0; i<8; i++)
    {

        val = data[i];



        p = &buf[3];

        j = 4;

        do
        {
            for(k=0; k<=4; k+=4)
            {

                p[k] >>= 1;

                if(val & 1) p[k] |= 0x80;

                val >>= 1;

            }


            p--;

        } while(--j);

    }



    memcpy(data, buf, 8);

}



static void doIp_1(byte data[])
{

unsigned char j, k;

byte r = 0;

byte buf[8];

byte *p;

byte i = 8;



    for(i=0; i<8; i++)
    {

        p = &data[3];

        j = 4;

        do
        {

            for(k=0; k<=4; k+=4)

            {

                r >>= 1;

                if(p[k] & 1) r |= 0x80;

                p[k] >>= 1;

            }



            p--;

        } while(--j);



        buf[i] = r;

    }



    memcpy(data, buf, 8);

}



static void makeK(byte *left, byte *right, byte *K)
{

byte i, j;

byte bit, val;

byte *p;



    for(i=0; i<8; i++)

    {

        val = 0;



        for(j=0; j<6; j++)

        {

            bit = PC2[i][j];

            if(bit < 29)

            {

                bit = 28-bit;

                p   = left;

            }

            else

            {

                bit = 56-bit;

                p   = right;

            }



            val <<= 1;

            if( p[bit >> 3] & (1 << (bit & 7)) ) val |= 1;

        }



        *K = val;

		K++;

    }

}



static void rightRot(byte key[])

{

byte *p     = key;

byte  i     = 3;

byte  carry = 0;



    carry = 0;

    if(*p & 1) carry = 0x08;



    do

    {

        *p = (*p >> 1) | ((p[1] & 1) ? 0x80 : 0);

        p++;

    } while(--i);



    *p = (*p >> 1) | carry;

}



static void rightRotKeys(byte left[], byte right[])

{

    rightRot(left);

    rightRot(right);

}



static void leftRot(byte key[])

{

byte i = 27;



    do

    {

        rightRot(key);

    } while(--i);

}



static void leftRotKeys(byte left[], byte right[])

{

    leftRot(left);

    leftRot(right);

}



static void desCore(byte data[], byte K[], byte result[])

{

byte i, j;

byte bit, val;



    memset(result, 0, 4);



    for(i=0; i<8; i++)

    {

        val = 0;



        for(j=0; j<6; j++)

        {

            bit = 32-E[i][j];



            val <<= 1;

            if( data[3 - (bit >> 3)] & (1 << (bit & 7)) ) val |= 1;

        }



        val ^= K[i];

        val  = SBOXES[i & 3][val];

        if(i > 3)

        {

            val >>= 4;

        }

        val &= 0x0f;



        result[i >> 1] |= (i & 1) ? val : (val << 4);

    }

}



static void permut32(byte data[])

{

byte i, j;

byte bit;

byte r[4];

byte *p;



    for(i=0; i<32; i++)

    {

        bit = 32-P[i];



        p = r;

        for(j=0; j<3; j++)

        {

            *p = (*p << 1) | ((p[1] & 0x80) ? 1 : 0);

            p++;

        }



        *p <<= 1;

        if( data[3 - (bit >> 3)] & (1 << (bit & 7)) ) *p |= 1;

    }



    memcpy(data, r, 4);

}



static void swap(byte left[], byte right[])

{

byte x[4];



    memcpy(x, right, 4);

    memcpy(right, left, 4);

    memcpy(left, x, 4);

}



static void desRound(byte left[], byte right[], byte data[], byte

		     mode, byte k8)

{

  byte i;

  byte K[8];

  byte r[4];

  byte tempr[4];

  unsigned short temp;



  memcpy(tempr, data+4, 4);



  /* Viaccess */

  temp = (short)k8*(short)tempr[0]+(short)k8+(short)tempr[0];

  tempr[0] = (temp & 0xff) - ((temp>>8) & 0xff);

  if((temp & 0xff) - (temp>>8) < 0)

    tempr[0]++;



  makeK(left, right, K);

  desCore(tempr, K, r);

  permut32(r);



  if(mode & DES_HASH)

    {

      i    = r[0];

      r[0] = r[1];

      r[1] = i;

    }



  for(i=0; i<4; i++)

    {

      *data ^= r[i];

      data++;

    }



    swap(data-4, data);

}



void des_(byte key[], byte mode, byte data[])

{

  byte i;

  byte left[8];

  byte right[8];

  byte *p = left;

  short DESShift = (mode & DES_RIGHT) ? 0x8103 : 0xc081;



  for(i=3; i>0; i--)

    {

        *p = (key[i-1] << 4) | (key[i] >> 4);

        p++;

    }

    left[3] =  key[0] >> 4;



    right[0] = key[6];

    right[1] = key[5];

    right[2] = key[4];

    right[3] = key[3] & 0x0f;



    if(mode & DES_IP) doIp(data);



    do

    {

        if(!(mode & DES_RIGHT))

        {

            leftRotKeys(left, right);

            if(!(DESShift & 0x8000)) leftRotKeys(left, right);

        }



        desRound(left, right, data, mode, key[7]);



        if(mode & DES_RIGHT)

        {

            rightRotKeys(left, right);

            if(!(DESShift & 0x8000)) rightRotKeys(left, right);

        }



        DESShift <<= 1;

    } while(DESShift);



    swap(data, data+4);



    if(mode & DES_IP_1) doIp_1(data);

}

static unsigned char getmask(unsigned char *OutData, unsigned char *Mask, unsigned char I, unsigned char J) {

  unsigned char K, B, M, M1 , D, DI, MI;

  K = I ^ J;
  DI = 7;
  if ((K & 4) == 4) {
    K ^= 7;
    DI ^= 7;
  }
  MI = 3;
  MI &= J;
  K ^= MI;
  K += MI;
  if ((K & 4) == 4) {
    return 0;
  }
  DI ^= J;
  D = OutData[DI];
  MI = 0;
  MI += J;
  M1 = Mask[MI];
  MI ^= 4;
  M = Mask[MI];
  B = 0;
  for(K = 0; K <=7; K++) {
    if ((D & 1) == 1) B += M;
    D = (D >> 1) + ((B & 1) << 7);
    B = B >> 1;
  }
  return D ^ M1;
}

static void v2mask(unsigned char *cw, unsigned char *mask) {

	int i, j;

	for(i = 7; i >= 0; i--)
		for(j = 7; j >=4; j--)
			cw[i] ^= getmask(cw, mask, i, j);
	for(i = 0; i <= 7; i++)
		for(j = 0; j <=3; j++)
			cw[i] ^= getmask(cw, mask, i, j);
}


void EuroDes(byte key1[], byte key2[], byte desMode, byte operatingMode, byte data[])

{

byte mode;



 if(key1[7]) { /* Viaccess */

   mode = (operatingMode == HASH) ? DES_ECM_HASH : DES_ECM_CRYPT;

   if(key2 != NULL)
     	v2mask(data, key2);
   des_(key1, mode, data);
   if(key2 != NULL)
     	v2mask(data, key2);
 }



 else if(TestBit(desMode, F_TRIPLE_DES))

   {

     /* Eurocrypt 3-DES */

     mode = (operatingMode == HASH) ?  0 : DES_RIGHT;



     des_(key1, (byte)(DES_IP | mode), data);

     mode ^= DES_RIGHT;

     des_(key2, mode, data);

     mode ^= DES_RIGHT;

     des_(key1, (byte)(mode | DES_IP_1), data);

   }

 else

   {

     if(TestBit(desMode, F_EURO_S2))

       {

	 /* Eurocrypt S2 */

	 mode = (operatingMode == HASH) ? DES_ECS2_CRYPT : DES_ECS2_DECRYPT;

       }

     else

       {

	 /* Eurocrypt M */

	 mode = (operatingMode == HASH) ? DES_ECM_HASH : DES_ECM_CRYPT;

       }

     des_(key1, mode, data);

   }

}


#define MD5_TEST
#ifdef MD5_TEST

/*
 * A JavaScript implementation of the RSA Data Security, Inc. MD5 Message
 * Digest Algorithm, as defined in RFC 1321.
 * Version 2.1 Copyright (C) Paul Johnston 1999 - 2002.
 * Other contributors: Greg Holt, Andrew Kepert, Ydnar, Lostinet
 * Distributed under the BSD License
 * See http://pajhome.org.uk/crypt/md5 for more info.
 */

/*
 * Configurable variables. You may need to tweak these to be compatible with
 * the server-side, but the defaults work in most cases.
 */
char hexcase = 0;  /* hex output format. 0 - lowercase; 1 - uppercase        */
char b64pad = '='; /* base-64 pad character. "=" for strict RFC compliance   */
char chrsz  = 8;  /* bits per input character. 8 - ASCII; 16 - Unicode      */

/*
 * Add integers, wrapping at 2^32. This uses 16-bit operations internally
 * to work around bugs in some JS interpreters.
 */
int safe_add(int x, int y)
{
  int lsw = (x & 0xFFFF) + (y & 0xFFFF);
  int msw = (x >> 16) + (y >> 16) + (lsw >> 16);
  return (msw << 16) | (lsw & 0xFFFF);
}

/*
 * Bitwise rotate a 32-bit number to the left.
 */
int bit_rol(int num, int cnt)
{
  return ((num << cnt) | (num >> (32 - cnt)));
}

/*
 * Convert a string to an array of little-endian words
 * If chrsz is ASCII, characters >255 have their hi-byte silently ignored.
 */
unsigned int *str2binl(char *str, int *len)
{
  static unsigned int bin[32];
  int mask;
  int i;

  memset(bin,0,32*4);
  mask = (1 << chrsz) - 1;
  for( i = 0; i < strlen(str) * chrsz; i += chrsz)
  {
  	bin[i>>5] |= (str[i/chrsz] & mask) << (i%32);
  }
  *len = (((i-chrsz)/32)+1);

  return bin;
}

/*
 * Convert an array of little-endian words to a string
 */
char *binl2str( unsigned int bin[], int len )
{
  static char str[17];
  int	 mask;
  int i;

  memset( str, 0, 17 );

  mask = (1 << chrsz) - 1;
  for( i = 0; i < len * 32; i += chrsz)
  {
  	str[i%8] = ((bin[i>>5] >> (i % 32)) & mask);
  }
  return str;
}

/*
 * Convert an array of little-endian words to a hex string.
 */
char *binl2hex( unsigned int *binarray, int len)
{
  static char str[33];
  int i;
  char hex_tab[17];

  memset(hex_tab, 0, 17);
  if ( hexcase )
  	memcpy(hex_tab, "0123456789ABCDEF", 16);
  else
  	memcpy(hex_tab, "0123456789abcdef", 16);

  memset( str, 0, 33 );
  for( i = 0; i < len * 4; i++)
  {
    str[i*2]= hex_tab[(binarray[i>>2] >> ((i%4)*8+4)) & 0xF];
    str[i*2+1]= hex_tab[(binarray[i>>2] >> ((i%4)*8  )) & 0xF];
  }
  return str;
}


/*
 * Convert an array of little-endian words to a base-64 string
 */
char* binl2b64(unsigned int *binarray, int len)
{
  char tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  static char FinalStr[22+1] = "";
  char *str;
  int i,j;
  int triplet;

  str = &FinalStr[0];
  for( i = 0; i < len * 4; i += 3)
  {
    triplet = (((binarray[i   >> 2] >> 8 * ( i   %4)) & 0xFF) << 16)
                | (((binarray[(i+1) >> 2] >> 8 * ((i+1)%4)) & 0xFF) << 8 )
                |  ((binarray[(i+2) >> 2] >> 8 * ((i+2)%4)) & 0xFF);
    for( j = 0; j < 4; j++)
    {
       if(i * 8 + j * 6 > len * 32)
	{
//		*str = b64pad;
//		str++;
		return FinalStr;
      	}
       else
	{
		*str = tab[(triplet >> 6*(3-j)) & 0x3F];
		str++;
       }
    }
  }
  return FinalStr;
}


/*
 * These functions implement the four basic operations the algorithm uses.
 */
int md5_cmn(int q, int a, int b, int x, int s, int t)
{
  return safe_add(bit_rol(safe_add(safe_add(a, q), safe_add(x, t)), s),b);
}
int md5_ff(int a, int b, int c, int d,int x, int s,int  t)
{
  return md5_cmn((b & c) | ((~b) & d), a, b, x, s, t);
}
int md5_gg(int a, int b, int c, int d, int x, int s, int t)
{
  return md5_cmn((b & d) | (c & (~d)), a, b, x, s, t);
}
int md5_hh(int a, int b, int c, int d, int x, int s, int t)
{
  return md5_cmn(b ^ c ^ d, a, b, x, s, t);
}
int md5_ii(int a, int b, int c, int d, int x, int s, int t)
{
  return md5_cmn(c ^ (b | (~d)), a, b, x, s, t);
}


/*
 * Calculate the MD5 of an array of little-endian words, and a bit length
 */
unsigned int  *core_md5( unsigned int *x, int len)
{
  long a =  1732584193;
  long b = -271733879;
  long c = -1732584194;
  long d =  271733878;

  static unsigned int retArray[4];
  int i;
  int iData4Bytes;


  memset ( retArray,0,4*sizeof(unsigned int) );

  /* append padding */
  x[len >> 5] |= 0x80 << ((len) % 32);
  x[(((len + 64) >> 9) << 4) + 14] = len;

  iData4Bytes = len/(chrsz*4);
  if ( iData4Bytes <= 0 )
  	iData4Bytes = 1;

  for( i= 0; i <iData4Bytes; i += 16)
  {
    long olda = a;
    long oldb = b;
    long oldc = c;
    long oldd = d;

    a = md5_ff(a, b, c, d, x[i+ 0], 7 , -680876936);
    d = md5_ff(d, a, b, c, x[i+ 1], 12, -389564586);
    c = md5_ff(c, d, a, b, x[i+ 2], 17,  606105819);
    b = md5_ff(b, c, d, a, x[i+ 3], 22, -1044525330);
    a = md5_ff(a, b, c, d, x[i+ 4], 7 , -176418897);
    d = md5_ff(d, a, b, c, x[i+ 5], 12,  1200080426);
    c = md5_ff(c, d, a, b, x[i+ 6], 17, -1473231341);
    b = md5_ff(b, c, d, a, x[i+ 7], 22, -45705983);
    a = md5_ff(a, b, c, d, x[i+ 8], 7 ,  1770035416);
    d = md5_ff(d, a, b, c, x[i+ 9], 12, -1958414417);
    c = md5_ff(c, d, a, b, x[i+10], 17, -42063);
    b = md5_ff(b, c, d, a, x[i+11], 22, -1990404162);
    a = md5_ff(a, b, c, d, x[i+12], 7 ,  1804603682);
    d = md5_ff(d, a, b, c, x[i+13], 12, -40341101);
    c = md5_ff(c, d, a, b, x[i+14], 17, -1502002290);
    b = md5_ff(b, c, d, a, x[i+15], 22,  1236535329);

    a = md5_gg(a, b, c, d, x[i+ 1], 5 , -165796510);
    d = md5_gg(d, a, b, c, x[i+ 6], 9 , -1069501632);
    c = md5_gg(c, d, a, b, x[i+11], 14,  643717713);
    b = md5_gg(b, c, d, a, x[i+ 0], 20, -373897302);
    a = md5_gg(a, b, c, d, x[i+ 5], 5 , -701558691);
    d = md5_gg(d, a, b, c, x[i+10], 9 ,  38016083);
    c = md5_gg(c, d, a, b, x[i+15], 14, -660478335);
    b = md5_gg(b, c, d, a, x[i+ 4], 20, -405537848);
    a = md5_gg(a, b, c, d, x[i+ 9], 5 ,  568446438);
    d = md5_gg(d, a, b, c, x[i+14], 9 , -1019803690);
    c = md5_gg(c, d, a, b, x[i+ 3], 14, -187363961);
    b = md5_gg(b, c, d, a, x[i+ 8], 20,  1163531501);
    a = md5_gg(a, b, c, d, x[i+13], 5 , -1444681467);
    d = md5_gg(d, a, b, c, x[i+ 2], 9 , -51403784);
    c = md5_gg(c, d, a, b, x[i+ 7], 14,  1735328473);
    b = md5_gg(b, c, d, a, x[i+12], 20, -1926607734);

    a = md5_hh(a, b, c, d, x[i+ 5], 4 , -378558);
    d = md5_hh(d, a, b, c, x[i+ 8], 11, -2022574463);
    c = md5_hh(c, d, a, b, x[i+11], 16,  1839030562);
    b = md5_hh(b, c, d, a, x[i+14], 23, -35309556);
    a = md5_hh(a, b, c, d, x[i+ 1], 4 , -1530992060);
    d = md5_hh(d, a, b, c, x[i+ 4], 11,  1272893353);
    c = md5_hh(c, d, a, b, x[i+ 7], 16, -155497632);
    b = md5_hh(b, c, d, a, x[i+10], 23, -1094730640);
    a = md5_hh(a, b, c, d, x[i+13], 4 ,  681279174);
    d = md5_hh(d, a, b, c, x[i+ 0], 11, -358537222);
    c = md5_hh(c, d, a, b, x[i+ 3], 16, -722521979);
    b = md5_hh(b, c, d, a, x[i+ 6], 23,  76029189);
    a = md5_hh(a, b, c, d, x[i+ 9], 4 , -640364487);
    d = md5_hh(d, a, b, c, x[i+12], 11, -421815835);
    c = md5_hh(c, d, a, b, x[i+15], 16,  530742520);
    b = md5_hh(b, c, d, a, x[i+ 2], 23, -995338651);

    a = md5_ii(a, b, c, d, x[i+ 0], 6 , -198630844);
    d = md5_ii(d, a, b, c, x[i+ 7], 10,  1126891415);
    c = md5_ii(c, d, a, b, x[i+14], 15, -1416354905);
    b = md5_ii(b, c, d, a, x[i+ 5], 21, -57434055);
    a = md5_ii(a, b, c, d, x[i+12], 6 ,  1700485571);
    d = md5_ii(d, a, b, c, x[i+ 3], 10, -1894986606);
    c = md5_ii(c, d, a, b, x[i+10], 15, -1051523);
    b = md5_ii(b, c, d, a, x[i+ 1], 21, -2054922799);
    a = md5_ii(a, b, c, d, x[i+ 8], 6 ,  1873313359);
    d = md5_ii(d, a, b, c, x[i+15], 10, -30611744);
    c = md5_ii(c, d, a, b, x[i+ 6], 15, -1560198380);
    b = md5_ii(b, c, d, a, x[i+13], 21,  1309151649);
    a = md5_ii(a, b, c, d, x[i+ 4], 6 , -145523070);
    d = md5_ii(d, a, b, c, x[i+11], 10, -1120210379);
    c = md5_ii(c, d, a, b, x[i+ 2], 15,  718787259);
    b = md5_ii(b, c, d, a, x[i+ 9], 21, -343485551);

    a = safe_add(a, olda);
    b = safe_add(b, oldb);
    c = safe_add(c, oldc);
    d = safe_add(d, oldd);
  }

  retArray[0] = a;
  retArray[1] = b;
  retArray[2] = c;
  retArray[3] = d;

  return retArray;

}


/*
 * Calculate the HMAC-MD5, of a key and some data
 */
unsigned int *core_hmac_md5(char *key, char *data)
{
  unsigned int ipad[16], opad[16];
  unsigned int iNew[32];
  unsigned int *bkey;
  //unsigned int hashRet[4];
  int iRetLen;
  unsigned int *hash;
  int i;

  bkey = str2binl(key, &iRetLen);
  if(iRetLen > 16)
  	bkey = core_md5(bkey, strlen(key) * chrsz);

  for(i = 0; i < 16; i++)
  {
    ipad[i] = bkey[i] ^ 0x36363636;
    opad[i] = bkey[i] ^ 0x5C5C5C5C;
  }

  bkey = str2binl(data, &iRetLen);

  memset(iNew, 0, 32*4);
  for(i = 0; i < 16+iRetLen ; i++)
  {
    if (i<16)
	iNew[i] = ipad[i];
    else
	iNew[i] = bkey[i-16];
  }
  hash = core_md5(iNew, 512 + strlen(data) * chrsz);

  memset(iNew, 0, 32*4);
  for(i = 0; i < 16+4; i++)
  {
    if (i<16)
		iNew[i] = opad[i];
    else
	iNew[i] = hash[i-16];
  }
  bkey = core_md5(iNew, 512 + 128);

  return bkey;
}


/*
 * These are the functions you'll usually want to call
 * They take string arguments and return either hex or base-64 encoded strings
 */
char *hex_md5(char *s )
{
	int iRetLen;
	char *retstr;
	unsigned int *bin;
	//int i;

	bin = str2binl(s, &iRetLen);
	retstr = binl2hex(core_md5(bin, strlen(s) * chrsz), 4);
	DES_DBG(( "\n hex_md5=[%s] \n", retstr ));
	return retstr;
}

char *b64_md5(char *s)
{
	int iRetLen;
	return binl2b64(core_md5(str2binl(s, &iRetLen), strlen(s) * chrsz), 4);
}

char *str_md5(char *s)
{
	int iRetLen;
	return binl2str(core_md5(str2binl(s, &iRetLen), strlen(s) * chrsz), 4);
}

char *hex_hmac_md5( char *key, char *data )
{
	return binl2hex(core_hmac_md5(key, data), 4);
}

char *b64_hmac_md5( char *key, char *data )
{
	return binl2b64(core_hmac_md5(key, data), 4);
}

char *str_hmac_md5( char *key, char *data )
{
	return binl2str(core_hmac_md5(key, data), 4);
}

/*
 * Perform a simple self-test to see if the VM is working
 */
int md5_vm_test1( void )
{
  char *cTemp;

  cTemp = hex_md5("abc") ;

  if (!strcmp(cTemp , "900150983cd24fb0d6963f7d28e17f72" ) )
  	return 1;
  else
  	return 0;
}

int md5_vm_test( void )
{
  char *cTemp;

  cTemp = hex_md5("ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ") ;

  if (!strcmp(cTemp , "3603fe1c214a9a000c7cb5b8747a94c7" ) )
  	return 1;
  else
  	return 0;
}


int hmac_md5_test( void )
{
  unsigned int *iTemp;
  //char uctemp[16+1];
  //int i;
/*
  memset( uctemp,0,16+1 );
  for ( i=0;i<16;i++ )
  	uctemp[i] = 0x0b;
*/
  iTemp = core_hmac_md5("Jefe", "what do ya want for nothing?") ;
//  iTemp = core_hmac_md5(uctemp, "Hi There") ;

  if ( iTemp[0] == 0x3e780c75 &&\
  	iTemp[1] == 0x03b5b06a &&\
  	iTemp[2] == 0x316ea8ea &&\
  	iTemp[3] == 0x38b75d0a )
  	return 1;
  else
  	return 0;
}

/*
 * Perform a simple self-test to see if the VM is working
 */
char *Crypt_MD5( const char *key, char *data )
{
  return b64_hmac_md5( (char*)key, data );
}


static char itoa64[] =/* 0 ... 63 => ascii - 64 */"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
void _crypt_to64(char *s, unsigned long v, int n)
{
	//int i;

	while (--n >= 0)
	{
		*s++ = itoa64[v&0x3f];
		v >>= 6;
	}
}

char *crypt_md5(char *pw, const char *salt)
{
	unsigned long l;
	int sl, pl;
	int i;
	char *cTemp;
  	static char cPlainPW[120];
	int iKeyLen;
  	static char cPlainPW1[120];
	int iKeyLen1;
	int iLengthTemp;
	unsigned int *bkey;
	int iRetLen;
	char *final_p;

	static const char *sp, *ep;
	static char passwd[120], *p;
	static const char *magic = "$1$";

	/* Refine the Salt first */
	sp = salt;

	/* If it starts with the magic string, then skip that */
	if(!strncmp(sp, magic, strlen(magic)))
		sp += strlen(magic);

	/* It stops at the first '$', max 8 chars */
	for(ep = sp; *ep && *ep != '$' && ep < (sp + 8); ep++)
		continue;

	/* get the length of the true salt */
	sl = ep - sp;

	memset ( cPlainPW, 0, 120 );
	memcpy( cPlainPW, pw, strlen(pw) );
	memcpy( cPlainPW+strlen(pw), magic, strlen(magic) );
	memcpy( cPlainPW+strlen(pw)+strlen(magic), sp, sl );
	iKeyLen = strlen(pw)+strlen(magic)+sl;


	/* Then just as many characters of the MD5(pw,salt,pw) */
	memset ( cPlainPW1, 0, 120 );
	memcpy( cPlainPW1, pw, strlen(pw) );
	memcpy( cPlainPW1+strlen(pw), sp, sl );
	memcpy( cPlainPW1+strlen(pw)+sl, pw, strlen(pw) );
	iKeyLen1 = strlen(pw)+sl+strlen(pw);

	bkey= core_md5(str2binl(cPlainPW1, &iRetLen), iKeyLen1*chrsz);
	final_p = (char*)bkey;

	for(pl = (int)strlen(pw); pl > 0; pl -= 16)
	{
		memcpy( cPlainPW+iKeyLen, final_p, (pl > 16 ? 16 : pl ));
	}
	iKeyLen += (pl > 16 ? 16 : pl );

	/* Don't leave anything around in vm they could use. */
	memset(final_p, 0, sizeof(final_p));

	/* Then something really weird... */
	cTemp = cPlainPW+iKeyLen;
	for (i = strlen(pw); i; i >>= 1)
	{
		if(i & 1)
			memcpy( cTemp, final_p, 1 );
		else
			memcpy( cTemp, pw, 1 );

		cTemp++;
		iKeyLen++;
	}

	/* Now make the output string */
	memset ( passwd, 0, 120 );
	strcpy(passwd, magic);
	strncat(passwd, sp, sl);
	strcat(passwd, "$");

	bkey= core_md5(str2binl(cPlainPW, &iRetLen), iKeyLen*chrsz);
	final_p = (char *)bkey;

	/*
	 * and now, just to make sure things don't run too fast
	 * On a 60 Mhz Pentium this takes 34 msec, so you would
	 * need 30 seconds to build a 1000 entry dictionary...
	 */

	for(i = 0; i < 1000; i++) {

		memset( cPlainPW1, 0, 120 );
		cTemp = cPlainPW1;
		iLengthTemp = 0;

		if(i & 1)
		{
			memcpy( cTemp, pw, strlen(pw));
			iLengthTemp += strlen(pw);
		}
		else
		{
			memcpy( cTemp, final_p, 16);
			iLengthTemp += 16;
		}

		if(i % 3)
		{
			memcpy( cTemp, sp, sl);
			iLengthTemp += sl;
		}
		if(i % 7)
		{
			memcpy( cTemp, pw, strlen(pw));
			iLengthTemp += strlen(pw);
		}

		if(i & 1)
		{
			memcpy( cTemp, final_p, 16);
			iLengthTemp += 16;
		}
		else
		{
			memcpy( cTemp, pw, strlen(pw));
			iLengthTemp += strlen(pw);
		}

		bkey = core_md5(str2binl(cTemp, &iRetLen), iLengthTemp*chrsz);
		final_p = (char*)bkey;

	}


	p = passwd + strlen(passwd);

	l = (final_p[ 0]<<16) | (final_p[ 6]<<8) | final_p[12];
	_crypt_to64(p, l, 4); p += 4;
	l = (final_p[ 1]<<16) | (final_p[ 7]<<8) | final_p[13];
	_crypt_to64(p, l, 4); p += 4;
	l = (final_p[ 2]<<16) | (final_p[ 8]<<8) | final_p[14];
	_crypt_to64(p, l, 4); p += 4;
	l = (final_p[ 3]<<16) | (final_p[ 9]<<8) | final_p[15];
	_crypt_to64(p, l, 4); p += 4;
	l = (final_p[ 4]<<16) | (final_p[10]<<8) | final_p[ 5];
	_crypt_to64(p, l, 4); p += 4;
	l = final_p[11];
	_crypt_to64(p, l, 2); p += 2;
	*p = '\0';

	return (passwd);
}


/* md5.c - an implementation of the MD5 algorithm and MD5 crypt */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2000, 2001  Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* See RFC 1321 for a description of the MD5 algorithm.
 */


#define cpu_to_le32(x) (x)
#define le32_to_cpu(x) cpu_to_le32(x)


#ifndef UINT4
typedef unsigned int UINT4;
#endif


/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x >> (32 - (n)))))

static UINT4 initstate[4] =
{
  0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476
};

static char s1[4] = {  7, 12, 17, 22 };
static char s2[4] = {  5,  9, 14, 20 };
static char s3[4] = {  4, 11, 16, 23 };
static char s4[4] = {  6, 10, 15, 21 };

static UINT4 T[64] =
{
  0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
  0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
  0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
  0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
  0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
  0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
  0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
  0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
  0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
  0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
  0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
  0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
  0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
  0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
  0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
  0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

static const char *b64t =
"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static UINT4 state[4];
static unsigned int length;
static unsigned char buffer[64];

//static void md5_transform (const unsigned char block[64])
static void md5_transform (const unsigned char *block)
{
  int i, j;
  UINT4 a,b,c,d,tmp;
  const UINT4 *x = (UINT4 *) block;

  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];

  /* Round 1 */
  for (i = 0; i < 16; i++)
    {
      tmp = a + F (b, c, d) + le32_to_cpu (x[i]) + T[i];
      tmp = ROTATE_LEFT (tmp, s1[i & 3]);
      tmp += b;
      a = d; d = c; c = b; b = tmp;
    }
  /* Round 2 */
  for (i = 0, j = 1; i < 16; i++, j += 5)
    {
      tmp = a + G (b, c, d) + le32_to_cpu (x[j & 15]) + T[i+16];
      tmp = ROTATE_LEFT (tmp, s2[i & 3]);
      tmp += b;
      a = d; d = c; c = b; b = tmp;
    }
  /* Round 3 */
  for (i = 0, j = 5; i < 16; i++, j += 3)
    {
      tmp = a + H (b, c, d) + le32_to_cpu (x[j & 15]) + T[i+32];
      tmp = ROTATE_LEFT (tmp, s3[i & 3]);
      tmp += b;
      a = d; d = c; c = b; b = tmp;
    }
  /* Round 4 */
  for (i = 0, j = 0; i < 16; i++, j += 7)
    {
      tmp = a + I (b, c, d) + le32_to_cpu (x[j & 15]) + T[i+48];
      tmp = ROTATE_LEFT (tmp, s4[i & 3]);
      tmp += b;
      a = d; d = c; c = b; b = tmp;
    }

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
}

static void md5_init(void)
{
  memcpy ((char *) state, (char *) initstate, sizeof (initstate));
  length = 0;
}

static void md5_update (const char *input, int inputlen)
{
  int buflen = length & 63;
  length += inputlen;
  if (buflen + inputlen < 64)
    {
      memcpy (buffer + buflen, input, inputlen);
      buflen += inputlen;
      return;
    }

  memcpy (buffer + buflen, input, 64 - buflen);
  md5_transform (buffer);
  input += 64 - buflen;
  inputlen -= 64 - buflen;
  while (inputlen >= 64)
    {
      md5_transform ((const unsigned char*)input);
      input += 64;
      inputlen -= 64;
    }
  memcpy (buffer, input, inputlen);
  buflen = inputlen;
}

static unsigned char *md5_final()
{
  int i, buflen = length & 63;

  buffer[buflen++] = 0x80;
  memset (buffer+buflen, 0, 64 - buflen);
  if (buflen > 56)
    {
      md5_transform (buffer);
      memset (buffer, 0, 64);
      buflen = 0;
    }

  *(UINT4 *) (buffer + 56) = cpu_to_le32 (8 * length);
  *(UINT4 *) (buffer + 60) = 0;
  md5_transform (buffer);

  for (i = 0; i < 4; i++)
    state[i] = cpu_to_le32 (state[i]);
  return (unsigned char *) state;
}

/* If CHECK is true, check a password for correctness. Returns 0
   if password was correct, and a value != 0 for error, similarly
   to strcmp.
   If CHECK is false, crypt KEY and save the result in CRYPTED.
   CRYPTED must have a salt.  */
int Crypt_pwd (char *key, char *crypted, int check)
{
  int keylen = strlen (key);
  char *salt = crypted + 3; /* skip $1$ header */
  char *p;
  int saltlen;
  int i, n;
  unsigned char alt_result[16];
  unsigned char *digest;

  if (check)
    {
      /* If our crypted password isn't 3 chars, then it can't be md5
	 crypted. So, they don't match.  */
      if (strlen(crypted) <= 3)
	return 1;

      saltlen = strstr (salt, "$") - salt;
    }
  else
    {
      char *end = strstr (salt, "$");
      if (end && end - salt < 8)
	saltlen = end - salt;
      else
	saltlen = 8;

      salt[saltlen] = '$';
    }

  md5_init ();
  md5_update (key, keylen);
  md5_update (salt, saltlen);
  md5_update (key, keylen);
  digest = md5_final ();
  memcpy (alt_result, digest, 16);

  memcpy ((char *) state, (char *) initstate, sizeof (initstate));
  length = 0;
  md5_update (key, keylen);
  md5_update (crypted, 3 + saltlen); /* include the $1$ header */
  for (i = keylen; i > 16; i -= 16)
    md5_update ((const char*)alt_result, 16);
  md5_update ((const char*)alt_result, i);

  for (i = keylen; i > 0; i >>= 1)
    md5_update (key + ((i & 1) ? keylen : 0), 1);
  digest = md5_final ();

  for (i = 0; i < 1000; i++)
    {
      memcpy (alt_result, digest, 16);

      memcpy ((char *) state, (char *) initstate, sizeof (initstate));
      length = 0;
      if ((i & 1) != 0)
	md5_update (key, keylen);
      else
	md5_update ((const char*)alt_result, 16);

      if (i % 3 != 0)
	md5_update (salt, saltlen);

      if (i % 7 != 0)
	md5_update (key, keylen);

      if ((i & 1) != 0)
	md5_update ((const char*)alt_result, 16);
      else
	md5_update (key, keylen);
      digest = md5_final ();
    }

  p = salt + saltlen + 1;
  for (i = 0; i < 5; i++)
    {
      unsigned int w =
	digest[i == 4 ? 5 : 12+i] | (digest[6+i] << 8) | (digest[i] << 16);
      for (n = 4; n-- > 0;)
	{
	  if (check)
	    {
	      if (*p++ != b64t[w & 0x3f])
		return 1;
	    }
	  else
	    {
	      *p++ = b64t[w & 0x3f];
	    }

	  w >>= 6;
	}
    }
  {
    unsigned int w = digest[11];
    for (n = 2; n-- > 0;)
      {
	if (check)
	  {
	    if (*p++ != b64t[w & 0x3f])
	      return 1;
	  }
	else
	  {
	    *p++ = b64t[w & 0x3f];
	  }

	w >>= 6;
      }
  }

  if (! check)
    *p = '\0';

  return 0;
}


#endif



