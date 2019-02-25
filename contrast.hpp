#ifndef CONTRAST_HPP
#define CONTRAST_HPP

#include <math.h>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>

class Contrast {
 public:
  const double alpha = 0.8;
  void glg(cv::Mat img);
  void mergeLevels(cv::Mat, double *, int *, float[], float[]);
  void lookUpTable(float *, float[], float[], int *, float[]);
};
#endif
