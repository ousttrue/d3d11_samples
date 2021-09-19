#pragma once
#include "types.h"
#include <DirectXMath.h>
#include <limits>

namespace banana {

struct OrbitCamera {
  Float2 screen = {1, 1};
  float near = 0.01f;
  float far = 100.0f;
  float fovYRad = DirectX::XMConvertToRadians(60.0f);
  Matrix4x4 projection;
  void calc_projection();

  Float3 translation = {0, 0, -5};
  float yaw = 0;
  float pitch = 0;
  Matrix4x4 view;
  void calc_view();

  Matrix3x4 normal_matrix() const;
  DirectX::XMFLOAT3 position() const;
  OrbitCamera();
  void yaw_pitch(int dx, int dy);
  void shift(int dx, int dy);
  void dolly(int d);
  void resize(float w, float h);
  void fit(float y, float half_height);
};

struct MouseBinder {
  bool _left = false;
  bool _right = false;
  bool _middle = false;
  int _x = std::numeric_limits<int>::min();
  int _y = std::numeric_limits<int>::min();
  OrbitCamera &_camera;

public:
  MouseBinder(banana::OrbitCamera &camera) : _camera(camera) {}
  void Left(bool isPress) { _left = isPress; }
  void Middle(bool isPress) { _middle = isPress; }
  void Right(bool isPress) { _right = isPress; }
  void Move(int x, int y) {
    if (_x == std::numeric_limits<int>::min()) {
      _x = x;
      _y = y;
      return;
    }

    auto dx = x - _x;
    _x = x;
    auto dy = y - _y;
    _y = y;
    if (_right) {
      _camera.yaw_pitch(dx, dy);
    }
    if (_middle) {
      _camera.shift(dx, dy);
    }
  }
  void Wheel(int d) { _camera.dolly(d); }
};

} // namespace banana
