#ifndef COLOREDGE_HPP
#define COLOREDGE_HPP

#include <math.h>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>

#define PI 3.14159265

class ColorEdge {
 public:
  void colorEdge(cv::Mat img, cv::Mat copy);
};
#endif
