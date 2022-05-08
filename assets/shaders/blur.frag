#version 130

/* modified from `imgui/imgui_impl_opengl3.cpp` */ 
in vec2 texture_coord_vert;

uniform sampler2D texture2d;

out vec4 color_out;

/* Inspired by: https://webglfundamentals.org/webgl/lessons/webgl-image-processing.html */
void main() {
  // uv step corresp. to 1 pixel (lod = 0 => entire image in mipmap)
  vec2 size = textureSize(texture2d, 0);
  vec2 step_uv = vec2(1, 1) / size;

  // average pixel value around current pixel (9x9 averaging filter)
  vec4 sum = 
    texture(texture2d, texture_coord_vert + step_uv * vec2(-1, -1)) +
    texture(texture2d, texture_coord_vert + step_uv * vec2( 0, -1)) +
    texture(texture2d, texture_coord_vert + step_uv * vec2( 1, -1)) +
    texture(texture2d, texture_coord_vert + step_uv * vec2(-1,  0)) +
    texture(texture2d, texture_coord_vert + step_uv * vec2( 0,  0)) +
    texture(texture2d, texture_coord_vert + step_uv * vec2( 1,  0)) +
    texture(texture2d, texture_coord_vert + step_uv * vec2(-1,  1)) +
    texture(texture2d, texture_coord_vert + step_uv * vec2( 0,  1)) +
    texture(texture2d, texture_coord_vert + step_uv * vec2( 1,  1));

  color_out = sum / 9.0;
}
