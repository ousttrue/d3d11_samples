
#include "scene_processor.h"
#include "banana/material.h"
#include "banana/mesh.h"
#include "banana/semantics.h"
#include "banana/types.h"

namespace banana {

void SceneProcessor::new_frame(const banana::OrbitCamera *camera,
                               std::span<const LightInfo> lights) {
  commands.clear();

  this->view = camera->view;
  this->viewprojection = camera->view * camera->projection;
  this->normal_matrix = camera->normal_matrix();
  auto cp = camera->position();
  this->camera_position = *((Float3 *)&cp);
  this->lights = lights;
}

void SceneProcessor::traverse(const std::shared_ptr<banana::Node> &node,
                              const Matrix4x4 &parent) {

  auto local = node->transform.matrix();
  auto m = local * parent;

  if (node->mesh) {
    for (auto &submesh : node->mesh->submeshes) {
      auto &material = submesh.material;
      commands.push_back(commands::Begin{
          node->mesh,
          material,
          submesh.state,
      });

      commands.push_back(commands::SetVariable{Semantics::WORLDVIEWPROJECTION,
                                               m * viewprojection});
      commands.push_back(
          commands::SetVariable{Semantics::VIEWPROJECTION, viewprojection});
      commands.push_back(commands::SetVariable{Semantics::WORLDVIEW, view});
      commands.push_back(
          commands::SetVariable{Semantics::NORMAL_MATRIX, m});
      commands.push_back(
          commands::SetVariable{Semantics::CAMERA_POSITION, camera_position});

      size_t offset = 0;
      for (auto &light : lights) {
        commands.push_back(
            commands::SetVariable{Semantics::LIGHT_LIST, light, offset});
        offset += sizeof(LightInfo);
      }

      // material
      for (auto [k, v] : material->properties) {
        commands.push_back(commands::SetVariable{k, v});
      }
      for (auto [k, v] : material->textures) {
        commands.push_back(commands::SetTexture{k, v});
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