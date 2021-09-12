#pragma once

#include <d3d11.h>
#include <vector>
#include <wrl/client.h>

namespace gorilla {

std::vector<D3D11_INPUT_ELEMENT_DESC>
get_elements(const Microsoft::WRL::ComPtr<ID3DBlob> &vblob);

Microsoft::WRL::ComPtr<ID3D11InputLayout>
create_input_layout(const Microsoft::WRL::ComPtr<ID3D11Device> &device,
                    const Microsoft::WRL::ComPtr<ID3DBlob> &compiled);
} // namespace gorilla
