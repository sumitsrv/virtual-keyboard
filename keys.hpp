#ifndef KEYS_HPP
#define KEYS_HPP

#include <iostream>
#include <map>
#include "ocr.hpp"

#define CV_NO_BACKWARD_COMPATIBILITY

#define FG 255
#define BG 0
#define CREATE_DATA_SET 0
#define ON_WORK 1
#define THRESH_1 150
#define THRESH_2 200
#define MAX_AREA 500000

class Keys {
 private:
  OCR ocr;
  void evaluateContourFitnessAndStore(
      std::vector<std::vector<cv::Point> > &squares,
      std::vector<std::vector<cv::Point> > &contours);
  bool isValidContour(std::vector<cv::Point> approxContour);
  std::map<int, std::string> keyMap;

 public:
  Keys();
  int count1, count2, count3;
  CvMemStorage *storage;

  const int thresh = 50, N = 11;
  const char *wndname = "Square Detection Demo";

  double angle(cv::Point, cv::Point, cv::Point);
  void remove_loose_ends(cv::Mat);
  void drawSquares(cv::Mat &, const std::vector<std::vector<cv::Point> > &);
  void findSquares(const cv::Mat &, std::vector<std::vector<cv::Point> > &);
  void getKeyTextFromSquares(const cv::Mat &,
                   const std::vector<std::vector<cv::Point> > &);
  int locate(cv::Mat);
  cv::Mat getDilationStructuringElement();
  void saveImage(std::string location, int fileName, cv::Mat image);
  std::string getKey(cv::Point);
};

#endif
