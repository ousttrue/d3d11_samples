#include <app.h>
#include <banana/geometry.h>
#include <banana/mesh.h>
#include <banana/orbit_camera.h>
#include <gorilla/window.h>
#include <memory>
#include <renderer.h>
#include <teapot.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "LightingPixelADS";
auto WIDTH = 320;
auto HEIGHT = 320;

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

  // setup pipeline
  auto node = std::make_shared<banana::Node>();
  node->transform.translation.y = 0.4f;

  auto mesh = std::make_shared<banana::Mesh>();
  mesh->assign(teapot::vertices(), teapot::indices());
  node->mesh = mesh;

  // node->mesh = banana::geometry::create_cube(0.4f);

  auto &submesh = node->mesh->submeshes.emplace_back(banana::SubMesh{});
  submesh.draw_offset = 0;
  submesh.draw_count = static_cast<UINT>(node->mesh->indices.size());
  submesh.material = std::make_shared<banana::Material>();
  submesh.material->shader_name = "lighting/pixel_ads.hlsl";
  submesh.material->properties[banana::Semantics::MATERIAL_COLOR] =
      banana::Float3(0.4f, 0.8f, 0.6f);
  submesh.material->properties[banana::Semantics::MATERIAL_AMBIENT] =
      banana::Float3(0.1f, 0.1f, 0.1f);
  submesh.material->properties[banana::Semantics::MATERIAL_SPECULAR] =
      banana::Float4(1.0f, 1.0f, 1.0f, 10.0f);

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
  auto context = app.context();
  banana::OrbitCamera camera;
  Renderer renderer;
  gorilla::ScreenState state;
  while (app.begin_frame(&state)) {
    update_camera(&camera, state);
    renderer.render(device, context, node, &camera, lights);
    app.end_frame();
  }

  return 0;
}
