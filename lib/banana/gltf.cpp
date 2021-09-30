#include "gltf.h"
#include "asset.h"
#include "banana/material.h"
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

struct Vertex {
  Float3 position;
  Float3 normal;
  Float2 tex0;
  Float4 color;
  Float4 tangent;
};
using Index = uint32_t;
const size_t NORMAL_OFFSET = offsetof(Vertex, normal);
const size_t TEX0_OFFSET = offsetof(Vertex, tex0);
const size_t COLOR_OFFSET = offsetof(Vertex, color);
const size_t TANGENT_OFFSET = offsetof(Vertex, tangent);

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

static MaterialWithState create_default_material(std::string_view name) {
  auto material = std::make_shared<Material>();
  material->shader_name = "gltf.hlsl";
  material->properties.insert(std::make_pair(BASE_COLOR, Float4{1, 1, 1, 1}));
  material->properties.insert(std::make_pair(NORMAL_MAP_SCALE, 1.0f));
  return {material, MaterialStatesNone};
}

// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/material.schema.json
static MaterialWithState
load_material(const nlohmann::json &gltf, std::span<const uint8_t> bin,
              const nlohmann::json &gltf_material,
              const std::vector<std::shared_ptr<Image>> &textures) {

  auto [material, s] = create_default_material(gltf_material["name"]);

  if (gltf_material.contains("pbrMetallicRoughness")) {
    auto pbrMetallicRoughness = gltf_material["pbrMetallicRoughness"];
    if (pbrMetallicRoughness.contains("baseColorTexture")) {
      auto texture = pbrMetallicRoughness["baseColorTexture"];
      if (texture.contains("index")) {
        int texture_index = texture["index"];
        material->textures[TextureSemantics::Color] = textures[texture_index];
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
      material->textures[TextureSemantics::Normal] = textures[texture_index];
    }
    if (texture.contains("scale")) {
      material->properties[NORMAL_MAP_SCALE] = (float)texture["scale"];
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
  auto vertex_index = *(
      (uint32_t *)(data->mesh->indices.data() + i * data->mesh->index_stride));
  auto t =
      (Float4 *)(data->mesh->vertices.data() +
                 vertex_index * data->mesh->vertex_stride + TANGENT_OFFSET);
  t->x = fvTangent[0];
  t->y = fvTangent[1];
  t->z = fvTangent[2];
  t->w = fSign;
}

template <typename T>
size_t load_indices(const nlohmann::json &gltf, std::span<const uint8_t> bin,
                    const std::shared_ptr<Mesh> &mesh,
                    int indices_accessor_index, size_t vertex_offset,
                    size_t index_offset) {
  mesh->index_stride = sizeof(T);
  auto indices = from_accessor<T>(gltf, bin, indices_accessor_index);
  mesh->indices.resize((index_offset + indices.size()) * mesh->index_stride);
  auto dst = (T *)mesh->indices.data();
  size_t i = 0;
  for (auto &index : indices) {
    dst[index_offset + i++] = static_cast<T>(vertex_offset + index);
  }
  return indices.size();
}

static std::shared_ptr<Mesh>
load_mesh(const nlohmann::json &gltf, std::span<const uint8_t> bin,
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

    auto attributes = gltf_prim["attributes"];
    size_t vertex_count = 0;

    {
      // position
      int position_accessor_index = attributes["POSITION"];
      auto position = from_accessor<Float3>(gltf, bin, position_accessor_index);
      vertex_count = position.size();
      mesh->vertices.resize((vertex_offset + position.size()) * sizeof(Vertex));
      size_t i = 0;
      auto vertices = (Vertex *)mesh->vertices.data();
      for (auto &p : position) {
        vertices[vertex_offset + (i++)].position = p;
        mesh->aabb.expand(p);
      }
    }

    if (attributes.contains("TEXCOORD_0")) {
      // tex
      int tex_accessor_index = attributes["TEXCOORD_0"];
      auto tex = from_accessor<Float2>(gltf, bin, tex_accessor_index);
      assert(tex.size() == vertex_count);
      size_t i = 0;
      auto vertices = (Vertex *)mesh->vertices.data();
      for (auto &uv : tex) {
        vertices[vertex_offset + i++].tex0 = uv;
      }
    }

    auto has_tangent = attributes.contains("TANGENT");
    if (has_tangent) {
      // tangent
      int accessor_index = attributes["TANGENT"];
      auto tangents = from_accessor<Float4>(gltf, bin, accessor_index);
      assert(tangents.size() == vertex_count);
      size_t i = 0;
      auto vertices = (Vertex *)mesh->vertices.data();
      for (auto &tangent : tangents) {
        vertices[vertex_offset + i++].tangent = tangent;
      }
    }

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
        auto indices_size =
            load_indices<uint8_t>(gltf, bin, mesh, indices_accessor_index,
                                  vertex_offset, index_offset);
        submesh.draw_offset = static_cast<uint32_t>(index_offset);
        submesh.draw_count = static_cast<uint32_t>(indices_size);
        break;
      }

      case 5123: {
        // UNSIGNED_SHORT
        auto indices_size =
            load_indices<uint16_t>(gltf, bin, mesh, indices_accessor_index,
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
        submesh.material->textures.find(TextureSemantics::Normal) !=
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
