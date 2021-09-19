#include "orbit_camera.h"
#include "banana/types.h"

namespace banana {

void OrbitCamera::calc_projection() {
  auto P = DirectX::XMMatrixPerspectiveFovRH(fovYRad, screen.x / screen.y, near,
                                             far);
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

void OrbitCamera::yaw_pitch(int dx, int dy) {
  yaw += dx / screen.y * 4;
  pitch += dy / screen.y * 4;
  calc_view();
}

void OrbitCamera::shift(int dx, int dy) {
  translation.x -=
      static_cast<float>(dx / screen.y * tan(fovYRad / 2) * translation.z * 2);
  translation.y +=
      static_cast<float>(dy / screen.y * tan(fovYRad / 2) * translation.z * 2);
  calc_view();
}

void OrbitCamera::dolly(int d) {
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

} // namespace banana