#include "webcam.hpp"

using namespace std;
using namespace cv;
using namespace boost;

signals::connection WebCam::subscribeToCamStream(
    const FrameGrabEvent::slot_type &slot) {
  return this->frameGrabEvent.connect(slot);
}

void WebCam::unsubscribeToCamStream(const signals::connection &connection) {
  connection.disconnect();
}

int WebCam::capture(VideoCapture capture, char *img_name) {
  Mat frame;
  int key = 0;

  /* create a window for the video */
  namedWindow("result", CV_WINDOW_AUTOSIZE);

  Mat img;
  capture >> frame;

  if (frame.empty()) {
    printf("Could not grab a frame\n\7");
    exit(0);
  }
  while (key != 27) {
    /* get a frame */
    capture >> frame;

    /* always check */
    if (frame.empty()) break;

    /* display current frame */
    imshow("result", frame);
    //        setMouseCallback( "result", mouseHandler, frame.data );
    /* exit if user press 'q' */
    key = cvWaitKey(5);
    // printf("%d  %c\t",key, key);
  }

  vector<int> compression_params;
  compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
  compression_params.push_back(100);

  if (!imwrite(img_name, frame, compression_params)) {
    printf("Could not save: %s\n", img_name);
  }

  cvDestroyWindow("result");
  return 0;
}

void WebCam::mouseHandler(int event, int x, int y, int, void *img) {
  switch (event) {
    /* left button down */
    case CV_EVENT_LBUTTONDOWN:
      cout << "Left" << endl;
      break;

      /* right button down */
    case CV_EVENT_RBUTTONDOWN:
      cout << "Right" << endl;
      break;

      /* mouse move */
    case CV_EVENT_MOUSEMOVE:
      break;
  }
}

void WebCam::stream(VideoCapture capture) {
  Mat frame;
  int key = 0;

  capture >> frame;

  if (frame.empty()) {
    printf("Could not grab a frame!\n");
    exit(0);
  }
  while (key != 27) {
    /* get a frame */
    capture >> frame;

    /* always check */
    if (frame.empty()) {
      printf("Error grabbing frame, exiting!\n");
      break;
    }
    imshow("handdetect", frame);
    this->frameGrabEvent(frame);
    /* exit if user press 'q' */
    key = cvWaitKey(5);
  }

  this->frameGrabConnection.disconnect();
  destroyWindow("handdetect");
  capture.release();
}
