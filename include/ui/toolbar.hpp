#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP

#include "imgui.h"

/* Main toolbar with booleans flags set on button click */
struct Toolbar {
  /* width/height in # of pixels to position & set size of image canvas */
  ImVec2 size;

  /* flags set on button click (needed to activate listeners in `Dialog`) */
  bool open_image, save_image, quit_app;

  Toolbar();
  void render(float y_offset);
};

#endif // TOOLBAR_HPP
