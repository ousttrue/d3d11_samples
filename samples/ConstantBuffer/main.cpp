#include <DirectXMath.h>
#include <assert.h>
#include <banana/asset.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/device.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>
#include <gorilla/swapchain.h>
#include <gorilla/window.h>
#include <iostream>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "ConstantBuffer";
auto WIDTH = 320;
auto HEIGHT = 320;

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);

  auto shader = banana::get_string("constant.hlsl");
  if (shader.empty()) {
    return 7;
  }

  gorilla::Window window;
  auto hwnd = window.create(hInstance, CLASS_NAME, WINDOW_TITLE, WIDTH, HEIGHT);
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
  auto [ok, error] =
      pipeline.compile_shader(device, shader, "vsMain", "gsMain", "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    return 4;
  }
  assert(pipeline.ps_stage.cb.size() == 0);
  DirectX::XMFLOAT4 xywh;
  assert(pipeline.gs_stage.cb.size() == 1);
  assert(pipeline.gs_stage.cb[0].desc.ByteWidth == sizeof(xywh));

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

      if (!render_target.create_rtv(device, backbuffer)) {
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
    pipeline.gs_stage.cb[0].update(context, &xywh, sizeof(xywh));

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, w, h);

    // draw
    pipeline.setup(context);
    pipeline.draw_empty(context);

    // vsync
    swapchain->Present(1, 0);
  }

  return 0;
}
