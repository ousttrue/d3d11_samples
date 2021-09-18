#include "types.h"
#include <DirectXMath.h>

namespace banana {
Matrix4x4 Matrix4x4::identity() {
  return {
      1, 0, 0, 0, //
      0, 1, 0, 0, //
      0, 0, 1, 0, //
      0, 0, 0, 1, //
  };
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &rhs) const {
  auto LHS = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4 *)this);
  auto RHS = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4 *)&rhs);
  auto M = DirectX::XMMatrixMultiply(LHS, RHS);
  Matrix4x4 m;
  DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4 *)&m, M);
  return m;
}

Float3 Matrix4x4::apply(const Float3 &src) const {
  auto M = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4 *)this);
  auto V = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&src);
  V = DirectX::XMVector3Transform(V, M);
  Float3 v;
  DirectX::XMStoreFloat3((DirectX::XMFLOAT3 *)&v, V);
  return v;
}

} // namespace banana
