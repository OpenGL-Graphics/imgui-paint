#version 130

/* modified from `imgui/imgui_impl_opengl3.cpp` */ 
in vec2 texture_coord_vert;

uniform sampler2D texture2d;

out vec4 color_out;

void main() {
  color_out = texture(texture2d, texture_coord_vert);
}
