#include <DirectXMath.h>
#include <assert.h>
#include <banana/asset.h>
#include <banana/grid_constant.h>
#include <banana/orbit_camera.h>
#include <banana/types.h>
#include <gorilla/drawable.h>
#include <gorilla/renderer.h>
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

  gorilla::Renderer renderer;
  auto [device, context] = renderer.create(hwnd);
  if (!device) {
    return 2;
  }

  // setup pipeline
  auto shader = banana::get_string("grid.hlsl");
  if (shader.empty()) {
    return 3;
  }
  gorilla::Drawable drawable;
  if (!drawable.state.create(device, true)) {
    return 4;
  }
  auto [ok, error] = drawable.pipeline.compile_shader(device, shader, "vsMain",
                                                      "gsMain", "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    return 5;
  }
  banana::GridConstant constant;

  // main loop
  banana::OrbitCamera camera;
  gorilla::ScreenState state;
  for (UINT frame_count = 0; window.process_messages(&state); ++frame_count) {

    // update
    update_camera(&camera, state);
    constant.update(camera);
    drawable.pipeline.gs_stage.cb[0].update(context, constant);
    drawable.pipeline.ps_stage.cb[0].update(context, constant);

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    renderer.begin_frame(state, clear);
    drawable.draw(context);
    renderer.end_frame();
  }

  return 0;
}
