#include "geometry.h"

namespace banana::geometry {

std::shared_ptr<Mesh> create_cube(float size) {
  auto mesh = std::make_shared<Mesh>();

  auto left = Float3(-1, 0, 0);
  auto right = Float3(1, 0, 0);
  auto bottom = Float3(0, -1, 0);
  auto top = Float3(0, 1, 0);
  auto forward = Float3(0, 0, 1);
  auto back = Float3(0, 0, -1);

  auto red = Float4(1, 0, 0, 1);
  auto green = Float4(0, 1, 0, 1);
  auto blue = Float4(0, 0, 1, 1);
  auto cyan = Float4(0, 1, 1, 1);
  auto magenta = Float4(1, 0, 1, 1);
  auto yellow = Float4(1, 1, 0, 1);

  mesh->vertices = {
      // x
      {Float3(-size, -size, -size), left, Float2(0, 1), cyan},
      {Float3(-size, -size, size), left, Float2(0, 0), cyan},
      {Float3(-size, size, size), left, Float2(1, 0), cyan},
      {Float3(-size, size, -size), left, Float2(1, 1), cyan},

      {Float3(size, -size, -size), right, Float2(0, 1), red},
      {Float3(size, size, -size), right, Float2(1, 1), red},
      {Float3(size, size, size), right, Float2(1, 0), red},
      {Float3(size, -size, size), right, Float2(0, 0), red},
      // y
      {Float3(-size, -size, -size), bottom, Float2(0, 1), magenta},
      {Float3(size, -size, -size), bottom, Float2(1, 1), magenta},
      {Float3(size, -size, size), bottom, Float2(1, 0), magenta},
      {Float3(-size, -size, size), bottom, Float2(0, 0), magenta},

      {Float3(-size, size, -size), top, Float2(0, 1), green},
      {Float3(-size, size, size), top, Float2(0, 0), green},
      {Float3(size, size, size), top, Float2(1, 0), green},
      {Float3(size, size, -size), top, Float2(1, 1), green},
      // z
      {Float3(-size, -size, size), forward, Float2(0, 1), yellow},
      {Float3(size, -size, size), forward, Float2(1, 1), yellow},
      {Float3(size, size, size), forward, Float2(1, 0), yellow},
      {Float3(-size, size, size), forward, Float2(0, 0), yellow},

      {Float3(-size, -size, -size), back, Float2(0, 1), blue},
      {Float3(-size, size, -size), back, Float2(0, 0), blue},
      {Float3(size, size, -size), back, Float2(1, 0), blue},
      {Float3(size, -size, -size), back, Float2(1, 1), blue},
  };
  mesh->indices = {
      0,  1,  2,  2,  3,  0,  // 0
      4,  5,  6,  6,  7,  4,  // 1
      8,  9,  10, 10, 11, 8,  // 2
      12, 13, 14, 14, 15, 12, // 3
      16, 17, 18, 18, 19, 16, // 4
      20, 21, 22, 22, 23, 20, // 5
  };
  return mesh;
}

} // namespace banana::geometry
