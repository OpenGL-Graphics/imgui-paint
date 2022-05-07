#include <iostream>

#include "glad/glad.h"
#include "ui/frame.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "render/renderer.hpp"
#include "framebuffer_exception.hpp"
#include "shader_exception.hpp"

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
  std::string path_image = "./assets/images/nature.jpg";
  Frame frame(window, path_image);

  ///
    // inspired by: https://gamedev.stackexchange.com/questions/150214
    // image to render in grayscale on framebuffer
    Texture2D texture_image(Image(path_image, false));

    // surface geometry to render to
    Program program("assets/shaders/fbo.vert", "assets/shaders/grayscale.frag");
    if (program.has_failed()) {
      window.destroy();
      throw ShaderException();
    }

    // vertexes in NDC space (source: https://learnopengl.com/Advanced-OpenGL/Framebuffers)
    // lower-left corner: (-1, -1), upper-right corner: (1, 1), and center of screen: (0, 0)
    std::vector<float> vertexes = {
    // xy     uv
      -1, -1, 0, 0,
       1, -1, 1, 0,
       1,  1, 1, 1,
      -1,  1, 0, 1,
    };
    std::vector<unsigned int> indices = {
      0, 1, 2, // triangle 1
      2, 3, 0, // triangle 2
    };
    Geometry geometry(vertexes, indices, {});

    Renderer surface(program, VBO(geometry), {
      {0, "position", 2, 4, 0},
      {1, "texture_coord", 2, 4, 2}
    });

    // rendering to texture via fbo (with frag. shader), so displayed image (modified by frag. shaders) can be saved to disk later
    // empty texture to fill when drawing to framebuffer
    Image image_framebuffer(window.width, window.height, GL_RGB, NULL);
    Texture2D texture_framebuffer(image_framebuffer);

    // framebuffers with empty texture attached to it (to render scene to texture)
    Framebuffer framebuffer;
    framebuffer.attach_texture(texture_framebuffer);

    if (!framebuffer.is_complete()) {
      window.destroy();
      throw FramebufferException();
    }
  ///

  // main loop
  while (!window.is_closed()) {
    // clear color & depth & stencil buffers before rendering every frame
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ///
    // render image to framebuffer texture
    {
      // clear framebuffer's attached color buffer in every frame
      framebuffer.bind();
      framebuffer.clear({ 1.0f, 1.0f, 1.0f, 1.0f });

      // draw 2d health bar HUD surface (scaling then translation with origin at lower left corner)
      surface.draw({ {"texture2d", texture_image} });
      framebuffer.unbind();
    }

    Image image = texture_framebuffer.get_image();
    image.save("image.png");
    image.free();
    ///

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

  // destroy framebuffers
  framebuffer.free();

  // shaders
  program.free();

  // buffers
  surface.free();

  // textures
  texture_framebuffer.free();
  texture_image.free();

  // destroy imgui
  frame.free();

  // destroy window & terminate glfw
  window.destroy();

  return 0;
}
