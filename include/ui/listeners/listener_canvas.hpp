#ifndef LISTENER_CANVAS_HPP
#define LISTENER_CANVAS_HPP

#include "ui/canvas.hpp"

/* Handler for events pertaining to image canvas */
class ListenerCanvas {
public:
  ListenerCanvas(Canvas* canvas);
  void handle_all();
private:
  Canvas* m_canvas;

  void on_open_image();
  void on_save_image();
  void on_to_grayscale();
  void on_blur();
  void on_view_color();
  void on_view_grayscale();
  void on_view_monochrome();
  void on_zoom_in();
  void on_zoom_out();
};

#endif // LISTENER_CANVAS_HPP
