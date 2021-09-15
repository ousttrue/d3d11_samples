#include <DirectXMath.h>
#include <assert.h>
#include <banana/asset.h>
#include <gorilla/device.h>
#include <banana/image.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>
#include <gorilla/swapchain.h>
#include <gorilla/texture.h>
#include <gorilla/window.h>
#include <iostream>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);

  auto shader = banana::asset::get_string("texture.hlsl");
  if (shader.empty()) {
    return 7;
  }

  gorilla::Window window;
  auto hwnd = window.create(hInstance, "CLASS_NAME", "Texture", 320, 320);
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
  gorilla::ShaderVariables ps_slots;
  if (!ps_slots.reflect(ps)) {
    return 7;
  }
  assert(ps_slots.sampler_slots.size() == 1);
  assert(ps_slots.srv_slots.size() == 1);
  UINT sampler_slot = 0;
  UINT srv_slot = 0;

  auto image_bytes = banana::asset::get_bytes("image.png");
  banana::asset::Image image;
  if (!image.load(image_bytes)) {
    return 9;
  }

  gorilla::Texture texture;
  if (!texture.create(device, image.data(), image.width(), image.height())) {
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

      if (!render_target.create_rtv(device, backbuffer)) {
        assert(false);
      }
    }

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, w, h);

    // draw
    pipeline.setup(context);
    texture.set_ps(context, srv_slot, sampler_slot);
    pipeline.draw_empty(context);

    // vsync
    swapchain->Present(1, 0);
  }

  return 0;
}
