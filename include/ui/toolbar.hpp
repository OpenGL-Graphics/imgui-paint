#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP

#include "imgui.h"

#include "ui/hover_mode.hpp"

/* Main toolbar with booleans flags set on button click */
struct Toolbar {
  /* width/height in # of pixels to position & set size of image canvas */
  ImVec2 size;

  /**
   * flags set on button click/radio button check (needed to activate listeners in `Dialog`)
   * Declared static so they can be accessed from all classes (incl. listeners)
   */
  static bool open_image, save_image, quit_app;
  static bool zoom_in, zoom_out;
  static HoverMode hover_mode;

  Toolbar();
  void render(float y_offset);
};

#endif // TOOLBAR_HPP
