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
  POSITION,
  NORMAL,
  NORMAL0,
  NORMAL1,
  TEXCOORD,
  TEXCOORD0,
  COLOR0,
  TANGENT,
  //
  CAMERA_POSITION,
  CAMERA_VIEW,
  CAMERA_PROJECTION,
  CAMERA_NEAR_FAR_FOVY,
  NORMAL_MATRIX, // NODE_WORLD_INVERSE_TRANSPOSE
  OBJECT_WORLD,
  WORLDVIEW,             // OBJECT_WORLD X CAMERA_VIEW
  VIEWPROJECTION,        // CAMERA_VIEW X CAMERA_PROJECTION
  WORLDVIEWPROJECTION,   // OBJECT_WORLD X CAMERA_VIEW X CAMERA_PROJECTION
  MATERIAL_COLOR,        // rgba value or texture or sampler
  MATERIAL_AMBIENT,      // rgb value
  MATERIAL_SPECULAR,     // rgb value + shininess
  MATERIAL_NORMAL,       // texture or sampler
  MATERIAL_NORMAL_SCALE, // float
  CURSOR_SCREEN_SIZE, // mouse[x, y], screen_size[w, h]: left-top origin, pixel
                      // unit
  //
  LIGHT_LIST, // LightInfo[5]
};

inline Semantics semantics_from_string(std::string_view src) {
  if (src.empty()) {
    throw std::runtime_error("no semantic");
  }

  auto opt = magic_enum::enum_cast<Semantics>(src);
  if (opt.has_value()) {
    return opt.value();
  }

  throw std::runtime_error(std::string("unknown semantic: ") +
                           std::string(src));
}

} // namespace banana
