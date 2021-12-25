#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <string>

#include "imgui.h"
#include "program.hpp"

/**
 * Canvas where image is displayed
 * Turned into a static class bcos static callback `draw_with_custom_shader()`'s data (texture & program) freed before it's called
 * https://github.com/ocornut/imgui/issues/4770
 */
class Canvas {
public:
  Canvas();
  void render(float y_offset);
  void free();

  void set_shader(const std::string& key);

  void change_image(const std::string& path_image);
  void save_image(const std::string& path_image);
  void to_grayscale();

private:
  /**
   * opened image to process
   */
  Image m_image;

  /**
   * opengl texture for showing image
   * static bcos otherwise allocated on the stack & freed before `draw_with_custom_shader()` is called => segfault
   * https://github.com/ocornut/imgui/issues/4770
   */
  Texture2D m_texture;

  /* shaders programs to pick from accord. to effect applied to image */
  std::unordered_map<std::string, Program> m_programs;

  /* Custom shader to show image in grayscale (otherwise 1-channel image shows in shades of red)
   * https://github.com/ocornut/imgui/issues/4748
   * Using a reference would've changed map (m_programs) values when switching to grayscale/color
   */
  Program* m_program;

  void use_shader();
  void unuse_shader();
  void render_image();

  /*
   * Holds texture & program to pass to `draw_with_custom_shader()`
   * static bcos if local variable, it's allocated on the stack & freed before `draw_with_custom_shader()` is called => segfault
   * contains GLUints bcos std::tuple container couldn't be constructed (defined) without providing arguments to Program/Texture constructors
   * https://github.com/ocornut/imgui/issues/4770
   */
  static std::array<GLuint, 2> callback_data;

  /* static methods can be passed as function pointers callbacks (no `this` argument) */
  static void draw_with_custom_shader(const ImDrawList* parent_list, const ImDrawCmd* cmd);
};

#endif // CANVAS_HPP
