#include <iostream>

#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include "ui/listeners/listener_canvas.hpp"
#include "ui/menu.hpp"
#include "ui/toolbar.hpp"

/**
 * @param canvas Pointer passed so it can be modified (instead of modifying a copy)
 */
ListenerCanvas::ListenerCanvas(Canvas* canvas):
  m_canvas(canvas)
{
}

/* Handle all events related to canvas fired on click on menu items or toolbar buttons */
void ListenerCanvas::handle_all() {
  on_open_image();
  on_save_image();
  on_to_grayscale();
  on_blur();
  on_view_color();
  on_view_grayscale();
  on_view_monochrome();
  on_zoom_in();
  on_zoom_out();
}

/* Open a new image */
void ListenerCanvas::on_open_image() {
  if (Menu::open_image || Toolbar::open_image) {
    std::cout << "Open image menu item enabled!" << '\n';

    // https://github.com/aiekick/ImGuiFileDialog#simple-dialog-
    // open image dialog
    ImGuiFileDialog::Instance()->OpenModal("OpenImageKey", "Open image", "Image files{.jpg,.png}", "./assets/images", "");
    Menu::open_image = false;
    Toolbar::open_image = false;
  }

  // display open image file dialog
  if (ImGuiFileDialog::Instance()->Display("OpenImageKey", ImGuiWindowFlags_None, ImVec2(600, 300), ImVec2(600, 300))) {
    // get file path if ok
    if (ImGuiFileDialog::Instance()->IsOk()) {
      // free previously opened image & open new one
      std::string path_image = ImGuiFileDialog::Instance()->GetFilePathName();
      m_canvas->change_image(path_image);
      std::cout << "Path of opened image: " << path_image << '\n';
    }

    // close file dialog
    ImGuiFileDialog::Instance()->Close();
  }
}

/* Save opened/edited image */
void ListenerCanvas::on_save_image() {
  if (Menu::save_image || Toolbar::save_image) {
    // open image dialog
    ImGuiFileDialog::Instance()->OpenModal("SaveImageKey", "Save image", "Image files{.jpg,.png}", "./assets/images", "");
    Menu::save_image = false;
    Toolbar::save_image = false;
  }

  // display save image file dialog
  if (ImGuiFileDialog::Instance()->Display("SaveImageKey", ImGuiWindowFlags_None, ImVec2(600, 300), ImVec2(600, 300))) {
    // get file path if ok
    if (ImGuiFileDialog::Instance()->IsOk()) {
      // free previously opened image & open new one
      std::string path_image = ImGuiFileDialog::Instance()->GetFilePathName();
      m_canvas->save_image(path_image);
      std::cout << "Path for saved image: " << path_image << '\n';
    }

    // close file dialog
    ImGuiFileDialog::Instance()->Close();
  }
}

/* convert opened image to grayscale & update shader to show monochrome image */
void ListenerCanvas::on_to_grayscale() {
  if (Menu::to_grayscale) {
    m_canvas->to_grayscale();
    Menu::to_grayscale = false;
  }
}

/* blur opened image using a 9x9 avg filter */
void ListenerCanvas::on_blur() {
  if (Menu::blur) {
    m_canvas->blur();
    Menu::blur = false;
  }
}

/* update to shader to show image in color */
void ListenerCanvas::on_view_color() {
  if (Menu::view_color) {
    m_canvas->set_shader("color");
    Menu::view_color = false;
  }
}

/* update shader to show image in grayscale */
void ListenerCanvas::on_view_grayscale() {
  if (Menu::view_grayscale) {
    m_canvas->set_shader("grayscale");
    Menu::view_grayscale = false;
  }
}

/* update to shader to show monochrome (1-channel) image */
void ListenerCanvas::on_view_monochrome() {
  if (Menu::view_monochrome) {
    m_canvas->set_shader("monochrome");
    Menu::view_monochrome = false;
  }
}

void ListenerCanvas::on_zoom_in() {
  if (Menu::zoom_in || Toolbar::zoom_in) {
    m_canvas->zoom_in();
    Menu::zoom_in = false;
    Toolbar::zoom_in = false;
  }
}

void ListenerCanvas::on_zoom_out() {
  if (Menu::zoom_out || Toolbar::zoom_out) {
    m_canvas->zoom_out();
    Menu::zoom_out = false;
    Toolbar::zoom_out = false;
  }
}
