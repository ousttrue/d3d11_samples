#pragma once
#include "image.h"
#include "types.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace banana {

const auto BASE_COLOR = "BaseColor";
const auto NORMAL_MAP_SCALE = "NormalMapScale";

enum MaterialStates {
  MaterialStatesNone = 0,
  MaterialStatesDoubleFace = 1,
  MaterialStatesAlphaBlend = 1 << 1,
  MaterialStatesMask = 2 << 1,
};

enum class TextureSemantics {
  None = 0,
  Color = 1 << 0,
  Normal = 1 << 1,
};
inline std::string_view semantic_srv_name(TextureSemantics semantic) {
  switch (semantic) {
  case TextureSemantics::None:
    break;
  case TextureSemantics::Color:
    return "BaseColorTexture";
  case TextureSemantics::Normal:
    return "NormalMapTexture";
  }
  return {};
}
inline std::string_view semantic_sampler_name(TextureSemantics semantic) {
  switch (semantic) {
  case TextureSemantics::None:
    break;
  case TextureSemantics::Color:
    return "BaseColorSampler";
  case TextureSemantics::Normal:
    return "NormalMapSampler";
  }
  return {};
}

struct Material {
  std::string name;
  std::string shader_name;

  // Use same name in shader variables
  std::unordered_map<std::string, Variable> properties;

  // Use same name SRV and Sampler.
  // SRV suffix is "Texture"
  // Sampler suffix is "Sampler"
  // ex. BaseColorTexture and BaseColorSampler
  std::unordered_map<TextureSemantics, std::shared_ptr<Image>> textures;
};

} // namespace banana
