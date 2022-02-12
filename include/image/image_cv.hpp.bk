#ifndef IMAGE_CV_HPP
#define IMAGE_CV_HPP

#include <opencv2/core/mat.hpp>

#include "image.hpp"

/* Wrapper around cv::Mat */
struct ImageCV {
  cv::Mat mat;

  ImageCV(const Image& image);
  void draw_circle(const cv::Point& center);
private:
  int get_type_mat(int n_channels);
};

#endif // IMAGE_CV_HPP
