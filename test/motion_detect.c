#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>

void x(IplImage *, IplImage*, IplImage*);

int main(int argc, char* argv[])
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
   
    int key = 0;
    while ( key != 27 ) {
        // load image one
        img1 = cvQueryFrame( capture );
        // convert rgb to grayscale
        // quit if user press 'q' and wait a bit between images
        key = cvWaitKey( 500 );
        // load image two
        img2 = cvQueryFrame( capture );
        // display difference
        x(img1, img2, imgsize);
        //cvNamedWindow( "video", win );
        //key = cvWaitKey( 5 );
        
    }

    cvReleaseCapture( &capture );
    //cvDestroyWindow( "video" );

	return 0;
}

void x(IplImage *img1, IplImage *img2, IplImage *imgsize)
{
	IplImage *imggray1;
    IplImage *imggray2;
    IplImage *imggray3;
    
	// grayscale buffers
    imggray1 = cvCreateImage( cvSize( imgsize->width, imgsize->height ), IPL_DEPTH_8U, 1);
    imggray2 = cvCreateImage( cvSize( imgsize->width, imgsize->height ), IPL_DEPTH_8U, 1);
    imggray3 = cvCreateImage( cvSize( imgsize->width, imgsize->height ), IPL_DEPTH_8U, 1);
	
	IplImage *hsv1 = cvCloneImage(img1);
	IplImage *hsv2 = cvCloneImage(img2);
	
	cvCvtColor( img2, imggray2, CV_RGB2GRAY );
    cvCvtColor(img1, hsv2, CV_BGR2HSV);
    cvSetImageCOI(hsv2, 1);
    cvCopy(hsv2, imggray2, 0);           
    // convert rgb to grayscale
    cvCvtColor( img1, imggray1, CV_RGB2GRAY );
    cvCvtColor(img2, hsv1, CV_BGR2HSV);
    cvSetImageCOI(hsv1, 1);
    cvCopy(hsv1, imggray1, 0);           
    // compute difference
    cvAbsDiff( imggray1, imggray2, imggray3 );
    cvShowImage( "video", imggray3 );    
    cvReleaseImage(&imggray1);
    cvReleaseImage(&imggray2);
    cvReleaseImage(&imggray3);
    cvReleaseImage(&hsv1);
    cvReleaseImage(&hsv2);
}
