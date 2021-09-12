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
struct ConstantVariable {
  size_t Slot;
  std::string Name;
  D3D11_SHADER_VARIABLE_DESC Desc;

  ConstantVariable(){}
  ConstantVariable(size_t slot, const D3D11_SHADER_VARIABLE_DESC &desc)
  {
    Slot = slot;
    Name = desc.Name;
    Desc = desc;
  }
};

struct ShaderResourceSlot {
  std::string Name;
  D3D11_SHADER_INPUT_BIND_DESC Desc;
};

struct ConstantBufferSlot {
  std::vector<ConstantVariable> Variables;
  size_t _size;
  ConstantBufferSlot(size_t size) : _size(size) {}
};

class VariablesByStage {
public:
  std::vector<ConstantBufferSlot> cb_slots;
  std::vector<ShaderResourceSlot> srv_slots;
  std::vector<ShaderResourceSlot> sampler_slots;

  bool reflect(const Microsoft::WRL::ComPtr<ID3DBlob> &compiled);

private:


  bool GetCBVariable(const std::string &name, ConstantVariable *pOut) {
    for (auto &slot : cb_slots) {
      for (auto &v : slot.Variables) {
        if (name == v.Name) {
          if (pOut) {
            *pOut = v;
            return true;
          }
        }
      }
    }
    return false;
  }

  void AddSRVSlot(const D3D11_SHADER_INPUT_BIND_DESC &desc) {
    srv_slots.push_back(ShaderResourceSlot{desc.Name, desc});
  }

  void AddSamplerSlot(const D3D11_SHADER_INPUT_BIND_DESC &desc) {
    sampler_slots.push_back(ShaderResourceSlot{desc.Name, desc});
  }
};

} // namespace gorilla
