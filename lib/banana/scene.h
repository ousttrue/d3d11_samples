#pragma once
#include "image.h"
#include <optional>
#include <stdint.h>
#include <string>
#include <memory>

namespace banana {

struct Float2 {
  float x;
  float y;
};

struct Float3 {
  float x;
  float y;
  float z;
};

struct Float4 {
  float x;
  float y;
  float z;
  float w;
};

struct Transform {
  Float3 translation;
  Float4 rotation; // quaternon
  Float3 scaling;
};

struct Material {
  std::shared_ptr<Image> base_color_texture;
};

struct SubMesh {
  uint32_t offset = 0;
  uint32_t draw_count = 0;
  std::shared_ptr<Material> material;
};

struct Mesh {
  struct Vertex {
    Float3 position;
    Float2 tex0;
  };
  using Index = uint32_t;

  std::vector<Vertex> vertices;
  std::vector<Index> indices;
  std::vector<SubMesh> submeshes;
};

class Node {
  std::string name;
  Transform transform;
};

} // namespace  banana
