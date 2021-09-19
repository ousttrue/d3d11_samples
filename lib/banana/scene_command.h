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

using Variable = std::variant<float, Float2, Float3, Float4, Matrix4x4>;

struct Begin {
  std::shared_ptr<Mesh> mesh;
  std::shared_ptr<Material> material;
};

struct SetVariable {
  std::string_view name;
  Variable value;
};

struct SetTexture {
  std::shared_ptr<Image> image;
  std::string_view srv;
  std::string_view sampler;
};

struct End {
  uint32_t draw_offset;
  uint32_t draw_count;
};

} // namespace commands

using Command = std::variant<commands::Begin, commands::SetVariable,
                             commands::SetTexture, commands::End>;

struct SceneCommand {
  std::vector<Command> commands;
  Matrix4x4 viewprojection = {};
  void new_frame(const OrbitCamera *camera);

  void traverse(const std::shared_ptr<Node> &root,
                const Matrix4x4 &parent = Matrix4x4::identity());
};

} // namespace banana