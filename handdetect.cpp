#include "handdetect.hpp"

using namespace cv;
using namespace std;

HandDetect::HandDetect(WebCam &webcam, Keys &keys) {
  this->frameGrabEventConnection = webcam.subscribeToCamStream(
      boost::bind(&HandDetect::getKeyPress, this, _1));
  this->keys = &keys;
}

void HandDetect::getKeyPress(Mat frame) {
  printf("Frame received!\n");

  Size sz = Size(frame.cols, frame.rows);

  Mat hsv_image = Mat(sz, 8, 3);
  Mat hsv_mask = Mat(sz, 8, 1);
  Mat handview = Mat(sz, 8, 1);
  Scalar hsv_min = Scalar(5, 70, 0, 0);
  Scalar hsv_max = Scalar(20, 150, 255, 0);  // H-> 0-20

  cvtColor(frame, hsv_image, CV_BGR2HSV);
  inRange(hsv_image, hsv_min, hsv_max, hsv_mask);
  medianBlur(hsv_mask, handview, 5);
  dilate(handview, handview, noArray(), Point(-1, -1), 3);
  vector<vector<Point> > contours;

  findContours(handview, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

  for (size_t i = 0; i < contours.size(); i++) {
    {
      // replace CV_FILLED with 1 to see the outlines
      double area = contourArea(contours[i], 0);
      if (area > 500) {
        drawContours(handview, contours, i, Scalar(255), 1, 8, noArray(), 0);
      }
    }
    int tipCount = getTipsCount(handview);

    for (int i = 0; i < tipCount; i++) {
      if (posmax == i)
        circle(frame, Point(tips_position[posmax][1], tips_position[posmax][0]),
               3, Scalar(0, 255, 0), 2, 8, 0);
      else
        circle(frame, Point(tips_position[i][1], tips_position[i][0]), 3,
               Scalar(255, 0, 0), 2, 8, 0);

      if (speed[i][1] > 1 && speed[i][2] > 1 && (int)speed[i][5] == 1 &&
          oldtips[(oldtipflag + 1) % FRAMERUN][i][2] == 1) {
        circle(frame, Point(speed[i][4], speed[i][3]), 5, Scalar(0, 0, 255), 3,
               8, 0);
        speed[i][1] = 0;
        speed[i][2] = 0;
        speed[i][5] = 0;
      } else if (speed[i][1] > 1 && speed[i][2] > 1 &&
                 oldtips[(oldtipflag + 1) % FRAMERUN][i][2] == -1) {
        speed[i][1] = speed[i][2];
        speed[i][2] = 0;
        speed[i][5] = 0;
      }
    }
    oldtipflag = (oldtipflag + 1) % FRAMERUN;
  }
}

void HandDetect::preserveAndGetHandOutline(Mat src) {
  size_t curr_pixel, step, x[src.cols];
  uchar *data = src.data;
  step = src.step;

  int width = src.cols;
  int height = src.rows;
  for (int i = 0; i < width; i++) {
    x[i] = 0;
    for (int j = 0; j < height; j++) {
      curr_pixel = data[j * step + i];
      if ((curr_pixel == 255)) {
        x[i] = j;
      }
    }

    src.at<uchar>(0, i) = (uchar)x[i];
  }
}

int HandDetect::getTipsCount(Mat img) {
  //  FILE *tipgraph = fopen("tipgraph.dat", "w");
  int width = img.cols;
  int height = img.rows;
  int tipcount = 0;
  Mat dst = Mat(1, img.cols, CV_32FC1);
  Mat src = Mat(1, img.cols, CV_32FC1);

  preserveAndGetHandOutline(src);

  // Discrete Fourier Transform on the source hand outline image.
  dft(src, dst, CV_DXT_SCALE, 0);

  // Applying filter to the transformed image source.
  for (int i = 0; i < width; i++) {
    if (i > 20) {
      dst.at<uchar>(0, i, 0) = 0;
      //      cvSetReal1D(dst, i, 0);
    }
  }

  // Applying inverse DFT to get a smooth curve highlighting only local maximas
  dft(dst, src, CV_DXT_INVERSE, 0);

  // Get tips position
  for (int i = 1; i < width - 1; i++) {
    /*if ((cvGetReal1D(src, i + 1) - cvGetReal1D(src, i) <= 0 &&
         cvGetReal1D(src, i) - cvGetReal1D(src, i - 1) >= 0))*/
    if (-src.at<uchar>(0, i) <= 0 &&
        src.at<uchar>(0, i) - src.at<uchar>(0, i - 1) >= 0) {
      tips_position[tipcount][0] = 0;

      // Accumulate tip position over 7 adjacent pixels, eventually average them
      // out in the next step.
      for (int j = -3; j <= 3; j++) {
        if ((i + j) < width && (i + j) > 0) {
          tips_position[tipcount][0] += src.at<uchar>(0, i + j);
        }
      }

      tips_position[tipcount][0] = (tips_position[tipcount][0]) / 7;

      if (tips_position[tipcount][0] > 40) {
        if ((tipcount > 1) && (i - tips_position[tipcount - 1][1]) < 55) {
          tips_position[tipcount - 1][0] =
              (tips_position[tipcount - 1][0] + tips_position[tipcount][0]) / 2;
          tips_position[tipcount - 1][1] =
              (tips_position[tipcount - 1][1] + i) / 2;
        } else {
          tips_position[tipcount++][1] = i;
        }
      }
    }

    //    fprintf(tipgraph, "%f\n", cvGetReal1D(src, i));
  }

  //  fclose(tipgraph);

  correlatedTips(tipcount);

  return tipcount;
}

/**
 * @brief HandDetect::correlatedTips
 * This method resolves corresponsing tips between current and previous frame.
 *
 * @param tipCount
 */
int HandDetect::getMaxSpeedFingerIndex(int tipCount) {
  int max = 0;
  float speedscale = 1;
  int posmax = -1;

  for (int i = 0; i < tipCount; i++) {
    speedscale = (((oldtipflag + 1) % FRAMERUN -
                   oldtips[(oldtipflag + 1) % FRAMERUN][i][3] + FRAMERUN) %
                  FRAMERUN) +
                 1;
    speed[i][0] =
        abs(pow((oldtips[oldtips[(oldtipflag + 1) % FRAMERUN][i][3]][i][0] -
                 tips_position[i][0]),
                2)) /
        speedscale;
    if (speed[i][0] > max && speed[i][0] > 30 &&
        oldtips[(oldtipflag + 1) % FRAMERUN][i][2] == -1) {
      max = speed[i][0];
      posmax = i;
    }
  }
  return posmax;
}

void HandDetect::correlatedTips(int tipCount) {
  int i = 0, j = 0, distx = 0, disty = 0, notfound = 0;

  for (j = 0; j < tipCount; j++) {
    for (i = j; i <= j; i++) {
      int ox = oldtips[oldtipflag][j][0];
      int tx = tips_position[i][0];
      distx = abs(ox - tx);
      disty = abs(oldtips[oldtipflag][j][1] - tips_position[i][1]);

      if (distx <= 300 && disty <= 300) {
        int posDiff = oldtips[oldtipflag][j][0] - tips_position[i][0];

        // Get direction of movement of the tip
        if (posDiff != 0) {
          oldtips[(oldtipflag + 1) % FRAMERUN][j][2] = (posDiff) / abs(posDiff);
        } else {
          oldtips[(oldtipflag + 1) % FRAMERUN][j][2] =
              oldtips[oldtipflag][j][2];
        }

        // Check if direction of the tip changes, if yes change the base frame.
        // Base frame is the one which is used to calculate speed difference
        // when direction changes
        if (oldtips[(oldtipflag + 1) % FRAMERUN][j][2] -
                oldtips[oldtipflag][j][2] ==
            0) {
          oldtips[(oldtipflag + 1) % FRAMERUN][j][3] =
              oldtips[oldtipflag][j][3];
        } else {
          oldtips[(oldtipflag + 1) % FRAMERUN][j][3] = oldtipflag;
        }
        break;
      } else {
        notfound++;
      }
    }
  }

  int posmax = getMaxSpeedFingerIndex(tipCount);

  float speedscale = 1;

  for (i = 0; i < tipCount; i++) {
    speedscale = (((oldtipflag + 1) % FRAMERUN -
                   oldtips[(oldtipflag + 1) % FRAMERUN][i][3] + FRAMERUN) %
                  FRAMERUN) +
                 1;

    if (speedscale == 2) {
      if (speed[i][1] == 0 || speed[i][1] == 1) {
        speed[i][1] = 1;
        if (posmax == i) speed[i][5] = 1;
      } else if (speed[i][1] != 0 && speed[i][2] == 0) {
        speed[i][2]++;
        speed[i][3] = oldtips[oldtipflag][i][0];
        speed[i][4] = oldtips[oldtipflag][i][1];
        speed[i][6] = oldtips[(oldtipflag + 1) % FRAMERUN][i][2];
      } else if (speed[i][1] != 0 && speed[i][2] != 0) {
        speed[i][1] = 1;
        speed[i][2] = 0;
        if (posmax == i) {
          speed[i][5] = 1;
        } else
          speed[i][5] = 0;
      }
    } else {
      if (speed[i][1] != 0 && speed[i][2] == 0) {
        speed[i][1]++;
        if (posmax == i) speed[i][5] = 1;
      } else if (speed[i][1] != 0 && speed[i][2] != 0) {
        speed[i][2]++;
      }
    }

    oldtips[(oldtipflag + 1) % FRAMERUN][i][0] = tips_position[i][0];
    oldtips[(oldtipflag + 1) % FRAMERUN][i][1] = tips_position[i][1];
    oldtips[(oldtipflag + 1) % FRAMERUN][10][0] = tipCount;
  }
}
