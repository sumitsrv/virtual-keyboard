#include <stdio.h>
#include "cv.h"
#include "highgui.h"

#define PI 3.14159265 

void toHue(IplImage *,  int *); 
 
IplImage *img0, *img1, *img2, *img3;
int maxh=-255, minh=255, maxs=-255, mins=255, maxv=-255, minv=255;
int *huearr;
 
void mouseHandler(int event, int x, int y, int flags, void *param)
{
	int h, s, v, r, g, b;
	int step = img2->widthStep;
    uchar* data= (uchar*)img2->imageData;
    uchar* datargb = (uchar*)img0->imageData;
    switch(event) {
        /* left button down */
        case CV_EVENT_LBUTTONDOWN:
            fprintf(stdout, "(%d, %d).\t\t", x, y);
			h = data[(y)*step + x*3];
            //h = huearr[y*(step/3) +x];
            s = data[(y)*step + x*3 + 1];
            v = data[(y)*step + x*3 + 2];
            r = datargb[(y)*step + x*3 ];
            g = datargb[(y)*step + x*3 + 1];
            b = datargb[(y)*step + x*3 + 2];
            
            if(h>maxh)
            maxh=h;
            if(h<minh)
            minh=h;
            if(s>maxs)
            maxs=s;
            if(s<mins)
            mins=s;
            if(v>maxv)
            maxv=v;
            if(v<minv)
            minv=v;
            
            printf("h=%d\t s=%d\t v=%d\t\t r=%d\t g=%d\t b=%d\t H->%d-%d\t S->%d-%d\t V->%d-%d\n", h, s, v, r, g, b, minh, maxh, mins, maxs, minv, maxv);
            break;
       
        /* right button down */
        case CV_EVENT_RBUTTONDOWN:
            fprintf(stdout, "Right button down (%d, %d).\n", x, y);
            break;
       
        /* mouse move */
        case CV_EVENT_MOUSEMOVE:
           // /* draw a rectangle */img1 = cvCloneImage(img0);
            cvShowImage("image", img1);
            break;
    }
}
 
int main(int argc, char** argv)
{
    /* load an image */
    img0 = cvLoadImage(argv[1], -1);
    img2 = cvCreateImage(cvGetSize(img0), img0->depth, 3);
    int width = img0->width;
	int height = img0->height;
	
	huearr = (int*)malloc(width*height*sizeof(int));
	toHue(img0, huearr);
	
	cvCvtColor(img0, img2, CV_BGR2HSV);
    /* create new window and register mouse handler */
    cvNamedWindow("image", CV_WINDOW_AUTOSIZE);
    cvSetMouseCallback( "image", mouseHandler, NULL );
   
    /* display the image */
    cvShowImage("image", img0);
    cvWaitKey(0);
   
    cvDestroyWindow("image");
    cvReleaseImage(&img0);
    cvReleaseImage(&img1);
	cvReleaseImage(&img2);
    cvReleaseImage(&img3);
	
    return 0;
}
 

void toHue(IplImage *img, int *huearr)
{
	int width = img->width;
	int height = img->height;
	int stepimg = img->widthStep;
	int stepcopy = stepimg/3;
	int nchannels = img->nChannels;
	
	int i, j, k, intensity[4];
	float h;
	
	uchar* data = (uchar*)img->imageData;
		
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
			//copy_data[i*stepcopy + j] = (int) (h*180/PI);
			huearr[i*stepcopy + j] = (int)(h * 180/PI);
		}
	}
}

