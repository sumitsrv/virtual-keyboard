#ifndef WEBCAM_HPP
#define WEBCAM_HPP

#include <boost/bind.hpp>
#include <boost/signals.hpp>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/highgui.hpp>

class WebCam {
 public:
  typedef boost::signal<void(cv::Mat frame)> FrameGrabEvent;

  int capture(cv::VideoCapture, char *);
  static void mouseHandler(int event, int x, int y, int flags, void *img);
  void stream(cv::VideoCapture);
  int end;

  void unsubscribeToCamStream(const boost::signals::connection &connection);
  boost::signals::connection subscribeToCamStream(
      const FrameGrabEvent::slot_type &slot);

 private:
  boost::signals::connection frameGrabConnection;
  FrameGrabEvent frameGrabEvent;
};

#endif
