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

enum class Float4Semantics {
  MATERIAL_BASECOLOR,
  CAMERA_POSITION,
};

enum class Float4x3Semantics {
  NODE_NORMAL, // NODE_WORLD_INVERSE_TRANSPOSE
};

enum class Float4x4Semantics {
  NODE_WORLD,
  CAMERA_VIEW,
  CAMERA_PROJECTION,
};

enum class TextureSemantics {
  MATERIAL_COLOR,
  MATERIAL_NORMAL,
};

inline std::string_view semantic_srv_name(TextureSemantics semantic) {
  switch (semantic) {
  case TextureSemantics::MATERIAL_COLOR:
    return "BaseColorTexture";
  case TextureSemantics::MATERIAL_NORMAL:
    return "NormalMapTexture";
  }
  return {};
}
inline std::string_view semantic_sampler_name(TextureSemantics semantic) {
  switch (semantic) {
  case TextureSemantics::MATERIAL_COLOR:
    return "BaseColorSampler";
  case TextureSemantics::MATERIAL_NORMAL:
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
