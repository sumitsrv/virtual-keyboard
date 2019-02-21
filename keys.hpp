#ifndef KEYS_HPP
#define KEYS_HPP

#include "ocr.hpp"

#define CV_NO_BACKWARD_COMPATIBILITY

#define FG 255
#define BG 0
#define CREATE_DATA_SET 0
#define ON_WORK 1
#define THRESH_1 150
#define THRESH_2 200

class Keys {
private:
    OCR ocr;

public:
  int count1, count2, count3;
  CvMemStorage *storage;

  const int thresh = 50, N = 11;
  const char *wndname = "Square Detection Demo";

  double angle(cv::Point, cv::Point, cv::Point);
  void remove_loose_ends(cv::Mat);
  void drawSquares(cv::Mat &, const std::vector<std::vector<cv::Point> > &);
  void findSquares(const cv::Mat &, std::vector<std::vector<cv::Point> > &);
  void saveSquares(const cv::Mat &, const std::vector<std::vector<cv::Point> > &);
  int locate(cv::Mat, cv::Mat);
};

#endif
