#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <list>
#include <string>
#include <string_view>
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
  std::vector<uint8_t> backing_store;

  ConstantBufferSlot(const D3D11_SHADER_BUFFER_DESC &d)
      : desc(d), backing_store(d.Size) {}

  void set_variable(std::string_view name, const void *p, size_t size) {
    for (auto &v : variables) {
      if (name == v.Name) {
        memcpy(backing_store.data() + v.StartOffset, p, size);
        return;
      }
    }
  }
};

class ShaderReflection {
  std::list<std::string> _string_buffer;
  char *cache_string(const char *src) {
    _string_buffer.push_back(src);
    return (char *)_string_buffer.back().data();
  }

public:
  std::vector<ConstantBufferSlot> cb_slots;
  std::vector<D3D11_SHADER_INPUT_BIND_DESC> srv_slots;
  std::vector<D3D11_SHADER_INPUT_BIND_DESC> sampler_slots;

  bool reflect(const Microsoft::WRL::ComPtr<ID3DBlob> &compiled);
};

} // namespace gorilla
