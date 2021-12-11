#version 130

/* modified from `imgui/imgui_impl_opengl3.cpp` */ 
in vec2 texture_coord_vert;

uniform sampler2D texture2d;

out vec4 color_out;

void main() {
  // convert color image to grayscale by averaging 3 channels
  vec4 color = texture(texture2d, texture_coord_vert);
  float avg = (color.r + color.g + color.b) / 3.0;
  color_out = vec4(vec3(avg), color.a);
}
