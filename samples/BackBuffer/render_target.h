#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace swtk {

class RenderTarget {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  ComPtr<ID3D11RenderTargetView> _rtv;
  ComPtr<ID3D11Texture2D> _depthStencil;
  ComPtr<ID3D11DepthStencilView> _dsv;

public:
  const ComPtr<ID3D11RenderTargetView> &get() const { return _rtv; }
  void release() { _rtv.Reset(); }
  void clear(const ComPtr<ID3D11DeviceContext> &context, const float clear[4]);
  bool create(const ComPtr<ID3D11Device> &device,
              const ComPtr<ID3D11Texture2D> &texture, bool create_dsv);
  void setup(const ComPtr<ID3D11DeviceContext> &context, int w, int h);
};

} // namespace swtk
