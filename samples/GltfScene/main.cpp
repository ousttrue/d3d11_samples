#include "banana/orbit_camera.h"
#include "gorilla/window.h"
#include <app.h>
#include <banana/gltf.h>
#include <imgui.h>
#include <renderer.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "GltfScene";

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class GltfSamples {
  std::shared_ptr<banana::Node> _root;
  std::string base = "glTF-Sample-Models/2.0/";

public:
  std::shared_ptr<banana::Node> root() const { return _root; }

  // "glTF-Sample-Models/2.0/BoxTextured/glTF-Binary/BoxTextured.glb"
  // "glTF-Sample-Models/2.0/Avocado/glTF-Binary/Avocado.glb"
  // "glTF-Sample-Models/2.0/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"
  // "glTF-Sample-Models/2.0/CesiumMilkTruck/glTF-Binary/CesiumMilkTruck.glb"
  bool load(const std::string &sample, banana::OrbitCamera *camera) {
    auto key = base + sample;
    banana::gltf::GltfLoader loader;
    if (!loader.load_from_asset(key)) {
      return {};
    }
    _root = loader.scenes[0].nodes[0];

    banana::AABB aabb;
    _root->calc_aabb(banana::Matrix4x4::identity(), &aabb);
    if (aabb.min.y < 0) {
      _root->transform.translation.y -= aabb.min.y;
    }
    auto half_height = aabb.height() / 2;
    camera->fit(half_height, half_height);

    return true;
  }
};

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

  banana::OrbitCamera camera;

  GltfSamples samples;
  if (!samples.load("CesiumMilkTruck/glTF-Binary/CesiumMilkTruck.glb", &camera)) {
    return 2;
  }

  // main loop
  Renderer renderer;
  auto context = app.context();
  gorilla::ScreenState state;
  while (app.begin_frame(&state)) {
    update_camera(&camera, state);

    if (ImGui::Begin("Some")) {
    }
    ImGui::End();

    renderer.render(device, context, samples.root(), &camera);
    app.end_frame();
  }

  return 0;
}
