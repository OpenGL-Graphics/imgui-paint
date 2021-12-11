#version 130

/* modified from `imgui/imgui_impl_opengl3.cpp` */ 
in vec2 position;
in vec2 texture_coord;

uniform mat4 transformation;

out vec2 texture_coord_vert;

void main() {
  gl_Position = transformation * vec4(position, 0, 1);
  texture_coord_vert = texture_coord;
}
