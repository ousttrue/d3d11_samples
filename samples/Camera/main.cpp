#include <DirectXMath.h>
#include <assert.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/device.h>
#include <gorilla/orbit_camera.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/shader.h>
#include <gorilla/swapchain.h>
#include <gorilla/window.h>
#include <iostream>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

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
  gorilla::MouseBinder binder(camera);
  window.bind_mouse(
      std::bind(&gorilla::MouseBinder::Left, &binder, std::placeholders::_1),
      std::bind(&gorilla::MouseBinder::Middle, &binder, std::placeholders::_1),
      std::bind(&gorilla::MouseBinder::Right, &binder, std::placeholders::_1),
      std::bind(&gorilla::MouseBinder::Move, &binder, std::placeholders::_1,
                std::placeholders::_2),
      std::bind(&gorilla::MouseBinder::Wheel, &binder, std::placeholders::_1));

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