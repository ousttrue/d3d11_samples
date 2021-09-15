#include <gorilla/pipeline.h>
#include <gorilla/shader.h>

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
  return {compiled, {}};
}

void Pipeline::setup(const ComPtr<ID3D11DeviceContext> &context) {
  // vs
  context->VSSetShader(_vs.Get(), nullptr, 0);

  // gs
  if (_gs) {
    context->GSSetShader(_gs.Get(), nullptr, 0);
  } else {
    context->GSSetShader(0, nullptr, 0);
  }

  // ps
  context->PSSetShader(_ps.Get(), nullptr, 0);
}

void Pipeline::draw_empty(const ComPtr<ID3D11DeviceContext> &context) {
  context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
  context->Draw(1, 0);
}

} // namespace gorilla
