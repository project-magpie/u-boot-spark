#ifndef _DES_H_
#define _DES_H_

#define CRYPT           0

#define HASH            1

#define UPDATE          2



#define DES_IP              1

#define DES_IP_1            2

#define DES_RIGHT           4

#define DES_HASH            8



#define DES_ECM_CRYPT       0

#define DES_ECM_HASH        DES_HASH

#define DES_ECS2_DECRYPT    (DES_IP | DES_IP_1 | DES_RIGHT)

#define DES_ECS2_CRYPT      (DES_IP | DES_IP_1)



#define COMMAND_OK  0x9000
#define COMMAND_KO  0x9008
#define HASH_KO     0x9040

#define F_EURO_S2       0
#define F_TRIPLE_DES    1

#define SetBit(addr, bit) (addr) |= (1 << (bit))
#define UnsetBit(addr, bit) (addr) &= ~(1<< (bit))
#define TestBit(addr, bit) ((addr) & (1 << bit))


#ifndef byte
typedef unsigned char byte;
#endif
#if 0
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int  u32;
typedef signed int  s32;
#endif


#ifdef __cplusplus

extern "C"

{

#endif		/* __cplusplus */

extern void EuroDes(byte key1[], byte key2[], byte DesMode, byte OperatingMode, byte data[]);

#ifdef __cplusplus

}

#endif		/* __cplusplus */



extern void doPC1_(byte data[]);


extern void des_(byte key[], byte mode, byte data[]);

enum { ECM=0, ECS2=1, ECS3=2 };

#endif



