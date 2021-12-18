#include <iostream>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "ui/dialog.hpp"

/**
 * Dialog made with imgui
 * Inspired by: https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp
 */
Dialog::Dialog(const Window& window):
  m_window(window),
  m_canvas()
{
  // setup imgui context & glfw/opengl backends
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(m_window.w, true);
  ImGui_ImplOpenGL3_Init("#version 130");
}

/* Render dialog in main loop */
void Dialog::render() {
  // start imgui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // top main menu
  render_menu();

  // image
  m_canvas.render(m_size_menu);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 * Render menu bar with its items & attach listeners to its items
 * Sets the size of menu drawn to place content windows below it
 */
void Dialog::render_menu() {
  // static vars only init on 1st function call
  static bool open_image = false;
  static bool save_image = false;
  static bool quit_app = false;
  static bool to_grayscale = false;
  static bool view_color = false;
  static bool view_grayscale = false;
  static bool view_monochrome = false;

  // menu buttons listeners
  if (open_image) {
    std::cout << "Open image menu item enabled!" << '\n';

    // https://github.com/aiekick/ImGuiFileDialog#simple-dialog-
    // open image dialog
    ImGuiFileDialog::Instance()->OpenModal("OpenImageKey", "Open image", "Image files{.jpg,.png}", "./assets/images", "");
    open_image = false;
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
  if (save_image) {
    // open image dialog
    ImGuiFileDialog::Instance()->OpenModal("SaveImageKey", "Save image", "Image files{.jpg,.png}", "./assets/images", "");
    save_image = false;
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
  if (to_grayscale) {
    m_canvas.to_grayscale();
    to_grayscale = false;
  }

  // update to shader to show image in color
  if (view_color) {
    m_canvas.set_shader("color");
    view_color = false;
  }

  // update shader to show image in grayscale
  if (view_grayscale) {
    m_canvas.set_shader("grayscale");
    view_grayscale = false;
  }

  // update to shader to show monochrome (1-channel) image
  if (view_monochrome) {
    m_canvas.set_shader("monochrome");
    view_monochrome = false;
  }

  if (quit_app) {
    m_window.close();
  }

  // menu items act like toggle buttons in imgui
  // bool var set/unset on every click
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

    m_size_menu = ImGui::GetWindowSize();
    ImGui::EndMainMenuBar();
  }
}

/* Destroy canvas & imgui */
void Dialog::free() {
  m_canvas.free();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
