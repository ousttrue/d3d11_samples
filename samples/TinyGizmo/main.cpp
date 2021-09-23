#include "gorilla/input_assembler.h"
#include <app.h>
#include <banana/geometry.h>
#include <banana/mesh.h>
#include <banana/orbit_camera.h>
#include <gorilla/window.h>
#include <memory>
#include <renderer.h>
#include <teapot.h>
#include <tiny-gizmo.hpp>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "TinyGizmo";
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
  submesh.material->shader_name = "lighting/vertex_ads.hlsl";
  submesh.material->properties["Kd"] = banana::Float3(0.4f, 0.8f, 0.6f);
  submesh.material->properties["Ka"] = banana::Float3(0.1f, 0.1f, 0.1f);
  submesh.material->properties["Ks"] = banana::Float3(1.0f, 1.0f, 1.0f);
  submesh.material->properties["Shininess"] = 10.0f;

  // world
  banana::LightInfo lights[5] = {0};
  lights[0].intensity = banana::Float3{1, 1, 1};
  lights[0].position = banana::Float3{0.5, -1, -1};
  lights[0].is_point = 0;
  lights[1].intensity = banana::Float3{0, 0, 0};
  lights[2].intensity = banana::Float3{0, 0, 0};
  lights[3].intensity = banana::Float3{0, 0, 0};
  lights[4].intensity = banana::Float3{0, 0, 0};

  // gizmo
  tinygizmo::gizmo_context gizmo_context;
  tinygizmo::gizmo_application_state gizmo_state;
  auto gizmo_node = std::make_shared<banana::Node>();
  gizmo_node->mesh = std::make_shared<banana::Mesh>();

  // main loop
  auto context = app.context();
  banana::OrbitCamera camera;
  Renderer renderer;
  gorilla::ScreenState state;
  while (app.begin_frame(&state)) {
    update_camera(&camera, state);

    {
      // update gizmo
      gizmo_state.mouse_left = state.mouse_button_flag &
                               gorilla::MouseButtonFlags::MouseButtonLeftDown;

      // Gizmo input interaction gizmo_state populated via win->on_input(...)
      // callback above. Update app parameters:
      gizmo_state.viewport_size = {state.width, state.height};
      gizmo_state.cam.near_clip = camera._near;
      gizmo_state.cam.far_clip = camera._far;
      gizmo_state.cam.yfov = camera.fovYRad;
      auto p = camera.position();
      gizmo_state.cam.position = {p.x, p.y, p.z};

      auto r = camera.rotation();
      gizmo_state.cam.orientation = {r.x, r.y, r.z, r.w};

      auto dir = camera.get_ray_direction();
      gizmo_state.ray_origin = {p.x, p.y, p.z};
      gizmo_state.ray_direction = {dir.x, dir.y, dir.z};
      // optional flag to draw the gizmos at a constant screen-space scale
      // gizmo.screenspace_scale = 40.f;

      gizmo_context.update(gizmo_state);
    }

    tinygizmo::position("teaport", gizmo_context, &node->transform.rotation.x,
                        &node->transform.translation.x);

    renderer.render(device, context, node, &camera, lights);
    app.clear_depth();
    {
      auto [vertices, indices] = gizmo_context.draw();
      gizmo_node->mesh->assign(vertices, indices);
      renderer.render(device, context, gizmo_node, &camera, lights);
    }

    app.end_frame();
  }

  return 0;
}