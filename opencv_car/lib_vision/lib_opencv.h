//----------------------------------------------------------------------------
//    프로그램명   : lib_opencv 관련 헤더
//
//    만든이         : 
//
//    날  짜      :
//    
//    최종 수정     :
//
//    MPU_Type      :
//
//    파일명         : lib_opencv.h
//----------------------------------------------------------------------------

                                                                                                  
#ifndef __HW_LIB_OPENCV_H__
#define __HW_LIB_OPENCV_H__


#ifdef   HW_LIB_OPENCV_LOCAL
#define  EXT_LIB_OPENCV_DEF
#else
#define  EXT_LIB_OPENCV_DEF     extern
#endif
 
 
 //-- For OpenNI2
//
#include <stdio.h>
#include <OpenNI.h>


//-- For OpenCV
//
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>

 


//#include "cv.h"
//#include "highgui.h"
#include "opencv2/opencv.hpp"

 
EXT_LIB_OPENCV_DEF CvRect VLib_GetOpejectRect( IplImage *imgSRC, int x, int y );


#endif