#version 130

/**
 * modified from `imgui/imgui_impl_opengl3.cpp`
 * Display a single-channel (monochrome) image
 */
in vec2 texture_coord_vert;

uniform sampler2D texture2d;

out vec4 color_out;

void main() {
  // imgui renders one-channel (grayscale) images in shades of red by default
  vec4 color = texture(texture2d, texture_coord_vert);
  color_out = vec4(color.r);
}
