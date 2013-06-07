#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define PI 3.14159265

void colorEdge(IplImage* img, IplImage* copy)
{
	int i, j, k, u[3], v[3], g_xx, g_yy, g_xy;
	double theta;
	int width = img->width;
	int height = img->height;
	int depth = img->depth;
	int step = img->widthStep;
	int nchannels = img->nChannels;
	
	//IplImage* copy = cvCreateImage(cvSize(width, height), depth, 1);
	IplImage* out = cvCreateImage(cvSize(width, height), depth, 1);
	
	uchar* data = (uchar*)img->imageData;
	uchar* copy_data = (uchar*)copy->imageData;
	
	for(i=1; i < height-1; i++)
	{
		for(j = 1; j < width-1; j++)
		{
			g_xx=0;
			g_yy=0;
			g_xy=0;
			for(k = 0; k < nchannels; k++)
			{
				u[k] = (data[(i+1)*step + (j-1)*nchannels + k] + 2*data[(i+1)*step + j*nchannels + k] + data[(i+1)*step + (j+1)*nchannels + k]) - (data[(i-1)*step + (j-1)*nchannels + k] + 2*data[(i-1)*step + j*nchannels + k] + data[(i-1)*step + (j+1)*nchannels + k]);
				v[k] = (data[(i-1)*step + (j+1)*nchannels + k] + 2*data[i*step + (j+1)*nchannels + k] + data[(i+1)*step + (j+1)*nchannels + k]) - (data[(i-1)*step + (j-1)*nchannels + k] + 2*data[(i)*step + (j-1)*nchannels + k] + data[(i+1)*step + (j-1)*nchannels + k]);
				
				g_xx = g_xx + pow(u[k],2);
				g_yy = g_yy + pow(v[k],2);
				g_xy = g_xy + u[k]*v[k];
			}
			
			if(g_xx-g_yy != 0)
				theta = atan(2*g_xy/(g_xx-g_yy));
			else
				theta = (0.5)*PI;
			//theta = 45*PI/180;
			if(theta < 0)
				theta = theta + PI;
					
			theta = (0.5)*theta;
				
			copy_data[i*step/nchannels + j] = pow(((0.5)*((g_xx + g_yy) + ((g_xx - g_yy)*cos(2*theta)) + (2 * g_xy * sin(2*theta)))),0.5);
		}
	}
	
	//cvCanny(copy, out, 50, 100, 3);
	//cvDilate(copy, out, 0, 1);
	//cvErode(out, copy, 0, 1);
	
	//cvShowImage("image",img);
	//cvShowImage("colorEdge",copy);
	//cvWaitKey(0);
	cvReleaseImage(&out);
	//cvReleaseImage(&copy);
}
