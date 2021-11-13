#include <iostream>
#include "glad/glad.h"

#include "gui/dialog.hpp"

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
    return 1;
  } else {
    std::cout << "Opengl version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
  }

  // initialize dialog with imgui
  Dialog dialog(window);

  // main loop
  while (!window.is_closed()) {
    // clear color & depth & stencil buffers before rendering every frame
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // render imgui dialog
    dialog.render();

    // process events & show rendered buffer
    window.process_events();
    window.render();
  }

  // destroy imgui
  dialog.free();

  // destroy window & terminate glfw
  window.destroy();

  return 0;
}
