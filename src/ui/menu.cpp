#include "ui/menu.hpp"

Menu::Menu():
  open_image(false), save_image(false), quit_app(false), // menu File
  to_grayscale(false), // menu Edit
  view_color(false), view_grayscale(false), view_monochrome(false), // menu View
  zoom_in(false), zoom_out(false) // menu Zoom
{
}

/**
 * Menu items act like toggle buttons in imgui
 * boolean flag set/unset on every click
 * Sets the size of menu drawn to place image canvas below it
 */
void Menu::render() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      ImGui::MenuItem("Open", NULL, &open_image);
      ImGui::MenuItem("Save", NULL, &save_image);
      ImGui::MenuItem("Quit", NULL, &quit_app);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
      ImGui::MenuItem("To grayscale", NULL, &to_grayscale);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Color", NULL, &view_color);
      ImGui::MenuItem("Grayscale", NULL, &view_grayscale);
      ImGui::MenuItem("Monochrome", NULL, &view_monochrome);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Zoom")) {
      ImGui::MenuItem("Zoom in", NULL, &zoom_in);
      ImGui::MenuItem("Zoom out", NULL, &zoom_out);
      ImGui::EndMenu();
    }

    size = ImGui::GetWindowSize();
    ImGui::EndMainMenuBar();
  }
}
