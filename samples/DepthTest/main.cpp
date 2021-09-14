#include <DirectXMath.h>
#include <assert.h>
#include <gorilla/asset.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/device.h>
#include <gorilla/input_assembler.h>
#include <gorilla/orbit_camera.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>
#include <gorilla/swapchain.h>
#include <gorilla/window.h>
#include <iostream>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct Vertex {
  DirectX::XMFLOAT4 POSITION;
  DirectX::XMFLOAT4 COLOR;
  DirectX::XMFLOAT2 UV;
};
auto size = 0.4f;
Vertex vertices[] = {
    // x
    {DirectX::XMFLOAT4(-size, -size, -size, 1.0f),
     DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
    {DirectX::XMFLOAT4(-size, -size, size, 1.0f),
     DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
    {DirectX::XMFLOAT4(-size, size, size, 1.0f),
     DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
    {DirectX::XMFLOAT4(-size, size, -size, 1.0f),
     DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},

    {DirectX::XMFLOAT4(size, -size, -size, 1.0f),
     DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
    {DirectX::XMFLOAT4(size, size, -size, 1.0f),
     DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},
    {DirectX::XMFLOAT4(size, size, size, 1.0f),
     DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
    {DirectX::XMFLOAT4(size, -size, size, 1.0f),
     DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
    // y
    {DirectX::XMFLOAT4(-size, size, -size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
    {DirectX::XMFLOAT4(-size, size, size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
    {DirectX::XMFLOAT4(size, size, size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
    {DirectX::XMFLOAT4(size, size, -size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},

    {DirectX::XMFLOAT4(-size, -size, -size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
    {DirectX::XMFLOAT4(size, -size, -size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},
    {DirectX::XMFLOAT4(size, -size, size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
    {DirectX::XMFLOAT4(-size, -size, size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
    // z
    {DirectX::XMFLOAT4(-size, -size, -size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(0, 1)},
    {DirectX::XMFLOAT4(-size, size, -size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(0, 0)},
    {DirectX::XMFLOAT4(size, size, -size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(1, 0)},
    {DirectX::XMFLOAT4(size, -size, -size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(1, 1)},

    {DirectX::XMFLOAT4(-size, -size, size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
    {DirectX::XMFLOAT4(size, -size, size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},
    {DirectX::XMFLOAT4(size, size, size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
    {DirectX::XMFLOAT4(-size, size, size, 1.0f),
     DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
};
unsigned int indices[] = {
    0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
    12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  auto shader = gorilla::assets::get_string("depth.hlsl");
  if (shader.empty()) {
    return 1;
  }

  gorilla::Window window;
  auto hwnd =
      window.create(hInstance, "WINDOW_CLASS", "InputAssembler", 320, 320);
  if (!hwnd) {
    return 2;
  }

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  auto device = gorilla::create_device();
  if (!device) {
    return 3;
  }
  ComPtr<ID3D11DeviceContext> context;
  device->GetImmediateContext(&context);

  auto swapchain = gorilla::create_swapchain(device, hwnd);
  if (!swapchain) {
    return 4;
  }

  // setup pipeline
  gorilla::Pipeline pipeline;
  auto [compiled, vserror] =
      pipeline.compile_vs(device, "vs", shader, "vsMain");
  if (!compiled) {
    if (vserror) {
      std::cerr << (const char *)vserror->GetBufferPointer() << std::endl;
    }
    return 5;
  }
  auto input_layout = gorilla::create_input_layout(device, compiled);
  if (!input_layout) {
    return 6;
  }
  auto [ps, pserror] = pipeline.compile_ps(device, "ps", shader, "psMain");
  if (!ps) {
    if (pserror) {
      std::cerr << (const char *)pserror->GetBufferPointer() << std::endl;
    }
    return 7;
  }

  gorilla::InputAssembler ia;
  if (!ia.create_vertices(device, input_layout, vertices, sizeof(vertices),
                          _countof(vertices))) {
    return 8;
  }
  if (!ia.create_indices(device, indices, sizeof(indices), _countof(indices))) {
    return 8;
  }

  gorilla::ConstantBuffer cb;
  if (!cb.create(device, sizeof(DirectX::XMFLOAT4X4))) {
    return 9;
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

  ComPtr<ID3D11RasterizerState> rs;
  D3D11_RASTERIZER_DESC rs_desc = {};
  rs_desc.CullMode = D3D11_CULL_NONE;
  rs_desc.FillMode = D3D11_FILL_SOLID;
  rs_desc.FrontCounterClockwise = true;
  rs_desc.ScissorEnable = false;
  rs_desc.MultisampleEnable = false;
  if (FAILED(device->CreateRasterizerState(&rs_desc, &rs))) {
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
      if (!render_target.create_dsv(device)) {
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

    context->RSSetState(rs.Get());
    pipeline.setup(context);
    cb.set_vs(context, cb_slot);
    ia.draw(context);

    // vsync
    context->Flush();
    swapchain->Present(1, 0);
  }

  return 0;
}
