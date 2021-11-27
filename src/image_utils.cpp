/**
 * Inspired by:
 * https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/
 */
#include <iostream>
#include "image_utils.hpp"

/* Convert image to grayscale by averaging rgb components */
/*
// output a 3-channel image, same value set for red, green & blue components in output image
Image ImageUtils::to_gray(const Image& image_in) {
  int n_channels = image_in.n_channels;
  int n_bytes = image_in.width * image_in.height * n_channels;
  unsigned char* data_in = image_in.data;
  unsigned char* data_out = new unsigned char[n_bytes];

  for (size_t i_byte = 0; i_byte < n_bytes; i_byte += n_channels) {
    unsigned char red = data_in[i_byte];
    unsigned char green = data_in[i_byte + 1];
    unsigned char blue = data_in[i_byte + 2];

    // didn't work with single byte per pixel & format = GL_RED
    unsigned char out = (red + green + blue) / 3;
    for (size_t i_channel = 0; i_channel < n_channels; i_channel++) {
      data_out[i_byte + i_channel] = out;
    }
  }

  // new image for result & free input image
  Image image_out(image_in.width, image_in.height, image_in.format, data_out, image_in.path);
  image_in.free();

  return image_out;
}
*/

/* Convert image to grayscale by averaging rgb components */
Image ImageUtils::to_gray(const Image& image_in) {
  int n_channels = image_in.n_channels;
  std::cout << "n_channels: " << n_channels << '\n';
  // int n_bytes = image_in.width * image_in.height * n_channels;
  int n_pixels = image_in.width * image_in.height;
  unsigned char* data_in = image_in.data;
  // unsigned char* data_out = new unsigned char[n_bytes];
  unsigned char* data_out = new unsigned char[n_pixels];

  // for (size_t i_byte = 0; i_byte < n_bytes; i_byte += n_channels) {
  for (size_t i_pixel = 0; i_pixel < n_pixels; i_pixel++) {
    unsigned char red = data_in[n_channels*i_pixel];
    unsigned char green = data_in[n_channels*i_pixel + 1];
    unsigned char blue = data_in[n_channels*i_pixel + 2];

    // output single-channel image
    // didn't work with single byte per pixel & format = GL_RED
    unsigned char out = (red + green + blue) / 3;
    data_out[i_pixel] = out;
  }

  // new image for result & free input image
  // Image image_out(image_in.width, image_in.height, image_in.format, data_out, image_in.path);
  Image image_out(image_in.width, image_in.height, GL_RED, data_out, image_in.path);
  image_in.free();

  return image_out;
}
