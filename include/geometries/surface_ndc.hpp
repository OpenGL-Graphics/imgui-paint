#ifndef SURFACE_NDC_HPP
#define SURFACE_NDC_HPP

#include "geometries/geometry.hpp"

/* Surface with vertexes coords in NDC space (image rendered on its texture using fbo) */
class SurfaceNDC : public Geometry {
public:
  SurfaceNDC();

private:
  static const std::vector<float> VERTEXES;
  static const std::vector<unsigned int> INDICES;
};

#endif // SURFACE_NDC_HPP
