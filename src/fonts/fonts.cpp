#include "imgui.h"
#include "IconsFontAwesome5.h"

#include "fonts/fonts.hpp"

/**
 * Load DroidSans text font & fontAwesome icon font
 * `ImGui::GetIO()` requires ImGui context to have been created
 */
void Fonts::load() {
  // load droid-sans text font (first font loaded used by default without push/pop)
  ImGuiIO& io = ImGui::GetIO(); // configures imgui
  float size_pixel = 16.0f;
  ImFont* font = io.Fonts->AddFontFromFileTTF("assets/fonts/DroidSans.ttf", size_pixel);

  // merge font-awesome icons with previous font (droid-sans)
  ImFontConfig icon_font_config;
  icon_font_config.MergeMode = true;
  const ImWchar range_glyphs[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
  ImFont* font_icon = io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", size_pixel, &icon_font_config, range_glyphs);

  // font's texture atlas normally auto built but `range_glyphs` needs to be persistent when called
  // https://github.com/ocornut/imgui/blob/master/docs/FONTS.md
  io.Fonts->Build();
}
