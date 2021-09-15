#include "scene_renderer.h"

struct SceneRendererImpl {};

SceneRenderer::SceneRenderer() : _impl(new SceneRendererImpl) {}

SceneRenderer::~SceneRenderer() { delete _impl; }

void SceneRenderer::Render(const ComPtr<ID3D11Device> &device,
                           const ComPtr<ID3D11DeviceContext> &context,
                           const DirectX::XMMATRIX &projection,
                           const DirectX::XMMATRIX &view,
                           const DirectX::XMMATRIX &parent,
                           const std::shared_ptr<banana::Node> &node) {

  // auto shader = banana::get_string("gltf.hlsl");
  // if (shader.empty()) {
  //   return 1;
  // }

  // // setup pipeline
  // gorilla::Pipeline pipeline;
  // auto [compiled, vserror] =
  //     pipeline.compile_vs(device, "vs", shader, "vsMain");
  // if (!compiled) {
  //   if (vserror) {
  //     std::cerr << (const char *)vserror->GetBufferPointer() << std::endl;
  //   }
  //   return 5;
  // }
  // auto input_layout = gorilla::create_input_layout(device, compiled);
  // if (!input_layout) {
  //   return 6;
  // }
  // auto [ps, pserror] = pipeline.compile_ps(device, "ps", shader, "psMain");
  // if (!ps) {
  //   if (pserror) {
  //     std::cerr << (const char *)pserror->GetBufferPointer() << std::endl;
  //   }
  //   return 7;
  // }
  // gorilla::ShaderVariables ps_slots;
  // if (!ps_slots.reflect(ps)) {
  //   return 7;
  // }
  // assert(ps_slots.sampler_slots.size() == 1);
  // assert(ps_slots.srv_slots.size() == 1);
  // UINT sampler_slot = 0;
  // UINT srv_slot = 0;

  // context->RSSetState(rs.Get());
  // pipeline.setup(context);
  // cb.set_vs(context, cb_slot);
  // texture.set_ps(context, srv_slot, sampler_slot);
  // ia.draw(context);
}
//   gorilla::ConstantBuffer cb;
// if (!cb.create(device, sizeof(DirectX::XMFLOAT4X4))) {
//   return 8;
// }
// UINT cb_slot = 0;

// ComPtr<ID3D11RasterizerState> rs;
// D3D11_RASTERIZER_DESC rs_desc = {};
// rs_desc.CullMode = D3D11_CULL_NONE;
// rs_desc.FillMode = D3D11_FILL_SOLID;
// rs_desc.FrontCounterClockwise = true;
// rs_desc.ScissorEnable = false;
// rs_desc.MultisampleEnable = false;
// if (FAILED(device->CreateRasterizerState(&rs_desc, &rs))) {
//   return 9;
// }

// auto mesh = loader.meshes[0];
// gorilla::InputAssembler ia;
// if (!ia.create_vertices(device, input_layout, mesh->vertices)) {
//   return 13;
// }
// if (!ia.create_indices(device, mesh->indices)) {
//   return 14;
// }
// auto image = mesh->submeshes[0].material->base_color_texture;
// gorilla::Texture texture;
// if (!texture.create(device, image->data(), image->width(),
// image->height())) {
//   return 10;
// }
//     cb.update(context, camera.matrix());
