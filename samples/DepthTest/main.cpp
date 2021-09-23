#include "app.h"
#include "banana/types.h"
#include <DirectXMath.h>
#include <assert.h>
#include <banana/asset.h>
#include <banana/geometry.h>
#include <banana/orbit_camera.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/device.h>
#include <gorilla/input_assembler.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>
#include <gorilla/swapchain.h>
#include <gorilla/window.h>
#include <iostream>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "DepthTest";
auto WIDTH = 320;
auto HEIGHT = 320;

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  gorilla::Window window;
  auto hwnd =
      window.create(hInstance, "WINDOW_CLASS", WINDOW_TITLE, WIDTH, HEIGHT);
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
  auto shader = banana::get_string("depth.hlsl");
  if (shader.empty()) {
    return 1;
  }
  auto [ok, error] =
      pipeline.compile_shader(device, shader, "vsMain", {}, "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    return 5;
  }

  auto cube = banana::geometry::create_cube(0.4f);

  gorilla::InputAssembler ia;
  if (!ia.create_vertices(device, cube->vertex_stride, cube->vertices.data(),
                          cube->vertices.size())) {
    return 6;
  }
  if (!ia.create_indices(device, cube->index_stride, cube->indices.data(),
                         cube->indices.size())) {
    return 7;
  }

  banana::OrbitCamera camera;

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
      if (!render_target.create_dsv(device)) {
        assert(false);
      }
    }

    // update
    update_camera(&camera, state);
    pipeline.vs_stage.cb[0].update(context, camera.view * camera.projection);

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, state.width, state.height);

    context->RSSetState(rs.Get());
    pipeline.setup(context);
    ia.draw(context);

    // vsync
    context->Flush();
    swapchain->Present(1, 0);
  }

  return 0;
}
