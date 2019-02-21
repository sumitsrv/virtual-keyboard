#ifndef EDGE_HPP
#define EDGE_HPP

#include <iostream>
#include <math.h>
#include <opencv/cv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>
#include <stdlib.h>

class Edge {
public:
  void drawEdge(cv::Mat img);
  cv::Mat getAndMarkLines(cv::Mat srcimg, cv::Mat dstimg, int linecolor);
};

#endif
