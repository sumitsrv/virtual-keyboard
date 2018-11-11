#ifndef CONTRAST_HPP
#define CONTRAST_HPP

#include<opencv2/highgui.hpp>
#include<opencv/cv.hpp>
#include<iostream>
#include<stdlib.h>
#include<math.h>

using namespace std;
using namespace cv;

#define alpha 0.8

class Contrast {
public:
    void glg(Mat img);
    void mergeLevels(Mat, float *, int*, float[], float[]);
    void lookUpTable(float *, float [], float [], int*, float []);

};
#endif
