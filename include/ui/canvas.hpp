#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <string>

#include "imgui.h"
#include "nanovg.h"

#include "framebuffer.hpp"
#include "program.hpp"

#include "tooltips/tooltip_image.hpp"
#include "tooltips/tooltip_pixel.hpp"

/**
 * Canvas where image is displayed
 * Turned into a static class bcos static callback `draw_with_custom_shader()`'s data (texture & program) freed before it's called
 * https://github.com/ocornut/imgui/issues/4770
 */
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
  /* opengl texture for showing image & to paint on (attached to fbo) */
  Image m_image;
  Texture2D m_texture;

  /* nanovg context & image (encapsulates `m_texture`) */
  NVGcontext* m_vg;
  int m_image_vg;

  /* framebuffer used to render image & shapes with nanovg on `m_texture` */
  Framebuffer m_framebuffer;

  /* shaders programs to pick from accord. to effect applied to image */
  std::unordered_map<std::string, Program> m_programs;

  /* Custom shader to show image in grayscale (otherwise 1-channel image shows in shades of red)
   * https://github.com/ocornut/imgui/issues/4748
   * Using a reference would've changed map (m_programs) values when switching to grayscale/color
   */
  Program* m_program;

  /*
   * Holds texture & program to pass to `draw_with_custom_shader()`
   * static bcos if local variable, it's allocated on the stack & freed before `draw_with_custom_shader()` is called => segfault
   * contains GLUints bcos std::tuple container couldn't be constructed (defined) without providing arguments to Program/Texture constructors
   * https://github.com/ocornut/imgui/issues/4770
   */
  static std::array<GLuint, 2> callback_data;

  float m_zoom;

  /* Tooltips */
  TooltipImage m_tooltip_image;
  TooltipPixel m_tooltip_pixel;

  /**
   * Current cursor position (used to draw a line on image)
   * `m_cursor` = (-1, -1) means it wasn't set yet
   */
  const ImVec2 VECTOR_UNSET = ImVec2(-1.0f, -1.0f);
  ImVec2 m_cursor;

  /* static methods can be passed as function pointers callbacks (no `this` argument) */
  static void draw_with_custom_shader(const ImDrawList* parent_list, const ImDrawCmd* cmd);

  void use_shader();
  void unuse_shader();
  void render_image(float y_offset);
  void draw_circle(float x, float y);
};

#endif // CANVAS_HPP
