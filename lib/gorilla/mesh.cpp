#include "mesh.h"

struct GltfShaderConstant {
  std::array<float, 16> MVP;
  std::array<float, 4> BaseColor;
};

namespace gorilla {

void Mesh::draw(const ComPtr<ID3D11DeviceContext> &context,
                const DirectX::XMMATRIX &projection,
                const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &model) {

  //
  // mesh level
  //
  auto VP = DirectX::XMMatrixMultiply(view, projection);
  auto MVP = DirectX::XMMatrixMultiply(model, VP);
  DirectX::XMFLOAT4X4 mvp;
  DirectX::XMStoreFloat4x4(&mvp, MVP);

  ia.setup(context);
  for (auto &submesh : submeshes) {
    //
    // submesh level
    //
    auto material = submesh.material;
    assert(material);
    material->pipeline.setup(context);

    // update constant buffer
    GltfShaderConstant constant;
    constant.MVP = *((const std::array<float, 16> *)&mvp);
    constant.BaseColor = material->base_color;
    for (auto &slot : material->pipeline.vs_stage.cb) {
      slot.update(context, constant);
    }
    for (auto &slot : material->pipeline.gs_stage.cb) {
      slot.update(context, constant);
    }
    for (auto &slot : material->pipeline.ps_stage.cb) {
      slot.update(context, constant);
    }

    // SRV
    if (material->base_color_texture) {
      material->base_color_texture->set_ps(context, 0, 0);
    }

    // STATE
    context->RSSetState(material->rs.Get());

    // draw submesh
    ia.draw_submesh(context, submesh.offset, submesh.draw_count);
  }
}

} // namespace gorilla