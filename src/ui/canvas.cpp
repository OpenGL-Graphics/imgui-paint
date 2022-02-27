#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "ui/canvas.hpp"
#include "ui/toolbar.hpp"
#include "ui/menu.hpp"
#include "ui/imgui_utils.hpp"
#include "ui/constants/size.hpp"
#include "ui/enumerations/hover_mode.hpp"

#include "image/image_utils.hpp"

#include "shader_exception.hpp"

/* Static class members require a declaration in *.cpp (to allocate space for them) or be declared as `inline` in *.hpp */
std::array<GLuint, 2> Canvas::callback_data;

/**
 * Canvas showing image
 * @param path_image Path to image to load initially
 * TODO: remove `path_image` to start with an empty canvas
 */
Canvas::Canvas(const std::string path_image):
  m_image_vector(path_image),
  m_image(path_image, false),
  m_texture(m_image, GL_TEXTURE0, Wrapping::BLACK),

  m_programs{
    {"color", Program("assets/shaders/basic.vert", "assets/shaders/color.frag")},
    {"grayscale", Program("assets/shaders/basic.vert", "assets/shaders/grayscale.frag")},
    {"monochrome", Program("assets/shaders/basic.vert", "assets/shaders/monochrome.frag")},
  },

  // `unordered_map::operator[]()` requires map's value (i.e. Program) to have default constructor
  m_program(&m_programs.at("color")),
  m_zoom(1.0f),
  m_tooltip_image(m_texture),
  m_tooltip_pixel(m_image),

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

  // imgui window of specified size, anchored at (0, 0), & without padding
  // origin at upper-left corner
  ImGui::SetNextWindowPos({ 0.0f, y_offset });
  ImGui::SetNextWindowSize(Size::canvas);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // otherwise cursor coords rel. to image org starts at 1 (not 0)
  bool p_open;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
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
  // render image using custom shader
  use_shader();

  // double casting avoids `warning: cast to pointer from integer of different size` i.e. smaller
  m_texture.attach();
  ImVec2 size_image = ImVec2(m_zoom * m_texture.width, m_zoom * m_texture.height);
  ImGui::Image((void*)(intptr_t) m_texture.id, size_image);

  // draw circle/line at mouse click position with Cairo (managed by listener)
  if (ImGui::IsItemClicked()) {
    if (Menu::draw_circle || Toolbar::draw_circle) {
      draw("circle");
      Menu::draw_circle = false;
      Toolbar::draw_circle = false;

      ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }

    // two clicks needed to draw a line (cursor saved on 1st click & reset after 2nd)
    if (Menu::draw_line || Toolbar::draw_line) {
      if (m_cursor.x == VECTOR_UNSET.x && m_cursor.y == VECTOR_UNSET.y) {
        move_cursor();
      } else {
        draw("line");

        m_cursor = VECTOR_UNSET;
        Menu::draw_line = false;
        Toolbar::draw_line = false;

        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
      }
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
    if (Menu::draw_circle || Toolbar::draw_circle || Menu::draw_line || Toolbar::draw_line)
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }

  unuse_shader();
}

/* Define line's start point */
void Canvas::move_cursor() {
  float y_offset = Size::menu.y + Size::toolbar.y;
  ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position({ 0.0f, y_offset });
  std::cout << "Line start point x: " << position_mouse_img.x << " y: " << position_mouse_img.y << '\n';

  m_cursor = position_mouse_img;
}

/**
 * Same function used to draw with Cairo a:
 * - circle at mouse cursor click position
 * - line starting from `m_cursor` (defined on 1st click in `move_cursor()`) to mouse 2nd click position
 * @param type_shape 'circle' or 'line'
 */
void Canvas::draw(const std::string& type_shape) {
  float y_offset = Size::menu.y + Size::toolbar.y;
  ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position({ 0.0f, y_offset });
  std::cout << "Line end point x: " << position_mouse_img.x << " y: " << position_mouse_img.y << '\n';

  // draw line/circle with Cairo (instead of OpenCV => anti-aliased edges by default with vectors)
  if (type_shape == "circle") {
    m_image_vector.draw_circle(position_mouse_img.x, position_mouse_img.y);
  } else if (type_shape == "line") {
    m_image_vector.draw_line(m_cursor.x, m_cursor.y, position_mouse_img.x, position_mouse_img.y);
  }

  // free previous image & set it from converted cairo surface
  m_image.free();
  m_image.data = m_image_vector.get_data();

  // copy resulting image to gpu tetxure
  m_texture.set_image(m_image);
}

/* Change image opened in canvas to given `path_image` */
void Canvas::change_image(const std::string& path_image) {
  // free pixel data & cairo surface
  m_image.free();
  m_image_vector.free();

  m_image_vector = ImageVector(path_image);
  m_image = Image(path_image, false);
  m_texture.set_image(m_image);
}

/* Save image opened in canvas to given `path_image` */
void Canvas::save_image(const std::string& path_image) {
  m_image.save(path_image);
}

/* Convert image to grayscale and switch shader to monochrome */
void Canvas::to_grayscale() {
  m_image = ImageUtils::to_grayscale(m_image);
  m_texture.set_image(m_image);
  m_program = &m_programs.at("monochrome");
}

/* Blur image using a 9x9 avg. filter */
void Canvas::blur() {
  m_image = ImageUtils::blur(m_image);
  m_texture.set_image(m_image);
}

/* Free opengl texture (image holder) & shaders programs used to display it */
void Canvas::free() {
  m_texture.free();
  m_image_vector.free();

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
