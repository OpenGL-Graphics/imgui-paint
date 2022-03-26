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
 * Get mouse cursor positions rel. to parent's origin defined by `offset`
 * @param offset Position of parent element (e.g. canvas)
 * Used by ui/tooltips
 */
ImVec2 ImGuiUtils::get_mouse_position(const ImVec2& offset) {
  // change of origin of cursor position (by default org=upper-left corner)
  ImGuiIO& io = ImGui::GetIO();
  ImVec2 position_mouse = io.MousePos;
  ImVec2 position_mouse_parent = ImVec2(position_mouse.x - offset.x, position_mouse.y - offset.y);

  return position_mouse_parent;
}

/**
 * Nanovg sets the origin at lower-left corner
 * Viewport set to size of texture
 */
ImVec2 ImGuiUtils::get_mouse_position_vg(int height_texture, int y_offset) {
  ImGuiIO& io = ImGui::GetIO();
  ImVec2 position_mouse = io.MousePos;
  ImVec2 position_mouse_img = ImVec2(position_mouse.x, height_texture - (position_mouse.y - y_offset));

  return position_mouse_img;
}
