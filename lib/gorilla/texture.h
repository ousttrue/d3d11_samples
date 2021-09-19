#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace gorilla {

class Texture {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
  ComPtr<ID3D11Texture2D> _texture;
  ComPtr<ID3D11ShaderResourceView> _srv;
  ComPtr<ID3D11SamplerState> _sampler;

  bool create(const ComPtr<ID3D11Device> &device, const void *p, UINT w,
              UINT h);
  void set_ps(const ComPtr<ID3D11DeviceContext> &context, UINT srv,
              UINT sampler);
};

} // namespace gorilla
