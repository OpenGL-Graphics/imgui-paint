#include "imgui.h"
#include "ui/tooltips/tooltip_image.hpp"
#include "ui/imgui_utils.hpp"

TooltipImage::TooltipImage(const Texture2D& texture):
  m_texture(texture)
{
}

/**
 * Render magnified image region around hovered pixel
 * @param y_offset Height of menu & toolbar
 * @param zoom Canvas' zoom factor
 */
void TooltipImage::render(float y_offset, float zoom) {
    ImGui::BeginTooltip();

    // mouse cursor rel. to canvas's origin
    ImVec2 position_mouse_img = ImGuiUtils::get_mouse_position({ 0.0f, y_offset });
    ImGui::Text("x: %f, y: %f", position_mouse_img.x, position_mouse_img.y);

    // starting & ending image offsets in [0, 1]
    ImVec2 size_image = ImVec2(zoom * m_texture.width, zoom * m_texture.height);
    float zoom_subset = 4.0f;
    float size_region = 32.0f;
    ImVec2 size_subset = ImVec2(zoom_subset * size_region, zoom_subset * size_region);
    ImVec2 uv_start = ImVec2(position_mouse_img.x / size_image.x, position_mouse_img.y / size_image.y);
    ImVec2 uv_end = ImVec2((position_mouse_img.x + size_region) / size_image.x, (position_mouse_img.y + size_region) / size_image.y);
    ImGui::Image((void*)(intptr_t) m_texture.id, size_subset, uv_start, uv_end);

    ImGui::EndTooltip();
}
