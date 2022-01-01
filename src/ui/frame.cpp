#include <iostream>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "ui/frame.hpp"
#include "fonts/fonts.hpp"

/**
 * Window frame made with imgui
 * Inspired by: https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp
 */
Frame::Frame(const Window& window):
  m_window(window),

  m_canvas(),
  m_menu(),
  m_toolbar(),

  m_listener_canvas(&m_canvas),
  m_listener_window(&m_window)
{
  // setup imgui context & glfw/opengl backends
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(m_window.w, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  // load text & icon fonts
  Fonts::load();
}

/* Render dialog in main loop */
void Frame::render() {
  // start imgui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // top main menu, toolbar, and image canvas
  m_menu.render();
  m_toolbar.render();
  m_canvas.render();

  // event listeners rel. to canvas & window
  m_listener_canvas.handle_all();
  m_listener_window.handle_all();

  // show metrics window (for loaded fonts & glyphs)
  // ImGui::ShowMetricsWindow();

  // show demo window (for imgui functionalities)
  ImGui::ShowDemoWindow();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/* Destroy canvas & imgui */
void Frame::free() {
  m_canvas.free();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
