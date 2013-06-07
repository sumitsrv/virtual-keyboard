#include <stdio.h>
#include "cv.h"
#include "highgui.h"
 
int main( int argc, char **argv )
{
    CvCapture *capture = 0;
    IplImage  *frame = 0;
    int       key = 0;
 
    /* initialize camera */
    capture = cvCaptureFromCAM(0);
 
    /* always check */
    if ( !capture ) {
        fprintf( stderr, "Cannot open initialize webcam!\n" );
        return 1;
    }
 
    /* create a window for the video */
    cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );
 
  IplImage* img = 0; 
if(!cvGrabFrame(capture)){              // capture a frame 
  printf("Could not grab a frame\n\7");
  exit(0);
}
img=cvRetrieveFrame(capture); 

	 if(!cvSaveImage("testimg1.jpg",img)) printf("Could not save: testimg1.jpg\n");
	cvShowImage( "result", img );
	cvWaitKey(0);
    /*while( key != 'q' ) {
       frame = cvQueryFrame( capture );
 
        if( !frame ) break;
       
        cvShowImage( "result", frame );
 
        key = cvWaitKey( 10 );
    }*/
    cvDestroyWindow( "result" );
    cvReleaseCapture( &capture );
	cvReleaseImage(&img);
    return 0;
}
