#include "ui/menu.hpp"

/* static members definition (avoids linking error) & initialization */
// menu File
bool Menu::open_image = false;
bool Menu::save_image = false;
bool Menu::quit_app = false;

// menu Edit
bool Menu::to_grayscale = false;
bool Menu::blur = false;

// menu View
bool Menu::view_color = false;
bool Menu::view_grayscale = false;
bool Menu::view_monochrome = false;

// menu Zoom
bool Menu::zoom_in = false;
bool Menu::zoom_out = false;

Menu::Menu()
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
      ImGui::MenuItem("Open", NULL, &Menu::open_image);
      ImGui::MenuItem("Save", NULL, &Menu::save_image);
      ImGui::MenuItem("Quit", NULL, &Menu::quit_app);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
      ImGui::MenuItem("To grayscale", NULL, &Menu::to_grayscale);
      ImGui::MenuItem("Blur", NULL, &Menu::blur);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Color", NULL, &Menu::view_color);
      ImGui::MenuItem("Grayscale", NULL, &Menu::view_grayscale);
      ImGui::MenuItem("Monochrome", NULL, &Menu::view_monochrome);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Zoom")) {
      ImGui::MenuItem("Zoom in", NULL, &Menu::zoom_in);
      ImGui::MenuItem("Zoom out", NULL, &Menu::zoom_out);
      ImGui::EndMenu();
    }

    size = ImGui::GetWindowSize();
    ImGui::EndMainMenuBar();
  }
}
