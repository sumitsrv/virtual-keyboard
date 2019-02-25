#ifndef EDGE_HPP
#define EDGE_HPP

#include <math.h>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

class Edge {
 public:
  void drawEdge(cv::Mat img);
  cv::Mat getAndMarkLines(cv::Mat srcimg, cv::Mat dstimg, int linecolor);
};

#endif
