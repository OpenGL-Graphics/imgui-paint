#ifndef TOOLTIP_PIXEL_HPP
#define TOOLTIP_PIXEL_HPP

#include "image.hpp"

class TooltipPixel {
public:
  TooltipPixel(const Image& image);
  void render(float y_offset);
private:
  Image m_image;
};

#endif // TOOLTIP_PIXEL_HPP
