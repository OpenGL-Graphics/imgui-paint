#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <string>

#include "imgui.h"

#include "window.hpp"
#include "texture.hpp"
#include "image.hpp"
#include "program.hpp"

class Dialog {
public:
  Dialog(const Window& window);
  void render();
  void free();

private:
  Window m_window;

  /* opened image to process */
  Image m_image;

  /* opengl texture for showing image */
  Texture2D m_texture;

  /* shaders programs to pick from accord. to effect applied to image */
  std::unordered_map<std::string, Program> m_programs;

  /* Custom shader to show image in grayscale (otherwise 1-channel image shows in shades of red)
   * https://github.com/ocornut/imgui/issues/4748
   * Using a reference would've changed map (m_programs) values when switching to grayscale/color
   */
  Program* m_program;

  void render_menu();
  void render_image();

  /* needed to position windows & content within them */
  ImVec2 m_size_menu;
  ImVec2 m_size_content;

  /* static methods can be passed as function pointers callbacks (no `this` argument) */
  static void draw_with_custom_shader(const ImDrawList* parent_list, const ImDrawCmd* cmd);
};

#endif // DIALOG_HPP
