#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_coord;

out vec2 texture_coord_vert;

/**
 * Shader used to render to texture via fbo, so displayed image (modified by frag. shaders) can be saved to disk later
 * vertex positions passed in NDC space
 */
void main() {
  // z = -1 at near-plane & z = 1 at far-plane (z = 0 in between) - left-hand rule
  // image (of same size) displayed only when z in [-1, 1]
  gl_Position = vec4(position, 0, 1);
  texture_coord_vert = texture_coord;
}
