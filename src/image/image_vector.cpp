#include <iostream>
#include <cmath>

#include "image/image_vector.hpp"

/**
 * Construct a Cairo surface from provided png image
 * Tricky to convert image data to Cairo formats (ARGB) from an in-memory image (i.e. stb standard RGBA format)
 * Another block of data would need to be allocated anyway!
 * https://www.cairographics.org/manual/cairo-Image-Surfaces.html#cairo-format-t
 */
ImageVector::ImageVector(const Image& image) {
  // cairo surface from png image
  m_surface = cairo_image_surface_create_from_png(image.path.c_str());

  // create context with surface as target
  m_context = cairo_create(m_surface);
}

/* Check error status for opened surface (e.g. "out of memory" when trying to open jpeg) */
bool ImageVector::has_failed() {
  cairo_status_t status = cairo_surface_status(m_surface);
  std::string message = cairo_status_to_string(status);
  std::cout << "Opening png with Cairo: " << message << '\n';

  return status != CAIRO_STATUS_SUCCESS;
}

/**
 * Draw filled circle on image
 * Better quality with Cairo compared to OpenCV (thanks to vectors)
 */
void ImageVector::draw_circle(double x, double y) {
  // circle outlines
  cairo_set_source_rgb(m_context, 0.0, 0.0, 1.0);
  cairo_arc(m_context, x, y, 5.0, 0.0, 2*M_PI);
  cairo_stroke_preserve(m_context);

  // fill drawn circle outlines
  cairo_set_source_rgb(m_context, 0.0, 1.0, 1.0);
  cairo_fill(m_context);
}

/* Draw line from starting point defined in `start_line()` to given point */
void ImageVector::draw_line(double x_start, double y_start, double x_end, double y_end) {
  cairo_move_to(m_context, x_start, y_start);
  cairo_line_to(m_context, x_end, y_end);
  cairo_stroke(m_context);
}

/**
 * Save modified image to temporary file
 * in-memory image data has a different format from Cairo surface's (alpha channel at beginning, stride, pre-multiplied alpha)
 * https://www.cairographics.org/manual/cairo-Image-Surfaces.html#cairo-format-t
 */
void ImageVector::save(const std::string& path_image) {
  cairo_status_t status = cairo_surface_write_to_png(m_surface, path_image.c_str());

  // status message e.g. "out of memory" when trying to save an opened jpeg surface
  std::string message = cairo_status_to_string(status);
  std::cout << "Saving png with Cairo: " << message << '\n';
}

/* Free context & created surface */
void ImageVector::free() {
  cairo_destroy(m_context);
  cairo_surface_destroy(m_surface);
}
