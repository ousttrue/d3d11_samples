#include <update_camera.h>
#include <DirectXMath.h>
#include <assert.h>
#include <banana/asset.h>
#include <banana/orbit_camera.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/device.h>
#include <gorilla/input_assembler.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>
#include <gorilla/swapchain.h>
#include <gorilla/window.h>
#include <iostream>
#include <stb_easy_font.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "FontMesh";
auto WIDTH = 320;
auto HEIGHT = 320;

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct Vertex {
  float x, y, z, w;
};

std::shared_ptr<gorilla::InputAssembler>
text_mesh(const ComPtr<ID3D11Device> &device, const char *text, float z) {
  int width = stb_easy_font_width((char *)text);
  int height = stb_easy_font_height((char *)text);

  std::vector<Vertex> font_data_(9999);
  int num_quads = stb_easy_font_print(
      static_cast<float>(-width / 2), static_cast<float>(-height / 2),
      (char *)text, nullptr, font_data_.data(),
      static_cast<int>(sizeof(font_data_[0]) * font_data_.size()));

  std::vector<Vertex> vert_data_;
  vert_data_.resize(num_quads * 4);
  for (size_t i = 0; i < vert_data_.size(); ++i) {
    vert_data_[i] = font_data_[i];
    vert_data_[i].y *= -1;
    vert_data_[i].z = z;
  }

  std::vector<uint32_t> index_data_;
  auto num_indices = 6 * num_quads;
  index_data_.resize(num_indices);
  for (uint32_t i = 0; 6 * i < index_data_.size(); ++i) {
    index_data_[6 * i + 0] = 4 * i + 0;
    index_data_[6 * i + 1] = 4 * i + 2;
    index_data_[6 * i + 2] = 4 * i + 1;
    index_data_[6 * i + 3] = 4 * i + 2;
    index_data_[6 * i + 4] = 4 * i + 0;
    index_data_[6 * i + 5] = 4 * i + 3;
  }

  auto ia = std::make_shared<gorilla::InputAssembler>();
  if (!ia->create_vertices(device, vert_data_)) {
    return {};
  }
  if (!ia->create_indices(device, index_data_)) {
    return {};
  }

  return ia;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  auto shader = banana::get_string("depth.hlsl");
  if (shader.empty()) {
    return 1;
  }

  gorilla::Window window;
  auto hwnd = window.create(hInstance, CLASS_NAME, WINDOW_TITLE, WIDTH, HEIGHT);
  if (!hwnd) {
    return 2;
  }

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  auto device = gorilla::create_device();
  if (!device) {
    return 3;
  }
  ComPtr<ID3D11DeviceContext> context;
  device->GetImmediateContext(&context);

  auto swapchain = gorilla::create_swapchain(device, hwnd);
  if (!swapchain) {
    return 4;
  }

  // setup pipeline
  gorilla::Pipeline pipeline;
  auto [ok, error] =
      pipeline.compile_shader(device, shader, "vsMain", {}, "psMain");
  if (!ok) {
    std::cerr << error << std::endl;
    return 5;
  }

  auto ia = text_mesh(device, "FontMesh", 5);

  banana::OrbitCamera camera;

  // main loop
  DXGI_SWAP_CHAIN_DESC desc;
  swapchain->GetDesc(&desc);
  gorilla::RenderTarget render_target;
  gorilla::ScreenState state;
  for (UINT frame_count = 0; window.process_messages(&state); ++frame_count) {

    if (state.width != desc.BufferDesc.Width || state.height != desc.BufferDesc.Height) {
      // clear backbuffer reference
      render_target.release();
      // resize swapchain
      swapchain->ResizeBuffers(desc.BufferCount, state.width, state.height, desc.BufferDesc.Format,
                               desc.Flags);
    }

    // ensure create backbuffer
    if (!render_target.get()) {
      ComPtr<ID3D11Texture2D> backbuffer;
      auto hr = swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
      if (FAILED(hr)) {
        assert(false);
      }

      if (!render_target.create_rtv(device, backbuffer)) {
        assert(false);
      }
    }

    // update
    update_camera(&camera, state);
    pipeline.vs_stage.cb[0].update(context, camera.view * camera.projection);

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, state.width, state.height);

    pipeline.setup(context);
    ia->draw(context);

    // vsync
    context->Flush();
    swapchain->Present(1, 0);
  }

  return 0;
}
