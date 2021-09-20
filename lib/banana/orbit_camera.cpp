#include "orbit_camera.h"
#include "banana/types.h"

namespace banana {

void OrbitCamera::calc_projection() {
  auto P = DirectX::XMMatrixPerspectiveFovRH(fovYRad, screen.x / screen.y,
                                             _near, _far);
  DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4 *)&projection, P);
}

void OrbitCamera::calc_view() {
  auto Y = DirectX::XMMatrixRotationY(yaw);
  auto P = DirectX::XMMatrixRotationX(pitch);
  auto T =
      DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
  auto M = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(Y, P), T);
  DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4 *)&view, M);
}

Matrix3x4 OrbitCamera::normal_matrix() const {
  // TODO: inverse, transpose
  Matrix3x4 n;
  n._11 = view._11;
  n._12 = view._12;
  n._13 = view._13;
  n._21 = view._21;
  n._22 = view._22;
  n._23 = view._23;
  n._31 = view._31;
  n._32 = view._32;
  n._33 = view._33;
  return n;
}

DirectX::XMFLOAT3 OrbitCamera::position() const {
  auto Y = DirectX::XMMatrixRotationY(-yaw);
  auto P = DirectX::XMMatrixRotationX(-pitch);
  auto T = DirectX::XMMatrixTranslation(-translation.x, -translation.y,
                                        -translation.z);
  auto M = DirectX::XMMatrixMultiply(T, DirectX::XMMatrixMultiply(P, Y));
  DirectX::XMFLOAT4 pos(0, 0, 0, 1);
  auto POS = DirectX::XMLoadFloat4(&pos);
  POS = DirectX::XMVector4Transform(POS, M);
  DirectX::XMStoreFloat4(&pos, POS);
  return {pos.x, pos.y, pos.z};
}

OrbitCamera::OrbitCamera() {
  calc_projection();
  calc_view();
}

void OrbitCamera::yaw_pitch(float dx, float dy) {
  yaw += dx / screen.y * 4;
  pitch += dy / screen.y * 4;
  calc_view();
}

void OrbitCamera::shift(float dx, float dy) {
  translation.x -=
      static_cast<float>(dx / screen.y * tan(fovYRad / 2) * translation.z * 2);
  translation.y +=
      static_cast<float>(dy / screen.y * tan(fovYRad / 2) * translation.z * 2);
  calc_view();
}

void OrbitCamera::dolly(float d) {
  if (d < 0) {
    translation.z *= 1.1f;
  } else if (d > 0) {
    translation.z *= 0.9f;
  }
  calc_view();
}

void OrbitCamera::resize(float w, float h) {
  if (w == screen.x && h == screen.y) {
    return;
  }
  screen = {w, h};
  calc_projection();
}

void OrbitCamera::fit(float y, float half_height) {
  translation.z = -half_height / static_cast<float>(tan(fovYRad * 0.5f)) * 1.5f;
  translation.y = -y;
  calc_view();
}

void OrbitCamera::update(float x, float y, float w, float h, bool left,
                         bool right, bool middle, float wheel) {

  resize(w, h);

  _left = left;
  _right = right;
  _middle = middle;

  if (_x == std::numeric_limits<float>::quiet_NaN()) {
    _x = x;
    _y = y;
    return;
  }

  auto dx = x - _x;
  _x = x;
  auto dy = y - _y;
  _y = y;
  if (_right) {
    yaw_pitch(dx, dy);
  }
  if (_middle) {
    shift(dx, dy);
  }
  if (wheel) {
    dolly(wheel);
  }
}

} // namespace banana