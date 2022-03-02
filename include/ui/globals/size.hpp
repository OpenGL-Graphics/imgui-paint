#ifndef SIZE_HPP
#define SIZE_HPP

#include "imgui/imgui.h"

/**
 * Static class for UI components sizes
 * width/height in # of pixels needed to position & set size of image canvas
 * declared static to make it global (avoid passing it to each & every function)
 */
struct Size {
  static ImVec2 menu;
  static ImVec2 toolbar;
  static ImVec2 canvas;
};

#endif // SIZE_HPP
