#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<stdio.h>

#define FRAMERUN 50

void tips(IplImage *img);
void correlated_tips();

int height, width, step, count_bgupdate=0;
int tips_position[10][2], tipcount=0, oldtips[FRAMERUN][11][4], oldtipflag=0; //oldtips[FRAMERUN][i][0] -> x-cordinate of tip, [i][1] -> y-cordinate of tip, [i][2] -> direction of movement, [i][3] -> base frame
int  posmax=0;	
float speed[10][7]; //speed[][0]->speed; [][1]->forward movement duration; [][2]->backward movement duration; [][3]->point of change (x); [][4]->point of change (y); [][5]->speed flag; [][6]->direction at point of deflection

int main()
{
	int c = 0, i = 0;
    CvCapture* capture = cvCaptureFromCAM(0);

	if(!cvQueryFrame(capture))
	{
		printf("Video capture failed, please check the camera.");
	}
	else
	{
		printf("Video camera capture status: OK");
	}

    CvSize sz = cvGetSize(cvQueryFrame( capture));
	
	height = sz.height;
    width = sz.width;
    step = sz.width;
    
    IplImage* src = cvCreateImage( sz, 8, 3 );
	IplImage* hsv_image = cvCreateImage( sz, 8, 3);
	IplImage* hsv_mask = cvCreateImage( sz, 8, 1);
	IplImage* handview = cvCreateImage(sz, 8, 1);
	CvScalar  hsv_min = cvScalar(5, 70, 0, 0);
	CvScalar  hsv_max = cvScalar(20, 150, 255, 0); //H-> 0-20

	while( c != 27)
	{
		//printf("%d\t\t",framecount);
		src = cvQueryFrame( capture);
		cvCvtColor(src, hsv_image, CV_BGR2HSV);
		cvInRangeS (hsv_image, hsv_min, hsv_max, hsv_mask);
		cvSmooth(hsv_mask, handview, CV_MEDIAN, 5, 0, 0, 0);
		cvDilate(handview, handview, NULL, 3);
		//cvDilate(hsv_mask, handview, NULL, 1);
		//cvErode(handview, handview, NULL, 1);
		//cvDilate(handview, handview, NULL, 1);
		
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contour = 0;
		cvFindContours(handview, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
		cvZero(handview);
		
		for( ; contour != 0; contour = contour->h_next )
		{
			// replace CV_FILLED with 1 to see the outlines         
			double area = cvContourArea( contour, CV_WHOLE_SEQ, 0);
			if(area > 500)
			{
				cvDrawContours( handview, contour, cvScalar( 255 ), cvScalar( 255 ), 0, 0, 8 );
				//max = area;
			}
		}
		//cvShowImage("handview", handview);
		
		tips(handview);

		//cvNamedWindow( "hsv-msk",1);
		//cvShowImage( "hsv-msk", hsv_mask); //hsv_mask->origin = 1; 
		
		for(i=0; i<tipcount; i++)
		{
			if(posmax == i)
				cvCircle(src, cvPoint(tips_position[posmax][1], tips_position[posmax][0]), 3, cvScalar(0,255,0), 2, 8, 0);	
			else
				cvCircle(src, cvPoint(tips_position[i][1], tips_position[i][0]), 3, cvScalar(255,0,0), 2, 8, 0);
			

			if(speed[i][1] > 1 && speed[i][2] > 1 && (int)speed[i][5] == 1 && oldtips[(oldtipflag+1)%FRAMERUN][i][2] == 1)
			{	
				cvCircle(src, cvPoint(speed[i][4], speed[i][3]), 5, cvScalar(0,0,255), 3, 8, 0);
				speed[i][1] = 0;
				speed[i][2] = 0;
				speed[i][5] = 0;
				//printf("check\t");
			}
			else if(speed[i][1] > 1 && speed[i][2] > 1  && oldtips[(oldtipflag+1)%FRAMERUN][i][2] == -1)
			{	
				//cvCircle(src, cvPoint(speed[posmax][4], speed[posmax][3]), 9, cvScalar(0,0,255), 3, 8, 0);
				speed[i][1] = speed[i][2];
				speed[i][2] = 0;
				speed[i][5] = 0;
				//printf("check\t");
			}
		}
					
		//printf("%d\t%d\t%d", (int)speed[3][1], (int)speed[3][2], (int)speed[3][5]);
			
		//printf("\n");
		/*if(speed[posmax][1] > 1 && speed[posmax][2] > 1 && oldtips[(oldtipflag+1)%FRAMERUN][posmax][2] == 1)
		{	
			cvCircle(src, cvPoint(speed[posmax][4], speed[posmax][3]), 5, cvScalar(0,0,255), 3, 8, 0);
			speed[posmax][1]=0;
			speed[posmax][2]=0;
			//printf("check\t");
		}
		else if(speed[posmax][1] > 1 && speed[posmax][2] > 1 && oldtips[(oldtipflag+1)%FRAMERUN][posmax][2] == -1)
		{	
			//cvCircle(src, cvPoint(speed[posmax][4], speed[posmax][3]), 5, cvScalar(0,0,255), 3, 8, 0);
			speed[posmax][1]=speed[posmax][2];
			speed[posmax][2]=0;
			//printf("check\t");
		}*/
		
		//sprintf(framename, "./frames/frame%d.jpg", framecount++);
		//cvSaveImage(framename, src);
			
		//maxspeed = posmax;
		oldtipflag = (oldtipflag+1)%FRAMERUN;	
		
		//printf("%d\t%f\t3\t%f\n", posmax, speed[posmax][0], speed[3][0]);
		cvNamedWindow( "src",1);
		cvShowImage( "src", src);

		c = cvWaitKey( 10);
	}

	cvReleaseCapture( &capture);
	cvReleaseImage(&hsv_image);
	cvReleaseImage(&hsv_mask);
	cvReleaseImage(&handview);
	cvDestroyAllWindows();
}

void tips(IplImage *img)
{
	FILE *tipgraph = fopen("tipgraph.dat","w");
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
		fprintf(tipgraph, "%d\n", x[i]);
			//printf("hi");
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
		if((cvGetReal1D(src, i+1) - cvGetReal1D(src, i) <= 0 && cvGetReal1D(src, i) - cvGetReal1D(src, i-1) >= 0) 
		//|| (cvGetReal1D(src, i+1) - cvGetReal1D(src, i) <= 0 && cvGetReal1D(src, i+1) - cvGetReal1D(src, i) >=-5 && cvGetReal1D(src, i) - cvGetReal1D(src, i-1) <= 0 && cvGetReal1D(src, i) - cvGetReal1D(src, i-1) >= -5)
		)
		{
			tips_position[tipcount][0] = 0;
			for(j=-3; j<=3; j++)
			{
				if((i+j)<640 && (i+j)>0)
				{
					tips_position[tipcount][0] +=  x[i+j];
				}
			}
			tips_position[tipcount][0] = (tips_position[tipcount][0])/7;
				
			if(tips_position[tipcount][0] > 40)
			{
				if((tipcount > 1) && (i-tips_position[tipcount-1][1]) < 55)
				{
					tips_position[tipcount-1][0] = (tips_position[tipcount-1][0] + tips_position[tipcount][0]) / 2;
					tips_position[tipcount-1][1] = (tips_position[tipcount-1][1] + i) / 2;
				}
				else
				{
					tips_position[tipcount++][1] = i;
				}
			}
		}
		
		fprintf(tipgraph, "%f\n", cvGetReal1D(src, i));
	}
    //printf("%d\t",tipcount);
	fclose(tipgraph);
	correlated_tips();
	//fclose(freq);		
}


void correlated_tips() // oldtips[][][0] -> y, oldtips[][][1] -> x, oldtips[][][2] -> direction of movement, oldtips[][][3] -> reference frame of speed
{
	int i=0, j=0, distx=0, disty=0, notfound=0;
	
	for(j=0; j<oldtips[oldtipflag][10][0]; j++)
	{
		for(i=j; i<=j; i++)
		{
			distx = abs(oldtips[oldtipflag][j][0]-tips_position[i][0]);
			disty = abs(oldtips[oldtipflag][j][1]-tips_position[i][1]);
			//printf("%d\t%d\t\t",(oldtips[oldtipflag][j][0]-tips_position[i][0]), (oldtips[oldtipflag][j][1]-tips_position[i][1]));
			if(distx <= 300 && disty <= 300)
			{
				/*flag = tips_position[i][0];
				tips_position[i][0] = tips_position[j][0];
				tips_position[j][0] = flag;
				flag = tips_position[i][1];
				tips_position[i][1] = tips_position[j][1];
				tips_position[j][1] = flag;
				*/
				if(oldtips[oldtipflag][j][0]-tips_position[i][0] != 0)
					oldtips[(oldtipflag+1)%FRAMERUN][j][2] = (oldtips[oldtipflag][j][0]-tips_position[i][0]) / abs(oldtips[oldtipflag][j][0]-tips_position[i][0]);
				else oldtips[(oldtipflag+1)%FRAMERUN][j][2] = oldtips[oldtipflag][j][2];
				
				if(oldtips[(oldtipflag+1)%FRAMERUN][j][2] - oldtips[oldtipflag][j][2] == 0 )
				{
					oldtips[(oldtipflag+1)%FRAMERUN][j][3] = oldtips[oldtipflag][j][3];
				}
				else
				{
					oldtips[(oldtipflag+1)%FRAMERUN][j][3] = oldtipflag;
				}
				break;
			}
			else notfound++;
		}
	}
	
	int max = 0;
	float speedscale=1;
	posmax = -1;
	
	for(i=0; i<tipcount; i++)
	{
		speedscale = (((oldtipflag +1)%FRAMERUN - oldtips[(oldtipflag+1)%FRAMERUN][i][3] + FRAMERUN)%FRAMERUN)+1;
		speed[i][0] = abs(pow((oldtips[oldtips[(oldtipflag+1)%FRAMERUN][i][3]][i][0]-tips_position[i][0]),2))/speedscale;
		//printf("%d\t",oldtips[(oldtipflag+1)%FRAMERUN][i][3]);
		//speed[i][5]=0;
		if(speed[i][0] > max && speed[i][0] > 30  && oldtips[(oldtipflag+1)%FRAMERUN][i][2] == -1)
		{
			max = speed[i][0];
			//printf("%d\t",oldtips[(oldtipflag+1)%FRAMERUN][j][2]);
			posmax=i;
		}
	}
	
	for(i=0; i<tipcount; i++)
	{
		speedscale = (((oldtipflag +1)%FRAMERUN - oldtips[(oldtipflag+1)%FRAMERUN][i][3] + FRAMERUN)%FRAMERUN)+1;
		//speed[i][0] = abs(pow((oldtips[oldtips[(oldtipflag+1)%FRAMERUN][i][3]][i][0]-tips_position[i][0]),2))/speedscale;
		//printf("%d\t",oldtips[(oldtipflag+1)%FRAMERUN][i][3]);
		
		if(speedscale == 2)
		{
			//printf("hi");
			if(speed[i][1] == 0 || speed[i][1] == 1)
			{
				speed[i][1] = 1;
				if(posmax == i)
				speed[i][5] = 1;
			}
			else if(speed[i][1] !=0 && speed[i][2] == 0)
			{
				speed[i][2]++;
				speed[i][3] = oldtips[oldtipflag][i][0];
				speed[i][4] = oldtips[oldtipflag][i][1];
				speed[i][6] = oldtips[(oldtipflag+1)%FRAMERUN][i][2];
			}
			else if(speed[i][1] != 0 && speed[i][2] !=0)
			{
				speed[i][1] = 1;
				speed[i][2] = 0;
				if(posmax == i)
				{
					speed[i][5] = 1;
				}
				else speed[i][5] = 0;
			}
		}
		else
		{
			if(speed[i][1] != 0 && speed[i][2] == 0)
			{
				speed[i][1]++;
				if(posmax == i)
				speed[i][5] = 1;
			}
			else if(speed[i][1] != 0 && speed[i][2] != 0)
			{
				speed[i][2]++;
			}
		}
		
		
		/*
		distx = abs(oldtips[oldtipflag][i][0]-tips_position[i][0]);
		disty = abs(oldtips[oldtipflag][i][1]-tips_position[i][1]);
		
		if(distx <= 0 && disty <= 0)
		{
			tips_position[i][0] = oldtips[oldtipflag][i][0];
			tips_position[i][1] = oldtips[oldtipflag][i][1];
		}
		*/
		oldtips[(oldtipflag+1)%FRAMERUN][i][0] = tips_position[i][0];
		oldtips[(oldtipflag+1)%FRAMERUN][i][1] = tips_position[i][1];
		//printf("%.1f\t%.1f\t%d\t%d\t%d\t\t",speed[i][1], speed[i][2], oldtips[(oldtipflag+1)%FRAMERUN][i][0], oldtips[(oldtipflag+1)%FRAMERUN][i][1], oldtips[(oldtipflag+1)%FRAMERUN][i][2]);
		//printf("%f\t",speedscale);
	}
	//printf("direction->%d\treference frame->%d\tcurrent frame->%d\t",oldtips[(oldtipflag+1)%FRAMERUN][3][2], oldtips[(oldtipflag+1)%FRAMERUN][3][3], oldtipflag);
	oldtips[(oldtipflag+1)%FRAMERUN][10][0] = tipcount;
	//printf("%d  %d\n", oldtips[oldtipflag][10][0], notfound);
}

/*void correlated_tips()
{
	int i=0, j=0, dist=0, flag=0, notfound=0;
	for(i=0; i<tipcount; i++)
	{
		flag = 0;
		for(j=0; j<oldtips[oldtipflag][10][0]; j++)
		{
			dist = abs(oldtips[oldtipflag][j][0]-tips_position[i][0]) + abs(oldtips[oldtipflag][j][1]-tips_position[i][1]);
			//printf("%d\t%d\n",(oldtips[oldtipflag][j][0]-tips_position[i][0]), (oldtips[oldtipflag][j][1]-tips_position[i][1]));
			if(dist <= 200)
			{
				oldtips[(oldtipflag+1)%10][j][0] = tips_position[i][0];
				oldtips[(oldtipflag+1)%10][j][1] = tips_position[i][1];
				flag = 1;
			}
		}
		if(flag==0)
		{
			oldtips[(oldtipflag+1)%10][oldtips[oldtipflag][10][0] + notfound][0] = tips_position[i][0];
			oldtips[(oldtipflag+1)%10][oldtips[oldtipflag][10][0] + notfound][1] = tips_position[i][1];
			notfound++;
			//printf("%d\t", notfound);
		}
	}
	oldtips[(oldtipflag+1)%10][10][0] = oldtips[oldtipflag][10][0] + notfound;
	printf("%d  %d\n", oldtips[oldtipflag][10][0], notfound);
	oldtipflag = (oldtipflag+1)%10;
}
*/
