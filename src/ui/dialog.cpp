#include <iostream>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "ui/dialog.hpp"
#include "fonts/fonts.hpp"

/**
 * Dialog made with imgui
 * Inspired by: https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp
 */
Dialog::Dialog(const Window& window):
  m_window(window),
  m_canvas(),
  m_menu(),
  m_toolbar()
{
  // setup imgui context & glfw/opengl backends
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(m_window.w, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  // load text & icon fonts
  Fonts::load();
}

/* Render dialog in main loop */
void Dialog::render() {
  // start imgui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // top main menu with its listeners
  m_menu.render();
  on_menu_click();

  // toolbar
  m_toolbar.render(m_menu.size.y);

  // image
  float y_offset = m_menu.size.y + m_toolbar.size.y;
  m_canvas.render(y_offset);

  // show metrics window (for loaded fonts & glyphs)
  // ImGui::ShowMetricsWindow();

  // show demo window (for imgui functionalities)
  ImGui::ShowDemoWindow();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/* Listeners for click on menu items */
void Dialog::on_menu_click() {
  // menu buttons listeners
  if (Menu::open_image || Toolbar::open_image) {
    std::cout << "Open image menu item enabled!" << '\n';

    // https://github.com/aiekick/ImGuiFileDialog#simple-dialog-
    // open image dialog
    ImGuiFileDialog::Instance()->OpenModal("OpenImageKey", "Open image", "Image files{.jpg,.png}", "./assets/images", "");
    Menu::open_image = false;
    Toolbar::open_image = false;
  }

  // display open image file dialog
  if (ImGuiFileDialog::Instance()->Display("OpenImageKey", ImGuiWindowFlags_None, ImVec2(600, 300), ImVec2(600, 300))) {
    // get file path if ok
    if (ImGuiFileDialog::Instance()->IsOk()) {
      // free previously opened image & open new one
      std::string path_image = ImGuiFileDialog::Instance()->GetFilePathName();
      m_canvas.change_image(path_image);
      std::cout << "Path of opened image: " << path_image << '\n';
    }

    // close file dialog
    ImGuiFileDialog::Instance()->Close();
  }

  // save edited image
  if (Menu::save_image || Toolbar::save_image) {
    // open image dialog
    ImGuiFileDialog::Instance()->OpenModal("SaveImageKey", "Save image", "Image files{.jpg,.png}", "./assets/images", "");
    Menu::save_image = false;
    Toolbar::save_image = false;
  }

  // display save image file dialog
  if (ImGuiFileDialog::Instance()->Display("SaveImageKey", ImGuiWindowFlags_None, ImVec2(600, 300), ImVec2(600, 300))) {
    // get file path if ok
    if (ImGuiFileDialog::Instance()->IsOk()) {
      // free previously opened image & open new one
      std::string path_image = ImGuiFileDialog::Instance()->GetFilePathName();
      m_canvas.save_image(path_image);
      std::cout << "Path for saved image: " << path_image << '\n';
    }

    // close file dialog
    ImGuiFileDialog::Instance()->Close();
  }

  // convert opened image to grayscale & update shader to show monochrome image
  if (Menu::to_grayscale) {
    m_canvas.to_grayscale();
    Menu::to_grayscale = false;
  }

  // update to shader to show image in color
  if (Menu::view_color) {
    m_canvas.set_shader("color");
    Menu::view_color = false;
  }

  // update shader to show image in grayscale
  if (Menu::view_grayscale) {
    m_canvas.set_shader("grayscale");
    Menu::view_grayscale = false;
  }

  // update to shader to show monochrome (1-channel) image
  if (Menu::view_monochrome) {
    m_canvas.set_shader("monochrome");
    Menu::view_monochrome = false;
  }

  // zoom in/out
  if (Menu::zoom_in || Toolbar::zoom_in) {
    m_canvas.zoom_in();
    Menu::zoom_in = false;
    Toolbar::zoom_in = false;
  }
  if (Menu::zoom_out || Toolbar::zoom_out) {
    m_canvas.zoom_out();
    Menu::zoom_out = false;
    Toolbar::zoom_out = false;
  }

  if (Menu::quit_app || Toolbar::quit_app) {
    m_window.close();
  }
}

/* Destroy canvas & imgui */
void Dialog::free() {
  m_canvas.free();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
