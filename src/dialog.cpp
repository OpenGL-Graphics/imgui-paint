#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
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
  m_image("./assets/images/grass_logo.png"),
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

  // imgui window with widgets
  ImGui::SetNextWindowSize({ 500.0f, 500.0f });
  ImGui::Begin("Dialog title");
  render_components();

  // draw rect with cutom shader, then reset imgui render state (i.e. to default shader)
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  change_shader();
  draw_list->AddRectFilled({0.0, 0.0}, {500.0, 500.0}, 0xFF0000FF);
  draw_list->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

  // render imgui window
  ImGui::End();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 * Change to custom shader to show 1-channel image in grayscale
 * Otherwise image will appear in shades of red by default
 * see: https://github.com/ocornut/imgui/issues/4174
 * @param draw_list Low-level list of polygons
 */
void Dialog::change_shader() {
  /*
  struct CallbackData {
    Program program;
    Texture2D texture;

    CallbackData(const Program& p, const Texture2D& t):
      texture(t), program(p)
    {}
  };
  CallbackData callback_data(m_program, m_texture);
  */

  auto callback_data = std::make_pair(&m_program, &m_texture);
  // std::cout << "before: " << mprogra << '\n';

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  draw_list->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    // function pointer based on lambda fct cannot capture variables (passed as arg below)
    /*
    auto* callback_data = static_cast<std::pair<Program*, Texture2D*> *>(cmd->UserCallbackData);
    Program* program = callback_data->first;
    Texture2D* texture = callback_data->second;
    std::cout << "texture: " << callback_data->second->id << '\n';
    */
    Program* program = static_cast<Program*>(cmd->UserCallbackData);

    // projection matrix from viewport size (`GetDrawData()`: what to render)
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImVec2 position = draw_data->DisplayPos;
    ImVec2 size = draw_data->DisplaySize;
    glm::mat4 projection2d = glm::ortho(0.0f, size.x, 0.0f, size.y);
    // std::cout << "position: " << position.x << " " << position.y << '\n';
    // std::cout << "size: " << size.x << " " << size.y << '\n';

    // pass shaders uniforms
    Uniforms uniforms = {
      {"ProjMtx", projection2d},
      // {"texture", *texture}
    };
    program->use();
    program->set_uniforms(uniforms);
  // }, &callback_data);
  }, &m_program);
}

/**
 * Render gui components (e.g. buttons)
 * Called in main loop
 */
void Dialog::render_components() {
  // buttons return true when clicked
  /*
  if (ImGui::Button("Quit")) {
    m_window.close();
  }

  ImGui::SameLine();
  ImGui::Text("<= Click this button to quit");
  bool show_demo_window = true;
  ImGui::ShowDemoWindow(&show_demo_window);
  */

  // show image from texture
  // double casting avoids `warning: cast to pointer from integer of different size` i.e. smaller
  m_texture.attach();
  ImGui::Image((void*)(intptr_t) m_texture.id, ImVec2(m_texture.width, m_texture.height));

  // menu
  render_menu();

  // plot values
  // float values[] = {0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f};
  // ImGui::PlotLines("Plot", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, ImVec2(0, 200));
}

/* Render menu bar with its items & attach listeners to its items */
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
