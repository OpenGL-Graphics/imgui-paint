#ifndef FRAME_HPP
#define FRAME_HPP

#include <string>

#include "imgui.h"

#include "window.hpp"
#include "texture.hpp"
#include "image.hpp"
#include "program.hpp"

#include "ui/canvas.hpp"
#include "ui/menu.hpp"
#include "ui/toolbar.hpp"

#include "ui/listeners/listener_canvas.hpp"
#include "ui/listeners/listener_window.hpp"

/* Main ImGui window */
class Frame {
public:
  Frame(const Window& window);
  void render();
  void free();

private:
  Window m_window;

  /* Main menu, image holder, toolbar components */
  Menu m_menu;
  Canvas m_canvas;
  Toolbar m_toolbar;

  /* listeners for events rel. to canvas & window */
  ListenerCanvas m_listener_canvas;
  ListenerWindow m_listener_window;
};

#endif // FRAME_HPP
