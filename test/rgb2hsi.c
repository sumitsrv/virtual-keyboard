#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define PI 3.14159265
int low, high;

int hsi(IplImage*);
void hsi2(IplImage *);
void toHue(IplImage *, IplImage *, int*);
void toIntensity(IplImage *, IplImage *, float*);
void toSaturation(IplImage *, IplImage *, float*);
void toRGB(IplImage*, int*, float*, float*);
void segmentation(int*, float*, float*, int, int);

int main(int argc, char * argv[])
{
	CvCapture *capture = cvCaptureFromCAM(0);
	IplImage  *frame = 0;
    int key = 0;
    
	printf("Enter the range for the hue to be preserved (low, high): ");
	scanf("%d %d",&low, &high);
	while(key != 27) 
	{
		/* get a frame */
        frame = cvQueryFrame( capture );
 
        /* always check */
        if( !frame ) break;
		hsi(frame);
		key = cvWaitKey(10);
       //printf("%d  %c\t",key, key);
    }
	return 0;
}

void hsi2(IplImage *img)
{
	IplImage *hsv = cvCloneImage(img);
	IplImage *h = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *s = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	cvCvtColor(img, hsv, CV_BGR2HSV);
	//cvShowImage("rgb",img);
	cvSetImageCOI(hsv,1);
	cvCopy(hsv, h, 0);
	cvThreshold( h, h, 40, 255, CV_THRESH_BINARY_INV );
    cvCopy(h, hsv, 0);
	           
	cvSetImageCOI(hsv,2);
	cvCopy(hsv, s, 0);
	cvThreshold( s, s, 35, 255, CV_THRESH_BINARY );
    cvCopy(s, hsv, 0);
	
	cvSetImageCOI(hsv, 0);
	cvCvtColor(hsv, img, CV_HSV2BGR);
	cvShowImage("img",img);
	cvReleaseImage(&hsv);
	cvReleaseImage(&s);
	cvReleaseImage(&h);
	
        
}

int hsi(IplImage* img)
{
	int *huearr;
	float *intarr, *satarr;
	
	int width = img->width;
	int height = img->height;
	int depth = img->depth;
	int nchannels = img->nChannels;
	
	//cvShowImage("RGB-Image(Original)",img);
			
	IplImage *intensity = cvCreateImage( cvSize(width,height), depth, 1);
	IplImage *saturation = cvCreateImage(cvSize(width,height), depth, 1);
	IplImage *hue = cvCreateImage(cvSize(width,height), depth, 1);
	IplImage *hsiimg = cvCreateImage(cvSize(width, height), depth, nchannels);
	
	huearr = (int*)malloc(width*height*sizeof(int));
	satarr = (float*)malloc(width*height*sizeof(int));
	intarr = (float*)malloc(width*height*sizeof(int));
	
	toIntensity(img, intensity, intarr);
	toSaturation(img, saturation, satarr);
	toHue(img, hue, huearr);
	
	segmentation(huearr, satarr, intarr, height, width);
	
	toRGB(hsiimg, huearr, satarr, intarr);
	
	//cvShowImage("Intensity-Image", intensity);
	//cvShowImage("Saturation-Image", saturation);
	//cvShowImage("Hue-Image", hue);
	cvShowImage("HSI2RGB-Image", hsiimg);
	
	//cvNamedWindow("RGB-Image(Original)",CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("HSI-Image",CV_WINDOW_AUTOSIZE);
	//cvMoveWindow("RGB-Image(Original)",100,100);
	//cvMoveWindow("HSI-Image",100,500);
	
	
	//cvDestroyWindow("Image");

	cvReleaseImage(&intensity);
	cvReleaseImage(&saturation);
	cvReleaseImage(&hsiimg);
	
return 0;
}

void toIntensity(IplImage *img, IplImage *copy, float *intarr)
{
	int width = img->width;
	int height = img->height;
	int stepimg = img->widthStep;
	int stepcopy = copy->widthStep;
	int nchannels = img->nChannels;
	
	int i, j, k, intensity=0;
	
	uchar* data = (uchar*)img->imageData;
	uchar* copy_data = (uchar*)copy->imageData;
		
	for(i=0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			intensity = 0;
			for(k = 0; k < nchannels; k++)
			{
				intensity = intensity + data[i*stepimg + j*nchannels + k]; 
			}
			copy_data[i*stepcopy + j] = intensity/3;
			intarr[i*stepcopy + j] = intensity/3;
		}
	}
}


void toSaturation(IplImage *img, IplImage *copy, float* satarr)
{
	int width = img->width;
	int height = img->height;
	int stepimg = img->widthStep;
	int stepcopy = copy->widthStep;
	int nchannels = img->nChannels;
	
	int i, j, k, intensity[4], min=0;
	
	uchar* data = (uchar*)img->imageData;
	uchar* copy_data = (uchar*)copy->imageData;
		
	for(i=0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			intensity[3] = 0;
			min = 256;
			for(k = 0; k < nchannels; k++)
			{
				intensity[k] = data[i*stepimg + j*nchannels + k]; 
				if(min >= intensity[k])
					min = intensity[k];
				intensity[3] = intensity[3] + intensity[k];
			}
			copy_data[i*stepcopy + j] = (1.0 - (float)3*min/intensity[3])*100;
			satarr[i*stepcopy + j] = (1.0 - (float)3*min/intensity[3])*100;
		}
	}
}

void toHue(IplImage *img, IplImage *copy, int *huearr)
{
	int width = img->width;
	int height = img->height;
	int stepimg = img->widthStep;
	int stepcopy = copy->widthStep;
	int nchannels = img->nChannels;
	
	int i, j, k, intensity[4];
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
			//if(h<200 || h>240)
			//h=0; 
			copy_data[i*stepcopy + j] = (int) (h*180/PI);
			huearr[i*stepcopy + j] = (int)(h * 180/PI);
		}
	}
}

void toRGB(IplImage* img, int* huearr, float* satarr, float* intarr) 
{
	int width = img->width;
	int height = img->height;
	int stepimg = img->widthStep;
	int nchannels = img->nChannels;
	
	uchar* data = (uchar*)img->imageData;
	
	int i, j, k, intensity[3];
	float h, s, I, x, y, z;
	
	
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			h = (float)huearr[i*stepimg/nchannels +j];
			s = satarr[i*stepimg/nchannels +j];
			I = intarr[i*stepimg/nchannels +j];
			h = h*PI/180;
			s = s/100;
			I = I/255;
			
			x = I*(1-s);
						
			if(h<PI/3)
			{
				y = I*(1+ (s*cos(h)/cos(PI/3 - h)));
				z = 3*I - (x + y);
				intensity[2] = x*255;
				intensity[0] = y*255;
				intensity[1] = z*255;
			}
			else if(h>=(2*PI/3) && h<(4*PI/3))
			{
				h = h - (2*PI/3);
				y = I*(1+ (s*cos(h)/cos(PI/3 - h)));
				z = 3*I - (x + y);
				intensity[2] = z*255;
				intensity[0] = x*255;
				intensity[1] = y*255;
			} 	
			else if(h>=(4*PI/3) && h<(2*PI))
			{
				h = h - (4*PI/3);
				y = I*(1+ (s*cos(h)/cos(PI/3 - h)));
				z = 3*I - (x + y);
				intensity[2] = y*255;
				intensity[0] = z*255;
				intensity[1] = x*255;
			}
			
			for(k=0; k<nchannels; k++)
			{
				data[i*stepimg + j*nchannels + k] = intensity[k];
			}
		}
	}
	
}


void segmentation(int* huearr, float* satarr, float* intarr, int height, int width)
{
	
	int i, j;
	
	for(i=0 ; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			if(huearr[i*width +j] < low || huearr[i*width +j] > high)
			{
				//printf("%d\t",huearr[i*width + j]);
				huearr[i*width +j] = 0;
				satarr[i*width +j] = 0;
				intarr[i*width +j] = 0;
			}
		}
	}
}
