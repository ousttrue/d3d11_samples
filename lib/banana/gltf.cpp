// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/
#include "gltf.h"
#include "asset.h"
#include "banana/material.h"
#include "banana/types.h"
#include "glb.h"
#include <DirectXMath.h>
#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <mikktspace.h>
#include <nlohmann/json.hpp>
#include <optional>
#include <stdexcept>
#include <utility>

namespace banana::gltf {

#define DefAttribute(attr) constexpr auto attr = #attr

DefAttribute(POSITION);
DefAttribute(NORMAL);
DefAttribute(TANGENT);
DefAttribute(TEXCOORD_0);
DefAttribute(COLOR_0);

// same as assets/gltf.hlsl
struct Vertex {
  Float3 position;
  Float2 tex0;
  Float3 normal;
  Float4 tangent;
  Float4 color0;
};
using Index = uint32_t;
const size_t NORMAL_OFFSET = offsetof(Vertex, normal);
const size_t TEX0_OFFSET = offsetof(Vertex, tex0);
const size_t TANGENT_OFFSET = offsetof(Vertex, tangent);
const size_t COLOR_OFFSET = offsetof(Vertex, color0);

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
                                                const get_buffer_t &get_buffer,
                                                int bufferview_index) {
  auto bufferview = gltf["bufferViews"][bufferview_index];

  int buffer_index = bufferview["buffer"];

  auto buffer = gltf["buffers"][buffer_index];

  std::span<const uint8_t> bytes;
  if (buffer.contains("uri")) {
    bytes = get_buffer(buffer["uri"]);
  } else {
    bytes = get_buffer({});
  }

  auto byteOffset = 0;
  if (bufferview.contains("byteOffset")) {
    byteOffset = bufferview["byteOffset"];
  }

  int byteLength = bufferview["byteLength"];

  return bytes.subspan(byteOffset, byteLength);
}

template <typename T>
static std::span<const T> from_accessor(const nlohmann::json &gltf,
                                        const get_buffer_t &get_buffer,
                                        int accessor_index) {
  auto accessor = gltf["accessors"][accessor_index];

  auto bufferview_bytes =
      from_bufferview(gltf, get_buffer, accessor["bufferView"]);

  auto byteOffset = 0;
  if (accessor.contains("byteOffset")) {
    byteOffset = accessor["byteOffset"];
  }

  std::span<const T> span((const T *)(bufferview_bytes.data() + byteOffset),
                          (size_t)accessor["count"]);
  return span;
}

static std::shared_ptr<Image> load_texture(const nlohmann::json &gltf,
                                           const get_buffer_t &get_buffer,
                                           const nlohmann::json &gltf_texture) {

  if (!gltf_texture.contains("source")) {
    return {};
  }

  int gltf_image_index = gltf_texture["source"];
  auto gltf_image = gltf["images"][gltf_image_index];

  std::span<const uint8_t> bytes;
  if (gltf_image.contains("uri")) {
    bytes = get_buffer(gltf_image["uri"]);
  } else {
    int bufferview_index = gltf_image["bufferView"];
    bytes = from_bufferview(gltf, get_buffer, bufferview_index);
  }

  auto image = std::make_shared<Image>();
  if (!image->load(bytes)) {
    assert(false);
    return {};
  }

  return image;
}

static MaterialWithState create_default_material(std::string_view name) {
  auto material = std::make_shared<Material>();
  material->shader_name = "gltf.hlsl";
  material->properties.insert(
      std::make_pair(Semantics::MATERIAL_COLOR, Float4{1, 1, 1, 1}));
  material->properties.insert(
      std::make_pair(Semantics::MATERIAL_NORMAL_SCALE, 1.0f));
  return {material, MaterialStatesNone};
}

// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/material.schema.json
static MaterialWithState
load_material(const nlohmann::json &gltf, const nlohmann::json &gltf_material,
              const std::vector<std::shared_ptr<Image>> &textures,
              std::string name) {
  if (gltf_material.contains("name")) {
    name = gltf_material["name"];
  }
  auto [material, s] = create_default_material(name);

  if (gltf_material.contains("pbrMetallicRoughness")) {
    auto pbrMetallicRoughness = gltf_material["pbrMetallicRoughness"];
    if (pbrMetallicRoughness.contains("baseColorTexture")) {
      auto texture = pbrMetallicRoughness["baseColorTexture"];
      if (texture.contains("index")) {
        int texture_index = texture["index"];
        material->textures[Semantics::MATERIAL_COLOR] = textures[texture_index];
      }
    }
    if (gltf_material.contains("baseColorFactor")) {
      material->properties[Semantics::MATERIAL_COLOR] =
          load_float_array<4, Float4>(gltf_material["baseColorFactor"]);
    }
  }

  if (gltf_material.contains("normalTexture")) {
    auto texture = gltf_material["normalTexture"];
    if (texture.contains("index")) {
      int texture_index = texture["index"];
      material->textures[Semantics::MATERIAL_NORMAL] = textures[texture_index];
    }
    if (texture.contains("scale")) {
      material->properties[Semantics::MATERIAL_NORMAL_SCALE] =
          (float)texture["scale"];
    }
  }

  if (gltf_material.contains("alphaMode")) {
    std::string alphaMode = gltf_material["alphaMode"];
    if (alphaMode == "OPAQUE") {
    } else if (alphaMode == "MASK") {
      s = (MaterialStates)(s | MaterialStatesMask);
    } else if (alphaMode == "BLEND") {
      s = (MaterialStates)(s | MaterialStatesAlphaBlend);
    }
  }

  if (gltf_material.contains("doubleSided")) {
    if ((bool)gltf_material["doubleSided"]) {
      s = (MaterialStates)(s | MaterialStatesDoubleFace);
    }
  }

  return {material, s};
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
  auto v = (Float3 *)(data->mesh->vertices.data() +
                      vertex_index * data->mesh->vertex_stride);
  fvPosOut[0] = v->x;
  fvPosOut[1] = v->y;
  fvPosOut[2] = v->z;
}

static void getNormal(const SMikkTSpaceContext *pContext, float fvNormOut[],
                      const int iFace, const int iVert) {
  auto data = (TangentData *)pContext->m_pUserData;
  auto i = data->index_offset + iFace * 3 + iVert;
  auto vertex_index = data->mesh->indices[i];
  auto n = (Float3 *)(data->mesh->vertices.data() +
                      vertex_index * data->mesh->vertex_stride + NORMAL_OFFSET);
  fvNormOut[0] = n->x;
  fvNormOut[1] = n->y;
  fvNormOut[2] = n->z;
}

static void getTexCoord(const SMikkTSpaceContext *pContext, float fvTexcOut[],
                        const int iFace, const int iVert) {
  auto data = (TangentData *)pContext->m_pUserData;
  auto i = data->index_offset + iFace * 3 + iVert;
  auto vertex_index = data->mesh->indices[i];
  auto uv = (Float2 *)(data->mesh->vertices.data() +
                       vertex_index * data->mesh->vertex_stride + TEX0_OFFSET);
  fvTexcOut[0] = uv->x;
  fvTexcOut[1] = uv->y;
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

  Float4 *t;
  switch (data->mesh->index_stride) {
  case 2: {
    auto vertex_index = *((uint16_t *)(data->mesh->indices.data() +
                                       i * data->mesh->index_stride));
    t = (Float4 *)(data->mesh->vertices.data() +
                   vertex_index * data->mesh->vertex_stride + TANGENT_OFFSET);
    break;
  }

  case 4: {
    auto vertex_index = *((uint32_t *)(data->mesh->indices.data() +
                                       i * data->mesh->index_stride));
    t = (Float4 *)(data->mesh->vertices.data() +
                   vertex_index * data->mesh->vertex_stride + TANGENT_OFFSET);
    break;
  }

  default:
    throw std::runtime_error("not implemented");
  }

  t->x = fvTangent[0];
  t->y = fvTangent[1];
  t->z = fvTangent[2];
  t->w = fSign;
}

template <typename T>
size_t load_indices(const nlohmann::json &gltf, const get_buffer_t &get_buffer,
                    const std::shared_ptr<Mesh> &mesh,
                    int indices_accessor_index, size_t vertex_offset,
                    size_t index_offset) {
  mesh->index_stride = sizeof(T);
  auto indices = from_accessor<T>(gltf, get_buffer, indices_accessor_index);
  mesh->indices.resize((index_offset + indices.size()) * mesh->index_stride);
  auto dst = (T *)mesh->indices.data();
  size_t i = 0;
  for (auto &index : indices) {
    dst[index_offset + i++] = static_cast<T>(vertex_offset + index);
  }
  return indices.size();
}

template <typename T>
std::optional<std::span<const T>>
get_attribute(const nlohmann::json &gltf, const get_buffer_t &get_buffer,
              const nlohmann::json &gltf_prim, const char *key) {
  auto attributes = gltf_prim["attributes"];
  if (!attributes.contains(key)) {
    return {};
  }

  int accessor_index = attributes[key];
  auto values = from_accessor<T>(gltf, get_buffer, accessor_index);
  return values;
}

static std::shared_ptr<Mesh>
load_mesh(const nlohmann::json &gltf, const get_buffer_t &get_buffer,
          const nlohmann::json &gltf_mesh,
          const std::vector<MaterialWithState> &materials) {
  auto mesh = std::make_shared<Mesh>();
  mesh->vertex_stride = sizeof(Vertex);
  size_t vertex_offset = 0;
  size_t index_offset = 0;
  for (auto &gltf_prim : gltf_mesh["primitives"]) {
    //
    // concat primitives
    //
    auto &submesh = mesh->submeshes.emplace_back(SubMesh{});
    if (gltf_prim.contains("material")) {
      int material_index = gltf_prim["material"];
      auto [material, s] = materials[material_index];
      submesh.material = material;
      submesh.state = s;
    }

    auto positions =
        get_attribute<Float3>(gltf, get_buffer, gltf_prim, POSITION);
    size_t vertex_count = 0;
    if (positions.has_value()) {
      vertex_count = positions->size();
      mesh->vertices.resize((vertex_offset + positions->size()) *
                            sizeof(Vertex));
      size_t i = 0;
      auto vertices = (Vertex *)mesh->vertices.data();
      for (auto &p : *positions) {
        vertices[vertex_offset + (i++)].position = p;
        mesh->aabb.expand(p);
      }
    } else {
      throw std::runtime_error("no POSITION");
    }

    {
      auto tex = get_attribute<Float2>(gltf, get_buffer, gltf_prim, TEXCOORD_0);
      if (tex.has_value()) {
        assert(tex->size() == vertex_count);
        size_t i = 0;
        auto vertices = (Vertex *)mesh->vertices.data();
        for (auto &uv : *tex) {
          vertices[vertex_offset + i++].tex0 = uv;
        }
      }
    }

    {
      auto normal = get_attribute<Float3>(gltf, get_buffer, gltf_prim, NORMAL);
      if (normal.has_value()) {
        assert(normal->size() == vertex_count);
        size_t i = 0;
        auto vertices = (Vertex *)mesh->vertices.data();
        for (auto &n : *normal) {
          vertices[vertex_offset + i++].normal = n;
        }
      }
    }

    bool has_tangent = false;
    {
      auto tangent =
          get_attribute<Float4>(gltf, get_buffer, gltf_prim, TANGENT);
      if (tangent.has_value()) {
        has_tangent = true;
        assert(tangent->size() == vertex_count);
        size_t i = 0;
        auto vertices = (Vertex *)mesh->vertices.data();
        for (auto &t : *tangent) {
          vertices[vertex_offset + i++].tangent = t;
        }
      }
    }

    // TODO Color
    // TODO Joint
    // TODO Weight

    if (gltf_prim.contains("indices")) {
      // indices
      int indices_accessor_index = gltf_prim["indices"];
      auto indices_accessor = gltf["accessors"][indices_accessor_index];
      int indices_type = indices_accessor["componentType"];
      switch (indices_type) {
      case 5120: // BYTE
        throw std::runtime_error("BYTE not implemented");
      case 5121: {
        // UNSIGNED_BYTE
        auto indices_size = load_indices<uint8_t>(gltf, get_buffer, mesh,
                                                  indices_accessor_index,
                                                  vertex_offset, index_offset);
        submesh.draw_offset = static_cast<uint32_t>(index_offset);
        submesh.draw_count = static_cast<uint32_t>(indices_size);
        break;
      }

      case 5123: {
        // UNSIGNED_SHORT
        auto indices_size = load_indices<uint16_t>(gltf, get_buffer, mesh,
                                                   indices_accessor_index,
                                                   vertex_offset, index_offset);
        submesh.draw_offset = static_cast<uint32_t>(index_offset);
        submesh.draw_count = static_cast<uint32_t>(indices_size);
        break;
      }

      default:
        throw std::runtime_error("UNKNOWN indices type not implemented");
      }

      index_offset += submesh.draw_count;
    } else {
      throw std::runtime_error("no indices");
    }

    vertex_offset += vertex_count;

    if (submesh.material &&
        submesh.material->textures.find(Semantics::MATERIAL_NORMAL) !=
            submesh.material->textures.end() &&
        !has_tangent) {
      // calc tangent
      SMikkTSpaceInterface iMikk = {0};
      iMikk.m_getNumFaces = &getNumFaces;
      iMikk.m_getNumVerticesOfFace = &getNumVerticesOfFace;
      iMikk.m_getPosition = &getPosition;
      iMikk.m_getNormal = &getNormal;
      iMikk.m_getTexCoord = &getTexCoord;
      iMikk.m_setTSpaceBasic = &setTSpaceBasic;

      SMikkTSpaceContext context = {0};
      context.m_pInterface = &iMikk;

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
load_node(const nlohmann::json &gltf, const nlohmann::json &gltf_node,
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
    textures.push_back(load_texture(gltf, get_buffer, gltf_texture));
  }

  {
    size_t i = 0;
    for (auto &gltf_material : gltf["materials"]) {
      materials.push_back(load_material(gltf, gltf_material, textures,
                                        std::format("material{}", i++)));
    }
  }

  for (auto &gltf_mesh : gltf["meshes"]) {
    meshes.push_back(load_mesh(gltf, get_buffer, gltf_mesh, materials));
  }

  for (auto &gltf_node : gltf["nodes"]) {
    nodes.push_back(load_node(gltf, gltf_node, meshes));
  }

  {
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

  if (scenes.size() == 1 && scenes[0].nodes.size() == 1) {
    root = scenes[0].nodes[0];
  } else {
    root = std::make_shared<Node>();
    for (auto &scene : scenes) {
      for (auto &node : scene.nodes) {
        root->add_child(node);
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

  // try glb
  Glb glb;
  if (glb.parse(bytes)) {
    this->json = glb.json;

    this->get_buffer = [bin = glb.bin](auto _) { return bin; };
    return load();
  }

  // try gltf
  this->json = std::string(bytes.begin(), bytes.end());
  std::filesystem::path path(key);
  auto dir = path.parent_path();
  this->get_buffer = [dir](std::string_view uri) {
    return get_bytes((dir / uri).generic_string());
  };
  return load();
}

} // namespace banana::gltf
