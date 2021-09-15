#include "scene_renderer.h"
#include <DirectXMath.h>
#include <assert.h>
#include <banana/asset.h>
#include <banana/glb.h>
#include <banana/gltf.h>
#include <banana/image.h>
#include <banana/orbit_camera.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/device.h>
#include <gorilla/input_assembler.h>
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

  //
  // scene
  //
  auto bytes = banana::get_bytes(
      // "glTF-Sample-Models/2.0/BoxTextured/glTF-Binary/BoxTextured.glb"
      // "glTF-Sample-Models/2.0/Avocado/glTF-Binary/Avocado.glb"
      "glTF-Sample-Models/2.0/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"
      //
  );
  if (bytes.empty()) {
    return 10;
  }
  banana::gltf::Glb glb;
  if (!glb.parse(bytes)) {
    return 11;
  }
  banana::gltf::GltfLoader loader(glb.json, glb.bin);
  if (!loader.load()) {
    return 12;
  }

  auto root = loader.scenes[0].nodes[0];

  banana::OrbitCamera camera;
  banana::MouseBinder binder(camera);
  window.bind_mouse(
      std::bind(&banana::MouseBinder::Left, &binder, std::placeholders::_1),
      std::bind(&banana::MouseBinder::Middle, &binder, std::placeholders::_1),
      std::bind(&banana::MouseBinder::Right, &binder, std::placeholders::_1),
      std::bind(&banana::MouseBinder::Move, &binder, std::placeholders::_1,
                std::placeholders::_2),
      std::bind(&banana::MouseBinder::Wheel, &binder, std::placeholders::_1));

  SceneRenderer renderer;

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

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, w, h);

    // scene
    renderer.Render(device, context,
                    DirectX::XMLoadFloat4x4(&camera.projection()),
                    DirectX::XMLoadFloat4x4(&camera.view()),
                    DirectX::XMMatrixIdentity(), root);

    // vsync
    context->Flush();
    swapchain->Present(1, 0);
  }

  return 0;
}
