#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ml.h>

void findX(IplImage*, int*, int*);
void findY(IplImage*, int*, int*);
CvRect findBB(IplImage*);
IplImage preprocessing(IplImage*, int, int);
float classify(IplImage*, int);
void test();
void getData();
void ocr();

const int k = 10;
int train_sample_count = 9, classes = 2, train_samples = 3;
CvMat* trainData = cvCreateMat( train_sample_count, 2, CV_32FC1 );
CvMat* trainClasses = cvCreateMat( train_sample_count, 1, CV_32FC1 );
CvKNearest* knn = new CvKNearest( trainData, trainClasses, 0, false, k);

void findX(IplImage* imgSrc,int* min, int* max)
{
	int i;
	int minFound=0;
	CvMat data;
	CvScalar maxVal=cvRealScalar(imgSrc->width * 255);
	CvScalar val=cvRealScalar(0);
	//For each col sum, if sum < width*255 then we find the min
	//then continue to end to search the max, if sum< width*255 then is new max
	for (i=0; i< imgSrc->width; i++)
	{
		cvGetCol(imgSrc, &data, i);
		val= cvSum(&data);
		if(val.val[0] < maxVal.val[0])
		{
			*max= i;
			if(!minFound)
			{
				*min= i;
				minFound= 1;
			}
		}
	}
}

void findY(IplImage* imgSrc,int* min, int* max)
{
	int i;
	int minFound=0;
	CvMat data;
	CvScalar maxVal=cvRealScalar(imgSrc->width * 255);
	CvScalar val=cvRealScalar(0);
	//For each col sum, if sum < width*255 then we find the min
	//then continue to end to search the max, if sum< width*255 then is new max
	for (i=0; i< imgSrc->height; i++)
	{
		cvGetRow(imgSrc, &data, i);
		val= cvSum(&data);
		if(val.val[0] < maxVal.val[0])
		{
			*max=i;
			if(!minFound)
			{
				*min= i;
				minFound= 1;
			}
		}
	}
}

CvRect findBB(IplImage* imgSrc)
{
	CvRect aux;
	int xmin, xmax, ymin, ymax;
	xmin=xmax=ymin=ymax=0;
	
	findX(imgSrc, &xmin, &xmax);
	findY(imgSrc, &ymin, &ymax);
	
	aux=cvRect(xmin, ymin, xmax-xmin, ymax-ymin);
	
	//printf("BB: %d,%d - %d,%d\n", aux.x, aux.y, aux.width, aux.height);
	
	return aux;

}

IplImage preprocessing(IplImage* imgSrc, int new_width, int new_height)
{
	IplImage* result;
	IplImage* scaledResult;

	CvMat data;
	CvMat dataA;
	CvRect bb;//bounding box
	CvRect bba;//boundinb box maintain aspect ratio

	//Find bounding box
	bb=findBB(imgSrc);
	
	//Get bounding box data and no with aspect ratio, the x and y can be corrupted
	cvGetSubRect(imgSrc, &data, cvRect(bb.x, bb.y, bb.width, bb.height));
	//Create image with this data with width and height with aspect ratio 1
	//then we get highest size betwen width and height of our bounding box
	//printf("%d %d\n", bb.height, bb.width);
	int size=(bb.width>bb.height)?bb.width:bb.height;
	result=cvCreateImage( cvSize( size, size ), 8, 1 );
	cvSet(result,CV_RGB(255,255,255),NULL);
	//Copy de data in center of image
	//int x=(int)floor((float)(size-bb.width)/2.0f);
	//int y=(int)floor((float)(size-bb.height)/2.0f);
	int x = bb.x;
	int y = bb.y;
	cvGetSubRect(result, &dataA, cvRect(x,y,bb.width, bb.height));
	cvCopy(&data, &dataA, NULL);
	//Scale result
	scaledResult=cvCreateImage( cvSize( new_width, new_height ), 8, 1 );
	cvResize(result, scaledResult, CV_INTER_NN);
	
	//Return processed data
	return *scaledResult;
	//return result;
}

void ocr()
{
	int i, filecount, x[4], y[4], temp;
	char* filename=(char*)calloc(50,1);
	IplImage scaledimage;
	IplImage *imgsrc;
	FILE *keyinfo, *keycount;
	
	keycount = fopen("./ocr_temp_data/keycount.dat","r");
	fscanf(keycount, "%d", &filecount);
	keyinfo = fopen("./ocr_temp_data/keyinfo.dat","r");
	
	for(i=0; i<filecount; i++)
	{
		fscanf(keyinfo, "%d\t%s\t%d %d %d %d %d %d %d %d\n", &temp, filename, &x[0], &y[0], &x[1], &y[1], &x[2], &y[2], &x[3], &y[3]);	
		//printf("%s\n", filename);
		imgsrc = cvLoadImage(filename, -1);
		cvThreshold(imgsrc, imgsrc, 150, 255, CV_THRESH_BINARY|CV_THRESH_OTSU);
		scaledimage = preprocessing(imgsrc, 60, 60);
		//cvShowImage("scaled image", &scaledimage);
		//cvShowImage("Original Key", imgsrc);
		//cvWaitKey(0);
	}
	cvReleaseImage(&imgsrc);
}


void getData()
{
	IplImage* src_image;
	IplImage prs_image;
	CvMat row,data;
	char file[255];
	int i,j;
	char *file_path = (char*)calloc(50, 1);
	file_path = strcat(file_path, "./ocr_temp_data/test/");
	int size = 60;
	
	for(i =0; i<classes; i++)
	{
		for( j = 0; j< train_samples; j++)
		{
			//Load file
			if(j<10)
			sprintf(file,"%s%d/%d_%d.jpg",file_path, i, i , j);
			else
			sprintf(file,"%s%d/%d_%d.jpg",file_path, i, i , j);
			src_image = cvLoadImage(file,0);
			if(!src_image)
			{
				printf("Error: Cant load image %s\n", file);
				//exit(-1);
			}
			//process file
			prs_image = preprocessing(src_image, size, size);
	
			//Set class label
			cvGetRow(trainClasses, &row, i*train_samples + j);
			cvSet(&row, cvRealScalar(i));
			//Set data
			cvGetRow(trainData, &row, i*train_samples + j);
		
			IplImage* img = cvCreateImage( cvSize( size, size ), IPL_DEPTH_32F, 1 );
			//convert 8 bits image to 32 float image
			cvConvertScale(&prs_image, img, 0.0039215, 0);
	
			cvGetSubRect(img, &data, cvRect(0,0, size,size));

			CvMat row_header, *row1;
			//convert data matrix sizexsize to vecor
			row1 = cvReshape( &data, &row_header, 0, 1 );
			cvCopy(row1, &row, NULL);
		}
	}
}


void test()
{
	IplImage* src_image;
	IplImage prs_image;
	CvMat row,data;
	char file[255];
	int i,j;
	int error=0;
	int testCount=0;
	int size = 60;
	char *file_path = (char*)calloc(50,1);
	file_path = strcat(file_path, "./ocr_temp_data/test/");
	for(i =0; i<classes; i++)
	{
		for( j = 50; j< 50+train_samples; j++)
		{
			sprintf(file,"%s%d/%d_%d.jpg",file_path, i, i , j);
			src_image = cvLoadImage(file,0);
			if(!src_image)
			{
				printf("Error: Cant load image %s\n", file);
				//exit(-1);
			}
			//process file
			prs_image = preprocessing(src_image, size, size);
			float r = classify(&prs_image, 0);
			if((int)r!=i)
				error++;
		
			testCount++;
		}
	}
	float totalerror=100*(float)error/(float)testCount;
	printf("System Error: %.2f%%\n", totalerror);
	
}


float classify(IplImage* img, int showResult)
{
	IplImage prs_image;
	CvMat data;
	CvMat* nearest=cvCreateMat(1,k,CV_32FC1);
	float result;
	int size = 60;
	//process file
	prs_image = preprocessing(img, size, size);
	
	//Set data
	IplImage* img32 = cvCreateImage( cvSize( size, size ), IPL_DEPTH_32F, 1 );
	cvConvertScale(&prs_image, img32, 0.0039215, 0);
	cvGetSubRect(img32, &data, cvRect(0,0, size,size));
	CvMat row_header, *row1;
	row1 = cvReshape( &data, &row_header, 0, 1 );

	result=knn->find_nearest(row1,k,0,0,nearest,0);
	
	int accuracy=0;
	for(int i=0;i<k;i++)
	{
		if( nearest->data.fl[i] == result)
		accuracy++;
	}
	float pre=100*((float)accuracy/(float)k);

	if(showResult==1)
	{
		printf("|\t%.0f\t| \t%.2f%%&nbsp; \t| \t%d of %d \t| \n",result,pre,accuracy,k);
		printf(" ---------------------------------------------------------------\n");
	}

	return result;
}

int  main()
{
	return 0;
}
