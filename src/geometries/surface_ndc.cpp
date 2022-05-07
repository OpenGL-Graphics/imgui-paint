#include "geometries/surface_ndc.hpp"

SurfaceNDC::SurfaceNDC():
  Geometry(VERTEXES, INDICES, {})
{
}

// vertexes in NDC space (source: https://learnopengl.com/Advanced-OpenGL/Framebuffers)
// lower-left corner: (-1, -1), upper-right corner: (1, 1), and center of screen: (0, 0)
const std::vector<float> SurfaceNDC::VERTEXES = {
// xy     uv
  -1, -1, 0, 0,
   1, -1, 1, 0,
   1,  1, 1, 1,
  -1,  1, 0, 1,
};

const std::vector<unsigned int> SurfaceNDC::INDICES = {
  0, 1, 2, // triangle 1
  2, 3, 0, // triangle 2
};
