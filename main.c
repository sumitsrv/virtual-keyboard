#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>

#include "haar.h"
#include "edge.h"
#include "keys.h"
#include "webcam.h"
#include "colorEdge.h"
#include "contrast.h"
#include "ocr.h"

int main(int argc, char* argv[])
{
	CvCapture *capture = 0;
	IplImage *img = 0;
	if( argc < 3 )
	{
		fprintf( stderr, "Usage: loadimg <filename> <cam(1)/image(2)>\n" );
		return 1;
    }
    else if (argv[2][0] == '1')
    {
		/* initialize camera */
		capture = cvCaptureFromCAM(0);
		/* always check */
		if ( !capture ) {
			fprintf( stderr, "Cannot open initialize webcam!\n" );
			fprintf( stderr, "Usage: loadimg <filename> <cam(1)/image(2)>\n" );
			return 1;
		}
		webcam(capture, argv[1]);
		img = cvLoadImage(argv[1],-1);
	}
	else if(argv[2][0] == '2')
	{
		img = cvLoadImage(argv[1],-1);
	}
	else
	{
		fprintf( stderr, "Invalid argument 2\n" );
		return 1;
	}
	
	if( img == 0 )
	{
        fprintf( stderr, "Cannot load file %s!\n", argv[1] );
        return 1;
    }
    
    int depth = img->depth;
	
	CvSize sz = cvSize( img->width & -2, img->height & -2 );
   
	IplImage* gray = cvCreateImage(cvSize(sz.width, sz.height), depth, 1);
	IplImage* copy = cvCreateImage(cvSize(sz.width, sz.height), depth, 3);
    cvCvtColor(img, gray, CV_BGR2GRAY);
    //gray= haar(img, gray);
    //edge(img);
    //colorEdge(img, gray);
    glg(img);
    keys(img, gray);
    //ocr();
    //cvShowImage("img",img);
    //cvWaitKey(0);
    cvReleaseImage(&img);
    cvReleaseImage(&copy);
    cvReleaseImage(&gray);
    return 0;
}
