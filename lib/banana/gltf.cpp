#include "gltf.h"
#include <DirectXMath.h>
#include <array>
#include <fstream>
#include <nlohmann/json.hpp>

namespace banana::gltf {

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

static std::shared_ptr<Image> load_texture(const nlohmann::json &gltf,
                                           std::span<const uint8_t> bin,
                                           const nlohmann::json &gltf_texture) {

  if (!gltf_texture.contains("source")) {
    return {};
  }

  int gltf_image_index = gltf_texture["source"];
  auto gltf_image = gltf["images"][gltf_image_index];

  int bufferview_index = gltf_image["bufferView"];
  auto bytes = from_bufferview(gltf, bin, bufferview_index);

  auto image = std::make_shared<Image>();
  if (!image->load(bytes)) {
    assert(false);
    return {};
  }

  return image;
}

static std::shared_ptr<Material>
load_material(const nlohmann::json &gltf, std::span<const uint8_t> bin,
              const nlohmann::json &gltf_material,
              const std::vector<std::shared_ptr<Image>> &textures) {
  auto material = std::make_shared<Material>();
  material->shader_name = "gltf.hlsl";

  if (gltf_material.contains("pbrMetallicRoughness")) {
    auto pbrMetallicRoughness = gltf_material["pbrMetallicRoughness"];
    if (pbrMetallicRoughness.contains("baseColorTexture")) {
      auto baseColorTexture = pbrMetallicRoughness["baseColorTexture"];
      if (baseColorTexture.contains("index")) {
        int base_color_texture_index = baseColorTexture["index"];
        material->base_color_texture = textures[base_color_texture_index];
      }
    }
  }

  if (gltf_material.contains("doubleSided")) {
    if ((bool)gltf_material["doubleSided"]) {
      throw std::runtime_error("not implemented");
    }
  }

  return material;
}

static std::shared_ptr<Mesh>
load_mesh(const nlohmann::json &gltf, std::span<const uint8_t> bin,
          const nlohmann::json &gltf_mesh,
          const std::vector<std::shared_ptr<Material>> &materials) {
  auto mesh = std::make_shared<Mesh>();
  size_t vertex_offset = 0;
  size_t index_offset = 0;
  for (auto &gltf_prim : gltf_mesh["primitives"]) {
    auto &submesh = mesh->submeshes.emplace_back(SubMesh{});
    if (gltf_prim.contains("material")) {
      int material_index = gltf_prim["material"];
      submesh.material = materials[material_index];
    }

    auto attributes = gltf_prim["attributes"];
    size_t vertex_count = 0;

    // position
    {
      int position_accessor_index = attributes["POSITION"];
      auto position = from_accessor<Float3>(gltf, bin, position_accessor_index);
      vertex_count = position.size();
      mesh->vertices.resize(vertex_offset + position.size());
      for (size_t i = 0; i < position.size(); ++i) {
        mesh->vertices[vertex_offset + i].position = position[i];
      }
    }

    if (attributes.contains("TEXCOORD_0")) {
      int tex_accessor_index = attributes["TEXCOORD_0"];
      auto tex = from_accessor<Float2>(gltf, bin, tex_accessor_index);
      assert(tex.size() == vertex_count);
      mesh->vertices.resize(vertex_offset + tex.size());
      for (size_t i = 0; i < tex.size(); ++i) {
        mesh->vertices[vertex_offset + i].tex0 = tex[i];
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
        mesh->indices.reserve(index_offset + indices.size());
        for (auto &i : indices) {
          mesh->indices.push_back(static_cast<uint32_t>(vertex_offset + i));
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

template <size_t N, typename T> T load_float_array(const nlohmann::json &j) {
  std::array<float, N> values;
  static_assert(sizeof(values) == sizeof(T));
  int i = 0;
  for (auto &e : j) {
    values[i++] = e;
  }
  return *((T *)&values);
}

static std::shared_ptr<Node>
load_node(const nlohmann::json &gltf, std::span<const uint8_t> bin,
          const nlohmann::json &gltf_node,
          const std::vector<std::shared_ptr<Mesh>> &meshes) {

  auto node = std::make_shared<Node>();

  if (gltf_node.contains("name")) {
    node->name = gltf_node["name"];
  }

  if (gltf_node.contains("matrix")) {
    auto m = load_float_array<16, DirectX::XMFLOAT4X4>(gltf_node["matrix"]);
    auto M = DirectX::XMLoadFloat4x4(&m);
    DirectX::XMVECTOR T;
    DirectX::XMVECTOR R;
    DirectX::XMVECTOR S;
    if (DirectX::XMMatrixDecompose(&S, &R, &T, M)) {
      DirectX::XMStoreFloat3((DirectX::XMFLOAT3 *)&node->transform.translation,
                             T);
      DirectX::XMStoreFloat4((DirectX::XMFLOAT4 *)&node->transform.rotation, R);
      DirectX::XMStoreFloat3((DirectX::XMFLOAT3 *)&node->transform.scaling, S);
    } else {
      assert(false);
    }

  } else {
    // TRS
    if (gltf_node.contains("translation")) {
      node->transform.translation =
          load_float_array<3, Float3>(gltf_node["translation"]);
    }
    if (gltf_node.contains("rotation")) {
      node->transform.rotation =
          load_float_array<4, Float4>(gltf_node["rotation"]);
    }
    if (gltf_node.contains("scale")) {
      node->transform.scaling = load_float_array<3, Float3>(gltf_node["scale"]);
    }
  }

  if (gltf_node.contains("mesh")) {
    auto mesh_index = gltf_node["mesh"];
    node->mesh = meshes[mesh_index];
  }

  return node;
}

bool GltfLoader::load() {
  auto gltf = nlohmann::json::parse(json);

  for (auto &gltf_texture : gltf["textures"]) {
    textures.push_back(load_texture(gltf, bin, gltf_texture));
  }

  for (auto &gltf_material : gltf["materials"]) {
    materials.push_back(load_material(gltf, bin, gltf_material, textures));
  }

  for (auto &gltf_mesh : gltf["meshes"]) {
    meshes.push_back(load_mesh(gltf, bin, gltf_mesh, materials));
  }

  for (auto &gltf_node : gltf["nodes"]) {
    nodes.push_back(load_node(gltf, bin, gltf_node, meshes));
  }

  int i = 0;
  for (auto &gltf_node : gltf["nodes"]) {

    if (gltf_node.contains("children")) {
      for (int child_index : gltf_node["children"]) {
        auto child = nodes[child_index];
        auto self = nodes[i];
        self->add_child(child);
      }
    }

    ++i;
  }

  for (auto &gltf_scene : gltf["scenes"]) {
    auto &scene = scenes.emplace_back(GltfScene{});
    if (gltf_scene.contains("nodes")) {
      for (int node_index : gltf_scene["nodes"]) {
        auto node = nodes[node_index];
        scene.nodes.push_back(node);
      }
    }
  }

  return true;
}

} // namespace banana::gltf
