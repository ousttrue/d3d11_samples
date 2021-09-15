#include "resource_manager.h"
#include <banana/asset.h>
#include <gorilla/input_assembler.h>
#include <iostream>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla::resource {

void Mesh::draw(const ComPtr<ID3D11DeviceContext> &context,
                const DirectX::XMMATRIX &projection,
                const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &model) {

  ia.setup(context);
  for (auto &submesh : submeshes) {
    auto material = submesh.material;

    // update constant buffer
    auto VP = DirectX::XMMatrixMultiply(view, projection);
    auto MVP = DirectX::XMMatrixMultiply(model, VP);
    DirectX::XMFLOAT4X4 mvp;
    DirectX::XMStoreFloat4x4(&mvp, MVP);
    material->pipeline.vs_cb[0].update(context, mvp);
    if (material->color_texture) {
      material->color_texture->set_ps(context, 0, 0);
    }

    material->pipeline.setup(context);

    context->RSSetState(material->rs.Get());

    ia.draw_submesh(context, submesh.offset, submesh.draw_count);
  }
}

std::shared_ptr<Texture>
ResourceManager::get_or_create(const ComPtr<ID3D11Device> &device,
                               const std::shared_ptr<banana::Image> &src) {

  if (!src) {
    return {};
  }

  auto found = _texture_map.find(src);
  if (found != _texture_map.end()) {
    return found->second;
  }

  auto texture = std::make_shared<Texture>();
  _texture_map.insert(std::make_pair(src, texture));
  texture->create(device, src->data(), src->width(), src->height());

  return texture;
}

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
  auto shader = banana::get_string(src->shader_name);
  if (shader.empty()) {
    assert(false);
    return {};
  }
  auto [vs, vserror] =
      material->pipeline.compile_vs(device, "vs", shader, "vsMain");
  if (!vs) {
    if (vserror) {
      std::cerr << (const char *)vserror->GetBufferPointer() << std::endl;
    }
    return {};
  }
  auto [gs, gserror] =
      material->pipeline.compile_gs(device, "gs", shader, "gsMain");
  if (!gs) {
    if (gserror) {
      std::cerr << (const char *)gserror->GetBufferPointer() << std::endl;
    }
    // return 5;
  }
  auto [ps, pserror] =
      material->pipeline.compile_ps(device, "ps", shader, "psMain");
  if (!ps) {
    if (pserror) {
      std::cerr << (const char *)pserror->GetBufferPointer() << std::endl;
    }
    return {};
  }

  material->color_texture = get_or_create(device, src->base_color_texture);

  D3D11_RASTERIZER_DESC rs_desc = {};
  rs_desc.CullMode = D3D11_CULL_NONE;
  rs_desc.FillMode = D3D11_FILL_SOLID;
  rs_desc.FrontCounterClockwise = true; // for glTF
  rs_desc.ScissorEnable = false;
  rs_desc.MultisampleEnable = false;
  if (FAILED(device->CreateRasterizerState(&rs_desc, &material->rs))) {
    return {};
  }

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
