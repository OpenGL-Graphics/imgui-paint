#ifndef MENU_HPP
#define MENU_HPP

/* Main menu with booleans flags set on menu item click */
struct Menu {
  /**
   * flags set on button click/radio button check (needed to activate listeners in `Dialog`)
   * Declared static so they can be accessed from all classes (incl. listeners)
   */
  static bool open_image, save_image, quit_app; // menu File
  static bool to_grayscale, blur; // menu Edit
  static bool view_color, view_grayscale, view_monochrome; // menu View
  static bool zoom_in, zoom_out; // menu Zoom
  static bool draw_circle, draw_line, brush; // menu Draw

  Menu();
  void render();
};

#endif // MENU_HPP
