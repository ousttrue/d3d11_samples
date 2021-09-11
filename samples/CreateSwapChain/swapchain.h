#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace swtk
{

Microsoft::WRL::ComPtr<IDXGISwapChain>
create_swapchain(const Microsoft::WRL::ComPtr<ID3D11Device> &device, HWND hwnd);

}
