#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP

/* Main toolbar with booleans flags set on button click */
struct Toolbar {
  /**
   * flags set on button click/radio button check (needed to activate listeners in `Dialog`)
   * Declared static so they can be accessed from all classes (incl. listeners)
   */
  static bool open_image, save_image, quit_app;
  static bool zoom_in, zoom_out;
  static bool draw_circle, draw_line, brush_circle, brush_line;
  static int hover_mode;

  Toolbar();
  void render();
};

#endif // TOOLBAR_HPP
