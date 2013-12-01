



#define HW_LIB_OPENCV_LOCAL
#include "lib_opencv.h"










/*---------------------------------------------------------------------------
	TITLE : Lib_Vision
	WORK  :
	ARG   : void
	RET   : void
---------------------------------------------------------------------------*/
void VLib_Init(void)
{
}





/*---------------------------------------------------------------------------
	TITLE : VLib_GetOpejectRect
	WORK  :
	ARG   : void
	RET   : void
---------------------------------------------------------------------------*/
CvRect VLib_GetOpejectRect( IplImage *imgSRC, int x, int y )
{
	unsigned char *pImg   = (unsigned char *)imgSRC->imageData;
	int i;
	int ix;
	int iy;
	int yOff;
	
	int width;
	int height;

	int find_sx;
	int find_ex;
	int find_sy;
	int find_ey;
	
	int find_size = 150;
	int size_gap  = 10;

	int Hist_x[512];
	int Hist_y[512];

	CvRect obj_box;

	width  = imgSRC->width;
	height = imgSRC->height;
	
	//-- Set X width
	find_sx = x - find_size;
	find_ex = x + find_size;

	if( find_sx < 0     ) find_sx = 0;
	if( find_ex > width ) find_ex = width;


	//-- Set Y width
	find_sy = y - find_size;
	find_ey = y + find_size;

	if( find_sy < 0      ) find_sy = 0;
	if( find_ey > height ) find_ey = height;

	for( i=0; i<512; i++ )
	{
		Hist_x[i] = 0;
		Hist_y[i] = 0;
	}

	
	//-- Y Histogram
	for(iy=find_sy; iy<find_ey; iy++)
	{
		yOff   = iy * width;
		for(ix=0; ix<width; ix++)
		{
			if( pImg[ yOff + ix ] == 255 )	Hist_y[ iy-find_sy ]++;
		}
	}		


	int found_sy;
	int found_ey;
	int sum;

	sum = 0;
	for(iy=find_sy; iy<find_ey; iy++)
	{
		sum += Hist_y[iy-find_sy];
		if( sum > 10 ) break;
	}		

	found_sy = iy - size_gap;
	if( found_sy < 0 ) found_sy = 0;

	sum = 0;
	for(iy=find_ey-1; iy>=find_sy; iy--)
	{
		sum += Hist_y[iy-find_sy];
		if( sum > 10 ) break;
	}		

	found_ey = iy;

	found_ey = iy + size_gap;
	if( found_ey > height ) found_ey = height;


	//-- X Histogram
	for(iy=find_sy; iy<find_ey; iy++)
	{
		yOff   = iy * width;
		for(ix=0; ix<width; ix++)
		{
			if( pImg[ yOff + ix ] == 255 )	Hist_x[ ix-find_sx ]++;
		}
	}		


	int found_sx;
	int found_ex;


	sum = 0;
	for(ix=find_sx; ix<find_ex; ix++)
	{
		sum += Hist_x[ix-find_sx];
		if( sum > 10 ) break;
	}		

	found_sx = ix - size_gap;
	if( found_sx < 0 ) found_sx = 0;

	sum = 0;
	for(ix=find_ex-1; ix>=find_sx; ix--)
	{
		sum += Hist_x[ix-find_sx];
		if( sum > 10 ) break;
	}		

	found_ex = ix;

	found_ex = ix + size_gap;
	if( found_ex > width ) found_ex = width;	


	obj_box.x = found_sx;
	obj_box.y = found_sy;
	obj_box.width  = found_ex - found_sx;
	obj_box.height = found_ey - found_sy;

	return obj_box;
}



#if 0


/*---------------------------------------------------------------------------
	TITLE : BlobLabeling
	WORK  :
	ARG   : void
	RET   : void
---------------------------------------------------------------------------*/
int VLib_BlobLabeling(IplImage *imSRC, IplImage *imLabel, LabelST *stLabel, int *pCountLabel)
{
	int ix, iy, ig;
	int arNumBuf[MAX_LABEL_ID] = {0, };
	int groupBuf[9] = { 0, };
	int minLabel;
	int maxLabel;
	int bufLabel;
	int curLabel;
	int outflag;
	int width, height;
	unsigned char *pImg   = (unsigned char *)imSRC->imageData;
	unsigned char *pLabel = (unsigned char *)imLabel->imageData;
	int yOffM1, yOff, yOffP1;
	int countLabel;
	
	
	width  = IMG_WIDTH;
	height = IMG_HEIGHT;
	
	
	// ∑π¿Ã∫Ì øµªÛ √ ±‚»≠
	for(iy=0; iy<height; iy++)
	{
		yOff   = iy * width;
		for(ix=0; ix<width; ix++)
		{
			pLabel[yOff + ix] = 255;
		}
	}
	for(ig=0; ig<MAX_LABEL_ID; ig++)
	{
		stLabel[ig].id         = 0;
		stLabel[ig].numPixel = 0;
		arNumBuf[ig] = 0;
	}
	
	
	// 1¬˜  ∑π¿Ã∫Ì ºˆ«‡
	maxLabel = -1;
	for(iy=1; iy<height-1; iy++)
	{
		yOffM1 = (iy-1) * width;
		yOff   = iy * width;
		
		for(ix=1; ix<width-1; ix++)
		{
			if(pImg[yOff+ix] == 255)
			{
				groupBuf[0] = pLabel[yOffM1+ix-1];
				groupBuf[1] = pLabel[yOffM1+ix  ];
				groupBuf[2] = pLabel[yOffM1+ix+1];
				groupBuf[3] = pLabel[yOff  +ix-1];
				
				minLabel = 255;
				for(ig=0; ig<4; ig++)
				{
					bufLabel = groupBuf[ig];
					if(bufLabel < minLabel)
						minLabel = bufLabel;
				}
				if(minLabel == 255)
				{
					maxLabel++;
					pLabel[yOff + ix] = maxLabel;
				}
				else
					pLabel[yOff + ix] = minLabel;
			}
		}
	}
	
	if(maxLabel == -1)
		return _FALSE;
	
	
	while(1)
	{
		outflag = 1;
		for(iy=1; iy<height-1; iy++)
		{
			yOffM1 = (iy-1) * width;
			yOff   = iy * width;
			yOffP1 = (iy+1) * width;
			for(ix=1; ix<width-1; ix++)
			{
				if(pImg[yOff + ix] == 255)
				{
					groupBuf[0] = pLabel[yOffM1 + ix-1];
					groupBuf[1] = pLabel[yOffM1 + ix  ];
					groupBuf[2] = pLabel[yOffM1 + ix+1];
					groupBuf[3] = pLabel[yOff   + ix-1];
					groupBuf[4] = pLabel[yOff   + ix  ];
					groupBuf[5] = pLabel[yOff   + ix+1];
					groupBuf[6] = pLabel[yOffP1 + ix-1];
					groupBuf[7] = pLabel[yOffP1 + ix  ];
					groupBuf[8] = pLabel[yOffP1 + ix+1];
					
					curLabel = groupBuf[4];
					minLabel = 255;
					for(ig=0; ig<9; ig++)
					{
						bufLabel = groupBuf[ig];
						if(bufLabel < minLabel)
							minLabel = bufLabel;
					}
					
					if(minLabel == 255 || curLabel == minLabel);
					else
					{
						pLabel[yOff + ix] = minLabel;
						outflag = 0;
					}
				}
			}
		}
		if(outflag == 1)
			break;
	}
	
	// ∑π¿Ã∫Ì º≥¡§.
	for(iy=1; iy<height-1; iy++)
	{
		yOff   = iy * width;
		for(ix=1; ix<width-1; ix++)
		{
			arNumBuf[pLabel[yOff + ix]]++;
		}
	}
	
	countLabel = 0;
	for(ig=0;ig<MAX_LABEL_ID; ig++)
	{
		if(arNumBuf[ig] != 0)
		{
			stLabel[countLabel].id       = ig;
			stLabel[countLabel].numPixel = arNumBuf[ig];			
			countLabel++;

			if( countLabel >= MAX_LABEL_ID ) break;
		}
	}
	
	if(countLabel == 0)
		return _FALSE;
	
	*pCountLabel = countLabel;
	
#if _DEBUG_WINDOW_ == 1
#if _DEBUG_LABELING_ == 1
	int arID[MAX_LABEL_ID] = {0, };
	int count;
	
	for(ig=0; ig<MAX_LABEL_ID; ig++)
	{
		arID[ig] = 0;
	}
	
	count = 0;
	
	for(ig=0;ig<MAX_LABEL_ID; ig++)
	{
		if(arNumBuf[ig] != 0)
		{
			arID[ig] = count;
			count++;
		}
	}
	
	for(iy=1; iy<height-1; iy++)
	{
		yOff   = iy * width;
		for(ix=1; ix<width-1; ix++)
		{
			if(pLabel[yOff + ix] == 255);
			else
				pLabel[yOff + ix] = 255/(count+1)*arID[pLabel[yOff+ix]];
		}
	}
#endif
#endif
	
	return _TRUE;
}



#define VLIB_MAX_LABEL_ID	254

typedef struct
{
	int numPixel;
	int id;
} VLIB_LabelST;

typedef struct 
{
	VLIB_LabelST stLabel[VLIB_MAX_LABEL_ID];
	int     	 countLabel;
	char		 strLine[255];

	IplImage 	*imLabel
} VLIB_BLOB_OBJ;


/*---------------------------------------------------------------------------
	TITLE : VLib_Blob_GetMainBlob
	WORK  :
	ARG   : void
	RET   : void
---------------------------------------------------------------------------*/
int VLib_Blob_GetMainBlob(IplImage *imLabel, LabelST *stLabel, int countLabel)
{
	int ix, iy, ig;
	int countBuf;
	int width, height;
	unsigned char *pLabel = (unsigned char *)imLabel->imageData;
	int yOff;
	int max = 0;
	int mainID;
	int count;

	width  = imLabel->width;
	height = imLabel->;
	


	if(countLabel == 1)
		mainID = 0;
	else
	{
		ig = 0;
		ix = 0;
		max = 0;
		for(ix=0; ix<countLabel; ix++)
		{
			countBuf = stLabel[ix].numPixel;
			if(countBuf > max)
			{
				max = countBuf;
				mainID = stLabel[ix].id;
			}
		}
	}
	
	count = 0;

	for(iy=0; iy<height; iy++)
	{
		yOff = iy*width;
		for(ix=0; ix<width; ix++)
		{
			if(pLabel[yOff + ix] == mainID)
			{
				pLabel[yOff + ix] = 255;
				count++;
			}
			else
				pLabel[yOff + ix] = 0;
		}
	}
	

	if( count > Lib_Vision_Thre_Label )
	{
		Lib_Vision_Value_Label = count;
		return _TRUE;
	}
	else
	{
		return _FALSE;
	}
}


#endif