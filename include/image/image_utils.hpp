#ifndef IMAGE_UTILS_HPP
#define IMAGE_UTILSHPP

#include "image.hpp"

namespace ImageUtils {
  Image to_grayscale(Image& image_in);
  Image blur(Image& image_in);
};

#endif // IMAGE_UTILS_HPP
