#pragma once
#include <d3d11.h>
#include <vector>
#include <wrl/client.h>

namespace gorilla {
class InputAssembler {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  ComPtr<ID3D11Buffer> _pVertexBuf;
  UINT _vertex_count = 0;
  UINT _strides[1] = {0};

  ComPtr<ID3D11Buffer> _pIndexBuf;
  UINT _index_count = 0;
  DXGI_FORMAT _index_format = DXGI_FORMAT_R32_UINT;

public:
  bool create_vertices(const ComPtr<ID3D11Device> &device,
                       const void *p, size_t size, size_t count);

  template <typename T>
  bool create_vertices(const ComPtr<ID3D11Device> &device,
                       const T *p, size_t count) {
    return create_vertices(device, p, sizeof(T) * count, count);
  }

  template <typename T>
  bool create_vertices(const ComPtr<ID3D11Device> &device,
                       const std::vector<T> &vertices) {
    return create_vertices(device, vertices.data(),
                           vertices.size());
  }

  bool create_indices(const ComPtr<ID3D11Device> &device, const void *p,
                      size_t size, size_t count);

  template <typename T>
  bool create_indices(const ComPtr<ID3D11Device> &device, const T *p,
                      size_t count) {
    return create_indices(device, p, sizeof(T) * count, count);
  }

  template <typename T>
  bool create_indices(const ComPtr<ID3D11Device> &device,
                      const std::vector<T> &indices) {
    return create_indices(device, indices.data(), indices.size());
  }

  void setup(const ComPtr<ID3D11DeviceContext> &context);
  void draw_submesh(const ComPtr<ID3D11DeviceContext> &context, UINT offset,
                    UINT count);
  void draw(const ComPtr<ID3D11DeviceContext> &context);
};

} // namespace gorilla
