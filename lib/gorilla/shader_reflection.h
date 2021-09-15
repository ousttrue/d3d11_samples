#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <vector>
#include <wrl/client.h>

namespace gorilla {

// input
std::vector<D3D11_INPUT_ELEMENT_DESC>
get_elements(const Microsoft::WRL::ComPtr<ID3DBlob> &vblob);

Microsoft::WRL::ComPtr<ID3D11InputLayout>
create_input_layout(const Microsoft::WRL::ComPtr<ID3D11Device> &device,
                    const Microsoft::WRL::ComPtr<ID3DBlob> &compiled);

// constant
struct ConstantBufferSlot {
  D3D11_SHADER_BUFFER_DESC desc;
  std::vector<D3D11_SHADER_VARIABLE_DESC> variables;
};

class ShaderVariables {
public:
  std::vector<ConstantBufferSlot> cb_slots;
  std::vector<D3D11_SHADER_INPUT_BIND_DESC> srv_slots;
  std::vector<D3D11_SHADER_INPUT_BIND_DESC> sampler_slots;

  bool reflect(const Microsoft::WRL::ComPtr<ID3DBlob> &compiled);
};

} // namespace gorilla
