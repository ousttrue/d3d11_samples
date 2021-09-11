#include "swapchain.h"
#include <DirectXMath.h>
#include <assert.h>
#include <device.h>
#include <iostream>
#include <shader.h>
#include <window.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class Pipeline {
  ComPtr<ID3D11VertexShader> _vs;
  ComPtr<ID3D11GeometryShader> _gs;
  ComPtr<ID3D11PixelShader> _ps;

public:
  bool compile_vs(const ComPtr<ID3D11Device> &device, const char *name,
                  std::string_view source, const char *entry_point) {
    auto [compiled, error] = swtk::compile_vs(name, source, entry_point);
    if (!compiled) {
      if (error) {
        std::cerr << (const char *)error->GetBufferPointer() << std::endl;
      }
      return false;
    }
    auto hr =
        device->CreateVertexShader((DWORD *)compiled->GetBufferPointer(),
                                   compiled->GetBufferSize(), nullptr, &_vs);
    if (FAILED(hr)) {
      return false;
    }
    return true;
  }
  bool compile_gs(const ComPtr<ID3D11Device> &device, const char *name,
                  std::string_view source, const char *entry_point) {
    auto [compiled, error] = swtk::compile_gs(name, source, entry_point);
    if (!compiled) {
      if (error) {
        std::cerr << (const char *)error->GetBufferPointer() << std::endl;
      }
      return false;
    }
    auto hr =
        device->CreateGeometryShader((DWORD *)compiled->GetBufferPointer(),
                                     compiled->GetBufferSize(), nullptr, &_gs);
    if (FAILED(hr)) {
      return false;
    }
    return true;
  }
  bool compile_ps(const ComPtr<ID3D11Device> &device, const char *name,
                  std::string_view source, const char *entry_point) {
    auto [compiled, error] = swtk::compile_ps(name, source, entry_point);
    if (!compiled) {
      if (error) {
        std::cerr << (const char *)error->GetBufferPointer() << std::endl;
      }
      return false;
    }
    auto hr =
        device->CreatePixelShader((DWORD *)compiled->GetBufferPointer(),
                                  compiled->GetBufferSize(), nullptr, &_ps);
    if (FAILED(hr)) {
      return false;
    }
    return true;
  }

  void draw(const ComPtr<ID3D11DeviceContext> &context) {
    context->VSSetShader(_vs.Get(), nullptr, 0);
    if (_gs) {
      context->GSSetShader(_gs.Get(), nullptr, 0);
    } else {
      context->GSSetShader(0, nullptr, 0);
    }
    context->PSSetShader(_ps.Get(), nullptr, 0);

    context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    context->Draw(1, 0);
  }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);

  std::string shader = swtk::read_file(lpCmdLine);
  if (shader.empty()) {
    return 7;
  }

  swtk::Window window;
  auto hwnd = window.create(hInstance, "CLASS_NAME", "BasicPipeline", 320, 320);
  if (!hwnd) {
    return 1;
  }

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  auto device = swtk::create_device();
  if (!device) {
    return 2;
  }
  ComPtr<ID3D11DeviceContext> context;
  device->GetImmediateContext(&context);

  auto swapchain = swtk::create_swapchain(device, hwnd);
  if (!swapchain) {
    return 3;
  }

  // setup pipeline
  Pipeline pipeline;
  if (!pipeline.compile_vs(device, "vs", shader, "vsMain")) {
    return 4;
  }
  if (!pipeline.compile_gs(device, "gs", shader, "gsMain")) {
    return 5;
  }
  if (!pipeline.compile_ps(device, "ps", shader, "psMain")) {
    return 6;
  }

  // main loop
  DXGI_SWAP_CHAIN_DESC desc;
  swapchain->GetDesc(&desc);
  ComPtr<ID3D11RenderTargetView> rtv;
  for (UINT frame_count = 0; window.process_messages(); ++frame_count) {

    RECT rect;
    GetClientRect(hwnd, &rect);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    if (w != desc.BufferDesc.Width || h != desc.BufferDesc.Height) {
      // clear backbuffer reference
      rtv.Reset();
      // resize swapchain
      swapchain->ResizeBuffers(desc.BufferCount, w, h, desc.BufferDesc.Format,
                               desc.Flags);
    }

    // ensure create backbuffer
    if (!rtv) {
      ComPtr<ID3D11Texture2D> backbuffer;
      auto hr = swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
      if (FAILED(hr)) {
        assert(false);
      }

      hr = device->CreateRenderTargetView(backbuffer.Get(), nullptr, &rtv);
      if (FAILED(hr)) {
        assert(false);
      }
    }

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    context->ClearRenderTargetView(rtv.Get(), clear);

    // set backbuffer & depthbuffer
    ID3D11RenderTargetView *rtv_list[] = {rtv.Get()};
    context->OMSetRenderTargets(1, rtv_list, nullptr);
    D3D11_VIEWPORT viewports[1] = {{0}};
    viewports[0].TopLeftX = 0;
    viewports[0].TopLeftY = 0;
    viewports[0].Width = (float)w;
    viewports[0].Height = (float)h;
    viewports[0].MinDepth = 0;
    viewports[0].MaxDepth = 1.0f;
    context->RSSetViewports(_countof(viewports), viewports);

    pipeline.draw(context);

    // vsync
    swapchain->Present(1, 0);
  }

  return 0;
}
