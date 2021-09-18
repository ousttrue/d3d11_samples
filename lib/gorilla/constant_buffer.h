#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace gorilla {

class ConstantBuffer {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
  D3D11_BUFFER_DESC desc = {0};
  ComPtr<ID3D11Buffer> buffer;
  bool create(const ComPtr<ID3D11Device> &device, UINT size);
  void update(const ComPtr<ID3D11DeviceContext> &context, const void *p,
              size_t size);

  template <typename T>
  void update(const ComPtr<ID3D11DeviceContext> &context, const T &t) {
    update(context, &t, sizeof(T));
  }
};

} // namespace gorilla
