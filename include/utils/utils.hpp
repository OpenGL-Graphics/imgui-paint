#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>

#include "imgui.h"

namespace Utils {
  ImVec4 vector_to_imvec4(const std::vector<unsigned char>& pixel_value);
};

#endif // UTILS_HPP
