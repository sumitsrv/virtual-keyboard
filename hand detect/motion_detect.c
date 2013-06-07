#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>

#define LOW_Thresh 0
#define UPP_Thresh 35
#define tou .10
#define framerate 1

void m(IplImage *);
void x(IplImage *, IplImage*, IplImage*);
void y(IplImage *, IplImage*);
void z(IplImage *);
void hsv_edge(IplImage *);
void tips(IplImage *);
void bgsub(IplImage *, IplImage *);

IplImage *avg, *stddev, *fg;
int height, width, step, count_bgupdate=0;
float *dataavg, *datastddev;
uchar  *datafg;
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
    
    avg = cvCreateImage(cvGetSize(imgsize), IPL_DEPTH_32F, 1);
	//cvCvtColor(imgsize, avg, CV_BGR2GRAY);
    stddev = cvCreateImage(cvGetSize(imgsize), IPL_DEPTH_32F, 1);
    fg = cvCreateImage(cvGetSize(imgsize), IPL_DEPTH_8U, 1);
    cvZero(stddev);
    height = imgsize->height;
    width = imgsize->width;
    step = width;
    dataavg = (float*)avg->imageData;
	datastddev = (float*)stddev->imageData;
	datafg = (uchar*)fg->imageData;
	
	uchar* data_source = (uchar*)img2->imageData;
	
    IplImage *hsv1 = cvCreateImage( cvGetSize(imgsize), IPL_DEPTH_8U, 3);
    IplImage *hsv2 = cvCreateImage( cvGetSize(imgsize), IPL_DEPTH_8U, 3);;
    int key = 0, i=0;
    
    while ( key != 1048603) {
        // load image one
        img2 = cvCloneImage( img1 );
        img1 = cvQueryFrame( capture );
        // convert rgb to grayscale
        // quit if user press 'q' and wait a bit between images
        cvCvtColor( img1, hsv2, CV_BGR2HSV );
		cvCvtColor(img1, imggray2, CV_BGR2GRAY);
		//hsv_edge(hsv2);
		//z(hsv2);
		//m(img2);
		
        key = cvWaitKey( 35 );
        // load image two
        
        cvCvtColor( img2, hsv1, CV_BGR2HSV );
		cvCvtColor(img2, imggray1, CV_BGR2GRAY);
		//display difference
        //x(imggray1, imggray2, hsv2);
        y(hsv1, hsv2);
        //cvAbsDiff(imggray1, imggray2, imggray3);
        //cvThreshold(imggray3, imggray3, 0, 255, CV_THRESH_TOZERO|CV_THRESH_OTSU);
        //bgsub(imggray3, img2);
        
        for(i=0; i<tipcount; i++)
		{
			cvCircle(img2, cvPoint(tips_position[i][1], tips_position[i][0]), 1, cvScalar(255,0,0), 2, 8, 0);
		}
		cvNamedWindow("Source", CV_WINDOW_AUTOSIZE);
		cvMoveWindow("Source", 0, 100);
		cvShowImage("Source",img2);
        
        //cvSmooth(imggray1, imggray1, CV_MEDIAN, 3, 0, 0, 0);
        //cvSmooth(imggray2, imggray2, CV_MEDIAN, 3, 0, 0, 0);
        //cvNamedWindow( "video", win );
        //key = cvWaitKey( 5 );
   }

    cvReleaseCapture( &capture );
    //cvDestroyWindow( "video" );
	cvReleaseImage(&imggray1);
    cvReleaseImage(&imggray2);
    cvReleaseImage(&imggray3);
    cvReleaseImage(&imgsize);
    cvReleaseImage(&img1);
    cvReleaseImage(&img2);
    cvReleaseImage(&hsv1);
	cvReleaseImage(&hsv2);
	cvReleaseImage(&stddev);
	cvReleaseImage(&avg);
	cvReleaseImage(&fg);
	
	return 0;
}

void bgsub(IplImage *img, IplImage *imgcolor)
{
	int i, j, pn_new;
	
	uchar* data = (uchar*)img->imageData;
	double alpha = tou*framerate,  temp,  pavg;
	IplImage *view = cvCreateImage(cvGetSize(img), img->depth, 1);
	//IplImage *imgblk = cvCreateImage(cvGetSize(img), img->depth, 1);
	
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			pn_new = data[i*step + j];
			//printf("%f\t", datastddev[i*step + j]); 
			//getchar();
			if(pn_new > (3*datastddev[i*step + j] + dataavg[i*step + j]))
			{
				datafg[i*step + j] = pn_new;
			}
			else
			{
				datafg[i*step + j] = 0;
			}		
			//if(count_bgupdate<100)
			{
				pavg = pn_new*alpha + (1-alpha)*abs(dataavg[i*step + j]);
				dataavg[i*step + j] = pavg;
				datastddev[i*step + j] = (abs(pn_new - pavg))*alpha + (1-alpha)*abs(datastddev[i*step + j]);
			}
			//printf("%d %f\t", pn_new, pavg);
			//temp = (abs(pn_new - pavg))*alpha;
			//if(fmod(temp, 1.0) > 0.5)
			//temp = ceil(temp);
			//else temp = floor(temp);
			//printf("%f\t", pavg);
			//getchar();	
		}
		
	}
	count_bgupdate++;
	//cvThreshold(avg, avg, 0, 255, CV_THRESH_TOZERO|CV_THRESH_OTSU);
	cvThreshold(fg, view, 0, 255, CV_THRESH_BINARY|CV_THRESH_OTSU);		
	//cvInRangeS(imgcolor, cvScalar(0, 30, 0), cvScalar(150, 130, 255), imgblk);
	//cvShowImage("img", img);
	//cvShowImage("avg", avg);
	cvShowImage("stddev", stddev);
	tips(view);
	//cvShowImage("img-black", imgblk);
	//cvDilate(fg, fg, NULL, 1);
	//cvShowImage("fg", fg);
	cvShowImage("view", view);
	tips(view);
	cvReleaseImage(&view);
	//cvReleaseImage(&imgblk);
}

void x(IplImage *imggray1, IplImage *imggray2, IplImage *img)
{
	IplImage *imggray3 = cvCreateImage(cvGetSize(imggray1), IPL_DEPTH_8U, 1);
	IplImage *hsv1 = cvCloneImage(img);
	IplImage *imgnew = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
	IplImage *img3 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *img4 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *h1 = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
	IplImage *h2 = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
	
	cvAbsDiff( imggray1, imggray2, imggray3 );
	//cvThreshold(imggray3, imggray3, 30, 255, CV_THRESH_BINARY);
    cvThreshold(imggray3, imggray3, 25, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    cvShowImage("img", imggray3);
    
    cvSetImageCOI(hsv1,1);
	cvCopy(hsv1, h1, 0);
	//cvShowImage("H", h1);
	cvAnd(h1, imggray3, h1, 0);
	
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
	cvCopy(h1, hsv1, 0);
	
	cvSetImageCOI(hsv1, 2);
	cvCopy(hsv1, h2);
	cvAnd(h2, imggray3, h2, 0);
	cvInRangeS(h2, cvScalar(30), cvScalar(130), h2);
	cvCopy(h2, hsv1, 0);
	
	cvSetImageCOI(hsv1, 3);
	cvCopy(hsv1, img3);
	cvAnd(h1, h2, h1, 0);
	cvAnd(h1, img3, img3, 0);
	cvCopy(img3, hsv1, 0);
	
	cvShowImage("img3-new", img3);
	cvShowImage("h1-new", h1);
	cvReleaseImage(&h1);
	cvReleaseImage(&h2);
	cvReleaseImage(&hsv1);
	cvReleaseImage(&img3);
	cvReleaseImage(&img4);
	cvReleaseImage(&imgnew);
    cvReleaseImage(&imggray3);
    
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
	cvDilate(img3, img3, NULL, 1);
	cvSmooth(img3, img3, CV_MEDIAN, 3, 0, 0, 0);
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


void hsv_edge(IplImage *img)
{
	IplImage *hsv = cvCloneImage(img);
	IplImage *img1 = cvCloneImage(img);
	
	IplImage *h = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *s = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *imggray1 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray2 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray3 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *edge = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	
	//cvCvtColor(img, hsv, CV_BGR2HSV);
	//cvShowImage("hsv",hsv);
	//cvInRangeS(hsv, cvScalar(LOW_Thresh, 30, 0), cvScalar(UPP_Thresh, 255, 255), imggray1);
	
	cvSetImageCOI(hsv,1);
	cvCopy(hsv, h, 0);
	cvCopy(hsv, imggray1, 0);
	cvInRangeS(h, cvScalar(LOW_Thresh),cvScalar(UPP_Thresh), imggray1);
	cvThreshold( h, h, UPP_Thresh, 255, CV_THRESH_TOZERO_INV );
	cvSmooth(imggray1, imggray1, CV_MEDIAN, 7, 0, 0, 0);
	//cvThreshold( imggray1, imggray1, 38, 255, CV_THRESH_BINARY_INV );
	//cvDilate(h, h, NULL,1);
	//cvErode(h, h, NULL,3);
	cvAnd(h, imggray1, h,0);
    //cvCopy(h, hsv, 0);
	           
	cvSetImageCOI(hsv,2);
	cvCopy(hsv, s, 0);
	cvInRangeS(s, cvScalar(30), cvScalar(150), imggray2);
	cvThreshold( s, s, 30, 150, CV_THRESH_TOZERO );
    cvSmooth(imggray2, imggray2, CV_MEDIAN, 5, 0, 0, 0);
    //cvThreshold( imggray2, imggray2, 68, 255, CV_THRESH_BINARY_INV );
    cvAnd(s, imggray2, s,0);
    //cvCopy(s, hsv, 0);
	
	cvSetImageCOI(hsv, 3);
	cvCopy(hsv, imggray3, 0);
	//cvInRangeS(imggray3, cvScalar(35), cvScalar(100), imggray3);
	//cvThreshold(imggray3, imggray3, 0, 255, CV_THRESH_TOZERO);
	cvSmooth(imggray3, imggray3, CV_MEDIAN, 5, 0, 0, 0);
	cvAnd(imggray1, imggray3, imggray3,0);
	//cvCopy(imggray3, hsv, 0);
	
	
	//cvSetImageCOI(hsv, 0);
	//cvCvtColor(hsv, img1, CV_HSV2BGR);
	//cvCvtColor(img1, edge, CV_BGR2GRAY);
	//cvThreshold(edge, edge, 0, 255, CV_THRESH_BINARY);
	//cvSmooth(edge, edge, CV_MEDIAN, 3, 0, 0, 0);
	//cvCanny(edge, edge, 150, 200, 3);
	//cvShowImage("edge",edge);
	cvAnd(h, s, imggray1, 0);
	cvAnd(imggray1, imggray3, imggray3, 0);
	cvThreshold(imggray3, imggray3, 0, 255, CV_THRESH_BINARY);
	//tips(edge);	
	cvShowImage("img",imggray3);
		
	//cvShowImage("hsv",hsv);
	
	//cvShowImage("h",imggray1);
	//cvShowImage("s",s);
	//cvShowImage("v",imggray3);
	
	cvReleaseImage(&imggray1);
	cvReleaseImage(&imggray2);
	cvReleaseImage(&imggray3);
	cvReleaseImage(&edge);
	cvReleaseImage(&img1);
	cvReleaseImage(&h);
	cvReleaseImage(&s);
	cvReleaseImage(&hsv);
}


void z(IplImage *img)
{
	IplImage *hsv = cvCloneImage(img);
	IplImage *img1 = cvCloneImage(img);
	
	IplImage *imggray1 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray2 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray3 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray4 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray5 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	
	IplImage *edge = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	
	cvInRangeS(hsv, cvScalar(LOW_Thresh, 30, 0), cvScalar(UPP_Thresh, 130, 255), imggray2);
	cvShowImage("hsv",imggray2);
	cvDilate(imggray2, imggray1, NULL, 2);
	cvCopy(imggray1, imggray4, 0);
	
	CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contour = 0;
    //cvThreshold( imggray1, imggray1, 1, 255, CV_THRESH_BINARY );
    cvNamedWindow( "Source", 1 );
    cvShowImage( "Source", imggray1 );

    cvFindContours( imggray1, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
    cvZero( imggray3 );
	double max = 0;
    
    for( ; contour != 0; contour = contour->h_next )
    {
        /* replace CV_FILLED with 1 to see the outlines */        
       double area = cvContourArea( contour, CV_WHOLE_SEQ, 0);
       if(area > 5000)
       {
		   cvDrawContours( imggray3, contour, cvScalar( 255 ), cvScalar( 255 ), 0, CV_FILLED, 8 );
		   //max = area;
	   }
    }
    
    
    cvErode(imggray3, imggray2, NULL, 2);
	//cvAnd(imggray3, imggray4, imggray2);
	cvCanny(imggray2, imggray2, 0, 0, 3);
    cvDilate(imggray2, imggray3, NULL, 2);
    cvNamedWindow( "Components", 1 );
    cvShowImage( "Components", imggray3 );
	
	
	cvReleaseImage(&imggray1);
	cvReleaseImage(&imggray2);
	cvReleaseImage(&imggray3);
	cvReleaseImage(&edge);
	cvReleaseImage(&img1);
	cvReleaseImage(&imggray4);
	cvReleaseImage(&imggray5);
	cvReleaseImage(&hsv);
}


void m(IplImage *img)
{
	int i, j;
	IplImage *hsv = cvCloneImage(img);
	cvCvtColor(img, hsv, CV_BGR2HSV);
	IplImage *img1 = cvCloneImage(hsv);
	
	IplImage *h = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *s = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *v = cvCreateImage(cvSize(hsv->width, hsv->height), hsv->depth, 1);
	IplImage *imggray1 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray2 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray3 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage *imggray4 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
		
	int width = h->width;
	int height = h->height;
	
	//cvCvtColor(img, hsv, CV_BGR2HSV);
	cvShowImage("hsv",hsv);
	
	cvSetImageCOI(hsv,1);
	cvCopy(hsv, h, 0);
	cvCopy(hsv, imggray1, 0);
	uchar* data = (uchar*)imggray1->imageData;
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			if((data[i*width + j] >=0 && data[i*width + j] <=35) || (data[i*width + j] >=160 && data[i*width + j] <=180))
				{
					data[i*width + j] = 255;
				}
			else data[i*width + j] = 0;
		}
	}
	cvAnd(h, imggray1, h, 0);
    cvCopy(h, hsv, 0);
	           
	cvSetImageCOI(hsv,2);
	cvCopy(hsv, s, 0);
	cvInRangeS(s, cvScalar(20), cvScalar(130), imggray2);
	cvAnd(s, imggray2, s,0);
    cvCopy(s, hsv, 0);
	
	cvSetImageCOI(hsv, 3);
	cvCopy(hsv, v, 0);
	cvAnd(imggray1, imggray2, imggray1, 0);
	cvAnd(imggray1, v, v,0);
	cvCopy(v, hsv, 0);
	
	cvSetImageCOI(hsv, 0);
	cvCvtColor(hsv, img1, CV_HSV2BGR);
	cvShowImage("img1",img1);
	//cvShowImage("hsv",hsv);
	
	cvShowImage("h",h);
	cvShowImage("s",s);
	cvShowImage("v",v);
	
	cvReleaseImage(&imggray1);
	cvReleaseImage(&imggray2);
	cvReleaseImage(&imggray3);
	cvReleaseImage(&imggray4);
	cvReleaseImage(&h);
	cvReleaseImage(&s);
	cvReleaseImage(&v);
	cvReleaseImage(&hsv);
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
		if(cvGetReal1D(src, i+1) - cvGetReal1D(src, i) <= 0 && cvGetReal1D(src, i) - cvGetReal1D(src, i-1) >= 0 )
		{
			//tipcount++;
			tips_position[tipcount][0] =  int(cvGetReal1D(src, i));
			tips_position[tipcount++][1] = i;
		}
		
		fprintf(tipgraph, "%f\n", cvGetReal1D(src, i));
	}
    //printf("%d\t",tipcount);
	fclose(tipgraph);
	//fclose(freq);
		
}
