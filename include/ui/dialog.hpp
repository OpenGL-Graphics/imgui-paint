#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <string>

#include "imgui.h"

#include "window.hpp"
#include "texture.hpp"
#include "image.hpp"
#include "program.hpp"
#include "ui/canvas.hpp"

/* Main ImGui window */
class Dialog {
public:
  Dialog(const Window& window);
  void render();
  void free();

private:
  Window m_window;

  /* needed to position windows & content within them */
  ImVec2 m_size_menu;

  /* Image holder */
  Canvas m_canvas;

  void render_menu();
};

#endif // DIALOG_HPP
