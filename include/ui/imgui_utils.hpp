#ifndef IMGUI_UTILS_HPP
#define IMGUI_UTILS_HPP

#include <vector>

#include "imgui.h"

namespace ImGuiUtils {
  ImVec4 arr_to_imvec4(unsigned char* pixel_value, int n_channels);
  ImVec2 get_mouse_position(const ImVec2& offset={ 0.0f, 0.0f }, float y_scroll=0.0f);
  ImVec2 get_mouse_position_vg(int height_texture, int y_offset);
};

#endif // IMGUI_UTILS_HPP
