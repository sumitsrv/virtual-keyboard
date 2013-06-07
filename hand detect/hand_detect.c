#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>

#define PI 3.14159265

void y(IplImage*);
void toHue(IplImage *, IplImage *);

int main(int argc, char *argv[])
{
	IplImage *img=cvLoadImage(argv[1], -1);
	IplImage *hsv = cvCloneImage(img);
	IplImage *h = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *s = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	//cvCvtColor(img, hsv, CV_BGR2HSV);
	cvShowImage("rgb",img);
	y(img);
	/*cvSetImageCOI(hsv,1);
	cvCopy(hsv, h, 0);
	cvThreshold( h, h, 38, 255, CV_THRESH_BINARY_INV );
	//cvDilate(h, h, NULL,1);
	//cvErode(h, h, NULL,1);
    cvCopy(h, hsv, 0);
	           
	cvSetImageCOI(hsv,2);
	cvCopy(hsv, s, 0);
	//cvThreshold( s, s, 40, 255, CV_THRESH_BINARY );
    cvCopy(s, hsv, 0);
	
	cvSetImageCOI(hsv, 0);
	cvCvtColor(hsv, img, CV_HSV2BGR);
	cvShowImage("img",img);
	cvShowImage("hsv",hsv);
	cvShowImage("h",h);
	cvShowImage("s",s);
	*/
	cvWaitKey(0);
	cvReleaseImage(&img);
	cvReleaseImage(&hsv);
	cvReleaseImage(&h);
	cvReleaseImage(&s);
}
	

void y(IplImage *img)
{
	int i, j;
	IplImage *hsv = cvCloneImage(img);
	cvCvtColor(img, hsv, CV_BGR2HSV);
	IplImage *img1 = cvCloneImage(hsv);
	
	IplImage *h = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *h2 = cvCreateImage(cvSize(hsv->width, hsv->height), img->depth, 1);
	IplImage *s = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *v = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *imggray1 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray2 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray3 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray4 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
		
	int width = h2->width;
	int height = h2->height;
	
	//cvCvtColor(img, hsv, CV_BGR2HSV);
	cvShowImage("hsv",hsv);
	
	cvSetImageCOI(hsv,1);
	cvCopy(hsv, h, 0);
	//toHue(img, h2);
	//cvCopy(h, h2, 0);
	cvCopy(hsv, imggray1, 0);
	uchar* data = (uchar*)imggray1->imageData;
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			if((data[i*width + j] >=0 && data[i*width + j] <=35) || (data[i*width + j] >=160 && data[i*width + j] <=180))
				{
					data[i*width + j] = 255;
				}
			else data[i*width + j] = 0;
		}
	}
	//cvInRangeS(h, cvScalar(0),cvScalar(35), imggray1);
	//cvThreshold( h, h, 35, 255, CV_THRESH_TOZERO_INV );
	//cvThreshold( imggray1, imggray1, 30, 255, CV_THRESH_BINARY_INV );
	//cvDilate(h, h, NULL,1);
	//cvErode(h, h, NULL,1);
	//cvShowImage("hue",h2);
    cvAnd(h, imggray1, h, 0);
    /*cvInRangeS(h2, cvScalar(-70),cvScalar(-100), imggray4);
	cvThreshold( h2, h2, -70, 255, CV_THRESH_TOZERO );
    cvShowImage("h2", h2);
    cvShowImage("imggray4", imggray4);
    //cvAnd(h2, imggray4, h2, 0);
    cvXor(h, h2, h, 0);
    cvAdd(imggray1, imggray4, imggray1, 0);
    */
    cvCopy(h, hsv, 0);
	           
	cvSetImageCOI(hsv,2);
	cvCopy(hsv, s, 0);
	cvInRangeS(s, cvScalar(20), cvScalar(130), imggray2);
	//cvThreshold( s, s, 130, 255, CV_THRESH_TOZERO_INV );
    //cvThreshold( imggray2, imggray2, 68, 255, CV_THRESH_BINARY_INV );
    cvAnd(s, imggray2, s,0);
    cvCopy(s, hsv, 0);
	
	cvSetImageCOI(hsv, 3);
	cvCopy(hsv, v, 0);
	/*cvInRangeS(v, cvScalar(40), cvScalar(150), imggray3);
	cvThreshold( v, v, 150, 255, CV_THRESH_TOZERO_INV );
    //cvThreshold( imggray2, imggray2, 68, 255, CV_THRESH_BINARY_INV );
    cvAnd(v, imggray3, v,0);
	*/cvAnd(imggray1, imggray2, imggray1, 0);
	cvAnd(imggray1, v, v,0);
	cvCopy(v, hsv, 0);
	
	cvSetImageCOI(hsv, 0);
	cvCvtColor(hsv, img1, CV_HSV2BGR);
	cvShowImage("img1",img1);
	//cvShowImage("hsv",hsv);
	
	cvShowImage("h",h);
	cvShowImage("s",s);
	cvShowImage("v",v);
	
	cvReleaseImage(&imggray1);
	cvReleaseImage(&imggray2);
	cvReleaseImage(&imggray3);
	cvReleaseImage(&imggray4);
	cvReleaseImage(&h2);
	cvReleaseImage(&h);
	cvReleaseImage(&s);
	cvReleaseImage(&v);
	cvReleaseImage(&hsv);
}


void toHue(IplImage *img, IplImage *copy)
{
	int width = img->width;
	int height = img->height;
	int stepimg = img->widthStep;
	int stepcopy = stepimg/3;
	int nchannels = img->nChannels;
	
	int i, j, k, intensity[4], h_final;
	float h;
	
	uchar* data = (uchar*)img->imageData;
	uchar* copy_data = (uchar*)copy->imageData;
		
	for(i=0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			
			intensity[3] = 0;
			for(k = 0; k < nchannels; k++)
			{
				intensity[k] = data[i*stepimg + j*nchannels + k]; 
				intensity[3] = intensity[3] + intensity[k];
			}
			
			h = 0.5*((float)(intensity[0]-intensity[1]) + (float)(intensity[0]-intensity[2]));
			h = h/sqrt(pow((float)(intensity[0]-intensity[1]),2) + (float)(intensity[0]-intensity[2])*(float)(intensity[1]-intensity[2]));
			h = acos(h);
			
			if(intensity[2] <= intensity[1])
			{
				h = h;
			}
			else
			{
				h = 2*PI - h;
			}
			h_final = (int)(h * 180/PI);
			
			if(h_final<200 || h_final>260)
			h_final=0;
			else h_final = 255;
			copy_data[i*stepcopy + j] = h_final;
			
		}
	}
}

