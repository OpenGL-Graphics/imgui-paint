#include <iostream>

#include "glad/glad.h"
#include "ui/frame.hpp"

int main() {
  // glfw window
  Window window("Image visualizer");

  if (window.is_null()) {
    std::cout << "Failed to create window or OpenGL context" << "\n";
    return 1;
  }

  // initialize glad before calling gl functions
  window.make_context();
  if (!gladLoadGL()) {
    std::cout << "Failed to load Glad (OpenGL)" << "\n";
    window.destroy();
    return 1;
  } else {
    std::cout << "Opengl version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
  }

  // initialize window frame with imgui
  std::string path_image = "./assets/images/rap.jpg";
  // std::string path_image = "./assets/images/samurai.jpg";
  // std::string path_image = "./assets/images/nature.jpg";
  // std::string path_image = "./assets/images/nature.png";
  Frame frame(window, path_image);

  // main loop
  while (!window.is_closed()) {
    // clear color & depth & stencil buffers before rendering every frame
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // render imgui frame
    frame.render();

    // process events & show rendered buffer
    window.process_events();
    window.render();

    /*
    float fps = ImGui::GetIO().Framerate;
    std::cout << "fps: " << fps << '\n';
    */
  }

  // destroy imgui
  frame.free();

  // destroy window & terminate glfw
  window.destroy();

  return 0;
}
