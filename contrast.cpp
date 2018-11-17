#include "contrast.hpp"

/**
 * @brief Contrast::glg
 * @param img
 *
 * Implementation of paper
 * https://ieeexplore.ieee.org/abstract/document/1658093.
 *
 * Works by creating overall histogram of the image, followed by merging in the
 * lesser impactful ranges into neighbouring buckets unless left over with about
 * 20 buckets. The buckets can be customized as per requirements.
 */
void Contrast::glg(Mat img_bgr) {
  int s, scale = 3, n = 0, i, j;
  Mat hist = Mat(Size(256, 1), CV_8UC1);

  Mat hist_glg = Mat(Size(256, 1), CV_8UC1);;

  Size sz = Size(img_bgr.cols & -2, img_bgr.rows & -2);
  Mat gray = Mat(Size(img_bgr.cols, img_bgr.rows), CV_8UC1);
  Mat img_hsv = Mat(Size(img_bgr.cols, img_bgr.rows), CV_8UC3);

  Mat(img_bgr, Rect(0, 0, sz.width, sz.height));

  /*
   * If source image is three channels, extract the third representing the
   * Variance, else use the source image.
   * */
  if (img_bgr.channels() == 3) {
    cvtColor(img_bgr, img_hsv, CV_BGR2HSV);
    int from_to[] = {2, 0};
    Mat out[] = {gray};
    mixChannels(&img_hsv, 1, out, 1, from_to, 1);
  } else {
    gray = img_bgr.clone();
  }

  int width = gray.cols;
  int height = gray.rows;
  int step = gray.step;

  uchar *data = gray.data;

  const float range[] = {0, 255};
  const float *ranges = {range};
  double max_value = 0, min_value = 0;
  float left[256], right[256], T[256];
  const int channels = 0;
  const int numBins = 256;
  const int hist_size = 256;
  float N;

  //        hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  //        hist_glg = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  //        cvClearHist(hist_glg);

  // IplImage* hist_img = cvCreateImage( cvSize(255*3,255*3), 8, 3 );
  // IplImage* hist_img_glg = cvCreateImage( cvSize(255*3,255*3), 8, 3 );

  /* Creates histogram of intensities in the image (grayscale/variance)*/
  calcHist(&gray, 1, &channels, noArray(), hist, 1, &hist_size, &ranges);

  cout << hist.rows << ":" << hist.cols << ":" << hist.channels() << ":" << hist.depth() << ":" << hist.type() << endl;

//  equalizeHist(gray, gray);
  //        cvCalcHist(&gray, hist, 0, NULL);
  minMaxIdx( hist, &min_value, &max_value, 0, 0 );
  // cvZero( hist_img );

  cout << "Hist calcucated: " << endl;

  int bin_glg;

  for (s = 0; s < hist_size; s++) {
    int bin_val = hist.at<int>(s);
    int intensity = cvRound(bin_val * 255 / max_value);
    // cvRectangle( hist_img, cvPoint( s*scale, 0 ), cvPoint( (s+1)*scale -
    // 1,intensity), CV_RGB(255,255,255), CV_FILLED );

    if (bin_val != 0) {
      bin_glg = hist_glg.at<int>(n);
      //                bin_glg = bin_val;
      // printf("%f\t",bin_val);
      left[n] = s;
      right[n] = s;
      // intensity = cvRound(bin_val*255/max_value);
      // cvRectangle( hist_img_glg, cvPoint( n*scale, 0 ), cvPoint( (n+1)*scale
      // - 1,intensity), CV_RGB(255,255,255), CV_FILLED );
      n++;
    }
  }

  cout << "Hist traversed " << endl;
  // printf("%f %f\n",min_value, max_value);
  // printf("%d\n\n",n);

  // cvShowImage( "H-S Histogram", hist_img );
  for (i = n - 40; i > 0; i--) {
    // cvZero(hist_img_glg);
    //            cvGetMinMaxHistValue( hist_glg, &min_value, &max_value, 0, 0
    //            );
    mergeLevels(hist_glg, &max_value, &n, left, right);

    for (s = 0; s < n; s++) {
      float bin_val = hist_glg.at<int>(s);
      int intensity = cvRound(bin_val * 255 / max_value);
      // printf("%f\t",bin_val);
      // cvRectangle( hist_img_glg, cvPoint( s*scale, 0 ), cvPoint( (s+1)*scale
      // - 1,intensity), CV_RGB(255,255,255), CV_FILLED );
    }
    // cvShowImage( "H-S Histogram2", hist_img_glg );
    // cvWaitKey(0);
  }

  cout << "Hist merged " << endl;

  lookUpTable(&N, left, right, &n, T);
  // cvShowImage( "H-S Histogram2", hist_img_glg );

    cout << "Hist Looked " << endl;
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      data[i * step + j] = T[data[i * step + j]];
    }
  }

  cout << "Hist data " << endl;

  if (img_bgr.channels() == 3) {
//    copy = gray.clone();
    int from_to[] = {0, 2};
    Mat out[] = {img_hsv};
    mixChannels(&gray, 1, out, 1, from_to, 1);
//    cvSetImageCOI(copy, 0);
    cvtColor(img_hsv, img_bgr, CV_HSV2BGR, 0);
  } else {
    img_bgr = gray.clone();
  }
}

void Contrast::mergeLevels(Mat hist_glg, double *max_value, int *hist_size,
                           float left[], float right[]) {
    int n, i = 0, index = 0, index_sec = 0;
  float val_prev = 0, val_next = 0, min_sec = 0;
  double min_value = *max_value;

  for (n = 0; n < *hist_size; n++) {
    float bin_val = hist_glg.at<int>(n);
    if (min_value > (double) bin_val) {
      min_value = (double) bin_val;
      index = n;
    }
  }
  if (index > 0 && index < (*hist_size) - 1) {
    // printf("Hi1\t");
    val_prev = hist_glg.at<int>(index - 1);
    val_next = hist_glg.at<int>(index + 1);
    if (val_prev <= val_next) {
      min_sec = val_prev;
      index_sec = index - 1;
    } else if (val_prev > val_next) {
      min_sec = val_next;
      index_sec = index + 1;
    }
  } else if (index == 0) {
    val_next = hist_glg.at<float>(1);
    val_prev = val_next;
    min_sec = val_next;
    index_sec = 1;
  } else if (index == (*hist_size) - 1) {
    // printf("Hi2\t");
    val_prev = hist_glg.at<float>(index - 1);
    val_next = val_prev;
    min_sec = val_prev;
    index_sec = index - 1;
  }

  if (index_sec < index)
    i = index_sec;
  else
    i = index;

  float bin_glg = hist_glg.at<float>(i);
  float bin_glg_next = hist_glg.at<float>(i + 1);
  bin_glg = bin_glg + bin_glg_next;
  left[i] = left[i];
  right[i] = right[i + 1];
  i++;

  for (; i < (*hist_size) - 1; i++) {
    bin_glg = hist_glg.at<float>(i);
    bin_glg_next = hist_glg.at<float>(i + 1);
    bin_glg = bin_glg_next;
    left[i] = left[i + 1];
    right[i] = right[i + 1];
  }

  bin_glg = hist_glg.at<float>((*hist_size) - 1);
  bin_glg = 0;

  left[*hist_size - 1] = 0;
  right[*hist_size - 1] = 0;

  (*hist_size)--;
  // printf("%f\t%d\t%d\n",min_value, index, *hist_size);
}

void Contrast::lookUpTable(float *N, float left[], float right[],
                           int *hist_size, float T[]) {
  int i, j, curr_bin = 0;

  if (left[0] == right[0]) {
    *N = 255 / (*hist_size - alpha);
  } else {
    *N = 255 / (*hist_size);
  }

  for (i = 0; i < 256; i++) {
    if (i > left[0] && i < right[*hist_size - 1]) {
      for (j = curr_bin; j < *hist_size; j++) {
        if (i >= left[j] && i <= right[j]) {
          curr_bin = j;
          // printf("%d %d -> %f -- %f\t", i, curr_bin, left[curr_bin],
          // right[curr_bin]);
          break;
        } else if (i < left[j + 1] && i > right[j]) {
          break;
        }
      }

      if (left[curr_bin] != right[curr_bin]) {
        if (left[0] == right[0]) {
          T[i] =
              (curr_bin + 1 - alpha -
               ((right[curr_bin] - i) / (right[curr_bin] - left[curr_bin]))) *
                  (*N) +
              1;
        } else {
          T[i] =
              (curr_bin + 1 -
               ((right[curr_bin] - i) / (right[curr_bin] - left[curr_bin]))) *
                  (*N) +
              1;
        }
      } else if ((left[curr_bin] == right[curr_bin]) ||
                 (i < left[curr_bin + 1] && i > right[curr_bin])) {
        if (left[0] == right[0]) {
          T[i] = (curr_bin + 1 - alpha) * (*N);
        } else {
          T[i] = (curr_bin + 1) * (*N);
        }
      }
    } else if (i <= left[0]) {
      T[i] = 0;
    } else if (i >= right[*hist_size - 1]) {
      T[i] = 255;
    }

    // printf("%d %f\n", i, T[i]);
  }
}
