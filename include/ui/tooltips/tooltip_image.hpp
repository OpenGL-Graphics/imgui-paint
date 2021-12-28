#ifndef TOOLTIP_IMAGE_HPP
#define TOOLTIP_IMAGE_HPP

#include "texture.hpp"

class TooltipImage {
public:
  TooltipImage(const Texture2D& texture);
  void render(float y_offset, float zoom);
private:
  Texture2D m_texture;
};

#endif // TOOLTIP_IMAGE_HPP
