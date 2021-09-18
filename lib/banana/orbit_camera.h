#pragma once
#include "types.h"
#include <DirectXMath.h>
#include <limits>

namespace banana {

struct OrbitCamera {
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

  DirectX::XMFLOAT3X4 normal_matrix() const {
    // inverse, transpose
    DirectX::XMFLOAT3X4 n;
    n._11 = _view._11;
    n._12 = _view._12;
    n._13 = _view._13;
    n._21 = _view._21;
    n._22 = _view._22;
    n._23 = _view._23;
    n._31 = _view._31;
    n._32 = _view._32;
    n._33 = _view._33;
    return n;
  }

  DirectX::XMFLOAT3 position() const {
    auto Y = DirectX::XMMatrixRotationY(-_yaw);
    auto P = DirectX::XMMatrixRotationX(-_pitch);
    auto T = DirectX::XMMatrixTranslation(-_shift.x, -_shift.y, -_shift.z);
    auto M = DirectX::XMMatrixMultiply(T, DirectX::XMMatrixMultiply(P, Y));
    DirectX::XMFLOAT4 pos(0, 0, 0, 1);
    auto POS = DirectX::XMLoadFloat4(&pos);
    POS = DirectX::XMVector4Transform(POS, M);
    DirectX::XMStoreFloat4(&pos, POS);
    return {pos.x, pos.y, pos.z};
  }

public:
  OrbitCamera() {
    calc_projection();
    calc_view();
  }

  const DirectX::XMFLOAT4X4 &view() const { return _view; }
  const DirectX::XMFLOAT4X4 &projection() const { return _projection; }
  DirectX::XMFLOAT4X4 view_projection_matrix() const {
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

  void fit(float y, float half_height) {
    _shift.z = -static_cast<float>(half_height / tan(_fovYRad * 0.5f)) * 1.5f;
    _shift.y = -y;
    calc_view();
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
