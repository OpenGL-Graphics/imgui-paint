#include "imgui.h"
#include "ui/tooltips/tooltip_pixel.hpp"
#include "ui/imgui_utils.hpp"

TooltipPixel::TooltipPixel(const Image& image):
  m_image(image)
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

    // get pixel value (image is a vector not a 2d array)
    unsigned int i_pixel = position_mouse_img.y*m_image.width + position_mouse_img.x;
    std::vector<unsigned char> pixel_value = m_image.get_pixel_value(i_pixel);
    ImGui::Text("ipixel: %d", i_pixel);

    // transform pixel value into a 4-component vector in [0, 1]
    ImVec4 color = ImGuiUtils::vector_to_imvec4(pixel_value);
    ImGui::Text("color: %f, %f, %f, %f", color.x, color.y, color.z, color.w);

    // show button with pixel color under cursor
    ImGui::ColorButton("MyColor##3c", color);

    ImGui::EndTooltip();
}
