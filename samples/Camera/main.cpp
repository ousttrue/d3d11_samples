#include <DirectXMath.h>
#include <assert.h>
#include <banana/asset.h>
#include <banana/orbit_camera.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/device.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/shader.h>
#include <gorilla/swapchain.h>
#include <gorilla/window.h>
#include <iostream>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "Camera";
auto WIDTH = 320;
auto HEIGHT = 320;

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);

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
  auto shader = banana::get_asset("mvp.hlsl");
  if (!shader) {
    return 7;
  }
  gorilla::Pipeline pipeline;
  auto [ok, error] =
      pipeline.compile_shader(device, shader, "vsMain", "gsMain", "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    return 4;
  }

  banana::OrbitCamera camera;

  // main loop
  DXGI_SWAP_CHAIN_DESC desc;
  swapchain->GetDesc(&desc);
  gorilla::RenderTarget render_target;
  gorilla::ScreenState state;
  for (UINT frame_count = 0; window.process_messages(&state); ++frame_count) {

    if (state.width != desc.BufferDesc.Width ||
        state.height != desc.BufferDesc.Height) {
      // clear backbuffer reference
      render_target.release();
      // resize swapchain
      swapchain->ResizeBuffers(desc.BufferCount, static_cast<UINT>(state.width),
                               static_cast<UINT>(state.height),
                               desc.BufferDesc.Format, desc.Flags);
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
    camera.update(state.mouse_x, state.mouse_y, state.width, state.height,
                  state.mouse_button_flag & gorilla::MouseButtonLeftDown,
                  state.mouse_button_flag & gorilla::MouseButtonRightDown,
                  state.mouse_button_flag & gorilla::MouseButtonMiddleDown,
                  state.wheel);

    pipeline.gs_stage.cb[0].update(context, camera.view * camera.projection);

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, state.width, state.height);

    // draw
    pipeline.setup(context);
    pipeline.draw_empty(context);

    // vsync
    swapchain->Present(1, 0);
  }

  return 0;
}
