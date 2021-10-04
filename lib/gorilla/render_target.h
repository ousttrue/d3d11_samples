#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace gorilla {

class RenderTarget {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  ComPtr<ID3D11RenderTargetView> _rtv;
  D3D11_TEXTURE2D_DESC _desc;

  ComPtr<ID3D11Texture2D> _depthStencil;
  ComPtr<ID3D11DepthStencilView> _dsv;
  ComPtr<ID3D11DepthStencilState> _dss;

public:
  const ComPtr<ID3D11RenderTargetView> &get() const { return _rtv; }
  void release() { _rtv.Reset(); }
  void clear(const ComPtr<ID3D11DeviceContext> &context, const float clear[4]);
  void clear_depth(const ComPtr<ID3D11DeviceContext> &context,
                   float depth = 1.0f);
  bool create_rtv(const ComPtr<ID3D11Device> &device,
                  const ComPtr<ID3D11Texture2D> &texture);
  bool create_dsv(const ComPtr<ID3D11Device> &device);
  void setup(const ComPtr<ID3D11DeviceContext> &context, float w, float h);
  void setup(const ComPtr<ID3D11DeviceContext> &context, int w, int h) {
    setup(context, static_cast<float>(w), static_cast<float>(h));
  }
};

} // namespace gorilla
