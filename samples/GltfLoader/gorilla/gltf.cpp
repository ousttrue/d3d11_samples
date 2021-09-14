#include "gltf.h"
#include <nlohmann/json.hpp>

namespace gorilla::gltf {

// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/

static std::span<const uint8_t> from_bufferview(const nlohmann::json &gltf,
                                                std::span<const uint8_t> bin,
                                                int bufferview_index) {
  auto bufferview = gltf["bufferViews"][bufferview_index];

  std::span<const uint8_t> buffer;
  if (bufferview.contains("uri")) {
    throw std::runtime_error("not implemented");
  } else {
    buffer = bin;
  }

  auto byteOffset = 0;
  if (bufferview.contains("byteOffset")) {
    byteOffset = bufferview["byteOffset"];
  }

  int byteLength = bufferview["byteLength"];

  return buffer.subspan(byteOffset, byteLength);
}

template <typename T>
static std::span<const T> from_accessor(const nlohmann::json &gltf,
                                        std::span<const uint8_t> bin,
                                        int accessor_index) {
  auto accessor = gltf["accessors"][accessor_index];

  auto bufferview_bytes = from_bufferview(gltf, bin, accessor["bufferView"]);

  auto byteOffset = 0;
  if (accessor.contains("byteOffset")) {
    byteOffset = accessor["byteOffset"];
  }

  std::span<const T> span((const T *)(bufferview_bytes.data() + byteOffset),
                          (size_t)accessor["count"]);
  return span;
}

bool GltfLoader::load(std::string_view json, std::span<const uint8_t> bin) {
  auto gltf = nlohmann::json::parse(json);
  for (auto &gltf_mesh : gltf["meshes"]) {

    auto &mesh = meshes.emplace_back(Mesh{});

    size_t vertex_offset = 0;
    for (auto &gltf_prim : gltf_mesh["primitives"]) {
      // position
      int position_accessor_index = gltf_prim["attributes"]["POSITION"];
      auto position = from_accessor<float3>(gltf, bin, position_accessor_index);
      mesh.vertices.resize(mesh.vertices.size() + position.size());
      for (size_t i = 0; i < position.size(); ++i) {
        mesh.vertices[vertex_offset + i].position = position[i];
      }

      if (gltf_prim.contains("indices")) {
        // indices
        int indices_accessor_index = gltf_prim["indices"];
        auto indices_accessor = gltf["accessors"][indices_accessor_index];
        int indices_type = indices_accessor["componentType"];
        switch (indices_type) {
        case 5120: // BYTE
        case 5121: // UNSIGNED_BYTE
          // 1
          throw std::runtime_error("not implemented");

        case 5123: // UNSIGNED_SHORT
        {
          auto indices =
              from_accessor<uint16_t>(gltf, bin, indices_accessor_index);
          mesh.indices.reserve(mesh.indices.size() + indices.size());
          for (auto &i : indices) {
            mesh.indices.push_back(i);
          }
          break;
        }

        default:
          throw std::runtime_error("not implemented");
        }
      }

      vertex_offset += position.size();
    }
  }

  return true;
}

} // namespace gorilla::gltf
