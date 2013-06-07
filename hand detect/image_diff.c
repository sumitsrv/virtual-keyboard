#include <cv.h>
#include <highgui.h>
  
/*
  This is a quick program to compute the absolute difference
  of two grayscale images taken from an attached camera.
 */
 
int main( int argc, char** argv )
{
	// open camera
    CvCapture *capture;
    // initialize camera
    capture = cvCaptureFromCAM( 0 );
    assert( capture );
     
    // image data structures
    IplImage *img1;
    IplImage *img2;
    IplImage *imggray1;
    IplImage *imggray2;
    IplImage *imggray3;
        
    // get the camera image size
    IplImage *imgsize;
    imgsize = cvQueryFrame( capture );
    if( !imgsize ) return -1;
 
    // grayscale buffers
    imggray1 = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
    imggray2 = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
    imggray3 = cvCreateImage( cvGetSize( imgsize ), IPL_DEPTH_8U, 1);
     
    int key = 0;
    while ( key != 'q' ) {
        // load image one
        img1 = cvQueryFrame( capture );
        // convert rgb to grayscale
        cvCvtColor( img1, imggray2, CV_RGB2GRAY );
                     
        // quit if user press 'q' and wait a bit between images
        key = cvWaitKey( 20 );
 
		// load image two
        img2 = cvQueryFrame( capture );
        // convert rgb to grayscale
        cvCvtColor( img2, imggray1, CV_RGB2GRAY );
         
        // compute difference
        cvAbsDiff( imggray1, imggray2, imggray3 );
        cvThreshold(imggray3, imggray3, 30, 255, CV_THRESH_BINARY);
        //cvAnd(imggray1, imggray3, imggray3);
        //cvThreshold(imggray3, imggray3, 25, 255, CV_THRESH_BINARY); 
		// display difference
        cvNamedWindow( "video", 1 );
        //cvShowImage("img1",imggray1);
        //cvShowImage("img2", imggray2);
        cvShowImage( "video", imggray3 );
    }
     
    cvReleaseCapture( &capture );
    cvDestroyWindow( "video" );
    return 0;

}
