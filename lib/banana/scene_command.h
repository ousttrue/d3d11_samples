#pragma once
#include "banana/types.h"
#include "node.h"
#include "orbit_camera.h"
#include <list>
#include <memory>
#include <stdint.h>
#include <vector>

namespace banana {

struct DrawCommand {
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
  std::vector<uint8_t> vs_backing_store;
  std::vector<uint8_t> ps_backing_store;
  uint32_t draw_offset;
  uint32_t draw_count;
};

struct SceneCommand {
  std::list<DrawCommand> pool;
  std::list<DrawCommand> commands;
  void new_frame(const OrbitCamera *camera);
  void traverse(const std::shared_ptr<Node> &root, const Matrix4x4 &parent = Matrix4x4::identity());
};

} // namespace banana