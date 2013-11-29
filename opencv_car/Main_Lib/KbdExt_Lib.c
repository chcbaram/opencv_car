


//----------------------------------------------------------------------------------------------
//            TITLE   : KbdExt_CheckMeta
//
//            WORK    : 
//            
//            DATE    : 2004. 3. 19
//----------------------------------------------------------------------------------------------
S16	KbdExt_CheckMeta( const char *Str_Ptr )
{
	U8   i        = 0;
	S16  Ret_Flag = 0;

	for( i = 0; i < KBD_EXT_MAX_META_STR; i++ ) 
	{
		// ��Ÿ ���ڿ� �迭���� ������ ���� Ret_Flag �� 
		// ��Ÿ ���ڿ��� �ش��ϴ� ��Ʈ ��ġ�� ������
		//
		if ( strcasecmp( Str_Ptr, KbdExt_MetaStr_Left[ i ] ) == 0 ) 
		{
			Ret_Flag = 1 << i;
		}

		if ( strcasecmp( Str_Ptr, KbdExt_MetaStr_Right[ i ] ) == 0 ) 
		{
			Ret_Flag = 1 << i;
		}
		
	}

	return Ret_Flag;
}





//----------------------------------------------------------------------------------------------
//            TITLE   : KbdExt_ReadKey_Map
//
//            WORK    : 
//            
//            DATE    : 2004. 3. 19
//----------------------------------------------------------------------------------------------
S16     KbdExt_ReadKey_Map( char *Map_File )
{
	S16		Meta_Flag;
	S16             Colum_Cnt;

	long            KeyCode_ID;
	long		KeyCode;
	S16             LineGet_Flag;
	
	
	FILE		*File_Ptr;
	
	char		Line_Str   [80];    // �� ���� ���� ����
	char		KeyName_Str[80];
	char		Flag_Str   [80];

	char            i;
	char            Opt_Flag;

	// ���� �ʱ�ȭ
	//
	KbdExt_MetaFlag = 0;

	memset( KbdExt_Keys  , 0, sizeof( KbdExt_Keys  ) );
	memset( KbdExt_Metas , 0, sizeof( KbdExt_Metas ) );
	memset( KbdExt_KeyMap, 0, sizeof( KbdExt_KeyMap) );

	memset( Line_Str, 0, sizeof( Line_Str) );


	// Ű ������ �ε�
	//
	File_Ptr = fopen( Map_File, "r" );
	if ( NULL == File_Ptr )
	{
		printf("File not found \n");
		return( RET_ERROR );
	}

	while ( ! feof( File_Ptr ) )
	{

		// ���Ͽ��� �����͸� ���پ� �о�鿩�� ó���Ѵ�.
		//
		//LineGet_Flag = fgets( Line_Str, sizeof( Line_Str )-1, File_Ptr )

		if ( NULL != fgets( Line_Str, sizeof( Line_Str )-1, File_Ptr ) )
		{

			// ���� ������ # �̸�  �ּ� ó��
			//
			if ( Line_Str[ 0 ] == '#' ) continue;

			Colum_Cnt = sscanf( Line_Str,"%s %lx", KeyName_Str, &KeyCode_ID );

			if ( Colum_Cnt < 1 ) continue;


			// ��Ÿ ���ڿ� �о ó���Ѵ�.
			//
			Meta_Flag = KbdExt_CheckMeta( KeyName_Str );

			if ( Meta_Flag != 0 )
			{
				if ( ( Colum_Cnt == 2) && (KeyCode_ID < 128) ) 
				{
					KbdExt_Metas[ KeyCode_ID ] = Meta_Flag;
				}
				
				continue;
			}

			// �ɼ� �� ����
			// 
			Opt_Flag = -1;
			for( i=0; i<KBD_EXT_MAX_OPT_STR; i++ )
			{
				if ( strcasecmp( KeyName_Str, KbdExt_OptStr[i] ) == 0 ) 
				{
					Opt_Flag = i;
					
					// KeyCode_ID �� �������� �б� ���� �ٽ� �д´�
					// �ɼǿ����� �������� ó���ϱ� ���ؼ�..
					//
					sscanf( Line_Str,"%s %d", KeyName_Str, &KeyCode_ID );									
					break;
				}
			}
			switch( Opt_Flag )
			{
				case KBD_EXT_OPT_KEY_FIRST:
					KbdExt_KeyRepeat_First = KeyCode_ID;
					continue;				
					break;
				case KBD_EXT_OPT_KEY_NEXT:
					KbdExt_KeyNext = KeyCode_ID;
					continue;				
					break;
			}		
			

			// ��Ÿ ���ڿ� , �ɼ� �� �� äũ�ϰ� ���� ������ ���� �ƴ��� �˻�
			//
			if ( strlen( KeyName_Str ) != 2 )       continue;
			if ( (! isxdigit( KeyName_Str[0]))  )   continue;
			if ( (! isxdigit( KeyName_Str[1]))  )   continue;
			

			Colum_Cnt = sscanf( Line_Str, "%lx %s %s %lx", 
			                              &KeyCode_ID, KeyName_Str, Flag_Str, &KeyCode );


			//printf("%lx %s %s %lx\n",KeyCode_ID, KeyName_Str, Flag_Str, KeyCode );
			

			if ( Colum_Cnt < 4 )    continue;

			if ( KeyCode_ID > 127 ) continue;

			Meta_Flag = 0;


			// ��ŸŰ�� �����Ǿ������� ��ŸŰ �迭�� �Ҵ��Ѵ�.
			//
			// S C F C O �� �ش��ϴ°�
			//
			for( i=0; i<KBD_EXT_MAX_META_STR; i++ )
			{
				if( strchr( Flag_Str, KbdExt_MetaChr[i] ) != NULL )
				{
					Meta_Flag |= (1<<i);
				}
			}


			KbdExt_KeyMap[ Meta_Flag ][ KeyCode_ID ].Key_Name = strdup( KeyName_Str );
			KbdExt_KeyMap[ Meta_Flag ][ KeyCode_ID ].Key_Code = KeyCode;
			

		}
        }

	fclose( File_Ptr );


	printf("Key First : %d \n", KbdExt_KeyRepeat_First );
	printf("Key Next  : %d \n", KbdExt_KeyNext );
	return  RET_OK;
}








//----------------------------------------------------------------------------------------------
//            TITLE   : KbdExt_Close
//
//            WORK    : 
//            
//            DATE    : 2004. 3. 19
//----------------------------------------------------------------------------------------------
void    KbdExt_Close( void )
{
	int	i;
	long	KeyCode;

	
	// Ű�� ���ȴٴ� ��ȣ�� �ִ°Ŵ� ��� �Ұ� ��Ų��. Ȥ�� �𸣴ϱ�
	//
	for( i = 0; i < 127; i++ ) 
	{
		if ( KbdExt_Keys[ i ] != 0 ) 
		{
			KeyCode = KbdExt_KeyMap[ 0 ][ i ].Key_Code & 0x2a000000;
			
			if ( KeyCode != 0 ) 
			{
				 KbdSharp_Putch( KeyCode );
			}
		}
	}
	
}




