#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

using namespace cv;

IplImage* markLines(IplImage*, IplImage*, int linecolor);

void edge(IplImage* img)
{
	IplImage  *edge=0, *grey=0, *resize=0, *grey_eqz=0;
	int size = 500, height, width, scale;
    
    height = img->height;
    width = img->width;
    scale = (height > width)? height: width;
    scale = scale/size;
    
    scale  = (scale < 1)? 1 : scale;
    height = height/scale;
    width = width/scale;
    //printf("%d %d %d\n",width, height, scale);
    
    resize = cvCreateImage(cvSize(width, height), img->depth, img->nChannels);
	//cvResize(img, resize);
	
	grey  = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	grey_eqz = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	edge  = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	
	cvCvtColor(img, grey, CV_BGR2GRAY);
	//cvEqualizeHist(grey, grey_eqz);
	//cvThreshold(grey_eqz,grey_eqz,0,255,CV_THRESH_BINARY);
	cvCanny(grey, edge, 150, 200, 3);
	//cvDilate(edge, edge, 0, 1);
	//cvErode(edge, edge, 0, 1);
	cvShowImage("Canny", edge);
	IplImage* color_dst = cvCreateImage( cvGetSize(grey), 8, 3 );
	color_dst = markLines(edge, edge, 255);
		
	cvShowImage("Image",img);
	cvShowImage("GRAY",grey);
	
	cvShowImage("Lines", color_dst);
	
	cvWaitKey(0);
	cvDestroyWindow("Canny");
	cvDestroyWindow("Image");
	cvDestroyWindow("GRAY");
	//cvReleaseImage(&img);
	cvReleaseImage(&edge);
	cvReleaseImage(&grey);
}


IplImage* markLines(IplImage* srcimg, IplImage *dstimg, int linecolor)
{
	Mat src(srcimg);
	Mat dst(dstimg);
	
	IplImage* color_dst_img = cvCreateImage( cvGetSize(srcimg), 8, 3 );
      
    Mat color_dst(color_dst_img);
      
	//Canny( src, dst, 50, 100, 3);
	//dilate( dst, dst, 0, Point(-1,-1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
	cvtColor( dst, color_dst, CV_GRAY2BGR );

#if 0
    vector<Vec2f> lines;
    HoughLines( dst, lines, 1, CV_PI/180, 100 );

    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0];
        float theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        Point pt1(cvRound(x0 + 1000*(-b)),
                  cvRound(y0 + 1000*(a)));
        Point pt2(cvRound(x0 - 1000*(-b)),
                  cvRound(y0 - 1000*(a)));
        line( dst, pt1, pt2, linecolor, 1, 8 );
    }
#else
    vector<Vec4i> lines;
    HoughLinesP( dst, lines, 1, CV_PI/180, 80, 40, 8);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( dst, Point(lines[i][0], lines[i][1]),
            Point(lines[i][2], lines[i][3]), linecolor, 1, 8 );
    }
#endif
   
    IplImage* return_img = cvCreateImage(cvGetSize(srcimg), 8, 1);
    CvMat oldmat = dst;
    
    return cvGetImage(&oldmat, return_img);     
}

