#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "ui/canvas.hpp"
#include "processing/image_utils.hpp"
#include "shader_exception.hpp"

/* Static class members require a declaration in *.cpp (to allocate space for them) or be declared as `inline` in *.hpp */
std::array<GLuint, 2> Canvas::callback_data;

/* Canvas showing image */
Canvas::Canvas():
  m_image("./assets/images/checkerboard.png", false),
  m_texture(m_image, GL_TEXTURE0, Wrapping::BLACK),

  m_programs{
    {"color", Program("assets/shaders/basic.vert", "assets/shaders/color.frag")},
    {"grayscale", Program("assets/shaders/basic.vert", "assets/shaders/grayscale.frag")},
    {"monochrome", Program("assets/shaders/basic.vert", "assets/shaders/monochrome.frag")},
  },

  // `unordered_map::operator[]()` requires map's value (i.e. Program) to have default constructor
  m_program(&m_programs.at("color")),
  m_zoom(1.0f),
  m_tooltip_image(m_texture)
{
  // vertex or fragment shaders failed to compile
  if (m_program->has_failed()) {
    throw ShaderException();
  }
}

/**
 * Change to custom shader (e.g. to show 1-channel image in grayscale)
 * Otherwise image will appear in shades of red by default
 * Shader & texture retrieved from static class member as pointer passed as 2nd arg to `ImDrawList->AddCallback()` allocated on stack (=>segfault)
 * => can only send one pointer
 * see: https://github.com/ocornut/imgui/issues/4174
 * @param draw_list Low-level list of polygons
 */
void Canvas::draw_with_custom_shader(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
  // construct program/texture from their ids in static class member
  Program program(callback_data[0]);
  Texture2D texture(callback_data[1]);

  // get current viewport size incl. menu (`GetDrawData()`: what to render)
  ImDrawData* draw_data = ImGui::GetDrawData();
  ImVec2 size_viewport = draw_data->DisplaySize;

  // parameters to glm::ortho corresp. to screen bounds & position coords above are rel. to these bounds
  // ortho mat inspired by: https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_opengl2.cpp#L146
  // originally viewport & image origins at lower-left corner, but bcos of ortho mat image will be inverted
  // this ortho mat (with inv. bottom & top) allows image to stick to top-left corner of window
  glm::mat4 projection2d = glm::ortho(0.0f, size_viewport.x, size_viewport.y, 0.0f); // left, right, bottom, top
  glm::mat4 view(1.0f);
  glm::mat4 model(1.0f); // position at window's origin (upper-left corner)
  glm::mat4 transformation = projection2d * view * model;

  // pass uniforms to shaders
  Uniforms uniforms = {
    {"transformation", transformation},
    {"texture2d", texture}
  };
  program.use();
  program.set_uniforms(uniforms);
}

/**
 * Render image inside ImGui window
 * @param y_offset heights of menu & toolbar used to offset image canvas
 */
void Canvas::render(float y_offset) {
  // draw rect in background
  // main window's content size
  ImGuiIO& io = ImGui::GetIO(); // configures imgui
  ImVec2 size_display = io.DisplaySize;
  ImVec2 m_size_content = { size_display.x, size_display.y - y_offset };
  // ImGui::GetForegroundDrawList()->AddRect({0, 0}, size_display, 0xFF0000FF);

  // imgui window of specified size, anchored at (0, 0), & without padding
  // origin at upper-left corner
  ImGui::SetNextWindowPos({ 0.0f, y_offset });
  ImGui::SetNextWindowSize(m_size_content);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  bool p_open;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
  ImGui::Begin("Dialog title", &p_open, window_flags);

  // render image using custom shader (for grayscale) on drawlist associated with current frame
  render_image(y_offset);

  // render imgui window
  ImGui::PopStyleVar(); // avoids assertion error (associated with padding above)
  ImGui::End();
}

/* Use custom shader (e.g. for grayscale) to render image on drawlist associated with current frame */
void Canvas::use_shader() {
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  callback_data = { m_program->id, m_texture.id };
  draw_list->AddCallback(draw_with_custom_shader, nullptr);
}

/* Reset to default ImGui shader */
void Canvas::unuse_shader() {
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
}

/**
 * Change to shader identified by its key
 * @param name Shader key in `m_programs`
 */
void Canvas::set_shader(const std::string& key) {
    m_program = &m_programs.at(key);
}

/**
 * Show image from texture using custom shader
 * Change to custom shader before rendering image
 * @param y_offset heights of menu & toolbar used to calculate cursor position rel. to image
 */
void Canvas::render_image(float y_offset) {
  // render image using custom shader
  use_shader();

  // double casting avoids `warning: cast to pointer from integer of different size` i.e. smaller
  m_texture.attach();
  ImVec2 size_image = ImVec2(m_zoom * m_texture.width, m_zoom * m_texture.height);
  ImGui::Image((void*)(intptr_t) m_texture.id, size_image);

  // show tooltip containing zoomed subset image (source: imgui_demo.cpp:986)
  if (ImGui::IsItemHovered()) {
    m_tooltip_image.render(y_offset, m_zoom);
  }

  unuse_shader();

  /// Color picker
  // initial color set only at beginning
  static float color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
  ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar;
  ImGui::ColorEdit4("My color", color, ImGuiColorEditFlags_AlphaBar);
  ///
}

/* Change image opened in canvas to given `path_image` */
void Canvas::change_image(const std::string& path_image) {
  m_image.free();
  m_image = Image(path_image, false);
  m_texture.set_image(m_image);
}

/* Save image opened in canvas to given `path_image` */
void Canvas::save_image(const std::string& path_image) {
  m_image.save(path_image);
}

void Canvas::to_grayscale() {
  m_image = ImageUtils::to_grayscale(m_image);
  m_texture.set_image(m_image);
  m_program = &m_programs.at("monochrome");
}

/* Free opengl texture (image holder) & shaders programs used to display it */
void Canvas::free() {
  m_texture.free();
  for (auto& pair: m_programs) {
    pair.second.free();
  }
}

void Canvas::zoom_in() {
  m_zoom *= 2.0f;
}

void Canvas::zoom_out() {
  m_zoom /= 2.0f;
}
