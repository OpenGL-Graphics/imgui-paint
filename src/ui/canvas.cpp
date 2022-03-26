#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>

#include "glad/glad.h"

#include "ui/canvas.hpp"
#include "ui/toolbar.hpp"
#include "ui/menu.hpp"
#include "ui/imgui_utils.hpp"
#include "ui/enumerations/hover_mode.hpp"

#include "ui/globals/size.hpp"
#include "ui/globals/color.hpp"

#include "image/image_utils.hpp"

#include "shader_exception.hpp"
#include "framebuffer_exception.hpp"
#include "profiling/profiler.hpp"

/* Static class members require a declaration in *.cpp (to allocate space for them) or be declared as `inline` in *.hpp */
std::array<GLuint, 2> Canvas::callback_data;

/**
 * Canvas showing image
 * @param path_image Path to image to load initially
 * TODO: remove `path_image` to start with an empty canvas
 */
Canvas::Canvas(const std::string path_image):
  m_texture(Image(path_image, false)),
  m_image_vg(m_texture),

  m_programs{
    {"color", Program("assets/shaders/basic.vert", "assets/shaders/color.frag")},
    {"grayscale", Program("assets/shaders/basic.vert", "assets/shaders/grayscale.frag")},
    {"monochrome", Program("assets/shaders/basic.vert", "assets/shaders/monochrome.frag")},
  },

  // `unordered_map::operator[]()` requires map's value (i.e. Program) to have default constructor
  m_program(&m_programs.at("color")),
  m_zoom(1.0f),
  m_tooltip_image(m_texture),
  m_tooltip_pixel(m_image_vg.framebuffer),

  // line's starting point not set yet
  m_cursor(VECTOR_UNSET)
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
 */
void Canvas::render() {
  // draw rect in background
  // main window's content size
  ImGuiIO& io = ImGui::GetIO(); // configures imgui
  ImVec2 size_display = io.DisplaySize;
  float y_offset = Size::menu.y + Size::toolbar.y;
  Size::canvas = { size_display.x, size_display.y - y_offset };

  // viewport of same size as texture
  // `nvgBeginFrame()` defines size of drawing area in relation to `glViewport()`
  glViewport(0, 0, m_texture.width, m_texture.height);

  // imgui window of specified size, anchored at (0, 0), & without padding, no dark background overlay
  // origin at upper-left corner
  ImGui::SetNextWindowPos({ 0.0f, y_offset });
  ImGui::SetNextWindowSize(Size::canvas);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // otherwise cursor coords rel. to image org starts at 1 (not 0)
  bool p_open;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;
  ImGui::Begin("Canvas", &p_open, window_flags);

  // render image using custom shader (for grayscale) on drawlist associated with current frame
  render_image(y_offset);

  // render imgui window
  ImGui::PopStyleVar(2); // cancel no-padding & no-border (i.e. arg=2 styles)
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
  // Error code = 502 (GL_INVALID_OPERATION) from imgui in frame.cpp after create frame
  // render image using custom shader
  // use_shader();

  // render image & graphics drawn on texture attached to fbo
  // double casting avoids `warning: cast to pointer from integer of different size` i.e. smaller
  m_texture.attach();
  ImVec2 size_image = ImVec2(m_zoom * m_texture.width, m_zoom * m_texture.height);
  ImGui::Image((void*)(intptr_t) m_texture.id, size_image);

  // draw circle/line at mouse click position
  if (ImGui::IsItemClicked()) {
    if (Toolbar::draw_circle) {
      ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position_vg(m_texture.height, y_offset);
      m_image_vg.draw_circle(position_mouse_img.x, position_mouse_img.y);
      Menu::draw_circle = false;
      Toolbar::draw_circle = false;

      ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }

    // two clicks needed to draw a line (cursor saved on 1st click & reset after 2nd)
    if (Toolbar::draw_line) {
      if (m_cursor.x == VECTOR_UNSET.x && m_cursor.y == VECTOR_UNSET.y) {
        move_cursor();
      } else {
        ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position_vg(m_texture.height, y_offset);
        m_image_vg.draw_line(m_cursor.x, m_cursor.y, position_mouse_img.x, position_mouse_img.y);

        m_cursor = VECTOR_UNSET;
        Menu::draw_line = false;
        Toolbar::draw_line = false;

        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
      }
    }
  }

  // brush tool paints while LMB dragged
  // https://github.com/ocornut/imgui/issues/493
  if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
    if (Toolbar::brush) {
      ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position_vg(m_texture.height, y_offset);
      m_image_vg.draw_circle(position_mouse_img.x, position_mouse_img.y);
    }
  }

  if (ImGui::IsItemHovered()) {
    // show tooltip containing zoomed subset image (source: imgui_demo.cpp:986) or pixel value accord. to toolbar radio button
    if (Toolbar::hover_mode == HoverMode::IMAGE_SUBSET) {
      m_tooltip_image.render(y_offset, m_zoom);
    } else if (Toolbar::hover_mode == HoverMode::PIXEL_VALUE) {
      m_tooltip_pixel.render(y_offset);
    }

    // change to hand cursor if hovering in drawing mode
    if (Toolbar::draw_circle || Toolbar::draw_line || Toolbar::brush)
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }

  // unuse_shader();
}

/* Define line's start point */
void Canvas::move_cursor() {
  // float y_offset = Size::menu.y + Size::toolbar.y;
  // ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position({ 0.0f, y_offset });
  float y_offset = Size::menu.y + Size::toolbar.y;
  ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position_vg(m_texture.height, y_offset);
  std::cout << "Line start point x: " << position_mouse_img.x << " y: " << position_mouse_img.y << '\n';

  m_cursor = position_mouse_img;
}

/* Change image opened in canvas to given `path_image` */
void Canvas::change_image(const std::string& path_image) {
  // replace image texture
  Image image_new = Image(path_image, false);
  m_texture.set_image(image_new);
}

/* Save image opened in canvas to given `path_image` */
void Canvas::save_image(const std::string& path_image) {
  // modified image retrieved from opengl texture & free original one
  m_texture.get_image();
  Image image = m_texture.image;

  // save image on disk
  image.save(path_image);
}

/* Convert image to grayscale and switch shader to monochrome */
void Canvas::to_grayscale() {
  Image image_in = m_texture.image;
  Image image_out = ImageUtils::to_grayscale(image_in);
  m_texture.set_image(image_out);
  m_program = &m_programs.at("monochrome");
}

/* Blur image using a 9x9 avg. filter */
void Canvas::blur() {
  Image image_in = m_texture.image;
  Image image_out = ImageUtils::blur(image_in);
  m_texture.set_image(image_out);
}

/* Free opengl texture (image holder) & shaders programs used to display it */
void Canvas::free() {
  for (auto& pair: m_programs) {
    pair.second.free();
  }

  // delete image & opengl texure
  m_texture.free();

  // destroy framebuffer & nanovg context
  m_image_vg.free();
}

void Canvas::zoom_in() {
  m_zoom *= 2.0f;
}

void Canvas::zoom_out() {
  m_zoom /= 2.0f;
}
