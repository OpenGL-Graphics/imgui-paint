#include <iostream>

#include "imgui/imgui.h"

#include "IconsFontAwesome5.h"
#include "ui/toolbar.hpp"
#include "ui/menu.hpp"
#include "ui/enumerations/hover_mode.hpp"

#include "ui/globals/size.hpp"
#include "ui/globals/color.hpp"
#include "ui/globals/cursor.hpp"
#include "ui/globals/mode.hpp"

/* static members definition (avoids linking error) & initialization */
bool Toolbar::open_image = false;
bool Toolbar::save_image = false;
bool Toolbar::quit_app = false;
bool Toolbar::zoom_in = false;
bool Toolbar::zoom_out = false;
bool Toolbar::draw_circle = false;
bool Toolbar::draw_line = false;
bool Toolbar::brush_circle = false;
bool Toolbar::brush_line = false;

// radio button (0: none, 1: image subset, 2: pixel value)
int Toolbar::hover_mode = HoverMode::NONE;

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
  ImGui::PushStyleColor(ImGuiCol_WindowBg, color_button);

  // anchored imgui window of specified size without padding
  ImGui::SetNextWindowPos({ 0.0f, Size::menu.y });
  ImGui::SetNextWindowSize(Size::toolbar);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  bool p_open;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoScrollbar |ImGuiWindowFlags_NoScrollWithMouse;
  ImGui::Begin("Toolbar", &p_open, window_flags);

  // avoids applying same style to subsequent windows (& to button tooltips)
  ImGui::PopStyleVar(1);
  ImGui::PopStyleColor();

  // buttons on same line & filling all vertical space (hence height = -1)
  if (ImGui::Button(ICON_FA_FOLDER_OPEN, { 2*size_font, -1.0f })) {
    Toolbar::open_image = true;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Open image");
  ImGui::SameLine(0, 1); // offset=0: right after previous item, spacing=1px

  if (ImGui::Button(ICON_FA_SAVE, { 2*size_font, -1.0f })) {
    Toolbar::save_image = true;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Save image");
  ImGui::SameLine(0, 1); // offset=0: right after previous item, spacing=1px

  if (ImGui::Button(ICON_FA_WINDOW_CLOSE, { 2*size_font, -1.0f })) {
    Toolbar::quit_app = true;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Quit");
  ImGui::SameLine(0, 1); // offset=0: right after previous item, spacing=1px

  if (ImGui::Button(ICON_FA_PLUS_CIRCLE, { 2*size_font, -1.0f })) {
    Toolbar::zoom_in = true;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Zoom in");
  ImGui::SameLine(0, 1); // offset=0: right after previous item, spacing=1px

  if (ImGui::Button(ICON_FA_MINUS_CIRCLE, { 2*size_font, -1.0f })) {
    Toolbar::zoom_out = true;
  }
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Zoom out");
  ImGui::SameLine(0, 1); // offset=0: right after previous item, spacing=1px

  // toolbar button disabled if already in right mode (https://github.com/ocornut/imgui/issues/5011)
  ImGui::BeginDisabled(Toolbar::draw_circle);
  if (ImGui::Button(ICON_FA_CIRCLE, { 2*size_font, -1.0f })) {
    Toolbar::draw_circle = Menu::draw_circle = true;
    mode = (mode == Mode::NORMAL) ? Mode::DRAWING : Mode::NORMAL;
  }
  ImGui::EndDisabled();

  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("Draw circle");
  ImGui::SameLine(0, 1); // offset=0: right after previous item, spacing=1px

  // toolbar button disabled if already in right mode (https://github.com/ocornut/imgui/issues/5011)
  ImGui::BeginDisabled(Toolbar::draw_line);
  if (ImGui::Button(ICON_FA_PEN, { 2*size_font, -1.0f })) {
    Toolbar::draw_line = Menu::draw_line = true;
    mode = (mode == Mode::NORMAL) ? Mode::DRAWING : Mode::NORMAL;
  }
  ImGui::EndDisabled();

  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("Draw line");
  ImGui::SameLine(0, 1); // offset=0: pos. right after previous item, spacing=1px

  // change color for toolbar button if in right mode
  ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[Menu::brush_circle ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
  if (ImGui::Button(ICON_FA_PAINT_BRUSH, { 2*size_font, -1.0f })) {
    Toolbar::brush_circle = Menu::brush_circle = !Menu::brush_circle;
    mode = (mode == Mode::NORMAL) ? Mode::DRAWING : Mode::NORMAL;
  }
  ImGui::PopStyleColor();

  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("Circle brush tool");
  ImGui::SameLine(0, 1); // offset=0: pos. right after previous item, spacing=1px

  // change color for toolbar button if in right mode
  ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[Menu::brush_line ? ImGuiCol_ButtonActive : ImGuiCol_Button]);
  if (ImGui::Button(ICON_FA_PAINT_ROLLER, { 2*size_font, -1.0f })) {
    Toolbar::brush_line = Menu::brush_line = !Menu::brush_line;
    mode = (mode == Mode::NORMAL) ? Mode::DRAWING : Mode::NORMAL;
  }
  ImGui::PopStyleColor();

  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("Line brush tool");
  ImGui::SameLine(0, 1); // offset=0: pos. right after previous item, spacing=1px

  // radio buttons for what to show on image hover (imgui_demo.cpp:560)
  // compile-time casting between pointer types works with reinterpret_cast (not with static_cast)
  ImGui::SetCursorPos({ ImGui::GetCursorPosX(), size_font/2.0f - 3.0f });
  ImGui::RadioButton("None", &Toolbar::hover_mode, HoverMode::NONE);
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Show nothing on hover");
  ImGui::SameLine();

  ImGui::RadioButton("Image subset", &Toolbar::hover_mode, HoverMode::IMAGE_SUBSET);
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Show image subset on hover");
  ImGui::SameLine();

  ImGui::RadioButton("Pixel value", &Toolbar::hover_mode, HoverMode::PIXEL_VALUE);
  if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Show hovered pixel value");

  // right alignment: https://github.com/ocornut/imgui/issues/934#issuecomment-340231002
  const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
  static float widthStrokeColor = 100.0f;
  ImGui::SameLine(ImGui::GetWindowWidth() - widthStrokeColor - itemSpacing); // offset <> 0: pos. relative to window-left corner
  ImGui::ColorEdit3("ColorStroke", (float*) &Color::stroke, ImGuiColorEditFlags_NoInputs);
  widthStrokeColor = ImGui::GetItemRectSize().x;

  // right alignment: https://github.com/ocornut/imgui/issues/934#issuecomment-340231002
  static float widthFillColor = 100.0f;
  ImGui::SameLine(ImGui::GetWindowWidth() - widthStrokeColor - widthFillColor - 2*itemSpacing); // offset <> 0: pos. relative to window-left corner
  ImGui::ColorEdit3("ColorFill", (float*) &Color::fill, ImGuiColorEditFlags_NoInputs);
  widthFillColor = ImGui::GetItemRectSize().x;

  ImGui::End();
}
