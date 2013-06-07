#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

using namespace cv;

IplImage* markLines(IplImage*, IplImage*);
IplImage* enhance_contrast_fuzzy(IplImage*);

int main(int argc, char* argv[])
{
	IplImage *img=0, *edge=0, *grey=0, *resize=0, *grey_eqz=0;
	int size = 500, height, width, scale;
	
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
    
    height = img->height;
    width = img->width;
    scale = (height > width)? height: width;
    scale = scale/size;
    
    scale  = (scale < 1)? 1 : scale;
    height = height/scale;
    width = width/scale;
    //printf("%d %d %d\n",width, height, scale);
    
    resize = cvCreateImage(cvSize(width, height), img->depth, img->nChannels);
	cvResize(img, resize);
	
	grey  = cvCreateImage(cvGetSize(resize), IPL_DEPTH_8U, 1);
	grey_eqz = cvCreateImage(cvGetSize(resize), IPL_DEPTH_8U, 1);
	edge  = cvCreateImage(cvGetSize(resize), IPL_DEPTH_8U, 1);
	
	cvCvtColor(resize, grey, CV_BGR2GRAY);
	cvEqualizeHist(grey, grey_eqz);
	cvThreshold(grey_eqz,grey_eqz,50,255,CV_THRESH_BINARY);
	cvCanny(grey_eqz, edge, 100, 200, 3);
	
	IplImage* color_dst = cvCreateImage( cvGetSize(grey), 8, 3 );
	color_dst = markLines(grey_eqz, edge);
		
	cvShowImage("Image",resize);
	cvShowImage("GRAY",grey_eqz);
	cvShowImage("Canny", edge);
	cvShowImage("Lines", color_dst);
	
	cvWaitKey(0);
	cvDestroyWindow("Canny");
	cvDestroyWindow("Image");
	cvDestroyWindow("GRAY");
	cvReleaseImage(&img);
	cvReleaseImage(&edge);
	cvReleaseImage(&grey);
	return 0;
}


IplImage* markLines(IplImage* srcimg, IplImage *dstimg)
{
	Mat src(srcimg);
	Mat dst(dstimg);
	
	IplImage* color_dst_img = cvCreateImage( cvGetSize(srcimg), 8, 3 );
      
    Mat color_dst(color_dst_img);
      
	Canny( src, dst, 100, 200, 3 );
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
        line( color_dst, pt1, pt2, Scalar(0,0,255), 3, 8 );
    }
#else
    vector<Vec4i> lines;
    HoughLinesP( dst, lines, 1, CV_PI/180, 80, 30, 10 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( color_dst, Point(lines[i][0], lines[i][1]),
            Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
    }
#endif
   
    IplImage* return_img = cvCreateImage(cvGetSize(srcimg), 8, 1);
    CvMat oldmat = color_dst;
    
    return cvGetImage(&oldmat, return_img);     
}

IplImage* enhance_contrast_fuzzy(IplImage *src)
{
	return src;
}
