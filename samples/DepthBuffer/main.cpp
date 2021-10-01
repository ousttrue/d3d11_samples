#include <DirectXMath.h>
#include <assert.h>
#include <banana/asset.h>
#include <banana/geometry.h>
#include <banana/orbit_camera.h>
#include <banana/types.h>
#include <gorilla/drawable.h>
#include <gorilla/device_and_target.h>
#include <gorilla/window.h>
#include <iostream>
#include <update_camera.h>

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
    return 1;
  }
  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  gorilla::DeviceAndTarget renderer;
  auto [device, context] = renderer.create(hwnd);
  if (!device) {
    return 2;
  }

  auto shader = banana::get_string("depth.hlsl");
  if (shader.empty()) {
    return 3;
  }
  gorilla::Drawable drawable;
  if (!drawable.state.create(device)) {
    return 4;
  }
  auto [ok, error] =
      drawable.pipeline.compile_shader(device, shader, "vsMain", {}, "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    return 5;
  }
  auto cube = banana::geometry::create_cube(0.4f);
  if (!drawable.ia.create_vertices(device, cube->vertex_stride,
                                   cube->vertices.data(),
                                   cube->vertices.size())) {
    return 6;
  }
  if (!drawable.ia.create_indices(device, cube->index_stride,
                                  cube->indices.data(), cube->indices.size())) {
    return 7;
  }

  // main loop
  banana::OrbitCamera camera;
  gorilla::ScreenState state;
  for (UINT frame_count = 0; window.process_messages(&state); ++frame_count) {

    // update
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    update_camera(&camera, state);
    drawable.pipeline.vs_stage.cb[0].update(context,
                                            camera.view * camera.projection);

    // draw
    renderer.begin_frame(state, clear);
    drawable.draw(context);
    renderer.end_frame();
  }

  return 0;
}
