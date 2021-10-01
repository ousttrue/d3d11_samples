#pragma once
#include <magic_enum.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

namespace banana {

enum class Semantics {
  // system value
  SV_POSITION,
  SV_TARGET,
  SV_DEPTH,
  // predefine
  NORMAL,
  TEXCOORD,
  //
  CAMERA_POSITION,
  CAMERA_VIEW,
  CAMERA_PROJECTION,
  CAMERA_NEAR_FAR_FOVY,
  OBJECT_NORMAL, // NODE_WORLD_INVERSE_TRANSPOSE
  OBJECT_WORLD,
  MATERIAL_COLOR,
  MATERIAL_NORMAL,
  CURSOR_SCREEN_SIZE, // mouse[x, y], screen_size[w, h]: left-top origin, pixel unit
};

inline Semantics semantics_from_string(std::string_view src) {
  auto opt = magic_enum::enum_cast<Semantics>(src);
  if (opt.has_value()) {
    return opt.value();
  }

  throw std::runtime_error(std::string("unknown semantic: ") +
                           std::string(src));
}

} // namespace banana
