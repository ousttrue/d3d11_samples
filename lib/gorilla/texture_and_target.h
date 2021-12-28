#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

namespace gorilla
{
  class Texture;
  class RenderTarget;

  class TextureAndTarget
  {
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    std::shared_ptr<Texture> _texture;
    std::shared_ptr<RenderTarget> _render_target;

  public:
    ComPtr<ID3D11ShaderResourceView> set_rtv(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context,
                                             int w, int h, const float *clear_color);
  };

}
