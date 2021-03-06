#ifndef IMAGE_VG_HPP
#define IMAGE_VG_HPP

#include "nanovg.h"

#include "texture.hpp"
#include "framebuffer.hpp"

/* Wrapper class for drawing on opengl texture with NanoVG */
struct ImageVG {
  ImageVG();
  void draw_circle(const Framebuffer& framebuffer, float x, float y);
  void draw_line(const Framebuffer& framebuffer, float x1, float y1, float x2, float y2);
  void free();

private:
  /* nanovg context */
  NVGcontext* m_vg;
};

#endif // IMAGE_VG_HPP
