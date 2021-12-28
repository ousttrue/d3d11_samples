#include "texture_and_target.h"
#include "texture.h"
#include "render_target.h"
#include <assert.h>

namespace gorilla
{

  template <typename T>
  using ComPtr = Microsoft::WRL::ComPtr<T>;

  ComPtr<ID3D11ShaderResourceView> TextureAndTarget::set_rtv(
      const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context,
      int w, int h, const float *clear_color)
  {
    if (_texture)
    {
      if (w != _texture->desc.Width || h != _texture->desc.Height)
      {
        _texture.reset();
        _render_target.reset();
      }
    }
    if (!_texture)
    {
      _texture = std::make_shared<Texture>();
      if (!_texture->create(device, nullptr, w, h))
      {
        assert(false);
        return nullptr;
      }

      _render_target = std::make_shared<RenderTarget>();
      _render_target->create_rtv(device, _texture->_texture);
      _render_target->create_dsv(device);
    }

    _render_target->setup(context, w, h);
    _render_target->clear(context, clear_color);
    return _texture->_srv;
  }
}
