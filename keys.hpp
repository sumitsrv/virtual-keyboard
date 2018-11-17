#ifndef KEYS_HPP
#define KEYS_HPP

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#define CV_NO_BACKWARD_COMPATIBILITY

#define FG 255
#define BG 0
#define CREATE_DATA_SET 0
#define ON_WORK 1
#define THRESH_1 150
#define THRESH_2 200

using namespace std;
using namespace cv;

class Keys {
public:
  int count1, count2, count3;
  CvMemStorage *storage;

  const int thresh = 50, N = 11;
  const char *wndname = "Square Detection Demo";

  double angle(Point, Point, Point);
  void remove_loose_ends(Mat);
  void drawSquares(Mat &, const vector<vector<Point> > &);
  void findSquares(const Mat &, vector<vector<Point> > &);
  int locate(Mat, Mat);
};

#endif
