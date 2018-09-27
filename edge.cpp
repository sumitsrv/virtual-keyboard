#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv/cv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

using namespace std;
using namespace cv;

class Edge{
    void drawEdge(IplImage* img)
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

        resize = cvCreateImage(cvSize(width, height), img->depth, img->nChannels);

        grey  = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
        grey_eqz = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
        edge  = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);

        cvCvtColor(img, grey, CV_BGR2GRAY);
        cvCanny(grey, edge, 150, 200, 3);
        cvShowImage("Canny", edge);
        IplImage* color_dst = cvCreateImage( cvGetSize(grey), 8, 3 );
        color_dst = getAndMarkLines(edge, edge, 255);

        cvShowImage("Image",img);
        cvShowImage("GRAY",grey);

        cvShowImage("Lines", color_dst);

        cvWaitKey(0);
        cvDestroyWindow("Canny");
        cvDestroyWindow("Image");
        cvDestroyWindow("GRAY");
        cvReleaseImage(&edge);
        cvReleaseImage(&grey);
    }


    IplImage* getAndMarkLines(IplImage* srcimg, IplImage *dstimg, int linecolor)
    {
        Mat src(srcimg);
        Mat dst(dstimg);

        IplImage* color_dst_img = cvCreateImage( cvGetSize(srcimg), 8, 3 );

        Mat color_dst(color_dst_img);

        cvtColor( dst, color_dst, CV_GRAY2BGR );

        vector<Vec4i> lines;
        HoughLinesP( dst, lines, 1, CV_PI/180, 80, 40, 8);
        for( size_t i = 0; i < lines.size(); i++ )
        {
            line( dst, Point(lines[i][0], lines[i][1]),
                    Point(lines[i][2], lines[i][3]), linecolor, 1, 8 );
        }

        IplImage* return_img = cvCreateImage(cvGetSize(srcimg), 8, 1);
        CvMat oldmat = dst;

        return cvGetImage(&oldmat, return_img);
    }

};