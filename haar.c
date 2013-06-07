#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>

void Haar(IplImage*, IplImage*, int);
void make_icon(IplImage*, IplImage*, int);
void up_Scale(IplImage* img, IplImage *upscale, int scale);

IplImage* haar(IplImage* img, IplImage* gray)
{
   	int depth = img->depth;
	int nchannels = img->nChannels;
	
	CvSize sz = cvSize( img->width & -2, img->height & -2 );
   
	cvShowImage("Image",img);
	IplImage *copy = cvCreateImage( sz, depth, nchannels);
	cvSetImageROI(img, cvRect(0, 0, sz.width, sz.height));
	cvCopy(img, copy, NULL);
	
	//cvCopy(copy, img, NULL);
	Haar(copy, img, 1);
	
	IplImage* downscaled = cvCreateImage(cvSize(sz.width/2, sz.height/2), depth, nchannels);
	IplImage* upscaled = cvCreateImage(cvSize(sz.width, sz.height), depth, nchannels);
	
	make_icon(copy, downscaled, 2);
	up_Scale(downscaled, upscaled, 1);
	cvCvtColor(upscaled, gray, CV_BGR2GRAY);
	
	cvNamedWindow("Image",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("MyHaar",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Upscaled-Haar",CV_WINDOW_AUTOSIZE);
	
	cvMoveWindow("Image",100,100);
	cvMoveWindow("MyHaar",510,100);
	cvShowImage("MyHaar",copy);
	cvShowImage("Downscaled-Haar",downscaled);
	cvShowImage("Upscaled-Haar", gray);
	
	cvWaitKey(0);
	cvDestroyWindow("MyHaar");
	cvDestroyWindow("Image");
	cvDestroyWindow("Downscaled-Haar");
	cvReleaseImage(&downscaled);
	cvReleaseImage(&upscaled);
	cvReleaseImage(&copy);
	
	return gray;
}


void Haar(IplImage *img, IplImage *copy, int scale)
{
	int width = img->width;
	int height = img->height;
	int step = img->widthStep;
	int nchannels = img->nChannels;
	
	CvSize sz = cvSize( img->width & -2, img->height & -2 );
	
	int* arr = (int*)malloc(width*height*nchannels*sizeof(int));
	int i, j, k, intensity[4], max[3];
	max[0]=0;
	max[1]=0;
	max[2]=0;
	
	uchar* data = (uchar*)img->imageData;
	uchar* copy_data = (uchar*)copy->imageData;
	
	width=width/scale;
	height=height/scale;
	
	for(i=0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			for(k = 0; k < nchannels; k++)
			{
				intensity[0] = copy_data[i*step + j*nchannels + k];
				intensity[1] = copy_data[i*step + (j+1)*nchannels + k];
				intensity[2] = copy_data[(i+1)*step + j*nchannels + k];
				intensity[3] = copy_data[(i+1)*step + (j+1)*nchannels + k];
								
				arr[(i/2)*step + (j/2)*nchannels + k] = (0.5)*(intensity[0] + intensity[1] + intensity[2] + intensity[3]);		
				if(arr[(i/2)*step + (j/2)*nchannels + k] >= max[k])
				{
					max[k] = arr[(i/2)*step + (j/2)*nchannels + k];
				}
				//data[(i/2)*step + (j/2)*nchannels + k] = (0.25)*(intensity[0] + intensity[1] + intensity[2] + intensity[3]);
				data[(i/2)*step + ((width/2) + (j/2))*nchannels + k] = (0.5)*(abs(intensity[0]  - intensity[1] + intensity[2] - intensity[3]));
				
				data[(height/2 + i/2 )*step + (j/2)*nchannels + k] = (0.5)*(abs(intensity[0]  + intensity[1] - intensity[2] - intensity[3]));
				data[(height/2 + i/2)*step + ((width/2) + (j/2))*nchannels + k] = (0.5)*(abs(intensity[0]  - intensity[1] - intensity[2] + intensity[3]));
			}
			j++;
		}
		i++;
	}
	
	for(k=0; k<nchannels; k++)
	{
		if(max[k] <=255)
		{
			max[k] = 255;
		}
	}

	for(i=0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			for(k = 0; k < nchannels; k++)
			{
				data[(i/2)*step + (j/2)*nchannels + k] = (arr[(i/2)*step + (j/2)*nchannels + k] * 255/max[k]);
			}
			j++;
		}
		i++;
	}
}

void make_icon(IplImage *src, IplImage* dest, int scale)
{
	int width = dest->width;
	int height = dest->height;
	int step = dest->widthStep;
	int nchannels = dest->nChannels;
	int i, j ,k;
	
	uchar* data = (uchar*)dest->imageData;
	cvSetImageROI(src, cvRect(0, height, width, height));
	cvCopy(src, dest);
	cvResetImageROI(src);
	cvSetImageROI(src, cvRect(width, 0, width, height));
	uchar* srcData = (uchar*)src->imageData;
	
	for(i = 0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			for(k=0; k<nchannels ; k++)
			{
				if(data[i*step + j*nchannels + k] <= srcData[i*(2*step) + (j+width)*nchannels + k])
				{
					data[i*step + j*nchannels + k] = srcData[i*(2*step) + (j+width)*nchannels + k];
				}
			}
		}
	}
	cvResetImageROI(src);
}


void up_Scale(IplImage* img, IplImage *upscale, int scale)
{
	int width = img->width;
	int height = img->height;
	int step = img->widthStep;
	int nchannels = img->nChannels;
	
	uchar* data = (uchar*)img->imageData;
	uchar* newdata = (uchar*)upscale->imageData;
	
	int i, j, k, p, q, intensity;
	
	scale = pow(2,scale);
	
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			for(k=0; k<nchannels; k++)
			{
				intensity = data[i*step + j*nchannels + k];
				for(p=0; p<scale; p++)
				{
					for(q=0; q<scale; q++)
					{
						newdata[((scale*i)+p)*step*scale + ((scale*j)+q)*nchannels + k] = intensity;
					}
				}
			}
		}
	}
}

