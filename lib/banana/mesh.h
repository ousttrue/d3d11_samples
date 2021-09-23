#pragma once
#include "material.h"
#include <span>
#include <stdint.h>

namespace banana {

struct SubMesh {
  uint32_t draw_offset = 0;
  uint32_t draw_count = 0;
  std::shared_ptr<Material> material;
};

struct Mesh {
  std::vector<uint8_t> vertices;
  size_t vertex_stride = 0;
  size_t vertex_dynamic_buffer_size = 0;
  bool vertex_updated = false;

  std::vector<uint8_t> indices;
  size_t index_stride = 0;
  size_t index_dynamic_buffer_size = 0;
  bool index_updated = false;
  size_t index_count() const { return indices.size() / index_stride; }

  std::vector<SubMesh> submeshes;
  AABB aabb = {0};

  template <typename V, typename I>
  void assign(std::span<V> v, std::span<I> i, bool dynamic = false) {
    vertex_stride = sizeof(V);
    vertices.assign((const uint8_t *)v.data(),
                    (const uint8_t *)v.data() + v.size() * vertex_stride);
    if (dynamic) {
      vertex_updated = true;
    }

    index_stride = sizeof(I);
    indices.assign((const uint8_t *)i.data(),
                   (const uint8_t *)i.data() + i.size() * index_stride);
    if (dynamic) {
      index_updated = true;
      if (submeshes.size() == 1) {
        submeshes[0].draw_count = static_cast<uint32_t>(indices.size() / index_stride);
      }
    }
  }
};

} // namespace banana
