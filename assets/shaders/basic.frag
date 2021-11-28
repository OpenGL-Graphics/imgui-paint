#version 130

/* modified from `imgui/imgui_impl_opengl3.cpp` */ 
uniform sampler2D Texture;

in vec2 Frag_UV;
in vec4 Frag_Color;

out vec4 Out_Color;

void main() {
  // change color of rectangle (set in `dialog.cpp` to red)
  // Out_Color = Frag_Color * texture(Texture, Frag_UV.st);
  Out_Color = texture(Texture, Frag_UV);
  // Out_Color = vec4(0.0, 1.0, 0.0, 0.5);

  // imgui renders one-channel (grayscale) image in shades of red by default
  vec4 color = texture(Texture, Frag_UV);
  Out_Color = vec4(color.r, color.r, color.r, color.r);
}
