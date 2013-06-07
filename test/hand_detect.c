#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>


int main(int argc, char *argv[])
{
	IplImage *img=cvLoadImage(argv[1], -1);
	IplImage *hsv = cvCloneImage(img);
	IplImage *h = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *s = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	cvCvtColor(img, hsv, CV_BGR2HSV);
	cvShowImage("rgb",img);
	cvSetImageCOI(hsv,1);
	cvCopy(hsv, h, 0);
	cvThreshold( h, h, 30, 255, CV_THRESH_BINARY_INV );
    cvCopy(h, hsv, 0);
	           
	cvSetImageCOI(hsv,2);
	cvCopy(hsv, s, 0);
	cvThreshold( s, s, 40, 255, CV_THRESH_BINARY );
    cvCopy(s, hsv, 0);
	
	cvSetImageCOI(hsv, 0);
	cvCvtColor(hsv, img, CV_HSV2BGR);
	cvShowImage("img",img);
	cvShowImage("hsv",hsv);
	cvShowImage("h",h);
	cvShowImage("s",s);
	
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvReleaseImage(&hsv);
}
	
