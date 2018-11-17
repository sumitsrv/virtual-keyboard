#ifndef CONTRAST_HPP
#define CONTRAST_HPP

#include <iostream>
#include <math.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>
#include <stdlib.h>

using namespace std;
using namespace cv;

#define alpha 0.8

class Contrast {
public:
  void glg(Mat img);
  void mergeLevels(Mat, double *, int *, float[], float[]);
  void lookUpTable(float *, float[], float[], int *, float[]);
};
#endif
