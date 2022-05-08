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

  /**
   * In Normal mode: texture to pass to shader drawing surface geometry
   * In Draw shapes mode: Texture attached to fbo
   * Initially: contains image
   */
  Texture2D m_texture_shapes;

  /**
   * In Normal mode: Texture attached to fbo (to render surface geometry to)
   * In Draw shapes mode: unused
   * Initially: empty (cleared in each frame)
   */
  Texture2D m_texture_effects;

  /**
   * Framebuffer used to:
   * - Render image accord. to shader used (`m_texture_effects` attached) - e.g. in grayscale
   * - Draw shapes with nanovg (`m_texture_shapes` attached)
   * - To read pixels values in tooltip
   */
  Framebuffer m_framebuffer;
  Renderer m_renderer;

  /* Needed to work out mouse location rel. to image */
  int m_width;
  int m_height;

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
