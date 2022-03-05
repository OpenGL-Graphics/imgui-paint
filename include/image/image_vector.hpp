#ifndef IMAGE_VECTOR_HPP
#define IMAGE_VECTOR_HPP

#include <cairo.h>

#include "imgui/imgui.h"

#include "image.hpp"

/* Wrapper around cairo_surface_t */
class ImageVector {
public:
  ImageVector(const std::string& path_image);
  bool has_failed();
  void draw_circle(const ImVec2& center, bool has_strokes=true);
  void draw_line(const ImVec2& point_start, const ImVec2& point_end);
  void free();
  void save(const std::string& path_image);
  unsigned char* get_data();

private:
  cairo_surface_t* m_surface;
  cairo_t* m_context;
};

#endif // IMAGE_VECTOR_HPP
