#pragma once
#include <DirectXMath.h>
#include <limits>

namespace gorilla {

class OrbitCamera {
  float _w = 1;
  float _h = 1;
  float _near = 0.01f;
  float _far = 100.0f;
  float _fovYRad = DirectX::XMConvertToRadians(60.0f);
  DirectX::XMFLOAT4X4 _projection;
  void calc_projection() {
    auto P = DirectX::XMMatrixPerspectiveFovRH(_fovYRad, _w / _h, _near, _far);
    DirectX::XMStoreFloat4x4(&_projection, P);
  }

  DirectX::XMFLOAT3 _shift = {0, 0, -5};
  float _yaw = 0;
  float _pitch = 0;
  DirectX::XMFLOAT4X4 _view;
  void calc_view() {
    auto Y = DirectX::XMMatrixRotationY(_yaw);
    auto P = DirectX::XMMatrixRotationX(_pitch);
    auto T = DirectX::XMMatrixTranslation(_shift.x, _shift.y, _shift.z);
    auto M = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(Y, P), T);
    DirectX::XMStoreFloat4x4(&_view, M);
  }

public:
  OrbitCamera() {
    calc_projection();
    calc_view();
  }

  DirectX::XMFLOAT4X4 matrix() const {
    auto V = DirectX::XMLoadFloat4x4(&_view);
    auto P = DirectX::XMLoadFloat4x4(&_projection);
    auto M = DirectX::XMMatrixMultiply(V, P);
    DirectX::XMFLOAT4X4 m;
    DirectX::XMStoreFloat4x4(&m, M);
    return m;
  }

  void yaw_pitch(int dx, int dy) {
    _yaw += dx / _h * 4;
    _pitch += dy / _h * 4;
    calc_view();
  }

  void shift(int dx, int dy) {
    _shift.x -= static_cast<float>(dx / _h * tan(_fovYRad / 2) * _shift.z * 2);
    _shift.y += static_cast<float>(dy / _h * tan(_fovYRad / 2) * _shift.z * 2);
    calc_view();
  }

  void dolly(int d) {
    if (d < 0) {
      _shift.z *= 1.1f;
    } else if (d > 0) {
      _shift.z *= 0.9f;
    }
    calc_view();
  }

  void resize(float w, float h) {
    if (w == _w && h == _h) {
      return;
    }
    _w = w;
    _h = h;
    calc_projection();
  }
};

struct MouseBinder {
  bool _left = false;
  bool _right = false;
  bool _middle = false;
  int _x = std::numeric_limits<int>::min();
  int _y = std::numeric_limits<int>::min();
  OrbitCamera &_camera;

public:
  MouseBinder(gorilla::OrbitCamera &camera) : _camera(camera) {}
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

} // namespace gorilla
