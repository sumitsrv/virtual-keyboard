#ifndef OCR_HPP
#define OCR_HPP

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>

class OCR {
 private:
  tesseract::TessBaseAPI *ocr;

 public:
  OCR();
  void findX(cv::Mat, int *, int *);
  void findY(cv::Mat, int *, int *);
  CvRect findBB(cv::Mat);
  cv::Mat preprocessing(cv::Mat, int, int);
  cv::String readText(cv::Mat src);
};

#endif
