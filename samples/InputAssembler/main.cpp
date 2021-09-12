#include <DirectXMath.h>
#include <assert.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/device.h>
#include <gorilla/orbit_camera.h>
#include <gorilla/pipeline.h>
#include <gorilla/render_target.h>
#include <gorilla/shader.h>
#include <gorilla/shader_reflection.h>
#include <gorilla/swapchain.h>
#include <gorilla/window.h>
#include <iostream>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

// auto textureFile = L"../MinTriangle/texture.png";

namespace gorilla {
class InputAssembler {
  ComPtr<ID3D11InputLayout> _input_layout;
  ComPtr<ID3D11Buffer> _pVertexBuf;
  UINT _strides[1] = {0};

  ComPtr<ID3D11Buffer> _pIndexBuf;
  UINT _index_count = 0;
  DXGI_FORMAT _index_format = DXGI_FORMAT_R32_UINT;

public:
  bool create(const ComPtr<ID3D11Device> &pDevice,
              const ComPtr<ID3D11InputLayout> &input_layout,
              const void *vertices, UINT vertices_size, UINT stride,
              const void *indices, UINT indices_size, UINT index_count) {
    {
      D3D11_BUFFER_DESC vdesc = {0};
      vdesc.ByteWidth = vertices_size;
      vdesc.Usage = D3D11_USAGE_DEFAULT;
      vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      vdesc.CPUAccessFlags = 0;
      D3D11_SUBRESOURCE_DATA vertexData = {0};
      vertexData.pSysMem = vertices;
      HRESULT hr = pDevice->CreateBuffer(&vdesc, &vertexData, &_pVertexBuf);
      if (FAILED(hr)) {
        return false;
      }
      _input_layout = input_layout;
      _strides[0] = stride;
    }
    {
      D3D11_BUFFER_DESC idesc = {0};
      idesc.ByteWidth = indices_size;
      idesc.Usage = D3D11_USAGE_DEFAULT;
      idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
      idesc.CPUAccessFlags = 0;
      D3D11_SUBRESOURCE_DATA indexData = {0};
      indexData.pSysMem = indices;
      HRESULT hr = pDevice->CreateBuffer(&idesc, &indexData, &_pIndexBuf);
      if (FAILED(hr)) {
        return false;
      }
      _index_count = index_count;
    }
    return true;
  }

  void draw(const ComPtr<ID3D11DeviceContext> &pDeviceContext) {
    pDeviceContext->IASetInputLayout(_input_layout.Get());

    // set vertexbuffer
    ID3D11Buffer *pBufferTbl[] = {_pVertexBuf.Get()};
    UINT OffsetTbl[] = {0};
    pDeviceContext->IASetVertexBuffers(0, 1, pBufferTbl, _strides, OffsetTbl);
    // set indexbuffer
    pDeviceContext->IASetIndexBuffer(_pIndexBuf.Get(), _index_format, 0);
    pDeviceContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pDeviceContext->DrawIndexed(_index_count, 0, 0);
  }
};

} // namespace gorilla

struct Vertex {
  DirectX::XMFLOAT4 pos;
  DirectX::XMFLOAT4 color;
  DirectX::XMFLOAT2 tex;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  std::string shader = gorilla::read_file(lpCmdLine);
  if (shader.empty()) {
    return 7;
  }

  gorilla::Window window;
  auto hwnd =
      window.create(hInstance, "WINDOW_CLASS", "InputAssembler", 320, 320);
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
  auto [compiled, vserror] =
      pipeline.compile_vs(device, "vs", shader, "vsMain");
  if (!compiled) {
    if (vserror) {
      std::cerr << (const char *)vserror->GetBufferPointer() << std::endl;
    }
    return 4;
  }
  auto input_layout = gorilla::create_input_layout(device, compiled);
  if (!input_layout) {
    return 9;
  }
  auto [ps, pserror] = pipeline.compile_ps(device, "ps", shader, "psMain");
  if (!ps) {
    if (pserror) {
      std::cerr << (const char *)pserror->GetBufferPointer() << std::endl;
    }
    return 6;
  }

  gorilla::InputAssembler ia;
  auto size = 0.4f;
  Vertex pVertices[] = {
      // x
      {DirectX::XMFLOAT4(-size, -size, -size, 1.0f),
       DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
      {DirectX::XMFLOAT4(-size, -size, size, 1.0f),
       DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
      {DirectX::XMFLOAT4(-size, size, size, 1.0f),
       DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
      {DirectX::XMFLOAT4(-size, size, -size, 1.0f),
       DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},

      {DirectX::XMFLOAT4(size, -size, -size, 1.0f),
       DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
      {DirectX::XMFLOAT4(size, size, -size, 1.0f),
       DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},
      {DirectX::XMFLOAT4(size, size, size, 1.0f),
       DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
      {DirectX::XMFLOAT4(size, -size, size, 1.0f),
       DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
      // y
      {DirectX::XMFLOAT4(-size, size, -size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
      {DirectX::XMFLOAT4(-size, size, size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
      {DirectX::XMFLOAT4(size, size, size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
      {DirectX::XMFLOAT4(size, size, -size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},

      {DirectX::XMFLOAT4(-size, -size, -size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
      {DirectX::XMFLOAT4(size, -size, -size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},
      {DirectX::XMFLOAT4(size, -size, size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
      {DirectX::XMFLOAT4(-size, -size, size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
      // z
      {DirectX::XMFLOAT4(-size, -size, -size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(0, 1)},
      {DirectX::XMFLOAT4(-size, size, -size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(0, 0)},
      {DirectX::XMFLOAT4(size, size, -size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(1, 0)},
      {DirectX::XMFLOAT4(size, -size, -size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f), DirectX::XMFLOAT2(1, 1)},

      {DirectX::XMFLOAT4(-size, -size, size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0, 1)},
      {DirectX::XMFLOAT4(size, -size, size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1, 1)},
      {DirectX::XMFLOAT4(size, size, size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1, 0)},
      {DirectX::XMFLOAT4(-size, size, size, 1.0f),
       DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0, 0)},
  };
  unsigned int vsize = sizeof(pVertices);
  unsigned int pIndices[] = {
      0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
      12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
  };
  unsigned int isize = sizeof(pIndices);
  unsigned int index_count = isize / sizeof(pIndices[0]);
  if (!ia.create(device, input_layout, pVertices, vsize, sizeof(Vertex),
                 pIndices, isize, index_count)) {
    return 8;
  }

  gorilla::ConstantBuffer cb;
  if (!cb.create(device, sizeof(DirectX::XMFLOAT4X4))) {
    return 7;
  }
  UINT cb_slot = 0;
  gorilla::OrbitCamera camera;
  gorilla::MouseBinder binder(camera);
  window.bind_mouse(
      std::bind(&gorilla::MouseBinder::Left, &binder, std::placeholders::_1),
      std::bind(&gorilla::MouseBinder::Middle, &binder, std::placeholders::_1),
      std::bind(&gorilla::MouseBinder::Right, &binder, std::placeholders::_1),
      std::bind(&gorilla::MouseBinder::Move, &binder, std::placeholders::_1,
                std::placeholders::_2),
      std::bind(&gorilla::MouseBinder::Wheel, &binder, std::placeholders::_1));

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

      if (!render_target.create(device, backbuffer, true)) {
        assert(false);
      }
    }

    // update
    camera.resize(static_cast<float>(w), static_cast<float>(h));
    cb.update(context, camera.matrix());

    // clear RTV
    auto v =
        (static_cast<float>(sin(frame_count / 180.0f * DirectX::XM_PI)) + 1) *
        0.5f;
    float clear[] = {0.5, v, 0.5, 1.0f};
    render_target.clear(context, clear);
    render_target.setup(context, w, h);

    pipeline.setup(context);
    cb.set_vs(context, cb_slot);
    ia.draw(context);

    // vsync
    context->Flush();
    swapchain->Present(1, 0);
  }

  return 0;
}
