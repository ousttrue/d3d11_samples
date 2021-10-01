#pragma once
#include "drawable.h"
#include "render_target.h"
#include "window.h"
#include <banana/orbit_camera.h>
#include <d3d11.h>
#include <tuple>
#include <wrl/client.h>

namespace gorilla {

class DeviceAndTarget {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _context;
  ComPtr<IDXGISwapChain> _swapchain;
  DXGI_SWAP_CHAIN_DESC _desc;
  gorilla::RenderTarget _render_target;

public:
  std::tuple<ComPtr<ID3D11Device>, ComPtr<ID3D11DeviceContext>>
  create(HWND hwnd);
  void begin_frame(const ScreenState &state, const float clear[4]);

  void end_frame();
  void clear_depth();
};

} // namespace gorilla
