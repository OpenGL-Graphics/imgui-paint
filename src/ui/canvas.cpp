#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>

#include "glad/glad.h"

#include "ui/canvas.hpp"
#include "ui/toolbar.hpp"
#include "ui/menu.hpp"
#include "ui/imgui_utils.hpp"

#include "ui/enumerations/hover_mode.hpp"
#include "ui/enumerations/mode.hpp"

#include "ui/globals/size.hpp"
#include "ui/globals/color.hpp"
#include "ui/globals/cursor.hpp"
#include "ui/globals/mode.hpp"

#include "image/image_utils.hpp"

#include "shader_exception.hpp"
#include "framebuffer_exception.hpp"
#include "profiling/profiler.hpp"
#include "geometries/surface_ndc.hpp"

/**
 * Canvas showing image
 * @param path_image Path to image to load initially
 * TODO: remove `path_image` to start with an empty canvas
 */
Canvas::Canvas(const std::string path_image):
  // `unordered_map::operator[]()` requires map's value (i.e. Program) to have default constructor
  m_programs {
    {"color", Program("assets/shaders/fbo.vert", "assets/shaders/color.frag")},
    {"grayscale", Program("assets/shaders/fbo.vert", "assets/shaders/grayscale.frag")},
    {"monochrome", Program("assets/shaders/fbo.vert", "assets/shaders/monochrome.frag")},
  },

  m_texture_shapes(Image(path_image, false)),
  m_texture_effects(Image(m_texture_shapes.width, m_texture_shapes.height, m_texture_shapes.format, NULL)),

  m_width(m_texture_shapes.width),
  m_height(m_texture_shapes.height),

  m_framebuffer(),
  m_renderer(m_programs.at("color"), VBO(SurfaceNDC{}), {
    {0, "position", 2, 4, 0},
    {1, "texture_coord", 2, 4, 2}
  }),

  m_image_vg(),

  m_zoom(1.0f),
  m_tooltip_image(m_texture_effects),
  m_tooltip_pixel(m_framebuffer)
{
  // attach input image texture in normal mode to fbo
  m_framebuffer.attach_texture(m_texture_effects);

  // one of the vertex/fragment shaders failed to compile
  for (auto& pair : m_programs) {
    if (pair.second.has_failed()) {
      throw ShaderException();
    }
  }
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
  glViewport(0, 0, m_width, m_height);

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

/**
 * Change to shader identified by its key
 * @param name Shader key in `m_programs`
 */
void Canvas::set_shader(const std::string& key) {
  m_renderer.program = m_programs.at(key);
}

/* render image to framebuffer texture */
void Canvas::render_to_fbo() {
  // clear framebuffer's attached color buffer in every frame
  m_framebuffer.bind();
  m_framebuffer.clear({ 1.0f, 1.0f, 1.0f, 1.0f });

  // draw 2d health bar HUD surface (scaling then translation with origin at lower left corner)
  m_renderer.draw({ {"texture2d", m_texture_shapes} });
  m_framebuffer.unbind();
}

/**
 * Show image from texture using custom shader
 * Change to custom shader before rendering image
 * @param y_offset heights of menu & toolbar used to calculate cursor position rel. to image
 */
void Canvas::render_image(float y_offset) {
  // different texture rendered & attached to fbo if in drawing/normal mode
  Texture2D texture = (mode == Mode::NORMAL) ? m_texture_effects : m_texture_shapes;
  m_framebuffer.attach_texture(texture);

  // only render to surface geometry when not in drawing mode
  if (mode == Mode::NORMAL)
    render_to_fbo();

  // render image & graphics drawn on texture attached to fbo
  // double casting avoids `warning: cast to pointer from integer of different size` i.e. smaller
  texture.attach();
  ImVec2 size_image = ImVec2(m_zoom * m_width, m_zoom * m_height);
  ImGui::Image((void*)(intptr_t) texture.id, size_image);

  // draw circle/line at mouse click position
  if (ImGui::IsItemClicked()) {
    if (Toolbar::draw_circle) {
      ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position_vg(m_height, y_offset);
      m_image_vg.draw_circle(m_framebuffer, position_mouse_img.x, position_mouse_img.y);
      Menu::draw_circle = false;
      Toolbar::draw_circle = false;

      ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }

    // two clicks needed to draw a line (cursor saved on 1st click & reset after 2nd)
    if (Toolbar::draw_line) {
      if (cursor.x == VECTOR_UNSET.x && cursor.y == VECTOR_UNSET.y) {
        move_cursor();
      } else {
        ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position_vg(m_height, y_offset);
        m_image_vg.draw_line(m_framebuffer, cursor.x, cursor.y, position_mouse_img.x, position_mouse_img.y);

        cursor = VECTOR_UNSET;
        Menu::draw_line = false;
        Toolbar::draw_line = false;

        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
      }
    }

    // set starting point (initial click) for brush line
    if (Toolbar::brush_line && cursor.x == VECTOR_UNSET.x && cursor.y == VECTOR_UNSET.y) {
      move_cursor();
    }
  }

  // brush tool paints while LMB dragged
  // https://github.com/ocornut/imgui/issues/493
  if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
    if (Toolbar::brush_circle) {
      ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position_vg(m_height, y_offset);
      m_image_vg.draw_circle(m_framebuffer, position_mouse_img.x, position_mouse_img.y);
    }
    else if (Toolbar::brush_line) {
        ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position_vg(m_height, y_offset);
        m_image_vg.draw_line(m_framebuffer, cursor.x, cursor.y, position_mouse_img.x, position_mouse_img.y);
        move_cursor();
    }
  }

  // unset cursor position when mouse released in brush line mode
  if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
    if (Toolbar::brush_line)
      cursor = VECTOR_UNSET;
  }

  if (ImGui::IsItemHovered()) {
    // show tooltip containing zoomed subset image (source: imgui_demo.cpp:986) or pixel value accord. to toolbar radio button
    if (Toolbar::hover_mode == HoverMode::IMAGE_SUBSET) {
      m_tooltip_image.render(y_offset, m_zoom);
    } else if (Toolbar::hover_mode == HoverMode::PIXEL_VALUE) {
      float y_scroll = ImGui::GetScrollY();
      m_tooltip_pixel.render(y_offset, y_scroll);
    }

    // change to hand cursor if hovering in drawing mode
    if (Toolbar::draw_circle || Toolbar::draw_line || Toolbar::brush_circle || Toolbar::brush_line)
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }
}

/* Define line's start point */
void Canvas::move_cursor() {
  // float y_offset = Size::menu.y + Size::toolbar.y;
  // ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position({ 0.0f, y_offset });
  float y_offset = Size::menu.y + Size::toolbar.y;
  ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position_vg(m_height, y_offset);

  cursor = position_mouse_img;
}

/* Change image opened in canvas to given `path_image` */
void Canvas::change_image(const std::string& path_image) {
  // replace image texture & reset shader
  Image image_new = Image(path_image, false);
  m_texture_shapes.set_image(image_new);
  m_renderer.program = m_programs.at("color");

  // update dimensions (needed to get mouse coord rel. to image)
  m_width = image_new.width;
  m_height = image_new.height;
}

/* Save image opened in canvas to given `path_image` */
void Canvas::save_image(const std::string& path_image) {
  // modified image retrieved from opengl texture (from gpu)
  Image image = m_texture_effects.get_image();

  // save image on disk & free pointer
  image.save(path_image);
  image.free();
}

/* Convert image to grayscale on gpu (through shader & fbo) */
void Canvas::to_grayscale() {
  m_renderer.program = m_programs.at("grayscale");
}

/* Blur image using a 9x9 avg. filter */
void Canvas::blur() {
  m_renderer.program = m_programs.at("blur");
}

/* Free opengl texture (image holder) & shaders programs used to display it */
void Canvas::free() {
  for (auto& pair: m_programs) {
    pair.second.free();
  }

  // destroy buffers
  m_renderer.free();

  // destroy texures
  m_texture_shapes.free();
  m_texture_effects.free();

  // destroy nanovg context
  m_image_vg.free();

  // destroy framebuffer
  m_framebuffer.free();
}

void Canvas::zoom_in() {
  m_zoom *= 2.0f;
}

void Canvas::zoom_out() {
  m_zoom /= 2.0f;
}
