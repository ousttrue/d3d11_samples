#include "pipeline.h"
#include "swapchain.h"
#include <DirectXMath.h>
#include <assert.h>
#include <constant_buffer.h>
#include <device.h>
#include <iostream>
#include <limits>
#include <render_target.h>
#include <shader.h>
#include <window.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla {

class OrbitCamera {
  float _w = 1;
  float _h = 1;
  float _near = 0.01f;
  float _far = 100.0f;
  float _fovYRad =  DirectX::XMConvertToRadians(60.0f);
  DirectX::XMFLOAT4X4 _projection;
  void calc_projection() {
    auto P = DirectX::XMMatrixPerspectiveFovRH(_fovYRad, _w / _h, _near, _far);
    DirectX::XMStoreFloat4x4(&_projection, P);
  }

  DirectX::XMFLOAT3 _shift = {0, 0, -5};
  float _yaw = 0;
  float _pitch = 0;
  DirectX::XMFLOAT4X4 _view;
  void calc_view() {
    auto Y = DirectX::XMMatrixRotationY(_yaw);
    auto P = DirectX::XMMatrixRotationX(_pitch);
    auto T = DirectX::XMMatrixTranslation(_shift.x, _shift.y, _shift.z);
    auto M = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(Y, P), T);
    DirectX::XMStoreFloat4x4(&_view, M);
  }

public:
  OrbitCamera() {
    calc_projection();
    calc_view();
  }

  DirectX::XMFLOAT4X4 matrix() const {
    auto V = DirectX::XMLoadFloat4x4(&_view);
    auto P = DirectX::XMLoadFloat4x4(&_projection);
    auto M = DirectX::XMMatrixMultiply(V, P);
    DirectX::XMFLOAT4X4 m;
    DirectX::XMStoreFloat4x4(&m, M);
    return m;
  }

  void yaw_pitch(int dx, int dy) {
    _yaw += dx/_h * 4;
    _pitch += dy/_h * 4;
    calc_view();
  }

  void shift(int dx, int dy) {
    _shift.x -= dx/_h * tan(_fovYRad/2) * _shift.z * 2;
    _shift.y += dy/_h * tan(_fovYRad/2) * _shift.z * 2;
    calc_view();
  }

  void dolly(int d) {
    if (d < 0) {
      _shift.z *= 1.1f;
    } else if (d > 0) {
      _shift.z *= 0.9f;
    }
    calc_view();
  }

  void resize(float w, float h) {
    if (w == _w && h == _h) {
      return;
    }
    _w = w;
    _h = h;
    calc_projection();
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
  auto hwnd = window.create(hInstance, "CLASS_NAME", "BasicPipeline", 320, 320);
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
  auto [vs, vserror] = pipeline.compile_vs(device, "vs", shader, "vsMain");
  if (!vs) {
    if (vserror) {
      std::cerr << (const char *)vserror->GetBufferPointer() << std::endl;
    }
    return 4;
  }
  auto [gs, gserror] = pipeline.compile_gs(device, "gs", shader, "gsMain");
  if (!gs) {
    if (gserror) {
      std::cerr << (const char *)gserror->GetBufferPointer() << std::endl;
    }
    return 5;
  }
  auto [ps, pserror] = pipeline.compile_ps(device, "ps", shader, "psMain");
  if (!ps) {
    if (pserror) {
      std::cerr << (const char *)pserror->GetBufferPointer() << std::endl;
    }
    return 6;
  }
  gorilla::ConstantBuffer cb;
  if (!cb.create(device, sizeof(DirectX::XMFLOAT4X4))) {
    return 7;
  }
  UINT cb_slot = 0;
  gorilla::OrbitCamera camera;
  struct MouseBinder {
    bool _left = false;
    bool _right = false;
    bool _middle = false;
    int _x = std::numeric_limits<int>::min();
    int _y = std::numeric_limits<int>::min();
    gorilla::OrbitCamera &_camera;

  public:
    MouseBinder(gorilla::OrbitCamera &camera) : _camera(camera) {}
    void Left(bool isPress) { _left = isPress; }
    void Middle(bool isPress) { _middle = isPress; }
    void Right(bool isPress) { _right = isPress; }
    void Move(int x, int y) {
      if (_x == std::numeric_limits<int>::min()) {
        _x = x;
        _y = y;
        return;
      }

      auto dx = x - _x;
      _x = x;
      auto dy = y - _y;
      _y = y;
      if (_right) {
        _camera.yaw_pitch(dx, dy);
      }
      if (_middle) {
        _camera.shift(dx, dy);
      }
    }
    void Wheel(int d) { _camera.dolly(d); }
  };
  MouseBinder binder(camera);

  window.bind_mouse(
      std::bind(&MouseBinder::Left, &binder, std::placeholders::_1),
      std::bind(&MouseBinder::Middle, &binder, std::placeholders::_1),
      std::bind(&MouseBinder::Right, &binder, std::placeholders::_1),
      std::bind(&MouseBinder::Move, &binder, std::placeholders::_1,
                std::placeholders::_2),
      std::bind(&MouseBinder::Wheel, &binder, std::placeholders::_1));

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
    camera.resize(static_cast<float>(w), static_cast<float>(h));
    cb.update(context, camera.matrix());

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, w, h);

    // draw
    pipeline.setup(context);
    cb.set_gs(context, cb_slot);
    pipeline.draw_empty(context);

    // vsync
    swapchain->Present(1, 0);
  }

  return 0;
}
