#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <string>

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

  /* Custom shader to show image in grayscale (otherwise 1-channel image shows in shades of red)
   * https://github.com/ocornut/imgui/issues/4748
   */
  Program m_program;

  void render_components();
  void render_menu();
  void change_shader();
};

#endif // DIALOG_HPP
