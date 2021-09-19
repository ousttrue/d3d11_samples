#pragma once
#include "banana/types.h"
#include "node.h"
#include "orbit_camera.h"
#include <list>
#include <memory>
#include <span>
#include <stdint.h>
#include <string_view>
#include <variant>
#include <vector>

namespace banana {

namespace commands {

struct Begin {
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
};

struct SetVariable {
  std::string name;
  Variable value;
  size_t offset = 0;
};

struct SetTexture {
  std::shared_ptr<Image> image;
  std::string srv;
  std::string sampler;
};

struct End {
  uint32_t draw_offset = {};
  uint32_t draw_count = {};
};

} // namespace commands

using Command = std::variant<commands::Begin, commands::SetVariable,
                             commands::SetTexture, commands::End>;

struct SceneProcessor {
  Matrix4x4 viewprojection = {};
  Matrix4x4 view = {};
  Matrix3x4 normal_matrix = {};
  std::span<const LightInfo> lights;

  std::vector<Command> commands;
  void new_frame(const OrbitCamera *camera,
                 std::span<const LightInfo> lights = {});

  void traverse(const std::shared_ptr<Node> &node,
                const Matrix4x4 &parent = Matrix4x4::identity());
};

} // namespace banana