#pragma once
#include "mesh.h"
#include <DirectXMath.h>

namespace banana {

struct Transform {
  Float3 translation = {0, 0, 0};
  Float4 rotation = {0, 0, 0, 1}; // quaternon
  Float3 scaling = {1, 1, 1};

  Matrix4x4 matrix() const;
};

struct Node : std::enable_shared_from_this<Node> {
  std::string name;
  Transform transform;

  std::weak_ptr<Node> parent;
  std::vector<std::shared_ptr<Node>> children;

  void add_child(const std::shared_ptr<Node> &child) {
    child->parent = shared_from_this();
    children.push_back(child);
  }

  std::shared_ptr<Mesh> mesh;

  void calc_aabb(const Matrix4x4 &parent, AABB *aabb) const;
};

} // namespace  banana
