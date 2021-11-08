#ifndef DIALOG_HPP
#define DIALOG_HPP

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>

#include "window.hpp"

class Dialog {
public:
  // Dialog(GLFWwindow* window, const std::string& title, const std::string& text);
  Dialog(const Window& window, const std::string& title, const std::string& text);
  void render();
  void free();

private:
  std::string m_title;
  std::string m_text;
};

#endif // DIALOG_HPP
