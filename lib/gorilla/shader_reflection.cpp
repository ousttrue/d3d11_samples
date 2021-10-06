#include "shader_reflection.h"
#include <d3d11shader.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla {

void ConstantBufferSlot::set_variable(std::string_view name, const void *p,
                                      size_t size, size_t offset) {
  for (auto &v : variables) {
    if (name == v.Name) {
      assert(v.StartOffset + offset + size <= backing_store.size());
      memcpy(backing_store.data() + v.StartOffset + offset, p, size);
      return;
    }
  }
}

static ComPtr<ID3D11ShaderReflection>
get_reflection(const ComPtr<ID3DBlob> &compiled) {
  ComPtr<ID3D11ShaderReflection> reflection;
  auto hr = D3DReflect(compiled->GetBufferPointer(), compiled->GetBufferSize(),
                       IID_ID3D11ShaderReflection, &reflection);
  if (FAILED(hr)) {
    return {};
  }
  return reflection;
}

static DXGI_FORMAT GetDxgiFormat(D3D10_REGISTER_COMPONENT_TYPE type,
                                 BYTE mask) {
  if ((mask & 0x0F) == 0x0F) {
    // xyzw
    switch (type) {
    case D3D10_REGISTER_COMPONENT_FLOAT32:
      return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }
  }

  if ((mask & 0x07) == 0x07) {
    // xyz
    switch (type) {
    case D3D10_REGISTER_COMPONENT_FLOAT32:
      return DXGI_FORMAT_R32G32B32_FLOAT;
    }
  }

  if ((mask & 0x03) == 0x03) {
    // xy
    switch (type) {
    case D3D10_REGISTER_COMPONENT_FLOAT32:
      return DXGI_FORMAT_R32G32_FLOAT;
    }
  }

  if ((mask & 0x1) == 0x1) {
    // x
    switch (type) {
    case D3D10_REGISTER_COMPONENT_FLOAT32:
      return DXGI_FORMAT_R32_FLOAT;
    }
  }

  return DXGI_FORMAT_UNKNOWN;
}

std::vector<D3D11_INPUT_ELEMENT_DESC>
get_elements(const ComPtr<ID3DBlob> &vblob) {

  auto pReflector = get_reflection(vblob);
  if (!pReflector) {
    return {};
  }

  // OutputDebugPrintfA("#### VertexShader ####\n");
  // if (!m_constant->Initialize(pDevice, SHADERSTAGE_VERTEX, pReflector)) {
  //   return false;
  // }

  D3D11_SHADER_DESC shaderdesc;
  pReflector->GetDesc(&shaderdesc);

  // Create InputLayout
  std::vector<D3D11_INPUT_ELEMENT_DESC> vbElement;
  for (UINT i = 0; i < shaderdesc.InputParameters; ++i) {
    D3D11_SIGNATURE_PARAMETER_DESC sigdesc;
    pReflector->GetInputParameterDesc(i, &sigdesc);

    auto format = GetDxgiFormat(sigdesc.ComponentType, sigdesc.Mask);

    D3D11_INPUT_ELEMENT_DESC eledesc = {
        sigdesc.SemanticName,
        sigdesc.SemanticIndex,
        format,
        0 // hardcoding
        ,
        D3D11_APPEND_ALIGNED_ELEMENT // hardcoding
        ,
        D3D11_INPUT_PER_VERTEX_DATA // hardcoding
        ,
        0 // hardcoding
    };
    vbElement.push_back(eledesc);
  }

  return vbElement;
}

ComPtr<ID3D11InputLayout>
create_input_layout(const ComPtr<ID3D11Device> &device,
                    const ComPtr<ID3DBlob> &compiled) {
  auto elements = gorilla::get_elements(compiled);
  if (elements.empty()) {
    return {};
  }
  ComPtr<ID3D11InputLayout> input_layout;
  auto hr = device->CreateInputLayout(
      elements.data(), static_cast<UINT>(elements.size()),
      compiled->GetBufferPointer(), compiled->GetBufferSize(), &input_layout);
  if (FAILED(hr)) {
    return {};
  }
  return input_layout;
}

bool ShaderReflection::reflect(const ComPtr<ID3DBlob> &compiled) {

  auto pReflector = get_reflection(compiled);
  if (!pReflector) {
    return false;
  }

  D3D11_SHADER_DESC shaderdesc;
  pReflector->GetDesc(&shaderdesc);

  // analyze constant buffer
  for (UINT i = 0; i < shaderdesc.ConstantBuffers; ++i) {
    auto cb = pReflector->GetConstantBufferByIndex(i);
    D3D11_SHADER_BUFFER_DESC desc;
    cb->GetDesc(&desc);
    // OutputDebugPrintfA("[%d: %s]\n", i, desc.Name);
    cb_slots.push_back({desc});

    for (UINT j = 0; j < desc.Variables; ++j) {
      auto v = cb->GetVariableByIndex(j);
      D3D11_SHADER_VARIABLE_DESC vdesc;
      v->GetDesc(&vdesc);
      vdesc.Name = cache_string(vdesc.Name);
      // OutputDebugPrintfA("(%d) %s %d\n", j, vdesc.Name, vdesc.StartOffset);
      cb_slots.back().variables.push_back(vdesc);
    }
  }

  for (UINT i = 0; i < shaderdesc.BoundResources; ++i) {
    D3D11_SHADER_INPUT_BIND_DESC desc = {};
    pReflector->GetResourceBindingDesc(i, &desc);
    desc.Name = cache_string(desc.Name);
    switch (desc.Type) {
    case D3D_SIT_TEXTURE:
      srv_slots.push_back(desc);
      break;

    case D3D_SIT_SAMPLER:
      sampler_slots.push_back(desc);
      break;
    }
  }

  return true;
}

} // namespace gorilla
