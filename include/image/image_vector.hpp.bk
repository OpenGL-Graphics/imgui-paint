#ifndef IMAGE_VECTOR_HPP
#define IMAGE_VECTOR_HPP

#include <cairo.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "imgui/imgui.h"

#include "image.hpp"

/* Wrapper around cairo_surface_t */
class ImageVector {
public:
  ImageVector(const std::string& path_image);
  bool has_failed();
  void draw_circle(const ImVec2& center, float radius, bool has_strokes=true);
  void draw_line(const ImVec2& point_start, const ImVec2& point_end);
  void free();
  void save(const std::string& path_image);
  unsigned char* get_data();
  unsigned char* get_subdata(const ImVec2& size, const ImVec2& offset);

  void free_pixbuf();

private:
  cairo_surface_t* m_surface;
  cairo_t* m_context;

  /* avoids memory leak (cannot free just after getting `char*` as it'd free data too */
  GdkPixbuf* m_pixbuf;
};

#endif // IMAGE_VECTOR_HPP
