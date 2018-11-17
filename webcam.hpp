#ifndef WEBCAM_HPP
#define WEBCAM_HPP

#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

class WebCam {
public:
  int capture(VideoCapture, char *);
  static void mouseHandler(int event, int x, int y, int flags, void *img);
  int end;
};
#endif
