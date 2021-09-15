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

static Texture load_texture(const nlohmann::json &gltf,
                            std::span<const uint8_t> bin,
                            const nlohmann::json &gltf_texture) {
  Texture texture = {};

  if (gltf_texture.contains("source")) {
    int image_index = gltf_texture["source"];
    texture.bytes = from_bufferview(gltf, bin, image_index);
  }

  return texture;
}

static Material load_material(const nlohmann::json &gltf,
                              std::span<const uint8_t> bin,
                              const nlohmann::json &gltf_material) {
  Material material = {};

  if (gltf_material.contains("pbrMetallicRoughness")) {
    auto pbrMetallicRoughness = gltf_material["pbrMetallicRoughness"];
    if (pbrMetallicRoughness.contains("baseColorTexture")) {
      auto baseColorTexture = pbrMetallicRoughness["baseColorTexture"];
      if (baseColorTexture.contains("index")) {
        material.base_color_texture_index = baseColorTexture["index"];
      }
    }
  }

  return material;
}

static Mesh load_mesh(const nlohmann::json &gltf, std::span<const uint8_t> bin,
                      const nlohmann::json &gltf_mesh) {
  Mesh mesh;
  size_t vertex_offset = 0;
  size_t index_offset = 0;
  for (auto &gltf_prim : gltf_mesh["primitives"]) {
    auto &submesh = mesh.submeshes.emplace_back(SubMesh{});
    if (gltf_prim.contains("material")) {
      submesh.material_index = gltf_prim["material"];
    }

    auto attributes = gltf_prim["attributes"];
    size_t vertex_count = 0;

    // position
    {
      int position_accessor_index = attributes["POSITION"];
      auto position = from_accessor<float3>(gltf, bin, position_accessor_index);
      vertex_count = position.size();
      mesh.vertices.resize(mesh.vertices.size() + position.size());
      for (size_t i = 0; i < position.size(); ++i) {
        mesh.vertices[vertex_offset + i].position = position[i];
      }
    }

    if (attributes.contains("TEXCOORD_0")) {
      int tex_accessor_index = attributes["TEXCOORD_0"];
      auto tex = from_accessor<float2>(gltf, bin, tex_accessor_index);
      assert(tex.size() == vertex_count);
      mesh.vertices.resize(mesh.vertices.size() + tex.size());
      for (size_t i = 0; i < tex.size(); ++i) {
        mesh.vertices[vertex_offset + i].tex0 = tex[i];
      }
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
          mesh.indices.push_back(static_cast<uint32_t>(vertex_offset + i));
        }
        submesh.offset = static_cast<uint32_t>(index_offset);
        submesh.draw_count = static_cast<uint32_t>(indices.size());
        break;
      }

      default:
        throw std::runtime_error("not implemented");
      }

      index_offset += submesh.draw_count;
    } else {
      throw std::runtime_error("not implemented");
    }

    vertex_offset += vertex_count;
  }
  return mesh;
}

bool GltfLoader::load() {
  auto gltf = nlohmann::json::parse(json);

  for (auto &gltf_texture : gltf["textures"]) {
    textures.emplace_back(load_texture(gltf, bin, gltf_texture));
  }

  for (auto &gltf_material : gltf["materials"]) {
    materials.emplace_back(load_material(gltf, bin, gltf_material));
  }

  for (auto &gltf_mesh : gltf["meshes"]) {
    meshes.emplace_back(load_mesh(gltf, bin, gltf_mesh));
  }

  return true;
}

} // namespace gorilla::gltf
