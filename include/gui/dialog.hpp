#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <string>

#include "window.hpp"
#include "texture.hpp"

class Dialog {
public:
  Dialog(const Window& window);
  void render();
  void free();

private:
  Window m_window;

  /* opengl texture for showing image */
  Texture2D m_texture;

  void render_components();
  void render_menu();
};

#endif // DIALOG_HPP
