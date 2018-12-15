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

  Mat hist_glg = Mat(Size(256, 1), CV_8UC1);
  ;

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
  const int hist_size = 256;
  float N;

  //        hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  //        hist_glg = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  //        cvClearHist(hist_glg);

  /* Creates histogram of intensities in the image (grayscale/variance)*/
  calcHist(&gray, 1, &channels, noArray(), hist, 1, &hist_size, &ranges);

  cout << hist.rows << ":" << hist.cols << ":" << hist.channels() << ":"
       << hist.depth() << ":" << hist.type() << endl;

  //  equalizeHist(gray, gray);
  //        cvCalcHist(&gray, hist, 0, NULL);
  minMaxIdx(hist, &min_value, &max_value, 0, 0);
  // cvZero( hist_img );

  cout << "Hist calcucated: " << endl;

  int bin_glg;

  for (s = 0; s < hist_size; s++) {
    int bin_val = hist.at<int>(s);

    if (bin_val != 0) {
      bin_glg = hist_glg.at<int>(n);
      left[n] = s;
      right[n] = s;
      n++;
    }
  }

  cout << "Hist traversed " << endl;
  for (i = n - 40; i > 0; i--) {
    mergeLevels(hist_glg, &max_value, &n, left, right);
  }

  cout << "Hist merged " << endl;

  lookUpTable(&N, left, right, &n, T);

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
    if (min_value > (double)bin_val) {
      min_value = (double)bin_val;
      index = n;
    }
  }
  if (index > 0 && index < (*hist_size) - 1) {
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

  }
}
