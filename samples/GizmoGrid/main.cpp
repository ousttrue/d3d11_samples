#include <DirectXMath.h>
#include <assert.h>
#include <banana/asset.h>
#include <banana/orbit_camera.h>
#include <banana/types.h>
#include <gorilla/device_and_target.h>
#include <gorilla/drawable.h>
#include <gorilla/gizmo.h>
#include <gorilla/window.h>
#include <iostream>
#include <update_camera.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "Grid";
auto WIDTH = 320;
auto HEIGHT = 320;

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);

  gorilla::Window window;
  auto hwnd = window.create(hInstance, CLASS_NAME, WINDOW_TITLE, WIDTH, HEIGHT);
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

  // setup pipeline
  auto grid = gorilla::gizmo::create_grid(device);
  if(!grid)
  {
    return 3;
  }

  // main loop
  banana::OrbitCamera camera;
  gorilla::ScreenState state;
  for (UINT frame_count = 0; window.process_messages(&state); ++frame_count) {

    // update
    update_camera(&camera, state);

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    renderer.begin_frame(state, clear);
    grid->draw(context, camera);
    renderer.end_frame();
  }

  return 0;
}
