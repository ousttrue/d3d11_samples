#include "scene_renderer.h"
#include <banana/asset.h>
#include <gorilla/mesh.h>
#include <iostream>
#include <memory>
#include <system_error>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

std::shared_ptr<gorilla::Texture>
ResourceManager::get_or_create(const ComPtr<ID3D11Device> &device,
                               const std::shared_ptr<banana::Image> &src) {

  if (!src) {
    return {};
  }

  auto found = _texture_map.find(src);
  if (found != _texture_map.end()) {
    return found->second;
  }

  auto texture = std::make_shared<gorilla::Texture>();
  _texture_map.insert(std::make_pair(src, texture));
  texture->create(device, src->data(), src->width(), src->height());

  return texture;
}

std::shared_ptr<gorilla::Material>
ResourceManager::get_or_create(const ComPtr<ID3D11Device> &device,
                               const std::shared_ptr<banana::Material> &src) {

  auto found = _material_map.find(src);
  if (found != _material_map.end()) {
    return found->second;
  }

  auto material = std::make_shared<gorilla::Material>();
  _material_map.insert(std::make_pair(src, material));
  // material->pipeline.
  auto shader = banana::get_string(src->shader_name);
  if (shader.empty()) {
    assert(false);
    return {};
  }
  auto [ok, error] =
      material->pipeline.compile_shader(device, shader, "vsMain", {}, "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    assert(false);
    return {};
  }

  material->base_color_texture = get_or_create(device, src->base_color_texture);
  if (!material->base_color_texture) {
    // default white
    static std::shared_ptr<gorilla::Texture> WHITE;
    if (!WHITE) {
      WHITE = std::make_shared<gorilla::Texture>();
      uint8_t white[2 * 2 * 4] = {
          1, 1, 1, 1, //
          1, 1, 1, 1, //
          1, 1, 1, 1, //
          1, 1, 1, 1, //
      };
      WHITE->create(device, white, 2, 2);
    }
    material->base_color_texture = WHITE;
  }
  material->normal_map = get_or_create(device, src->normal_map);

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

std::shared_ptr<gorilla::Mesh>
ResourceManager::get_or_create(const ComPtr<ID3D11Device> &device,
                               const std::shared_ptr<banana::Mesh> &src) {
  auto found = _mesh_map.find(src);
  if (found != _mesh_map.end()) {
    return found->second;
  }

  auto mesh = std::make_shared<gorilla::Mesh>();
  _mesh_map.insert(std::make_pair(src, mesh));
  if (!mesh->ia.create_vertices(device, src->vertices)) {
    return {};
  }
  if (!mesh->ia.create_indices(device, src->indices)) {
    return {};
  }

  for (auto &sub : src->submeshes) {
    auto &submesh = mesh->submeshes.emplace_back(gorilla::SubMesh{});
    submesh.offset = sub.offset;
    submesh.draw_count = sub.draw_count;
    assert(sub.material);
    if (sub.material) {
      submesh.material = get_or_create(device, sub.material);
    }
  }

  return mesh;
}

void ResourceManager::draw(const ComPtr<ID3D11Device> &device,
                           const ComPtr<ID3D11DeviceContext> &context,
                           const banana::DrawCommand &command) {

  auto drawable = get_or_create(device, command.mesh);
  auto material = get_or_create(device, command.material);
  assert(material);

  {
    // VS
    auto stage = &material->pipeline.vs_stage;
    stage->cb[0].update(context, command.vs_backing_store.data(),
                        command.vs_backing_store.size());
  }

  {
    // PS
    auto stage = &material->pipeline.ps_stage;
    stage->cb[0].update(context, command.ps_backing_store.data(),
                        command.ps_backing_store.size());

    // SRV
    if (material->base_color_texture) {
      material->base_color_texture->set_ps(context, 0, 0);
    }
    if (material->normal_map) {
      material->normal_map->set_ps(context, 1, 1);
    }
  }

  material->pipeline.setup(context);
  drawable->ia.setup(context);

  // STATE
  context->RSSetState(material->rs.Get());

  // draw submesh
  drawable->ia.draw_submesh(context, command.draw_offset, command.draw_count);
}

struct GltfShaderConstant {
  std::array<float, 16> MVP;
  std::array<float, 4> BaseColor;
};

static void draw(const ComPtr<ID3D11DeviceContext> &context,
                 const std::shared_ptr<gorilla::Mesh> &drawable,
                 const DirectX::XMMATRIX &projection,
                 const DirectX::XMMATRIX &view,
                 const DirectX::XMMATRIX &model) {

  //
  // mesh level
  //
  auto VP = DirectX::XMMatrixMultiply(view, projection);
  auto MVP = DirectX::XMMatrixMultiply(model, VP);
  DirectX::XMFLOAT4X4 mvp;
  DirectX::XMStoreFloat4x4(&mvp, MVP);

  drawable->ia.setup(context);
  for (auto &submesh : drawable->submeshes) {
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
    if (material->normal_map) {
      material->normal_map->set_ps(context, 1, 1);
    }

    // STATE
    context->RSSetState(material->rs.Get());

    // draw submesh
    drawable->ia.draw_submesh(context, submesh.offset, submesh.draw_count);
  }
}

void SceneRenderer::Render(const ComPtr<ID3D11Device> &device,
                           const ComPtr<ID3D11DeviceContext> &context,
                           const DirectX::XMMATRIX &projection,
                           const DirectX::XMMATRIX &view,
                           const DirectX::XMMATRIX &parent,
                           const std::shared_ptr<banana::Node> &node) {

  auto local = node->transform.matrix();
  auto M = DirectX::XMMatrixMultiply(
      DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4 *)&local), parent);

  if (node->mesh) {
    auto drawable = _resource_manager.get_or_create(device, node->mesh);
    draw(context, drawable, projection, view, M);
  }

  for (auto &child : node->children) {
    Render(device, context, projection, view, M, child);
  }
}
