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

struct GltfLoader {
  std::string json;
  std::vector<uint8_t> bin;
  std::vector<std::shared_ptr<Image>> textures;
  std::vector<std::shared_ptr<Material>> materials;
  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<std::shared_ptr<Node>> nodes;
  std::vector<GltfScene> scenes;

  GltfLoader(std::string_view json, std::span<const uint8_t> bin)
      : json(json), bin(bin.begin(), bin.end()) {}

  bool load();
};

} // namespace banana::gltf
