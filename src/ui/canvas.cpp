#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

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

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

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

  // create nanovg context (similar to html5 canvas')
  m_vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);

  // load image data with stb_image, create texture, copy image data to gpu texture, then free image data
  // TODO: delete Canvas::m_image (redundant)
  m_image_vg = nvgCreateImage(m_vg, path_image.c_str(), NVG_IMAGE_FLIPY);

  // empty texture (same size as openend image) to fill when drawing to framebuffer
  int width, height;
  nvgImageSize(m_vg, m_image_vg, &width, &height);
  Image image_framebuffer(width, height, GL_RGBA, NULL);
  m_texture = Texture2D(image_framebuffer);

  // attach empty texture to framebuffer
  m_framebuffer.attach_texture(m_texture);
  std::cout << std::hex << "OpenGL Error: " << glGetError() << '\n';

  if (!m_framebuffer.is_complete()) {
    throw FramebufferException();
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

  // imgui window of specified size, anchored at (0, 0), & without padding, no dark background overlay
  // origin at upper-left corner
  ImGui::SetNextWindowPos({ 0.0f, y_offset });
  ImGui::SetNextWindowSize(Size::canvas);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); // otherwise cursor coords rel. to image org starts at 1 (not 0)
  bool p_open;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;
  ImGui::Begin("Canvas", &p_open, window_flags);

  // draw vector shapes & image on fbo's texture with nanovg
  // TODO: sometimes opengl throws: `glGetError 506` (incomplete fbo)
  {
    // clear framebuffer's attached color buffer in every frame
    m_framebuffer.bind();
    m_framebuffer.clear({ 0.0f, 0.0f, 0.0f, 1.0f });

    float pixel_ratio = 1.0f; // framebuffer & window have same size
    nvgBeginFrame(m_vg, Size::canvas.x, Size::canvas.y, pixel_ratio);

    // origin at lower-left corner of image (2d projection)
    // y is offset as otherwise image will stretch till top of window (behind menu)
    // draw image on fbo's texture
    float alpha = 1.0f;
    float angle = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
    int width = m_texture.width;
    int height = m_texture.height;
    nvgImageSize(m_vg, m_image_vg, &width, &height);
    NVGpaint imgPaint = nvgImagePattern(m_vg, x, y - y_offset, width, height - y_offset, angle, m_image_vg, alpha);
    nvgBeginPath(m_vg);
    nvgRect(m_vg, x, y, width, height);
    nvgFillPaint(m_vg, imgPaint);
    nvgFill(m_vg);
    nvgClosePath(m_vg);

    // draw rectangle & circle on fbo's texture
    nvgBeginPath(m_vg);
    // nvgRect(m_vg, 100,100, 120,30);
    nvgCircle(m_vg, 100, 100, 25);
    nvgFillColor(m_vg, nvgRGBA(255,192,0,255));
    nvgFill(m_vg);
    nvgClosePath(m_vg);

    nvgEndFrame(m_vg);

    // detach framebuffer
    m_framebuffer.unbind();
  }

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
  // Error code = 502 (GL_INVALID_OPERATION) when used
  // render image using custom shader
  // use_shader();

  // render image & graphics drawn on texture attached to fbo
  // double casting avoids `warning: cast to pointer from integer of different size` i.e. smaller
  m_texture.attach();
  ImVec2 size_image = ImVec2(m_zoom * m_texture.width, m_zoom * m_texture.height);
  ImGui::Image((void*)(intptr_t) m_texture.id, size_image);

  // draw circle/line at mouse click position with Cairo (managed by listener)
  if (ImGui::IsItemClicked()) {
    if (Toolbar::draw_circle) {
      draw("circle");
      Menu::draw_circle = false;
      Toolbar::draw_circle = false;

      ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }

    // two clicks needed to draw a line (cursor saved on 1st click & reset after 2nd)
    if (Toolbar::draw_line) {
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

  // brush tool paints while LMB dragged
  // https://github.com/ocornut/imgui/issues/493
  if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
    if (Toolbar::brush)
      draw("circle", false);
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
void Canvas::draw(const std::string& type_shape, bool has_strokes) {
  float y_offset = Size::menu.y + Size::toolbar.y;
  ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position({ 0.0f, y_offset });
  ImVec2 size, offset;

  // draw line/circle with Cairo (instead of OpenCV => anti-aliased edges by default with vectors)
  // TODO: Both line & circle strokes are cut at top-left bcos of Cairo (=> enlarge crop rectangle)
  if (type_shape == "circle") {
    float radius = 5.0f;
    m_image_vector.draw_circle(position_mouse_img, radius, has_strokes);

    // `+ 1` to include both extremities (see glTexSubImage2D() docs)
    ImVec2 start_bbox( position_mouse_img.x - radius, position_mouse_img.y - radius );
    ImVec2 end_bbox( position_mouse_img.x + radius, position_mouse_img.y + radius );
    offset = start_bbox;
    size = { end_bbox.x - start_bbox.x + 1, end_bbox.y - start_bbox.y + 1 };
  } else if (type_shape == "line") {
    std::cout << "Line start point x: " << m_cursor.x << " y: " << m_cursor.y << '\n';
    std::cout << "Line end point x: " << position_mouse_img.x << " y: " << position_mouse_img.y << '\n';
    m_image_vector.draw_line(m_cursor, position_mouse_img);

    // `+ 1` to include both extremities (see glTexSubImage2D() docs)
    offset = { m_cursor.x, m_cursor.y };
    size = { position_mouse_img.x - m_cursor.x + 1, position_mouse_img.y - m_cursor.y + 1 };
  }

  // get updated subimage from converted cairo surface
  Profiler profiler;
  profiler.start();
  unsigned char* subdata = m_image_vector.get_subdata(size, offset);
  Image subimage(size.x, size.y, m_image.format, subdata, m_image.path);
  profiler.stop();
  // profiler.print("surface -> pixbuf -> char*");

  // copy modified sub-image to gpu texture
  profiler.start();
  m_texture.set_subimage(subimage, { size.x, size.y }, { offset.x, offset.y });
  m_image_vector.free_pixbuf(); // also frees image's `uchar *` data
  profiler.stop();
  // profiler.print("To GPU");
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
  m_image_vector.free();

  for (auto& pair: m_programs) {
    pair.second.free();
  }

  // destroy framebuffer & texture attached to it
  m_texture.free();
  m_framebuffer.free();

  // delete image texture created by nanovg
  nvgDeleteImage(m_vg, m_image_vg);

  // free nanovg context
  nvgDeleteGL3(m_vg);
}

void Canvas::zoom_in() {
  m_zoom *= 2.0f;
}

void Canvas::zoom_out() {
  m_zoom /= 2.0f;
}
