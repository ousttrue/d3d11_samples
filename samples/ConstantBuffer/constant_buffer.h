#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace gorilla {

class ConstantBuffer {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  ComPtr<ID3D11Buffer> _buffer;
  D3D11_BUFFER_DESC _desc = {0};

public:
  bool create(const ComPtr<ID3D11Device> &device, UINT size);
  void update(const ComPtr<ID3D11DeviceContext> &context, const void *p,
              UINT size);

  template <typename T>
  void update(const ComPtr<ID3D11DeviceContext> &context, const T &t) {
    update(context, &t, sizeof(T));
  }

  void set_gs(const ComPtr<ID3D11DeviceContext> &context, int slot);
};

} // namespace gorilla
