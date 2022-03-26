#include "ui/imgui_utils.hpp"

/**
 * Convert pixel value extracted from fbo to a ImGui 4-channel vector
 * Transform pixel value in [0, 255] into a 4-component vector in [0, 1]
 */
ImVec4 ImGuiUtils::arr_to_imvec4(unsigned char* pixel_value, int n_channels) {
  ImVec4 color;
  switch (n_channels) {
    case 4: // rgba
      color = {
        pixel_value[0] / 255.0f,
        pixel_value[1] / 255.0f,
        pixel_value[2] / 255.0f,
        pixel_value[3] / 255.0f,
      };
      break;
    case 3: // rgb
      color = {
        pixel_value[0] / 255.0f,
        pixel_value[1] / 255.0f,
        pixel_value[2] / 255.0f,
        1.0f,
      };
      break;
    case 1: // monochrome
      color = {
        pixel_value[0] / 255.0f,
        pixel_value[0] / 255.0f,
        pixel_value[0] / 255.0f,
        1.0f,
      };
      break;
  }

  return color;
}

/**
 * Get mouse cursor positions rel. to parent's origin defined by `offset` (origin at upper-left corner)
 * @param offset Position of parent element (e.g. canvas)
 * @param y_scroll Height of vertical scroll (bcoz imgui ignores it when given cursor's x,y)
 *                 Passed as param bcoz this function is called inside tooltip (ImGui::GetScrollY() ret always 0)
 * Used by ui/tooltips
 */
ImVec2 ImGuiUtils::get_mouse_position(const ImVec2& offset, float y_scroll) {
  ImGuiIO& io = ImGui::GetIO();
  ImVec2 position_mouse = io.MousePos;

  // count scroll-y <> 0 when image taller than canvas (otherwise y calculated rel. to upper-left corner)
  // offset-y is the part of the image hidden above after vertical scrolling
  position_mouse.y += y_scroll;

  // change of origin of cursor position (by default org=upper-left corner)
  ImVec2 position_mouse_parent = ImVec2(position_mouse.x - offset.x, position_mouse.y - offset.y);

  return position_mouse_parent;
}

/**
 * Calculate mouse position relative to nanovg's origin (to draw shapes)
 * Nanovg sets the origin at lower-left corner
 * Viewport set to size of texture
 */
ImVec2 ImGuiUtils::get_mouse_position_vg(int height_texture, int y_offset) {
  ImGuiIO& io = ImGui::GetIO();
  ImVec2 position_mouse = io.MousePos;

  // count scroll-y <> 0 when image taller than canvas (otherwise y calculated rel. to upper-left corner)
  // offset-y is the part of the image hidden above after vertical scrolling
  float y_scroll = ImGui::GetScrollY();
  position_mouse.y += y_scroll;

  // change origin to lower-left corner
  ImVec2 position_mouse_img = ImVec2(position_mouse.x, height_texture - (position_mouse.y - y_offset));

  return position_mouse_img;
}
