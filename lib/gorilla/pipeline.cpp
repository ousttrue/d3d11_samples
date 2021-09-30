#include <assert.h>
#include <gorilla/pipeline.h>
#include <gorilla/shader.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla {

std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
Pipeline::compile_vs(const ComPtr<ID3D11Device> &device, const char *name,
                     std::string_view source, const char *entry_point, const D3D_SHADER_MACRO *define) {
  auto [compiled, error] = gorilla::compile_vs(name, source, entry_point, define);
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
                     std::string_view source, const char *entry_point, const D3D_SHADER_MACRO *define) {
  auto [compiled, error] = gorilla::compile_gs(name, source, entry_point, define);
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
                     std::string_view source, const char *entry_point, const D3D_SHADER_MACRO *define) {
  auto [compiled, error] = gorilla::compile_ps(name, source, entry_point, define);
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
                         std::string_view source, const D3D_SHADER_MACRO *define, const char *vs_entry,
                         const char *gs_entry, const char *ps_entry) {
  {
    auto [compiled, error] = compile_vs(device, "vs", source, vs_entry, define);
    if (!compiled) {
      return {false, (const char *)error->GetBufferPointer()};
    }
  }
  if (gs_entry) {
    auto [compiled, error] = compile_gs(device, "gs", source, gs_entry, define);
    if (!compiled) {
      return {false, (const char *)error->GetBufferPointer()};
    }
  }
  {
    auto [compiled, error] = compile_ps(device, "ps", source, ps_entry, define);
    if (!compiled) {
      return {false, (const char *)error->GetBufferPointer()};
    }
  }

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
