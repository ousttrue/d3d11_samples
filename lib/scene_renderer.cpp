#include "scene_renderer.h"
#include <banana/asset.h>
#include <gorilla/mesh.h>
#include <iostream>

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
