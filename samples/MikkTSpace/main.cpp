#include "banana/orbit_camera.h"
#include "gorilla/window.h"
#include <app.h>
#include <banana/gltf.h>
#include <renderer.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "MikkTSpace";

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

  banana::gltf::GltfLoader loader;
  if (!loader.load_from_asset("glTF-Sample-Models/2.0/DamagedHelmet/"
                              "glTF-Binary/DamagedHelmet.glb")) {
    return 2;
  }
  auto root = loader.scenes[0].nodes[0];
  root->mesh->submeshes[0].material->shader_name = "lighting/tspace.hlsl";
  
  // adjust
  banana::AABB aabb;
  root->calc_aabb(banana::Matrix4x4::identity(), &aabb);
  banana::Float3 move{0, -aabb.min.y, 0};
  root->transform.translation -= move;
  aabb.min += move;
  aabb.max += move;

  banana::OrbitCamera camera;
  camera.fit(aabb);

  // world
  banana::LightInfo lights[5] = {0};
  lights[0].intensity = banana::Float3{1, 1, 1};
  lights[0].position = banana::Float3{0.5, -1, -1};
  lights[0].is_point = 0;
  lights[1].intensity = banana::Float3{0, 0, 0};
  lights[2].intensity = banana::Float3{0, 0, 0};
  lights[3].intensity = banana::Float3{0, 0, 0};
  lights[4].intensity = banana::Float3{0, 0, 0};

  // main loop
  Renderer renderer;
  gorilla::ScreenState state;
  auto context = app.context();
  while (app.begin_frame(&state)) {
    update_camera(&camera, state);
    renderer.render(device, context, root, &camera, lights);
    app.end_frame();
  }

  return 0;
}
