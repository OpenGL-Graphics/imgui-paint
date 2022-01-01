#include <iostream>
#include <opencv2/imgproc.hpp>

#include "image/image_cv.hpp"

/* Construct a cv::Mat from a pointer to an unsigned char */
ImageCV::ImageCV(const Image& image):
  mat(image.height, image.width, get_type_mat(image.n_channels), image.data)
{
}

/* image type accord. to # of channels */
int ImageCV::get_type_mat(int n_channels) {
  switch (n_channels) {
    case 4:
      return CV_8UC4;
    case 3:
      return CV_8UC3;
    default:
      return CV_8UC1;
  }
}

/* Draw a circle on image */
void ImageCV::draw_circle(const cv::Point& center) {
  cv::Scalar color(0xff, 0xff, 0xff);
  cv::circle(mat, center, 5, color);   
}
