#include "imgui.h"
#include "ui/tooltips/tooltip_pixel.hpp"
#include "ui/imgui_utils.hpp"

/**
 * Get image pixel value directly from fbo
 * bcoz user may paint on texture in the meantime
 */
TooltipPixel::TooltipPixel(const Framebuffer& framebuffer):
  m_framebuffer(framebuffer)
{
}

/**
 * Render pixel value at cursor location
 * @param y_offset Height of menu & toolbar
 */
void TooltipPixel::render(float y_offset) {
    ImGui::BeginTooltip();

    // mouse cursor rel. to canvas's origin
    ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position({ 0.0f, y_offset });
    ImGui::Text("x: %f, y: %f", position_mouse_img.x, position_mouse_img.y);

    // read pixel value at (x, y) from fbo
    int n_channels = m_framebuffer.n_channels;
    unsigned char pixel_value[n_channels];
    m_framebuffer.get_pixel_value(position_mouse_img.x, position_mouse_img.y, pixel_value);

    // transform pixel value in [0, 255] into a 4-component vector in [0, 1]
    ImVec4 color = ImGuiUtils::arr_to_imvec4(pixel_value, n_channels);
    ImGui::Text("color: %f, %f, %f, %f", color.x, color.y, color.z, color.w);

    // show button with pixel color under cursor
    ImGui::ColorButton("MyColor##3c", color);

    ImGui::EndTooltip();
}
