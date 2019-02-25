#include "keys.hpp"

using namespace std;
using namespace cv;

Keys::Keys() { ocr = OCR(); }

double Keys::angle(Point pt1, Point pt2, Point pt0) {
  double dx1 = pt1.x - pt0.x;
  double dy1 = pt1.y - pt0.y;
  double dx2 = pt2.x - pt0.x;
  double dy2 = pt2.y - pt0.y;
  return (dx1 * dx2 + dy1 * dy2) /
         sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

string Keys::getKey(Point touchPoint) {
  cout << "Getting key for touchpoint: " << touchPoint << endl;

  return "touchPoint.x";
}

/**
 * Square contours should have 4 vertices after approximation,
 * relatively large area (to filter out noisy contours) and be convex. It
 * shouldn't be very large either.
 *
 * Note: Absolute value of an area is used because area may be positive or
 * negative - in accordance with the contour orientation.
 *
 * @brief Keys::isValidContour
 * @param approxContour
 * @return
 */
bool Keys::isValidContour(vector<Point> approxContour) {
  if (approxContour.size() != 4) {
    return false;
  }

  if (fabs(contourArea(approxContour)) < 1000 ||
      fabs(contourArea(approxContour)) >= 500000) {
    return false;
  }
  if (!isContourConvex(approxContour)) {
    return false;
  }
  double maxCosine = 0;

  for (int j = 2; j < 5; j++) {
    // find the maximum cosine of the angle between joint edges
    double cosine = fabs(angle(approxContour[j % 4], approxContour[j - 2],
                               approxContour[j - 1]));
    maxCosine = MAX(maxCosine, cosine);
  }

  // if cosines of all angles are small
  // (all angles are ~90 degree) then write quandrange
  // vertices to resultant sequence
  if (maxCosine >= 0.3) {
    return false;
  }

  return true;
}

/**
 * Returns sequence of squares detected on the image.
 *
 * @brief Keys::evaluateContourFitnessAndStore
 * @param squares
 * @param contours
 */
void Keys::evaluateContourFitnessAndStore(vector<vector<Point> > &squares,
                                          vector<vector<Point> > &contours) {
  vector<Point> approxContour;

  for (size_t i = 0; i < contours.size(); i++) {
    // approximate contour with accuracy proportional
    // to the contour perimeter
    approxPolyDP(contours[i], approxContour,
                 arcLength(contours[i], true) * 0.02, true);

    if (isValidContour(approxContour)) {
      squares.push_back(approxContour);
    }
  }
}

void Keys::findSquares(const Mat &image, vector<vector<Point> > &squares) {
  squares.clear();

  Mat pyr, timg, gray0(image.size(), CV_8U), gray;

  // down-scale and upscale the image to filter out the noise
  pyrDown(image, pyr, Size(image.cols / 2, image.rows / 2));
  pyrUp(pyr, timg, image.size());
  vector<vector<Point> > contours;

  // find squares in every color plane of the image
  for (int c = 0; c < image.channels(); c++) {
    int ch[] = {c, 0};
    mixChannels(&timg, 1, &gray0, 1, ch, 1);

    // try several threshold levels
    for (int l = 0; l < N; l++) {
      // hack: use Canny instead of zero threshold level.
      // Canny helps to catch squares with gradient shading
      if (l == 0) {
        // apply Canny. Take the upper threshold from slider
        // and set the lower to 0 (which forces edges merging)
        Canny(gray0, gray, 0, thresh, 5);
        // dilate canny output to remove potential
        // holes between edge segments
        dilate(gray, gray, Mat(), Point(-1, -1));
      } else {
        // apply threshold if l!=0:
        //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
        gray = gray0 >= (l + 1) * 255 / N;
      }

      // find contours and store them all as a list
      findContours(gray, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

      // Test each contour
      evaluateContourFitnessAndStore(squares, contours);
    }
  }
}

// the function draws all the squares in the image
void Keys::drawSquares(Mat &image, const vector<vector<Point> > &squares) {
  for (size_t i = 0; i < squares.size(); i++) {
    double area = contourArea(squares[i]);
    if (area > MAX_AREA) {
      continue;
    }
    const Point *p = &squares[i][0];
    int n = (int)squares[i].size();
    polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);
  }
}

// the function draws all the squares in the image
Mat Keys::getDilationStructuringElement() {
  int max_iters = 1;
  int open_close_pos = 0;

  int n = open_close_pos - max_iters;
  int an = n > 0 ? n : -n;

  return getStructuringElement(MORPH_RECT, Size(an * 2 + 1, an * 2 + 1),
                               Point(an, an));
}

void Keys::saveImage(string location, int fileName, Mat image) {
  stringstream fileNameSrc;
  fileNameSrc << location << fileName << ".png";
  imwrite(fileNameSrc.str(), image);
}

void Keys::getKeyTextFromSquares(const Mat &image,
                                 const vector<vector<Point> > &squares) {
  Mat element = getDilationStructuringElement();

  for (size_t i = 0; i < squares.size(); i++) {
    const Point *p = &squares[i][0];
    if (p[2].x <= p[0].x || p[2].y <= p[0].y) {
      continue;
    }
    Rect ro(p[0].x, p[0].y, p[2].x - p[0].x, p[2].y - p[0].y);
    if (contourArea(squares[i]) > MAX_AREA) {
      return;
    }

    Mat src = image(ro);
    Mat grayImage;
    Mat thresholdedImage;

    cvtColor(src, grayImage, CV_BGR2GRAY);

    //    saveImage("./ocr_temp_data/key-src", (int)i, grayImage);

    threshold(grayImage, thresholdedImage, 245, 255, THRESH_BINARY_INV);

    // adaptiveThreshold(grayImage, thresholdedImage, 255,
    // ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 0);

    Mat dilatedImage;

    dilate(thresholdedImage, dilatedImage, element);
    string keyText = ocr.readText(dilatedImage);

    this->keyMap[p->x] = keyText;

    //    saveImage("./ocr_temp_data/key", (int)i, dilatedImage);
  }
}

int Keys::locate(Mat source) {
  vector<vector<Point> > squares;

  findSquares(source, squares);
  getKeyTextFromSquares(source, squares);
  drawSquares(source, squares);

  return 0;
}

void Keys::remove_loose_ends(Mat canny) {
  Mat clone = canny.clone();

  uchar *data = (uchar *)canny.data;
  uchar *cldata = (uchar *)clone.data;

  int height = canny.rows;
  int width = canny.cols;
  int step = canny.step;

  int i, j, k, count = 0;

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      count = 0;
      if (data[i * step + j] == BG) {
        for (k = -1; k < 2; k++) {
          if (cldata[(i + k) * step + (j + k)] == BG) {
            count++;
          }
          if (cldata[i * step + j + k] == BG) {
            count++;
          }
          if (cldata[(i + k) * step + j] == BG) {
            count++;
          }
        }
        if (count < 4) data[i * step + j] = FG;
      }
    }
  }
}
