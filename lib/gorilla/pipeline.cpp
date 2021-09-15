#include <assert.h>
#include <gorilla/pipeline.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla {

std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
Pipeline::compile_vs(const ComPtr<ID3D11Device> &device, const char *name,
                     std::string_view source, const char *entry_point) {
  auto [compiled, error] = gorilla::compile_vs(name, source, entry_point);
  if (!compiled) {
    return {{}, error};
  }
  auto hr =
      device->CreateVertexShader((DWORD *)compiled->GetBufferPointer(),
                                 compiled->GetBufferSize(), nullptr, &_vs);
  if (FAILED(hr)) {
    return {};
  }

  create_cb(vs_cb, device, compiled);

  _input_layout = gorilla::create_input_layout(device, compiled);
  // if (!_input_layout) {
  //   return {};
  // }

  return {compiled, {}};
}

std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
Pipeline::compile_gs(const ComPtr<ID3D11Device> &device, const char *name,
                     std::string_view source, const char *entry_point) {
  auto [compiled, error] = gorilla::compile_gs(name, source, entry_point);
  if (!compiled) {
    return {{}, error};
  }
  auto hr =
      device->CreateGeometryShader((DWORD *)compiled->GetBufferPointer(),
                                   compiled->GetBufferSize(), nullptr, &_gs);
  if (FAILED(hr)) {
    return {};
  }

  create_cb(gs_cb, device, compiled);

  return {compiled, {}};
}

std::tuple<ComPtr<ID3DBlob>, ComPtr<ID3DBlob>>
Pipeline::compile_ps(const ComPtr<ID3D11Device> &device, const char *name,
                     std::string_view source, const char *entry_point) {
  auto [compiled, error] = gorilla::compile_ps(name, source, entry_point);
  if (!compiled) {
    return {{}, error};
  }
  auto hr = device->CreatePixelShader((DWORD *)compiled->GetBufferPointer(),
                                      compiled->GetBufferSize(), nullptr, &_ps);
  if (FAILED(hr)) {
    return {};
  }

  create_cb(ps_cb, device, compiled);

  return {compiled, {}};
}

void Pipeline::create_cb(std::vector<ConstantBuffer> &buffers,
                         const ComPtr<ID3D11Device> &device,
                         const ComPtr<ID3DBlob> &compiled) {
  gorilla::ShaderVariables reflection;
  if (!reflection.reflect(compiled)) {
    assert(false);
    return;
  }

  for (auto &slot : reflection.cb_slots) {
    auto &cb = buffers.emplace_back(ConstantBuffer{});
    cb.create(device, slot.desc.Size);
  }
}

void Pipeline::setup(const ComPtr<ID3D11DeviceContext> &context) {
  // vs
  context->VSSetShader(_vs.Get(), nullptr, 0);

  _tmp_list.clear();
  for (auto &slot : vs_cb) {
    _tmp_list.push_back(slot.buffer.Get());
  }
  context->VSSetConstantBuffers(0, static_cast<UINT>(_tmp_list.size()),
                                _tmp_list.data());
  context->IASetInputLayout(_input_layout.Get());

  // gs
  if (_gs) {
    context->GSSetShader(_gs.Get(), nullptr, 0);
    _tmp_list.clear();
    for (auto &slot : gs_cb) {
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
  for (auto &slot : ps_cb) {
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
