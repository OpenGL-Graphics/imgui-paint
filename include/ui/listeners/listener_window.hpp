#ifndef LISTENER_WINDOW_HPP
#define LISTENER_WINDOW_HPP

#include "window.hpp"

/* Handler for events pertaining to glfw window */
class ListenerWindow {
public:
  ListenerWindow(Window* window);
  void handle_all();
private:
  Window* m_window;

  void on_quit_app();
};

#endif // LISTENER_WINDOW_HPP
