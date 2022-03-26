#ifndef TOOLTIP_PIXEL_HPP
#define TOOLTIP_PIXEL_HPP

#include "framebuffer.hpp"

class TooltipPixel {
public:
  TooltipPixel(const Framebuffer& framebuffer);
  void render(float y_offset, float y_scroll);
private:
  Framebuffer m_framebuffer;
};

#endif // TOOLTIP_PIXEL_HPP
