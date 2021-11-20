#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <string>

#include "window.hpp"
#include "texture.hpp"
#include "image.hpp"

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

  void render_components();
  void render_menu();
};

#endif // DIALOG_HPP
