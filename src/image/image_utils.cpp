/**
 * Inspired by:
 * https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/
 */
#include <iostream>
#include <cstring>

#include "image/image_utils.hpp"

/* Convert image to grayscale by averaging rgb components */
Image ImageUtils::to_grayscale(Image& image_in) {
  int n_channels = image_in.n_channels;
  std::cout << "n_channels: " << n_channels << '\n';
  int n_pixels = image_in.width * image_in.height;
  unsigned char* data_in = image_in.data;
  unsigned char* data_out = new unsigned char[n_pixels];

  for (size_t i_pixel = 0; i_pixel < n_pixels; i_pixel++) {
    unsigned char red = data_in[n_channels*i_pixel];
    unsigned char green = data_in[n_channels*i_pixel + 1];
    unsigned char blue = data_in[n_channels*i_pixel + 2];

    // output single-channel image
    unsigned char out = (red + green + blue) / 3;
    data_out[i_pixel] = out;
  }

  // new image for result & free input image
  Image image_out(image_in.width, image_in.height, 1, data_out);
  image_in.free();

  return image_out;
}

/* Blur image using averaging filter */
Image ImageUtils::blur(Image& image_in) {
  int n_channels = image_in.n_channels;
  int width = image_in.width;
  int height = image_in.height;
  unsigned char* data_in = image_in.data;

  // transform image data to 2D array
  unsigned char** data_in_2d = image_in.to_2d_array();

  // 9x9 averaging kernel
  size_t n_bytes_row = width * n_channels;
  unsigned char** data_out_2d = new unsigned char*[height];

  for (size_t i_height = 0; i_height < height; i_height++) {
    data_out_2d[i_height] = new unsigned char[n_bytes_row];

    for (size_t i_width = 0; i_width < width; i_width++) {
      // pixels on border directly copied
      if (i_height == 0 || i_width == 0 || i_height == height - 1 || i_width == width - 1) {
        for (size_t i_channel = 0; i_channel < n_channels; i_channel++) {
          size_t i_col = n_channels*i_width + i_channel;
          data_out_2d[i_height][i_col] = data_in_2d[i_height][i_col];
        }
        continue;
      }

      // average pixel values in 9x9 neighborhood
      for (size_t i_channel = 0; i_channel < n_channels; i_channel++) {
        size_t i_col_m1 = n_channels*(i_width - 1) + i_channel;
        size_t i_col = n_channels*i_width + i_channel;
        size_t i_col_p1 = n_channels*(i_width + 1) + i_channel;

        unsigned char out = (
            data_in_2d[i_height - 1][i_col_m1] + data_in_2d[i_height - 1][i_col] + data_in_2d[i_height - 1][i_col_p1] +
            data_in_2d[i_height][i_col_m1]     + data_in_2d[i_height][i_col]     + data_in_2d[i_height][i_col_p1]     +
            data_in_2d[i_height + 1][i_col_m1] + data_in_2d[i_height + 1][i_col] + data_in_2d[i_height + 1][i_col_p1]
          ) / 9;
        data_out_2d[i_height][i_col] = out;
      }
    }
  }

  delete[] data_in_2d;

  // new image for result & free input image
  Image image_out = Image::from_2d_array(data_out_2d, width, height, n_channels);
  image_in.free();

  return image_out;
}
