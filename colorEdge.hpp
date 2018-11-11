#ifndef COLOREDGE_HPP
#define COLOREDGE_HPP

#include<opencv2/highgui.hpp>
#include<opencv/cv.hpp>
#include<iostream>
#include<stdlib.h>
#include<math.h>

#define PI 3.14159265

using namespace std;
using namespace cv;

class ColorEdge {
public:
    void colorEdge(Mat img, Mat copy);
};
#endif
