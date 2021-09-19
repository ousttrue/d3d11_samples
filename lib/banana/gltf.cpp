#include "gltf.h"
#include "asset.h"
#include "banana/types.h"
#include "glb.h"
#include <DirectXMath.h>
#include <array>
#include <fstream>
#include <mikktspace.h>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <utility>

namespace banana::gltf {

// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/

template <size_t N, typename T> T load_float_array(const nlohmann::json &j) {
  std::array<float, N> values;
  static_assert(sizeof(values) == sizeof(T));
  int i = 0;
  for (auto &e : j) {
    values[i++] = e;
  }
  return *((T *)&values);
}

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
create_default_material(std::string_view name) {
  auto material = std::make_shared<Material>();
  material->shader_name = "gltf.hlsl";
  material->properties.insert(std::make_pair(BASE_COLOR, Float4{1, 1, 1, 1}));
  material->properties.insert(std::make_pair(NORMAL_MAP_SCALE, 1.0f));
  return material;
}

// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/material.schema.json
static std::shared_ptr<Material>
load_material(const nlohmann::json &gltf, std::span<const uint8_t> bin,
              const nlohmann::json &gltf_material,
              const std::vector<std::shared_ptr<Image>> &textures) {

  auto material = create_default_material(gltf_material["name"]);

  if (gltf_material.contains("pbrMetallicRoughness")) {
    auto pbrMetallicRoughness = gltf_material["pbrMetallicRoughness"];
    if (pbrMetallicRoughness.contains("baseColorTexture")) {
      auto texture = pbrMetallicRoughness["baseColorTexture"];
      if (texture.contains("index")) {
        int texture_index = texture["index"];
        material->textures[BASE_COLOR_TEXTURE] = textures[texture_index];
      }
    }
    if (gltf_material.contains("baseColorFactor")) {
      material->properties[BASE_COLOR] =
          load_float_array<4, Float4>(gltf_material["baseColorFactor"]);
    }
  }

  if (gltf_material.contains("normalTexture")) {
    auto texture = gltf_material["normalTexture"];
    if (texture.contains("index")) {
      int texture_index = texture["index"];
      material->textures[NORMAL_MAP_TEXTURE] = textures[texture_index];
    }
    if (texture.contains("scale")) {
      material->properties[NORMAL_MAP_SCALE] = (float)texture["scale"];
    }
  }

  if (gltf_material.contains("doubleSided")) {
    if ((bool)gltf_material["doubleSided"]) {
      throw std::runtime_error("not implemented");
    }
  }

  return material;
}

struct TangentData {
  size_t index_offset;
  int index_count;
  Mesh *mesh;
};

// Returns the number of faces (triangles/quads) on the mesh to be processed.
static int getNumFaces(const SMikkTSpaceContext *pContext) {
  auto data = (TangentData *)pContext->m_pUserData;
  return data->index_count / 3;
}

// Returns the number of vertices on face number iFace
// iFace is a number in the range {0, 1, ..., getNumFaces()-1}
static int getNumVerticesOfFace(const SMikkTSpaceContext *pContext,
                                const int iFace) {
  return 3; // triangle
}

// returns the position/normal/texcoord of the referenced face of vertex number
// iVert. iVert is in the range {0,1,2} for triangles and {0,1,2,3} for quads.
static void getPosition(const SMikkTSpaceContext *pContext, float fvPosOut[],
                        const int iFace, const int iVert) {
  auto data = (TangentData *)pContext->m_pUserData;
  auto i = data->index_offset + iFace * 3 + iVert;
  auto vertex_index = data->mesh->indices[i];
  auto &v = data->mesh->vertices[vertex_index];
  fvPosOut[0] = v.position.x;
  fvPosOut[1] = v.position.y;
  fvPosOut[2] = v.position.z;
}

static void getNormal(const SMikkTSpaceContext *pContext, float fvNormOut[],
                      const int iFace, const int iVert) {
  auto data = (TangentData *)pContext->m_pUserData;
  auto i = data->index_offset + iFace * 3 + iVert;
  auto vertex_index = data->mesh->indices[i];
  auto &v = data->mesh->vertices[vertex_index];
  fvNormOut[0] = v.normal.x;
  fvNormOut[1] = v.normal.y;
  fvNormOut[2] = v.normal.z;
}

static void getTexCoord(const SMikkTSpaceContext *pContext, float fvTexcOut[],
                        const int iFace, const int iVert) {
  auto data = (TangentData *)pContext->m_pUserData;
  auto i = data->index_offset + iFace * 3 + iVert;
  auto vertex_index = data->mesh->indices[i];
  auto &v = data->mesh->vertices[vertex_index];
  fvTexcOut[0] = v.tex0.x;
  fvTexcOut[1] = v.tex0.y;
}

// either (or both) of the two setTSpace callbacks can be set.
// The call-back m_setTSpaceBasic() is sufficient for basic normal mapping.

// This function is used to return the tangent and fSign to the application.
// fvTangent is a unit length vector.
// For normal maps it is sufficient to use the following simplified version of
// the bitangent which is generated at pixel/vertex level. bitangent = fSign *
// cross(vN, tangent); Note that the results are returned unindexed. It is
// possible to generate a new index list But averaging/overwriting tangent
// spaces by using an already existing index list WILL produce INCRORRECT
// results. DO NOT! use an already existing index list.
static void setTSpaceBasic(const SMikkTSpaceContext *pContext,
                           const float fvTangent[], const float fSign,
                           const int iFace, const int iVert) {
  auto data = (TangentData *)pContext->m_pUserData;
  auto i = data->index_offset + iFace * 3 + iVert;
  auto vertex_index = data->mesh->indices[i];
  auto &v = data->mesh->vertices[vertex_index];
  v.tangent.x = fvTangent[0];
  v.tangent.y = fvTangent[1];
  v.tangent.z = fvTangent[2];
  v.tangent.w = fSign;
}

static std::shared_ptr<Mesh>
load_mesh(const nlohmann::json &gltf, std::span<const uint8_t> bin,
          const nlohmann::json &gltf_mesh,
          const std::vector<std::shared_ptr<Material>> &materials) {
  auto mesh = std::make_shared<Mesh>();
  size_t vertex_offset = 0;
  size_t index_offset = 0;
  for (auto &gltf_prim : gltf_mesh["primitives"]) {
    //
    // concat primitives
    //
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
      size_t i = 0;
      for (auto &p : position) {
        mesh->vertices[vertex_offset + (i++)].position = p;
        mesh->aabb.expand(p);
      }
    }

    // tex
    if (attributes.contains("TEXCOORD_0")) {
      int tex_accessor_index = attributes["TEXCOORD_0"];
      auto tex = from_accessor<Float2>(gltf, bin, tex_accessor_index);
      assert(tex.size() == vertex_count);
      mesh->vertices.resize(vertex_offset + tex.size());
      size_t i = 0;
      for (auto &uv : tex) {
        mesh->vertices[vertex_offset + i++].tex0 = uv;
      }
    }

    auto has_tangent = attributes.contains("TANGENT");
    if (has_tangent) {
      throw std::runtime_error("not implemented");
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
        submesh.draw_offset = static_cast<uint32_t>(index_offset);
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

    if (submesh.material &&
        submesh.material->textures.find("normal_map_texture") !=
            submesh.material->textures.end() &&
        !has_tangent) {
      // calc tangent
      SMikkTSpaceInterface interface = {0};
      interface.m_getNumFaces = &getNumFaces;
      interface.m_getNumVerticesOfFace = &getNumVerticesOfFace;
      interface.m_getPosition = &getPosition;
      interface.m_getNormal = &getNormal;
      interface.m_getTexCoord = &getTexCoord;
      interface.m_setTSpaceBasic = &setTSpaceBasic;

      SMikkTSpaceContext context = {0};
      context.m_pInterface = &interface;

      TangentData user_data;
      user_data.index_count = submesh.draw_count;
      user_data.index_offset = index_offset - submesh.draw_count;
      user_data.mesh = mesh.get();
      context.m_pUserData = &user_data;

      genTangSpaceDefault(&context);
    }
  }
  return mesh;
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

bool GltfLoader::load_from_asset(std::string_view key) {
  auto bytes = banana::get_bytes(key);
  if (bytes.empty()) {
    return false;
  }

  Glb glb;
  if (!glb.parse(bytes)) {
    return false;
  }

  this->json = glb.json;
  this->bin = {glb.bin.begin(), glb.bin.end()};

  return load();
}

} // namespace banana::gltf
