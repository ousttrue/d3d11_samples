#include "scene_renderer.h"
#include "banana/scene_command.h"
#include "banana/types.h"
#include <banana/asset.h>
#include <gorilla/mesh.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <variant>

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
  material->normal_map = get_or_create(device, src->normal_map_texture);

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
    submesh.offset = sub.draw_offset;
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
                           const banana::Command &command) {

  if (auto p = std::get_if<banana::commands::SetVariable>(&command)) {
    auto span = std::visit(
        [](const auto &x) {
          return std::span<const uint8_t>{(const uint8_t *)&x, sizeof(x)};
        },
        p->value);
    _material->pipeline.set_variable(p->name, span.data(), span.size());
  } else if (auto p = std::get_if<banana::commands::SetTexture>(&command)) {
    auto texture = get_or_create(device, p->image);
    // texture->set_ps(context, p->srv, p->sampler);
    _material->pipeline.set_srv(context, p->srv, texture->_srv);
    _material->pipeline.set_sampler(context, p->sampler, texture->_sampler);
  } else if (auto p = std::get_if<banana::commands::Begin>(&command)) {
    assert(p->mesh);
    assert(p->material);
    _drawable = get_or_create(device, p->mesh);
    _material = get_or_create(device, p->material);
    assert(_material);
    context->RSSetState(_material->rs.Get());
  } else if (auto p = std::get_if<banana::commands::End>(&command)) {
    _material->pipeline.update(context);
    _material->pipeline.setup(context);
    _drawable->ia.setup(context);
    _drawable->ia.draw_submesh(context, p->draw_offset, p->draw_count);
  } else {
    throw std::runtime_error("not implemented");
  }
}

struct GltfShaderConstant {
  std::array<float, 16> MVP;
  std::array<float, 4> BaseColor;
};

static void draw(const ComPtr<ID3D11DeviceContext> &context,
                 const std::shared_ptr<gorilla::Mesh> &drawable,
                 const banana::Matrix4x4 &projection,
                 const banana::Matrix4x4 &view,
                 const banana::Matrix4x4 &model) {

  //
  // mesh level
  //
  auto vp = view * projection;
  auto mvp = model * vp;

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
                           const banana::Matrix4x4 &projection,
                           const banana::Matrix4x4 &view,
                           const banana::Matrix4x4 &parent,
                           const std::shared_ptr<banana::Node> &node) {

  auto local = node->transform.matrix();
  auto m = local * parent;

  if (node->mesh) {
    auto drawable = _resource_manager.get_or_create(device, node->mesh);
    draw(context, drawable, projection, view, m);
  }

  for (auto &child : node->children) {
    Render(device, context, projection, view, m, child);
  }
}
