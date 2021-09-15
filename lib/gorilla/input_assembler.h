#pragma once
#include <d3d11.h>
#include <wrl/client.h>

namespace gorilla {
class InputAssembler {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  ComPtr<ID3D11InputLayout> _input_layout;
  ComPtr<ID3D11Buffer> _pVertexBuf;
  UINT _vertex_count = 0;
  UINT _strides[1] = {0};

  ComPtr<ID3D11Buffer> _pIndexBuf;
  UINT _index_count = 0;
  DXGI_FORMAT _index_format = DXGI_FORMAT_R32_UINT;

public:
  bool create_vertices(const ComPtr<ID3D11Device> &device,
                       const ComPtr<ID3D11InputLayout> &input_layout,
                       const void *p, size_t size, size_t count);

  template <typename T>
  bool create_vertices(const ComPtr<ID3D11Device> &device,
                       const ComPtr<ID3D11InputLayout> &input_layout,
                       const T *p, size_t count) {
    return create_vertices(device, input_layout, p, sizeof(T) * count, count);
  }

  bool create_indices(const ComPtr<ID3D11Device> &device, const void *p,
                      size_t size, size_t count);

  template <typename T>
  bool create_indices(const ComPtr<ID3D11Device> &device, const T *p,
                      size_t count) {
    return create_indices(device, p, sizeof(T) * count, count);
  }

  void draw(const ComPtr<ID3D11DeviceContext> &context);
};

} // namespace gorilla
