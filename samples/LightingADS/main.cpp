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

  struct LightInfo {
    DirectX::XMFLOAT3 Position;
    float IsPoint;
    DirectX::XMFLOAT3 Intensity;
    float _padding1;
  };
  static_assert(sizeof(LightInfo) == 32);

  struct World {
    DirectX::XMFLOAT4X4 ModelViewMatrix;
    DirectX::XMFLOAT3X4 NormalMatrix;
    DirectX::XMFLOAT4X4 MVP;
    LightInfo Lights[5];
  };
  // 64 + 64 + 36+12 + 160 = 312
  static_assert(sizeof(World) == 336);
  // static_assert(sizeof(Material) == 336);

  struct Material {
    DirectX::XMFLOAT3 Diffuse;
    float _padding0;
    DirectX::XMFLOAT3 Ambient;
    float _padding1;
    DirectX::XMFLOAT3 Specular;
    float Shininess;
  };
  static_assert(sizeof(Material) == 48);

  World world;
  world.Lights[0].Intensity = DirectX::XMFLOAT3(1, 1, 1);
  world.Lights[0].Position = DirectX::XMFLOAT3(0.5, -1, -1);
  world.Lights[0].IsPoint = 0;
  world.Lights[1].Intensity = DirectX::XMFLOAT3(0, 0, 0);
  world.Lights[2].Intensity = DirectX::XMFLOAT3(0, 0, 0);
  world.Lights[3].Intensity = DirectX::XMFLOAT3(0, 0, 0);
  world.Lights[4].Intensity = DirectX::XMFLOAT3(0, 0, 0);
  Material material;
  material.Diffuse = DirectX::XMFLOAT3(0.4f, 0.8f, 0.6f);
  material.Ambient = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
  material.Specular = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
  material.Shininess = 1.0f;

  // main loop
  auto context = app.context();
  auto camera = app.camera();
  while (app.begin_frame()) {
    // update
    world.MVP = camera->view_projection_matrix();
    world.NormalMatrix = camera->normal_matrix();
    world.ModelViewMatrix = camera->_view;
    pipeline.vs_stage.cb[0].update(context, world);
    pipeline.vs_stage.cb[1].update(context, material);
    // draw
    pipeline.setup(context);
    ia.draw(context);

    app.end_frame();
  }

  return 0;
}
