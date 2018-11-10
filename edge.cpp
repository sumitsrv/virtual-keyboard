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
    void drawEdge(Mat img)
    {
        Mat edge;
        Mat grey;
        Mat resize;
        Mat grey_eqz;

        int size = 500, height, width, scale;

        height = img.rows;
        width = img.cols;
        scale = (height > width)? height: width;
        scale = scale/size;

        scale  = (scale < 1)? 1 : scale;
        height = height/scale;
        width = width/scale;

        resize = Mat(Size(width, height), img.depth());
//        resize = cvCreateImage(cvSize(width, height), img->depth, img->nChannels);

        grey  = Mat(img.rows, img.cols, IPL_DEPTH_8U, 1);
        grey_eqz = Mat(img.rows, img.cols, IPL_DEPTH_8U, 1);
        edge  = Mat(img.rows, img.cols, IPL_DEPTH_8U, 1);

//        grey  = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
//        grey_eqz = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
//        edge  = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);

        cvtColor(img, grey, CV_BGR2GRAY);
        Canny(grey, edge, 150, 200, 3);
        imshow("Canny", edge);
        Mat color_dst = Mat( grey.rows, grey.cols, 8, 3 );
        color_dst = getAndMarkLines(edge, edge, 255);

        imshow("Image",img);
        imshow("GRAY",grey);

        imshow("Lines", color_dst);

        cvWaitKey(0);
//        cvDestroyWindow("Canny");
//        cvDestroyWindow("Image");
//        cvDestroyWindow("GRAY");
//        edge = 0;
//        grey = 0;
    }


    Mat getAndMarkLines(Mat srcimg, Mat dstimg, int linecolor)
    {
        Mat src(srcimg);
        Mat dst(dstimg);

        Mat color_dst_img = Mat( srcimg.rows, srcimg.cols, 8, 3 );

        Mat color_dst(color_dst_img);

        cvtColor( dst, color_dst, CV_GRAY2BGR );

        vector<Vec4i> lines;
        HoughLinesP( dst, lines, 1, CV_PI/180, 80, 40, 8);
        for( size_t i = 0; i < lines.size(); i++ )
        {
            line( dst, Point(lines[i][0], lines[i][1]),
                    Point(lines[i][2], lines[i][3]), linecolor, 1, 8 );
        }

        return dst;
//        Mat return_img = Mat(srcimg.rows, srcimg.cols, 8, 1);
//        CvMat oldmat = dst;

//        return cvGetImage(&oldmat, return_img);
    }

};
