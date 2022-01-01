#ifndef MOUSE_HPP
#define MOUSE_HPP

#include "ui/enumerations/hover_mode.hpp"
#include "ui/enumerations/click_mode.hpp"

/* Static class for mouse modes on hover & on click */
struct Mouse {
  static HoverMode hover_mode;
  static ClickMode click_mode;
};

#endif // MOUSE_HPP
