#ifndef	__VFD_H__
#define	__VFD_H__

#ifndef BOOL
#define BOOL int
#endif

#ifdef YW_CONFIG_VFD
//-------------------VFD Ïà¹Øº¯Êý¡¡SH
extern void YWVFD_ROOTFS_P(int pro_rate);
extern void YWVFD_U_BOOT(void);
extern void YWVFD_U_Kernel(void);
extern void YWVFD_Warn(char*  str);
extern void YWVFD_U_MENU(void);
extern void YWVFD_BOOT_P(int pro_rate);
extern void YWVFD_KERNEL_P(int pro_rate);
extern void YWVFD_Print(char* str);
extern int YWVFD_GetScanKey(int *key);
extern BOOL YWVFD_HasKeyPress(int *pKey);
extern BOOL YWVFD_HasKeyPressJudge(void);
extern void YWVFD_WaitKeyRelease(void);
extern void YWVFD_WaitKeyPress(int *pKey);
extern BOOL YWVFD_WaitNewKeyPress(int *pKey);
extern BOOL YWVFD_GetScanKeyUntilRelease(int *pKey);
extern void YWVFD_Init(void);
extern int YWVFD_LBD_PowerOff(void);
extern BOOL YWVFD_IsStandbyTypeVfd(void);
#else
#define YWVFD_ROOTFS_P(pro_rate)
#define YWVFD_U_Kernel(void)
#define YWVFD_U_BOOT(void)
#define YWVFD_Warn(str)
#define YWVFD_U_MENU(void)
#define YWVFD_BOOT_P(pro_rate)
#define YWVFD_KERNEL_P(pro_rate)
#define YWVFD_Print(str)
#define YWVFD_GetScanKey(key)
#define YWVFD_HasKeyPress(pKey)
#define YWVFD_HasKeyPressJudge(void)
#define YWVFD_WaitKeyRelease(void)
#define YWVFD_WaitKeyPress(pKey)
#define YWVFD_WaitNewKeyPress(pKey)
#define YWVFD_GetScanKeyUntilRelease(pKey)
#define YWVFD_Init(void)
#define YWVFD_LBD_PowerOff(void)
#define YWVFD_IsStandbyTypeVfd(void)
#endif

#endif
