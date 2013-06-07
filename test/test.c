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


void x(IplImage *img1)
{
	IplImage *i1 = cvCloneImage(img1);
	IplImage *i2 = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 3);
	
	IplImage *imggray1 = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 1);
	IplImage *imggray2 = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 1);
	IplImage *imggray3 = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 1);
	
	cvSetImageCOI(i1, 1);
	cvCopy(i1, imggray1, 0);
	cvEqualizeHist(imggray1, imggray1);
	cvCopy(imggray1, i1);
	
	cvSetImageCOI(i1, 2);
	cvCopy(i1, imggray2, 0);
	cvEqualizeHist(imggray2, imggray2);
	cvCopy(imggray2, i1);
	
	cvSetImageCOI(i1, 1);
	cvCopy(i1, imggray3, 0);
	cvEqualizeHist(imggray3, imggray3);
	cvCopy(imggray3, i1);
	
	//cvSetImageCOI(i1, 1);
	//cvSetImageCOI(i2, 1);
	//cvCopy(i1, imggray1, 0);
	//cvCopy(i2, imggray2, 0);
	//cvCvtColor(img2, imggray2, CV_BGR2GRAY );
    //cvCvtColor(img1, imggray1, CV_BGR2GRAY);
    //cvShowImage("Img1", imggray1);
    //cvShowImage("img2", imggray2);
    //cvThreshold(imggray3, imggray3, 15, 255, CV_THRESH_TOZERO);
    //cvErode(imggray3, imggray1, NULL, 3);
    //cvCanny(imggray3, imggray3, 150, 200, 3);
    //cvShowImage( "img", imggray3 );
    
    //cvCvtColor(i1, i2, CV_HSV2BGR);
    cvShowImage("new", i1);
        
    cvReleaseImage(&imggray1);
    cvReleaseImage(&imggray2);
    cvReleaseImage(&imggray3);
    cvReleaseImage(&i1);
    cvReleaseImage(&i2);
}
