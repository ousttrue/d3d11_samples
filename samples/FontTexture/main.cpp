#include <DirectXMath.h>
#include <assert.h>
#include <banana/asset.h>
#include <banana/image.h>
#include <banana/orbit_camera.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/device.h>
#include <gorilla/input_assembler.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>
#include <gorilla/swapchain.h>
#include <gorilla/texture.h>
#include <gorilla/window.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string_view>
#include <update_camera.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "FontTexture";
auto WIDTH = 320;
auto HEIGHT = 320;

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct Glyph {
  int x;
  int y;
  int width;
  int height;
  int originX;
  int originY;
  int advance;
};

struct FontManger {
  gorilla::Pipeline pipeline;
  UINT sampler_slot = 0;
  UINT srv_slot = 0;
  gorilla::Texture _texture;
  std::unordered_map<char, Glyph> _char_map;
  float _image_width = 0;
  float _image_height = 0;

public:
  bool load(const ComPtr<ID3D11Device> &device, const std::string &key) {
    auto shader = banana::get_string(key + ".hlsl");
    if (shader.empty()) {
      return false;
    }

    // setup pipeline
    auto [ok, error] =
        pipeline.compile_shader(device, shader, "vsMain", {}, "psMain");
    if (!ok) {
      std::cerr << error << std::endl;
      return false;
    }
    assert(pipeline.ps_stage.reflection.sampler_slots.size() == 1);
    assert(pipeline.ps_stage.reflection.srv_slots.size() == 1);

    auto image_bytes = banana::get_bytes(key + ".png");
    banana::Image image;
    if (!image.load(image_bytes)) {
      return false;
    }
    _image_width = static_cast<float>(image.width());
    _image_height = static_cast<float>(image.height());

    if (!_texture.create(device, image.data(), image.width(), image.height())) {
      return false;
    }

    auto json = nlohmann::json::parse(banana::get_string("font/Arial32.json"));
    for (auto [k, v] : json["characters"].items()) {
      auto key = k.c_str()[0];
      auto added = _char_map.insert(std::make_pair(key, Glyph{}));
      assert(added.second);
      auto &glyph = added.first->second;

      glyph.x = v["x"];
      glyph.y = v["y"];
      glyph.width = v["width"];
      glyph.height = v["height"];
      glyph.originX = v["originX"];
      glyph.originY = v["originY"];
      glyph.advance = v["advance"];
    }

    return true;
  }

  std::shared_ptr<gorilla::InputAssembler>
  create_text(const ComPtr<ID3D11Device> &device, std::string_view text) {
    auto ia = std::make_shared<gorilla::InputAssembler>();

    struct float2 {
      float x;
      float y;
    };
    struct Vertex {
      float2 position;
      float2 uv;
    };
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    auto push_glyph = [&vertices, &indices, W = _image_width,
                       H = _image_height](const Glyph &g, float left) {
      // pixel to screen
      float FACTOR = 0.01f;
      ;
      left *= FACTOR;
      float right = left + g.width * FACTOR;
      float top = static_cast<float>(g.height) * FACTOR;
      float bottom = 0;

      // pixel to uv
      float uv_left = g.x / W;
      float uv_right = uv_left + g.width / W;
      float uv_top = g.y / H;
      float uv_bottom = uv_top + g.height / H;

      auto i = static_cast<uint16_t>(vertices.size());

      // vertex
      vertices.push_back(Vertex{{left, top}, {uv_left, uv_top}});
      vertices.push_back(Vertex{{left, bottom}, {uv_left, uv_bottom}});
      vertices.push_back(Vertex{{right, bottom}, {uv_right, uv_bottom}});
      vertices.push_back(Vertex{{right, top}, {uv_right, uv_top}});

      // index
      indices.push_back(i);
      indices.push_back(i + 1);
      indices.push_back(i + 2);

      indices.push_back(i + 2);
      indices.push_back(i + 3);
      indices.push_back(i);

      return g.width;
    };

    int x = 0;
    for (auto c : text) {
      auto &glyph = _char_map[c];
      x += push_glyph(glyph, static_cast<float>(x));
    }

    if (!ia->create_vertices(device,
                             std::span{vertices.begin(), vertices.end()})) {
      return {};
    }
    if (!ia->create_indices(device, indices)) {
      return {};
    }

    return ia;
  }

  void setup(const ComPtr<ID3D11DeviceContext> &context) {
    pipeline.setup(context);
    _texture.set_ps(context, srv_slot, sampler_slot);
  }
};

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

  auto device = gorilla::create_device();
  if (!device) {
    return 2;
  }
  ComPtr<ID3D11DeviceContext> context;
  device->GetImmediateContext(&context);

  auto swapchain = gorilla::create_swapchain(device, hwnd);
  if (!swapchain) {
    return 3;
  }

  FontManger font;
  if (!font.load(device, "font/Arial32")) {
    return 4;
  }
  auto ia = font.create_text(device, "ABCDXYZW");
  if (!ia) {
    return 5;
  }

  ComPtr<ID3D11RasterizerState> rs;
  D3D11_RASTERIZER_DESC rs_desc = {};
  rs_desc.CullMode = D3D11_CULL_NONE;
  rs_desc.FillMode = D3D11_FILL_SOLID;
  rs_desc.FrontCounterClockwise = true;
  rs_desc.ScissorEnable = false;
  rs_desc.MultisampleEnable = false;
  if (FAILED(device->CreateRasterizerState(&rs_desc, &rs))) {
    return 15;
  }

  // main loop
  banana::OrbitCamera camera;
  DXGI_SWAP_CHAIN_DESC desc;
  swapchain->GetDesc(&desc);
  gorilla::RenderTarget render_target;
  gorilla::ScreenState state;
  for (UINT frame_count = 0; window.process_messages(&state); ++frame_count) {
    if (state.width != desc.BufferDesc.Width ||
        state.height != desc.BufferDesc.Height) {
      // clear backbuffer reference
      render_target.release();
      // resize swapchain
      swapchain->ResizeBuffers(desc.BufferCount, static_cast<UINT>(state.width),
                               static_cast<UINT>(state.height),
                               desc.BufferDesc.Format, desc.Flags);
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
      if (!render_target.create_dsv(device)) {
        assert(false);
      }
    }

    // update
    update_camera(&camera, state);

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, state.width, state.height);

    context->RSSetState(rs.Get());
    font.pipeline.vs_stage.cb[0].update(context,
                                        camera.view * camera.projection);
    font.setup(context);
    ia->draw(context);

    // vsync
    context->Flush();
    swapchain->Present(1, 0);
  }

  return 0;
}
