#pragma once
#include <limits>
#include <variant>

namespace banana {

struct Float2 {
  float x;
  float y;
};

struct Float3 {
  float x;
  float y;
  float z;
};

struct Float4 {
  float x;
  float y;
  float z;
  float w;
};

struct Matrix3x4 {
  float _11, _12, _13, _14;
  float _21, _22, _23, _24;
  float _31, _32, _33, _34;
};

struct Matrix4x4 {
  float _11, _12, _13, _14;
  float _21, _22, _23, _24;
  float _31, _32, _33, _34;
  float _41, _42, _43, _44;

  static Matrix4x4 identity();
  Matrix4x4 operator*(const Matrix4x4 &rhs) const;
  Float3 apply(const Float3 &src) const;
};

struct AABB {
  Float3 min = {std::numeric_limits<float>::infinity(),
                std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity()};
  Float3 max = {-std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity()};

  float height() const { return max.y - min.y; }

  void expand(const Float3 &p) {
    if (p.x < min.x) {
      min.x = p.x;
    }
    if (p.y < min.y) {
      min.y = p.y;
    }
    if (p.z < min.z) {
      min.z = p.z;
    }

    if (p.x > max.x) {
      max.x = p.x;
    }
    if (p.y > max.y) {
      max.y = p.y;
    }
    if (p.z > max.z) {
      max.z = p.z;
    }
  }
};

struct LightInfo {
  Float3 position;
  float is_point;
  Float3 intensity;
  float _padding1;
};
static_assert(sizeof(LightInfo) == 32);

using Variable = std::variant<float, Float2, Float3, Float4, Matrix4x4,
                              Matrix3x4, LightInfo>;

} // namespace banana
