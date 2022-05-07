#include "image/image_vg.hpp"
#include "ui/globals/color.hpp"

#include "framebuffer_exception.hpp"

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

ImageVG::ImageVG(const Framebuffer& framebuffer):
  m_framebuffer(framebuffer)
{
  // create nanovg context (similar to html5 canvas)
  m_vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
}

/* Draw circle with nanovg to fbo (i.e. to image texture) */
void ImageVG::draw_circle(float x, float y) {
  // append to framebuffer's attached color buffer
  m_framebuffer.bind();

  // same size `nvgBeginFrame()` as `glViewport()`/texture to avoid stretching drawn shapes
  float pixel_ratio = 1.0f; // framebuffer (i.e. texture) & image have same size
  nvgBeginFrame(m_vg, m_framebuffer.width, m_framebuffer.height, pixel_ratio);

  // draw rectangle & circle on fbo's texture
  nvgBeginPath(m_vg);
  nvgCircle(m_vg, x, y, 25);
  NVGcolor color_fill = { Color::fill.x, Color::fill.y, Color::fill.z, 1.0f - Color::fill.w };
  nvgFillColor(m_vg, color_fill);
  nvgFill(m_vg);
  nvgClosePath(m_vg);

  nvgEndFrame(m_vg);

  // detach framebuffer
  m_framebuffer.unbind();
}

/* Draw line with nanovg to fbo (i.e. to image texture) */
void ImageVG::draw_line(float x1, float y1, float x2, float y2) {
  // append to framebuffer's attached color buffer
  m_framebuffer.bind();

  // same size `nvgBeginFrame()` as `glViewport()`/texture to avoid stretching drawn shapes
  float pixel_ratio = 1.0f; // framebuffer (i.e. texture) & image have same size
  nvgBeginFrame(m_vg, m_framebuffer.width, m_framebuffer.height, pixel_ratio);

  // draw rectangle & circle on fbo's texture
  nvgBeginPath(m_vg);
  nvgMoveTo(m_vg, x1, y1);
  nvgLineTo(m_vg, x2, y2);
  NVGcolor color_stroke = { Color::stroke.x, Color::stroke.y, Color::stroke.z, 1.0f - Color::stroke.w };
  nvgStrokeWidth(m_vg, 10.0f);
  nvgStrokeColor(m_vg, color_stroke);
  nvgStroke(m_vg);

  nvgEndFrame(m_vg);

  // detach framebuffer
  m_framebuffer.unbind();
}

void ImageVG::free() {
  // free nanovg context
  nvgDeleteGL3(m_vg);
}
