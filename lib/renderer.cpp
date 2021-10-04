#include "renderer.h"
#include "banana/material.h"
#include "banana/types.h"
#include "gorilla/drawable.h"
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

  auto pipeline = std::make_shared<gorilla::Pipeline>();
  _material_map.insert(std::make_pair(src, pipeline));

  auto shader = banana::get_asset(src->shader_name);
  if (!shader) {
    assert(false);
    return {};
  }

  // hlsl defines
  std::vector<D3D_SHADER_MACRO> defines;
  if (src->textures.contains(banana::Semantics::MATERIAL_COLOR)) {
    defines.push_back({
        "TEXTURE_COLOR",
        "1",
    });
  }
  if (src->textures.contains(banana::Semantics::MATERIAL_NORMAL)) {
    defines.push_back({
        "TEXTURE_NORMAL",
        "1",
    });
  }
  defines.push_back({});

  auto [ok, error] = pipeline->compile_shader(device, shader, defines.data(),
                                              "vsMain", {}, "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    assert(false);
    return {};
  }

  return pipeline;
}

std::shared_ptr<gorilla::State>
Renderer::get_or_create(const ComPtr<ID3D11Device> &device,
                        const banana::MaterialStates &src) {
  auto found = _state_map.find(src);
  if (found != _state_map.end()) {
    return found->second;
  }

  auto state = std::make_shared<gorilla::State>();
  _state_map.insert(std::make_pair(src, state));
  if (src & banana::MaterialStatesDoubleFace) {
    state->rs_desc.CullMode = D3D11_CULL_NONE;
  }
  if (!state->create(device, src & banana::MaterialStatesAlphaBlend)) {
    return {};
  }

  return state;
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

  if (src->vertex_dynamic_buffer_size) {
    if (!mesh->create_dynamic_vertices(device, src->vertex_stride,
                                       src->vertex_dynamic_buffer_size)) {
      return {};
    }
  } else {
    if (!mesh->create_vertices(device, src->vertex_stride, src->vertices.data(),
                               src->vertices.size())) {
      return {};
    }
  }

  if (src->index_dynamic_buffer_size) {
    if (!mesh->create_dynamic_indices(device, src->index_stride,
                                      src->index_dynamic_buffer_size)) {
      return {};
    }
  } else {
    if (!mesh->create_indices(device, src->index_stride, src->indices.data(),
                              src->indices.size())) {
      return {};
    }
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
    _pipeline->set_variable(p->semantic, span.data(), span.size(), p->offset);
  } else if (auto p = std::get_if<banana::commands::SetTexture>(&command)) {
    auto texture = get_or_create(device, p->image);
    UINT slot;
    // vs
    if (_pipeline->vs_stage.reflection.try_get_srv(p->semantic, &slot)) {
      _vs_list.srv[slot] = texture->_srv.Get();
    }
    if (_pipeline->vs_stage.reflection.try_get_sampler(p->semantic, &slot)) {
      _vs_list.sampler[slot] = texture->_sampler.Get();
    }
    // gs
    if (_pipeline->gs_stage.reflection.try_get_srv(p->semantic, &slot)) {
      _gs_list.srv[slot] = texture->_srv.Get();
    }
    if (_pipeline->gs_stage.reflection.try_get_sampler(p->semantic, &slot)) {
      _gs_list.sampler[slot] = texture->_sampler.Get();
    }
    // ps
    if (_pipeline->ps_stage.reflection.try_get_srv(p->semantic, &slot)) {
      _ps_list.srv[slot] = texture->_srv.Get();
    }
    if (_pipeline->ps_stage.reflection.try_get_sampler(p->semantic, &slot)) {
      _ps_list.sampler[slot] = texture->_sampler.Get();
    }
  } else if (auto p = std::get_if<banana::commands::Begin>(&command)) {
    assert(p->mesh);
    assert(p->material);
    _ia = get_or_create(device, p->mesh);
    if (p->mesh->vertex_updated) {
      _ia->update_vertices(context, p->mesh->vertices.data(),
                           p->mesh->vertices.size());
      p->mesh->vertex_updated = false;
    }
    if (p->mesh->index_updated) {
      _ia->update_indices(context, p->mesh->indices.data(),
                          p->mesh->indices.size());
      p->mesh->index_updated = false;
    }

    _pipeline = get_or_create(device, p->material);
    assert(_pipeline);
    _vs_list.clear(_pipeline->vs_stage.reflection);
    _gs_list.clear(_pipeline->gs_stage.reflection);
    _ps_list.clear(_pipeline->ps_stage.reflection);

    _state = get_or_create(device, p->state);
  } else if (auto p = std::get_if<banana::commands::End>(&command)) {
    _pipeline->update(context);
    _pipeline->setup(context);
    _ia->setup(context);
    _state->setup(context);
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
    _ia->draw_submesh(context, p->draw_offset, p->draw_count);
  } else {
    throw std::runtime_error("not implemented");
  }
}
