#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>
#include <stdlib.h>

#include "colorEdge.hpp"
#include "contrast.hpp"
#include "edge.hpp"
#include "haar.hpp"
#include "keys.hpp"
#include "ocr.hpp"
#include "webcam.hpp"
#include "handdetect.hpp"

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
  WebCam webcam;
  HandDetect *handDetect = new HandDetect(webcam);
  VideoCapture capture;
  Mat img;

  if (argc < 3) {
    fprintf(stderr, "Usage: loadimg <filename> <cam(1)/image(2)>\n");
    return 1;
  } else if (argv[2][0] == '1') {
    /* initialize camera */
    capture = VideoCapture(0);

    capture.set(CV_CAP_PROP_FRAME_WIDTH, 10000);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 10000);

    /* always check */
    if (!capture.isOpened()) {
      fprintf(stderr, "Cannot open initialize webcam!\n");
      fprintf(stderr, "Usage: loadimg <filename> <cam(1)/image(2)>\n");
      return 1;
    }

    webcam.capture(capture, argv[1]);
    img = imread(argv[1], -1);
  } else if (argv[2][0] == '2') {
    img = imread(argv[1], -1);
  } else {
    fprintf(stderr, "Invalid argument 2\n");
    return 1;
  }

  if (img.empty()) {
    fprintf(stderr, "Cannot load file %s!\n", argv[1]);
    return 1;
  }

  int depth = img.depth();

  Size sz = Size(img.cols & -2, img.rows & -2);

  Mat gray = Mat(cvSize(sz.width, sz.height), depth, 1);
  Mat copy = Mat(cvSize(sz.width, sz.height), depth, 3);

  webcam.stream(capture);
//  cvtColor(img, gray, CV_BGR2GRAY);
//  Contrast contrast = Contrast();
//  contrast.glg(img);

//  Keys keys = Keys();
//  keys.locate(img, gray);

//  imshow("Keys", img);
  waitKey(10000);
  return 0;
}
