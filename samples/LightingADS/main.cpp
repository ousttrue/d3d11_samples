#include <app.h>
#include <banana/asset.h>
#include <banana/geometry.h>
#include <gorilla/input_assembler.h>
#include <gorilla/pipeline.h>
#include <iostream>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "LightingADS";
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
  gorilla::Pipeline pipeline;
  {
    auto shader = banana::get_string("lighting/ads.hlsl");
    if (shader.empty()) {
      return 2;
    }
    auto [ok, error] =
        pipeline.compile_shader(device, shader, "vsMain", {}, "psMain");
    if (!ok) {
      std::cerr << error << std::endl;
      return 3;
    }
  }

  gorilla::InputAssembler ia;
  {
    auto cube = banana::geometry::create_cube(0.4f);
    if (!ia.create_vertices(device, cube->vertices)) {
      return 4;
    }
    if (!ia.create_indices(device, cube->indices)) {
      return 5;
    }
  }

  // main loop
  while (
      app.new_frame([&pipeline, &ia](const ComPtr<ID3D11DeviceContext> &context,
                                     const banana::OrbitCamera &camera) {
        // update
        pipeline.vs_stage.cb[0].update(context, camera.matrix());
        // draw
        pipeline.setup(context);
        ia.draw(context);
      }))
    ;

  return 0;
}
