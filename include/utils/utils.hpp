#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>

#include "imgui.h"

namespace Utils {
  ImVec4 vector_to_imvec4(const std::vector<unsigned char>& pixel_value);
  ImVec2 get_mouse_position(const ImVec2& offset={ 0.0f, 0.0f });
};

#endif // UTILS_HPP
