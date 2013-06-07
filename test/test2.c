#include <cv.h>
#include <highgui.h>
#include <math.h>

using namespace cv;
void x(IplImage *);

int main(int argc, char** argv)
{
	CvCapture *capture=0;
    // initialize camera
    capture = cvCaptureFromCAM( 0 );
    //assert( capture );
    // image data structures
    IplImage *img1;
    IplImage *img2;
    // get the camera image size
    IplImage *imgsize;
    imgsize = cvQueryFrame( capture );
    //if( !imgsize ) return -1;
    img1 = cvCloneImage(imgsize);
    img2 = cvCloneImage(imgsize);
    IplImage *imggray1;
    IplImage *imggray2;
    IplImage *imggray3;
    
	// grayscale buffers
    imggray1 = cvCreateImage( cvGetSize(imgsize), IPL_DEPTH_8U, 3);
    imggray2 = cvCreateImage( cvGetSize(imgsize), IPL_DEPTH_8U, 3);
    imggray3 = cvCreateImage( cvGetSize(imgsize), IPL_DEPTH_8U, 1);
    
    int key = 0;
    
    while ( key != 27 ) {
        // load image one
        img1 = cvQueryFrame( capture );
        cvCvtColor( img2, imggray2, CV_BGR2HSV );
		x(img2);
			
        key = cvWaitKey( 50 );
        // load image two
        img2 = cvQueryFrame( capture );
        cvCvtColor( img1, imggray1, CV_BGR2HSV );
		cvShowImage("original",img2);
        //display difference
        //cvNamedWindow( "video", win );
        //key = cvWaitKey( 5 );
   }

    cvReleaseCapture( &capture );
    //cvDestroyWindow( "video" );
	cvReleaseImage(&imggray1);
    cvReleaseImage(&imggray2);
    cvReleaseImage(&imggray3);
    cvReleaseImage(&imgsize);
    cvReleaseImage(&img1);
    cvReleaseImage(&img2);
    return 0;
}
