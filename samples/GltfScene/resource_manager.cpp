#include "resource_manager.h"
#include <gorilla/input_assembler.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla::resource {
void Mesh::draw(const ComPtr<ID3D11DeviceContext> &context,
                const DirectX::XMMATRIX &projection,
                const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &model) {

  ia.setup(context);
  for (auto &submesh : submeshes) {
    submesh.material->setup(context);
    ia.draw_submesh(context, submesh.offset, submesh.draw_count);
  }
}

// gorilla::InputAssembler ia;

// std::shared_ptr<Texture>
// ResourceManager::get_or_create(const ComPtr<ID3D11Device> &device,
//                                const std::shared_ptr<banana::Image> &src) {}

std::shared_ptr<Material>
ResourceManager::get_or_create(const ComPtr<ID3D11Device> &device,
                               const std::shared_ptr<banana::Material> &src) {

  auto found = _material_map.find(src);
  if (found != _material_map.end()) {
    return found->second;
  }

  auto material = std::make_shared<Material>();
  _material_map.insert(std::make_pair(src, material));
  // material->pipeline.
  // material->_color_texture = 


  return material;
}

std::shared_ptr<Mesh>
ResourceManager::get_or_create(const ComPtr<ID3D11Device> &device,
                               const std::shared_ptr<banana::Mesh> &src) {
  auto found = _mesh_map.find(src);
  if (found != _mesh_map.end()) {
    return found->second;
  }

  auto mesh = std::make_shared<Mesh>();
  _mesh_map.insert(std::make_pair(src, mesh));
  if (!mesh->ia.create_vertices(device, src->vertices)) {
    return {};
  }
  if (!mesh->ia.create_indices(device, src->indices)) {
    return {};
  }

  for (auto &sub : src->submeshes) {
    auto &submesh = mesh->submeshes.emplace_back(SubMesh{});
    submesh.offset = sub.offset;
    submesh.draw_count = sub.draw_count;
    if (sub.material) {
      submesh.material = get_or_create(device, sub.material);
    }
  }

  return mesh;
}

} // namespace gorilla::resource

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
// auto image = mesh->submeshes[0].material->base_color_texture;
// gorilla::Texture texture;
// if (!texture.create(device, image->data(), image->width(),
// image->height())) {
//   return 10;
// }
//     cb.update(context, camera.matrix());
