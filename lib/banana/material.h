#pragma once
#include "image.h"
#include "types.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace banana {

const auto BASE_COLOR = "BaseColor";
const auto BASE_COLOR_TEXTURE = "BaseColor";
const auto NORMAL_MAP_TEXTURE = "NormalMap";
const auto NORMAL_MAP_SCALE = "NormalMapScale";

struct Material {
  std::string name;
  std::string shader_name;

  // Use same name in shader variables
  std::unordered_map<std::string, Variable> properties;

  // Use same name SRV and Sampler.
  // SRV suffix is "Texture"
  // Sampler suffix is "Sampler"
  // ex. BaseColorTexture and BaseColorSampler
  std::unordered_map<std::string, std::shared_ptr<Image>> textures;
};

} // namespace banana
