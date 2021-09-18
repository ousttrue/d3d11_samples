#include "node.h"

namespace banana {

Matrix4x4 Transform::matrix() const {
  auto T =
      DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
  auto Q = DirectX::XMLoadFloat4((const DirectX::XMFLOAT4 *)&rotation);
  auto R = DirectX::XMMatrixRotationQuaternion(Q);
  auto S = DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);
  auto M = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(S, R), T);
  Matrix4x4 m;
  DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4 *)&m, M);
  return m;
}

void Node::calc_aabb(const Matrix4x4 &parent, AABB *aabb) const {

  auto world = transform.matrix() * parent;

  if (mesh) {
    auto world_min = parent.apply(mesh->aabb.min);
    aabb->expand(world_min);
    auto world_max = parent.apply(mesh->aabb.max);
    aabb->expand(world_max);
  }

  for (auto &child : children) {
    child->calc_aabb(world, aabb);
  }
}

} // namespace banana