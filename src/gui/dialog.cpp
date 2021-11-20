#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "gui/dialog.hpp"

/**
 * Dialog made with imgui
 * Inspired by: https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp
 */
Dialog::Dialog(const Window& window):
  m_window(window),
  m_texture(Image("image.jpg"))
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

  // imgui window with widgets
  ImGui::Begin("Dialog title");
  render_components();
  ImGui::End();

  // render imgui window
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 * Render gui components (e.g. buttons)
 * Called in main loop
 */
void Dialog::render_components() {
  // static vars only init on 1st function call
  static bool open_image = false;
  static bool quit_app = false;

  // menu buttons listeners
  if (open_image) {
    std::cout << "Menu item enabled!" << '\n';
    open_image = false;
  }
  if (quit_app) {
    m_window.close();
  }

  // buttons return true when clicked
  /*
  if (ImGui::Button("Quit")) {
    m_window.close();
  }

  ImGui::SameLine();
  ImGui::Text("<= Click this button to quit");
  */
  bool show_demo_window = true;
  ImGui::ShowDemoWindow(&show_demo_window);

  // show image from texture
  // double casting avoids `warning: cast to pointer from integer of different size` i.e. smaller
  m_texture.attach();
  ImGui::Image((void*)(intptr_t) m_texture.id, ImVec2(m_texture.width, m_texture.height));

  // menu items act like toggle buttons in imgui
  // bool var set/unset on every click
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Menu")) {
      ImGui::MenuItem("Open image", NULL, &open_image);
      ImGui::MenuItem("Quit", NULL, &quit_app);
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  // plot values
  // float values[] = {0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f};
  // ImGui::PlotLines("Plot", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, ImVec2(0, 200));
}

/* Destroy imgui */
void Dialog::free() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // free opengl texture (image holder)
  m_texture.free();
}
