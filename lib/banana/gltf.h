#pragma once
#include "bytereader.h"
#include "node.h"
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace banana::gltf {

struct GltfScene {
  std::vector<std::shared_ptr<Node>> nodes;
};

using MaterialWithState = std::tuple<std::shared_ptr<Material>, MaterialStates>;

struct GltfLoader {
  std::string json;
  std::vector<uint8_t> bin;
  std::vector<std::shared_ptr<Image>> textures;
  std::vector<MaterialWithState> materials;
  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<std::shared_ptr<Node>> nodes;
  std::vector<GltfScene> scenes;
  std::shared_ptr<Node> root;
  
  GltfLoader(){}

  GltfLoader(std::string_view json, std::span<const uint8_t> bin)
      : json(json), bin(bin.begin(), bin.end()) {}

  bool load();

  bool load_from_asset(std::string_view key);
};

} // namespace banana::gltf
