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
  if (m_menu.open_image || m_toolbar.open_image) {
    std::cout << "Open image menu item enabled!" << '\n';

    // https://github.com/aiekick/ImGuiFileDialog#simple-dialog-
    // open image dialog
    ImGuiFileDialog::Instance()->OpenModal("OpenImageKey", "Open image", "Image files{.jpg,.png}", "./assets/images", "");
    m_menu.open_image = false;
    m_toolbar.open_image = false;
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
  if (m_menu.save_image || m_toolbar.save_image) {
    // open image dialog
    ImGuiFileDialog::Instance()->OpenModal("SaveImageKey", "Save image", "Image files{.jpg,.png}", "./assets/images", "");
    m_menu.save_image = false;
    m_toolbar.save_image = false;
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
  if (m_menu.to_grayscale) {
    m_canvas.to_grayscale();
    m_menu.to_grayscale = false;
  }

  // update to shader to show image in color
  if (m_menu.view_color) {
    m_canvas.set_shader("color");
    m_menu.view_color = false;
  }

  // update shader to show image in grayscale
  if (m_menu.view_grayscale) {
    m_canvas.set_shader("grayscale");
    m_menu.view_grayscale = false;
  }

  // update to shader to show monochrome (1-channel) image
  if (m_menu.view_monochrome) {
    m_canvas.set_shader("monochrome");
    m_menu.view_monochrome = false;
  }

  // zoom in/out
  if (m_menu.zoom_in || m_toolbar.zoom_in) {
    m_canvas.zoom *= 2;
    m_menu.zoom_in = false;
    m_toolbar.zoom_in = false;
  }
  if (m_menu.zoom_out || m_toolbar.zoom_out) {
    m_canvas.zoom /= 2;
    m_menu.zoom_out = false;
    m_toolbar.zoom_out = false;
  }

  if (m_menu.quit_app || m_toolbar.quit_app) {
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
