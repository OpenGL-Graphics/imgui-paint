#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <string>

#include "imgui.h"

#include "window.hpp"
#include "texture.hpp"
#include "image.hpp"
#include "program.hpp"
#include "ui/canvas.hpp"
#include "ui/menu.hpp"
#include "ui/toolbar.hpp"

/* Main ImGui window */
class Dialog {
public:
  Dialog(const Window& window);
  void render();
  void free();

private:
  Window m_window;

  /* Main menu, image holder, toolbar */
  Menu m_menu;
  Canvas m_canvas;
  Toolbar m_toolbar;

  /* listener for menu click events */
  void on_menu_click();
};

#endif // DIALOG_HPP
