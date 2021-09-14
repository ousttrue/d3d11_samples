#include <DirectXMath.h>
#include <assert.h>
#include <gorilla/asset.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/device.h>
#include <gorilla/gltf.h>
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

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


  auto bytes = gorilla::assets::get_bytes(
      "glTF-Sample-Models/2.0/Box/glTF-Binary/Box.glb");
  if (bytes.empty()) {
    return 10;
  }
  auto shader = gorilla::assets::get_string("gltf.hlsl");
  if (shader.empty()) {
    return 1;
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

  gorilla::gltf::Glb glb;
  if (!glb.parse(bytes)) {
    return 11;
  }

  gorilla::gltf::GltfLoader loader;
  if (!loader.load(glb.json, glb.bin)) {
    return 12;
  }

  auto &mesh = loader.meshes[0];

  gorilla::InputAssembler ia;
  if (!ia.create_vertices(device, input_layout, mesh.vertices.data(),
                          mesh.vertices.size())) {
    return 13;
  }
  if (!ia.create_indices(device, mesh.indices.data(), mesh.indices.size())) {
    return 14;
  }

  gorilla::ConstantBuffer cb;
  if (!cb.create(device, sizeof(DirectX::XMFLOAT4X4))) {
    return 8;
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
    return 9;
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
