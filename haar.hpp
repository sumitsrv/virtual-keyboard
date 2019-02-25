#ifndef HAAR_HPP
#define HAAR_HPP

#include <iostream>
#include <opencv/cv.hpp>

class Haar {
 public:
  IplImage haar(IplImage *img, IplImage *gray);
  void haar(IplImage *, IplImage *, int);
  void make_icon(IplImage *, IplImage *, int);
  void up_Scale(IplImage *img, IplImage *upscale, int scale);
};

#endif
