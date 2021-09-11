#include "input_layout.h"
#include <d3d11shader.h>
#include <d3dcompiler.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace swtk {

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
  // vertex shader reflection
  ComPtr<ID3D11ShaderReflection> pReflector;
  auto hr = D3DReflect(vblob->GetBufferPointer(), vblob->GetBufferSize(),
                       IID_ID3D11ShaderReflection, &pReflector);
  if (FAILED(hr)) {
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
  // if (!vbElement.empty()) {
  //   hr = pDevice->CreateInputLayout(
  //       &vbElement[0], static_cast<UINT>(vbElement.size()),
  //       vblob->GetBufferPointer(), vblob->GetBufferSize(), &m_pInputLayout);
  //   if (FAILED(hr))
  //     return false;
  // }
}

ComPtr<ID3D11InputLayout>
create_input_layout(const ComPtr<ID3D11Device> &device,
                    const ComPtr<ID3DBlob> &compiled) {
  auto elements = swtk::get_elements(compiled);
  ComPtr<ID3D11InputLayout> input_layout;
  auto hr = device->CreateInputLayout(elements.data(), elements.size(),
                                      compiled->GetBufferPointer(),
                                      compiled->GetBufferSize(), &input_layout);
  if (FAILED(hr)) {
    return {};
  }
  return input_layout;
}

} // namespace swtk
