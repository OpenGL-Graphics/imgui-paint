#include <iostream>

#include "IconsFontAwesome5.h"
#include "ui/toolbar.hpp"

Toolbar::Toolbar():
  open_image(false),
  save_image(false),
  quit_app(false)
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
    open_image = true;
  }
  ImGui::SameLine(2*size_font + 1); // relative to window left corner

  if (ImGui::Button(ICON_FA_SAVE, { 2*size_font, -1.0f })) {
    save_image = true;
  }
  ImGui::SameLine(2 * (2*size_font + 1)); // relative to window left corner

  if (ImGui::Button(ICON_FA_WINDOW_CLOSE, { 2*size_font, -1.0f })) {
    quit_app = true;
  }

  // avoids applying same style to subsequent windows
  ImGui::PopStyleColor();
  ImGui::PopStyleVar();
  ImGui::End();
}
