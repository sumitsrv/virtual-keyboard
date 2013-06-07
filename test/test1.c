#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>

void Haar(IplImage*, IplImage*, int);
void make_icon(IplImage*, IplImage*, int, int);
void up_Scale(IplImage*, IplImage*, int);

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
    
    int threshold;
   	int width = img->width;
	int height = img->height;
	int depth = img->depth;
	int nchannels = img->nChannels;
	
	printf("Enter the threshold to which you want to apply the transform : ");
	scanf("%d",&threshold);
	
	cvShowImage("Image",img);
			
	//printf("width=%d\theight=%d\tnchannels=%d\tdepth=%d\n",width,height,nchannels,depth);
	IplImage *copy = cvCreateImage( cvSize(width,height), depth, nchannels);
	cvCopy(img, copy, NULL);
	
	//down=pow(2,level);
	
	IplImage* downscaled = cvCreateImage( cvSize((width/2), (height/2)), depth, nchannels);
	
	cvNamedWindow("Image",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("MyHaar",CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("Upscaled-Haar",CV_WINDOW_AUTOSIZE);
	
	cvMoveWindow("Image",100,100);
	cvMoveWindow("MyHaar",510,100);
	
	cvCopy(copy, img, NULL);
	Haar(copy, img, 1);
		
	make_icon(copy, downscaled, 2, threshold);
	
	//printf("Enter the level to which the image is to be scaled : ");
	///scanf("%d",&level);
	//up=pow(2,level);
	//IplImage* upscaled = cvCreateImage(cvSize(width*up/down, height*up/down), depth, nchannels);
	
	cvShowImage("MyHaar",copy);
	//up_Scale(downscaled, upscaled, level);
	cvShowImage("Down-scaled-Haar",downscaled);
	
	cvWaitKey(0);
	cvDestroyWindow("MyHaar");
	cvDestroyWindow("Image");
	//cvDestroyWindow("Upscaled-Haar");
	cvReleaseImage(&downscaled);
	cvReleaseImage(&img);
	cvReleaseImage(&copy);
	//cvReleaseImage(&upscaled);
	
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
	printf("%d\t%d\t%d\n",max[0],max[1],max[2]);
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

void make_icon(IplImage *src, IplImage* dest, int scale, int threshold)
{
	int width = dest->width;
	int height = dest->height;
	int step = src->widthStep;
	int nchannels = dest->nChannels;
	int i, j, k;
	
	uchar *data = (uchar*)src->imageData;
	
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			for(k=0; k<nchannels; k++)
			{
				(dest->imageData)[((i*step)/scale) + j*nchannels + k] = 255;
				//(dest->imageData)[((i*step)/scale) + j*nchannels + k] = (src->imageData)[i*step + j*nchannels + k];
				if(data[(i*step) + j*nchannels + width + k] >= threshold)
				{
					(dest->imageData)[((i*step)/scale) + j*nchannels + k] = 127;
				}
				if(data[((i+height)*step) + j*nchannels + k] >= threshold)
				{
					(dest->imageData)[((i*step)/scale) + j*nchannels + k] = 127;
				}
				
			}
		}
	}
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
