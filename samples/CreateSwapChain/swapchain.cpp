#include "swapchain.h"
#include <dxgi1_2.h>

namespace gorilla {

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

ComPtr<IDXGIFactory2> get_factory(const ComPtr<IDXGIDevice1> &pDXGIDevice) {

  ComPtr<IDXGIAdapter> pDXGIAdapter;
  auto hr = pDXGIDevice->GetAdapter(&pDXGIAdapter);
  if (FAILED(hr)) {
    return nullptr;
  }

  Microsoft::WRL::ComPtr<IDXGIFactory2> pDXGIFactory;
  hr = pDXGIAdapter->GetParent(IID_PPV_ARGS(&pDXGIFactory));
  if (FAILED(hr)) {
    return nullptr;
  }

  return pDXGIFactory;
}

ComPtr<IDXGISwapChain>
create_swapchain(const Microsoft::WRL::ComPtr<ID3D11Device> &device,
                 HWND hwnd) {

  ComPtr<IDXGIDevice1> pDXGIDevice;
  HRESULT hr = device.As(&pDXGIDevice);
  if (FAILED(hr)) {
    return nullptr;
  }

  auto pDXGIFactory = get_factory(pDXGIDevice);
  if (!pDXGIFactory) {
    return nullptr;
  }

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC sfd = {0};
  sfd.Windowed = TRUE;

  DXGI_SWAP_CHAIN_DESC1 sd = {0};
  sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
#if 1
  sd.BufferCount = 2;
  sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
#else
  sd.BufferCount = 1;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
#endif
  // sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  // sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

  ComPtr<IDXGISwapChain1> pSwapChain;
  hr = pDXGIFactory->CreateSwapChainForHwnd(pDXGIDevice.Get(), hwnd, &sd, &sfd,
                                            nullptr, pSwapChain.GetAddressOf());
  if (FAILED(hr)) {
    return nullptr;
  }

#if 1
  pDXGIFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES |
                                                DXGI_MWA_NO_ALT_ENTER);
#endif

  return pSwapChain;
}

} // namespace gorilla
