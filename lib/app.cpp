#include "app.h"
#include <gorilla/device.h>
#include <gorilla/swapchain.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

ComPtr<ID3D11Device> App::initialize(HINSTANCE hInstance, LPSTR lpCmdLine,
                                     int nCmdShow, const char *CLASS_NAME,
                                     const char *WINDOW_TITLE, int width,
                                     int height) {
  _hwnd = _window.create(hInstance, CLASS_NAME, WINDOW_TITLE, width, height);
  if (!_hwnd) {
    return {};
  }

  ShowWindow(_hwnd, nCmdShow);
  UpdateWindow(_hwnd);

  _device = gorilla::create_device();
  if (!_device) {
    return {};
  }
  _device->GetImmediateContext(&_context);

  _swapchain = gorilla::create_swapchain(_device, _hwnd);
  if (!_swapchain) {
    return {};
  }
  _swapchain->GetDesc(&_desc);

  auto binder = std::make_shared<banana::MouseBinder>(_camera);
  _window.bind_mouse([binder](bool isPress) { binder->Left(isPress); },
                     [binder](bool isPress) { binder->Middle(isPress); },
                     [binder](bool isPress) { binder->Right(isPress); },
                     [binder](int x, int y) { binder->Move(x, y); },
                     [binder](int d) { binder->Wheel(d); });

  return _device;
}

bool App::new_frame(
    const std::function<void(const ComPtr<ID3D11Device> &,
                             const ComPtr<ID3D11DeviceContext> &,
                             const banana::OrbitCamera)> &callback) {

  if (!_window.process_messages()) {
    return false;
  }

  RECT rect;
  GetClientRect(_hwnd, &rect);
  int w = rect.right - rect.left;
  int h = rect.bottom - rect.top;
  if (w != _desc.BufferDesc.Width || h != _desc.BufferDesc.Height) {
    // clear backbuffer reference
    _render_target.release();
    // resize swapchain
    _swapchain->ResizeBuffers(_desc.BufferCount, w, h, _desc.BufferDesc.Format,
                              _desc.Flags);
  }

  // ensure create backbuffer
  if (!_render_target.get()) {
    ComPtr<ID3D11Texture2D> backbuffer;
    auto hr = _swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
    if (FAILED(hr)) {
      return false;
    }

    if (!_render_target.create_rtv(_device, backbuffer)) {
      return false;
    }
    if (!_render_target.create_dsv(_device)) {
      return false;
    }
  }

  _camera.resize(static_cast<float>(w), static_cast<float>(h));
  // clear RTV
  auto v =
      (static_cast<float>(sin(_frame_count / 180.0f * DirectX::XM_PI)) + 1) *
      0.5f;
  float clear[] = {0.5, v, 0.5, 1.0f};
  _render_target.clear(_context, clear);
  _render_target.setup(_context, w, h);

  callback(_device, _context, _camera);

  // vsync
  _context->Flush();
  _swapchain->Present(1, 0);

  _frame_count++;
  return true;
}
