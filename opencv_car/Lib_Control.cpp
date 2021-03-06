#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>


#define USE_SERIAL_SIGANL	0
#define USE_SERIAL			1


#include "./Main_Lib/Define.h"

#include "./Main_Lib/Uart_Lib.h"
#include "./Main_Lib/Uart_Sig_Lib.h"
#include "./Main_Lib/Thread_Lib.h"                                                                                          
                                                                                                 
#include "./Main_Lib/Uart_Lib.c"
#include "./Main_Lib/Uart_Sig_Lib.c"


int Lib_Control_Enable = 0;

int Lib_Control_State = 0;
int Lib_Control_Cnt;


extern int Lib_Vision_CarDetected;
extern int Lib_Vision_LedCnt;
extern int Lib_Vision_LedSize;



void Lib_Control_Uart_Rxd_Func( char Data )
{
	printf("%c", Data );
}



void *Lib_Control(void *Arg)
{
	int  Uart_Handle_Ptr;
	int  Uart_Handle_Ptr_2;

	THREAD_OBJ *pArg = (THREAD_OBJ *)Arg;


	#if USE_SERIAL == 1
	//-- 시리얼 통신 초기화 
	Uart_Handle_Ptr = Uart_Open( 0, "/dev/rfcomm0", BAUD_115200 );

	if( Uart_Handle_Ptr < 0 )
	{
		printf("Uart Open Failed \n");
		pArg->Thread_Ret = 1;
		return (void *)&(pArg->Thread_Ret);	
	}

	//Uart_Sig_Init( Uart_Handle_Ptr );
	//Uart_Sig_Func_Init( Uart_Handle_Ptr, Lib_Control_Uart_Rxd_Func );
	#endif


	#if USE_SERIAL_SIGANL == 1
	//-- 시리얼 통신 초기화 
	Uart_Handle_Ptr_2 = Uart_Open( 1, "/dev/ttyUSB0", BAUD_115200 );

	if( Uart_Handle_Ptr_2 < 0 )
	{
		printf("Uart Open Failed \n");
		pArg->Thread_Ret = 1;
		return (void *)&(pArg->Thread_Ret);	
	}

	//Uart_Sig_Init( Uart_Handle_Ptr );
	//Uart_Sig_Func_Init( Uart_Handle_Ptr, Lib_Control_Uart_Rxd_Func );
	#endif

	printf("Enter Control Thread \n");

	while(pArg->Thread_Stop == FALSE)
	{
		if( Lib_Control_Enable == 0 )
		{
			Lib_Control_State = 0;
		}



		switch( Lib_Control_State )
		{
			case 0:
				if( Lib_Control_Enable == 1 )
				{
					printf("Wait for Car..\n");

					if( Lib_Vision_CarDetected == TRUE )
					{
						printf("Detected Car..\n");
						Lib_Control_State = 1;
						Lib_Vision_LedCnt = 0;
						Lib_Control_Cnt = 0;
					}
				}
				break;

			case 1:
				printf("Wait for LED.. %d %d\n",Lib_Vision_LedSize, Lib_Vision_LedCnt);		

				Lib_Control_Cnt++;

				if( Lib_Control_Cnt > (60) )
				{
					if( Lib_Vision_LedCnt > 40 )
					{
						Lib_Control_State = 2;
						printf("Left..\n");
						#if USE_SERIAL == 1
						Uart_Print(0, "B");
						#endif
						#if USE_SERIAL_SIGANL == 1
						Uart_Print(1, "B");
						#endif						
					}
					else
					{
						Lib_Control_State = 2;
						printf("Go..\n");
						#if USE_SERIAL == 1
						Uart_Print(0, "A");
						#endif	
						printf("Go..\n");
						#if USE_SERIAL_SIGANL == 1
						Uart_Print(1, "A");
						#endif							
					}
				}
				break;

			case 2:
				printf("Wait for out of way..\n");

				if( Lib_Vision_CarDetected == FALSE )
				{
					Lib_Control_State = 0;	
				}		
				break;

		}

		usleep(100*1000);
	}

	printf("Exit Control Thread \n");


	return (void *)&(pArg->Thread_Ret);	
}




