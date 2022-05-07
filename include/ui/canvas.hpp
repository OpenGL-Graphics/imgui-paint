#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <string>

#include "imgui.h"

#include "framebuffer.hpp"
#include "program.hpp"
#include "render/renderer.hpp"

#include "tooltips/tooltip_image.hpp"
#include "tooltips/tooltip_pixel.hpp"

#include "image/image_vg.hpp"

/* Canvas where image is displayed */
class Canvas {
public:
  Canvas(const std::string path_image);
  void render();
  void free();

  void set_shader(const std::string& key);

  void change_image(const std::string& path_image);
  void save_image(const std::string& path_image);
  void to_grayscale();
  void blur();

  void zoom_in();
  void zoom_out();

  void move_cursor();
  void draw(const std::string& type_shape, bool has_strokes=true);

private:
  /* shaders programs to pick from accord. to effect applied to image */
  std::unordered_map<std::string, Program> m_programs;

  /* texture to pass to shader (original) & texture to render to with fbo (modified by shader or with shapes drawn with VG) */
  Texture2D m_texture_image;
  Texture2D m_texture_framebuffer;

  /* framebuffer used to render image & shapes with nanovg on `m_texture_framebuffer` (used to read pixels in tooltip) */
  Framebuffer m_framebuffer;
  Renderer m_renderer;

  /* for drawing shapes on texture (through fbo) using VG */
  ImageVG m_image_vg;

  float m_zoom;

  /* Tooltips */
  TooltipImage m_tooltip_image;
  TooltipPixel m_tooltip_pixel;

  void render_to_fbo();
  void render_image(float y_offset);
};

#endif // CANVAS_HPP
