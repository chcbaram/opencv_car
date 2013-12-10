//----------------------------------------------------------------------------------------------
//            TITLE       : Serial 통신 처리 함수 모음 
//            WORK        :
//            DATE        : 2003. 3. 7
//            FILE        : Uart_Lib.c
//            Modified By : Hanchol Cho     - IRSL -
//----------------------------------------------------------------------------------------------


#include <stdarg.h>





//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_OpenPortNonCanonical_Mode
//
//            WORK    : 시리얼 포트 초기화
//            
//            DATE    : 2003. 7. 4
//----------------------------------------------------------------------------------------------------
int Uart_Open( int Ch, char *pPortName, int Uart_BaudData )
{


        Uart_Handle[Ch] = open( pPortName, O_RDWR | O_NOCTTY | O_NONBLOCK );
	if( Uart_Handle[Ch] < 0 )
	{
                perror( pPortName );
                exit( -1 );
	}

	// 저장한다 현제 시리얼포트 정보를
	tcgetattr( Uart_Handle[Ch], &Term_OldIo );
	// Write zeros to a byte string
	bzero( &Term_NewIo, sizeof( Term_NewIo ) );
	// CS8   : 8N1 (8bit, No Parity, 1 Stopbit
	// CREAD : 수신 가능
	Term_NewIo.c_cflag = Uart_Baud[ Uart_BaudData ] | CS8 | CLOCAL | CREAD;
	// IGNPAR : Parity 에러가 있는 문자 바이트를 무시한다
	// ICRNL  : CR 문자를 NL 문자로 변환 처리한다. ( 이설정을 안하면 다른 컴퓨터는 
	//          CR 문자를 한 줄의 종료문자로 인식하지 않을 수 있다
	Term_NewIo.c_iflag = IGNPAR ;
	
	Term_NewIo.c_oflag = 0;

	Term_NewIo.c_cc[ VTIME ]   = 0;
	Term_NewIo.c_cc[ VMIN  ]   = 0;

	tcflush  ( Uart_Handle[Ch], TCIFLUSH );
	tcsetattr( Uart_Handle[Ch], TCSANOW, &Term_NewIo );
	
	// Flush
	//
	//ioctl( Uart_Handle, TCFLSH, 0 );

	Uart_ModeFlag = NONE_CANON_MODE;

	return 	Uart_Handle[Ch];
}





//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Close
//
//            WORK    : 시리얼 포트 닫는다 
//            
//            DATE    : 2003. 7. 4
//----------------------------------------------------------------------------------------------
void Uart_Close( int Ch )
{
	tcsetattr( Uart_Handle[Ch], TCSANOW, &Term_OldIo );	
}






//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Getch
//
//            WORK    : 문자 1개를 받는다 
//            
//            DATE    : 2003. 7. 4
//----------------------------------------------------------------------------------------------------
int Uart_Getch( int Ch )
{
	char Uart_GetData;
	
	switch( Uart_ModeFlag )
	{
		case CANON_MODE:
			
			/* 데이터 1바이트 가져온다 */			
			read( Uart_Handle[Ch], Uart_Buf, 1 );

			Uart_GetData = Uart_Buf[0];
					
			break;
			
		case NONE_CANON_MODE:
		
			Uart_GetData = read( Uart_Handle[Ch], Uart_Buf, 1 );

			if( Uart_GetData < 1) return 1000;

			Uart_GetData = Uart_Buf[0];
			break;
	}

	return Uart_GetData;
}




	
//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Putch
//
//            WORK    : 문자 1개를 보낸다 
//            
//            DATE    : 2003. 3. 7
//----------------------------------------------------------------------------------------------
void Uart_Putch( int Ch, char Uart_PutData )
{
	
	
	switch( Uart_ModeFlag )
	{
		case CANON_MODE:
		case NONE_CANON_MODE:
			
			/* 데이터 1바이트 보낸다 */			
			write( Uart_Handle[Ch], &Uart_PutData, 1 );
			break;
	}

}



//----------------------------------------------------------------------------
//
//          TITLE : Uart_Print
//
//          WORK  : 시리얼로 문자열을 보낸다.
//
//----------------------------------------------------------------------------
void Uart_Print(int Ch, char *UartPrintData)
{
	while( *UartPrintData != '\0') 
	{
		Uart_Putch( Ch, *UartPrintData);
               
		if( *UartPrintData == 0x0A )  // LF 에 CR을 보탠다
			Uart_Putch(Ch, 0x0D);	
               		
		UartPrintData++;
	}        
}






/*---------------------------------------------------------------------------
     TITLE	: Uart_Printf
     WORK
     ARG
     RET
---------------------------------------------------------------------------*/
void Uart_Printf( int Ch, char *format, ... )
{
	char Str[200];
	
	va_list ap;
	
	va_start( ap, format );

	vsprintf( Str, format, ap );	

	Uart_Print( Ch, Str);
	
	va_end(ap);	
}






//----------------------------------------------------------------------------------------------
//            TITLE   : Uart_Gets
//
//            WORK    : 문자 1개를 받는다 
//            
//            DATE    : 2003. 3. 7
//----------------------------------------------------------------------------------------------
char *Uart_Gets( int Ch )
{
	
	switch( Uart_ModeFlag )
	{
		// 표준 모드
		case CANON_MODE:
			
			/* 데이터열을 가져온다 */
			Buf_ReadSize = read( Uart_Handle[Ch], Uart_Buf, 255 );
			Uart_Buf[ Buf_ReadSize ] = 0;			
			
			return Uart_Buf;
			
			break;
		// 비표준 모드
		case NONE_CANON_MODE:
			
			Buf_ReadSize = read( Uart_Handle[Ch], Uart_Buf, 255);
			
			//if( Buf_ReadSize < 1 ) return Ser_Buf;
			
			Term_NewIo.c_cc[ VMIN  ]   = Buf_ReadSize-1;
			tcsetattr( Uart_Handle[Ch], TCSANOW, &Term_NewIo );
			
			/* 데이터 1바이트 가져온다 */
			Buf_ReadSize = read( Uart_Handle[Ch], Uart_Buf, 255 );
			
			Uart_Buf[ Buf_ReadSize ] = 0;
						
			Term_NewIo.c_cc[ VMIN  ]   = 0;
			tcsetattr( Uart_Handle[Ch], TCSANOW, &Term_NewIo );
			
			return Uart_Buf;
			
			break;
	}

	return 0;	
}
		
