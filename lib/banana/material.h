#pragma once
#include "image.h"
#include "types.h"
#include <memory>
#include <string>

namespace banana {

struct Material {
  std::string shader_name;

  Float4 base_color = {1, 1, 1, 1};
  std::shared_ptr<Image> base_color_texture;

  std::shared_ptr<Image> normal_map;
  float normal_map_scale = 1.0f;
};

} // namespace banana
