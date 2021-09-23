#include "renderer.h"
#include "banana/types.h"
#include "gorilla/input_assembler.h"
#include "gorilla/pipeline.h"
#include <banana/asset.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <variant>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

std::shared_ptr<gorilla::Texture>
Renderer::get_or_create(const ComPtr<ID3D11Device> &device,
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

std::shared_ptr<gorilla::Pipeline>
Renderer::get_or_create(const ComPtr<ID3D11Device> &device,
                        const std::shared_ptr<banana::Material> &src) {
  auto found = _material_map.find(src);
  if (found != _material_map.end()) {
    return found->second;
  }

  auto material = std::make_shared<gorilla::Pipeline>();
  _material_map.insert(std::make_pair(src, material));

  auto shader = banana::get_string(src->shader_name);
  if (shader.empty()) {
    assert(false);
    return {};
  }
  auto [ok, error] =
      material->compile_shader(device, shader, "vsMain", {}, "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    assert(false);
    return {};
  }

  return material;
}

std::shared_ptr<gorilla::InputAssembler>
Renderer::get_or_create(const ComPtr<ID3D11Device> &device,
                        const std::shared_ptr<banana::Mesh> &src) {
  auto found = _mesh_map.find(src);
  if (found != _mesh_map.end()) {
    return found->second;
  }

  auto mesh = std::make_shared<gorilla::InputAssembler>();
  _mesh_map.insert(std::make_pair(src, mesh));
  if (!mesh->create_vertices(device, src->vertex_stride, src->vertices.data(),
                             src->vertices.size())) {
    return {};
  }
  if (!mesh->create_indices(device, src->index_stride, src->indices.data(),
                            src->indices.size())) {
    return {};
  }

  return mesh;
}

void Renderer::render(const ComPtr<ID3D11Device> &device,
                      const ComPtr<ID3D11DeviceContext> &context,
                      const std::shared_ptr<banana::Node> &root,
                      const banana::OrbitCamera *camera,
                      std::span<const banana::LightInfo> lights) {

  _processor.new_frame(camera, lights);
  _processor.traverse(root);
  for (auto &command : _processor.commands) {
    draw(device, context, command);
  }
}

void Renderer::draw(const ComPtr<ID3D11Device> &device,
                    const ComPtr<ID3D11DeviceContext> &context,
                    const banana::Command &command) {

  if (auto p = std::get_if<banana::commands::SetVariable>(&command)) {
    auto span = std::visit(
        [](const auto &x) {
          return std::span<const uint8_t>{(const uint8_t *)&x, sizeof(x)};
        },
        p->value);
    _material->set_variable(p->name, span.data(), span.size(), p->offset);
  } else if (auto p = std::get_if<banana::commands::SetTexture>(&command)) {
    auto texture = get_or_create(device, p->image);
    UINT slot;
    // vs
    if (_material->vs_stage.reflection.try_get_srv(p->srv, &slot)) {
      _vs_list.srv[slot] = texture->_srv.Get();
    }
    if (_material->vs_stage.reflection.try_get_sampler(p->sampler, &slot)) {
      _vs_list.sampler[slot] = texture->_sampler.Get();
    }
    // gs
    if (_material->gs_stage.reflection.try_get_srv(p->srv, &slot)) {
      _gs_list.srv[slot] = texture->_srv.Get();
    }
    if (_material->gs_stage.reflection.try_get_sampler(p->sampler, &slot)) {
      _gs_list.sampler[slot] = texture->_sampler.Get();
    }
    // ps
    if (_material->ps_stage.reflection.try_get_srv(p->srv, &slot)) {
      _ps_list.srv[slot] = texture->_srv.Get();
    }
    if (_material->ps_stage.reflection.try_get_sampler(p->sampler, &slot)) {
      _ps_list.sampler[slot] = texture->_sampler.Get();
    }
  } else if (auto p = std::get_if<banana::commands::Begin>(&command)) {
    assert(p->mesh);
    assert(p->material);
    _drawable = get_or_create(device, p->mesh);
    _material = get_or_create(device, p->material);
    assert(_material);
    _vs_list.clear(_material->vs_stage.reflection);
    _gs_list.clear(_material->gs_stage.reflection);
    _ps_list.clear(_material->ps_stage.reflection);
  } else if (auto p = std::get_if<banana::commands::End>(&command)) {
    _material->update(context);
    _material->setup(context);
    _drawable->setup(context);
    // vs
    context->VSSetShaderResources(0, static_cast<UINT>(_vs_list.srv.size()),
                                  _vs_list.srv.data());
    context->VSSetSamplers(0, static_cast<UINT>(_vs_list.sampler.size()),
                           _vs_list.sampler.data());
    // gs
    context->GSSetShaderResources(0, static_cast<UINT>(_gs_list.srv.size()),
                                  _gs_list.srv.data());
    context->GSSetSamplers(0, static_cast<UINT>(_gs_list.sampler.size()),
                           _gs_list.sampler.data());
    // ps
    context->PSSetShaderResources(0, static_cast<UINT>(_ps_list.srv.size()),
                                  _ps_list.srv.data());
    context->PSSetSamplers(0, static_cast<UINT>(_ps_list.sampler.size()),
                           _ps_list.sampler.data());
    _drawable->draw_submesh(context, p->draw_offset, p->draw_count);
  } else {
    throw std::runtime_error("not implemented");
  }
}
