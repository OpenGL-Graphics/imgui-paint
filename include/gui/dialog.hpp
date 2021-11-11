#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <string>

#include "window.hpp"

class Dialog {
public:
  Dialog(const Window& window);
  void render();
  void free();

private:
  Window m_window;

  void render_components();
};

#endif // DIALOG_HPP
