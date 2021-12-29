#include "ui/listeners/listener_window.hpp"
#include "ui/menu.hpp"
#include "ui/toolbar.hpp"

/**
 * @param canvas Pointer passed so it can be modified (instead of modifying a copy)
 */
ListenerWindow::ListenerWindow(Window* window):
  m_window(window)
{
}

/* Handle all events related to glfw window fired on click on menu items or toolbar buttons */
void ListenerWindow::handle_all() {
  on_quit_app();
}

void ListenerWindow::on_quit_app() {
  if (Menu::quit_app || Toolbar::quit_app) {
    m_window->close();
  }
}
