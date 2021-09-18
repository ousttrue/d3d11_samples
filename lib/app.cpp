#include "app.h"
#include <banana/asset.h>
#include <gorilla/device.h>
#include <gorilla/pipeline.h>
#include <gorilla/swapchain.h>
#include <iostream>

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

  _window.bind_key([hwnd = _hwnd](int key) {
    if (key == 27) {
      // esc
      SendMessage(hwnd, WM_CLOSE, 0, 0);

    } else {
      std::cout << key << std::endl;
    }
  });

  // gizmo
  auto shader = banana::get_string("grid.hlsl");
  if (shader.empty()) {
    return {};
  }
  auto [ok, error] =
      _grid.compile_shader(_device, shader, "vsMain", "gsMain", "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    return {};
  }

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

  // gizmo
#pragma pack(push)
#pragma pack(16)
  struct Constants {
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
    DirectX::XMFLOAT3 cameraPosition;
    float _padding2;
    DirectX::XMFLOAT2 screenSize;
    float fovY;
    float _padding3;
  };
#pragma pack(pop)
  static_assert(sizeof(Constants) == 16 * 10, "sizeof ConstantsSize");
  Constants constant;
  constant.fovY = _camera._fovYRad;
  constant.screenSize.x = static_cast<float>(w);
  constant.screenSize.y = static_cast<float>(h);
  constant.view = _camera._view;
  constant.projection = _camera._projection;
  constant.cameraPosition = _camera.position();
  _grid.gs_stage.cb[0].update(_context, constant);
  _grid.ps_stage.cb[0].update(_context, constant);
  _grid.setup(_context);
  _grid.draw_empty(_context);

  // draw
  callback(_device, _context, _camera);

  // vsync
  _context->Flush();
  _swapchain->Present(1, 0);

  _frame_count++;
  return true;
}
