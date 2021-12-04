#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "dialog.hpp"
#include "image_utils.hpp"
#include "shader_exception.hpp"

/**
 * Dialog made with imgui
 * Inspired by: https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp
 */
Dialog::Dialog(const Window& window):
  m_window(window),
  m_image("./assets/images/fruits.jpg"),
  // m_image("./assets/images/grass_logo.png"),
  m_texture(m_image), // TODO: notice how image is vertically-inverted
  m_program("assets/shaders/basic.vert", "assets/shaders/basic.frag")
{
  // vertex or fragment shaders failed to compile
  if (m_program.has_failed()) {
    throw ShaderException();
  }

  // setup imgui context & glfw/opengl backends
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(m_window.w, true);
  ImGui_ImplOpenGL3_Init("#version 130");
}

/* Render dialog in main loop */
void Dialog::render() {
  // start imgui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ///
  // enable docking
  ImGuiIO io = ImGui::GetIO(); // configures imgui
  // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ///

  // main menu top toolbar
  render_menu();

  // draw rect in background
  // main window's content size
  ImVec2 size_display = io.DisplaySize;
  m_size_content = { size_display.x, size_display.y - m_size_menu.y };
  ImGui::GetForegroundDrawList()->AddRect({0, 0}, size_display, 0xFF0000FF);

  // imgui window of specified size, anchored at (0, 0), & without padding
  // origin at upper-left corner
  ImGui::SetNextWindowPos({ 0.0f, m_size_menu.y });
  ImGui::SetNextWindowSize(m_size_content);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  bool p_open;
  // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar;
  ImGui::Begin("Dialog title", &p_open, window_flags);

  // render image using custom shader (for grayscale) on drawlist associated with current frame
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  auto callback_data = std::make_tuple(m_program, m_texture);
  draw_list->AddCallback(draw_with_custom_shader, &callback_data);
  render_image();

  // draw_list->AddRectFilled({0, 0}, m_size_content, 0x0000FFFF);
  // draw_list->AddRectFilled({0, 0}, {19, 19}, 0xFF0000FF);

  // reset imgui render state (i.e. to default shader)
  draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

  // render imgui window
  ImGui::PopStyleVar(); // avoids assertion error (associated with padding above)
  ImGui::End();

  ///
  // docking with 2nd frame
  /*
  ImGui::SetNextWindowPos({ 0.0f, size_menu.y });
  ImGui::Begin("Dialog title2");
    if (ImGui::Button("Quit")) {
      m_window.close();
    }

    ImGui::SameLine();
    ImGui::Text("<= Click this button to quit");
  ImGui::End();
  */
  ///

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 * Change to custom shader to show 1-channel image in grayscale
 * Otherwise image will appear in shades of red by default
 * Shader & texture passed as 2nd arg to `ImDrawList->AddCallback()`
 * see: https://github.com/ocornut/imgui/issues/4174
 * @param draw_list Low-level list of polygons
 */
void Dialog::draw_with_custom_shader(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
  // function pointer based on lambda fct cannot capture variables (passed as arg below)
  auto* callback_data = static_cast<std::tuple<Program, Texture2D> *>(cmd->UserCallbackData);
  Program program = std::get<0>(*callback_data);
  Texture2D texture = std::get<1>(*callback_data);

  // get current viewport size incl. menu (`GetDrawData()`: what to render)
  ImDrawData* draw_data = ImGui::GetDrawData();
  ImVec2 size_viewport = draw_data->DisplaySize;

  // parameters to glm::ortho corresp. to screen bounds & position coords above are rel. to these bounds
  // ortho mat inspired by: https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_opengl2.cpp#L146
  // originally viewport & image origins at lower-left corner, but bcos of ortho mat image will be inverted
  // this ortho mat (with inv. bottom & top) allows image to stick to top-left corner of window
  glm::vec2 position(0.0f, 0.0f);
  glm::mat4 projection2d = glm::ortho(0.0f, size_viewport.x, size_viewport.y, 0.0f); // left, right, bottom, top
  glm::mat4 view(1.0f);
  glm::mat4 model(1.0f); // position at window's origin (upper-left corner)
  glm::mat4 transformation = projection2d * view * model;

  // pass uniforms to shaders
  Uniforms uniforms = {
    {"ProjMtx", transformation},
    {"Texture", texture}
  };
  program.use();
  program.set_uniforms(uniforms);
}

/* Show image from texture */
void Dialog::render_image() {
  // double casting avoids `warning: cast to pointer from integer of different size` i.e. smaller
  m_texture.attach();
  ImGui::Image((void*)(intptr_t) m_texture.id, ImVec2(m_texture.width, m_texture.height));
}

/**
 * Render menu bar with its items & attach listeners to its items
 * Sets the size of menu drawn to place content windows below it
 */
void Dialog::render_menu() {
  // static vars only init on 1st function call
  static bool open_image = false;
  static bool quit_app = false;
  static bool to_gray = false;

  // menu buttons listeners
  if (open_image) {
    std::cout << "Open image menu item enabled!" << '\n';

    // https://github.com/aiekick/ImGuiFileDialog#simple-dialog-
    // open file dialog
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileKey", "Choose file", ".jpg,.png", "../assets");
    open_image = false;
  }

  // display file dialog
  if (ImGuiFileDialog::Instance()->Display("ChooseFileKey")) {
    // get file path if ok
    if (ImGuiFileDialog::Instance()->IsOk()) {
      // free previously opened image & open new one
      std::string path_image = ImGuiFileDialog::Instance()->GetFilePathName();
      m_image.free();
      m_image = Image(path_image);
      m_texture.set_image(m_image);
      std::cout << "path image: " << path_image << '\n';
    }

    // close file dialog
    ImGuiFileDialog::Instance()->Close();
  }

  // convert opened image to grayscale
  if (to_gray) {
    m_image = ImageUtils::to_gray(m_image);
    m_texture.set_image(m_image);
    to_gray = false;
  }

  if (quit_app) {
    m_window.close();
  }

  // menu items act like toggle buttons in imgui
  // bool var set/unset on every click
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      ImGui::MenuItem("Open", NULL, &open_image);
      ImGui::MenuItem("Quit", NULL, &quit_app);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Image")) {
      ImGui::MenuItem("To gray", NULL, &to_gray);
      ImGui::EndMenu();
    }

    m_size_menu = ImGui::GetWindowSize();
    ImGui::EndMainMenuBar();
  }
}

/* Destroy imgui */
void Dialog::free() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // free opengl texture (image holder) & shader used to display it
  m_texture.free();
  m_program.free();
}
