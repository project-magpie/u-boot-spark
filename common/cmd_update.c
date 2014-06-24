//2008-03-06 LinFan add for update

#include <common.h>
#include <command.h>

#ifdef CONFIG_CMD_UPDATE  //we don't need this now

#define COMMAND_SUCCESS 0
#define COMMAND_FAIL 1

extern int Call_command ( char *cmd, int flag);

//YWDRIVER_MODI D02SH 2009/09/08 add begin
//Description:add for C test
#include <linux/ctype.h>
//YWDRIVER_MODI D02SH 2009/09/08 add end
#define UPDATE_BOOTLOADER_KEY   '0'
#define UPDATE_KERNEL_KEY       '1'
#define UPDATE_ROOTFS_KEY       '2'
#define UPDATE_ESC_KEY          '3'

#define UPDATE_BOOT_KEY         '4'
#define UPDATE_BOOT_KEY_2       'b'
#define UPDATE_BOOT_KEY_3       'B'

#define UPDATE_RESET_KEY        '5'
#define UPDATE_RESET_KEY_2      'r'
#define UPDATE_RESET_KEY_3      'R'

//YWDRIVER_MODI D02SH 2009/09/08 add begin
//Description:add for loader
#define UPDATE_LOADER_KEY       '6'
#define UPDATE_LOADER_KEY_2     'l'
#define UPDATE_LOADER_KEY_3     'L'
//YWDRIVER_MODI D02SH 2009/09/08 add end

#include "vfd.h"

//-----------------------------------
#define DEBUG_UART_UPDATE 1
#if (DEBUG_UART_UPDATE == 1)
    #define PRINTK(x) printf x
#else
    #define PRINTK(x)
#endif

#define ESC 0x1B
#define SPACE ' '
#define ENTER '\r'

#define UPDATE_CFG_MONITOR_LEN      CFG_MONITOR_LEN	/* Reserve  kB for Monitor */
#define UPDATE_CFG_MONITOR_BASE     CFG_MONITOR_BASE
#define UPDATE_CFG_MONITOR_SEC      MONITOR_SECTORS

#define UPDATE_CFG_KERNEL_BASE      CFG_YW_SPARK_KERNEL_BASE  //内核基址
#define UPDATE_CFG_KERNEL_LEN       CFG_YW_SPARK_KERNEL_LEN

#define UPDATE_CFG_USERFS_BASE      CFG_YW_SPARK_USERFS_BASE      //usr root基址
#define UPDATE_CFG_USERFS_LEN       CFG_YW_SPARK_USERFS_LEN

#define UPDATE_LOADFILE_ERROR_BASE 0
enum
{
    UPDATE_LOADFILE_NO_ERROR = UPDATE_LOADFILE_ERROR_BASE,
    UPDATE_LOADFILE_LOADB_ERROR,
    UPDATE_LOADFILE_FILESIZE_ERROR
};

typedef enum get_key_style
{
	UPDATE_MENU_GETKEY,
	UPDATE_WARNING_GETKEY
}get_key_style_e;	//d48zm add

extern int auto_update_flag;
unsigned int filetype = 0;
unsigned int filelen = 0;
unsigned int filelen_recv =0;


#if 1 //没有人维护这一块了
void Waitkey(void)
{
	int c;

    printf ("\n     ##press any key to continue ...\n");
    for (;;)
    {
        if (tstc())           /* we got a key press	*/
        {
            (void) getc();  /* consume input	*/
            break;
        }
//YWDRIVER_MODI 2009/12/28 d48zm add
		else if((0 == YWVFD_GetScanKey(&c)) && (c != 0x00 ))
		{
			break;
		}
//add end
    }
}

void Updatewarning(void)
{
    printf("\n###################################################\n");
    printf("\n             Update Warning\n");
    printf("****************************************************\n"
           "*  ATTENTION!! PLEASE READ THIS NOTICE CAREFULLY!  *\n"
           "****************************************************\n\n"
           "This program  will update your computer's  firmware.\n"
           "Do NOT  reset the  machine,  or do anything that \n"
           "might disrupt functionality.  If this Program fails, \n");
    printf("your computer  might be unusable, and you will \n"
           "need to return your  board for reflashing.If you \n"
           "find this too risky,press (ESC) to return to \n"
           "Updatemenu or switch off your machine now.Otherwise \n "
           "press the (SPACE) key now to start the process\n\n");
    puts ("1. Canceled this update : Please press '1' or ESC or FP 'MENU'\n");
    puts ("2. Continue this update : Please press '2' or SPACE or FP 'OK'\n");
    printf("\n###################################################\n");
    puts ("\n     Please enter your choice : ");
}

void Updatemenu(void)
{
    puts ("\n*******************************************\n");
    puts ("\n       Update Menu V2.0 \n\n");
    puts ("0. Update Uboot  : Please press '0' or FP 'UP'\n");
    puts ("1. Update Kernel  : Please press '1' or FP 'DOWN'\n");
    puts ("2. Update RootFS  : Please press '2' or FP 'LEFT'\n");
    puts ("3. Exit menu     : please press '3' or ESC or FP 'MENU'\n");
    puts ("4. boot          : Please press '4' or 'B' or FP 'OK'\n");
    puts ("5. Reset         : Please press '5' or 'R' or FP 'POWER'\n");
    puts ("6. Loader        : Please press '6' or 'L' or FP 'RIGHT'\n");
    puts ("\n*******************************************\n");
    puts ("\n     Please enter your choice : ");
    YWVFD_U_MENU();
}

static __inline__ int update_abortboot(int bootdelay)
{
	int abort = 0;

	printf("Hit ESC to stop autoboot: %2d ", bootdelay);

#if defined CONFIG_ZERO_BOOTDELAY_CHECK
	/*
	 * Check if key already pressed
	 * Don't check if bootdelay < 0
	 */
	if (bootdelay >= 0) {
		if (tstc()) {	/* we got a key press	*/
// YWDRIVER_MODI by cc 2010/01/20 begin
#if 1
			char	ch;

			ch = getc();
			if(ch == ESC)
			{
				abort = 1;	/* don't auto boot	*/
			}
#else
			(void) getc();  /* consume input	*/
			puts ("\b\b\b 0");
			abort = 1;	/* don't auto boot	*/
#endif
// YWDRIVER_MODI by cc 2010/01/20 end
		}
	}
#endif

	while ((bootdelay > 0) && (!abort)) {
		int i;

		--bootdelay;
		/* delay 100 * 10ms */
		for (i=0; !abort && i<100; ++i) {
			if (tstc()) {	/* we got a key press	*/
				abort  = 1;	/* don't auto boot	*/
				bootdelay = 0;	/* no more delay	*/

				(void) getc();  /* consume input	*/

				break;
			}
			udelay (10000);
		}

		printf ("\b\b\b%2d ", bootdelay);
	}

	putc ('\n');

	return abort;
}

int loadfile( int flag)
{
    char *arg;
    char * s;
    long filesize=0;
    int rcode=0;

    //PRINTK(("\n Now please select the right file to transmit:\n\   (using the kermit protocol)\n\n"));
    printf("\n\n");
    //传送内核
    arg="loadb";
    rcode = Call_command ( arg, flag ) ;
    if (rcode != COMMAND_SUCCESS)
    {
        printf("\n     Transmit File failed.\n");
        YWVFD_Warn("T-File:E");
        return UPDATE_LOADFILE_LOADB_ERROR;
    }

    //传送长度为零，则失败返回。
    s = getenv ("filesize");
    filesize = s ? simple_strtol(s, NULL, 16) : 0;

    if (0==filesize)
    {
        printf("\n Transmit filesize is 0, please check your choiced file\n");
        YWVFD_Warn("S-File:e");
        return UPDATE_LOADFILE_FILESIZE_ERROR;
    }

    return UPDATE_LOADFILE_NO_ERROR;
}

int exec_load_erase_write_file(int flag,int key)
{
        //char c;
        char *arg;
        int rcode=0;
        //int i;
        char *s;
        filelen_recv = 0;
// zww define the progress of upgrade
// 1, start translate kernel 15%
// 2, translate kernel success and start erase kernel 60%
// 3, erase kernel success and start translate fs 80%
// 4, translate fs success and start erase fs 90%
// 5. erase fs succss 99%
// 6, 100%
        switch (key)
        {
            case KERNEL_UPDATE_CHAR:   //进入升级kernel 菜单
                YWVFD_KERNEL_P(15);
            break;

            case BOOT_UPDATE_CHAR:   //进入bootloader内核菜单

                YWVFD_BOOT_P(15);
            break;
            case FS_UPDATE_CHAR:   //进入fs内核菜单
                YWVFD_ROOTFS_P(15);
            break;

            default:
            break;
        }

        rcode = loadfile(flag);
        if (rcode == UPDATE_LOADFILE_NO_ERROR)
        {
//YWDRIVER_MODI D02SH 2009/01/09 add begin
#if 0
            if (filelen_recv != filelen)
            {
                PRINTK(("filelen_recv len error\n"));
                printf("filelen_recv=0x%x, filelen =0x%x\n",filelen_recv, filelen);
                  //vfd interrupt info display
                return  -4;
            }

#endif
//YWDRIVER_MODI D02SH 2009/01/09 add end


            switch (key)
            {
                case KERNEL_UPDATE_CHAR:   //进入升级kernel 菜单

                      YWVFD_KERNEL_P(60);

                    s = getenv ("kernel_len");
                    if (s==NULL)
                    {
                        setenv ("kernel_len", UPDATE_CFG_KERNEL_LEN);  /* set kernel_sec variable */
                    }
                    s = getenv ("kernel_base");
                    if (s==NULL)
                    {
                        setenv ("kernel_base", XSTR(UPDATE_CFG_KERNEL_BASE));  /* set kernel_base variable */
                    }
                    arg="nand erase $kernel_base $kernel_size;nand write.i $load_addr $kernel_base $filesize";
                 //   PRINTK(("arg=%s\n",arg));
                    rcode = Call_command ( arg, flag ) ;
                    YWVFD_KERNEL_P(99);

                    if (rcode != COMMAND_SUCCESS)
                    {
                        PRINTK(("Call_command=%d\n",rcode));
                        return -2;
                    }
                break;

                case BOOT_UPDATE_CHAR:   //enter bootloader upgrade menu

                    YWVFD_BOOT_P(60);
                    arg="nand erase $monitor_base $monitor_len;nand write.i $load_addr $monitor_base $filesize";
                    rcode = Call_command ( arg, flag ) ;
                    YWVFD_BOOT_P(99);

                    if (rcode != COMMAND_SUCCESS)
                    {
                        PRINTK(("Call_command=%d\n",rcode));
                        return -2;
                    }

                break;


                case FS_UPDATE_CHAR:   //enter fs upgrade menu
                    YWVFD_ROOTFS_P(60);

                    s = getenv ("userfs_len");
                    if (s==NULL)
                    {
                        setenv ("userfs_len", UPDATE_CFG_USERFS_LEN);  /* set kernel_sec variable */
                    }
                    s = getenv ("userfs_base");
                    if (s==NULL)
                    {
                        setenv ("userfs_base", XSTR(UPDATE_CFG_USERFS_BASE));  /* set kernel_base variable */
                    }
                   arg="nand erase $userfs_base userfs_len;nand write.yaffs2 $load_addr $userfs_base $filesize";
                   // PRINTK(("arg=%s\n",arg));
                    rcode = Call_command ( arg, flag ) ;
                    YWVFD_ROOTFS_P(99);

                    if (rcode != COMMAND_SUCCESS)
                    {
                        PRINTK(("Call_command=%d\n",rcode));

                        return -2;
                    }
                break;

                default:
                    PRINTK(("default flag=%d\n",flag));
                return  -3;
            }

            if (filetype != ALL_UPDATE_CHAR)
            {
                //vfd  info 100%
            }

        }
        else
        {
            PRINTK(("load fail flag\n"));
            return  -1;
        }
        return  0;
}

//YWDRIVER_MODI d48zm add
int get_input_key(char *keyp, get_key_style_e style)
{
	int ret = 0;
	int c = 0;

	if (tstc())
	{
        *keyp = getc();		/* read character		*/
	}
#ifdef YW_CONFIG_VFD
    else if (YWVFD_GetScanKeyUntilRelease(&c))
    {
		if (style == UPDATE_WARNING_GETKEY)
		{
			if (c == 0x80)
			{
				*keyp = '1';
			}
			else if (c == 0x08)
			{
				*keyp = '2';
			}
			else
			{
				ret = -1;
			}
		}
		else if (style == UPDATE_MENU_GETKEY)
		{
			switch (c)
	        {
	            case 0x02:
	            {
	                *keyp = UPDATE_ROOTFS_KEY;
	                break;
	            }
	            case 0x04:
	            {
	                *keyp = UPDATE_BOOTLOADER_KEY;
	                break;
	            }
	            case 0x08:
	            {
	                *keyp = UPDATE_BOOT_KEY;
	                break;
	            }
	            case 0x10:
	            {
	                *keyp = UPDATE_LOADER_KEY;
	                break;
	            }
	            case 0x20:
	            {
	                *keyp = UPDATE_KERNEL_KEY;
	                break;
	            }
	            case 0x40:
	            {
	                *keyp = UPDATE_RESET_KEY;
	                break;
	            }
	            case 0x80:
	            {
	                *keyp = UPDATE_ESC_KEY;
	                break;
	            }
	            default :
	            {
	                //*keyp = ' ';
	                break;
	            }
	        }
		}
		else
		{
			ret = -1;
		}
    }
#endif
    else
    {
      	ret = -1;
    }

	return ret;
}
//add end

int do_update (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
    int i = 0;
    char c ;
    char *arg;
    int rcode=0;
    char *s;
    int bootdelay;

    PRINTK(("\n     do_update auto_update_flag=%d\n",auto_update_flag));

    filetype = 0;

    if (auto_update_flag == 1)
    {
        c = getc();		/* read character		*/
        if (c != START_UPDATE_CHAR)
        {
            arg = getenv ("bootcmd");
            Call_command ( arg, flag ) ;
            return 0;
        }

        while (c == START_UPDATE_CHAR)
        {
                c = getc();		/* read character		*/
                putc(c);
                if (i++ > 0xffff)
                {
                    break;
                }
        }

       if ((c != BOOT_UPDATE_CHAR)  && (c != KERNEL_UPDATE_CHAR) && \
            (c != FS_UPDATE_CHAR)  && (c != ALL_UPDATE_CHAR) )
        {
            arg = getenv ("bootcmd");
            Call_command ( arg, flag ) ;
            return 0;
        }

        //putc(READY_UPDATE_CHAR);//send to server 'r'
        putc(c); //将c返回
        filetype = c;


       if (filetype != ALL_UPDATE_CHAR)
        {
           if ( exec_load_erase_write_file(flag,filetype)  == 0)
            {
                putc(FINISH_UPDATE_CHAR);//send to server 'o'
                printf("\n     Update  done.\n");
                YWVFD_Print("P-FINISH");
            }
           else
            {
//YWDRIVER_MODI D02SH 2009/01/09 add begin
                putc(FAIL_UPDATE_CHAR);//send to server 'E'
//YWDRIVER_MODI D02SH 2009/01/09 add end
                switch (filetype)
                {
                    case KERNEL_UPDATE_CHAR:
                        YWVFD_Warn("F-KERNEL");
                        printf("\n     Update kernel  fail.\n");
                    break;
                    case BOOT_UPDATE_CHAR:
                        YWVFD_Warn("  F-BOOT");
                        printf("\n     Update boot  fail.\n");
                    break;
                    case FS_UPDATE_CHAR:
                        YWVFD_Warn("  F-FS");
                        printf("\n     Update root   fail.\n");
                    break;
                    default:
                    break;
                }

            }
        }
       else
        {

//YWDRIVER_MODI D02SH 2009/01/09 add begin
#if 0
                 //putc(BOOT_UPDATE_CHAR);//send to server 'u'
#else
               putc(KERNEL_UPDATE_CHAR);//send to server 'u'
#endif
//YWDRIVER_MODI D02SH 2009/01/09 add end
                if ( exec_load_erase_write_file(flag,KERNEL_UPDATE_CHAR) < 0)
                {
                    putc(FAIL_UPDATE_CHAR);//send to server 'E'
                    YWVFD_Warn("F-KERNEL");
                    printf("\n     Update Kernel fail.\n");

                }
                else
                {
                    putc(FS_UPDATE_CHAR);//send to server 'k'

                    if ( exec_load_erase_write_file(flag,FS_UPDATE_CHAR) < 0)
                    {
                        putc(FAIL_UPDATE_CHAR);//send to server 'E'
                        YWVFD_Warn("  F-FS");
                        printf("\n     Update fs  fail.\n");

                    }
                    else
                    {
//YWDRIVER_MODI D02SH 2009/01/09 add begin
#if 0
                        //putc(FS_UPDATE_CHAR);//send to server 'f'

#else
                       putc(FINISH_UPDATE_CHAR);//send to server 'o'
#endif
//YWDRIVER_MODI D02SH 2009/01/09 add end
                        YWVFD_Print("P-FINISH");

                        printf("\n     Update all done.\n");
                    }
                }
        }

        arg = getenv ("bootcmd");
        rcode = Call_command ( arg, flag ) ;
        if (rcode != COMMAND_SUCCESS)
        {
            PRINTK (("\n     Couldn't startup kernel.\n"));
            YWVFD_Print("BOOTFAIL");
          //  Waitkey();
        }
        else
        {
            YWVFD_Print("BOOTSUCC");
        }

    }
    #if 0
   else
    {
        s = getenv ("bootdelay");
        bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

        Updatemenu();
        YWVFD_WaitKeyRelease();

        for (;;)
        {
   			if(0!=get_input_key(&c, UPDATE_MENU_GETKEY))
   			{
				continue;
			}

            switch (c)
            {
                case UPDATE_KERNEL_KEY:   //进入升级内核菜单

                        putc (c);
                        putc ('\n');

                        Updatewarning();
                        do
                        {
                            char x;
							while (0!=get_input_key(&x, UPDATE_WARNING_GETKEY));

                            if ((x == SPACE) || (x=='2'))
                            {
                                puts ("SPACE\n");

                                rcode = loadfile(flag);
                                if (rcode != UPDATE_LOADFILE_NO_ERROR)
                                {
                                    printf("\n     loadfile error: %2d.\n",rcode);
                                    Waitkey();
                                    break;
                            	 }

                            	char * s;
                                s = getenv ("kernel_sec");
                                if (s==NULL)
                                {
                                    setenv ("kernel_sec", UPDATE_CFG_KERNEL_SEC);  /* set kernel_sec variable */
                                }
                                s = getenv ("kernel_base");
                                if (s==NULL)
                                {
                                    setenv ("kernel_base", XSTR(UPDATE_CFG_KERNEL_BASE));  /* set kernel_base variable */
                                }
                                arg="erase $kernel_sec;cp.b $load_addr $kernel_base $filesize";
                                rcode = Call_command ( arg, flag ) ;

                                if (rcode != COMMAND_SUCCESS)
                                {
                                    puts ("\n     Update Kernel failed.\n");

                                    Waitkey();

                                    break;
                                }
                                else
                                {
                                    printf("\n     Update Kernel done.\n");
                                }

                                printf("\n     Now will startup Kernel.\n");
                                if(update_abortboot(bootdelay)==0)
                                {
                                    arg = getenv ("bootcmd");
                                    rcode = Call_command ( arg, flag ) ;
                                    if (rcode != COMMAND_SUCCESS)
                                    {
                                        puts ("\n     Couldn't startup kernel.\n");

                                        Waitkey();

                                        break;
                                    }
                                }

                                break;
                            }

                            if ((x == ESC) || ( x=='1'))
                            {
                                puts ("ESC\n");
                                printf("\n      You had pressed ESC to cancel this update.\n");
                                break;
                            }
                        } while (1);

                        Updatemenu();

                    break;

                case UPDATE_BOOTLOADER_KEY:   //进入bootloader内核菜单

                    putc (c);
                    putc ('\n');

                    Updatewarning();

                    do
                    {
                        char x;
                        while (0!=get_input_key(&x, UPDATE_WARNING_GETKEY));

                        if ((x == SPACE) || (x=='2'))
                        {
                            puts ("SPACE\n");

                            rcode = loadfile(flag);
                            if (rcode != UPDATE_LOADFILE_NO_ERROR)
                            {
                                printf("\n     loadfile error: %2d.\n",rcode);
                                Waitkey();
                                break;
                        	 }

                            arg="protect off $monitor_sec;erase $monitor_sec;cp.b $load_addr $monitor_base $monitor_len;protect on $monitor_sec";
                            rcode = Call_command ( arg, flag ) ;
                            if (rcode != COMMAND_SUCCESS)
                            {
                                puts ("\n     Update uboot failed.\n");
                                Waitkey();
                                break;
                            }
                            else
                            {
                                printf("\n     Update uboot done.\n");
                            }

                            puts ("\n     Now will reset the board:\n");

                            if(update_abortboot(bootdelay)==0)
                            {
                                arg="reset";
                                rcode=Call_command ( arg, flag ) ;
                                if (rcode != COMMAND_SUCCESS)
                                {
                                    puts ("\n     Couldn't soft reset board,please press button to reset.\n");

                                    Waitkey();

                                    break;
                                }
                            }

                            break;
                        }
                        if ((x == ESC) || ( x=='1'))
                        {
                            puts ("ESC\n");
                            printf("\n      You had pressed ESC to cancel this update.\n");
                            break;
                        }
                    } while (1);

                    Updatemenu();

                break;


                case UPDATE_ROOTFS_KEY:   //进入bootloader内核菜单

                    putc (c);
                    putc ('\n');

                    Updatewarning();

                    do
                    {
                        char x;
                        while (0!=get_input_key(&x, UPDATE_WARNING_GETKEY));

                        if ((x == SPACE) || (x=='2'))
                        {
                            puts ("SPACE\n");

                            rcode = loadfile(flag);
                            if (rcode != UPDATE_LOADFILE_NO_ERROR)
                            {
                                printf("\n     loadfile error: %2d.\n",rcode);
                                Waitkey();
                                break;
                        	 }
                        	 printf("\n     load file ok\n");
                        	char * s;
                            s = getenv ("rootfs_sec");
                            if (s==NULL)
                            {
                                setenv ("rootfs_sec", UPDATE_CFG_ROOTFS_SEC);  /* set kernel_sec variable */
                            }
                            s = getenv ("rootfs_base");
                            if (s==NULL)
                            {
                                setenv ("rootfs_base", XSTR(UPDATE_CFG_ROOTFS_BASE));  /* set kernel_base variable */
                            }
                            arg="erase $rootfs_sec; cp.b $load_addr $rootfs_base $filesize";
                             printf("\n rootusr arg=%s\n",arg);
                            rcode = Call_command ( arg, flag ) ;

                            if (rcode != COMMAND_SUCCESS)
                            {
                                puts ("\n     Update Rootfs failed.\n");

                                Waitkey();

                                break;
                            }
                            else
                            {
                                printf("\n     Update rootfs done.\n");
                            }

                            printf("\n     Now will startup rootfs.\n");
                            if(update_abortboot(bootdelay)==0)
                            {
                                arg = getenv ("bootcmd");
                                rcode = Call_command ( arg, flag ) ;
                                if (rcode != COMMAND_SUCCESS)
                                {
                                    puts ("\n     Couldn't startup rootfs.\n");

                                    Waitkey();

                                    break;
                                }
                            }

                            break;
                        }
                        if ((x == ESC) || ( x=='1'))
                        {
                            puts ("ESC\n");
                            printf("\n      You had pressed ESC to cancel this update.\n");
                            break;
                        }
                    } while (1);

                    Updatemenu();

                break;


                case  UPDATE_RESET_KEY:
                case  UPDATE_RESET_KEY_2:
                case  UPDATE_RESET_KEY_3:

                    putc (c);
                    putc ('\n');
                    puts ("\n     Now will reset the board:\n");

                    arg="reset";
                    rcode=Call_command ( arg, flag ) ;

                    if (rcode != COMMAND_SUCCESS)
                    {
                        puts ("\n     Couldn't soft reset board,please press button to reset.\n");

                        Waitkey();
                    }

                    Updatemenu();

                    break;

                case  UPDATE_BOOT_KEY:
                case  UPDATE_BOOT_KEY_2:
                case  UPDATE_BOOT_KEY_3:

                    putc (c);
                    putc ('\n');
                    puts ("\n     Now will startup kernel:\n\n");

                    arg="bootd";
                    rcode=Call_command ( arg, flag ) ;
                    if (rcode != COMMAND_SUCCESS)
                    {
                        puts ("\n     Couldn't startup kernel.\n");

                        Waitkey();

                    }

                    Updatemenu();

                    break;
//YWDRIVER_MODI D02SH 2009/09/08 add begin
//Description:add for loader
                case  UPDATE_LOADER_KEY:
                case  UPDATE_LOADER_KEY_2:
                case  UPDATE_LOADER_KEY_3:
                    arg = getenv ("bootloader");
                    rcode = Call_command ( arg, flag ) ;
                    if (rcode != COMMAND_SUCCESS)
                    {
                        puts ("\n     Couldn't startup kernel.\n");
                        YWVFD_Warn("L-start");
                        Waitkey();
                    }
                    Updatemenu();
                    break;
//YWDRIVER_MODI D02SH 2009/09/08 add end
                case UPDATE_ESC_KEY:
                case ESC:   /* ESC */

                    puts ("ESC\n");
                    printf("\n      you had pressed ESC to exit update menu. \n");
                    printf("\n      you can recall this menu by using 'update' command. \n\n");

                    return (-1);

                case SPACE:   /* SPACE */

                    puts ("SPACE\n");
                    Updatemenu();

                    break;
                case ENTER:   /* ENTER */

                    puts ("ENTER\n");
                    Updatemenu();

                    break;

                default:
                    //putc (c);
                    continue;
            }
        }
    }
   #endif
	return 0;
}

U_BOOT_CMD(
	update,	1,	1,	do_update,
	"update  - update uboot or kernel\n",
	NULL
);
#endif
// YWDRIVER_MODI sqy 2008/12/2 begin
int do_updaterootfs (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    #if 1
    int rcode = 0;
    char *path;
    char *arg=NULL;

    char *s;
    int bootdelay;

    YWVFD_U_Kernel();

    s = getenv ("bootdelay");
    bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

    path = getenv ("rootfsnfspath") ;

    if (path == NULL)
    {
		printf ("## Error: \"rootfsnfspath\" is not defined,you must defind it first.\n");
		printf ("## \"rootfsnfspath\" format:NfsServerIP:YourKernelPath.\n");
		printf ("## for example:\"set rootfsnfspath 192.168.3.120:/home/d55sqy/workspace/target\".\n");
		rcode = -1;
        YWVFD_Warn("P-root");
		return rcode;
    }
    printf ("rootfsnfspath: \"%s\" \n",path);

    path = getenv ("rootfs_name") ;

    if (path == NULL)
    {
		printf ("## Error: \"rootfsl_name\" is not defined,you must defind it first.\n");
		printf ("## for example:\"set rootfsl_name  \".\n");
		rcode = -1;
        YWVFD_Warn("N-root");
		return rcode;
    }
    printf ("rootfs_name: \"%s\" \n",path);

    arg="nfs $load_addr $rootfsnfspath/$rootfs_name";

    rcode = Call_command ( arg, flag ) ;

    if (rcode != COMMAND_SUCCESS)
    {
        puts ("\n     nfs root failed.\n");
        YWVFD_Warn("NFS:e");
        return (-1);
    }
    else
    {
        printf("\n     nfs root done.\n\n");
    }

    //char * s;
    s = getenv ("rootfs_sec");
    if (s==NULL)
    {
        setenv ("rootfs_sec", UPDATE_CFG_ROOTFS_SEC);  /* set kernel_sec variable */
    }
    s = getenv ("rootfs_base");
    if (s==NULL)
    {
        setenv ("rootfs_base", XSTR(UPDATE_CFG_ROOTFS_BASE));  /* set kernel_base variable */
    }

    arg="erase $rootfs_sec;cp.b $load_addr $rootfs_base $filesize";
    rcode = Call_command ( arg, flag ) ;

    if (rcode != COMMAND_SUCCESS)
    {
        puts ("\n     Update root failed.\n");
        YWVFD_Warn("U-root:e");
        YWVFD_Warn("-root:e");
        YWVFD_Warn("root:e");
        YWVFD_Warn("UP-R KO");
        return (-1);
    }
    else
    {
        printf("\n     Update root done.\n");
    }
    printf("\n     Now will startup Kernel.\n");
    if(update_abortboot(bootdelay)==0)
    {
        arg = getenv ("bootcmd");
        rcode = Call_command ( arg, flag ) ;
        if (rcode != COMMAND_SUCCESS)
        {
            puts ("\n     Couldn't startup kernel.\n");

            YWVFD_Warn("C-start");
            Waitkey();
        }
    }
    YWVFD_Print("UP-R ok");
    return rcode;
    #endif
}

U_BOOT_CMD(
	updaterootfs,	1,	1,	do_updaterootfs,
	"updaterootfs  - update rootfs from the path of env_setings 'rootfsnfspath'\n",
	NULL
);

// YWDRIVER_MODI sqy 2008/12/2 end

// YWDRIVER_MODI cc 2008/11/19 add begin
int do_updatekernel (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    #if 1
    int rcode = 0;
    char *path;
    char *arg=NULL;

    char *s;
    int bootdelay;

    YWVFD_U_Kernel();

    s = getenv ("bootdelay");
    bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

    path = getenv ("kernelnfspath") ;

    if (path == NULL)
    {
		printf ("## Error: \"kernelnfspath\" is not defined,you must defind it first.\n");
		printf ("## \"kernelnfspath\" format:NfsServerIP:YourKernelPath.\n");
		printf ("## for example:\"set kernelnfspath 192.168.3.119:/home/d22cj/workspace/target\".\n");
		rcode = -1;
        YWVFD_Warn("P-kernel");
		return rcode;
    }
    printf ("kernelnfspath: \"%s\" \n",path);

    path = getenv ("kernel_name") ;

    if (path == NULL)
    {
		printf ("## Error: \"kernel_name\" is not defined,you must defind it first.\n");
		printf ("## for example:\"set kernel_name vmlinux.ub\".\n");
		rcode = -1;
        YWVFD_Warn("N-kernel");
		return rcode;
    }
    printf ("kernel_name: \"%s\" \n",path);

    arg="nfs $load_addr $kernelnfspath/$kernel_name";

    rcode = Call_command ( arg, flag ) ;

    if (rcode != COMMAND_SUCCESS)
    {
        puts ("\n     nfs Kernel failed.\n");
        YWVFD_Warn("NFS:e");
        return (-1);
    }
    else
    {
        printf("\n     nfs Kernel done.\n\n");
    }

    //char * s;
    s = getenv ("kernel_sec");
    if (s==NULL)
    {
        setenv ("kernel_sec", UPDATE_CFG_KERNEL_SEC);  /* set kernel_sec variable */
    }
    s = getenv ("kernel_base");
    if (s==NULL)
    {
        setenv ("kernel_base", XSTR(UPDATE_CFG_KERNEL_BASE));  /* set kernel_base variable */
    }

    arg="protect off $kernel_sec;erase $kernel_sec;cp.b $load_addr $kernel_base $filesize;protect on $kernel_sec";
    rcode = Call_command ( arg, flag ) ;

    if (rcode != COMMAND_SUCCESS)
    {
        puts ("\n     Update Kernel failed.\n");
        YWVFD_Warn("U-Kernel:e");
        YWVFD_Warn("-Kernel:e");
        YWVFD_Warn("Kernel:e");
        YWVFD_Warn("UP-K KO");
        return (-1);
    }
    else
    {
        printf("\n     Update Kernel done.\n");
    }
    printf("\n     Now will startup Kernel.\n");
    if(update_abortboot(bootdelay)==0)
    {
        arg = getenv ("bootcmd");
        rcode = Call_command ( arg, flag ) ;
        if (rcode != COMMAND_SUCCESS)
        {
            puts ("\n     Couldn't startup kernel.\n");

            YWVFD_Warn("C-start");
            Waitkey();

        }
    }
    YWVFD_Print("UP-K ok");
    return rcode;
    #endif
}

U_BOOT_CMD(
	updatekernel,	1,	1,	do_updatekernel,
	"updatekernel  - update kernel from the path of env_setings 'kernelnfspath'\n",
	NULL
);

int do_updateuboot (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int rcode = 0;
    char *path=NULL;
    char *arg=NULL;

    char *s;
    int bootdelay;
    s = getenv ("bootdelay");
    bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

    YWVFD_U_BOOT();

    path = getenv ("ubootnfspath") ;


    if (path == NULL)
    {
		printf ("## Error: \"ubootnfspath\" is not defined,you must defind it first.\n");
		printf ("## \"ubootnfspath\" format:NfsServerIP:YourUbootPath.\n");
		printf ("## for example:\"set ubootnfspath 192.168.3.119:/home/d22cj/workspace/target\".\n");
		rcode = -1;
        YWVFD_Warn("P-UBOOT");
		return rcode;
    }
    printf ("ubootnfspath: \"%s\" \n",path);

    path = getenv ("uboot_name") ;

    if (path == NULL)
    {
		printf ("## Error: \"uboot_name\" is not defined,you must defind it first.\n");
		printf ("## for example:\"set uboot_name u-boot.bin\".\n");
		rcode = -1;
		return rcode;
    }
    printf ("ubootname: \"%s\" \n",path);

    arg="nfs $load_addr $ubootnfspath/$uboot_name";
    rcode = Call_command ( arg, flag ) ;

    if (rcode != COMMAND_SUCCESS)
    {
        puts ("\n     nfs uboot failed.\n");
        return (-1);
    }
    else
    {
        printf("\n     nfs uboot done.\n\n");
    }

    arg="protect off $monitor_sec;erase $monitor_sec;cp.b $load_addr $monitor_base $monitor_len;protect on $monitor_sec";
    rcode = Call_command ( arg, flag ) ;
    if (rcode != COMMAND_SUCCESS)
    {
        puts ("\n     Update uboot failed.\n");
        YWVFD_Print("UP-U KO");
        return (-1);
    }
    else
    {
        printf("\n     Update uboot done.\n");
        YWVFD_Print("UP-U OK");
    }
    puts ("\n     Now will reset the board:\n");
    YWVFD_Warn("reset");
    if(update_abortboot(bootdelay)==0)
    {
        arg="reset";
        rcode=Call_command ( arg, flag ) ;
        if (rcode != COMMAND_SUCCESS)
        {
            puts ("\n     Couldn't soft reset board,please press button to reset.\n");

            Waitkey();

        }
    }
    return rcode;
}

U_BOOT_CMD(
	updateuboot,	1,	1,	do_updateuboot,
	"updateuboot  - update uboot from the path of env_setings 'ubootnfspath'\n",
	NULL
);
// YWDRIVER_MODI cc 2008/11/19 add end
#endif
