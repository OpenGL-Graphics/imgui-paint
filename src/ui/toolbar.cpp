#include <iostream>

#include "IconsFontAwesome5.h"
#include "ui/toolbar.hpp"

/* static members definition (avoids linking error) & initialization */
bool Toolbar::open_image = false;
bool Toolbar::save_image = false;
bool Toolbar::quit_app = false;
bool Toolbar::zoom_in = false;
bool Toolbar::zoom_out = false;
HoverMode Toolbar::hover_mode = HoverMode::NONE;

Toolbar::Toolbar()
{
}

/**
 * Button with FontAwesome fonts as labels
 * @param y_offset Menu height
 */
void Toolbar::render(float y_offset) {
  // precalculate toolbar size a priori
  float size_font = ImGui::GetFontSize();
  ImGuiIO& io = ImGui::GetIO(); // configures imgui
  ImVec2 size_display = io.DisplaySize;
  size = { size_display.x, 2*size_font };

  // change background color to button color
  ImGuiStyle style = ImGui::GetStyle();
  ImVec4 color_button = style.Colors[ImGuiCol_Button];
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertFloat4ToU32(color_button));

  // anchored imgui window of specified size without padding
  ImGui::SetNextWindowPos({ 0.0f, y_offset });
  ImGui::SetNextWindowSize(size);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  bool p_open;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
  ImGui::Begin("Toolbar", &p_open, window_flags);

  // buttons on same line & filling all vertical space
  if (ImGui::Button(ICON_FA_FOLDER_OPEN, { 2*size_font, -1.0f })) {
    Toolbar::open_image = true;
  }
  ImGui::SameLine(2*size_font + 1); // relative to window left corner

  if (ImGui::Button(ICON_FA_SAVE, { 2*size_font, -1.0f })) {
    Toolbar::save_image = true;
  }
  ImGui::SameLine(2 * (2*size_font + 1)); // relative to window left corner

  if (ImGui::Button(ICON_FA_WINDOW_CLOSE, { 2*size_font, -1.0f })) {
    Toolbar::quit_app = true;
  }
  ImGui::SameLine(3 * (2*size_font + 1)); // relative to window left corner

  if (ImGui::Button(ICON_FA_PLUS_CIRCLE, { 2*size_font, -1.0f })) {
    Toolbar::zoom_in = true;
  }
  ImGui::SameLine(4 * (2*size_font + 1)); // relative to window left corner

  if (ImGui::Button(ICON_FA_MINUS_CIRCLE, { 2*size_font, -1.0f })) {
    Toolbar::zoom_out = true;
  }
  ImGui::SameLine(5 * (2*size_font + 1)); // relative to window left corner

  // radio buttons for what to show on image hover (imgui_demo.cpp:560)
  // compile-time casting between pointer types works with reinterpret_cast (not with static_cast)
  ImGui::SetCursorPos({ ImGui::GetCursorPosX(), size_font/2.0f - 3.0f });
  ImGui::RadioButton("None", reinterpret_cast<int *>(&Toolbar::hover_mode), static_cast<int>(HoverMode::NONE));
  ImGui::SameLine();
  ImGui::RadioButton("Image subset", reinterpret_cast<int *>(&Toolbar::hover_mode), static_cast<int>(HoverMode::IMAGE_SUBSET));
  ImGui::SameLine();
  ImGui::RadioButton("Pixel value", reinterpret_cast<int *>(&Toolbar::hover_mode), static_cast<int>(HoverMode::PIXEL_VALUE));

  // avoids applying same style to subsequent windows
  ImGui::PopStyleColor();
  ImGui::PopStyleVar(1);
  ImGui::End();
}
