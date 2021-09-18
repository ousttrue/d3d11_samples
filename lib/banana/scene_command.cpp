
#include "scene_command.h"
#include "banana/types.h"

namespace banana {

void SceneCommand::new_frame(const banana::OrbitCamera *camera) {
  commands.clear();

  // renderer.Render(,
  //                 DirectX::XMLoadFloat4x4(&camera->projection()),
  //                 DirectX::XMLoadFloat4x4(&camera->view()),
  //                 DirectX::XMMatrixIdentity(), root);
}

void SceneCommand::traverse(const std::shared_ptr<banana::Node> &root, const Matrix4x4 &parent) {

  // update constant buffer
  // GltfShaderConstant constant;
  // constant.MVP = *((const std::array<float, 16> *)&mvp);
  // constant.BaseColor = material->base_color;
  // for (auto &slot : material->pipeline.vs_stage.cb) {
  //   slot.update(context, constant);
  // }
  // for (auto &slot : material->pipeline.gs_stage.cb) {
  //   slot.update(context, constant);
  // }
  // for (auto &slot : material->pipeline.ps_stage.cb) {
  //   slot.update(context, constant);
  // }

}

} // namespace banana