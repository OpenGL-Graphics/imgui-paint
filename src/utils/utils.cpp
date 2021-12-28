#include "utils/utils.hpp"

/**
 * Convert pixel value extracted from stb_image to a ImGui 4-channel vector
 * Transform pixel value into a 4-component vector in [0, 1]
 */
ImVec4 Utils::vector_to_imvec4(const std::vector<unsigned char>& pixel_value) {
  ImVec4 color;
  switch (pixel_value.size()) {
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
