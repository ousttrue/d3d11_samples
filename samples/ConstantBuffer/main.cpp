#include "pipeline.h"
#include "swapchain.h"
#include <DirectXMath.h>
#include <assert.h>
#include <device.h>
#include <iostream>
#include <render_target.h>
#include <shader.h>
#include <window.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla {
class ConstantBuffer {
  Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
  D3D11_BUFFER_DESC _desc = {0};

public:
  bool create(const ComPtr<ID3D11Device> &device, UINT size) {
    _desc.ByteWidth = size;
    _desc.Usage = D3D11_USAGE_DEFAULT;
    _desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    auto hr = device->CreateBuffer(&_desc, nullptr, &_buffer);
    if (FAILED(hr)) {
      return false;
    }
    return true;
  }

  void update(const ComPtr<ID3D11DeviceContext> &context, const void *p,
              UINT size) {
    assert(_desc.ByteWidth == size);
    context->UpdateSubresource(_buffer.Get(), 0, nullptr, p, 0, 0);
  }

  void set_gs(const ComPtr<ID3D11DeviceContext> &context, int slot) {
    ID3D11Buffer *cbs[1] = {_buffer.Get()};
    context->GSSetConstantBuffers(slot, 1, cbs);
  }
};
} // namespace gorilla

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);

  std::string shader = gorilla::read_file(lpCmdLine);
  if (shader.empty()) {
    return 7;
  }

  gorilla::Window window;
  auto hwnd = window.create(hInstance, "CLASS_NAME", "ConstantBuffer", 320, 320);
  if (!hwnd) {
    return 1;
  }

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  auto device = gorilla::create_device();
  if (!device) {
    return 2;
  }
  ComPtr<ID3D11DeviceContext> context;
  device->GetImmediateContext(&context);

  auto swapchain = gorilla::create_swapchain(device, hwnd);
  if (!swapchain) {
    return 3;
  }

  // setup pipeline
  gorilla::Pipeline pipeline;
  if (!pipeline.compile_vs(device, "vs", shader, "vsMain")) {
    return 4;
  }
  if (!pipeline.compile_gs(device, "gs", shader, "gsMain")) {
    return 5;
  }
  if (!pipeline.compile_ps(device, "ps", shader, "psMain")) {
    return 6;
  }
  DirectX::XMFLOAT4 xywh;
  gorilla::ConstantBuffer cb;
  if (!cb.create(device, sizeof(xywh))) {
    return 10;
  }

  // main loop
  DXGI_SWAP_CHAIN_DESC desc;
  swapchain->GetDesc(&desc);
  gorilla::RenderTarget render_target;
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

    // update
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(hwnd, &point);
    xywh.x = static_cast<float>(point.x);
    xywh.y = static_cast<float>(point.y);
    xywh.z = static_cast<float>(w);
    xywh.w = static_cast<float>(h);
    cb.update(context, &xywh, sizeof(xywh));

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, w, h);

    // draw
    pipeline.setup(context);
    cb.set_gs(context, 0);
    pipeline.draw_empty(context);

    // vsync
    swapchain->Present(1, 0);
  }

  return 0;
}
