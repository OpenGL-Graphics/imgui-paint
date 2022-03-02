#ifndef COLOR_HPP
#define COLOR_HPP

#include "imgui/imgui.h"

/**
 * Static class for UI stroke & fill colors used when drawing with Cairo
 * declared static to make it global (avoid passing it to each & every function)
 */
struct Color {
  static ImVec4 stroke;
  static ImVec4 fill;
};

#endif // COLOR_HPP
