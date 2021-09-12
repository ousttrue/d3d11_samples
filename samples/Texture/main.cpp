#include "pipeline.h"
#include "swapchain.h"
#include <DirectXMath.h>
#include <assert.h>
#include <device.h>
#include <iostream>
#include <render_target.h>
#include <shader.h>
#include <shader_reflection.h>
#include <window.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla {

class Texture {
  Microsoft::WRL::ComPtr<ID3D11Texture2D> _texture;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srv;
  Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler;

public:
  bool create(const ComPtr<ID3D11Device> &device, const void *p, UINT w,
              UINT h) {
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = w;
    desc.Height = h;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = p;
    initData.SysMemPitch = w * 4;
    initData.SysMemSlicePitch = w * h * 4;

    auto hr = device->CreateTexture2D(&desc, &initData, &_texture);
    if (FAILED(hr)) {
      return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(_texture.Get(), &SRVDesc, &_srv);
    if (FAILED(hr)) {
      return false;
    }

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    hr = device->CreateSamplerState(&samplerDesc, &_sampler);
    if (FAILED(hr)) {
      return false;
    }

    return true;
  }

  void set_ps(const ComPtr<ID3D11DeviceContext> &context, UINT srv,
              UINT sampler) {
    context->PSSetShaderResources(srv, 1, _srv.GetAddressOf());
    context->PSSetSamplers(sampler, 1, _sampler.GetAddressOf());
  }
};

} // namespace gorilla

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);

  std::string shader = gorilla::read_file(lpCmdLine);
  if (shader.empty()) {
    return 7;
  }

  gorilla::Window window;
  auto hwnd = window.create(hInstance, "CLASS_NAME", "Texture", 320, 320);
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

  // setup pipeline
  gorilla::Pipeline pipeline;
  auto [vs, vserror] = pipeline.compile_vs(device, "vs", shader, "vsMain");
  if (!vs) {
    if (vserror) {
      std::cerr << (const char *)vserror->GetBufferPointer() << std::endl;
    }
    return 4;
  }
  auto [gs, gserror] = pipeline.compile_gs(device, "gs", shader, "gsMain");
  if (!gs) {
    if (gserror) {
      std::cerr << (const char *)gserror->GetBufferPointer() << std::endl;
    }
    return 5;
  }
  auto [ps, pserror] = pipeline.compile_ps(device, "ps", shader, "psMain");
  if (!ps) {
    if (pserror) {
      std::cerr << (const char *)pserror->GetBufferPointer() << std::endl;
    }
    return 6;
  }
  gorilla::ShaderVariables ps_slots;
  if (!ps_slots.reflect(ps)) {
    return 7;
  }
  assert(ps_slots.sampler_slots.size() == 1);
  assert(ps_slots.srv_slots.size() == 1);
  UINT sampler_slot = 0;
  UINT srv_slot = 0;

  // image
  struct RGBA {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  };
  RGBA image[256 * 256] = {0};
  static_assert(sizeof(image) == 256 * 256 * 4);
  {
    auto row = image;
    for (int y = 0; y < 256; ++y, row += 256) {
      for (int x = 0; x < 256; ++x) {
        auto &rgba = row[x];
        rgba.r = x;
        rgba.g = y;
        rgba.b = 0;
        rgba.a = 255;
      }
    }
  }
  gorilla::Texture texture;
  if (!texture.create(device, image, 256, 256)) {
    return 10;
  }

  // main loop
  DXGI_SWAP_CHAIN_DESC desc;
  swapchain->GetDesc(&desc);
  gorilla::RenderTarget render_target;
  for (UINT frame_count = 0; window.process_messages(); ++frame_count) {

    RECT rect;
    GetClientRect(hwnd, &rect);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    if (w != desc.BufferDesc.Width || h != desc.BufferDesc.Height) {
      // clear backbuffer reference
      render_target.release();
      // resize swapchain
      swapchain->ResizeBuffers(desc.BufferCount, w, h, desc.BufferDesc.Format,
                               desc.Flags);
    }

    // ensure create backbuffer
    if (!render_target.get()) {
      ComPtr<ID3D11Texture2D> backbuffer;
      auto hr = swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
      if (FAILED(hr)) {
        assert(false);
      }

      if (!render_target.create(device, backbuffer, false)) {
        assert(false);
      }
    }

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, w, h);

    // draw
    pipeline.setup(context);
    texture.set_ps(context, srv_slot, sampler_slot);
    pipeline.draw_empty(context);

    // vsync
    swapchain->Present(1, 0);
  }

  return 0;
}
