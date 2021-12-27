#ifndef MENU_HPP
#define MENU_HPP

#include "imgui.h"

/* Main menu with booleans flags set on menu item click */
struct Menu {
  /* width/height in # of pixels needed to position & set size of image canvas */
  ImVec2 size;

  /* flags set on button click (needed to activate listeners in `Dialog`) */
  bool open_image, save_image, quit_app; // menu File
  bool to_grayscale; // menu Edit
  bool view_color, view_grayscale, view_monochrome; // menu View
  bool zoom_in, zoom_out; // menu Zoom

  Menu();
  void render();
};

#endif // MENU_HPP
