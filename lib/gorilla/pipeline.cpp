#include "banana/semantics.h"
#include <assert.h>
#include <gorilla/pipeline.h>
#include <gorilla/shader.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla {

static bool match(const AnnotationSemantics &s,
                  const D3D11_SHADER_VARIABLE_DESC &desc) {
  return s.name == desc.Name;
}

static bool match(const AnnotationSemantics &s,
                  const D3D11_SHADER_INPUT_BIND_DESC &desc) {
  return s.name == desc.Name;
}

void ShaderStage::create_semantics_map(
    std::span<const AnnotationSemantics> semantics) {
  for (auto &s : semantics) {
    for (size_t i = 0; i < reflection.cb_slots.size(); ++i) {
      for (auto &v : reflection.cb_slots[i].variables) {
        if (match(s, v)) {
          VariablePosition value{static_cast<UINT>(i), v.StartOffset, v.Size};
          semantics_map.insert(std::make_pair(s.semantic, value));
        }
      }
    }
    for (UINT i = 0; i < reflection.srv_slots.size(); ++i) {
      if (match(s, reflection.srv_slots[i])) {
        semantics_srv_map.insert(std::make_pair(s.semantic, i));
      }
    }
    for (UINT i = 0; i < reflection.sampler_slots.size(); ++i) {
      if (match(s, reflection.sampler_slots[i])) {
        semantics_sampler_map.insert(std::make_pair(s.semantic, i));
      }
    }
  }
}

void ShaderStage::set_variable(banana::Semantics semantic, const void *p,
                               size_t size, size_t offset) {
  auto it = semantics_map.find(semantic);
  if (it != semantics_map.end()) {
    memcpy(reflection.cb_slots[it->second.slot].backing_store.data() +
               it->second.offset + offset,
           p, size);
  }
}

void ShaderStage::set_variables(const banana::OrbitCamera &camera) {

  for (auto [k, v] : semantics_map) {
    switch (k) {
    case banana::Semantics::CAMERA_VIEW:
      set_variable(banana::Semantics::CAMERA_VIEW, camera.view);
      break;

    case banana::Semantics::CAMERA_PROJECTION:
      set_variable(banana::Semantics::CAMERA_PROJECTION, camera.projection);
      break;

    case banana::Semantics::CAMERA_NEAR_FAR_FOVY:
      set_variable(banana::Semantics::CAMERA_NEAR_FAR_FOVY,
                   banana::Float3{camera._near, camera._far, camera.fovYRad});
      break;

    case banana::Semantics::CAMERA_POSITION: {
      auto p = camera.position();
      set_variable(banana::Semantics::CAMERA_POSITION,
                   banana::Float3{p.x, p.y, p.z});
      break;

    case banana::Semantics::CURSOR_SCREEN_SIZE:
      set_variable(banana::Semantics::CURSOR_SCREEN_SIZE,
                   banana::Float4{0, 0, camera.screen.x, camera.screen.y});
      break;
    }
    }
  }
}

std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
Pipeline::compile_vs(const ComPtr<ID3D11Device> &device, const char *name,
                     const std::shared_ptr<banana::Asset> &asset, const char *entry_point,
                     const D3D_SHADER_MACRO *define) {
  auto [compiled, error] =
      gorilla::compile_vs(name, asset, entry_point, define);
  if (!compiled) {
    return {{}, error};
  }
  auto hr =
      device->CreateVertexShader((DWORD *)compiled->GetBufferPointer(),
                                 compiled->GetBufferSize(), nullptr, &_vs);
  if (FAILED(hr)) {
    return {};
  }

  create_cb(vs_stage, device, compiled);

  _input_layout = gorilla::create_input_layout(device, compiled);
  // if (!_input_layout) {
  //   return {};
  // }

  return {compiled, {}};
}

std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
Pipeline::compile_gs(const ComPtr<ID3D11Device> &device, const char *name,
                     const std::shared_ptr<banana::Asset> &asset, const char *entry_point,
                     const D3D_SHADER_MACRO *define) {
  auto [compiled, error] =
      gorilla::compile_gs(name, asset, entry_point, define);
  if (!compiled) {
    return {{}, error};
  }
  auto hr =
      device->CreateGeometryShader((DWORD *)compiled->GetBufferPointer(),
                                   compiled->GetBufferSize(), nullptr, &_gs);
  if (FAILED(hr)) {
    return {};
  }

  create_cb(gs_stage, device, compiled);

  return {compiled, {}};
}

std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
Pipeline::compile_ps(const ComPtr<ID3D11Device> &device, const char *name,
                     const std::shared_ptr<banana::Asset> &asset, const char *entry_point,
                     const D3D_SHADER_MACRO *define) {
  auto [compiled, error] =
      gorilla::compile_ps(name, asset, entry_point, define);
  if (!compiled) {
    return {{}, error};
  }
  auto hr = device->CreatePixelShader((DWORD *)compiled->GetBufferPointer(),
                                      compiled->GetBufferSize(), nullptr, &_ps);
  if (FAILED(hr)) {
    return {};
  }

  create_cb(ps_stage, device, compiled);

  return {compiled, {}};
}

void Pipeline::create_cb(ShaderStage &stage, const ComPtr<ID3D11Device> &device,
                         const ComPtr<ID3DBlob> &compiled) {
  if (!stage.reflection.reflect(compiled)) {
    assert(false);
    return;
  }

  for (auto &slot : stage.reflection.cb_slots) {
    auto &cb = stage.cb.emplace_back(ConstantBuffer{});
    cb.create(device, slot.desc.Size);
  }
}

std::pair<bool, std::string>
Pipeline::compile_shader(const ComPtr<ID3D11Device> &device,
                         const std::shared_ptr<banana::Asset> &asset,
                         const D3D_SHADER_MACRO *define, const char *vs_entry,
                         const char *gs_entry, const char *ps_entry) {
  {
    auto [compiled, error] = compile_vs(device, "vs", asset, vs_entry, define);
    if (!compiled) {
      return {false, (const char *)error->GetBufferPointer()};
    }
  }
  if (gs_entry) {
    auto [compiled, error] = compile_gs(device, "gs", asset, gs_entry, define);
    if (!compiled) {
      return {false, (const char *)error->GetBufferPointer()};
    }
  }
  {
    auto [compiled, error] = compile_ps(device, "ps", asset, ps_entry, define);
    if (!compiled) {
      return {false, (const char *)error->GetBufferPointer()};
    }
  }

  _dxsas.parse(asset);
  vs_stage.create_semantics_map(_dxsas.semantics);
  gs_stage.create_semantics_map(_dxsas.semantics);
  ps_stage.create_semantics_map(_dxsas.semantics);

  return {true, {}};
}

void Pipeline::set_variable(std::string_view name, const void *p, size_t size,
                            size_t offset) {
  for (auto &slot : vs_stage.reflection.cb_slots) {
    slot.set_variable(name, p, size, offset);
  }
  for (auto &slot : gs_stage.reflection.cb_slots) {
    slot.set_variable(name, p, size, offset);
  }
  for (auto &slot : ps_stage.reflection.cb_slots) {
    slot.set_variable(name, p, size, offset);
  }
}

void Pipeline::update(const ComPtr<ID3D11DeviceContext> &context) {
  for (size_t i = 0; i < vs_stage.cb.size(); ++i) {
    auto &bs = vs_stage.reflection.cb_slots[i].backing_store;
    vs_stage.cb[i].update(context, bs.data(), bs.size());
  }
  for (size_t i = 0; i < gs_stage.cb.size(); ++i) {
    auto &bs = gs_stage.reflection.cb_slots[i].backing_store;
    gs_stage.cb[i].update(context, bs.data(), bs.size());
  }
  for (size_t i = 0; i < ps_stage.cb.size(); ++i) {
    auto &bs = ps_stage.reflection.cb_slots[i].backing_store;
    ps_stage.cb[i].update(context, bs.data(), bs.size());
  }
}

void Pipeline::setup(const ComPtr<ID3D11DeviceContext> &context) {
  // vs
  context->VSSetShader(_vs.Get(), nullptr, 0);

  _tmp_list.clear();
  for (auto &slot : vs_stage.cb) {
    _tmp_list.push_back(slot.buffer.Get());
  }
  context->VSSetConstantBuffers(0, static_cast<UINT>(_tmp_list.size()),
                                _tmp_list.data());
  context->IASetInputLayout(_input_layout.Get());

  // gs
  if (_gs) {
    context->GSSetShader(_gs.Get(), nullptr, 0);
    _tmp_list.clear();
    for (auto &slot : gs_stage.cb) {
      _tmp_list.push_back(slot.buffer.Get());
    }
    context->GSSetConstantBuffers(0, static_cast<UINT>(_tmp_list.size()),
                                  _tmp_list.data());

  } else {
    context->GSSetShader(0, nullptr, 0);
  }

  // ps
  context->PSSetShader(_ps.Get(), nullptr, 0);
  _tmp_list.clear();
  for (auto &slot : ps_stage.cb) {
    _tmp_list.push_back(slot.buffer.Get());
  }
  context->PSSetConstantBuffers(0, static_cast<UINT>(_tmp_list.size()),
                                _tmp_list.data());
}

void Pipeline::draw_empty(const ComPtr<ID3D11DeviceContext> &context) {
  context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
  context->Draw(1, 0);
}

} // namespace gorilla
