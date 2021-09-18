#include <DirectXMath.h>
#include <app.h>
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
#include <scene_renderer.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "GltfScene";

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  App app;
  auto device =
      app.initialize(hInstance, lpCmdLine, nCmdShow, CLASS_NAME, WINDOW_TITLE);
  if (!device) {
    return 1;
  }

  //
  // scene
  //
  auto bytes = banana::get_bytes(
      // "glTF-Sample-Models/2.0/BoxTextured/glTF-Binary/BoxTextured.glb"
      // "glTF-Sample-Models/2.0/Avocado/glTF-Binary/Avocado.glb"
      // "glTF-Sample-Models/2.0/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"
      "glTF-Sample-Models/2.0/CesiumMilkTruck/glTF-Binary/CesiumMilkTruck.glb"
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

  SceneRenderer renderer;

  // main loop
  auto context = app.context();
  auto camera = app.camera();
  while (app.begin_frame()) {
    renderer.Render(device, context,
                    DirectX::XMLoadFloat4x4(&camera->projection()),
                    DirectX::XMLoadFloat4x4(&camera->view()),
                    DirectX::XMMatrixIdentity(), root);

    app.end_frame();
  }

  return 0;
}
