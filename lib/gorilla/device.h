#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace gorilla {

Microsoft::WRL::ComPtr<ID3D11Device>
create_device(const Microsoft::WRL::ComPtr<IDXGIAdapter> &adapter = nullptr);

} // namespace gorilla
