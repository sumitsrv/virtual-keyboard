#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>

void Haar(IplImage*, IplImage*, int);
void make_icon(IplImage*, IplImage*, int);


int main(int argc, char * argv[])
{
	IplImage *img=0;
	
	if( argc < 2 )
	{
        fprintf( stderr, "Usage: loadimg <filename>\n" );
        return 1;
    }
	
	img = cvLoadImage(argv[1],-1);
	
	if( img == 0 )
	{
        fprintf( stderr, "Cannot load file %s!\n", argv[1] );
        return 1;
    }
    
    int level, i, up, down;
   	int width = img->width;
	int height = img->height;
	int depth = img->depth;
	int nchannels = img->nChannels;
	
	cvShowImage("Image",img);
	IplImage *copy = cvCreateImage( cvSize(width,height), depth, nchannels);
	cvCopy(img, copy, NULL);
	
	cvCopy(copy, img, NULL);
	Haar(copy, img, 1);
	
	IplImage* downscaled = cvCreateImage(cvSize(width/2, height/2), depth, nchannels);
	
	make_icon(copy, downscaled, 2);
	
	cvNamedWindow("Image",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("MyHaar",CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("Upscaled-Haar",CV_WINDOW_AUTOSIZE);
	
	cvMoveWindow("Image",100,100);
	cvMoveWindow("MyHaar",510,100);
	cvShowImage("MyHaar",copy);
	cvShowImage("Downscaled-Haar",downscaled);
	
	cvWaitKey(0);
	cvDestroyWindow("MyHaar");
	cvDestroyWindow("Image");
	cvDestroyWindow("Downscaled-Haar");
	cvReleaseImage(&downscaled);
	cvReleaseImage(&img);
	cvReleaseImage(&copy);
	
return 0;
}


void Haar(IplImage *img, IplImage *copy, int scale)
{
	int width = copy->width;
	int height = copy->height;
	int step = copy->widthStep;
	int nchannels = copy->nChannels;
	
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
	int step = src->widthStep;
	int nchannels = dest->nChannels;
	int i, j, k;
	
	cvSetImageROI(src, cvRect(0, 0, width, height));
	cvCopy(src, dest);
	cvResetImageROI(src);
}
