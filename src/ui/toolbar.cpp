#include <iostream>

#include "imgui/imgui.h"

#include "IconsFontAwesome5.h"
#include "ui/toolbar.hpp"
#include "ui/menu.hpp"
#include "ui/constants/mouse.hpp"
#include "ui/constants/size.hpp"

/* static members definition (avoids linking error) & initialization */
bool Toolbar::open_image = false;
bool Toolbar::save_image = false;
bool Toolbar::quit_app = false;
bool Toolbar::zoom_in = false;
bool Toolbar::zoom_out = false;
bool Toolbar::draw_circle = false;

Toolbar::Toolbar()
{
}

/**
 * Button with FontAwesome fonts as labels
 * @param y_offset Menu height
 */
void Toolbar::render() {
  // precalculate toolbar size a priori
  float size_font = ImGui::GetFontSize();
  ImGuiIO& io = ImGui::GetIO(); // configures imgui
  ImVec2 size_display = io.DisplaySize;
  Size::toolbar = { size_display.x, 2*size_font };

  // change background color to button color
  ImGuiStyle style = ImGui::GetStyle();
  ImVec4 color_button = style.Colors[ImGuiCol_Button];
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertFloat4ToU32(color_button));

  // anchored imgui window of specified size without padding
  ImGui::SetNextWindowPos({ 0.0f, Size::menu.y });
  ImGui::SetNextWindowSize(Size::toolbar);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  bool p_open;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
  ImGui::Begin("Toolbar", &p_open, window_flags);

  // avoids applying same style to subsequent windows (& to button tooltips)
  ImGui::PopStyleVar(1);
  ImGui::PopStyleColor();

  // buttons on same line & filling all vertical space
  if (ImGui::Button(ICON_FA_FOLDER_OPEN, { 2*size_font, -1.0f })) {
    Toolbar::open_image = true;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Open image");
  ImGui::SameLine(2*size_font + 1); // relative to window left corner

  if (ImGui::Button(ICON_FA_SAVE, { 2*size_font, -1.0f })) {
    Toolbar::save_image = true;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Save image");
  ImGui::SameLine(2 * (2*size_font + 1)); // relative to window left corner

  if (ImGui::Button(ICON_FA_WINDOW_CLOSE, { 2*size_font, -1.0f })) {
    Toolbar::quit_app = true;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Quit");
  ImGui::SameLine(3 * (2*size_font + 1)); // relative to window left corner

  if (ImGui::Button(ICON_FA_PLUS_CIRCLE, { 2*size_font, -1.0f })) {
    Toolbar::zoom_in = true;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Zoom in");
  ImGui::SameLine(4 * (2*size_font + 1)); // relative to window left corner

  if (ImGui::Button(ICON_FA_MINUS_CIRCLE, { 2*size_font, -1.0f })) {
    Toolbar::zoom_out = true;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Zoom out");
  ImGui::SameLine(5 * (2*size_font + 1)); // relative to window left corner

  // toolbar button disabled if already in right mode
  if (Mouse::click_mode == ClickMode::DRAW_CIRCLE)
    ImGui::BeginDisabled(true);

  if (ImGui::Button(ICON_FA_CIRCLE, { 2*size_font, -1.0f })) {
    Mouse::click_mode = ClickMode::DRAW_CIRCLE;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Draw circle");

  if (Mouse::click_mode == ClickMode::DRAW_CIRCLE)
    ImGui::EndDisabled();

  ImGui::SameLine(6 * (2*size_font + 1)); // relative to window left corner

  // radio buttons for what to show on image hover (imgui_demo.cpp:560)
  // compile-time casting between pointer types works with reinterpret_cast (not with static_cast)
  ImGui::SetCursorPos({ ImGui::GetCursorPosX(), size_font/2.0f - 3.0f });
  ImGui::RadioButton("None", reinterpret_cast<int *>(&Mouse::hover_mode), static_cast<int>(HoverMode::NONE));
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Show nothing on hover");
  ImGui::SameLine();

  ImGui::RadioButton("Image subset", reinterpret_cast<int *>(&Mouse::hover_mode), static_cast<int>(HoverMode::IMAGE_SUBSET));
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Show image subset on hover");
  ImGui::SameLine();

  ImGui::RadioButton("Pixel value", reinterpret_cast<int *>(&Mouse::hover_mode), static_cast<int>(HoverMode::PIXEL_VALUE));
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Show hovered pixel value");

  ImGui::End();
}
