#pragma once
#include "bytereader.h"
#include "node.h"
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace banana::gltf {

struct GltfScene {
  std::vector<std::shared_ptr<Node>> nodes;
};

using MaterialWithState = std::tuple<std::shared_ptr<Material>, MaterialStates>;

using get_buffer_t = std::function<std::span<const uint8_t>(std::string_view uri)>;

struct GltfLoader {
  std::string json;
  get_buffer_t get_buffer;
  std::vector<std::shared_ptr<Image>> textures;
  std::vector<MaterialWithState> materials;
  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<std::shared_ptr<Node>> nodes;
  std::vector<GltfScene> scenes;
  std::shared_ptr<Node> root;

  GltfLoader() {}

  GltfLoader(std::string_view json, const get_buffer_t &get)
      : json(json), get_buffer(get) {}

  bool load();

  bool load_from_asset(std::string_view key);
};

} // namespace banana::gltf
