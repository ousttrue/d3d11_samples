#pragma once
#include "material.h"

namespace banana {

struct SubMesh {
  uint32_t draw_offset = 0;
  uint32_t draw_count = 0;
  std::shared_ptr<Material> material;
};

struct Vertex {
  Float3 position;
  Float3 normal;
  Float2 tex0;
  Float4 color;
  Float4 tangent;
};
using Index = uint32_t;

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<Index> indices;
  std::vector<SubMesh> submeshes;

  AABB aabb = {0};
};

} // namespace banana