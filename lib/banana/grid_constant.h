#pragma once
#include "orbit_camera.h"
#include <array>

namespace banana {

#pragma pack(push)
#pragma pack(16)
struct GridConstant {
  std::array<float, 16> view;
  std::array<float, 16> projection;
  std::array<float, 3> cameraPosition;
  float _padding2;
  std::array<float, 2> screenSize;
  float fovY;
  float _padding3;

  void update(const OrbitCamera &camera) {
    fovY = camera.fovYRad;
    screenSize = {camera.screen.x, camera.screen.y};
    view = *((std::array<float, 16> *)&camera.view);
    projection = *((std::array<float, 16> *)&camera.projection);
    auto p = camera.position();
    cameraPosition = {p.x, p.y, p.z};
  }
};
#pragma pack(pop)
static_assert(sizeof(GridConstant) == 40 * 4, "sizeof ConstantsSize");

} // namespace  banana
