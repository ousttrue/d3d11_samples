#include "render_target.h"
#include "swapchain.h"
#include <DirectXMath.h>
#include <assert.h>
#include <device.h>
#include <window.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  swtk::Window window;
  auto hwnd = window.create(hInstance, "CLASS_NAME", "CreateSwapChain");
  if (!hwnd) {
    return 1;
  }

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  auto device = swtk::create_device();
  if (!device) {
    return 2;
  }
  ComPtr<ID3D11DeviceContext> context;
  device->GetImmediateContext(&context);

  auto swapchain = swtk::create_swapchain(device, hwnd);
  if (!swapchain) {
    return 3;
  }
  DXGI_SWAP_CHAIN_DESC desc;
  swapchain->GetDesc(&desc);

  swtk::RenderTarget render_target;

  for (UINT frame_count = 0; window.process_messages(); ++frame_count) {

    RECT rect;
    GetClientRect(hwnd, &rect);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    if (w != desc.BufferDesc.Width || h != desc.BufferDesc.Height) {
      // clear backbuffer reference
      render_target.release();
      // resize swapchain
      swapchain->ResizeBuffers(desc.BufferCount, w, h, desc.BufferDesc.Format,
                               desc.Flags);
    }

    // ensure create backbuffer
    if (!render_target.get()) {
      ComPtr<ID3D11Texture2D> backbuffer;
      auto hr = swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
      if (FAILED(hr)) {
        assert(false);
      }

      if (!render_target.create(device, backbuffer, false)) {
        assert(false);
      }
    }

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);

    // vsync
    swapchain->Present(1, 0);
  }

  return 0;
}
