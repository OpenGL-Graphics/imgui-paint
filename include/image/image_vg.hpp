#ifndef IMAGE_VG_HPP
#define IMAGE_VG_HPP

#include "nanovg.h"

#include "texture.hpp"
#include "framebuffer.hpp"

/* Wrapper class for drawing on opengl texture with NanoVG */
struct ImageVG {
  /* framebuffer used to render image & shapes with nanovg on `m_texture` (used to read pixels in tooltip) */
  Framebuffer framebuffer;

  ImageVG(const Texture2D& texture);
  void draw_circle(float x, float y);
  void draw_line(float x1, float y1, float x2, float y2);
  void free();

private:
  /* opengl texture for showing image & to paint on (attached to fbo) */
  Texture2D m_texture;

  /* nanovg context */
  NVGcontext* m_vg;
};

#endif // IMAGE_VG_HPP
