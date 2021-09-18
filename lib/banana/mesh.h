#pragma once
#include "material.h"

namespace banana {

struct SubMesh {
  uint32_t offset = 0;
  uint32_t draw_count = 0;
  std::shared_ptr<Material> material;
};

struct Mesh {
  struct Vertex {
    Float3 position;
    Float3 normal;
    Float2 tex0;
    Float4 color;
  };
  using Index = uint32_t;

  std::vector<Vertex> vertices;
  std::vector<Index> indices;
  std::vector<SubMesh> submeshes;

  AABB aabb = {0};
};

} // namespace banana