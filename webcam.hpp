#ifndef WEBCAM_HPP
#define WEBCAM_HPP

#include <boost/bind.hpp>
#include <boost/signals.hpp>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;
using namespace boost;

class WebCam {

public:
  typedef signal<void(Mat frame)> FrameGrabEvent;

  WebCam();
  int capture(VideoCapture, char *);
  static void mouseHandler(int event, int x, int y, int flags, void *img);
  void stream(VideoCapture);
  int end;

  void unsubscribeToCamStream(const signals::connection &connection);
  signals::connection subscribeToCamStream(const FrameGrabEvent::slot_type &slot);

private:
  signals::connection frameGrabConnection;
  FrameGrabEvent frameGrabEvent;
};
#endif
