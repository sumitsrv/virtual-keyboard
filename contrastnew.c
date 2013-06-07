#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define alpha 0.8

void mergeLevels(CvHistogram*, float *, int*, float[], float[]);
void lookUpTable(float *, float [], float [], int*, float []);

void glg(IplImage *img)
{
	int s, scale = 3, hist_size=256, n=0, i, j;
	CvHistogram *hist, *hist_glg;
	
	CvSize sz = cvSize( img->width & -2, img->height & -2 );
   	IplImage* gray = cvCreateImage(cvSize(sz.width, sz.height), 8, 1);
	IplImage* copy = cvCreateImage(cvSize(sz.width, sz.height), 8, 3);
	
	cvSetImageROI(img, cvRect(0, 0, sz.width, sz.height));
	
	if(img->nChannels == 3)
	{
		cvCvtColor(img, copy, CV_BGR2HSV);
		cvSetImageCOI(copy, 3);
		cvCopy(copy, gray, 0);
	}
	else
	{
		cvCopy(img, gray, 0);
	}
	
	int width = gray->width;
	int height = gray->height;
	int step = gray->widthStep;
	
	uchar* data = (uchar*)gray->imageData;
		
	float range[] = {0,256};
	float *ranges[] = {range};
	float max_value=0, min_value=0;
	float left[256], right[256], T[256];
	float N;
	
	hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	hist_glg = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	cvClearHist(hist_glg);
	
	//IplImage* hist_img = cvCreateImage( cvSize(255*3,255*3), 8, 3 );
    //IplImage* hist_img_glg = cvCreateImage( cvSize(255*3,255*3), 8, 3 );
      
	cvCalcHist(&gray, hist, 0, NULL);
	cvGetMinMaxHistValue( hist, &min_value, &max_value, 0, 0 );
    //cvZero( hist_img );
	
	float *bin_glg;
	
	for( s = 0; s < hist_size; s++ )
    {
          float bin_val = cvQueryHistValue_1D( hist, s );
          int intensity = cvRound(bin_val*255/max_value);
          //cvRectangle( hist_img, cvPoint( s*scale, 0 ), cvPoint( (s+1)*scale - 1,intensity), CV_RGB(255,255,255), CV_FILLED );
          
          if(bin_val != 0)
          {
			  bin_glg = cvGetHistValue_1D(hist_glg,n);
			  *bin_glg = bin_val;
			  //printf("%f\t",bin_val);
			  left[n] = s;
			  right[n] = s;
			  //intensity = cvRound(bin_val*255/max_value);
              //cvRectangle( hist_img_glg, cvPoint( n*scale, 0 ), cvPoint( (n+1)*scale - 1,intensity), CV_RGB(255,255,255), CV_FILLED );
              n++;
          }    
    }
    
    //printf("%f %f\n",min_value, max_value);
	//printf("%d\n\n",n);
          
	//cvShowImage( "H-S Histogram", hist_img );
    for(i=n-40; i>0; i--)
    {
		//cvZero(hist_img_glg);
		cvGetMinMaxHistValue( hist_glg, &min_value, &max_value, 0, 0 );
    	mergeLevels(hist_glg, &max_value, &n, left, right);
		
		for( s = 0; s < n; s++ )
		{
          float bin_val = cvQueryHistValue_1D( hist_glg, s );
          int intensity = cvRound(bin_val*255/max_value);
          //printf("%f\t",bin_val); 
          //cvRectangle( hist_img_glg, cvPoint( s*scale, 0 ), cvPoint( (s+1)*scale - 1,intensity), CV_RGB(255,255,255), CV_FILLED );
		}
		//cvShowImage( "H-S Histogram2", hist_img_glg );
	    //cvWaitKey(0);
	}
	
	lookUpTable(&N, left, right, &n, T);
	//cvShowImage( "H-S Histogram2", hist_img_glg );
		
	for(i=0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			data[i*step + j] = T[data[i*step + j]];
		}
	}
	
	if(img->nChannels == 3)
	{
		cvCopy(gray, copy, 0);
		cvSetImageCOI(copy, 0);	
		cvCvtColor(copy, img, CV_HSV2BGR);	
	}
	else
	{
		cvCopy(gray, img, 0);
	}
	cvResetImageROI(img);
	cvReleaseImage(&gray);
	cvReleaseImage(&copy);
}

void mergeLevels(CvHistogram* hist_glg, float *max_value, int *hist_size, float left[], float right[])
{
	int n, i=0, index=0, index_sec=0;
	float val_prev=0, val_next=0, min_sec=0;
	float min_value=*max_value;
	
	for( n = 0; n < *hist_size; n++ )
    {   
		float bin_val = cvQueryHistValue_1D( hist_glg, n );
		if(min_value > bin_val)
		{
			min_value = bin_val;
			index = n;
		}	
    }
    if(index>0 && index<(*hist_size)-1)
	{
		//printf("Hi1\t");
		val_prev = cvQueryHistValue_1D(hist_glg, index-1);
		val_next = cvQueryHistValue_1D(hist_glg, index+1);
		if(val_prev <= val_next)
		{
			min_sec = val_prev;
			index_sec = index - 1;
		}	
		else if(val_prev > val_next)
		{
			min_sec = val_next;
			index_sec = index + 1;
		}	
	}
	else if(index==0)
	{
		val_next = cvQueryHistValue_1D(hist_glg, 1);
		val_prev = val_next;
		min_sec = val_next;
		index_sec = 1;
	}
	else if(index==(*hist_size)-1)
	{	
		//printf("Hi2\t");
		val_prev = cvQueryHistValue_1D(hist_glg, index-1);
		val_next = val_prev;
		min_sec = val_prev;
		index_sec = index-1;
	}
	
	if(index_sec < index) 
		i=index_sec;
	else i=index;
	
	float *bin_glg = cvGetHistValue_1D(hist_glg, i);
	float *bin_glg_next = cvGetHistValue_1D(hist_glg, i+1);
	*bin_glg = *bin_glg + *bin_glg_next;
	left[i] = left[i];
	right[i] = right[i+1];
	i++;
	 	
	for(; i<(*hist_size)-1; i++)
	{
		bin_glg = cvGetHistValue_1D(hist_glg, i);
		bin_glg_next = cvGetHistValue_1D(hist_glg, i+1);
		*bin_glg = *bin_glg_next;
		left[i] = left[i+1];
		right[i] = right[i+1];
	}
	
	bin_glg = cvGetHistValue_1D(hist_glg, (*hist_size) - 1);
	*bin_glg=0;		 
	
	left[*hist_size -1]=0;
	right[*hist_size -1]=0;
	
	(*hist_size)-- ;
    //printf("%f\t%d\t%d\n",min_value, index, *hist_size);    
 
}


void lookUpTable(float *N, float left[], float right[], int* hist_size, float T[])
{
	int i, j, curr_bin=0;
	
	if(left[0] == right[0])
	{
		*N = 255/(*hist_size -alpha);
	}
	else
	{
		*N = 255/(*hist_size);
	}
	
	for(i=0; i<256; i++)
	{
		if(i > left[0] && i < right[*hist_size -1])
		{
			for(j=curr_bin; j<*hist_size ; j++)
			{
				if(i >= left[j] && i <= right[j])
				{
					curr_bin = j;
					//printf("%d %d -> %f -- %f\t", i, curr_bin, left[curr_bin], right[curr_bin]);
					break;
				}
				else if(i < left[j+1] && i > right[j])
				{
					break;
				}
					
			}
			
			if(left[curr_bin] != right[curr_bin])
			{
				if(left[0] == right[0])
				{
					T[i] = (curr_bin + 1 - alpha - ((right[curr_bin]-i)/(right[curr_bin]-left[curr_bin])))*(*N) +1;
				}
				else
				{
					T[i] = (curr_bin + 1 - ((right[curr_bin]-i)/(right[curr_bin]-left[curr_bin])))*(*N) +1;
				}
			}
			else if((left[curr_bin] == right[curr_bin]) || (i < left[curr_bin+1] && i > right[curr_bin]))
			{
				if( left[0] == right[0])
				{
					T[i] = (curr_bin+1-alpha)*(*N);
				}
				else
				{
					T[i] = (curr_bin+1)* (*N);
				}
			}
		}
		else if(i <= left[0])
		{
			T[i] = 0;
		}
		else if(i >= right[*hist_size -1])
		{
			T[i] = 255;
		}
		
		//printf("%d %f\n", i, T[i]);
	}
}
   

