#ifndef IMAGE_VECTOR_HPP
#define IMAGE_VECTOR_HPP

#include <cairo.h>

#include "image.hpp"

/* Wrapper around cairo_surface_t */
class ImageVector {
public:
  ImageVector(const Image& image);
  bool has_failed();
  void draw_circle(double x, double y);
  void free();
  void save(const std::string& path_image);

private:
  cairo_surface_t* m_surface;
  cairo_t* m_context;
};

#endif // IMAGE_VECTOR_HPP
