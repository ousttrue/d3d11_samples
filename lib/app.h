#pragma once

#include <banana/orbit_camera.h>
#include <d3d11.h>
#include <gorilla/render_target.h>
#include <gorilla/window.h>
#include <wrl/client.h>

class App {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  gorilla::Window _window;
  HWND _hwnd = nullptr;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _context;
  ComPtr<IDXGISwapChain> _swapchain;
  banana::OrbitCamera _camera;
  DXGI_SWAP_CHAIN_DESC _desc;
  UINT _frame_count = 0;
  gorilla::RenderTarget _render_target;

public:
  ComPtr<ID3D11Device> initialize(HINSTANCE hInstance, LPSTR lpCmdLine,
                                  int nCmdShow, const char *CLASS_NAME,
                                  const char *WINDOW_TITLE, int width = 320,
                                  int height = 320);
  bool
  new_frame(const std::function<void(const ComPtr<ID3D11Device> &,
                                     const ComPtr<ID3D11DeviceContext> &,
                                     const banana::OrbitCamera)> &callback);
};
