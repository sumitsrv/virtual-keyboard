#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>

#define LOW_Thresh 0
#define UPP_Thresh 35

void y(IplImage *, IplImage*);
void tips(IplImage *);

int height, width, step, count_bgupdate=0;
int tips_position[10][2], tipcount=0;	

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
    
    IplImage *imggray1;
    IplImage *imggray2;
    IplImage *imggray3;
    
	// grayscale buffers
    imggray1 = cvCreateImage( cvGetSize(imgsize), IPL_DEPTH_8U, 1);
    imggray2 = cvCreateImage( cvGetSize(imgsize), IPL_DEPTH_8U, 1);
    imggray3 = cvCreateImage( cvGetSize(imgsize), IPL_DEPTH_8U, 1);
    
    height = imgsize->height;
    width = imgsize->width;
    step = width;
    
	IplImage *hsv1 = cvCreateImage( cvGetSize(imgsize), IPL_DEPTH_8U, 3);
    IplImage *hsv2 = cvCreateImage( cvGetSize(imgsize), IPL_DEPTH_8U, 3);;
    int key = 0, i=0;
    
    while ( key != 27) {
        // load image one
        img2 = cvCloneImage( img1 );
        img1 = cvQueryFrame( capture );
        // convert rgb to grayscale
        // quit if user press 'q' and wait a bit between images
        cvCvtColor( img1, hsv2, CV_BGR2HSV );
		cvCvtColor(img1, imggray2, CV_BGR2GRAY);

        key = cvWaitKey( 10 );
        // load image two
        
        cvCvtColor( img2, hsv1, CV_BGR2HSV );
		cvCvtColor(img2, imggray1, CV_BGR2GRAY);
		y(hsv1, hsv2);
        
        for(i=0; i<tipcount; i++)
		{
			cvCircle(img2, cvPoint(tips_position[i][1], tips_position[i][0]), 1, cvScalar(255,0,0), 2, 8, 0);
		}
		cvNamedWindow("Source", CV_WINDOW_AUTOSIZE);
		cvMoveWindow("Source", 0, 100);
		cvShowImage("Source",img2);
   }

    cvReleaseCapture( &capture );
	cvReleaseImage(&imggray1);
    cvReleaseImage(&imggray2);
    cvReleaseImage(&imggray3);
    cvReleaseImage(&imgsize);
    cvReleaseImage(&img1);
    cvReleaseImage(&img2);
    cvReleaseImage(&hsv1);
	cvReleaseImage(&hsv2);
	
	return 0;
}

void y(IplImage *img1, IplImage *img2)
{
	IplImage *hsv1 = cvCloneImage(img1);
	IplImage *hsv2 = cvCloneImage(img2);
	IplImage *imgnew = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 3);
	IplImage *img = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 1);
	IplImage *img3 = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 1);
	IplImage *img4 = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 1);
	IplImage *h1 = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
	IplImage *h2 = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
	
	cvSetImageCOI(hsv1,1);
	cvCopy(hsv1, h1, 0);
	           
	cvSetImageCOI(hsv2,1);
	cvCopy(hsv2, h2, 0);
	
	cvAbsDiff(h1, h2, img);
	//cvShowImage("hue diff", img);
	// cvThreshold(img, img, 25, 255, CV_THRESH_BINARY);
	//cvSmooth(img, img, CV_MEDIAN, 3, 0, 0, 0);	
	
	cvSetImageCOI(hsv1,2);
	cvCopy(hsv1, h1, 0);
	cvSetImageCOI(hsv2,2);
	cvCopy(hsv2, h2, 0);
	cvAbsDiff(h1, h2, img3);
	//cvShowImage("sat diff", img3);
	
	cvSetImageCOI(hsv1,3);
	cvCopy(hsv1, h1, 0);
	cvSetImageCOI(hsv2,3);
	cvCopy(hsv2, h2, 0);
	cvAbsDiff(h1, h2, img4);
	//cvShowImage("val diff", img4);
	//cvThreshold(img4, img4, 25, 255, CV_THRESH_TOZERO|CV_THRESH_OTSU);
	//cvErode(img4, img4, NULL, 2);
	//cvShowImage("val diff new", img4);
	
	//cvAnd(img4, img4, img);
	cvAnd(img, img3, img);
	cvAnd(img, img4, img);
	cvThreshold(img, img, 0, 255, CV_THRESH_BINARY);
	//cvSmooth(img, img, CV_MEDIAN, 3, 0, 0, 0);
	//cvDilate(img, img, NULL, 2);
	//cvErode(img, img, NULL, 1);
	
	//new segment
	//cvShowImage("Old", img);
	cvSetImageCOI(hsv1,1);
	cvCopy(hsv1, h1, 0);
	//cvShowImage("H", h1);
	cvAnd(h1, img, h1, 0);
	
	int i, j;
	int height = h1->height;
	int width = h1->width;
	uchar* data = (uchar*)h1->imageData;
	
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			if((data[i*width + j] >=3 && data[i*width + j] <=35)|| (data[i*width + j] >=160 && data[i*width + j] <=180)) // 
				{
					data[i*width + j] = 255;
				}
			else data[i*width + j] = 0;
		}
	}
	//cvCopy(h1, hsv1, 0);
	
	//cvShowImage("H", h1);
	
	cvSetImageCOI(hsv1, 2);
	cvCopy(hsv1, h2);
	cvAnd(h2, img, h2, 0);
	cvInRangeS(h2, cvScalar(30), cvScalar(130), h2);
	//cvCopy(h2, hsv1, 0);
	
	cvSetImageCOI(hsv1, 3);
	cvCopy(hsv1, img3);
	cvAnd(h1, h2, h1, 0);
	//cvThreshold(img3, img3, 110, 255, CV_THRESH_TOZERO_INV);
	cvSmooth(h1, h1, CV_MEDIAN, 3, 0, 0, 0);
	//cvDilate(h1, h1, NULL, 2);
	//cvErode(h1, h1, NULL, 2);
	//cvDilate(h1, h1, NULL, 2);
	cvAnd(h1, img3, img3, 0);
	//cvCopy(img3, hsv1, 0);
	cvThreshold(img3, img3, 0, 255, CV_THRESH_BINARY);
	//cvDilate(img3, img3, NULL, 3);
	//cvSmooth(img3, img3, CV_MEDIAN, 5, 0, 0, 0);
	//cvErode(img3, img3, NULL, 3);
	cvNamedWindow("Hand", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("Hand", 650, 100);
	cvShowImage("Hand", img3);
	//cvShowImage("h1-new", h1);
	//cvSetImageCOI(hsv1, 0);
	//cvCvtColor(hsv1, imgnew, CV_HSV2BGR);
	//cvShowImage("New HSV", hsv1);
	//new segment done
	
	//cvShowImage("S", img3);
	//cvShowImage("V", img4);
	
	//cvCanny(img, img, 150, 150, 3);
	//cvShowImage("img", img);
	tips(img3);
	
	cvReleaseImage(&h1);
	cvReleaseImage(&h2);
	cvReleaseImage(&hsv1);
	cvReleaseImage(&hsv2);
	cvReleaseImage(&img);
	cvReleaseImage(&img3);
	cvReleaseImage(&img4);
	cvReleaseImage(&imgnew);
}

void tips(IplImage *img)
{
	//FILE *tipgraph = fopen("tipgraph.dat","w");
	//FILE *freq = fopen("freq.dat", "w");

	int i, j, curr_pixel, step, x[640];
	tipcount=0;
	uchar *data = (uchar*)img->imageData;
	step = img->widthStep;
	CvMat *dst = cvCreateMat(1, 640, CV_32FC1 );
    CvMat *src = cvCreateMat(1, 640, CV_32FC1 );
	
	for(i=0; i<width; i++)
	{
		x[i] = 0;
		for(j=0; j<height; j++)
		{
			curr_pixel = data[j*step + i];
			if((curr_pixel == 255) )
			{
				//data[x[i]*step + i] = 0;
				x[i] = j;
			}
			//else data[j*step + i] = 0;
		}
		cvSetReal1D(src, i, x[i]);
		//fprintf(tipgraph, "%d\n", x[i]);
	}
	
    cvDFT(src, dst, CV_DXT_SCALE, 0);
    for(i=0; i<width; i++)
    {
		//fprintf(freq, "%f\n", abs(cvGetReal1D(dst, i)));
		if(i>20) cvSetReal1D(dst, i, 0);
	}
    cvDFT(dst, src, CV_DXT_INVERSE, 0);
    for(i=1; i<width-1; i++)
    {
		if(cvGetReal1D(src, i+1) - cvGetReal1D(src, i) <= 0 && cvGetReal1D(src, i) - cvGetReal1D(src, i-1) >= 0  && cvGetReal1D(src, i) > 20)
		{
			tips_position[tipcount][0] = 0;
			for(j=-4; j<=4; j++)
			{
				if((i+j)<640 && (i+j)>0)
				{
					tips_position[tipcount][0] +=  x[i+j];
				}
			}
			tips_position[tipcount][0] = (tips_position[tipcount][0])/9;
			
			
			if((tipcount > 1) && (i-tips_position[tipcount-1][1]) < 10)
			{
				tips_position[tipcount-1][0] = (tips_position[tipcount-1][0] + tips_position[tipcount][0]) / 2;
				tips_position[tipcount-1][1] = (tips_position[tipcount-1][1] + i) / 2;
			}
			else
			{
				tips_position[tipcount++][1] = i;
			}
		}
		
		//fprintf(tipgraph, "%f\n", cvGetReal1D(src, i));
	}
    //printf("%d\t",tipcount);
	//fclose(tipgraph);
	//fclose(freq);
		
}

