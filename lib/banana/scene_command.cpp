
#include "scene_command.h"
#include "banana/mesh.h"

namespace banana {

void SceneCommand::new_frame(const banana::OrbitCamera *camera) {
  commands.clear();

  // _camera = camera;
  viewprojection = camera->view * camera->projection;
}

void SceneCommand::traverse(const std::shared_ptr<banana::Node> &node,
                            const Matrix4x4 &parent) {

  auto local = node->transform.matrix();
  auto m = local * parent;

  if (node->mesh) {
    for (auto &submesh : node->mesh->submeshes) {
      auto &material = submesh.material;
      commands.push_back(commands::Begin{
          node->mesh,
          material,
      });

      commands.push_back(commands::SetVariable{"MVP", m * viewprojection});
      commands.push_back(
          commands::SetVariable{"BaseColor", material->base_color});

      if (material->base_color_texture) {
        commands.push_back(commands::SetTexture{material->base_color_texture,
                                                "BaseColorTexture",
                                                "BaseColorSampler"});
      }
      if (material->normal_map_texture) {
        commands.push_back(commands::SetTexture{material->normal_map_texture,
                                                "NormalMapTexture",
                                                "NormalMapSampler"});
      }

      commands.push_back(
          commands::End{submesh.draw_offset, submesh.draw_count});
    }
  }

  for (auto &child : node->children) {
    traverse(child, m);
  }
}

} // namespace banana