#pragma once
#include "types.h"
#include <DirectXMath.h>
#include <limits>

namespace banana {

struct OrbitCamera {
  //
  // view
  //
  Float3 gaze = {0, 0, 0};
  Float3 translation = {0, 0, -5};
  float yaw = 0;
  float pitch = 0;
  Matrix4x4 view;
  void calc_view();
  Matrix3x4 normal_matrix() const;
  DirectX::XMFLOAT3 position() const;
  DirectX::XMFLOAT4 rotation() const;
  void yaw_pitch(float dx, float dy);
  void shift(float dx, float dy);
  void dolly(float d);

  //
  // screen | mouse
  //
  Float2 screen = {1, 1};
  bool _left = false;
  bool _right = false;
  bool _middle = false;
  // float _x = std::numeric_limits<float>::quiet_NaN();
  // float _y = std::numeric_limits<float>::quiet_NaN();
  DirectX::XMFLOAT3 get_ray_direction(float x, float y) const;

  //
  // projection
  //
  float _near = 0.01f;
  float _far = 100.0f;
  float fovYRad = DirectX::XMConvertToRadians(60.0f);
  Matrix4x4 projection;
  void calc_projection();
  void resize(float w, float h);

  OrbitCamera();
  void fit(const AABB &aabb);
  void update(float x, float y, float w, float h, bool left, bool right,
              bool middle, float wheel);
};

} // namespace banana
